set root [file normalize [file join [file dirname [info script]] ..]]
cd $root/build
open_project ip_export
open_solution solution
if {[catch {export_design -format ip_catalog -version 1.0 -display_name {MobileHeart Shared FP32 Pointwise}} msg opts]} {
 puts stderr "IP_EXPORT_ERROR=$msg"
 puts stderr [dict get $opts -errorinfo]
 exit 1
}
exit
