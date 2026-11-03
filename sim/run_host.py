"""Host exact-word CNN regression; not an RTL or board test."""
from pathlib import Path
import argparse,json,os,subprocess,hashlib
ROOT=Path(__file__).resolve().parents[1]
a=argparse.ArgumentParser(description=__doc__);a.add_argument('--host-bin',type=Path,required=True);a.add_argument('--output',type=Path);args=a.parse_args()
out=(args.output or ROOT/'sim/generated/host-exact').resolve()
if out.exists():a.error('Use a fresh output directory')
out.mkdir(parents=True)
env=os.environ.copy();env['PATH']=str(args.host_bin)+os.pathsep+env.get('PATH','');env['LUNG_REPLAY_DIR']=str(ROOT/'data/cnn_replay')
compiler=args.host_bin/('g++.exe' if os.name=='nt' else 'g++')
flags=['-O2','-std=c++17','-ffp-contract=off','-fno-fast-math','-I'+str(ROOT/'src/pl/hls/cnn'),'-I'+str(ROOT/'sim/cnn/host')]
commands=[]
for source,name in [(ROOT/'sim/cnn/reference.cpp','reference'),(ROOT/'src/pl/hls/cnn/conv.cpp','candidate')]:
 commands.append([compiler,*flags,'-c',source,'-o',out/(name+'.o')])
exe=out/('exact.exe' if os.name=='nt' else 'exact')
commands.append([compiler,*flags,ROOT/'sim/cnn/test_exact.cpp',out/'reference.o',out/'candidate.o','-o',exe]);commands.append([exe])
output=''
with (out/'test.log').open('w',encoding='utf-8') as log:
 for cmd in commands:
  log.write(json.dumps(list(map(str,cmd)))+'\n');log.flush()
  p=subprocess.run(list(map(str,cmd)),env=env,cwd=ROOT,capture_output=True,text=True)
  output=p.stdout+p.stderr;log.write(output);log.flush()
  if p.returncode:raise SystemExit('Host test failed; see '+str(out/'test.log'))
if 'PASS' not in output:raise SystemExit('No PASS in this test output')
files=[ROOT/'src/pl/hls/cnn/conv.cpp',ROOT/'src/pl/hls/cnn/conv.hpp',ROOT/'sim/cnn/test_exact.cpp',ROOT/'sim/cnn/reference.cpp',Path(__file__)]+list((ROOT/'data/cnn_replay').glob('*.bin'))
(out/'receipt.json').write_text(json.dumps({'passed':True,'board_accessed':False,'scope':'Host exact numeric regression only, not HLS/RTL or physical throughput','source_sha256':{p.relative_to(ROOT).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in files},'output':output},indent=2)+'\n')
print(output)
