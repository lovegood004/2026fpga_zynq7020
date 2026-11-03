from pathlib import Path
import json,hashlib,zipfile,sys
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[1]
manifest=json.loads((ROOT/'build/FILE_MANIFEST.json').read_text(encoding='utf-8'))
errors=[]
for name,record in manifest['files'].items():
 p=(ROOT/name).resolve()
 if not p.is_relative_to(ROOT) or not p.is_file():errors.append(name+': missing/escaped');continue
 if p.stat().st_size!=record['bytes'] or hashlib.sha256(p.read_bytes()).hexdigest()!=record['sha256']:errors.append(name+': changed')
f=ROOT/'board/firmware'
with zipfile.ZipFile(f/'hardware.xsa') as z:
 bits=[n for n in z.namelist() if n.endswith('.bit')]
 if len(bits)!=1 or z.read(bits[0])!=(f/'system.bit').read_bytes():errors.append('XSA/bit mismatch')
sys.path.insert(0,str(ROOT/'build/scripts'))
from boot_audit import audit
review=audit((f/'BOOT.bin').read_bytes(),{n:(f/n).read_bytes() for n in ['fsbl.elf','system.bit','application.elf']})
print(json.dumps({'passed':not errors,'checked_files':len(manifest['files']),'errors':errors,'boot_payload_audit':review['passed'],'scope':'File integrity/XSA-bit/BOOT payload only; not a new board test'},indent=2))
sys.exit(bool(errors))
