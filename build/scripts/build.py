"""Portable offline build. Default is a plan; never connects to hardware."""
from pathlib import Path
import argparse,hashlib,json,os,shutil,subprocess,sys,zipfile
sys.dont_write_bytecode=True
ROOT=Path(__file__).resolve().parents[2]

def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 ap=argparse.ArgumentParser(description=__doc__)
 ap.add_argument('target',nargs='?',default='flash',choices=['arm','fsbl','boot','flash','project','implement','hls-cnn','hls-linear'])
 ap.add_argument('--vitis',type=Path);ap.add_argument('--vivado',type=Path);ap.add_argument('--hls',type=Path)
 ap.add_argument('--output',type=Path);ap.add_argument('--execute',action='store_true')
 a=ap.parse_args();out=(a.output or ROOT/'build/generated'/a.target).resolve()
 if not a.execute:
  print('PLAN',a.target,'output=',out,'; no tools or hardware accessed');return
 if a.target=='implement':
  if not (out/'system.xpr').is_file():ap.error('Use --output pointing to an existing project created by target project')
 else:
  if out.exists():ap.error('Choose a fresh --output; existing build evidence is preserved')
  out.mkdir(parents=True)
 def run(argv,cwd=out):
  with (out/'build.log').open('a',encoding='utf-8') as log:
   log.write(json.dumps(list(map(str,argv)))+'\n');log.flush()
   p=subprocess.run(list(map(str,argv)),cwd=cwd,stdout=log,stderr=subprocess.STDOUT)
  if p.returncode:raise RuntimeError('Build failed; see '+str(out/'build.log'))
 def vitis():
  path=a.vitis or (Path(os.environ['VITIS_ROOT']) if os.environ.get('VITIS_ROOT') else None)
  if not path:raise ValueError('Provide --vitis <Vitis 2021.1 installation> or VITIS_ROOT')
  return path.resolve()
 def arm(dest):
  dest.mkdir(exist_ok=True);bins=vitis()/'gnu/aarch32/nt/gcc-arm-none-eabi/bin'
  for cmd in json.loads((ROOT/'src/ps/commands.json').read_text()):
   run([x.replace('{ROOT}',ROOT.as_posix()).replace('{OUT}',dest.as_posix()).replace('{ARM_BIN}',bins.as_posix()) for x in cmd],ROOT)
  return dest/'heart_system.elf'
 def fsbl(dest):
  dest.mkdir(exist_ok=True);source=dest/'source';shutil.copytree(ROOT/'src/boot/source',source)
  bsp=ROOT/'src/boot/bsp';cc=vitis()/'gnu/aarch32/nt/gcc-arm-none-eabi/bin/arm-none-eabi-gcc.exe'
  flags=['-mcpu=cortex-a9','-mfpu=vfpv3','-mfloat-abi=hard']
  order=json.loads((source/'link_order.json').read_text());objects=[]
  for name in order:
   obj=Path(name).with_suffix('.o').name;objects.append(obj)
   run([cc,*flags,'-MMD','-MP','-O2','-c',name,'-o',obj,'-I'+str(bsp/'include'),'-I.'],source)
  run([cc,'-o','fsbl.elf',*objects,*flags,'-Wl,-build-id=none','-specs=Xilinx.spec','-lrsa','-Wl,--start-group,-lxil,-lgcc,-lc,--end-group','-Wl,--start-group,-lxilffs,-lxil,-lgcc,-lc,--end-group','-Wl,--start-group,-lrsa,-lxil,-lgcc,-lc,--end-group','-Wl,--gc-sections','-L'+str(bsp/'lib'),'-L.','-Tlscript.ld'],source)
  shutil.copyfile(source/'fsbl.elf',dest/'fsbl.elf');return dest/'fsbl.elf'
 result={'board_accessed':False,'target':a.target}
 if a.target=='arm':
  p=arm(out);result.update(passed=True,sha256=sha(p),matches_reference=sha(p)==sha(ROOT/'board/firmware/application.elf'))
 elif a.target=='fsbl':
  p=fsbl(out);result.update(passed=True,sha256=sha(p),matches_reference=sha(p)==sha(ROOT/'board/firmware/fsbl.elf'))
 elif a.target in ['boot','flash']:
  app=arm(out/'arm') if a.target=='flash' else ROOT/'board/firmware/application.elf'
  loader=fsbl(out/'fsbl') if a.target=='flash' else ROOT/'board/firmware/fsbl.elf'
  for src,name in [(app,'application.elf'),(loader,'fsbl.elf'),(ROOT/'board/firmware/system.bit','system.bit')]:shutil.copyfile(src,out/name)
  (out/'boot.bif').write_text('the_ROM_image:\n{\n [bootloader] fsbl.elf\n system.bit\n application.elf\n}\n',encoding='ascii')
  run([vitis()/'bin/bootgen.bat','-arch','zynq','-image','boot.bif','-o','BOOT.bin','-w','on'])
  from boot_audit import audit
  result.update(audit((out/'BOOT.bin').read_bytes(),{n:(out/n).read_bytes() for n in ['fsbl.elf','system.bit','application.elf']}))
  result['matches_reference']=sha(out/'BOOT.bin')==sha(ROOT/'board/firmware/BOOT.bin')
 elif a.target in ['project','implement']:
  if not a.vivado:raise ValueError('Provide --vivado <Vivado 2021.1 installation>')
  script=ROOT/'src/pl'/('create_project.tcl' if a.target=='project' else 'implement.tcl')
  run([a.vivado/'bin/vivado.bat','-mode','batch','-source',script,'-notrace','-nolog','-nojournal','-tclargs',out])
  result.update(passed=True,scope='New implementation requires new numerical/STA/CDC/DRC/IO review before deployment')
 else:
  if not a.hls:raise ValueError('Provide --hls <Vitis_HLS 2021.1 installation>')
  kernel='cnn' if a.target=='hls-cnn' else 'linear';top='lung_conv_fp32' if kernel=='cnn' else 'lung_linear_int8'
  source='conv.cpp' if kernel=='cnn' else 'dot.cpp'
  # Use supplied HLS streams library, not the host-only test shim.
  tcl=f'''open_project kernel
set_top {top}
set flags "-I{(ROOT/'src/pl/hls'/kernel).as_posix()} -ffp-contract=off -fno-fast-math"
add_files {(ROOT/'src/pl/hls'/kernel/source).as_posix()} -cflags $flags
add_files -tb {(ROOT/'sim'/kernel/'tb.cpp').as_posix()} -cflags $flags
open_solution solution -flow_target vivado
set_part xc7z020clg400-2
create_clock -period 10
set_clock_uncertainty 1.0
config_compile -unsafe_math_optimizations=false -no_signed_zeros=false
csim_design
csynth_design
cosim_design -rtl verilog -tool xsim -trace_level none
exit
'''
  (out/'run.tcl').write_text(tcl,encoding='ascii')
  env_dir=str(ROOT/'data/cnn_replay');prior=os.environ.get('LUNG_REPLAY_DIR');os.environ['LUNG_REPLAY_DIR']=env_dir
  try:run([a.hls/'bin/vitis_hls.bat','-f',out/'run.tcl'])
  finally:
   if prior is None:os.environ.pop('LUNG_REPLAY_DIR',None)
   else:os.environ['LUNG_REPLAY_DIR']=prior
  rpt=out/'kernel/solution/sim/report'/(top+'_cosim.rpt')
  if 'Verilog|      Pass' not in rpt.read_text():raise RuntimeError('C/RTL co-simulation did not pass')
  result.update(passed=True,scope='HLS csim/csynth/cosim only; does not replace frozen system IP automatically')
 (out/'receipt.json').write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
 print(json.dumps(result,indent=2))
if __name__=='__main__':main()
