# ALINX AX7020 official user manual, section 7.4, J10.
# J10 pin 1=GND; pin 2=+5V (not used); pin 3=W19; pin 4=W18.
set_property PACKAGE_PIN W19 [get_ports acq_uart_rx]
set_property PACKAGE_PIN W18 [get_ports acq_uart_tx]
set_property IOSTANDARD LVCMOS33 [get_ports {acq_uart_rx acq_uart_tx}]
set_property PULLUP true [get_ports acq_uart_rx]
set_property DRIVE 4 [get_ports acq_uart_tx]
set_property SLEW SLOW [get_ports acq_uart_tx]
