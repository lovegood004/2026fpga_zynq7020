"""Pinned acquisition-preview candidate, plan by default. One explicit fresh-board RAM load."""
from pathlib import Path
import argparse,hashlib,json,struct,subprocess,shutil
from datetime import datetime,timezone
from path_alias import compiler_root
from portable_paths import path
ROOT=Path(__file__).resolve().parents[1]
def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--execute',action='store_true');ap.add_argument('--jtag-serial')
    ap.add_argument('--fresh-board-records-discarded',action='store_true')
    ap.add_argument('--candidate',type=Path,default=ROOT/'outputs/acq_preview_20261028/candidate.json')
    a=ap.parse_args();candidate_path=a.candidate.resolve();candidate_path.relative_to(ROOT)
    c=json.loads(candidate_path.read_text(encoding='utf-8'))
    assert c['candidate']=='ACQ_PREVIEW_20261028' and c['status']=='offline_checked_not_board_accepted'
    for section in ('sources','artifacts'):
        for item in c[section]:
            p=ROOT/item['path'];assert p.stat().st_size==item['bytes'] and hashlib.sha256(p.read_bytes()).hexdigest()==item['sha256'],item['path']+' changed'
    application=Path(c['application']);(ROOT/application).resolve().relative_to(ROOT)
    assert not application.is_absolute() and not any(x in c['application'] for x in '{}\\\r\n')
    assert any(i['path']==c['application'] for i in c['artifacts'])
    print('Acquisition-preview candidate identities verified. RAM load only; no reset/Flash/automatic capture.')
    if not a.execute:
        print('PLAN ONLY: no hardware connection. Existing recording remains untouched.');return
    if not a.jtag_serial or not a.fresh_board_records_discarded:ap.error('Explicit adapter and discarded-RAM acknowledgement required')
    out=ROOT/'deployment_logs'/datetime.now().strftime('acq_preview_%Y%m%d_%H%M%S');out.mkdir(parents=True,exist_ok=False)
    (out/'candidate_at_load.json').write_text(json.dumps(c,ensure_ascii=False,indent=2),encoding='utf-8')
    shutil.copyfile(ROOT/c['application'],out/'application.elf')
    elf=(ROOT/application).read_bytes();address=int(c['symbols']['main']['address'],16)
    assert (out/'application.elf').read_bytes()==elf, 'Archived ELF mismatch'
    h=struct.unpack_from('<HHIIIIIHHHHHH',elf,16);words=None
    for i in range(h[9]):
        kind,offset,virt,phys,size,mem,flags,align=struct.unpack_from('<8I',elf,h[4]+i*h[8])
        if kind==1 and virt<=address and address+64<=virt+size:words=struct.unpack_from('<16I',elf,offset+address-virt)
    assert words is not None
    receipt=dict(passed=False,board_load_completed=False,jtag_serial=a.jtag_serial,
        reset_issued=False,fpga_flash_written=False,automatic_start_sent=False,
        elf_sha256=hashlib.sha256(elf).hexdigest(),verified_files=len(c['sources'])+len(c['artifacts']))
    (out/'load_intent.json').write_text(json.dumps(dict(receipt,expected_main_words=words,symbols=c['symbols']),indent=2))
    try:
        with compiler_root(ROOT) as root:
            mapped_out=root/out.relative_to(ROOT)
            # Brace quoting requires simple trusted paths/serial; user input cannot add Tcl commands.
            if any(x in a.jtag_serial for x in '{}\\\r\n'):raise ValueError('Invalid adapter serial')
            command='acq_candidate_load {'+root.as_posix()+'} {'+mapped_out.as_posix()+'} {'+a.jtag_serial+'} {'+' '.join(map(str,words))+'} '+c['symbols']['main']['address']+' '+c['symbols']['raw_report']['address']+' '+c['symbols']['lcd_pl_report']['address']+' '+c['symbols']['acq_preview_report']['address']+' {'+application.as_posix()+'}'
            script='source {'+(root/'tools/fpga_loader.tcl').as_posix()+'}\nsource {'+(root/'platform/acq_preview/load.tcl').as_posix()+'}\nif {[catch {'+command+'} problem options]} {puts stderr $problem;puts stderr [dict get $options -errorinfo];exit 1}\nexit 0\n'
            (out/'entry.tcl').write_text(script,encoding='ascii')
            cmd=[str(Path(path('vitis_bin'))/'xsdb.bat'),str(mapped_out/'entry.tcl')]
            receipt['command']=cmd
            with (out/'load.log').open('w',encoding='utf-8') as log:
                proc=subprocess.Popen(cmd,stdout=log,stderr=subprocess.STDOUT,creationflags=subprocess.CREATE_NO_WINDOW)
                try:proc.wait(timeout=180)
                except subprocess.TimeoutExpired:
                    subprocess.run(['taskkill','/PID',str(proc.pid),'/T','/F'],stdout=log,stderr=subprocess.STDOUT)
                    proc.wait(timeout=20);raise
            output=(out/'load.log').read_text(encoding='utf-8',errors='replace')
            receipt['exit_code']=proc.returncode
            receipt['passed']=proc.returncode==0 and 'ACQ_LOAD_COMPLETE RAM_ONLY_NO_FLASH_NO_RESET_NO_START' in output
            receipt['board_load_completed']=receipt['passed']
            print(output[-3500:])
    except Exception as e:
        receipt['error']=str(e)
    finally:
        receipt['finished_utc']=datetime.now(timezone.utc).isoformat()
        (out/'load_receipt.json').write_text(json.dumps(receipt,indent=2))
    print('Receipt: '+str(out/'load_receipt.json'))
    if not receipt['passed']:raise SystemExit(1)
if __name__=='__main__':main()
