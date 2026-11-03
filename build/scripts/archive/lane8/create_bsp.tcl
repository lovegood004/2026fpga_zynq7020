set root {E:/FPGA_competition/AX7020_Project}
set out $root/build_work/lung_cnn_lane8_20261101/system/bsp
hsi open_hw_design $root/build_work/lung_cnn_lane8_20261101/system/results/lung_pl.xsa
hsi create_sw_design lung_bsp -proc ps7_cortexa9_0 -os standalone
hsi set_property CONFIG.stdin ps7_uart_1 [hsi get_os]
hsi set_property CONFIG.stdout ps7_uart_1 [hsi get_os]
hsi generate_bsp -dir $out
hsi close_hw_design [hsi current_hw_design]
puts LUNG_PL_BSP_CREATED_NO_BOARD_ACCESS
