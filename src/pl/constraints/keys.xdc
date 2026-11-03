# AX7020 User Manual 7.7: active-low PL KEY1..KEY4, Banks35/34, 3.3V.
set_property PACKAGE_PIN N15 [get_ports {pl_keys_n[0]}]
set_property PACKAGE_PIN N16 [get_ports {pl_keys_n[1]}]
set_property PACKAGE_PIN T17 [get_ports {pl_keys_n[2]}]
set_property PACKAGE_PIN R17 [get_ports {pl_keys_n[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {pl_keys_n[*]}]
# Board already has pull-ups; weak internal pull-ups retain released defaults.
set_property PULLUP true [get_ports {pl_keys_n[*]}]
# Only the asynchronous pad -> first synchronizer stage is false-pathed.
set_false_path -from [get_ports {pl_keys_n[*]}] -to [get_cells -hier -filter {NAME =~ *key_meta_reg*}]
