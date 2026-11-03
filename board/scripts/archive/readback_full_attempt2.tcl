connect -url tcp:127.0.0.1:3121
after 1000
set failed [catch {
 set rows [targets -target-properties -filter {name == "xc7z020"}]
 if {[llength $rows]!=1 || [dict get [lindex $rows 0] jtag_cable_serial] ne "210512180081"} {error "Wrong FPGA/adapter"}
 targets -set [dict get [lindex $rows 0] target_id]
 if {[fpga -state] ne "FPGA is not configured"} {error "Reader session changed"}
 set rows [targets -target-properties -filter {name =~ "*Cortex-A9*#0"}]
 if {[llength $rows]!=1 || [dict get [lindex $rows 0] jtag_cable_serial] ne "210512180081"} {error "Wrong reader CPU"}
 targets -set [dict get [lindex $rows 0] target_id]
 set r [mrd -value 0x07000000 16]
 if {[lindex $r 0]!=0x524f4651 || [lindex $r 1]!=2 || [lindex $r 2]!=1 || [lindex $r 4]!=0} {error "Reader not freshly ready"}
 set id [mrd -value 0x07000040 4]
 if {$id ne "423685888 0 423685888 0"} {error "Unexpected JEDEC ID"}
 foreach item {{programmed_full_readback_attempt2.bin 0x08000000 0x02000000 2}} {
  lassign $item filename dst count cmd
  set path [file join {E:/FPGA_competition/AX7020_Project/build_work/flash_deployment_20261103} $filename]
  if {[file exists $path]} {error "Refuse overwrite backup"}
  set prevcounter [lindex [mrd -value 0x07000000 16] 9]
  mwr 0x0700001c $dst
  mwr 0x07000018 $count
  mwr 0x0700000c $cmd
  set last -1;set stuck 0
  while {1} {
   after 1000
   set r [mrd -value 0x07000000 16]
   set stage [lindex $r 2]; set progress [lindex $r 5]
   puts [list FLASH_READ_PROGRESS $filename $stage $progress]
   flush stdout
   if {[lindex $r 4]!=0 || $stage==0xdead} {error "QSPI read failed"}
   if {$stage==3 && [lindex $r 9]==$prevcounter+1} {
    if {$progress!=$count} {error "Incomplete flash read"}
    break
   }
   if {$progress==$last} {incr stuck} else {set stuck 0}
   if {$stuck>30} {stop;error "Read stalled, no flash writes issued"}
   set last $progress
  }
  mrd -bin -file $path $dst [expr {$count/4}]
  if {[file size $path]!=$count} {error "Wrong backup length"}
  puts [list BACKUP_FILE_COMPLETE $filename [file size $path]]
  flush stdout
 }
 mrd -bin -file {E:/FPGA_competition/AX7020_Project/build_work/flash_deployment_20261103/post_program_final_status_attempt2.bin} 0x07000000 256
} problem]
catch {disconnect}
if {$failed} {puts stderr $problem;exit 1}
puts POST_PROGRAM_FULL_READBACK_COMPLETE
exit 0
