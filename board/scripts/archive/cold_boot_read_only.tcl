# Attach only. No download, processor stop/reset/continue, or memory writes.
connect -url tcp:127.0.0.1:3121
after 1000
set failed [catch {
 puts [list COLD_BOOT_TARGETS [targets -target-properties]]
 set rows [targets -target-properties -filter {name == "xc7z020"}]
 if {[llength $rows]!=1 || [dict get [lindex $rows 0] jtag_cable_serial] ne "210512180081"} {error "Wrong adapter"}
 targets -set [dict get [lindex $rows 0] target_id]
 if {[fpga -state] ne "FPGA is configured"} {error "FPGA did not configure from cold boot"}
 set rows [targets -target-properties -filter {name =~ "*Cortex-A9*#0"}]
 if {[llength $rows]!=1 || [dict get [lindex $rows 0] suspended]!=0} {error "Core not running"}
 targets -set [dict get [lindex $rows 0] target_id]
 set bootmode [lindex [mrd -value 0xf800025c] 0]
 if {($bootmode & 15)!=1} {error "Not QSPI boot strap"}
 puts [list ACTUAL_BOOT_MODE $bootmode]
 set expected {3912060912 3809428976 3979184898 3796750444 3808514752 3959399616 3959399645 3809225928 3809227140 3812630636 3785363460 3812626550 3785359362 3812634641 3812643043 3850448896}
 set got [mrd -value 0x1394a0 16]
 if {[llength $got]!=16} {error "Invalid main readback"}
 for {set i 0} {$i<16} {incr i} {if {[lindex $got $i]!=[lindex $expected $i]} {error "Wrong normal main fingerprint"}}
 puts COLD_BOOT_ORIGINAL25_NORMAL_MAIN_VERIFIED
 set r [mrd -value 0x00680f80 20]
 if {[llength $r]!=20 || [lindex $r 0]!=0x32525348 || [lindex $r 1]!=2} {error "Invalid runtime"}
 foreach i {3 4 7 8 10 13 14} {if {[lindex $r $i]!=0} {error "Non-idle or error runtime"}}
 puts [list COLD_BOOT_RUNTIME $r]
 puts [list COLD_BOOT_LCD [mrd -value 0x00953f14 7]]
 puts [list COLD_BOOT_ACQ [mrd -value 0x00953f30 13]]
 loadhw -hw {E:/FPGA_competition/2/firmware/hardware.xsa}
 set cnn [lindex [mrd -value 0x43c60000] 0]
 set linear [lindex [mrd -value 0x43c70000] 0]
 if {$cnn!=4 || $linear!=4} {error "Accelerator not idle"}
 puts [list COLD_BOOT_ACCELERATORS $cnn $linear]
} problem]
catch {disconnect}
if {$failed} {puts stderr $problem;exit 1}
puts QSPI_COLD_BOOT_NORMAL_STARTUP_READ_ONLY_PASS
exit 0
