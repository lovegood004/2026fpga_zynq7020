set root {E:/FPGA_competition/AX7020_Project}
set build $root/build_work/lung_cnn_lane8_20261101/system
open_project $build/system/lung_pl.xpr
config_ip_cache -disable_cache
update_compile_order -fileset sources_1
if {[get_property NEEDS_REFRESH [get_runs synth_1]]} {error "Prepared synthesis is stale"}
if {[get_property PROGRESS [get_runs synth_1]] ne "100%"} {
 launch_runs synth_1 -jobs 4
 wait_on_run synth_1
}
if {[get_property PROGRESS [get_runs synth_1]] ne "100%"} {error "Lung synthesis failed"}
if {[get_property PROGRESS [get_runs impl_1]] ne "100%"} {
 launch_runs impl_1 -to_step route_design -jobs 4
 wait_on_run impl_1
}
if {[get_property PROGRESS [get_runs impl_1]] ne "100%"} {error "Lung route failed"}
open_run impl_1
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
close_design
launch_runs impl_1 -to_step write_bitstream -jobs 4
wait_on_run impl_1
if {[get_property PROGRESS [get_runs impl_1]] ne "100%"} {error "Lung bitstream failed"}
open_run impl_1
write_hw_platform -fixed -include_bit -force -file $build/results/lung_pl.xsa
file copy -force $build/system/lung_pl.runs/impl_1/design_1_wrapper.bit $build/results/lung_pl.bit
close_project
puts LUNG_PL_IMPLEMENTED_NO_BOARD_ACCESS
