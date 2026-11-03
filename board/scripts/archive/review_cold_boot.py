"""Review closed, read-only QSPI cold-boot evidence against delivered schemas."""
from pathlib import Path
import json,hashlib,re,datetime
W=Path(__file__).resolve().parent;D=W.parents[1].parent/'2'
script=(W/'cold_boot_read_only.tcl').read_text(encoding='utf-8')
log=(W/'cold_boot_read_only.log').read_text(encoding='utf-8')
for marker in ['ACTUAL_BOOT_MODE 1','COLD_BOOT_ORIGINAL25_NORMAL_MAIN_VERIFIED',
 'QSPI_COLD_BOOT_NORMAL_STARTUP_READ_ONLY_PASS','COLD_BOOT_ACCELERATORS 4 4']:assert marker in log
commands=[s.strip() for s in script.splitlines() if s.strip() and not s.lstrip().startswith('#')]
assert not any(re.match(r'^(dow|mwr|rwr|stop|con|rst|fpga\s+-file)\b',line) for line in commands)
def words(label):
 m=re.search(r'(?m)^'+label+r' \{([^}]+)\}',log);assert m
 return list(map(int,m[1].split()))
r=words('COLD_BOOT_RUNTIME');lcd=words('COLD_BOOT_LCD');acq=words('COLD_BOOT_ACQ')
assert len(r)==20 and r[:2]==[0x32525348,2] and all(r[i]==0 for i in [3,4,7,8,10,13,14])
assert len(lcd)==7 and lcd[:2]==[0x4c434431,1] and lcd[2]==0 and lcd[5]==0
source=D/'software/src/modules/acq_preview/acq_preview.c'
match=re.search(r'acq_preview_report=\{\.magic=(0x[0-9a-fA-F]+),\.version=(\d+)\}',source.read_text(encoding='utf-8'))
assert match and int(match[1],16)==0x41505231 and int(match[2])==1
assert len(acq)==13 and acq[:2]==[int(match[1],16),int(match[2])] and acq[2:]==[0]*11
q=json.loads((W/'FLASH_READBACK_QUALIFICATION.json').read_text(encoding='utf-8'));assert q['passed']
rec=dict(time=datetime.datetime.now().astimezone().isoformat(),passed=True,actual_boot_mode=1,
 user_confirmed_QSPI_cold_power_cycle=True,read_only_attach_only=True,
 no_RAM_reload_or_reset_after_user_power_cycle=True,FPGA_configured=True,core0_running=True,
 normal_main_verified=True,runtime_idle_no_error=True,LCD_no_error=True,acquisition_idle=True,
 runtime_words=r,LCD_words=lcd,acquisition_words=acq,CNN_control=4,Linear_control=4,
 ARM_sha256='a9d959c50fcd6fd8fd348aa69504c88dc57c671191a1655a213ee48cc8619372',
 bit_sha256='e5597b4deec14daad2ba330c7945ca2ea1d1be20ba36ecf55e2a90545ce42417',
 BOOT_sha256=q['BOOT_payload_sha256'],
 script_sha256=hashlib.sha256((W/'cold_boot_read_only.tcl').read_bytes()).hexdigest(),
 log_sha256=hashlib.sha256((W/'cold_boot_read_only.log').read_bytes()).hexdigest(),
 acquisition_report_source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),
 acceptance_scope='One physical QSPI cold boot to normal idle menu; no automatic real recording or new inference test',
 power_current_qualified=False,sd_written=False,esp_written=False)
(W/'COLD_BOOT_QUALIFICATION.json').write_text(json.dumps(rec,indent=2),encoding='utf-8')
print('QSPI_COLD_BOOT_QUALIFIED',rec['time'])
