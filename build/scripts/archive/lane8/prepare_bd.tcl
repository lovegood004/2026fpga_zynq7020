set root {E:/FPGA_competition/AX7020_Project}
set build $root/build_work/lung_cnn_lane8_20261101/system
open_project $build/system/lung_pl.xpr
open_bd_design [get_files design_1.bd]
generate_target simulation [get_files design_1.bd]
export_simulation -of_objects [get_files design_1.bd] -simulator xsim -directory $build/sim_export -force
close_project
puts LUNG_PL_BD_SIMULATION_PREPARED_NO_BOARD_ACCESS
