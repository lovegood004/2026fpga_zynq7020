`timescale 1ns/1ps
// Physical LCD outputs have exactly one owner, including reset and backlight.
// Original EMIO low six bits are readback only. PS GPIO cannot drive the pads.
module keys_lcd_pl(
    input wire clk,input wire resetn,input wire [3:0] pl_keys_n,
    input wire [5:0] lcd_out,output wire [9:0] gpio_i,
    output wire [5:0] lcd_tri_io
);
(* ASYNC_REG="TRUE",SHREG_EXTRACT="NO" *)reg [3:0] key_meta=4'hf;
(* ASYNC_REG="TRUE",SHREG_EXTRACT="NO" *)reg [3:0] key_sync=4'hf;
always @(posedge clk)begin
    if(!resetn)begin key_meta<=4'hf;key_sync<=4'hf;end
    else begin key_meta<=pl_keys_n;key_sync<=key_meta;end
end
assign gpio_i={key_sync,lcd_out};
assign lcd_tri_io=lcd_out;
endmodule
