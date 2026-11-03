"""Offline Zynq verification helpers. Python 3.10+, standard library only."""
import argparse
import hashlib
import json
import math
from pathlib import Path
import re
import shutil
import subprocess
import sys
from datetime import datetime, timezone


def require(ok, message):
    if not ok:
        raise ValueError(message)


def digest(path):
    h = hashlib.sha256()
    with Path(path).open('rb') as f:
        for block in iter(lambda: f.read(1024 * 1024), b''):
            h.update(block)
    return h.hexdigest()


def read_json(path):
    return json.loads(Path(path).read_text(encoding='utf-8-sig'))


def inside(root, name):
    require(isinstance(name, str) and name and not Path(name).is_absolute(), 'Expected relative path')
    p = (Path(root) / name).resolve()
    require(p.is_relative_to(Path(root).resolve()) and p != Path(root).resolve(), 'Path escapes root: ' + name)
    return p


def transactions(path):
    s = Path(path).read_text(encoding='utf-8-sig')
    require(s.strip().startswith('[[[runtime]]]') and s.strip().endswith('[[[/runtime]]]'), 'Missing runtime envelope')
    body = s.strip()[len('[[[runtime]]]'):-len('[[[/runtime]]]')]
    pattern = r'\[\[transaction\]\]\s+(\d+)\s+(.*?)\[\[/transaction\]\]'
    matches = list(re.finditer(pattern, body, re.S))
    require(matches and not re.sub(pattern, '', body, flags=re.S).strip(), 'Malformed or empty transaction file')
    result = []
    for m in matches:
        words = m[2].split()
        require(words and all(re.fullmatch(r'(?:0[xX])?[0-9a-fA-F]+', w) for w in words), 'Empty transaction or X/Z/invalid word')
        result.append((int(m[1]), [int(w, 16) for w in words]))
    require(len({i for i, _ in result}) == len(result), 'Duplicate transaction IDs')
    return result


def compare(a):
    require(math.isfinite(a.atol) and math.isfinite(a.rtol) and a.atol >= 0 and a.rtol >= 0, 'Invalid tolerance')
    require(a.format == 'numeric' or (a.atol == 0 and a.rtol == 0), 'Tolerance only applies to numeric format')
    if a.format == 'bytes':
        equal = Path(a.golden).read_bytes() == Path(a.actual).read_bytes()
        require(Path(a.golden).stat().st_size > 0, 'Empty golden file')
        return dict(passed=equal, mode='byte-exact', golden_sha256=digest(a.golden), actual_sha256=digest(a.actual))
    if a.format == 'hls':
        g, v = transactions(a.golden), transactions(a.actual)
        require([i for i, _ in g] == [i for i, _ in v], 'Transaction ID/order/count mismatch')
        mismatches = 0
        examples = []
        for (tid, x), (_, y) in zip(g, v):
            require(len(x) == len(y), 'Word count mismatch at transaction ' + str(tid))
            for i, (gx, vy) in enumerate(zip(x, y)):
                if gx != vy:
                    mismatches += 1
                    if len(examples) < 10:
                        examples.append(dict(transaction=tid, word=i, golden=hex(gx), actual=hex(vy)))
        return dict(passed=mismatches == 0, transactions=len(g), words=sum(len(w) for _, w in g), mismatches=mismatches, examples=examples, golden_sha256=digest(a.golden), actual_sha256=digest(a.actual))
    def numbers(path):
        tokens = Path(path).read_text(encoding='utf-8-sig').replace(',', ' ').split()
        require(tokens, 'Empty numeric data')
        values = [float(t) for t in tokens]
        require(all(math.isfinite(x) for x in values), 'NaN/Inf forbidden')
        return values
    g, v = numbers(a.golden), numbers(a.actual)
    require(len(g) == len(v), 'Numeric length mismatch')
    bad = [i for i, (x, y) in enumerate(zip(g, v)) if abs(y-x) > a.atol + a.rtol * abs(x)]
    return dict(passed=not bad, values=len(g), mismatches=len(bad), first_indices=bad[:10], max_abs_error=max(abs(x-y) for x,y in zip(g,v)), atol=a.atol, rtol=a.rtol, golden_sha256=digest(a.golden), actual_sha256=digest(a.actual))


CHECKS = ('no_clock', 'constant_clock', 'pulse_width_clock', 'unconstrained_internal_endpoints', 'no_input_delay', 'no_output_delay', 'multiple_clock', 'generated_clocks', 'loops', 'partial_input_delay', 'partial_output_delay', 'latch_loops')
LABELS = {'lut': 'Slice LUTs', 'ff': 'Slice Registers', 'bram36': 'Block RAM Tile', 'dsp': 'DSPs'}


