open_project shared
set_top pw_shared
set flags {-std=c++11 -I"E:/FPGA_competition/Github/src/pl/hls/pw/src" -I"E:/FPGA_competition/Github/src/pl/hls/pw/vendor/blas/L1/include/hw" -I"E:/FPGA_competition/Github/src/pl/hls/pw/vendor/blas/L1/include/hw/xf_blas" -ffp-contract=off}
add_files {E:/FPGA_competition/Github/src/pl/hls/pw/src/pw_shared.cpp} -cflags $flags
add_files -tb {E:/FPGA_competition/Github/src/pl/hls/pw/tests/tb_shared.cpp} -cflags $flags
open_solution solution -flow_target vivado
set_part xc7z020clg400-2
create_clock -period 10
config_interface -m_axi_addr64=false
csim_design -argv {E:/FPGA_competition/Github/src/pl/hls/pw/data}
puts PW_REQUESTED_STAGE_COMPLETE_NO_BOARD_ACCESS
exit
