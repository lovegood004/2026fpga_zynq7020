set root [file normalize [file join [file dirname [info script]] ..]]
cd $root/build
open_project shared
open_solution solution
csim_design -argv "$root/data"
cosim_design -rtl verilog -tool xsim -trace_level none -argv "$root/data --rtl"
exit