def qor(a):
    cfg = read_json(a.policy)
    t = Path(a.timing).read_text(encoding='utf-8-sig')
    u = Path(a.utilization).read_text(encoding='utf-8-sig')
    require('Design State : Routed' in u or re.search(r'Design State\s*:\s*Routed', u), 'Utilization must be routed')
    m = re.search(r'^\s*WNS\(ns\).*\n\s*[-\s]+\n\s*([^\n]+)', t, re.M)
    require(m is not None, 'Timing summary not recognized')
    vals = [float(x) for x in m[1].split()]
    require(len(vals) == 12 and all(math.isfinite(x) for x in vals), 'Expected 12 timing summary fields')
    issues = []
    for i, key in [(0, 'min_wns_ns'), (4, 'min_whs_ns'), (8, 'min_wpws_ns')]:
        threshold = float(cfg.get(key, 0))
        require(math.isfinite(threshold) and threshold >= 0, 'Invalid slack policy')
        if vals[i] < threshold:
            issues.append(key + ' failed')
    if any(vals[i] != 0 for i in (1, 2, 5, 6, 9, 10)):
        issues.append('Timing violations or failing endpoints')
    counts = {}
    exceptions = cfg.get('check_timing_exceptions', {})
    require(set(exceptions) <= set(CHECKS), 'Unknown timing exception')
    for name in CHECKS:
        found = re.findall(r'checking ' + name + r' \((\d+)\)', t)
        require(found and len(set(found)) == 1, 'Missing/inconsistent check_timing: ' + name)
        counts[name] = int(found[0])
        expected = 0
        if name in exceptions:
            rule = exceptions[name]
            require(rule.get('reason') and rule.get('scope') and type(rule.get('count')) is int and rule['count'] >= 0, 'Exception needs exact count, scope and reason')
            expected = rule['count']
        if counts[name] != expected:
            issues.append(name + ': count differs from policy')
    resources = {}
    require(set(cfg.get('max_util_pct', {})) == set(LABELS), 'Require lut/ff/bram36/dsp budgets')
    for key, label in LABELS.items():
        rows = [line.split('|')[1:-1] for line in u.splitlines() if re.match(r'^\|\s*' + re.escape(label) + r'\s*\|', line)]
        require(rows and len({tuple(x.strip() for x in row) for row in rows}) == 1, 'Missing/conflicting resource row: ' + label)
        cells = [x.strip() for x in rows[0]]
        used, available, percent = float(cells[1]), float(cells[-2]), float(cells[-1])
        limit = float(cfg['max_util_pct'][key])
        require(all(math.isfinite(x) for x in (used, available, percent, limit)) and used >= 0 and available > 0 and 0 <= percent and 0 <= limit <= 100, 'Invalid resource values')
        require(abs(100*used/available-percent) <= 0.02, 'Resource percentage inconsistent')
        resources[key] = dict(used=used, available=available, percent=percent, limit=limit)
        if 100*used/available > limit:
            issues.append(key + ': resource budget exceeded')
    return dict(passed=not issues, issues=issues, wns_ns=vals[0], whs_ns=vals[4], wpws_ns=vals[8], check_timing=counts, exceptions=exceptions, utilization=resources, evidence_sha256={str(p):digest(p) for p in (a.timing,a.utilization,a.policy)}, scope='Report-only STA/resource check; DRC/CDC/pin identity and design binding require separate review')


def manifest_files(root, names):
    require(isinstance(names, list) and names and len(set(names)) == len(names), 'Require nonempty unique file list')
    result = {}
    for n in names:
        p = inside(root, n)
        require(p.is_file(), 'Missing file: ' + n)
        result[n] = dict(bytes=p.stat().st_size, sha256=digest(p))
    return result


def freeze(a):
    cfg = read_json(a.spec)
    require(isinstance(cfg.get('toolchain'), dict) and cfg['toolchain'], 'Declare toolchain versions')
    return dict(passed=True, schema=1, files=manifest_files(a.root,cfg['files']), toolchain=cfg['toolchain'], scope='Input identity baseline; not proof of rebuildability')


def verify_manifest(root, manifest):
    require(manifest.get('schema') == 1 and manifest.get('files'), 'Invalid manifest')
    current = manifest_files(root, list(manifest['files']))
    changed = [n for n in current if current[n] != manifest['files'][n]]
    return dict(passed=not changed, files=len(current), changed=changed, scope='Declared file integrity only; undeclared dependencies are not checked')


