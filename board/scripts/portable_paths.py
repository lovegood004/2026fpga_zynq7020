from pathlib import Path
import json, os, shutil
ROOT=Path(__file__).resolve().parents[2]
def path(key, tail=''):
    if key=='root':return str(ROOT/tail)
    config=ROOT/'tools.local.json'
    cfg=json.loads(config.read_text(encoding='utf-8')) if config.exists() else {}
    base=cfg.get(key) or os.environ.get(key.upper())
    if not base:
        name={'host_bin':'gcc','arm_bin':'arm-none-eabi-gcc','vivado_bin':'vivado','hls_bin':'vitis_hls','vitis_bin':'xsdb'}[key]
        found=shutil.which(name) or shutil.which(name+'.bat')
        if found:base=str(Path(found).parent)
    if not base:raise RuntimeError('Configure '+key+' in tools.local.json or '+key.upper())
    return str(Path(base)/tail)
