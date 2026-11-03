set root [file normalize [file join [file dirname [info script]] ..]]
set lib $root/vendor/blas/L1/include/hw
file mkdir $root/build
cd $root/build
open_project -reset shared
set_top pw_shared
set flags "-std=c++11 -I$root/src -I$lib -I$lib/xf_blas -ffp-contract=off"
add_files $root/src/pw_shared.cpp -cflags $flags
add_files -tb $root/tests/tb_shared.cpp -cflags $flags
open_solution -reset solution -flow_target vivado
set_part xc7z020clg400-2
create_clock -period 10
config_interface -m_axi_addr64=false
csim_design -argv "$root/data"
csynth_design
cosim_design -rtl verilog -tool xsim -trace_level none -argv "$root/data --rtl"
exit