def reproduce(a):
    cfg, baseline = read_json(a.spec), read_json(a.manifest)
    checked = verify_manifest(a.root, baseline)
    require(checked['passed'], 'Inputs changed: ' + repr(checked['changed']))
    require(cfg.get('commands') and cfg.get('outputs'), 'Require commands and outputs')
    require(isinstance(cfg.get('timeout_seconds'), int) and cfg['timeout_seconds'] > 0, 'Require positive timeout_seconds')
    for cmd in cfg['commands']:
        require(isinstance(cmd, list) and cmd and all(isinstance(x,str) for x in cmd), 'Commands must be argv lists')
    for name in cfg['outputs']:
        inside(a.root, name)
        require(name not in baseline['files'], 'Output must not be a copied input')
    if not a.execute:
        return dict(passed=None, status='PLAN', commands=cfg['commands'], outputs=cfg['outputs'], scope='No build executed; inspect commands for offline-only behavior')
    work = Path(a.work).resolve()
    require(not work.exists(), 'Work directory exists; use a fresh path')
    require(not Path(a.root).resolve().is_relative_to(work), 'Work directory cannot contain input root')
    work.mkdir(parents=True)
    results = []
    for run in ('a','b'):
        dst = work/run
        dst.mkdir()
        for name in baseline['files']:
            target = inside(dst,name)
            target.parent.mkdir(parents=True,exist_ok=True)
            shutil.copy2(inside(a.root,name),target)
        require(verify_manifest(dst,baseline)['passed'], 'Copy changed inputs')
        for i, cmd in enumerate(cfg['commands']):
            argv = [x.replace('{root}', str(dst)).replace('{python}',sys.executable) for x in cmd]
            with (work/(run+'-'+str(i)+'.log')).open('w',encoding='utf-8') as log:
                # No shell interpolation; user-supplied build commands still execute code.
                proc = subprocess.Popen(argv,cwd=dst,stdout=log,stderr=subprocess.STDOUT)
                try:
                    rc = proc.wait(timeout=cfg['timeout_seconds'])
                except subprocess.TimeoutExpired:
                    if sys.platform == 'win32':
                        subprocess.run(['taskkill','/PID',str(proc.pid),'/T','/F'],stdout=log,stderr=subprocess.STDOUT)
                    else:
                        proc.kill()
                    proc.wait()
                    raise ValueError('Build timed out; logs preserved')
            require(rc == 0, 'Build failed; see '+str(work/(run+'-'+str(i)+'.log')))
        results.append(manifest_files(dst,cfg['outputs']))
    changed = [n for n in results[0] if results[0][n] != results[1][n]]
    after = verify_manifest(a.root,baseline)
    return dict(passed=not changed and after['passed'], runs=results, changed_outputs=changed, original_inputs_unchanged=after['passed'], toolchain_declaration=baseline.get('toolchain'), spec_sha256=digest(a.spec), manifest_sha256=digest(a.manifest), scope='Two fresh declared-input builds, byte-exact outputs. Toolchain/environment are shared; no hermetic or functional/board claim')


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--report', help='New JSON receipt path; never overwrites')
    sub = p.add_subparsers(dest='command', required=True)
    c = sub.add_parser('compare')
    c.add_argument('--golden',required=True); c.add_argument('--actual',required=True)
    c.add_argument('--format',choices=['bytes','hls','numeric'],default='bytes')
    c.add_argument('--atol',type=float,default=0); c.add_argument('--rtol',type=float,default=0)
    q = sub.add_parser('qor')
    for n in ('timing','utilization','policy'): q.add_argument('--'+n,required=True)
    f = sub.add_parser('freeze'); f.add_argument('--root',required=True); f.add_argument('--spec',required=True)
    v = sub.add_parser('verify'); v.add_argument('--root',required=True); v.add_argument('--manifest',required=True)
    r = sub.add_parser('reproduce')
    for n in ('root','manifest','spec','work'): r.add_argument('--'+n,required=True)
    r.add_argument('--execute',action='store_true')
    a = p.parse_args()
    if a.report and Path(a.report).exists():
        p.error('Report exists; preserve evidence and choose a new path')
    try:
        result = {'compare':compare,'qor':qor,'freeze':freeze,'verify':lambda x:verify_manifest(x.root,read_json(x.manifest)),'reproduce':reproduce}[a.command](a)
    except (ValueError, OSError, KeyError, TypeError, OverflowError) as e:
        result = dict(passed=False,error=str(e))
    result.update(command=a.command, utc=datetime.now(timezone.utc).isoformat(), board_accessed=False)
    output = json.dumps(result,ensure_ascii=False,indent=2,allow_nan=False)+'\n'
    if a.report:
        target = Path(a.report); target.parent.mkdir(parents=True,exist_ok=True)
        with target.open('x',encoding='utf-8') as f: f.write(output)
    print(output)
    return 0 if result.get('passed') is True else (2 if result.get('status') == 'PLAN' else 1)


if __name__ == '__main__':
    sys.exit(main())
