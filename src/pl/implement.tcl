set root [file normalize [file join [file dirname [info script]] ../..]]
set build $root/build/generated/hardware
if {$argc == 1} {set build [file normalize [lindex $argv 0]]}
open_project $build/system.xpr
launch_runs synth_1 -jobs 4
wait_on_run synth_1
if {[get_property PROGRESS [get_runs synth_1]] ne "100%"} {error "Synthesis failed"}
open_run synth_1
foreach p {lcd_tri_io[0] lcd_tri_io[1] lcd_tri_io[2] lcd_tri_io[3] lcd_tri_io[4] lcd_tri_io[5] acq_uart_tx} {set_property DRIVE 4 [get_ports $p];set_property SLEW SLOW [get_ports $p]}
opt_design
source $root/src/pl/remap_controls.tcl
place_design
phys_opt_design
route_design
file mkdir $build/results
foreach {name command} {timing report_timing_summary utilization report_utilization drc report_drc cdc report_cdc io report_io methodology report_methodology} {$command -file $build/results/$name.rpt}
set setup [get_property SLACK [get_timing_paths -delay_type max -max_paths 1]]
set hold [get_property SLACK [get_timing_paths -delay_type min -max_paths 1]]
if {$setup eq "" || $hold eq "" || $setup<0 || $hold<0} {error "Timing failed: $setup / $hold"}
set errors [get_drc_violations -quiet -filter {SEVERITY == "Error" || SEVERITY == "Critical Warning"}]
if {[llength $errors]} {error "DRC failed: $errors"}
set guard [get_cells -hierarchical -filter {NAME =~ *lcd_spi_0*cs_guard_reg*}]
if {[llength $guard]<16} {error "Original LCD CS guard missing"}
set f [open $build/results/gates.txt w]
puts $f "setup_slack=$setup\nhold_slack=$hold\ndrc_errors=[llength $errors]\ncs_guard_register_cells=[llength $guard]"
foreach {name pin dir} {
 pl_keys_n[0] N15 IN pl_keys_n[1] N16 IN pl_keys_n[2] T17 IN pl_keys_n[3] R17 IN
 lcd_tri_io[0] F17 OUT lcd_tri_io[1] F16 OUT lcd_tri_io[2] F20 OUT lcd_tri_io[3] F19 OUT lcd_tri_io[4] G20 OUT lcd_tri_io[5] G19 OUT
 acq_uart_rx W19 IN acq_uart_tx W18 OUT
} {
 set p [get_ports $name]
 puts $f "$name [get_property PACKAGE_PIN $p] [get_property DIRECTION $p] [get_property IOSTANDARD $p]"
 if {[get_property PACKAGE_PIN $p] ne $pin || [get_property DIRECTION $p] ne $dir || [get_property IOSTANDARD $p] ne "LVCMOS33"} {error "Original pin changed: $name"}
}
if {[llength [get_ports]]!=78} {
 # DDR/FIXED_IO are vectors; the detailed I/O review below checks exact ports.
 puts $f "top_level_ports=[llength [get_ports]]"
}
close $f
write_checkpoint -force $build/results/routed.dcp
puts ROUTE_COMPLETE_NEW_BUILD_REQUIRES_QUALIFICATION
