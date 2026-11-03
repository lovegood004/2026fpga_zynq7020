"""PW HLS reproduction in a fresh directory. Plan by default; offline only."""
from pathlib import Path
import argparse,hashlib,json,subprocess,time,sys
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[2]
PW=ROOT/'src/pl/hls/pw'

def sha(path):
 return hashlib.sha256(path.read_bytes()).hexdigest()

def main():
 ap=argparse.ArgumentParser(description=__doc__)
 ap.add_argument('--hls',type=Path,help='Vitis_HLS 2021.1 installation root')
 ap.add_argument('--stage',choices=['csim','synth','cosim'],default='csim',help='Cumulative: csim; csim+synthesis; or csim+synthesis+RTL co-simulation')
 ap.add_argument('--output',type=Path)
 ap.add_argument('--execute',action='store_true')
 a=ap.parse_args();out=(a.output or ROOT/'build/generated'/('pw-'+a.stage)).resolve()
 if not a.execute:
  print('PLAN ONLY:',a.stage,'output=',out,'; no EDA or board access');return
 if not a.hls:ap.error('Provide --hls <Vitis_HLS 2021.1 installation>')
 if out.exists() or out.is_relative_to(PW.resolve()):ap.error('Use a fresh output outside the copied PW source tree')
 tool=a.hls.resolve()/'bin/vitis_hls.bat'
 if not tool.is_file():ap.error('Vitis_HLS bin/vitis_hls.bat not found')
 # Braced Tcl literals and quoted C include flags must not admit Tcl syntax.
 for path in [PW,out]:
  if any(c in str(path) for c in '{}"\r\n'):ap.error('Use source/output paths without braces, quotes or line breaks')
 files=sorted(p for folder in ['src','tests','data','vendor','hls'] for p in (PW/folder).rglob('*') if p.is_file())
 if not files:ap.error('PW source/dependency files missing')
 before={p.relative_to(ROOT).as_posix():sha(p) for p in files}
 lib=PW/'vendor/blas/L1/include/hw'
 flags='-std=c++11 '+ ' '.join('-I"'+p.as_posix()+'"' for p in [PW/'src',lib,lib/'xf_blas'])+' -ffp-contract=off'
 tcl=f'''open_project shared
set_top pw_shared
set flags {{{flags}}}
add_files {{{(PW/'src/pw_shared.cpp').as_posix()}}} -cflags $flags
add_files -tb {{{(PW/'tests/tb_shared.cpp').as_posix()}}} -cflags $flags
open_solution solution -flow_target vivado
set_part xc7z020clg400-2
create_clock -period 10
config_interface -m_axi_addr64=false
csim_design -argv {{{(PW/'data').as_posix()}}}
'''
 if a.stage in ['synth','cosim']:tcl+='csynth_design\n'
 if a.stage=='cosim':tcl+='cosim_design -rtl verilog -tool xsim -trace_level none -argv {'+(PW/'data').as_posix()+' --rtl}\n'
 tcl+='puts PW_REQUESTED_STAGE_COMPLETE_NO_BOARD_ACCESS\nexit\n'
 out.mkdir(parents=True);(out/'run.tcl').write_text(tcl,encoding='utf-8')
 result={'passed':False,'board_accessed':False,'stage':a.stage,'source_sha256':before,'scope':'PW HLS source verification only. No IP export/replacement, whole-system rebuild or board qualification. C covers 30 full layers; RTL uses full layer0 and at most65 positions from each other layer plus synthetic/invalid cases.'}
 (out/'intent.json').write_text(json.dumps(result,indent=2)+'\n')
 start=time.monotonic()
 try:
  with (out/'build.log').open('w',encoding='utf-8') as log:
   p=subprocess.run([str(tool),'-f',str(out/'run.tcl')],cwd=out,stdout=log,stderr=subprocess.STDOUT)
  result['exit_code']=p.returncode
  text=(out/'build.log').read_text(encoding='utf-8',errors='replace')
  if p.returncode!=0 or 'PW_REQUESTED_STAGE_COMPLETE_NO_BOARD_ACCESS' not in text:raise RuntimeError('HLS stage failed; inspect retained build.log')
  csim=out/'shared/solution/csim/report/pw_shared_csim.log'
  records=[]
  for line in csim.read_text(encoding='utf-8',errors='replace').splitlines():
   try:item=json.loads(line)
   except ValueError:continue
   if isinstance(item,dict) and item.get('test')=='pw_shared':records.append(item)
  if len(records)!=1 or records[0].get('failures')!=0 or records[0].get('rtl') is not False or records[0].get('calls')!=43:raise RuntimeError('Expected fresh 43-call C simulation result not found')
  result['csim']=records[0]
  if a.stage in ['synth','cosim']:
   if not (out/'shared/solution/syn/report/pw_shared_csynth.rpt').is_file():raise RuntimeError('Synthesis report missing')
  if a.stage=='cosim':
   rpt=out/'shared/solution/sim/report/pw_shared_cosim.rpt'
   if 'Verilog|      Pass' not in rpt.read_text():raise RuntimeError('C/RTL co-simulation did not pass')
  if before!={p.relative_to(ROOT).as_posix():sha(p) for p in files}:raise RuntimeError('Source/dependency inputs changed during run')
  result['passed']=True
 except Exception as e:result['error']=str(e)
 result['seconds']=time.monotonic()-start
 result['report_sha256']={p.relative_to(out).as_posix():sha(p) for p in out.rglob('*') if p.is_file() and (p.suffix=='.rpt' or p.name in ['run.tcl','build.log','pw_shared_csim.log'])}
 (out/'receipt.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
 print(json.dumps({k:v for k,v in result.items() if k not in ['source_sha256','report_sha256']},indent=2))
 if not result['passed']:raise SystemExit(1)
if __name__=='__main__':main()
