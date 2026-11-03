# AX7020 official manual section 7.5, J11 pins 3..8 (Bank35, 3.3V).
# [0]=SCK, [1]=MOSI(SDA), [2]=CS, [3]=D/C(WR), [4]=RST, [5]=backlight(PWR).
set_property PACKAGE_PIN F17 [get_ports {lcd_tri_io[0]}]
set_property PACKAGE_PIN F16 [get_ports {lcd_tri_io[1]}]
set_property PACKAGE_PIN F20 [get_ports {lcd_tri_io[2]}]
set_property PACKAGE_PIN F19 [get_ports {lcd_tri_io[3]}]
set_property PACKAGE_PIN G20 [get_ports {lcd_tri_io[4]}]
set_property PACKAGE_PIN G19 [get_ports {lcd_tri_io[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {lcd_tri_io[*]}]
set_property DRIVE 4 [get_ports {lcd_tri_io[*]}]
set_property SLEW SLOW [get_ports {lcd_tri_io[*]}]
set_property PULLUP true [get_ports {lcd_tri_io[2]}]
set_property PULLDOWN true [get_ports {lcd_tri_io[0] lcd_tri_io[1] lcd_tri_io[3] lcd_tri_io[4] lcd_tri_io[5]}]
# GPIO defaults to inputs after PS reset. CS weak high, RST/backlight weak low.
# SPI pins must remain disconnected until wiring has been checked with power off.
