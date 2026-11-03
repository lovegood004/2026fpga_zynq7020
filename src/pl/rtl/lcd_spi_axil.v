`timescale 1ns/1ps
// Single-clock AXI4-Lite FIFO + mode-0 SPI. No PS GPIO drives these pins.
// DATA: [15:0] big-endian payload, [16] DC, [17] LAST, [18] TWO_BYTES.
// HALF >= MIN_HALF (production 50 at 100 MHz). Divisor changes only at idle.
module lcd_spi_axil #(
    parameter integer FIFO_DEPTH=1024,
    parameter integer MIN_HALF=50,
    parameter integer DEFAULT_HALF=50,
    parameter integer GAP_TIMEOUT=1000000
)(
    (* X_INTERFACE_PARAMETER = "ASSOCIATED_BUSIF S_AXI, ASSOCIATED_RESET aresetn" *)
    (* X_INTERFACE_INFO = "xilinx.com:signal:clock:1.0 aclk CLK" *) input wire aclk,
    (* X_INTERFACE_PARAMETER = "POLARITY ACTIVE_LOW" *)
    (* X_INTERFACE_INFO = "xilinx.com:signal:reset:1.0 aresetn RST" *) input wire aresetn,
    (* X_INTERFACE_PARAMETER = "PROTOCOL AXI4LITE, DATA_WIDTH 32, ADDR_WIDTH 8, HAS_BURST 0, HAS_LOCK 0, HAS_CACHE 0, HAS_QOS 0, HAS_REGION 0, SUPPORTS_NARROW_BURST 0, NUM_READ_OUTSTANDING 1, NUM_WRITE_OUTSTANDING 1" *)
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWADDR" *) input wire [7:0] s_axi_awaddr,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWPROT" *) input wire [2:0] s_axi_awprot,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWVALID" *) input wire s_axi_awvalid,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI AWREADY" *) output wire s_axi_awready,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WDATA" *) input wire [31:0] s_axi_wdata,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WSTRB" *) input wire [3:0] s_axi_wstrb,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WVALID" *) input wire s_axi_wvalid,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI WREADY" *) output wire s_axi_wready,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BRESP" *) output reg [1:0] s_axi_bresp,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BVALID" *) output reg s_axi_bvalid,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI BREADY" *) input wire s_axi_bready,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARADDR" *) input wire [7:0] s_axi_araddr,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARPROT" *) input wire [2:0] s_axi_arprot,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARVALID" *) input wire s_axi_arvalid,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI ARREADY" *) output wire s_axi_arready,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RDATA" *) output reg [31:0] s_axi_rdata,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RRESP" *) output reg [1:0] s_axi_rresp,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RVALID" *) output reg s_axi_rvalid,
    (* X_INTERFACE_INFO = "xilinx.com:interface:aximm:1.0 S_AXI RREADY" *) input wire s_axi_rready,
    output reg lcd_sck, output reg lcd_mosi, output reg lcd_cs_n,
    output reg lcd_dc, output wire lcd_rst_n, output wire lcd_bl
);
localparam integer PW=$clog2(FIFO_DEPTH);
reg [18:0] fifo [0:FIFO_DEPTH-1];
reg [PW-1:0] wr_ptr,rd_ptr;
reg [PW:0] count;
reg [2:0] control;
reg [15:0] half_period,ticks,cs_guard;
reg [31:0] errors,bytes_sent,transactions;
reg [31:0] gap_ticks;
reg [7:0] awaddr;
reg [31:0] wdata;
reg [3:0] wstrb;
reg aw_pending,w_pending;
reg active,second_pending,last_byte;
reg [7:0] data,second_data;
reg [2:0] bitpos;
wire idle=(count==0)&&!active&&lcd_cs_n&&(cs_guard==0);
assign lcd_rst_n=control[1];
assign lcd_bl=control[2];
assign s_axi_awready=aresetn&&!aw_pending&&!s_axi_bvalid;
assign s_axi_wready=aresetn&&!w_pending&&!s_axi_bvalid;
assign s_axi_arready=aresetn&&!s_axi_rvalid;
wire write_fire=aw_pending&&w_pending&&!s_axi_bvalid;
wire data_write=write_fire&&(awaddr==8'h10);
wire valid_data=(wstrb[2:0]==3'b111)&&(wdata[31:19]==0)&&
                (wdata[18]||(wdata[15:8]==0));
wire push=data_write&&control[0]&&(count<FIFO_DEPTH)&&valid_data;
wire abort_write=write_fire&&(awaddr==0)&&wstrb[0]&&(wdata[7:3]==0)&&!wdata[0];
wire pop=control[0]&&!active&&(count!=0)&&(cs_guard==0)&&!abort_write;

always @(posedge aclk) begin
    if(!aresetn) begin
        control<=0;half_period<=DEFAULT_HALF;errors<=0;bytes_sent<=0;transactions<=0;gap_ticks<=0;
        wr_ptr<=0;rd_ptr<=0;count<=0;active<=0;second_pending<=0;last_byte<=0;
        data<=0;second_data<=0;bitpos<=7;ticks<=0;cs_guard<=0;
        lcd_sck<=0;lcd_mosi<=0;lcd_cs_n<=1;lcd_dc<=0;
        awaddr<=0;wdata<=0;wstrb<=0;aw_pending<=0;w_pending<=0;
        s_axi_bresp<=0;s_axi_bvalid<=0;s_axi_rdata<=0;s_axi_rresp<=0;s_axi_rvalid<=0;
    end else begin
        if(cs_guard!=0)cs_guard<=cs_guard-1'b1;
        if(s_axi_awready&&s_axi_awvalid)begin awaddr<=s_axi_awaddr;aw_pending<=1;end
        if(s_axi_wready&&s_axi_wvalid)begin wdata<=s_axi_wdata;wstrb<=s_axi_wstrb;w_pending<=1;end
        if(s_axi_bvalid&&s_axi_bready)s_axi_bvalid<=0;
        if(s_axi_rvalid&&s_axi_rready)s_axi_rvalid<=0;
        if(s_axi_arvalid&&s_axi_arready)begin
            s_axi_rvalid<=1;s_axi_rresp<=0;
            case(s_axi_araddr)
                8'h00:s_axi_rdata<={29'b0,control};
                8'h04:s_axi_rdata<={16'b0,half_period};
                8'h08:s_axi_rdata<={14'b0,!idle,control[0],16'b0}|count;
                8'h0c:s_axi_rdata<=errors;
                8'h14:s_axi_rdata<=FIFO_DEPTH-count;
                8'h18:s_axi_rdata<=bytes_sent;
                8'h1c:s_axi_rdata<=transactions;
                8'h20:s_axi_rdata<=32'h4c434431; // LCD1 ABI identity
                default:begin s_axi_rdata<=0;s_axi_rresp<=2;end
            endcase
        end
        if(write_fire)begin
            aw_pending<=0;w_pending<=0;s_axi_bvalid<=1;s_axi_bresp<=0;
            case(awaddr)
                8'h00:begin
                    if(wstrb[0]&&(wdata[7:3]==0))begin
                        // RST/BL changes during traffic are explicitly rejected.
                        if(!idle&&wdata[0]&&(wdata[2:1]!=control[2:1]))begin
                            s_axi_bresp<=2;errors<=errors|4;
                        end else control<=wdata[2:0];
                    end else if(wstrb!=0)begin s_axi_bresp<=2;errors<=errors|2;end
                end
                8'h04:begin
                    if(wstrb[1:0]==3&&wdata>=MIN_HALF&&wdata<=65535&&idle)
                        half_period<=wdata[15:0];
                    else begin s_axi_bresp<=2;errors<=errors|4;end
                end
                8'h0c:begin
                    if(wstrb[0])errors<=errors&~{24'b0,wdata[7:0]};
                end
                8'h10:begin
                    if(!push)begin s_axi_bresp<=2;errors<=errors|(count==FIFO_DEPTH?1:2);end
                end
                default:begin s_axi_bresp<=2;errors<=errors|2;end
            endcase
        end
        if(abort_write)begin
            wr_ptr<=0;rd_ptr<=0;count<=0;active<=0;second_pending<=0;cs_guard<=0;
            lcd_sck<=0;lcd_mosi<=0;lcd_cs_n<=1;lcd_dc<=0;
        end else begin
            case({push,pop})
                2'b10:count<=count+1'b1;
                2'b01:count<=count-1'b1;
                default:count<=count;
            endcase
            if(push)begin fifo[wr_ptr]<=wdata[18:0];wr_ptr<=wr_ptr+1'b1;end
            // One-cycle distributed read; 19-bit x1024 is small and bounded.
            if(pop)begin
                data<=fifo[rd_ptr][18]?fifo[rd_ptr][15:8]:fifo[rd_ptr][7:0];
                lcd_mosi<=fifo[rd_ptr][18]?fifo[rd_ptr][15]:fifo[rd_ptr][7];
                second_data<=fifo[rd_ptr][7:0];second_pending<=fifo[rd_ptr][18];
                last_byte<=fifo[rd_ptr][17];lcd_dc<=fifo[rd_ptr][16];
                lcd_cs_n<=0;lcd_sck<=0;active<=1;bitpos<=7;ticks<=half_period-1'b1;
                rd_ptr<=rd_ptr+1'b1;
            end else if(active)begin
                if(ticks!=0)ticks<=ticks-1'b1;
                else begin
                    ticks<=half_period-1'b1;
                    if(!lcd_sck)lcd_sck<=1;
                    else begin
                        lcd_sck<=0;
                        if(bitpos==0)begin
                            bytes_sent<=bytes_sent+1'b1;
                            if(second_pending)begin
                                second_pending<=0;data<=second_data;
                                lcd_mosi<=second_data[7];bitpos<=7;
                            end else begin
                                active<=0;
                                if(last_byte)begin
                                    lcd_cs_n<=1;transactions<=transactions+1'b1;
                                    // Keep CS inactive for a full half-period too.
                                    cs_guard<=half_period-1'b1;
                                end
                            end
                        end else begin bitpos<=bitpos-1'b1;lcd_mosi<=data[bitpos-1'b1];end
                    end
                end
            end
        end
        // A truncated transaction cannot hold CS forever. Only a gap with no
        // active byte and no FIFO data times out; a long valid frame never does.
        if(!idle&&!active&&count==0&&!lcd_cs_n&&!abort_write&&!push)begin
            if(gap_ticks==GAP_TIMEOUT-1)begin
                gap_ticks<=0;lcd_cs_n<=1;lcd_sck<=0;lcd_mosi<=0;
                control[0]<=0;errors<=errors|8;
            end else gap_ticks<=gap_ticks+1'b1;
        end else gap_ticks<=0;
    end
end
wire unused=&{1'b0,s_axi_awprot,s_axi_arprot};
endmodule
