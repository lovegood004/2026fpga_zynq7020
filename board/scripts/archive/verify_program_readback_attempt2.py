"""Compare independent full chip readback, including every untouched byte."""
from pathlib import Path
import json,hashlib,datetime,runpy,struct
W=Path(__file__).resolve().parent;R=W.parents[1];D=R.parent/'2'
def sha(b):return hashlib.sha256(b).hexdigest()
g=json.loads((W/'BACKUP_AND_IMAGE_GATE_V2.json').read_text())
native=json.loads((W/'NATIVE_PROGRAM_RECEIPT_ATTEMPT2.json').read_text())
assert g['passed'] and native['native_success_claim']
original=(W/'original_a_v2.bin').read_bytes()
assert sha(original)==g['backup_a']['sha256']
readback=(W/'programmed_full_readback_attempt2.bin').read_bytes()
image=(W/'program_image_v2.bin').read_bytes();boot=(D/'boot/BOOT.bin').read_bytes()
assert len(original)==len(readback)==g['capacity_bytes']==33554432
assert sha(image)==g['program_image']['sha256'] and sha(boot)==g['boot']['sha256']
end=g['erase_end_exclusive'];assert len(image)==end
assert readback[:len(boot)]==boot,'BOOT payload not byte-identical'
assert readback[:end]==image,'Written aligned range differs'
assert readback[len(boot):end]==original[len(boot):end],'Original final-block padding changed'
assert readback[end:]==original[end:],'Outside intended erase range changed'
status=struct.unpack('<16I',(W/'post_program_final_status_attempt2.bin').read_bytes()[:64])
assert status[:3]==(0x524f4651,2,3) and status[4]==0 and status[5]==33554432 and status[9]==1
meta=(W/'metadata_post_attempt2.bin').read_bytes()
assert meta[65:68]==meta[73:76]==bytes.fromhex('ef4019')
runpy.run_path(str(R/'tools/cnn_replay_unbounded_supervisor_20261102.py'),run_name='post_flash_freeze')['frozen']()
rec=dict(time=datetime.datetime.now().astimezone().isoformat(),passed=True,
 capacity_bytes=len(readback),full_readback_sha256=sha(readback),
 BOOT_payload_bytes=len(boot),BOOT_payload_sha256=sha(boot),
 native_program_and_verify_passed=True,independent_full_chip_readback_passed=True,
 exact_BOOT_bytes=True,exact_aligned_program_image=True,
 original_partial_block_tail_preserved=True,all_bytes_outside_erase_preserved=True,
 flash_written=True,sd_written=False,esp_written=False,
 cold_boot_verified=False,power_current_qualified=False,
 protected_folder1_and_current_release_unchanged=True)
(W/'FLASH_READBACK_QUALIFICATION.json').write_text(json.dumps(rec,indent=2),encoding='utf-8')
print(json.dumps(rec,indent=2))
