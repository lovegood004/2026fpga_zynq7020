set root {E:/FPGA_competition/AX7020_Project}
# Explicit encoding and read preflight avoid ambient Windows Tcl locale state.
foreach f [list {E:/Xilinx/Vivado/2021.1/scripts/builtin.tcl} $root/build_work/acq_preview/system/results/design_1_acq.tcl] {
 set ch [open $f r]
 fconfigure $ch -encoding utf-8
 puts "TCL_READ_PREFLIGHT [file tail $f] [string length [read $ch]]"
 close $ch
}
set build $root/build_work/lung_cnn_lane8_20261101/system
file mkdir $build/results
if {[file exists $build/system/lung_pl.xpr]} {error "Preserve existing isolated system before rebuilding"}
set AX_CNN_REPAIR_CH [open $root/build_work/acq_preview/system/results/design_1_acq.tcl r]
fconfigure $AX_CNN_REPAIR_CH -encoding utf-8
set AX_CNN_REPAIR_BD_SCRIPT [read $AX_CNN_REPAIR_CH]
close $AX_CNN_REPAIR_CH
create_project lung_pl $build/system -part xc7z020clg400-2
create_bd_design design_1
set_property source_mgmt_mode All [current_project]
set_property target_language Verilog [current_project]
set_property ip_repo_paths [list $root/platform/pw_ip $root/build_work/lung_cnn_lane8_20261101/hls/conv/solution/impl/ip $root/build_work/linear_pl/hls/dot/solution/impl/ip] [current_project]
update_ip_catalog
add_files -norecurse [list $root/platform/lcd_pl/lcd_spi_axil.v $root/platform/lcd_pl/keys_lcd_pl.v $root/platform/acq_preview/acq_preview_axil.v]
update_compile_order -fileset sources_1
# Vivado's source wrapper reports EILSEQ despite a successful read of this
# ASCII script. Execute the exact hash-bound bytes through standard Tcl I/O.
# Its script_folder variable is metadata only (never used by the BD builder).
puts "REPAIR_BEFORE_BD root=$root encoding=[encoding system]"
uplevel #0 $AX_CNN_REPAIR_BD_SCRIPT
set ps [get_bd_cells processing_system7_0]
set_property CONFIG.NUM_MI 5 [get_bd_cells control_bus]
set_property CONFIG.NUM_SI 3 [get_bd_cells memory_bus]
set ip [create_bd_cell -type ip -vlnv ax7020:lung:lung_conv_fp32:1.0 lung_conv_0]
set slice [create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 lung_memory_slice]
connect_bd_intf_net [get_bd_intf_pins $ip/s_axi_control] [get_bd_intf_pins control_bus/M03_AXI]
connect_bd_intf_net [get_bd_intf_pins $ip/m_axi_mem] [get_bd_intf_pins $slice/S_AXI]
connect_bd_intf_net [get_bd_intf_pins $slice/M_AXI] [get_bd_intf_pins memory_bus/S01_AXI]
connect_bd_net [get_bd_pins $ps/FCLK_CLK0] [get_bd_pins $ip/ap_clk] [get_bd_pins $slice/aclk] [get_bd_pins control_bus/M03_ACLK] [get_bd_pins memory_bus/S01_ACLK]
connect_bd_net [get_bd_pins reset_100m/peripheral_aresetn] [get_bd_pins $ip/ap_rst_n] [get_bd_pins $slice/aresetn]
connect_bd_net [get_bd_pins reset_100m/interconnect_aresetn] [get_bd_pins control_bus/M03_ARESETN] [get_bd_pins memory_bus/S01_ARESETN]
assign_bd_address -offset 0x43c60000 -range 0x10000 -target_address_space [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs $ip/s_axi_control/Reg] -force
assign_bd_address -offset 0 -range 0x40000000 -target_address_space [get_bd_addr_spaces $ip/Data_m_axi_mem] [get_bd_addr_segs processing_system7_0/S_AXI_HP0/HP0_DDR_LOWOCM] -force
set linear [create_bd_cell -type ip -vlnv ax7020:lung:lung_linear_int8:1.0 lung_linear_0]
set linear_slice [create_bd_cell -type ip -vlnv xilinx.com:ip:axi_register_slice:2.1 linear_memory_slice]
connect_bd_intf_net [get_bd_intf_pins $linear/s_axi_control] [get_bd_intf_pins control_bus/M04_AXI]
connect_bd_intf_net [get_bd_intf_pins $linear/m_axi_mem] [get_bd_intf_pins $linear_slice/S_AXI]
connect_bd_intf_net [get_bd_intf_pins $linear_slice/M_AXI] [get_bd_intf_pins memory_bus/S02_AXI]
connect_bd_net [get_bd_pins $ps/FCLK_CLK0] [get_bd_pins $linear/ap_clk] [get_bd_pins $linear_slice/aclk] [get_bd_pins control_bus/M04_ACLK] [get_bd_pins memory_bus/S02_ACLK]
connect_bd_net [get_bd_pins reset_100m/peripheral_aresetn] [get_bd_pins $linear/ap_rst_n] [get_bd_pins $linear_slice/aresetn]
connect_bd_net [get_bd_pins reset_100m/interconnect_aresetn] [get_bd_pins control_bus/M04_ARESETN] [get_bd_pins memory_bus/S02_ARESETN]
assign_bd_address -offset 0x43c70000 -range 0x10000 -target_address_space [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs $linear/s_axi_control/Reg] -force
assign_bd_address -offset 0 -range 0x40000000 -target_address_space [get_bd_addr_spaces $linear/Data_m_axi_mem] [get_bd_addr_segs processing_system7_0/S_AXI_HP0/HP0_DDR_LOWOCM] -force
validate_bd_design
save_bd_design
generate_target all [get_files design_1.bd]
add_files -norecurse [make_wrapper -files [get_files design_1.bd] -top]
foreach xdc [list $root/outputs/acquisition_uart_20261023/vivado/acquisition_uart.xdc $root/outputs/lcd_control_20261023/vivado/lcd.xdc $root/platform/keys_ui/keys.xdc] {add_files -fileset constrs_1 -norecurse $xdc}
set_property top design_1_wrapper [current_fileset]
update_compile_order -fileset sources_1
write_bd_tcl -force $build/results/design_1_lung.tcl
write_hw_platform -fixed -force -file $build/results/lung_pl_preimpl.xsa
puts LUNG_PL_SYSTEM_CREATED_NO_BOARD_ACCESS
close_project
