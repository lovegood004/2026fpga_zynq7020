
    wire dl_reset;
    wire dl_clock;
    assign dl_reset = ap_rst_n;
    assign dl_clock = ap_clk;
    wire [2:0] proc_0_data_FIFO_blk;
    wire [2:0] proc_0_data_PIPO_blk;
    wire [2:0] proc_0_start_FIFO_blk;
    wire [2:0] proc_0_TLF_FIFO_blk;
    wire [2:0] proc_0_input_sync_blk;
    wire [2:0] proc_0_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_0;
    reg [2:0] proc_dep_vld_vec_0_reg;
    wire [2:0] in_chan_dep_vld_vec_0;
    wire [80:0] in_chan_dep_data_vec_0;
    wire [2:0] token_in_vec_0;
    wire [2:0] out_chan_dep_vld_vec_0;
    wire [26:0] out_chan_dep_data_0;
    wire [2:0] token_out_vec_0;
    wire dl_detect_out_0;
    wire dep_chan_vld_1_0;
    wire [26:0] dep_chan_data_1_0;
    wire token_1_0;
    wire dep_chan_vld_2_0;
    wire [26:0] dep_chan_data_2_0;
    wire token_2_0;
    wire dep_chan_vld_3_0;
    wire [26:0] dep_chan_data_3_0;
    wire token_3_0;
    wire [2:0] proc_1_data_FIFO_blk;
    wire [2:0] proc_1_data_PIPO_blk;
    wire [2:0] proc_1_start_FIFO_blk;
    wire [2:0] proc_1_TLF_FIFO_blk;
    wire [2:0] proc_1_input_sync_blk;
    wire [2:0] proc_1_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_1;
    reg [2:0] proc_dep_vld_vec_1_reg;
    wire [2:0] in_chan_dep_vld_vec_1;
    wire [80:0] in_chan_dep_data_vec_1;
    wire [2:0] token_in_vec_1;
    wire [2:0] out_chan_dep_vld_vec_1;
    wire [26:0] out_chan_dep_data_1;
    wire [2:0] token_out_vec_1;
    wire dl_detect_out_1;
    wire dep_chan_vld_0_1;
    wire [26:0] dep_chan_data_0_1;
    wire token_0_1;
    wire dep_chan_vld_2_1;
    wire [26:0] dep_chan_data_2_1;
    wire token_2_1;
    wire dep_chan_vld_3_1;
    wire [26:0] dep_chan_data_3_1;
    wire token_3_1;
    wire [2:0] proc_2_data_FIFO_blk;
    wire [2:0] proc_2_data_PIPO_blk;
    wire [2:0] proc_2_start_FIFO_blk;
    wire [2:0] proc_2_TLF_FIFO_blk;
    wire [2:0] proc_2_input_sync_blk;
    wire [2:0] proc_2_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_2;
    reg [2:0] proc_dep_vld_vec_2_reg;
    wire [3:0] in_chan_dep_vld_vec_2;
    wire [107:0] in_chan_dep_data_vec_2;
    wire [3:0] token_in_vec_2;
    wire [2:0] out_chan_dep_vld_vec_2;
    wire [26:0] out_chan_dep_data_2;
    wire [2:0] token_out_vec_2;
    wire dl_detect_out_2;
    wire dep_chan_vld_0_2;
    wire [26:0] dep_chan_data_0_2;
    wire token_0_2;
    wire dep_chan_vld_1_2;
    wire [26:0] dep_chan_data_1_2;
    wire token_1_2;
    wire dep_chan_vld_3_2;
    wire [26:0] dep_chan_data_3_2;
    wire token_3_2;
    wire dep_chan_vld_26_2;
    wire [26:0] dep_chan_data_26_2;
    wire token_26_2;
    wire [6:0] proc_3_data_FIFO_blk;
    wire [6:0] proc_3_data_PIPO_blk;
    wire [6:0] proc_3_start_FIFO_blk;
    wire [6:0] proc_3_TLF_FIFO_blk;
    wire [6:0] proc_3_input_sync_blk;
    wire [6:0] proc_3_output_sync_blk;
    wire [6:0] proc_dep_vld_vec_3;
    reg [6:0] proc_dep_vld_vec_3_reg;
    wire [5:0] in_chan_dep_vld_vec_3;
    wire [161:0] in_chan_dep_data_vec_3;
    wire [5:0] token_in_vec_3;
    wire [6:0] out_chan_dep_vld_vec_3;
    wire [26:0] out_chan_dep_data_3;
    wire [6:0] token_out_vec_3;
    wire dl_detect_out_3;
    wire dep_chan_vld_0_3;
    wire [26:0] dep_chan_data_0_3;
    wire token_0_3;
    wire dep_chan_vld_1_3;
    wire [26:0] dep_chan_data_1_3;
    wire token_1_3;
    wire dep_chan_vld_4_3;
    wire [26:0] dep_chan_data_4_3;
    wire token_4_3;
    wire dep_chan_vld_5_3;
    wire [26:0] dep_chan_data_5_3;
    wire token_5_3;
    wire dep_chan_vld_6_3;
    wire [26:0] dep_chan_data_6_3;
    wire token_6_3;
    wire dep_chan_vld_7_3;
    wire [26:0] dep_chan_data_7_3;
    wire token_7_3;
    wire [7:0] proc_4_data_FIFO_blk;
    wire [7:0] proc_4_data_PIPO_blk;
    wire [7:0] proc_4_start_FIFO_blk;
    wire [7:0] proc_4_TLF_FIFO_blk;
    wire [7:0] proc_4_input_sync_blk;
    wire [7:0] proc_4_output_sync_blk;
    wire [7:0] proc_dep_vld_vec_4;
    reg [7:0] proc_dep_vld_vec_4_reg;
    wire [8:0] in_chan_dep_vld_vec_4;
    wire [242:0] in_chan_dep_data_vec_4;
    wire [8:0] token_in_vec_4;
    wire [7:0] out_chan_dep_vld_vec_4;
    wire [26:0] out_chan_dep_data_4;
    wire [7:0] token_out_vec_4;
    wire dl_detect_out_4;
    wire dep_chan_vld_3_4;
    wire [26:0] dep_chan_data_3_4;
    wire token_3_4;
    wire dep_chan_vld_5_4;
    wire [26:0] dep_chan_data_5_4;
    wire token_5_4;
    wire dep_chan_vld_7_4;
    wire [26:0] dep_chan_data_7_4;
    wire token_7_4;
    wire dep_chan_vld_9_4;
    wire [26:0] dep_chan_data_9_4;
    wire token_9_4;
    wire dep_chan_vld_11_4;
    wire [26:0] dep_chan_data_11_4;
    wire token_11_4;
    wire dep_chan_vld_13_4;
    wire [26:0] dep_chan_data_13_4;
    wire token_13_4;
    wire dep_chan_vld_15_4;
    wire [26:0] dep_chan_data_15_4;
    wire token_15_4;
    wire dep_chan_vld_17_4;
    wire [26:0] dep_chan_data_17_4;
    wire token_17_4;
    wire dep_chan_vld_19_4;
    wire [26:0] dep_chan_data_19_4;
    wire token_19_4;
    wire [5:0] proc_5_data_FIFO_blk;
    wire [5:0] proc_5_data_PIPO_blk;
    wire [5:0] proc_5_start_FIFO_blk;
    wire [5:0] proc_5_TLF_FIFO_blk;
    wire [5:0] proc_5_input_sync_blk;
    wire [5:0] proc_5_output_sync_blk;
    wire [5:0] proc_dep_vld_vec_5;
    reg [5:0] proc_dep_vld_vec_5_reg;
    wire [1:0] in_chan_dep_vld_vec_5;
    wire [53:0] in_chan_dep_data_vec_5;
    wire [1:0] token_in_vec_5;
    wire [5:0] out_chan_dep_vld_vec_5;
    wire [26:0] out_chan_dep_data_5;
    wire [5:0] token_out_vec_5;
    wire dl_detect_out_5;
    wire dep_chan_vld_3_5;
    wire [26:0] dep_chan_data_3_5;
    wire token_3_5;
    wire dep_chan_vld_21_5;
    wire [26:0] dep_chan_data_21_5;
    wire token_21_5;
    wire [2:0] proc_6_data_FIFO_blk;
    wire [2:0] proc_6_data_PIPO_blk;
    wire [2:0] proc_6_start_FIFO_blk;
    wire [2:0] proc_6_TLF_FIFO_blk;
    wire [2:0] proc_6_input_sync_blk;
    wire [2:0] proc_6_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_6;
    reg [2:0] proc_dep_vld_vec_6_reg;
    wire [4:0] in_chan_dep_vld_vec_6;
    wire [134:0] in_chan_dep_data_vec_6;
    wire [4:0] token_in_vec_6;
    wire [2:0] out_chan_dep_vld_vec_6;
    wire [26:0] out_chan_dep_data_6;
    wire [2:0] token_out_vec_6;
    wire dl_detect_out_6;
    wire dep_chan_vld_3_6;
    wire [26:0] dep_chan_data_3_6;
    wire token_3_6;
    wire dep_chan_vld_7_6;
    wire [26:0] dep_chan_data_7_6;
    wire token_7_6;
    wire dep_chan_vld_8_6;
    wire [26:0] dep_chan_data_8_6;
    wire token_8_6;
    wire dep_chan_vld_9_6;
    wire [26:0] dep_chan_data_9_6;
    wire token_9_6;
    wire dep_chan_vld_10_6;
    wire [26:0] dep_chan_data_10_6;
    wire token_10_6;
    wire [4:0] proc_7_data_FIFO_blk;
    wire [4:0] proc_7_data_PIPO_blk;
    wire [4:0] proc_7_start_FIFO_blk;
    wire [4:0] proc_7_TLF_FIFO_blk;
    wire [4:0] proc_7_input_sync_blk;
    wire [4:0] proc_7_output_sync_blk;
    wire [4:0] proc_dep_vld_vec_7;
    reg [4:0] proc_dep_vld_vec_7_reg;
    wire [5:0] in_chan_dep_vld_vec_7;
    wire [161:0] in_chan_dep_data_vec_7;
    wire [5:0] token_in_vec_7;
    wire [4:0] out_chan_dep_vld_vec_7;
    wire [26:0] out_chan_dep_data_7;
    wire [4:0] token_out_vec_7;
    wire dl_detect_out_7;
    wire dep_chan_vld_3_7;
    wire [26:0] dep_chan_data_3_7;
    wire token_3_7;
    wire dep_chan_vld_4_7;
    wire [26:0] dep_chan_data_4_7;
    wire token_4_7;
    wire dep_chan_vld_6_7;
    wire [26:0] dep_chan_data_6_7;
    wire token_6_7;
    wire dep_chan_vld_8_7;
    wire [26:0] dep_chan_data_8_7;
    wire token_8_7;
    wire dep_chan_vld_9_7;
    wire [26:0] dep_chan_data_9_7;
    wire token_9_7;
    wire dep_chan_vld_11_7;
    wire [26:0] dep_chan_data_11_7;
    wire token_11_7;
    wire [2:0] proc_8_data_FIFO_blk;
    wire [2:0] proc_8_data_PIPO_blk;
    wire [2:0] proc_8_start_FIFO_blk;
    wire [2:0] proc_8_TLF_FIFO_blk;
    wire [2:0] proc_8_input_sync_blk;
    wire [2:0] proc_8_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_8;
    reg [2:0] proc_dep_vld_vec_8_reg;
    wire [2:0] in_chan_dep_vld_vec_8;
    wire [80:0] in_chan_dep_data_vec_8;
    wire [2:0] token_in_vec_8;
    wire [2:0] out_chan_dep_vld_vec_8;
    wire [26:0] out_chan_dep_data_8;
    wire [2:0] token_out_vec_8;
    wire dl_detect_out_8;
    wire dep_chan_vld_7_8;
    wire [26:0] dep_chan_data_7_8;
    wire token_7_8;
    wire dep_chan_vld_21_8;
    wire [26:0] dep_chan_data_21_8;
    wire token_21_8;
    wire dep_chan_vld_23_8;
    wire [26:0] dep_chan_data_23_8;
    wire token_23_8;
    wire [5:0] proc_9_data_FIFO_blk;
    wire [5:0] proc_9_data_PIPO_blk;
    wire [5:0] proc_9_start_FIFO_blk;
    wire [5:0] proc_9_TLF_FIFO_blk;
    wire [5:0] proc_9_input_sync_blk;
    wire [5:0] proc_9_output_sync_blk;
    wire [5:0] proc_dep_vld_vec_9;
    reg [5:0] proc_dep_vld_vec_9_reg;
    wire [0:0] in_chan_dep_vld_vec_9;
    wire [26:0] in_chan_dep_data_vec_9;
    wire [0:0] token_in_vec_9;
    wire [5:0] out_chan_dep_vld_vec_9;
    wire [26:0] out_chan_dep_data_9;
    wire [5:0] token_out_vec_9;
    wire dl_detect_out_9;
    wire dep_chan_vld_4_9;
    wire [26:0] dep_chan_data_4_9;
    wire token_4_9;
    wire [2:0] proc_10_data_FIFO_blk;
    wire [2:0] proc_10_data_PIPO_blk;
    wire [2:0] proc_10_start_FIFO_blk;
    wire [2:0] proc_10_TLF_FIFO_blk;
    wire [2:0] proc_10_input_sync_blk;
    wire [2:0] proc_10_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_10;
    reg [2:0] proc_dep_vld_vec_10_reg;
    wire [5:0] in_chan_dep_vld_vec_10;
    wire [161:0] in_chan_dep_data_vec_10;
    wire [5:0] token_in_vec_10;
    wire [2:0] out_chan_dep_vld_vec_10;
    wire [26:0] out_chan_dep_data_10;
    wire [2:0] token_out_vec_10;
    wire dl_detect_out_10;
    wire dep_chan_vld_5_10;
    wire [26:0] dep_chan_data_5_10;
    wire token_5_10;
    wire dep_chan_vld_6_10;
    wire [26:0] dep_chan_data_6_10;
    wire token_6_10;
    wire dep_chan_vld_11_10;
    wire [26:0] dep_chan_data_11_10;
    wire token_11_10;
    wire dep_chan_vld_12_10;
    wire [26:0] dep_chan_data_12_10;
    wire token_12_10;
    wire dep_chan_vld_13_10;
    wire [26:0] dep_chan_data_13_10;
    wire token_13_10;
    wire dep_chan_vld_14_10;
    wire [26:0] dep_chan_data_14_10;
    wire token_14_10;
    wire [4:0] proc_11_data_FIFO_blk;
    wire [4:0] proc_11_data_PIPO_blk;
    wire [4:0] proc_11_start_FIFO_blk;
    wire [4:0] proc_11_TLF_FIFO_blk;
    wire [4:0] proc_11_input_sync_blk;
    wire [4:0] proc_11_output_sync_blk;
    wire [4:0] proc_dep_vld_vec_11;
    reg [4:0] proc_dep_vld_vec_11_reg;
    wire [6:0] in_chan_dep_vld_vec_11;
    wire [188:0] in_chan_dep_data_vec_11;
    wire [6:0] token_in_vec_11;
    wire [4:0] out_chan_dep_vld_vec_11;
    wire [26:0] out_chan_dep_data_11;
    wire [4:0] token_out_vec_11;
    wire dl_detect_out_11;
    wire dep_chan_vld_4_11;
    wire [26:0] dep_chan_data_4_11;
    wire token_4_11;
    wire dep_chan_vld_5_11;
    wire [26:0] dep_chan_data_5_11;
    wire token_5_11;
    wire dep_chan_vld_7_11;
    wire [26:0] dep_chan_data_7_11;
    wire token_7_11;
    wire dep_chan_vld_10_11;
    wire [26:0] dep_chan_data_10_11;
    wire token_10_11;
    wire dep_chan_vld_12_11;
    wire [26:0] dep_chan_data_12_11;
    wire token_12_11;
    wire dep_chan_vld_13_11;
    wire [26:0] dep_chan_data_13_11;
    wire token_13_11;
    wire dep_chan_vld_15_11;
    wire [26:0] dep_chan_data_15_11;
    wire token_15_11;
    wire [2:0] proc_12_data_FIFO_blk;
    wire [2:0] proc_12_data_PIPO_blk;
    wire [2:0] proc_12_start_FIFO_blk;
    wire [2:0] proc_12_TLF_FIFO_blk;
    wire [2:0] proc_12_input_sync_blk;
    wire [2:0] proc_12_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_12;
    reg [2:0] proc_dep_vld_vec_12_reg;
    wire [2:0] in_chan_dep_vld_vec_12;
    wire [80:0] in_chan_dep_data_vec_12;
    wire [2:0] token_in_vec_12;
    wire [2:0] out_chan_dep_vld_vec_12;
    wire [26:0] out_chan_dep_data_12;
    wire [2:0] token_out_vec_12;
    wire dl_detect_out_12;
    wire dep_chan_vld_11_12;
    wire [26:0] dep_chan_data_11_12;
    wire token_11_12;
    wire dep_chan_vld_21_12;
    wire [26:0] dep_chan_data_21_12;
    wire token_21_12;
    wire dep_chan_vld_23_12;
    wire [26:0] dep_chan_data_23_12;
    wire token_23_12;
    wire [5:0] proc_13_data_FIFO_blk;
    wire [5:0] proc_13_data_PIPO_blk;
    wire [5:0] proc_13_start_FIFO_blk;
    wire [5:0] proc_13_TLF_FIFO_blk;
    wire [5:0] proc_13_input_sync_blk;
    wire [5:0] proc_13_output_sync_blk;
    wire [5:0] proc_dep_vld_vec_13;
    reg [5:0] proc_dep_vld_vec_13_reg;
    wire [0:0] in_chan_dep_vld_vec_13;
    wire [26:0] in_chan_dep_data_vec_13;
    wire [0:0] token_in_vec_13;
    wire [5:0] out_chan_dep_vld_vec_13;
    wire [26:0] out_chan_dep_data_13;
    wire [5:0] token_out_vec_13;
    wire dl_detect_out_13;
    wire dep_chan_vld_4_13;
    wire [26:0] dep_chan_data_4_13;
    wire token_4_13;
    wire [2:0] proc_14_data_FIFO_blk;
    wire [2:0] proc_14_data_PIPO_blk;
    wire [2:0] proc_14_start_FIFO_blk;
    wire [2:0] proc_14_TLF_FIFO_blk;
    wire [2:0] proc_14_input_sync_blk;
    wire [2:0] proc_14_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_14;
    reg [2:0] proc_dep_vld_vec_14_reg;
    wire [5:0] in_chan_dep_vld_vec_14;
    wire [161:0] in_chan_dep_data_vec_14;
    wire [5:0] token_in_vec_14;
    wire [2:0] out_chan_dep_vld_vec_14;
    wire [26:0] out_chan_dep_data_14;
    wire [2:0] token_out_vec_14;
    wire dl_detect_out_14;
    wire dep_chan_vld_9_14;
    wire [26:0] dep_chan_data_9_14;
    wire token_9_14;
    wire dep_chan_vld_10_14;
    wire [26:0] dep_chan_data_10_14;
    wire token_10_14;
    wire dep_chan_vld_15_14;
    wire [26:0] dep_chan_data_15_14;
    wire token_15_14;
    wire dep_chan_vld_16_14;
    wire [26:0] dep_chan_data_16_14;
    wire token_16_14;
    wire dep_chan_vld_17_14;
    wire [26:0] dep_chan_data_17_14;
    wire token_17_14;
    wire dep_chan_vld_18_14;
    wire [26:0] dep_chan_data_18_14;
    wire token_18_14;
    wire [4:0] proc_15_data_FIFO_blk;
    wire [4:0] proc_15_data_PIPO_blk;
    wire [4:0] proc_15_start_FIFO_blk;
    wire [4:0] proc_15_TLF_FIFO_blk;
    wire [4:0] proc_15_input_sync_blk;
    wire [4:0] proc_15_output_sync_blk;
    wire [4:0] proc_dep_vld_vec_15;
    reg [4:0] proc_dep_vld_vec_15_reg;
    wire [6:0] in_chan_dep_vld_vec_15;
    wire [188:0] in_chan_dep_data_vec_15;
    wire [6:0] token_in_vec_15;
    wire [4:0] out_chan_dep_vld_vec_15;
    wire [26:0] out_chan_dep_data_15;
    wire [4:0] token_out_vec_15;
    wire dl_detect_out_15;
    wire dep_chan_vld_4_15;
    wire [26:0] dep_chan_data_4_15;
    wire token_4_15;
    wire dep_chan_vld_9_15;
    wire [26:0] dep_chan_data_9_15;
    wire token_9_15;
    wire dep_chan_vld_11_15;
    wire [26:0] dep_chan_data_11_15;
    wire token_11_15;
    wire dep_chan_vld_14_15;
    wire [26:0] dep_chan_data_14_15;
    wire token_14_15;
    wire dep_chan_vld_16_15;
    wire [26:0] dep_chan_data_16_15;
    wire token_16_15;
    wire dep_chan_vld_17_15;
    wire [26:0] dep_chan_data_17_15;
    wire token_17_15;
    wire dep_chan_vld_19_15;
    wire [26:0] dep_chan_data_19_15;
    wire token_19_15;
    wire [2:0] proc_16_data_FIFO_blk;
    wire [2:0] proc_16_data_PIPO_blk;
    wire [2:0] proc_16_start_FIFO_blk;
    wire [2:0] proc_16_TLF_FIFO_blk;
    wire [2:0] proc_16_input_sync_blk;
    wire [2:0] proc_16_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_16;
    reg [2:0] proc_dep_vld_vec_16_reg;
    wire [2:0] in_chan_dep_vld_vec_16;
    wire [80:0] in_chan_dep_data_vec_16;
    wire [2:0] token_in_vec_16;
    wire [2:0] out_chan_dep_vld_vec_16;
    wire [26:0] out_chan_dep_data_16;
    wire [2:0] token_out_vec_16;
    wire dl_detect_out_16;
    wire dep_chan_vld_15_16;
    wire [26:0] dep_chan_data_15_16;
    wire token_15_16;
    wire dep_chan_vld_21_16;
    wire [26:0] dep_chan_data_21_16;
    wire token_21_16;
    wire dep_chan_vld_24_16;
    wire [26:0] dep_chan_data_24_16;
    wire token_24_16;
    wire [4:0] proc_17_data_FIFO_blk;
    wire [4:0] proc_17_data_PIPO_blk;
    wire [4:0] proc_17_start_FIFO_blk;
    wire [4:0] proc_17_TLF_FIFO_blk;
    wire [4:0] proc_17_input_sync_blk;
    wire [4:0] proc_17_output_sync_blk;
    wire [4:0] proc_dep_vld_vec_17;
    reg [4:0] proc_dep_vld_vec_17_reg;
    wire [0:0] in_chan_dep_vld_vec_17;
    wire [26:0] in_chan_dep_data_vec_17;
    wire [0:0] token_in_vec_17;
    wire [4:0] out_chan_dep_vld_vec_17;
    wire [26:0] out_chan_dep_data_17;
    wire [4:0] token_out_vec_17;
    wire dl_detect_out_17;
    wire dep_chan_vld_4_17;
    wire [26:0] dep_chan_data_4_17;
    wire token_4_17;
    wire [2:0] proc_18_data_FIFO_blk;
    wire [2:0] proc_18_data_PIPO_blk;
    wire [2:0] proc_18_start_FIFO_blk;
    wire [2:0] proc_18_TLF_FIFO_blk;
    wire [2:0] proc_18_input_sync_blk;
    wire [2:0] proc_18_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_18;
    reg [2:0] proc_dep_vld_vec_18_reg;
    wire [5:0] in_chan_dep_vld_vec_18;
    wire [161:0] in_chan_dep_data_vec_18;
    wire [5:0] token_in_vec_18;
    wire [2:0] out_chan_dep_vld_vec_18;
    wire [26:0] out_chan_dep_data_18;
    wire [2:0] token_out_vec_18;
    wire dl_detect_out_18;
    wire dep_chan_vld_13_18;
    wire [26:0] dep_chan_data_13_18;
    wire token_13_18;
    wire dep_chan_vld_14_18;
    wire [26:0] dep_chan_data_14_18;
    wire token_14_18;
    wire dep_chan_vld_19_18;
    wire [26:0] dep_chan_data_19_18;
    wire token_19_18;
    wire dep_chan_vld_20_18;
    wire [26:0] dep_chan_data_20_18;
    wire token_20_18;
    wire dep_chan_vld_21_18;
    wire [26:0] dep_chan_data_21_18;
    wire token_21_18;
    wire dep_chan_vld_22_18;
    wire [26:0] dep_chan_data_22_18;
    wire token_22_18;
    wire [3:0] proc_19_data_FIFO_blk;
    wire [3:0] proc_19_data_PIPO_blk;
    wire [3:0] proc_19_start_FIFO_blk;
    wire [3:0] proc_19_TLF_FIFO_blk;
    wire [3:0] proc_19_input_sync_blk;
    wire [3:0] proc_19_output_sync_blk;
    wire [3:0] proc_dep_vld_vec_19;
    reg [3:0] proc_dep_vld_vec_19_reg;
    wire [4:0] in_chan_dep_vld_vec_19;
    wire [134:0] in_chan_dep_data_vec_19;
    wire [4:0] token_in_vec_19;
    wire [3:0] out_chan_dep_vld_vec_19;
    wire [26:0] out_chan_dep_data_19;
    wire [3:0] token_out_vec_19;
    wire dl_detect_out_19;
    wire dep_chan_vld_4_19;
    wire [26:0] dep_chan_data_4_19;
    wire token_4_19;
    wire dep_chan_vld_13_19;
    wire [26:0] dep_chan_data_13_19;
    wire token_13_19;
    wire dep_chan_vld_15_19;
    wire [26:0] dep_chan_data_15_19;
    wire token_15_19;
    wire dep_chan_vld_18_19;
    wire [26:0] dep_chan_data_18_19;
    wire token_18_19;
    wire dep_chan_vld_20_19;
    wire [26:0] dep_chan_data_20_19;
    wire token_20_19;
    wire [2:0] proc_20_data_FIFO_blk;
    wire [2:0] proc_20_data_PIPO_blk;
    wire [2:0] proc_20_start_FIFO_blk;
    wire [2:0] proc_20_TLF_FIFO_blk;
    wire [2:0] proc_20_input_sync_blk;
    wire [2:0] proc_20_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_20;
    reg [2:0] proc_dep_vld_vec_20_reg;
    wire [2:0] in_chan_dep_vld_vec_20;
    wire [80:0] in_chan_dep_data_vec_20;
    wire [2:0] token_in_vec_20;
    wire [2:0] out_chan_dep_vld_vec_20;
    wire [26:0] out_chan_dep_data_20;
    wire [2:0] token_out_vec_20;
    wire dl_detect_out_20;
    wire dep_chan_vld_19_20;
    wire [26:0] dep_chan_data_19_20;
    wire token_19_20;
    wire dep_chan_vld_21_20;
    wire [26:0] dep_chan_data_21_20;
    wire token_21_20;
    wire dep_chan_vld_24_20;
    wire [26:0] dep_chan_data_24_20;
    wire token_24_20;
    wire [6:0] proc_21_data_FIFO_blk;
    wire [6:0] proc_21_data_PIPO_blk;
    wire [6:0] proc_21_start_FIFO_blk;
    wire [6:0] proc_21_TLF_FIFO_blk;
    wire [6:0] proc_21_input_sync_blk;
    wire [6:0] proc_21_output_sync_blk;
    wire [6:0] proc_dep_vld_vec_21;
    reg [6:0] proc_dep_vld_vec_21_reg;
    wire [0:0] in_chan_dep_vld_vec_21;
    wire [26:0] in_chan_dep_data_vec_21;
    wire [0:0] token_in_vec_21;
    wire [6:0] out_chan_dep_vld_vec_21;
    wire [26:0] out_chan_dep_data_21;
    wire [6:0] token_out_vec_21;
    wire dl_detect_out_21;
    wire dep_chan_vld_5_21;
    wire [26:0] dep_chan_data_5_21;
    wire token_5_21;
    wire [0:0] proc_22_data_FIFO_blk;
    wire [0:0] proc_22_data_PIPO_blk;
    wire [0:0] proc_22_start_FIFO_blk;
    wire [0:0] proc_22_TLF_FIFO_blk;
    wire [0:0] proc_22_input_sync_blk;
    wire [0:0] proc_22_output_sync_blk;
    wire [0:0] proc_dep_vld_vec_22;
    reg [0:0] proc_dep_vld_vec_22_reg;
    wire [3:0] in_chan_dep_vld_vec_22;
    wire [107:0] in_chan_dep_data_vec_22;
    wire [3:0] token_in_vec_22;
    wire [0:0] out_chan_dep_vld_vec_22;
    wire [26:0] out_chan_dep_data_22;
    wire [0:0] token_out_vec_22;
    wire dl_detect_out_22;
    wire dep_chan_vld_17_22;
    wire [26:0] dep_chan_data_17_22;
    wire token_17_22;
    wire dep_chan_vld_18_22;
    wire [26:0] dep_chan_data_18_22;
    wire token_18_22;
    wire dep_chan_vld_23_22;
    wire [26:0] dep_chan_data_23_22;
    wire token_23_22;
    wire dep_chan_vld_24_22;
    wire [26:0] dep_chan_data_24_22;
    wire token_24_22;
    wire [3:0] proc_23_data_FIFO_blk;
    wire [3:0] proc_23_data_PIPO_blk;
    wire [3:0] proc_23_start_FIFO_blk;
    wire [3:0] proc_23_TLF_FIFO_blk;
    wire [3:0] proc_23_input_sync_blk;
    wire [3:0] proc_23_output_sync_blk;
    wire [3:0] proc_dep_vld_vec_23;
    reg [3:0] proc_dep_vld_vec_23_reg;
    wire [4:0] in_chan_dep_vld_vec_23;
    wire [134:0] in_chan_dep_data_vec_23;
    wire [4:0] token_in_vec_23;
    wire [3:0] out_chan_dep_vld_vec_23;
    wire [26:0] out_chan_dep_data_23;
    wire [3:0] token_out_vec_23;
    wire dl_detect_out_23;
    wire dep_chan_vld_5_23;
    wire [26:0] dep_chan_data_5_23;
    wire token_5_23;
    wire dep_chan_vld_8_23;
    wire [26:0] dep_chan_data_8_23;
    wire token_8_23;
    wire dep_chan_vld_9_23;
    wire [26:0] dep_chan_data_9_23;
    wire token_9_23;
    wire dep_chan_vld_12_23;
    wire [26:0] dep_chan_data_12_23;
    wire token_12_23;
    wire dep_chan_vld_25_23;
    wire [26:0] dep_chan_data_25_23;
    wire token_25_23;
    wire [3:0] proc_24_data_FIFO_blk;
    wire [3:0] proc_24_data_PIPO_blk;
    wire [3:0] proc_24_start_FIFO_blk;
    wire [3:0] proc_24_TLF_FIFO_blk;
    wire [3:0] proc_24_input_sync_blk;
    wire [3:0] proc_24_output_sync_blk;
    wire [3:0] proc_dep_vld_vec_24;
    reg [3:0] proc_dep_vld_vec_24_reg;
    wire [4:0] in_chan_dep_vld_vec_24;
    wire [134:0] in_chan_dep_data_vec_24;
    wire [4:0] token_in_vec_24;
    wire [3:0] out_chan_dep_vld_vec_24;
    wire [26:0] out_chan_dep_data_24;
    wire [3:0] token_out_vec_24;
    wire dl_detect_out_24;
    wire dep_chan_vld_13_24;
    wire [26:0] dep_chan_data_13_24;
    wire token_13_24;
    wire dep_chan_vld_16_24;
    wire [26:0] dep_chan_data_16_24;
    wire token_16_24;
    wire dep_chan_vld_17_24;
    wire [26:0] dep_chan_data_17_24;
    wire token_17_24;
    wire dep_chan_vld_20_24;
    wire [26:0] dep_chan_data_20_24;
    wire token_20_24;
    wire dep_chan_vld_25_24;
    wire [26:0] dep_chan_data_25_24;
    wire token_25_24;
    wire [2:0] proc_25_data_FIFO_blk;
    wire [2:0] proc_25_data_PIPO_blk;
    wire [2:0] proc_25_start_FIFO_blk;
    wire [2:0] proc_25_TLF_FIFO_blk;
    wire [2:0] proc_25_input_sync_blk;
    wire [2:0] proc_25_output_sync_blk;
    wire [2:0] proc_dep_vld_vec_25;
    reg [2:0] proc_dep_vld_vec_25_reg;
    wire [2:0] in_chan_dep_vld_vec_25;
    wire [80:0] in_chan_dep_data_vec_25;
    wire [2:0] token_in_vec_25;
    wire [2:0] out_chan_dep_vld_vec_25;
    wire [26:0] out_chan_dep_data_25;
    wire [2:0] token_out_vec_25;
    wire dl_detect_out_25;
    wire dep_chan_vld_23_25;
    wire [26:0] dep_chan_data_23_25;
    wire token_23_25;
    wire dep_chan_vld_24_25;
    wire [26:0] dep_chan_data_24_25;
    wire token_24_25;
    wire dep_chan_vld_26_25;
    wire [26:0] dep_chan_data_26_25;
    wire token_26_25;
    wire [1:0] proc_26_data_FIFO_blk;
    wire [1:0] proc_26_data_PIPO_blk;
    wire [1:0] proc_26_start_FIFO_blk;
    wire [1:0] proc_26_TLF_FIFO_blk;
    wire [1:0] proc_26_input_sync_blk;
    wire [1:0] proc_26_output_sync_blk;
    wire [1:0] proc_dep_vld_vec_26;
    reg [1:0] proc_dep_vld_vec_26_reg;
    wire [2:0] in_chan_dep_vld_vec_26;
    wire [80:0] in_chan_dep_data_vec_26;
    wire [2:0] token_in_vec_26;
    wire [1:0] out_chan_dep_vld_vec_26;
    wire [26:0] out_chan_dep_data_26;
    wire [1:0] token_out_vec_26;
    wire dl_detect_out_26;
    wire dep_chan_vld_2_26;
    wire [26:0] dep_chan_data_2_26;
    wire token_2_26;
    wire dep_chan_vld_21_26;
    wire [26:0] dep_chan_data_21_26;
    wire token_21_26;
    wire dep_chan_vld_25_26;
    wire [26:0] dep_chan_data_25_26;
    wire token_25_26;
    wire [26:0] dl_in_vec;
    wire dl_detect_out;
    wire token_clear;
    reg [26:0] origin;

    reg ap_done_reg_0;// for module grp_compute_fu_306.process_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_0 <= 'b0;
        end
        else begin
            ap_done_reg_0 <= grp_compute_fu_306.process_U0.ap_done & ~grp_compute_fu_306.process_U0.ap_continue;
        end
    end

    reg ap_done_reg_1;// for module grp_compute_fu_306.process_U0.tagAB_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_1 <= 'b0;
        end
        else begin
            ap_done_reg_1 <= grp_compute_fu_306.process_U0.tagAB_U0.ap_done & ~grp_compute_fu_306.process_U0.tagAB_U0.ap_continue;
        end
    end

    reg ap_done_reg_2;// for module grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_2 <= 'b0;
        end
        else begin
            ap_done_reg_2 <= grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.ap_done & ~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.ap_continue;
        end
    end

    reg ap_done_reg_3;// for module grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_3 <= 'b0;
        end
        else begin
            ap_done_reg_3 <= grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.ap_done & ~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.ap_continue;
        end
    end

    reg ap_done_reg_4;// for module grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_4 <= 'b0;
        end
        else begin
            ap_done_reg_4 <= grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.ap_done & ~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.ap_continue;
        end
    end

    reg ap_done_reg_5;// for module grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_5 <= 'b0;
        end
        else begin
            ap_done_reg_5 <= grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.ap_done & ~grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.ap_continue;
        end
    end

    reg ap_done_reg_6;// for module grp_compute_fu_306.process_U0.merge_U0.merge17_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_6 <= 'b0;
        end
        else begin
            ap_done_reg_6 <= grp_compute_fu_306.process_U0.merge_U0.merge17_U0.ap_done & ~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.ap_continue;
        end
    end

    reg ap_done_reg_7;// for module grp_compute_fu_306.process_U0.merge_U0.merge18_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_7 <= 'b0;
        end
        else begin
            ap_done_reg_7 <= grp_compute_fu_306.process_U0.merge_U0.merge18_U0.ap_done & ~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.ap_continue;
        end
    end

    reg ap_done_reg_8;// for module grp_compute_fu_306.process_U0.macs19_U0.muls23_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_8 <= 'b0;
        end
        else begin
            ap_done_reg_8 <= grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.ap_done & ~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.ap_continue;
        end
    end

    reg ap_done_reg_9;// for module grp_compute_fu_306.process_U0.macs20_U0.muls25_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_9 <= 'b0;
        end
        else begin
            ap_done_reg_9 <= grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.ap_done & ~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.ap_continue;
        end
    end

    reg ap_done_reg_10;// for module grp_compute_fu_306.process_U0.macs21_U0.muls27_U0
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            ap_done_reg_10 <= 'b0;
        end
        else begin
            ap_done_reg_10 <= grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.ap_done & ~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.ap_continue;
        end
    end

reg [15:0] trans_in_cnt_0;// for process grp_compute_fu_306.process_U0.tagAB_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_in_cnt_0 <= 16'h0;
    end
    else if (grp_compute_fu_306.process_U0.tagAB_U0.start_write == 1'b1) begin
        trans_in_cnt_0 <= trans_in_cnt_0 + 16'h1;
    end
    else begin
        trans_in_cnt_0 <= trans_in_cnt_0;
    end
end

reg [15:0] trans_out_cnt_0;// for process grp_compute_fu_306.process_U0.tagAB_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_out_cnt_0 <= 16'h0;
    end
    else if (grp_compute_fu_306.process_U0.tagAB_U0.ap_done == 1'b1 && grp_compute_fu_306.process_U0.tagAB_U0.ap_continue == 1'b1) begin
        trans_out_cnt_0 <= trans_out_cnt_0 + 16'h1;
    end
    else begin
        trans_out_cnt_0 <= trans_out_cnt_0;
    end
end

reg [15:0] trans_in_cnt_1;// for process grp_compute_fu_306.process_U0.systolicArray_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_in_cnt_1 <= 16'h0;
    end
    else if (grp_compute_fu_306.process_U0.systolicArray_U0.start_write == 1'b1) begin
        trans_in_cnt_1 <= trans_in_cnt_1 + 16'h1;
    end
    else begin
        trans_in_cnt_1 <= trans_in_cnt_1;
    end
end

reg [15:0] trans_out_cnt_1;// for process grp_compute_fu_306.process_U0.systolicArray_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_out_cnt_1 <= 16'h0;
    end
    else if (grp_compute_fu_306.process_U0.systolicArray_U0.ap_done == 1'b1 && grp_compute_fu_306.process_U0.systolicArray_U0.ap_continue == 1'b1) begin
        trans_out_cnt_1 <= trans_out_cnt_1 + 16'h1;
    end
    else begin
        trans_out_cnt_1 <= trans_out_cnt_1;
    end
end

reg [15:0] trans_in_cnt_2;// for process grp_compute_fu_306.process_U0.macs19_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_in_cnt_2 <= 16'h0;
    end
    else if (grp_compute_fu_306.process_U0.macs19_U0.start_write == 1'b1) begin
        trans_in_cnt_2 <= trans_in_cnt_2 + 16'h1;
    end
    else begin
        trans_in_cnt_2 <= trans_in_cnt_2;
    end
end

reg [15:0] trans_out_cnt_2;// for process grp_compute_fu_306.process_U0.macs19_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_out_cnt_2 <= 16'h0;
    end
    else if (grp_compute_fu_306.process_U0.macs19_U0.ap_done == 1'b1 && grp_compute_fu_306.process_U0.macs19_U0.ap_continue == 1'b1) begin
        trans_out_cnt_2 <= trans_out_cnt_2 + 16'h1;
    end
    else begin
        trans_out_cnt_2 <= trans_out_cnt_2;
    end
end

reg [15:0] trans_in_cnt_3;// for process grp_compute_fu_306.process_U0.merge_U0.merge17_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_in_cnt_3 <= 16'h0;
    end
    else if (grp_compute_fu_306.process_U0.merge_U0.merge17_U0.start_write == 1'b1) begin
        trans_in_cnt_3 <= trans_in_cnt_3 + 16'h1;
    end
    else begin
        trans_in_cnt_3 <= trans_in_cnt_3;
    end
end

reg [15:0] trans_out_cnt_3;// for process grp_compute_fu_306.process_U0.merge_U0.merge17_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_out_cnt_3 <= 16'h0;
    end
    else if (grp_compute_fu_306.process_U0.merge_U0.merge17_U0.ap_done == 1'b1 && grp_compute_fu_306.process_U0.merge_U0.merge17_U0.ap_continue == 1'b1) begin
        trans_out_cnt_3 <= trans_out_cnt_3 + 16'h1;
    end
    else begin
        trans_out_cnt_3 <= trans_out_cnt_3;
    end
end

reg [15:0] trans_in_cnt_4;// for process grp_compute_fu_306.compute_Block_split36_proc_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_in_cnt_4 <= 16'h0;
    end
    else if (grp_compute_fu_306.compute_Block_split36_proc_U0.start_write == 1'b1) begin
        trans_in_cnt_4 <= trans_in_cnt_4 + 16'h1;
    end
    else begin
        trans_in_cnt_4 <= trans_in_cnt_4;
    end
end

reg [15:0] trans_out_cnt_4;// for process grp_compute_fu_306.compute_Block_split36_proc_U0
always @(negedge dl_reset or posedge dl_clock) begin
    if (~dl_reset) begin
         trans_out_cnt_4 <= 16'h0;
    end
    else if (grp_compute_fu_306.compute_Block_split36_proc_U0.ap_done == 1'b1 && grp_compute_fu_306.compute_Block_split36_proc_U0.ap_continue == 1'b1) begin
        trans_out_cnt_4 <= trans_out_cnt_4 + 16'h1;
    end
    else begin
        trans_out_cnt_4 <= trans_out_cnt_4;
    end
end

    // Process: grp_compute_fu_306.feed_w_U0
    pw_shared_hls_deadlock_detect_unit #(27, 0, 3, 3) pw_shared_hls_deadlock_detect_unit_0 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_0),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_0),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_0),
        .token_in_vec(token_in_vec_0),
        .dl_detect_in(dl_detect_out),
        .origin(origin[0]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_0),
        .out_chan_dep_data(out_chan_dep_data_0),
        .token_out_vec(token_out_vec_0),
        .dl_detect_out(dl_in_vec[0]));

    assign proc_0_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.feed_w_U0.a3_blk_n);
    assign proc_0_data_PIPO_blk[0] = 1'b0;
    assign proc_0_start_FIFO_blk[0] = 1'b0;
    assign proc_0_TLF_FIFO_blk[0] = 1'b0;
    assign proc_0_input_sync_blk[0] = 1'b0;
    assign proc_0_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_0[0] = dl_detect_out ? proc_dep_vld_vec_0_reg[0] : (proc_0_data_FIFO_blk[0] | proc_0_data_PIPO_blk[0] | proc_0_start_FIFO_blk[0] | proc_0_TLF_FIFO_blk[0] | proc_0_input_sync_blk[0] | proc_0_output_sync_blk[0]);
    assign proc_0_data_FIFO_blk[1] = 1'b0;
    assign proc_0_data_PIPO_blk[1] = 1'b0;
    assign proc_0_start_FIFO_blk[1] = 1'b0;
    assign proc_0_TLF_FIFO_blk[1] = 1'b0;
    assign proc_0_input_sync_blk[1] = 1'b0 | (grp_compute_fu_306.ap_sync_feed_w_U0_ap_ready & grp_compute_fu_306.feed_w_U0.ap_idle & ~grp_compute_fu_306.ap_sync_feed_x_U0_ap_ready);
    assign proc_0_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_0[1] = dl_detect_out ? proc_dep_vld_vec_0_reg[1] : (proc_0_data_FIFO_blk[1] | proc_0_data_PIPO_blk[1] | proc_0_start_FIFO_blk[1] | proc_0_TLF_FIFO_blk[1] | proc_0_input_sync_blk[1] | proc_0_output_sync_blk[1]);
    assign proc_0_data_FIFO_blk[2] = 1'b0;
    assign proc_0_data_PIPO_blk[2] = 1'b0;
    assign proc_0_start_FIFO_blk[2] = 1'b0;
    assign proc_0_TLF_FIFO_blk[2] = 1'b0;
    assign proc_0_input_sync_blk[2] = 1'b0 | (grp_compute_fu_306.ap_sync_feed_w_U0_ap_ready & grp_compute_fu_306.feed_w_U0.ap_idle & ~grp_compute_fu_306.ap_sync_compute_Block_split36_proc_U0_ap_ready);
    assign proc_0_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_0[2] = dl_detect_out ? proc_dep_vld_vec_0_reg[2] : (proc_0_data_FIFO_blk[2] | proc_0_data_PIPO_blk[2] | proc_0_start_FIFO_blk[2] | proc_0_TLF_FIFO_blk[2] | proc_0_input_sync_blk[2] | proc_0_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_0_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_0_reg <= proc_dep_vld_vec_0;
        end
    end
    assign in_chan_dep_vld_vec_0[0] = dep_chan_vld_1_0;
    assign in_chan_dep_data_vec_0[26 : 0] = dep_chan_data_1_0;
    assign token_in_vec_0[0] = token_1_0;
    assign in_chan_dep_vld_vec_0[1] = dep_chan_vld_2_0;
    assign in_chan_dep_data_vec_0[53 : 27] = dep_chan_data_2_0;
    assign token_in_vec_0[1] = token_2_0;
    assign in_chan_dep_vld_vec_0[2] = dep_chan_vld_3_0;
    assign in_chan_dep_data_vec_0[80 : 54] = dep_chan_data_3_0;
    assign token_in_vec_0[2] = token_3_0;
    assign dep_chan_vld_0_3 = out_chan_dep_vld_vec_0[0];
    assign dep_chan_data_0_3 = out_chan_dep_data_0;
    assign token_0_3 = token_out_vec_0[0];
    assign dep_chan_vld_0_1 = out_chan_dep_vld_vec_0[1];
    assign dep_chan_data_0_1 = out_chan_dep_data_0;
    assign token_0_1 = token_out_vec_0[1];
    assign dep_chan_vld_0_2 = out_chan_dep_vld_vec_0[2];
    assign dep_chan_data_0_2 = out_chan_dep_data_0;
    assign token_0_2 = token_out_vec_0[2];

    // Process: grp_compute_fu_306.feed_x_U0
    pw_shared_hls_deadlock_detect_unit #(27, 1, 3, 3) pw_shared_hls_deadlock_detect_unit_1 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_1),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_1),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_1),
        .token_in_vec(token_in_vec_1),
        .dl_detect_in(dl_detect_out),
        .origin(origin[1]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_1),
        .out_chan_dep_data(out_chan_dep_data_1),
        .token_out_vec(token_out_vec_1),
        .dl_detect_out(dl_in_vec[1]));

    assign proc_1_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.feed_x_U0.b4_blk_n);
    assign proc_1_data_PIPO_blk[0] = 1'b0;
    assign proc_1_start_FIFO_blk[0] = 1'b0;
    assign proc_1_TLF_FIFO_blk[0] = 1'b0;
    assign proc_1_input_sync_blk[0] = 1'b0;
    assign proc_1_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_1[0] = dl_detect_out ? proc_dep_vld_vec_1_reg[0] : (proc_1_data_FIFO_blk[0] | proc_1_data_PIPO_blk[0] | proc_1_start_FIFO_blk[0] | proc_1_TLF_FIFO_blk[0] | proc_1_input_sync_blk[0] | proc_1_output_sync_blk[0]);
    assign proc_1_data_FIFO_blk[1] = 1'b0;
    assign proc_1_data_PIPO_blk[1] = 1'b0;
    assign proc_1_start_FIFO_blk[1] = 1'b0;
    assign proc_1_TLF_FIFO_blk[1] = 1'b0;
    assign proc_1_input_sync_blk[1] = 1'b0 | (grp_compute_fu_306.ap_sync_feed_x_U0_ap_ready & grp_compute_fu_306.feed_x_U0.ap_idle & ~grp_compute_fu_306.ap_sync_feed_w_U0_ap_ready);
    assign proc_1_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_1[1] = dl_detect_out ? proc_dep_vld_vec_1_reg[1] : (proc_1_data_FIFO_blk[1] | proc_1_data_PIPO_blk[1] | proc_1_start_FIFO_blk[1] | proc_1_TLF_FIFO_blk[1] | proc_1_input_sync_blk[1] | proc_1_output_sync_blk[1]);
    assign proc_1_data_FIFO_blk[2] = 1'b0;
    assign proc_1_data_PIPO_blk[2] = 1'b0;
    assign proc_1_start_FIFO_blk[2] = 1'b0;
    assign proc_1_TLF_FIFO_blk[2] = 1'b0;
    assign proc_1_input_sync_blk[2] = 1'b0 | (grp_compute_fu_306.ap_sync_feed_x_U0_ap_ready & grp_compute_fu_306.feed_x_U0.ap_idle & ~grp_compute_fu_306.ap_sync_compute_Block_split36_proc_U0_ap_ready);
    assign proc_1_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_1[2] = dl_detect_out ? proc_dep_vld_vec_1_reg[2] : (proc_1_data_FIFO_blk[2] | proc_1_data_PIPO_blk[2] | proc_1_start_FIFO_blk[2] | proc_1_TLF_FIFO_blk[2] | proc_1_input_sync_blk[2] | proc_1_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_1_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_1_reg <= proc_dep_vld_vec_1;
        end
    end
    assign in_chan_dep_vld_vec_1[0] = dep_chan_vld_0_1;
    assign in_chan_dep_data_vec_1[26 : 0] = dep_chan_data_0_1;
    assign token_in_vec_1[0] = token_0_1;
    assign in_chan_dep_vld_vec_1[1] = dep_chan_vld_2_1;
    assign in_chan_dep_data_vec_1[53 : 27] = dep_chan_data_2_1;
    assign token_in_vec_1[1] = token_2_1;
    assign in_chan_dep_vld_vec_1[2] = dep_chan_vld_3_1;
    assign in_chan_dep_data_vec_1[80 : 54] = dep_chan_data_3_1;
    assign token_in_vec_1[2] = token_3_1;
    assign dep_chan_vld_1_3 = out_chan_dep_vld_vec_1[0];
    assign dep_chan_data_1_3 = out_chan_dep_data_1;
    assign token_1_3 = token_out_vec_1[0];
    assign dep_chan_vld_1_0 = out_chan_dep_vld_vec_1[1];
    assign dep_chan_data_1_0 = out_chan_dep_data_1;
    assign token_1_0 = token_out_vec_1[1];
    assign dep_chan_vld_1_2 = out_chan_dep_vld_vec_1[2];
    assign dep_chan_data_1_2 = out_chan_dep_data_1;
    assign token_1_2 = token_out_vec_1[2];

    // Process: grp_compute_fu_306.compute_Block_split36_proc_U0
    pw_shared_hls_deadlock_detect_unit #(27, 2, 4, 3) pw_shared_hls_deadlock_detect_unit_2 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_2),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_2),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_2),
        .token_in_vec(token_in_vec_2),
        .dl_detect_in(dl_detect_out),
        .origin(origin[2]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_2),
        .out_chan_dep_data(out_chan_dep_data_2),
        .token_out_vec(token_out_vec_2),
        .dl_detect_out(dl_in_vec[2]));

    assign proc_2_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.compute_Block_split36_proc_U0.np_c_blk_n);
    assign proc_2_data_PIPO_blk[0] = 1'b0;
    assign proc_2_start_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.start_for_collect_U0_U.if_full_n & grp_compute_fu_306.compute_Block_split36_proc_U0.ap_start & ~grp_compute_fu_306.compute_Block_split36_proc_U0.real_start & (trans_in_cnt_4 == trans_out_cnt_4) & ~grp_compute_fu_306.start_for_collect_U0_U.if_read);
    assign proc_2_TLF_FIFO_blk[0] = 1'b0;
    assign proc_2_input_sync_blk[0] = 1'b0;
    assign proc_2_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_2[0] = dl_detect_out ? proc_dep_vld_vec_2_reg[0] : (proc_2_data_FIFO_blk[0] | proc_2_data_PIPO_blk[0] | proc_2_start_FIFO_blk[0] | proc_2_TLF_FIFO_blk[0] | proc_2_input_sync_blk[0] | proc_2_output_sync_blk[0]);
    assign proc_2_data_FIFO_blk[1] = 1'b0;
    assign proc_2_data_PIPO_blk[1] = 1'b0;
    assign proc_2_start_FIFO_blk[1] = 1'b0;
    assign proc_2_TLF_FIFO_blk[1] = 1'b0;
    assign proc_2_input_sync_blk[1] = 1'b0 | (grp_compute_fu_306.ap_sync_compute_Block_split36_proc_U0_ap_ready & grp_compute_fu_306.compute_Block_split36_proc_U0.ap_idle & ~grp_compute_fu_306.ap_sync_feed_w_U0_ap_ready);
    assign proc_2_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_2[1] = dl_detect_out ? proc_dep_vld_vec_2_reg[1] : (proc_2_data_FIFO_blk[1] | proc_2_data_PIPO_blk[1] | proc_2_start_FIFO_blk[1] | proc_2_TLF_FIFO_blk[1] | proc_2_input_sync_blk[1] | proc_2_output_sync_blk[1]);
    assign proc_2_data_FIFO_blk[2] = 1'b0;
    assign proc_2_data_PIPO_blk[2] = 1'b0;
    assign proc_2_start_FIFO_blk[2] = 1'b0;
    assign proc_2_TLF_FIFO_blk[2] = 1'b0;
    assign proc_2_input_sync_blk[2] = 1'b0 | (grp_compute_fu_306.ap_sync_compute_Block_split36_proc_U0_ap_ready & grp_compute_fu_306.compute_Block_split36_proc_U0.ap_idle & ~grp_compute_fu_306.ap_sync_feed_x_U0_ap_ready);
    assign proc_2_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_2[2] = dl_detect_out ? proc_dep_vld_vec_2_reg[2] : (proc_2_data_FIFO_blk[2] | proc_2_data_PIPO_blk[2] | proc_2_start_FIFO_blk[2] | proc_2_TLF_FIFO_blk[2] | proc_2_input_sync_blk[2] | proc_2_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_2_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_2_reg <= proc_dep_vld_vec_2;
        end
    end
    assign in_chan_dep_vld_vec_2[0] = dep_chan_vld_0_2;
    assign in_chan_dep_data_vec_2[26 : 0] = dep_chan_data_0_2;
    assign token_in_vec_2[0] = token_0_2;
    assign in_chan_dep_vld_vec_2[1] = dep_chan_vld_1_2;
    assign in_chan_dep_data_vec_2[53 : 27] = dep_chan_data_1_2;
    assign token_in_vec_2[1] = token_1_2;
    assign in_chan_dep_vld_vec_2[2] = dep_chan_vld_3_2;
    assign in_chan_dep_data_vec_2[80 : 54] = dep_chan_data_3_2;
    assign token_in_vec_2[2] = token_3_2;
    assign in_chan_dep_vld_vec_2[3] = dep_chan_vld_26_2;
    assign in_chan_dep_data_vec_2[107 : 81] = dep_chan_data_26_2;
    assign token_in_vec_2[3] = token_26_2;
    assign dep_chan_vld_2_26 = out_chan_dep_vld_vec_2[0];
    assign dep_chan_data_2_26 = out_chan_dep_data_2;
    assign token_2_26 = token_out_vec_2[0];
    assign dep_chan_vld_2_0 = out_chan_dep_vld_vec_2[1];
    assign dep_chan_data_2_0 = out_chan_dep_data_2;
    assign token_2_0 = token_out_vec_2[1];
    assign dep_chan_vld_2_1 = out_chan_dep_vld_vec_2[2];
    assign dep_chan_data_2_1 = out_chan_dep_data_2;
    assign token_2_1 = token_out_vec_2[2];

    // Process: grp_compute_fu_306.process_U0.tagAB_U0
    pw_shared_hls_deadlock_detect_unit #(27, 3, 6, 7) pw_shared_hls_deadlock_detect_unit_3 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_3),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_3),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_3),
        .token_in_vec(token_in_vec_3),
        .dl_detect_in(dl_detect_out),
        .origin(origin[3]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_3),
        .out_chan_dep_data(out_chan_dep_data_3),
        .token_out_vec(token_out_vec_3),
        .dl_detect_out(dl_in_vec[3]));

    assign proc_3_data_FIFO_blk[0] = 1'b0;
    assign proc_3_data_PIPO_blk[0] = 1'b0;
    assign proc_3_start_FIFO_blk[0] = 1'b0;
    assign proc_3_TLF_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.array_m_blocks_1_channel_U.if_empty_n & grp_compute_fu_306.process_U0.tagAB_U0.ap_idle & ~grp_compute_fu_306.array_m_blocks_1_channel_U.if_write) | (~grp_compute_fu_306.array_m_k_channel_U.if_empty_n & grp_compute_fu_306.process_U0.tagAB_U0.ap_idle & ~grp_compute_fu_306.array_m_k_channel_U.if_write);
    assign proc_3_input_sync_blk[0] = 1'b0;
    assign proc_3_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_3[0] = dl_detect_out ? proc_dep_vld_vec_3_reg[0] : (proc_3_data_FIFO_blk[0] | proc_3_data_PIPO_blk[0] | proc_3_start_FIFO_blk[0] | proc_3_TLF_FIFO_blk[0] | proc_3_input_sync_blk[0] | proc_3_output_sync_blk[0]);
    assign proc_3_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.tagAB_U0.grp_tagAB_Pipeline_VITIS_LOOP_270_1_VITIS_LOOP_271_2_fu_72.a3_blk_n);
    assign proc_3_data_PIPO_blk[1] = 1'b0;
    assign proc_3_start_FIFO_blk[1] = 1'b0;
    assign proc_3_TLF_FIFO_blk[1] = 1'b0;
    assign proc_3_input_sync_blk[1] = 1'b0;
    assign proc_3_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_3[1] = dl_detect_out ? proc_dep_vld_vec_3_reg[1] : (proc_3_data_FIFO_blk[1] | proc_3_data_PIPO_blk[1] | proc_3_start_FIFO_blk[1] | proc_3_TLF_FIFO_blk[1] | proc_3_input_sync_blk[1] | proc_3_output_sync_blk[1]);
    assign proc_3_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.tagAB_U0.grp_tagAB_Pipeline_VITIS_LOOP_270_1_VITIS_LOOP_271_2_fu_72.b4_blk_n);
    assign proc_3_data_PIPO_blk[2] = 1'b0;
    assign proc_3_start_FIFO_blk[2] = 1'b0;
    assign proc_3_TLF_FIFO_blk[2] = 1'b0;
    assign proc_3_input_sync_blk[2] = 1'b0;
    assign proc_3_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_3[2] = dl_detect_out ? proc_dep_vld_vec_3_reg[2] : (proc_3_data_FIFO_blk[2] | proc_3_data_PIPO_blk[2] | proc_3_start_FIFO_blk[2] | proc_3_TLF_FIFO_blk[2] | proc_3_input_sync_blk[2] | proc_3_output_sync_blk[2]);
    assign proc_3_data_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.tagAB_U0.grp_tagAB_Pipeline_VITIS_LOOP_270_1_VITIS_LOOP_271_2_fu_72.l_aStr1_blk_n) | (~grp_compute_fu_306.process_U0.tagAB_U0.grp_tagAB_Pipeline_VITIS_LOOP_270_1_VITIS_LOOP_271_2_fu_72.l_bStr2_blk_n);
    assign proc_3_data_PIPO_blk[3] = 1'b0;
    assign proc_3_start_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_systolicArray_U0_U.if_full_n & grp_compute_fu_306.process_U0.tagAB_U0.ap_start & ~grp_compute_fu_306.process_U0.tagAB_U0.real_start & (trans_in_cnt_0 == trans_out_cnt_0) & ~grp_compute_fu_306.process_U0.start_for_systolicArray_U0_U.if_read);
    assign proc_3_TLF_FIFO_blk[3] = 1'b0;
    assign proc_3_input_sync_blk[3] = 1'b0;
    assign proc_3_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_3[3] = dl_detect_out ? proc_dep_vld_vec_3_reg[3] : (proc_3_data_FIFO_blk[3] | proc_3_data_PIPO_blk[3] | proc_3_start_FIFO_blk[3] | proc_3_TLF_FIFO_blk[3] | proc_3_input_sync_blk[3] | proc_3_output_sync_blk[3]);
    assign proc_3_data_FIFO_blk[4] = 1'b0 | (~grp_compute_fu_306.process_U0.tagAB_U0.this_m_blocks_c24_blk_n);
    assign proc_3_data_PIPO_blk[4] = 1'b0;
    assign proc_3_start_FIFO_blk[4] = 1'b0;
    assign proc_3_TLF_FIFO_blk[4] = 1'b0;
    assign proc_3_input_sync_blk[4] = 1'b0;
    assign proc_3_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_3[4] = dl_detect_out ? proc_dep_vld_vec_3_reg[4] : (proc_3_data_FIFO_blk[4] | proc_3_data_PIPO_blk[4] | proc_3_start_FIFO_blk[4] | proc_3_TLF_FIFO_blk[4] | proc_3_input_sync_blk[4] | proc_3_output_sync_blk[4]);
    assign proc_3_data_FIFO_blk[5] = 1'b0 | (~grp_compute_fu_306.process_U0.tagAB_U0.this_m_k_c27_blk_n);
    assign proc_3_data_PIPO_blk[5] = 1'b0;
    assign proc_3_start_FIFO_blk[5] = 1'b0;
    assign proc_3_TLF_FIFO_blk[5] = 1'b0;
    assign proc_3_input_sync_blk[5] = 1'b0;
    assign proc_3_output_sync_blk[5] = 1'b0;
    assign proc_dep_vld_vec_3[5] = dl_detect_out ? proc_dep_vld_vec_3_reg[5] : (proc_3_data_FIFO_blk[5] | proc_3_data_PIPO_blk[5] | proc_3_start_FIFO_blk[5] | proc_3_TLF_FIFO_blk[5] | proc_3_input_sync_blk[5] | proc_3_output_sync_blk[5]);
    assign proc_3_data_FIFO_blk[6] = 1'b0;
    assign proc_3_data_PIPO_blk[6] = 1'b0;
    assign proc_3_start_FIFO_blk[6] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_macs19_U0_U.if_full_n & grp_compute_fu_306.process_U0.tagAB_U0.ap_start & ~grp_compute_fu_306.process_U0.tagAB_U0.real_start & (trans_in_cnt_0 == trans_out_cnt_0) & ~grp_compute_fu_306.process_U0.start_for_macs19_U0_U.if_read);
    assign proc_3_TLF_FIFO_blk[6] = 1'b0;
    assign proc_3_input_sync_blk[6] = 1'b0;
    assign proc_3_output_sync_blk[6] = 1'b0;
    assign proc_dep_vld_vec_3[6] = dl_detect_out ? proc_dep_vld_vec_3_reg[6] : (proc_3_data_FIFO_blk[6] | proc_3_data_PIPO_blk[6] | proc_3_start_FIFO_blk[6] | proc_3_TLF_FIFO_blk[6] | proc_3_input_sync_blk[6] | proc_3_output_sync_blk[6]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_3_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_3_reg <= proc_dep_vld_vec_3;
        end
    end
    assign in_chan_dep_vld_vec_3[0] = dep_chan_vld_0_3;
    assign in_chan_dep_data_vec_3[26 : 0] = dep_chan_data_0_3;
    assign token_in_vec_3[0] = token_0_3;
    assign in_chan_dep_vld_vec_3[1] = dep_chan_vld_1_3;
    assign in_chan_dep_data_vec_3[53 : 27] = dep_chan_data_1_3;
    assign token_in_vec_3[1] = token_1_3;
    assign in_chan_dep_vld_vec_3[2] = dep_chan_vld_4_3;
    assign in_chan_dep_data_vec_3[80 : 54] = dep_chan_data_4_3;
    assign token_in_vec_3[2] = token_4_3;
    assign in_chan_dep_vld_vec_3[3] = dep_chan_vld_5_3;
    assign in_chan_dep_data_vec_3[107 : 81] = dep_chan_data_5_3;
    assign token_in_vec_3[3] = token_5_3;
    assign in_chan_dep_vld_vec_3[4] = dep_chan_vld_6_3;
    assign in_chan_dep_data_vec_3[134 : 108] = dep_chan_data_6_3;
    assign token_in_vec_3[4] = token_6_3;
    assign in_chan_dep_vld_vec_3[5] = dep_chan_vld_7_3;
    assign in_chan_dep_data_vec_3[161 : 135] = dep_chan_data_7_3;
    assign token_in_vec_3[5] = token_7_3;
    assign dep_chan_vld_3_2 = out_chan_dep_vld_vec_3[0];
    assign dep_chan_data_3_2 = out_chan_dep_data_3;
    assign token_3_2 = token_out_vec_3[0];
    assign dep_chan_vld_3_0 = out_chan_dep_vld_vec_3[1];
    assign dep_chan_data_3_0 = out_chan_dep_data_3;
    assign token_3_0 = token_out_vec_3[1];
    assign dep_chan_vld_3_1 = out_chan_dep_vld_vec_3[2];
    assign dep_chan_data_3_1 = out_chan_dep_data_3;
    assign token_3_1 = token_out_vec_3[2];
    assign dep_chan_vld_3_4 = out_chan_dep_vld_vec_3[3];
    assign dep_chan_data_3_4 = out_chan_dep_data_3;
    assign token_3_4 = token_out_vec_3[3];
    assign dep_chan_vld_3_6 = out_chan_dep_vld_vec_3[4];
    assign dep_chan_data_3_6 = out_chan_dep_data_3;
    assign token_3_6 = token_out_vec_3[4];
    assign dep_chan_vld_3_7 = out_chan_dep_vld_vec_3[5];
    assign dep_chan_data_3_7 = out_chan_dep_data_3;
    assign token_3_7 = token_out_vec_3[5];
    assign dep_chan_vld_3_5 = out_chan_dep_vld_vec_3[6];
    assign dep_chan_data_3_5 = out_chan_dep_data_3;
    assign token_3_5 = token_out_vec_3[6];

    // Process: grp_compute_fu_306.process_U0.systolicArray_U0
    pw_shared_hls_deadlock_detect_unit #(27, 4, 9, 8) pw_shared_hls_deadlock_detect_unit_4 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_4),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_4),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_4),
        .token_in_vec(token_in_vec_4),
        .dl_detect_in(dl_detect_out),
        .origin(origin[4]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_4),
        .out_chan_dep_data(out_chan_dep_data_4),
        .token_out_vec(token_out_vec_4),
        .dl_detect_out(dl_in_vec[4]));

    assign proc_4_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.l_aStr1_blk_n) | (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.l_bStr2_blk_n);
    assign proc_4_data_PIPO_blk[0] = 1'b0;
    assign proc_4_start_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_systolicArray_U0_U.if_empty_n & grp_compute_fu_306.process_U0.systolicArray_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_systolicArray_U0_U.if_write);
    assign proc_4_TLF_FIFO_blk[0] = 1'b0;
    assign proc_4_input_sync_blk[0] = 1'b0;
    assign proc_4_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_4[0] = dl_detect_out ? proc_dep_vld_vec_4_reg[0] : (proc_4_data_FIFO_blk[0] | proc_4_data_PIPO_blk[0] | proc_4_start_FIFO_blk[0] | proc_4_TLF_FIFO_blk[0] | proc_4_input_sync_blk[0] | proc_4_output_sync_blk[0]);
    assign proc_4_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.p_aOut_0_0_0_0_blk_n) | (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.p_bOut_0_0_0_0_blk_n);
    assign proc_4_data_PIPO_blk[1] = 1'b0;
    assign proc_4_start_FIFO_blk[1] = 1'b0;
    assign proc_4_TLF_FIFO_blk[1] = 1'b0;
    assign proc_4_input_sync_blk[1] = 1'b0;
    assign proc_4_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_4[1] = dl_detect_out ? proc_dep_vld_vec_4_reg[1] : (proc_4_data_FIFO_blk[1] | proc_4_data_PIPO_blk[1] | proc_4_start_FIFO_blk[1] | proc_4_TLF_FIFO_blk[1] | proc_4_input_sync_blk[1] | proc_4_output_sync_blk[1]);
    assign proc_4_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.p_aOut_0_0_0_01_blk_n);
    assign proc_4_data_PIPO_blk[2] = 1'b0;
    assign proc_4_start_FIFO_blk[2] = 1'b0;
    assign proc_4_TLF_FIFO_blk[2] = 1'b0;
    assign proc_4_input_sync_blk[2] = 1'b0;
    assign proc_4_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_4[2] = dl_detect_out ? proc_dep_vld_vec_4_reg[2] : (proc_4_data_FIFO_blk[2] | proc_4_data_PIPO_blk[2] | proc_4_start_FIFO_blk[2] | proc_4_TLF_FIFO_blk[2] | proc_4_input_sync_blk[2] | proc_4_output_sync_blk[2]);
    assign proc_4_data_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.p_aOut_0_0_0_02_blk_n);
    assign proc_4_data_PIPO_blk[3] = 1'b0;
    assign proc_4_start_FIFO_blk[3] = 1'b0;
    assign proc_4_TLF_FIFO_blk[3] = 1'b0;
    assign proc_4_input_sync_blk[3] = 1'b0;
    assign proc_4_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_4[3] = dl_detect_out ? proc_dep_vld_vec_4_reg[3] : (proc_4_data_FIFO_blk[3] | proc_4_data_PIPO_blk[3] | proc_4_start_FIFO_blk[3] | proc_4_TLF_FIFO_blk[3] | proc_4_input_sync_blk[3] | proc_4_output_sync_blk[3]);
    assign proc_4_data_FIFO_blk[4] = 1'b0 | (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.p_aOut_0_0_0_03_blk_n);
    assign proc_4_data_PIPO_blk[4] = 1'b0;
    assign proc_4_start_FIFO_blk[4] = 1'b0;
    assign proc_4_TLF_FIFO_blk[4] = 1'b0;
    assign proc_4_input_sync_blk[4] = 1'b0;
    assign proc_4_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_4[4] = dl_detect_out ? proc_dep_vld_vec_4_reg[4] : (proc_4_data_FIFO_blk[4] | proc_4_data_PIPO_blk[4] | proc_4_start_FIFO_blk[4] | proc_4_TLF_FIFO_blk[4] | proc_4_input_sync_blk[4] | proc_4_output_sync_blk[4]);
    assign proc_4_data_FIFO_blk[5] = 1'b0;
    assign proc_4_data_PIPO_blk[5] = 1'b0;
    assign proc_4_start_FIFO_blk[5] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_macs20_U0_U.if_full_n & grp_compute_fu_306.process_U0.systolicArray_U0.ap_start & ~grp_compute_fu_306.process_U0.systolicArray_U0.real_start & (trans_in_cnt_1 == trans_out_cnt_1) & ~grp_compute_fu_306.process_U0.start_for_macs20_U0_U.if_read);
    assign proc_4_TLF_FIFO_blk[5] = 1'b0;
    assign proc_4_input_sync_blk[5] = 1'b0;
    assign proc_4_output_sync_blk[5] = 1'b0;
    assign proc_dep_vld_vec_4[5] = dl_detect_out ? proc_dep_vld_vec_4_reg[5] : (proc_4_data_FIFO_blk[5] | proc_4_data_PIPO_blk[5] | proc_4_start_FIFO_blk[5] | proc_4_TLF_FIFO_blk[5] | proc_4_input_sync_blk[5] | proc_4_output_sync_blk[5]);
    assign proc_4_data_FIFO_blk[6] = 1'b0;
    assign proc_4_data_PIPO_blk[6] = 1'b0;
    assign proc_4_start_FIFO_blk[6] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_macs21_U0_U.if_full_n & grp_compute_fu_306.process_U0.systolicArray_U0.ap_start & ~grp_compute_fu_306.process_U0.systolicArray_U0.real_start & (trans_in_cnt_1 == trans_out_cnt_1) & ~grp_compute_fu_306.process_U0.start_for_macs21_U0_U.if_read);
    assign proc_4_TLF_FIFO_blk[6] = 1'b0;
    assign proc_4_input_sync_blk[6] = 1'b0;
    assign proc_4_output_sync_blk[6] = 1'b0;
    assign proc_dep_vld_vec_4[6] = dl_detect_out ? proc_dep_vld_vec_4_reg[6] : (proc_4_data_FIFO_blk[6] | proc_4_data_PIPO_blk[6] | proc_4_start_FIFO_blk[6] | proc_4_TLF_FIFO_blk[6] | proc_4_input_sync_blk[6] | proc_4_output_sync_blk[6]);
    assign proc_4_data_FIFO_blk[7] = 1'b0;
    assign proc_4_data_PIPO_blk[7] = 1'b0;
    assign proc_4_start_FIFO_blk[7] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_macs_122_U0_U.if_full_n & grp_compute_fu_306.process_U0.systolicArray_U0.ap_start & ~grp_compute_fu_306.process_U0.systolicArray_U0.real_start & (trans_in_cnt_1 == trans_out_cnt_1) & ~grp_compute_fu_306.process_U0.start_for_macs_122_U0_U.if_read);
    assign proc_4_TLF_FIFO_blk[7] = 1'b0;
    assign proc_4_input_sync_blk[7] = 1'b0;
    assign proc_4_output_sync_blk[7] = 1'b0;
    assign proc_dep_vld_vec_4[7] = dl_detect_out ? proc_dep_vld_vec_4_reg[7] : (proc_4_data_FIFO_blk[7] | proc_4_data_PIPO_blk[7] | proc_4_start_FIFO_blk[7] | proc_4_TLF_FIFO_blk[7] | proc_4_input_sync_blk[7] | proc_4_output_sync_blk[7]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_4_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_4_reg <= proc_dep_vld_vec_4;
        end
    end
    assign in_chan_dep_vld_vec_4[0] = dep_chan_vld_3_4;
    assign in_chan_dep_data_vec_4[26 : 0] = dep_chan_data_3_4;
    assign token_in_vec_4[0] = token_3_4;
    assign in_chan_dep_vld_vec_4[1] = dep_chan_vld_5_4;
    assign in_chan_dep_data_vec_4[53 : 27] = dep_chan_data_5_4;
    assign token_in_vec_4[1] = token_5_4;
    assign in_chan_dep_vld_vec_4[2] = dep_chan_vld_7_4;
    assign in_chan_dep_data_vec_4[80 : 54] = dep_chan_data_7_4;
    assign token_in_vec_4[2] = token_7_4;
    assign in_chan_dep_vld_vec_4[3] = dep_chan_vld_9_4;
    assign in_chan_dep_data_vec_4[107 : 81] = dep_chan_data_9_4;
    assign token_in_vec_4[3] = token_9_4;
    assign in_chan_dep_vld_vec_4[4] = dep_chan_vld_11_4;
    assign in_chan_dep_data_vec_4[134 : 108] = dep_chan_data_11_4;
    assign token_in_vec_4[4] = token_11_4;
    assign in_chan_dep_vld_vec_4[5] = dep_chan_vld_13_4;
    assign in_chan_dep_data_vec_4[161 : 135] = dep_chan_data_13_4;
    assign token_in_vec_4[5] = token_13_4;
    assign in_chan_dep_vld_vec_4[6] = dep_chan_vld_15_4;
    assign in_chan_dep_data_vec_4[188 : 162] = dep_chan_data_15_4;
    assign token_in_vec_4[6] = token_15_4;
    assign in_chan_dep_vld_vec_4[7] = dep_chan_vld_17_4;
    assign in_chan_dep_data_vec_4[215 : 189] = dep_chan_data_17_4;
    assign token_in_vec_4[7] = token_17_4;
    assign in_chan_dep_vld_vec_4[8] = dep_chan_vld_19_4;
    assign in_chan_dep_data_vec_4[242 : 216] = dep_chan_data_19_4;
    assign token_in_vec_4[8] = token_19_4;
    assign dep_chan_vld_4_3 = out_chan_dep_vld_vec_4[0];
    assign dep_chan_data_4_3 = out_chan_dep_data_4;
    assign token_4_3 = token_out_vec_4[0];
    assign dep_chan_vld_4_7 = out_chan_dep_vld_vec_4[1];
    assign dep_chan_data_4_7 = out_chan_dep_data_4;
    assign token_4_7 = token_out_vec_4[1];
    assign dep_chan_vld_4_11 = out_chan_dep_vld_vec_4[2];
    assign dep_chan_data_4_11 = out_chan_dep_data_4;
    assign token_4_11 = token_out_vec_4[2];
    assign dep_chan_vld_4_15 = out_chan_dep_vld_vec_4[3];
    assign dep_chan_data_4_15 = out_chan_dep_data_4;
    assign token_4_15 = token_out_vec_4[3];
    assign dep_chan_vld_4_19 = out_chan_dep_vld_vec_4[4];
    assign dep_chan_data_4_19 = out_chan_dep_data_4;
    assign token_4_19 = token_out_vec_4[4];
    assign dep_chan_vld_4_9 = out_chan_dep_vld_vec_4[5];
    assign dep_chan_data_4_9 = out_chan_dep_data_4;
    assign token_4_9 = token_out_vec_4[5];
    assign dep_chan_vld_4_13 = out_chan_dep_vld_vec_4[6];
    assign dep_chan_data_4_13 = out_chan_dep_data_4;
    assign token_4_13 = token_out_vec_4[6];
    assign dep_chan_vld_4_17 = out_chan_dep_vld_vec_4[7];
    assign dep_chan_data_4_17 = out_chan_dep_data_4;
    assign token_4_17 = token_out_vec_4[7];

    // Process: grp_compute_fu_306.process_U0.macs19_U0
    pw_shared_hls_deadlock_detect_unit #(27, 5, 2, 6) pw_shared_hls_deadlock_detect_unit_5 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_5),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_5),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_5),
        .token_in_vec(token_in_vec_5),
        .dl_detect_in(dl_detect_out),
        .origin(origin[5]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_5),
        .out_chan_dep_data(out_chan_dep_data_5),
        .token_out_vec(token_out_vec_5),
        .dl_detect_out(dl_in_vec[5]));

    assign proc_5_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0.this_m_blocks_blk_n) | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.this_m_k_blk_n);
    assign proc_5_data_PIPO_blk[0] = 1'b0;
    assign proc_5_start_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_macs19_U0_U.if_empty_n & grp_compute_fu_306.process_U0.macs19_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_macs19_U0_U.if_write);
    assign proc_5_TLF_FIFO_blk[0] = 1'b0;
    assign proc_5_input_sync_blk[0] = 1'b0;
    assign proc_5_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_5[0] = dl_detect_out ? proc_dep_vld_vec_5_reg[0] : (proc_5_data_FIFO_blk[0] | proc_5_data_PIPO_blk[0] | proc_5_start_FIFO_blk[0] | proc_5_TLF_FIFO_blk[0] | proc_5_input_sync_blk[0] | proc_5_output_sync_blk[0]);
    assign proc_5_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_0_blk_n) | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_0_blk_n);
    assign proc_5_data_PIPO_blk[1] = 1'b0;
    assign proc_5_start_FIFO_blk[1] = 1'b0;
    assign proc_5_TLF_FIFO_blk[1] = 1'b0;
    assign proc_5_input_sync_blk[1] = 1'b0;
    assign proc_5_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_5[1] = dl_detect_out ? proc_dep_vld_vec_5_reg[1] : (proc_5_data_FIFO_blk[1] | proc_5_data_PIPO_blk[1] | proc_5_start_FIFO_blk[1] | proc_5_TLF_FIFO_blk[1] | proc_5_input_sync_blk[1] | proc_5_output_sync_blk[1]);
    assign proc_5_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_07_blk_n) | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.this_m_k_c26_blk_n);
    assign proc_5_data_PIPO_blk[2] = 1'b0;
    assign proc_5_start_FIFO_blk[2] = 1'b0;
    assign proc_5_TLF_FIFO_blk[2] = 1'b0;
    assign proc_5_input_sync_blk[2] = 1'b0;
    assign proc_5_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_5[2] = dl_detect_out ? proc_dep_vld_vec_5_reg[2] : (proc_5_data_FIFO_blk[2] | proc_5_data_PIPO_blk[2] | proc_5_start_FIFO_blk[2] | proc_5_TLF_FIFO_blk[2] | proc_5_input_sync_blk[2] | proc_5_output_sync_blk[2]);
    assign proc_5_data_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.grp_adds1624_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_0_blk_n);
    assign proc_5_data_PIPO_blk[3] = 1'b0;
    assign proc_5_start_FIFO_blk[3] = 1'b0;
    assign proc_5_TLF_FIFO_blk[3] = 1'b0;
    assign proc_5_input_sync_blk[3] = 1'b0;
    assign proc_5_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_5[3] = dl_detect_out ? proc_dep_vld_vec_5_reg[3] : (proc_5_data_FIFO_blk[3] | proc_5_data_PIPO_blk[3] | proc_5_start_FIFO_blk[3] | proc_5_TLF_FIFO_blk[3] | proc_5_input_sync_blk[3] | proc_5_output_sync_blk[3]);
    assign proc_5_data_FIFO_blk[4] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0.this_m_blocks_c23_blk_n);
    assign proc_5_data_PIPO_blk[4] = 1'b0;
    assign proc_5_start_FIFO_blk[4] = 1'b0;
    assign proc_5_TLF_FIFO_blk[4] = 1'b0;
    assign proc_5_input_sync_blk[4] = 1'b0;
    assign proc_5_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_5[4] = dl_detect_out ? proc_dep_vld_vec_5_reg[4] : (proc_5_data_FIFO_blk[4] | proc_5_data_PIPO_blk[4] | proc_5_start_FIFO_blk[4] | proc_5_TLF_FIFO_blk[4] | proc_5_input_sync_blk[4] | proc_5_output_sync_blk[4]);
    assign proc_5_data_FIFO_blk[5] = 1'b0;
    assign proc_5_data_PIPO_blk[5] = 1'b0;
    assign proc_5_start_FIFO_blk[5] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_merge_U0_U.if_full_n & grp_compute_fu_306.process_U0.macs19_U0.ap_start & ~grp_compute_fu_306.process_U0.macs19_U0.real_start & (trans_in_cnt_2 == trans_out_cnt_2) & ~grp_compute_fu_306.process_U0.start_for_merge_U0_U.if_read);
    assign proc_5_TLF_FIFO_blk[5] = 1'b0;
    assign proc_5_input_sync_blk[5] = 1'b0;
    assign proc_5_output_sync_blk[5] = 1'b0;
    assign proc_dep_vld_vec_5[5] = dl_detect_out ? proc_dep_vld_vec_5_reg[5] : (proc_5_data_FIFO_blk[5] | proc_5_data_PIPO_blk[5] | proc_5_start_FIFO_blk[5] | proc_5_TLF_FIFO_blk[5] | proc_5_input_sync_blk[5] | proc_5_output_sync_blk[5]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_5_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_5_reg <= proc_dep_vld_vec_5;
        end
    end
    assign in_chan_dep_vld_vec_5[0] = dep_chan_vld_3_5;
    assign in_chan_dep_data_vec_5[26 : 0] = dep_chan_data_3_5;
    assign token_in_vec_5[0] = token_3_5;
    assign in_chan_dep_vld_vec_5[1] = dep_chan_vld_21_5;
    assign in_chan_dep_data_vec_5[53 : 27] = dep_chan_data_21_5;
    assign token_in_vec_5[1] = token_21_5;
    assign dep_chan_vld_5_3 = out_chan_dep_vld_vec_5[0];
    assign dep_chan_data_5_3 = out_chan_dep_data_5;
    assign token_5_3 = token_out_vec_5[0];
    assign dep_chan_vld_5_4 = out_chan_dep_vld_vec_5[1];
    assign dep_chan_data_5_4 = out_chan_dep_data_5;
    assign token_5_4 = token_out_vec_5[1];
    assign dep_chan_vld_5_11 = out_chan_dep_vld_vec_5[2];
    assign dep_chan_data_5_11 = out_chan_dep_data_5;
    assign token_5_11 = token_out_vec_5[2];
    assign dep_chan_vld_5_23 = out_chan_dep_vld_vec_5[3];
    assign dep_chan_data_5_23 = out_chan_dep_data_5;
    assign token_5_23 = token_out_vec_5[3];
    assign dep_chan_vld_5_10 = out_chan_dep_vld_vec_5[4];
    assign dep_chan_data_5_10 = out_chan_dep_data_5;
    assign token_5_10 = token_out_vec_5[4];
    assign dep_chan_vld_5_21 = out_chan_dep_vld_vec_5[5];
    assign dep_chan_data_5_21 = out_chan_dep_data_5;
    assign token_5_21 = token_out_vec_5[5];

    // Process: grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0
    pw_shared_hls_deadlock_detect_unit #(27, 6, 5, 3) pw_shared_hls_deadlock_detect_unit_6 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_6),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_6),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_6),
        .token_in_vec(token_in_vec_6),
        .dl_detect_in(dl_detect_out),
        .origin(origin[6]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_6),
        .out_chan_dep_data(out_chan_dep_data_6),
        .token_out_vec(token_out_vec_6),
        .dl_detect_out(dl_in_vec[6]));

    assign proc_6_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0.this_m_blocks_blk_n);
    assign proc_6_data_PIPO_blk[0] = 1'b0;
    assign proc_6_start_FIFO_blk[0] = 1'b0;
    assign proc_6_TLF_FIFO_blk[0] = 1'b0;
    assign proc_6_input_sync_blk[0] = 1'b0;
    assign proc_6_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_6[0] = dl_detect_out ? proc_dep_vld_vec_6_reg[0] : (proc_6_data_FIFO_blk[0] | proc_6_data_PIPO_blk[0] | proc_6_start_FIFO_blk[0] | proc_6_TLF_FIFO_blk[0] | proc_6_input_sync_blk[0] | proc_6_output_sync_blk[0]);
    assign proc_6_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0.this_m_blocks_c23_blk_n);
    assign proc_6_data_PIPO_blk[1] = 1'b0;
    assign proc_6_start_FIFO_blk[1] = 1'b0;
    assign proc_6_TLF_FIFO_blk[1] = 1'b0;
    assign proc_6_input_sync_blk[1] = 1'b0;
    assign proc_6_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_6[1] = dl_detect_out ? proc_dep_vld_vec_6_reg[1] : (proc_6_data_FIFO_blk[1] | proc_6_data_PIPO_blk[1] | proc_6_start_FIFO_blk[1] | proc_6_TLF_FIFO_blk[1] | proc_6_input_sync_blk[1] | proc_6_output_sync_blk[1]);
    assign proc_6_data_FIFO_blk[2] = 1'b0;
    assign proc_6_data_PIPO_blk[2] = 1'b0;
    assign proc_6_start_FIFO_blk[2] = 1'b0;
    assign proc_6_TLF_FIFO_blk[2] = 1'b0;
    assign proc_6_input_sync_blk[2] = 1'b0 | (grp_compute_fu_306.process_U0.macs19_U0.ap_sync_entry_proc57_U0_ap_ready & grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs19_U0.ap_sync_muls23_U0_ap_ready);
    assign proc_6_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_6[2] = dl_detect_out ? proc_dep_vld_vec_6_reg[2] : (proc_6_data_FIFO_blk[2] | proc_6_data_PIPO_blk[2] | proc_6_start_FIFO_blk[2] | proc_6_TLF_FIFO_blk[2] | proc_6_input_sync_blk[2] | proc_6_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_6_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_6_reg <= proc_dep_vld_vec_6;
        end
    end
    assign in_chan_dep_vld_vec_6[0] = dep_chan_vld_3_6;
    assign in_chan_dep_data_vec_6[26 : 0] = dep_chan_data_3_6;
    assign token_in_vec_6[0] = token_3_6;
    assign in_chan_dep_vld_vec_6[1] = dep_chan_vld_7_6;
    assign in_chan_dep_data_vec_6[53 : 27] = dep_chan_data_7_6;
    assign token_in_vec_6[1] = token_7_6;
    assign in_chan_dep_vld_vec_6[2] = dep_chan_vld_8_6;
    assign in_chan_dep_data_vec_6[80 : 54] = dep_chan_data_8_6;
    assign token_in_vec_6[2] = token_8_6;
    assign in_chan_dep_vld_vec_6[3] = dep_chan_vld_9_6;
    assign in_chan_dep_data_vec_6[107 : 81] = dep_chan_data_9_6;
    assign token_in_vec_6[3] = token_9_6;
    assign in_chan_dep_vld_vec_6[4] = dep_chan_vld_10_6;
    assign in_chan_dep_data_vec_6[134 : 108] = dep_chan_data_10_6;
    assign token_in_vec_6[4] = token_10_6;
    assign dep_chan_vld_6_3 = out_chan_dep_vld_vec_6[0];
    assign dep_chan_data_6_3 = out_chan_dep_data_6;
    assign token_6_3 = token_out_vec_6[0];
    assign dep_chan_vld_6_10 = out_chan_dep_vld_vec_6[1];
    assign dep_chan_data_6_10 = out_chan_dep_data_6;
    assign token_6_10 = token_out_vec_6[1];
    assign dep_chan_vld_6_7 = out_chan_dep_vld_vec_6[2];
    assign dep_chan_data_6_7 = out_chan_dep_data_6;
    assign token_6_7 = token_out_vec_6[2];

    // Process: grp_compute_fu_306.process_U0.macs19_U0.muls23_U0
    pw_shared_hls_deadlock_detect_unit #(27, 7, 6, 5) pw_shared_hls_deadlock_detect_unit_7 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_7),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_7),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_7),
        .token_in_vec(token_in_vec_7),
        .dl_detect_in(dl_detect_out),
        .origin(origin[7]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_7),
        .out_chan_dep_data(out_chan_dep_data_7),
        .token_out_vec(token_out_vec_7),
        .dl_detect_out(dl_in_vec[7]));

    assign proc_7_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.this_m_k_blk_n);
    assign proc_7_data_PIPO_blk[0] = 1'b0;
    assign proc_7_start_FIFO_blk[0] = 1'b0;
    assign proc_7_TLF_FIFO_blk[0] = 1'b0;
    assign proc_7_input_sync_blk[0] = 1'b0;
    assign proc_7_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_7[0] = dl_detect_out ? proc_dep_vld_vec_7_reg[0] : (proc_7_data_FIFO_blk[0] | proc_7_data_PIPO_blk[0] | proc_7_start_FIFO_blk[0] | proc_7_TLF_FIFO_blk[0] | proc_7_input_sync_blk[0] | proc_7_output_sync_blk[0]);
    assign proc_7_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_0_blk_n) | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_0_blk_n);
    assign proc_7_data_PIPO_blk[1] = 1'b0;
    assign proc_7_start_FIFO_blk[1] = 1'b0;
    assign proc_7_TLF_FIFO_blk[1] = 1'b0;
    assign proc_7_input_sync_blk[1] = 1'b0;
    assign proc_7_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_7[1] = dl_detect_out ? proc_dep_vld_vec_7_reg[1] : (proc_7_data_FIFO_blk[1] | proc_7_data_PIPO_blk[1] | proc_7_start_FIFO_blk[1] | proc_7_TLF_FIFO_blk[1] | proc_7_input_sync_blk[1] | proc_7_output_sync_blk[1]);
    assign proc_7_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_07_blk_n) | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.this_m_k_c26_blk_n);
    assign proc_7_data_PIPO_blk[2] = 1'b0;
    assign proc_7_start_FIFO_blk[2] = 1'b0;
    assign proc_7_TLF_FIFO_blk[2] = 1'b0;
    assign proc_7_input_sync_blk[2] = 1'b0;
    assign proc_7_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_7[2] = dl_detect_out ? proc_dep_vld_vec_7_reg[2] : (proc_7_data_FIFO_blk[2] | proc_7_data_PIPO_blk[2] | proc_7_start_FIFO_blk[2] | proc_7_TLF_FIFO_blk[2] | proc_7_input_sync_blk[2] | proc_7_output_sync_blk[2]);
    assign proc_7_data_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.l_mul1_blk_n) | (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.this_m_k_c_blk_n);
    assign proc_7_data_PIPO_blk[3] = 1'b0;
    assign proc_7_start_FIFO_blk[3] = 1'b0;
    assign proc_7_TLF_FIFO_blk[3] = 1'b0;
    assign proc_7_input_sync_blk[3] = 1'b0;
    assign proc_7_output_sync_blk[3] = 1'b0 | (ap_done_reg_8 & grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.ap_done & ~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.ap_done);
    assign proc_dep_vld_vec_7[3] = dl_detect_out ? proc_dep_vld_vec_7_reg[3] : (proc_7_data_FIFO_blk[3] | proc_7_data_PIPO_blk[3] | proc_7_start_FIFO_blk[3] | proc_7_TLF_FIFO_blk[3] | proc_7_input_sync_blk[3] | proc_7_output_sync_blk[3]);
    assign proc_7_data_FIFO_blk[4] = 1'b0;
    assign proc_7_data_PIPO_blk[4] = 1'b0;
    assign proc_7_start_FIFO_blk[4] = 1'b0;
    assign proc_7_TLF_FIFO_blk[4] = 1'b0;
    assign proc_7_input_sync_blk[4] = 1'b0 | (grp_compute_fu_306.process_U0.macs19_U0.ap_sync_muls23_U0_ap_ready & grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs19_U0.ap_sync_entry_proc57_U0_ap_ready);
    assign proc_7_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_7[4] = dl_detect_out ? proc_dep_vld_vec_7_reg[4] : (proc_7_data_FIFO_blk[4] | proc_7_data_PIPO_blk[4] | proc_7_start_FIFO_blk[4] | proc_7_TLF_FIFO_blk[4] | proc_7_input_sync_blk[4] | proc_7_output_sync_blk[4]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_7_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_7_reg <= proc_dep_vld_vec_7;
        end
    end
    assign in_chan_dep_vld_vec_7[0] = dep_chan_vld_3_7;
    assign in_chan_dep_data_vec_7[26 : 0] = dep_chan_data_3_7;
    assign token_in_vec_7[0] = token_3_7;
    assign in_chan_dep_vld_vec_7[1] = dep_chan_vld_4_7;
    assign in_chan_dep_data_vec_7[53 : 27] = dep_chan_data_4_7;
    assign token_in_vec_7[1] = token_4_7;
    assign in_chan_dep_vld_vec_7[2] = dep_chan_vld_6_7;
    assign in_chan_dep_data_vec_7[80 : 54] = dep_chan_data_6_7;
    assign token_in_vec_7[2] = token_6_7;
    assign in_chan_dep_vld_vec_7[3] = dep_chan_vld_8_7;
    assign in_chan_dep_data_vec_7[107 : 81] = dep_chan_data_8_7;
    assign token_in_vec_7[3] = token_8_7;
    assign in_chan_dep_vld_vec_7[4] = dep_chan_vld_9_7;
    assign in_chan_dep_data_vec_7[134 : 108] = dep_chan_data_9_7;
    assign token_in_vec_7[4] = token_9_7;
    assign in_chan_dep_vld_vec_7[5] = dep_chan_vld_11_7;
    assign in_chan_dep_data_vec_7[161 : 135] = dep_chan_data_11_7;
    assign token_in_vec_7[5] = token_11_7;
    assign dep_chan_vld_7_3 = out_chan_dep_vld_vec_7[0];
    assign dep_chan_data_7_3 = out_chan_dep_data_7;
    assign token_7_3 = token_out_vec_7[0];
    assign dep_chan_vld_7_4 = out_chan_dep_vld_vec_7[1];
    assign dep_chan_data_7_4 = out_chan_dep_data_7;
    assign token_7_4 = token_out_vec_7[1];
    assign dep_chan_vld_7_11 = out_chan_dep_vld_vec_7[2];
    assign dep_chan_data_7_11 = out_chan_dep_data_7;
    assign token_7_11 = token_out_vec_7[2];
    assign dep_chan_vld_7_8 = out_chan_dep_vld_vec_7[3];
    assign dep_chan_data_7_8 = out_chan_dep_data_7;
    assign token_7_8 = token_out_vec_7[3];
    assign dep_chan_vld_7_6 = out_chan_dep_vld_vec_7[4];
    assign dep_chan_data_7_6 = out_chan_dep_data_7;
    assign token_7_6 = token_out_vec_7[4];

    // Process: grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0
    pw_shared_hls_deadlock_detect_unit #(27, 8, 3, 3) pw_shared_hls_deadlock_detect_unit_8 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_8),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_8),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_8),
        .token_in_vec(token_in_vec_8),
        .dl_detect_in(dl_detect_out),
        .origin(origin[8]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_8),
        .out_chan_dep_data(out_chan_dep_data_8),
        .token_out_vec(token_out_vec_8),
        .dl_detect_out(dl_in_vec[8]));

    assign proc_8_data_FIFO_blk[0] = 1'b0;
    assign proc_8_data_PIPO_blk[0] = 1'b0;
    assign proc_8_start_FIFO_blk[0] = 1'b0;
    assign proc_8_TLF_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.this_m_blocks_c_channel_U.if_empty_n & grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs19_U0.this_m_blocks_c_channel_U.if_write);
    assign proc_8_input_sync_blk[0] = 1'b0;
    assign proc_8_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_8[0] = dl_detect_out ? proc_dep_vld_vec_8_reg[0] : (proc_8_data_FIFO_blk[0] | proc_8_data_PIPO_blk[0] | proc_8_start_FIFO_blk[0] | proc_8_TLF_FIFO_blk[0] | proc_8_input_sync_blk[0] | proc_8_output_sync_blk[0]);
    assign proc_8_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.this_m_k_blk_n) | (~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.grp_adds1624_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.l_mul1_blk_n);
    assign proc_8_data_PIPO_blk[1] = 1'b0;
    assign proc_8_start_FIFO_blk[1] = 1'b0;
    assign proc_8_TLF_FIFO_blk[1] = 1'b0;
    assign proc_8_input_sync_blk[1] = 1'b0;
    assign proc_8_output_sync_blk[1] = 1'b0 | (ap_done_reg_2 & grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.ap_done & ~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.ap_done);
    assign proc_dep_vld_vec_8[1] = dl_detect_out ? proc_dep_vld_vec_8_reg[1] : (proc_8_data_FIFO_blk[1] | proc_8_data_PIPO_blk[1] | proc_8_start_FIFO_blk[1] | proc_8_TLF_FIFO_blk[1] | proc_8_input_sync_blk[1] | proc_8_output_sync_blk[1]);
    assign proc_8_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.grp_adds1624_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_0_blk_n);
    assign proc_8_data_PIPO_blk[2] = 1'b0;
    assign proc_8_start_FIFO_blk[2] = 1'b0;
    assign proc_8_TLF_FIFO_blk[2] = 1'b0;
    assign proc_8_input_sync_blk[2] = 1'b0;
    assign proc_8_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_8[2] = dl_detect_out ? proc_dep_vld_vec_8_reg[2] : (proc_8_data_FIFO_blk[2] | proc_8_data_PIPO_blk[2] | proc_8_start_FIFO_blk[2] | proc_8_TLF_FIFO_blk[2] | proc_8_input_sync_blk[2] | proc_8_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_8_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_8_reg <= proc_dep_vld_vec_8;
        end
    end
    assign in_chan_dep_vld_vec_8[0] = dep_chan_vld_7_8;
    assign in_chan_dep_data_vec_8[26 : 0] = dep_chan_data_7_8;
    assign token_in_vec_8[0] = token_7_8;
    assign in_chan_dep_vld_vec_8[1] = dep_chan_vld_21_8;
    assign in_chan_dep_data_vec_8[53 : 27] = dep_chan_data_21_8;
    assign token_in_vec_8[1] = token_21_8;
    assign in_chan_dep_vld_vec_8[2] = dep_chan_vld_23_8;
    assign in_chan_dep_data_vec_8[80 : 54] = dep_chan_data_23_8;
    assign token_in_vec_8[2] = token_23_8;
    assign dep_chan_vld_8_6 = out_chan_dep_vld_vec_8[0];
    assign dep_chan_data_8_6 = out_chan_dep_data_8;
    assign token_8_6 = token_out_vec_8[0];
    assign dep_chan_vld_8_7 = out_chan_dep_vld_vec_8[1];
    assign dep_chan_data_8_7 = out_chan_dep_data_8;
    assign token_8_7 = token_out_vec_8[1];
    assign dep_chan_vld_8_23 = out_chan_dep_vld_vec_8[2];
    assign dep_chan_data_8_23 = out_chan_dep_data_8;
    assign token_8_23 = token_out_vec_8[2];

    // Process: grp_compute_fu_306.process_U0.macs20_U0
    pw_shared_hls_deadlock_detect_unit #(27, 9, 1, 6) pw_shared_hls_deadlock_detect_unit_9 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_9),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_9),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_9),
        .token_in_vec(token_in_vec_9),
        .dl_detect_in(dl_detect_out),
        .origin(origin[9]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_9),
        .out_chan_dep_data(out_chan_dep_data_9),
        .token_out_vec(token_out_vec_9),
        .dl_detect_out(dl_in_vec[9]));

    assign proc_9_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0.this_m_blocks_blk_n);
    assign proc_9_data_PIPO_blk[0] = 1'b0;
    assign proc_9_start_FIFO_blk[0] = 1'b0;
    assign proc_9_TLF_FIFO_blk[0] = 1'b0;
    assign proc_9_input_sync_blk[0] = 1'b0;
    assign proc_9_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_9[0] = dl_detect_out ? proc_dep_vld_vec_9_reg[0] : (proc_9_data_FIFO_blk[0] | proc_9_data_PIPO_blk[0] | proc_9_start_FIFO_blk[0] | proc_9_TLF_FIFO_blk[0] | proc_9_input_sync_blk[0] | proc_9_output_sync_blk[0]);
    assign proc_9_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.this_m_k_blk_n) | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_04_blk_n);
    assign proc_9_data_PIPO_blk[1] = 1'b0;
    assign proc_9_start_FIFO_blk[1] = 1'b0;
    assign proc_9_TLF_FIFO_blk[1] = 1'b0;
    assign proc_9_input_sync_blk[1] = 1'b0;
    assign proc_9_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_9[1] = dl_detect_out ? proc_dep_vld_vec_9_reg[1] : (proc_9_data_FIFO_blk[1] | proc_9_data_PIPO_blk[1] | proc_9_start_FIFO_blk[1] | proc_9_TLF_FIFO_blk[1] | proc_9_input_sync_blk[1] | proc_9_output_sync_blk[1]);
    assign proc_9_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_01_blk_n);
    assign proc_9_data_PIPO_blk[2] = 1'b0;
    assign proc_9_start_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_macs20_U0_U.if_empty_n & grp_compute_fu_306.process_U0.macs20_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_macs20_U0_U.if_write);
    assign proc_9_TLF_FIFO_blk[2] = 1'b0;
    assign proc_9_input_sync_blk[2] = 1'b0;
    assign proc_9_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_9[2] = dl_detect_out ? proc_dep_vld_vec_9_reg[2] : (proc_9_data_FIFO_blk[2] | proc_9_data_PIPO_blk[2] | proc_9_start_FIFO_blk[2] | proc_9_TLF_FIFO_blk[2] | proc_9_input_sync_blk[2] | proc_9_output_sync_blk[2]);
    assign proc_9_data_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_08_blk_n) | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.this_m_k_c25_blk_n);
    assign proc_9_data_PIPO_blk[3] = 1'b0;
    assign proc_9_start_FIFO_blk[3] = 1'b0;
    assign proc_9_TLF_FIFO_blk[3] = 1'b0;
    assign proc_9_input_sync_blk[3] = 1'b0;
    assign proc_9_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_9[3] = dl_detect_out ? proc_dep_vld_vec_9_reg[3] : (proc_9_data_FIFO_blk[3] | proc_9_data_PIPO_blk[3] | proc_9_start_FIFO_blk[3] | proc_9_TLF_FIFO_blk[3] | proc_9_input_sync_blk[3] | proc_9_output_sync_blk[3]);
    assign proc_9_data_FIFO_blk[4] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.grp_adds1626_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_01_blk_n);
    assign proc_9_data_PIPO_blk[4] = 1'b0;
    assign proc_9_start_FIFO_blk[4] = 1'b0;
    assign proc_9_TLF_FIFO_blk[4] = 1'b0;
    assign proc_9_input_sync_blk[4] = 1'b0;
    assign proc_9_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_9[4] = dl_detect_out ? proc_dep_vld_vec_9_reg[4] : (proc_9_data_FIFO_blk[4] | proc_9_data_PIPO_blk[4] | proc_9_start_FIFO_blk[4] | proc_9_TLF_FIFO_blk[4] | proc_9_input_sync_blk[4] | proc_9_output_sync_blk[4]);
    assign proc_9_data_FIFO_blk[5] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0.this_m_blocks_c22_blk_n);
    assign proc_9_data_PIPO_blk[5] = 1'b0;
    assign proc_9_start_FIFO_blk[5] = 1'b0;
    assign proc_9_TLF_FIFO_blk[5] = 1'b0;
    assign proc_9_input_sync_blk[5] = 1'b0;
    assign proc_9_output_sync_blk[5] = 1'b0;
    assign proc_dep_vld_vec_9[5] = dl_detect_out ? proc_dep_vld_vec_9_reg[5] : (proc_9_data_FIFO_blk[5] | proc_9_data_PIPO_blk[5] | proc_9_start_FIFO_blk[5] | proc_9_TLF_FIFO_blk[5] | proc_9_input_sync_blk[5] | proc_9_output_sync_blk[5]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_9_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_9_reg <= proc_dep_vld_vec_9;
        end
    end
    assign in_chan_dep_vld_vec_9[0] = dep_chan_vld_4_9;
    assign in_chan_dep_data_vec_9[26 : 0] = dep_chan_data_4_9;
    assign token_in_vec_9[0] = token_4_9;
    assign dep_chan_vld_9_6 = out_chan_dep_vld_vec_9[0];
    assign dep_chan_data_9_6 = out_chan_dep_data_9;
    assign token_9_6 = token_out_vec_9[0];
    assign dep_chan_vld_9_7 = out_chan_dep_vld_vec_9[1];
    assign dep_chan_data_9_7 = out_chan_dep_data_9;
    assign token_9_7 = token_out_vec_9[1];
    assign dep_chan_vld_9_4 = out_chan_dep_vld_vec_9[2];
    assign dep_chan_data_9_4 = out_chan_dep_data_9;
    assign token_9_4 = token_out_vec_9[2];
    assign dep_chan_vld_9_15 = out_chan_dep_vld_vec_9[3];
    assign dep_chan_data_9_15 = out_chan_dep_data_9;
    assign token_9_15 = token_out_vec_9[3];
    assign dep_chan_vld_9_23 = out_chan_dep_vld_vec_9[4];
    assign dep_chan_data_9_23 = out_chan_dep_data_9;
    assign token_9_23 = token_out_vec_9[4];
    assign dep_chan_vld_9_14 = out_chan_dep_vld_vec_9[5];
    assign dep_chan_data_9_14 = out_chan_dep_data_9;
    assign token_9_14 = token_out_vec_9[5];

    // Process: grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0
    pw_shared_hls_deadlock_detect_unit #(27, 10, 6, 3) pw_shared_hls_deadlock_detect_unit_10 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_10),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_10),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_10),
        .token_in_vec(token_in_vec_10),
        .dl_detect_in(dl_detect_out),
        .origin(origin[10]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_10),
        .out_chan_dep_data(out_chan_dep_data_10),
        .token_out_vec(token_out_vec_10),
        .dl_detect_out(dl_in_vec[10]));

    assign proc_10_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0.this_m_blocks_blk_n);
    assign proc_10_data_PIPO_blk[0] = 1'b0;
    assign proc_10_start_FIFO_blk[0] = 1'b0;
    assign proc_10_TLF_FIFO_blk[0] = 1'b0;
    assign proc_10_input_sync_blk[0] = 1'b0;
    assign proc_10_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_10[0] = dl_detect_out ? proc_dep_vld_vec_10_reg[0] : (proc_10_data_FIFO_blk[0] | proc_10_data_PIPO_blk[0] | proc_10_start_FIFO_blk[0] | proc_10_TLF_FIFO_blk[0] | proc_10_input_sync_blk[0] | proc_10_output_sync_blk[0]);
    assign proc_10_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0.this_m_blocks_c22_blk_n);
    assign proc_10_data_PIPO_blk[1] = 1'b0;
    assign proc_10_start_FIFO_blk[1] = 1'b0;
    assign proc_10_TLF_FIFO_blk[1] = 1'b0;
    assign proc_10_input_sync_blk[1] = 1'b0;
    assign proc_10_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_10[1] = dl_detect_out ? proc_dep_vld_vec_10_reg[1] : (proc_10_data_FIFO_blk[1] | proc_10_data_PIPO_blk[1] | proc_10_start_FIFO_blk[1] | proc_10_TLF_FIFO_blk[1] | proc_10_input_sync_blk[1] | proc_10_output_sync_blk[1]);
    assign proc_10_data_FIFO_blk[2] = 1'b0;
    assign proc_10_data_PIPO_blk[2] = 1'b0;
    assign proc_10_start_FIFO_blk[2] = 1'b0;
    assign proc_10_TLF_FIFO_blk[2] = 1'b0;
    assign proc_10_input_sync_blk[2] = 1'b0 | (grp_compute_fu_306.process_U0.macs20_U0.ap_sync_entry_proc56_U0_ap_ready & grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs20_U0.ap_sync_muls25_U0_ap_ready);
    assign proc_10_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_10[2] = dl_detect_out ? proc_dep_vld_vec_10_reg[2] : (proc_10_data_FIFO_blk[2] | proc_10_data_PIPO_blk[2] | proc_10_start_FIFO_blk[2] | proc_10_TLF_FIFO_blk[2] | proc_10_input_sync_blk[2] | proc_10_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_10_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_10_reg <= proc_dep_vld_vec_10;
        end
    end
    assign in_chan_dep_vld_vec_10[0] = dep_chan_vld_5_10;
    assign in_chan_dep_data_vec_10[26 : 0] = dep_chan_data_5_10;
    assign token_in_vec_10[0] = token_5_10;
    assign in_chan_dep_vld_vec_10[1] = dep_chan_vld_6_10;
    assign in_chan_dep_data_vec_10[53 : 27] = dep_chan_data_6_10;
    assign token_in_vec_10[1] = token_6_10;
    assign in_chan_dep_vld_vec_10[2] = dep_chan_vld_11_10;
    assign in_chan_dep_data_vec_10[80 : 54] = dep_chan_data_11_10;
    assign token_in_vec_10[2] = token_11_10;
    assign in_chan_dep_vld_vec_10[3] = dep_chan_vld_12_10;
    assign in_chan_dep_data_vec_10[107 : 81] = dep_chan_data_12_10;
    assign token_in_vec_10[3] = token_12_10;
    assign in_chan_dep_vld_vec_10[4] = dep_chan_vld_13_10;
    assign in_chan_dep_data_vec_10[134 : 108] = dep_chan_data_13_10;
    assign token_in_vec_10[4] = token_13_10;
    assign in_chan_dep_vld_vec_10[5] = dep_chan_vld_14_10;
    assign in_chan_dep_data_vec_10[161 : 135] = dep_chan_data_14_10;
    assign token_in_vec_10[5] = token_14_10;
    assign dep_chan_vld_10_6 = out_chan_dep_vld_vec_10[0];
    assign dep_chan_data_10_6 = out_chan_dep_data_10;
    assign token_10_6 = token_out_vec_10[0];
    assign dep_chan_vld_10_14 = out_chan_dep_vld_vec_10[1];
    assign dep_chan_data_10_14 = out_chan_dep_data_10;
    assign token_10_14 = token_out_vec_10[1];
    assign dep_chan_vld_10_11 = out_chan_dep_vld_vec_10[2];
    assign dep_chan_data_10_11 = out_chan_dep_data_10;
    assign token_10_11 = token_out_vec_10[2];

    // Process: grp_compute_fu_306.process_U0.macs20_U0.muls25_U0
    pw_shared_hls_deadlock_detect_unit #(27, 11, 7, 5) pw_shared_hls_deadlock_detect_unit_11 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_11),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_11),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_11),
        .token_in_vec(token_in_vec_11),
        .dl_detect_in(dl_detect_out),
        .origin(origin[11]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_11),
        .out_chan_dep_data(out_chan_dep_data_11),
        .token_out_vec(token_out_vec_11),
        .dl_detect_out(dl_in_vec[11]));

    assign proc_11_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.this_m_k_blk_n) | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_04_blk_n);
    assign proc_11_data_PIPO_blk[0] = 1'b0;
    assign proc_11_start_FIFO_blk[0] = 1'b0;
    assign proc_11_TLF_FIFO_blk[0] = 1'b0;
    assign proc_11_input_sync_blk[0] = 1'b0;
    assign proc_11_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_11[0] = dl_detect_out ? proc_dep_vld_vec_11_reg[0] : (proc_11_data_FIFO_blk[0] | proc_11_data_PIPO_blk[0] | proc_11_start_FIFO_blk[0] | proc_11_TLF_FIFO_blk[0] | proc_11_input_sync_blk[0] | proc_11_output_sync_blk[0]);
    assign proc_11_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_01_blk_n);
    assign proc_11_data_PIPO_blk[1] = 1'b0;
    assign proc_11_start_FIFO_blk[1] = 1'b0;
    assign proc_11_TLF_FIFO_blk[1] = 1'b0;
    assign proc_11_input_sync_blk[1] = 1'b0;
    assign proc_11_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_11[1] = dl_detect_out ? proc_dep_vld_vec_11_reg[1] : (proc_11_data_FIFO_blk[1] | proc_11_data_PIPO_blk[1] | proc_11_start_FIFO_blk[1] | proc_11_TLF_FIFO_blk[1] | proc_11_input_sync_blk[1] | proc_11_output_sync_blk[1]);
    assign proc_11_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_08_blk_n) | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.this_m_k_c25_blk_n);
    assign proc_11_data_PIPO_blk[2] = 1'b0;
    assign proc_11_start_FIFO_blk[2] = 1'b0;
    assign proc_11_TLF_FIFO_blk[2] = 1'b0;
    assign proc_11_input_sync_blk[2] = 1'b0;
    assign proc_11_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_11[2] = dl_detect_out ? proc_dep_vld_vec_11_reg[2] : (proc_11_data_FIFO_blk[2] | proc_11_data_PIPO_blk[2] | proc_11_start_FIFO_blk[2] | proc_11_TLF_FIFO_blk[2] | proc_11_input_sync_blk[2] | proc_11_output_sync_blk[2]);
    assign proc_11_data_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.l_mul1_blk_n) | (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.this_m_k_c_blk_n);
    assign proc_11_data_PIPO_blk[3] = 1'b0;
    assign proc_11_start_FIFO_blk[3] = 1'b0;
    assign proc_11_TLF_FIFO_blk[3] = 1'b0;
    assign proc_11_input_sync_blk[3] = 1'b0;
    assign proc_11_output_sync_blk[3] = 1'b0 | (ap_done_reg_9 & grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.ap_done & ~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.ap_done);
    assign proc_dep_vld_vec_11[3] = dl_detect_out ? proc_dep_vld_vec_11_reg[3] : (proc_11_data_FIFO_blk[3] | proc_11_data_PIPO_blk[3] | proc_11_start_FIFO_blk[3] | proc_11_TLF_FIFO_blk[3] | proc_11_input_sync_blk[3] | proc_11_output_sync_blk[3]);
    assign proc_11_data_FIFO_blk[4] = 1'b0;
    assign proc_11_data_PIPO_blk[4] = 1'b0;
    assign proc_11_start_FIFO_blk[4] = 1'b0;
    assign proc_11_TLF_FIFO_blk[4] = 1'b0;
    assign proc_11_input_sync_blk[4] = 1'b0 | (grp_compute_fu_306.process_U0.macs20_U0.ap_sync_muls25_U0_ap_ready & grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs20_U0.ap_sync_entry_proc56_U0_ap_ready);
    assign proc_11_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_11[4] = dl_detect_out ? proc_dep_vld_vec_11_reg[4] : (proc_11_data_FIFO_blk[4] | proc_11_data_PIPO_blk[4] | proc_11_start_FIFO_blk[4] | proc_11_TLF_FIFO_blk[4] | proc_11_input_sync_blk[4] | proc_11_output_sync_blk[4]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_11_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_11_reg <= proc_dep_vld_vec_11;
        end
    end
    assign in_chan_dep_vld_vec_11[0] = dep_chan_vld_4_11;
    assign in_chan_dep_data_vec_11[26 : 0] = dep_chan_data_4_11;
    assign token_in_vec_11[0] = token_4_11;
    assign in_chan_dep_vld_vec_11[1] = dep_chan_vld_5_11;
    assign in_chan_dep_data_vec_11[53 : 27] = dep_chan_data_5_11;
    assign token_in_vec_11[1] = token_5_11;
    assign in_chan_dep_vld_vec_11[2] = dep_chan_vld_7_11;
    assign in_chan_dep_data_vec_11[80 : 54] = dep_chan_data_7_11;
    assign token_in_vec_11[2] = token_7_11;
    assign in_chan_dep_vld_vec_11[3] = dep_chan_vld_10_11;
    assign in_chan_dep_data_vec_11[107 : 81] = dep_chan_data_10_11;
    assign token_in_vec_11[3] = token_10_11;
    assign in_chan_dep_vld_vec_11[4] = dep_chan_vld_12_11;
    assign in_chan_dep_data_vec_11[134 : 108] = dep_chan_data_12_11;
    assign token_in_vec_11[4] = token_12_11;
    assign in_chan_dep_vld_vec_11[5] = dep_chan_vld_13_11;
    assign in_chan_dep_data_vec_11[161 : 135] = dep_chan_data_13_11;
    assign token_in_vec_11[5] = token_13_11;
    assign in_chan_dep_vld_vec_11[6] = dep_chan_vld_15_11;
    assign in_chan_dep_data_vec_11[188 : 162] = dep_chan_data_15_11;
    assign token_in_vec_11[6] = token_15_11;
    assign dep_chan_vld_11_7 = out_chan_dep_vld_vec_11[0];
    assign dep_chan_data_11_7 = out_chan_dep_data_11;
    assign token_11_7 = token_out_vec_11[0];
    assign dep_chan_vld_11_4 = out_chan_dep_vld_vec_11[1];
    assign dep_chan_data_11_4 = out_chan_dep_data_11;
    assign token_11_4 = token_out_vec_11[1];
    assign dep_chan_vld_11_15 = out_chan_dep_vld_vec_11[2];
    assign dep_chan_data_11_15 = out_chan_dep_data_11;
    assign token_11_15 = token_out_vec_11[2];
    assign dep_chan_vld_11_12 = out_chan_dep_vld_vec_11[3];
    assign dep_chan_data_11_12 = out_chan_dep_data_11;
    assign token_11_12 = token_out_vec_11[3];
    assign dep_chan_vld_11_10 = out_chan_dep_vld_vec_11[4];
    assign dep_chan_data_11_10 = out_chan_dep_data_11;
    assign token_11_10 = token_out_vec_11[4];

    // Process: grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0
    pw_shared_hls_deadlock_detect_unit #(27, 12, 3, 3) pw_shared_hls_deadlock_detect_unit_12 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_12),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_12),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_12),
        .token_in_vec(token_in_vec_12),
        .dl_detect_in(dl_detect_out),
        .origin(origin[12]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_12),
        .out_chan_dep_data(out_chan_dep_data_12),
        .token_out_vec(token_out_vec_12),
        .dl_detect_out(dl_in_vec[12]));

    assign proc_12_data_FIFO_blk[0] = 1'b0;
    assign proc_12_data_PIPO_blk[0] = 1'b0;
    assign proc_12_start_FIFO_blk[0] = 1'b0;
    assign proc_12_TLF_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.this_m_blocks_c_channel_U.if_empty_n & grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs20_U0.this_m_blocks_c_channel_U.if_write);
    assign proc_12_input_sync_blk[0] = 1'b0;
    assign proc_12_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_12[0] = dl_detect_out ? proc_dep_vld_vec_12_reg[0] : (proc_12_data_FIFO_blk[0] | proc_12_data_PIPO_blk[0] | proc_12_start_FIFO_blk[0] | proc_12_TLF_FIFO_blk[0] | proc_12_input_sync_blk[0] | proc_12_output_sync_blk[0]);
    assign proc_12_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.this_m_k_blk_n) | (~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.grp_adds1626_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.l_mul1_blk_n);
    assign proc_12_data_PIPO_blk[1] = 1'b0;
    assign proc_12_start_FIFO_blk[1] = 1'b0;
    assign proc_12_TLF_FIFO_blk[1] = 1'b0;
    assign proc_12_input_sync_blk[1] = 1'b0;
    assign proc_12_output_sync_blk[1] = 1'b0 | (ap_done_reg_3 & grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.ap_done & ~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.ap_done);
    assign proc_dep_vld_vec_12[1] = dl_detect_out ? proc_dep_vld_vec_12_reg[1] : (proc_12_data_FIFO_blk[1] | proc_12_data_PIPO_blk[1] | proc_12_start_FIFO_blk[1] | proc_12_TLF_FIFO_blk[1] | proc_12_input_sync_blk[1] | proc_12_output_sync_blk[1]);
    assign proc_12_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.grp_adds1626_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_01_blk_n);
    assign proc_12_data_PIPO_blk[2] = 1'b0;
    assign proc_12_start_FIFO_blk[2] = 1'b0;
    assign proc_12_TLF_FIFO_blk[2] = 1'b0;
    assign proc_12_input_sync_blk[2] = 1'b0;
    assign proc_12_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_12[2] = dl_detect_out ? proc_dep_vld_vec_12_reg[2] : (proc_12_data_FIFO_blk[2] | proc_12_data_PIPO_blk[2] | proc_12_start_FIFO_blk[2] | proc_12_TLF_FIFO_blk[2] | proc_12_input_sync_blk[2] | proc_12_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_12_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_12_reg <= proc_dep_vld_vec_12;
        end
    end
    assign in_chan_dep_vld_vec_12[0] = dep_chan_vld_11_12;
    assign in_chan_dep_data_vec_12[26 : 0] = dep_chan_data_11_12;
    assign token_in_vec_12[0] = token_11_12;
    assign in_chan_dep_vld_vec_12[1] = dep_chan_vld_21_12;
    assign in_chan_dep_data_vec_12[53 : 27] = dep_chan_data_21_12;
    assign token_in_vec_12[1] = token_21_12;
    assign in_chan_dep_vld_vec_12[2] = dep_chan_vld_23_12;
    assign in_chan_dep_data_vec_12[80 : 54] = dep_chan_data_23_12;
    assign token_in_vec_12[2] = token_23_12;
    assign dep_chan_vld_12_10 = out_chan_dep_vld_vec_12[0];
    assign dep_chan_data_12_10 = out_chan_dep_data_12;
    assign token_12_10 = token_out_vec_12[0];
    assign dep_chan_vld_12_11 = out_chan_dep_vld_vec_12[1];
    assign dep_chan_data_12_11 = out_chan_dep_data_12;
    assign token_12_11 = token_out_vec_12[1];
    assign dep_chan_vld_12_23 = out_chan_dep_vld_vec_12[2];
    assign dep_chan_data_12_23 = out_chan_dep_data_12;
    assign token_12_23 = token_out_vec_12[2];

    // Process: grp_compute_fu_306.process_U0.macs21_U0
    pw_shared_hls_deadlock_detect_unit #(27, 13, 1, 6) pw_shared_hls_deadlock_detect_unit_13 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_13),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_13),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_13),
        .token_in_vec(token_in_vec_13),
        .dl_detect_in(dl_detect_out),
        .origin(origin[13]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_13),
        .out_chan_dep_data(out_chan_dep_data_13),
        .token_out_vec(token_out_vec_13),
        .dl_detect_out(dl_in_vec[13]));

    assign proc_13_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0.this_m_blocks_blk_n);
    assign proc_13_data_PIPO_blk[0] = 1'b0;
    assign proc_13_start_FIFO_blk[0] = 1'b0;
    assign proc_13_TLF_FIFO_blk[0] = 1'b0;
    assign proc_13_input_sync_blk[0] = 1'b0;
    assign proc_13_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_13[0] = dl_detect_out ? proc_dep_vld_vec_13_reg[0] : (proc_13_data_FIFO_blk[0] | proc_13_data_PIPO_blk[0] | proc_13_start_FIFO_blk[0] | proc_13_TLF_FIFO_blk[0] | proc_13_input_sync_blk[0] | proc_13_output_sync_blk[0]);
    assign proc_13_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.this_m_k_blk_n) | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_05_blk_n);
    assign proc_13_data_PIPO_blk[1] = 1'b0;
    assign proc_13_start_FIFO_blk[1] = 1'b0;
    assign proc_13_TLF_FIFO_blk[1] = 1'b0;
    assign proc_13_input_sync_blk[1] = 1'b0;
    assign proc_13_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_13[1] = dl_detect_out ? proc_dep_vld_vec_13_reg[1] : (proc_13_data_FIFO_blk[1] | proc_13_data_PIPO_blk[1] | proc_13_start_FIFO_blk[1] | proc_13_TLF_FIFO_blk[1] | proc_13_input_sync_blk[1] | proc_13_output_sync_blk[1]);
    assign proc_13_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_02_blk_n);
    assign proc_13_data_PIPO_blk[2] = 1'b0;
    assign proc_13_start_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_macs21_U0_U.if_empty_n & grp_compute_fu_306.process_U0.macs21_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_macs21_U0_U.if_write);
    assign proc_13_TLF_FIFO_blk[2] = 1'b0;
    assign proc_13_input_sync_blk[2] = 1'b0;
    assign proc_13_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_13[2] = dl_detect_out ? proc_dep_vld_vec_13_reg[2] : (proc_13_data_FIFO_blk[2] | proc_13_data_PIPO_blk[2] | proc_13_start_FIFO_blk[2] | proc_13_TLF_FIFO_blk[2] | proc_13_input_sync_blk[2] | proc_13_output_sync_blk[2]);
    assign proc_13_data_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_09_blk_n) | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.this_m_k_c1_blk_n);
    assign proc_13_data_PIPO_blk[3] = 1'b0;
    assign proc_13_start_FIFO_blk[3] = 1'b0;
    assign proc_13_TLF_FIFO_blk[3] = 1'b0;
    assign proc_13_input_sync_blk[3] = 1'b0;
    assign proc_13_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_13[3] = dl_detect_out ? proc_dep_vld_vec_13_reg[3] : (proc_13_data_FIFO_blk[3] | proc_13_data_PIPO_blk[3] | proc_13_start_FIFO_blk[3] | proc_13_TLF_FIFO_blk[3] | proc_13_input_sync_blk[3] | proc_13_output_sync_blk[3]);
    assign proc_13_data_FIFO_blk[4] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.grp_adds1628_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_02_blk_n);
    assign proc_13_data_PIPO_blk[4] = 1'b0;
    assign proc_13_start_FIFO_blk[4] = 1'b0;
    assign proc_13_TLF_FIFO_blk[4] = 1'b0;
    assign proc_13_input_sync_blk[4] = 1'b0;
    assign proc_13_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_13[4] = dl_detect_out ? proc_dep_vld_vec_13_reg[4] : (proc_13_data_FIFO_blk[4] | proc_13_data_PIPO_blk[4] | proc_13_start_FIFO_blk[4] | proc_13_TLF_FIFO_blk[4] | proc_13_input_sync_blk[4] | proc_13_output_sync_blk[4]);
    assign proc_13_data_FIFO_blk[5] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0.this_m_blocks_c21_blk_n);
    assign proc_13_data_PIPO_blk[5] = 1'b0;
    assign proc_13_start_FIFO_blk[5] = 1'b0;
    assign proc_13_TLF_FIFO_blk[5] = 1'b0;
    assign proc_13_input_sync_blk[5] = 1'b0;
    assign proc_13_output_sync_blk[5] = 1'b0;
    assign proc_dep_vld_vec_13[5] = dl_detect_out ? proc_dep_vld_vec_13_reg[5] : (proc_13_data_FIFO_blk[5] | proc_13_data_PIPO_blk[5] | proc_13_start_FIFO_blk[5] | proc_13_TLF_FIFO_blk[5] | proc_13_input_sync_blk[5] | proc_13_output_sync_blk[5]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_13_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_13_reg <= proc_dep_vld_vec_13;
        end
    end
    assign in_chan_dep_vld_vec_13[0] = dep_chan_vld_4_13;
    assign in_chan_dep_data_vec_13[26 : 0] = dep_chan_data_4_13;
    assign token_in_vec_13[0] = token_4_13;
    assign dep_chan_vld_13_10 = out_chan_dep_vld_vec_13[0];
    assign dep_chan_data_13_10 = out_chan_dep_data_13;
    assign token_13_10 = token_out_vec_13[0];
    assign dep_chan_vld_13_11 = out_chan_dep_vld_vec_13[1];
    assign dep_chan_data_13_11 = out_chan_dep_data_13;
    assign token_13_11 = token_out_vec_13[1];
    assign dep_chan_vld_13_4 = out_chan_dep_vld_vec_13[2];
    assign dep_chan_data_13_4 = out_chan_dep_data_13;
    assign token_13_4 = token_out_vec_13[2];
    assign dep_chan_vld_13_19 = out_chan_dep_vld_vec_13[3];
    assign dep_chan_data_13_19 = out_chan_dep_data_13;
    assign token_13_19 = token_out_vec_13[3];
    assign dep_chan_vld_13_24 = out_chan_dep_vld_vec_13[4];
    assign dep_chan_data_13_24 = out_chan_dep_data_13;
    assign token_13_24 = token_out_vec_13[4];
    assign dep_chan_vld_13_18 = out_chan_dep_vld_vec_13[5];
    assign dep_chan_data_13_18 = out_chan_dep_data_13;
    assign token_13_18 = token_out_vec_13[5];

    // Process: grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0
    pw_shared_hls_deadlock_detect_unit #(27, 14, 6, 3) pw_shared_hls_deadlock_detect_unit_14 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_14),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_14),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_14),
        .token_in_vec(token_in_vec_14),
        .dl_detect_in(dl_detect_out),
        .origin(origin[14]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_14),
        .out_chan_dep_data(out_chan_dep_data_14),
        .token_out_vec(token_out_vec_14),
        .dl_detect_out(dl_in_vec[14]));

    assign proc_14_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0.this_m_blocks_blk_n);
    assign proc_14_data_PIPO_blk[0] = 1'b0;
    assign proc_14_start_FIFO_blk[0] = 1'b0;
    assign proc_14_TLF_FIFO_blk[0] = 1'b0;
    assign proc_14_input_sync_blk[0] = 1'b0;
    assign proc_14_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_14[0] = dl_detect_out ? proc_dep_vld_vec_14_reg[0] : (proc_14_data_FIFO_blk[0] | proc_14_data_PIPO_blk[0] | proc_14_start_FIFO_blk[0] | proc_14_TLF_FIFO_blk[0] | proc_14_input_sync_blk[0] | proc_14_output_sync_blk[0]);
    assign proc_14_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0.this_m_blocks_c21_blk_n);
    assign proc_14_data_PIPO_blk[1] = 1'b0;
    assign proc_14_start_FIFO_blk[1] = 1'b0;
    assign proc_14_TLF_FIFO_blk[1] = 1'b0;
    assign proc_14_input_sync_blk[1] = 1'b0;
    assign proc_14_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_14[1] = dl_detect_out ? proc_dep_vld_vec_14_reg[1] : (proc_14_data_FIFO_blk[1] | proc_14_data_PIPO_blk[1] | proc_14_start_FIFO_blk[1] | proc_14_TLF_FIFO_blk[1] | proc_14_input_sync_blk[1] | proc_14_output_sync_blk[1]);
    assign proc_14_data_FIFO_blk[2] = 1'b0;
    assign proc_14_data_PIPO_blk[2] = 1'b0;
    assign proc_14_start_FIFO_blk[2] = 1'b0;
    assign proc_14_TLF_FIFO_blk[2] = 1'b0;
    assign proc_14_input_sync_blk[2] = 1'b0 | (grp_compute_fu_306.process_U0.macs21_U0.ap_sync_entry_proc_U0_ap_ready & grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs21_U0.ap_sync_muls27_U0_ap_ready);
    assign proc_14_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_14[2] = dl_detect_out ? proc_dep_vld_vec_14_reg[2] : (proc_14_data_FIFO_blk[2] | proc_14_data_PIPO_blk[2] | proc_14_start_FIFO_blk[2] | proc_14_TLF_FIFO_blk[2] | proc_14_input_sync_blk[2] | proc_14_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_14_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_14_reg <= proc_dep_vld_vec_14;
        end
    end
    assign in_chan_dep_vld_vec_14[0] = dep_chan_vld_9_14;
    assign in_chan_dep_data_vec_14[26 : 0] = dep_chan_data_9_14;
    assign token_in_vec_14[0] = token_9_14;
    assign in_chan_dep_vld_vec_14[1] = dep_chan_vld_10_14;
    assign in_chan_dep_data_vec_14[53 : 27] = dep_chan_data_10_14;
    assign token_in_vec_14[1] = token_10_14;
    assign in_chan_dep_vld_vec_14[2] = dep_chan_vld_15_14;
    assign in_chan_dep_data_vec_14[80 : 54] = dep_chan_data_15_14;
    assign token_in_vec_14[2] = token_15_14;
    assign in_chan_dep_vld_vec_14[3] = dep_chan_vld_16_14;
    assign in_chan_dep_data_vec_14[107 : 81] = dep_chan_data_16_14;
    assign token_in_vec_14[3] = token_16_14;
    assign in_chan_dep_vld_vec_14[4] = dep_chan_vld_17_14;
    assign in_chan_dep_data_vec_14[134 : 108] = dep_chan_data_17_14;
    assign token_in_vec_14[4] = token_17_14;
    assign in_chan_dep_vld_vec_14[5] = dep_chan_vld_18_14;
    assign in_chan_dep_data_vec_14[161 : 135] = dep_chan_data_18_14;
    assign token_in_vec_14[5] = token_18_14;
    assign dep_chan_vld_14_10 = out_chan_dep_vld_vec_14[0];
    assign dep_chan_data_14_10 = out_chan_dep_data_14;
    assign token_14_10 = token_out_vec_14[0];
    assign dep_chan_vld_14_18 = out_chan_dep_vld_vec_14[1];
    assign dep_chan_data_14_18 = out_chan_dep_data_14;
    assign token_14_18 = token_out_vec_14[1];
    assign dep_chan_vld_14_15 = out_chan_dep_vld_vec_14[2];
    assign dep_chan_data_14_15 = out_chan_dep_data_14;
    assign token_14_15 = token_out_vec_14[2];

    // Process: grp_compute_fu_306.process_U0.macs21_U0.muls27_U0
    pw_shared_hls_deadlock_detect_unit #(27, 15, 7, 5) pw_shared_hls_deadlock_detect_unit_15 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_15),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_15),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_15),
        .token_in_vec(token_in_vec_15),
        .dl_detect_in(dl_detect_out),
        .origin(origin[15]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_15),
        .out_chan_dep_data(out_chan_dep_data_15),
        .token_out_vec(token_out_vec_15),
        .dl_detect_out(dl_in_vec[15]));

    assign proc_15_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.this_m_k_blk_n) | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_05_blk_n);
    assign proc_15_data_PIPO_blk[0] = 1'b0;
    assign proc_15_start_FIFO_blk[0] = 1'b0;
    assign proc_15_TLF_FIFO_blk[0] = 1'b0;
    assign proc_15_input_sync_blk[0] = 1'b0;
    assign proc_15_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_15[0] = dl_detect_out ? proc_dep_vld_vec_15_reg[0] : (proc_15_data_FIFO_blk[0] | proc_15_data_PIPO_blk[0] | proc_15_start_FIFO_blk[0] | proc_15_TLF_FIFO_blk[0] | proc_15_input_sync_blk[0] | proc_15_output_sync_blk[0]);
    assign proc_15_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_02_blk_n);
    assign proc_15_data_PIPO_blk[1] = 1'b0;
    assign proc_15_start_FIFO_blk[1] = 1'b0;
    assign proc_15_TLF_FIFO_blk[1] = 1'b0;
    assign proc_15_input_sync_blk[1] = 1'b0;
    assign proc_15_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_15[1] = dl_detect_out ? proc_dep_vld_vec_15_reg[1] : (proc_15_data_FIFO_blk[1] | proc_15_data_PIPO_blk[1] | proc_15_start_FIFO_blk[1] | proc_15_TLF_FIFO_blk[1] | proc_15_input_sync_blk[1] | proc_15_output_sync_blk[1]);
    assign proc_15_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_09_blk_n) | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.this_m_k_c1_blk_n);
    assign proc_15_data_PIPO_blk[2] = 1'b0;
    assign proc_15_start_FIFO_blk[2] = 1'b0;
    assign proc_15_TLF_FIFO_blk[2] = 1'b0;
    assign proc_15_input_sync_blk[2] = 1'b0;
    assign proc_15_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_15[2] = dl_detect_out ? proc_dep_vld_vec_15_reg[2] : (proc_15_data_FIFO_blk[2] | proc_15_data_PIPO_blk[2] | proc_15_start_FIFO_blk[2] | proc_15_TLF_FIFO_blk[2] | proc_15_input_sync_blk[2] | proc_15_output_sync_blk[2]);
    assign proc_15_data_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.l_mul1_blk_n) | (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.this_m_k_c_blk_n);
    assign proc_15_data_PIPO_blk[3] = 1'b0;
    assign proc_15_start_FIFO_blk[3] = 1'b0;
    assign proc_15_TLF_FIFO_blk[3] = 1'b0;
    assign proc_15_input_sync_blk[3] = 1'b0;
    assign proc_15_output_sync_blk[3] = 1'b0 | (ap_done_reg_10 & grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.ap_done & ~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.ap_done);
    assign proc_dep_vld_vec_15[3] = dl_detect_out ? proc_dep_vld_vec_15_reg[3] : (proc_15_data_FIFO_blk[3] | proc_15_data_PIPO_blk[3] | proc_15_start_FIFO_blk[3] | proc_15_TLF_FIFO_blk[3] | proc_15_input_sync_blk[3] | proc_15_output_sync_blk[3]);
    assign proc_15_data_FIFO_blk[4] = 1'b0;
    assign proc_15_data_PIPO_blk[4] = 1'b0;
    assign proc_15_start_FIFO_blk[4] = 1'b0;
    assign proc_15_TLF_FIFO_blk[4] = 1'b0;
    assign proc_15_input_sync_blk[4] = 1'b0 | (grp_compute_fu_306.process_U0.macs21_U0.ap_sync_muls27_U0_ap_ready & grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs21_U0.ap_sync_entry_proc_U0_ap_ready);
    assign proc_15_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_15[4] = dl_detect_out ? proc_dep_vld_vec_15_reg[4] : (proc_15_data_FIFO_blk[4] | proc_15_data_PIPO_blk[4] | proc_15_start_FIFO_blk[4] | proc_15_TLF_FIFO_blk[4] | proc_15_input_sync_blk[4] | proc_15_output_sync_blk[4]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_15_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_15_reg <= proc_dep_vld_vec_15;
        end
    end
    assign in_chan_dep_vld_vec_15[0] = dep_chan_vld_4_15;
    assign in_chan_dep_data_vec_15[26 : 0] = dep_chan_data_4_15;
    assign token_in_vec_15[0] = token_4_15;
    assign in_chan_dep_vld_vec_15[1] = dep_chan_vld_9_15;
    assign in_chan_dep_data_vec_15[53 : 27] = dep_chan_data_9_15;
    assign token_in_vec_15[1] = token_9_15;
    assign in_chan_dep_vld_vec_15[2] = dep_chan_vld_11_15;
    assign in_chan_dep_data_vec_15[80 : 54] = dep_chan_data_11_15;
    assign token_in_vec_15[2] = token_11_15;
    assign in_chan_dep_vld_vec_15[3] = dep_chan_vld_14_15;
    assign in_chan_dep_data_vec_15[107 : 81] = dep_chan_data_14_15;
    assign token_in_vec_15[3] = token_14_15;
    assign in_chan_dep_vld_vec_15[4] = dep_chan_vld_16_15;
    assign in_chan_dep_data_vec_15[134 : 108] = dep_chan_data_16_15;
    assign token_in_vec_15[4] = token_16_15;
    assign in_chan_dep_vld_vec_15[5] = dep_chan_vld_17_15;
    assign in_chan_dep_data_vec_15[161 : 135] = dep_chan_data_17_15;
    assign token_in_vec_15[5] = token_17_15;
    assign in_chan_dep_vld_vec_15[6] = dep_chan_vld_19_15;
    assign in_chan_dep_data_vec_15[188 : 162] = dep_chan_data_19_15;
    assign token_in_vec_15[6] = token_19_15;
    assign dep_chan_vld_15_11 = out_chan_dep_vld_vec_15[0];
    assign dep_chan_data_15_11 = out_chan_dep_data_15;
    assign token_15_11 = token_out_vec_15[0];
    assign dep_chan_vld_15_4 = out_chan_dep_vld_vec_15[1];
    assign dep_chan_data_15_4 = out_chan_dep_data_15;
    assign token_15_4 = token_out_vec_15[1];
    assign dep_chan_vld_15_19 = out_chan_dep_vld_vec_15[2];
    assign dep_chan_data_15_19 = out_chan_dep_data_15;
    assign token_15_19 = token_out_vec_15[2];
    assign dep_chan_vld_15_16 = out_chan_dep_vld_vec_15[3];
    assign dep_chan_data_15_16 = out_chan_dep_data_15;
    assign token_15_16 = token_out_vec_15[3];
    assign dep_chan_vld_15_14 = out_chan_dep_vld_vec_15[4];
    assign dep_chan_data_15_14 = out_chan_dep_data_15;
    assign token_15_14 = token_out_vec_15[4];

    // Process: grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0
    pw_shared_hls_deadlock_detect_unit #(27, 16, 3, 3) pw_shared_hls_deadlock_detect_unit_16 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_16),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_16),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_16),
        .token_in_vec(token_in_vec_16),
        .dl_detect_in(dl_detect_out),
        .origin(origin[16]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_16),
        .out_chan_dep_data(out_chan_dep_data_16),
        .token_out_vec(token_out_vec_16),
        .dl_detect_out(dl_in_vec[16]));

    assign proc_16_data_FIFO_blk[0] = 1'b0;
    assign proc_16_data_PIPO_blk[0] = 1'b0;
    assign proc_16_start_FIFO_blk[0] = 1'b0;
    assign proc_16_TLF_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.this_m_blocks_c_channel_U.if_empty_n & grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs21_U0.this_m_blocks_c_channel_U.if_write);
    assign proc_16_input_sync_blk[0] = 1'b0;
    assign proc_16_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_16[0] = dl_detect_out ? proc_dep_vld_vec_16_reg[0] : (proc_16_data_FIFO_blk[0] | proc_16_data_PIPO_blk[0] | proc_16_start_FIFO_blk[0] | proc_16_TLF_FIFO_blk[0] | proc_16_input_sync_blk[0] | proc_16_output_sync_blk[0]);
    assign proc_16_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.this_m_k_blk_n) | (~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.grp_adds1628_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.l_mul1_blk_n);
    assign proc_16_data_PIPO_blk[1] = 1'b0;
    assign proc_16_start_FIFO_blk[1] = 1'b0;
    assign proc_16_TLF_FIFO_blk[1] = 1'b0;
    assign proc_16_input_sync_blk[1] = 1'b0;
    assign proc_16_output_sync_blk[1] = 1'b0 | (ap_done_reg_4 & grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.ap_done & ~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.ap_done);
    assign proc_dep_vld_vec_16[1] = dl_detect_out ? proc_dep_vld_vec_16_reg[1] : (proc_16_data_FIFO_blk[1] | proc_16_data_PIPO_blk[1] | proc_16_start_FIFO_blk[1] | proc_16_TLF_FIFO_blk[1] | proc_16_input_sync_blk[1] | proc_16_output_sync_blk[1]);
    assign proc_16_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.grp_adds1628_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_02_blk_n);
    assign proc_16_data_PIPO_blk[2] = 1'b0;
    assign proc_16_start_FIFO_blk[2] = 1'b0;
    assign proc_16_TLF_FIFO_blk[2] = 1'b0;
    assign proc_16_input_sync_blk[2] = 1'b0;
    assign proc_16_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_16[2] = dl_detect_out ? proc_dep_vld_vec_16_reg[2] : (proc_16_data_FIFO_blk[2] | proc_16_data_PIPO_blk[2] | proc_16_start_FIFO_blk[2] | proc_16_TLF_FIFO_blk[2] | proc_16_input_sync_blk[2] | proc_16_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_16_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_16_reg <= proc_dep_vld_vec_16;
        end
    end
    assign in_chan_dep_vld_vec_16[0] = dep_chan_vld_15_16;
    assign in_chan_dep_data_vec_16[26 : 0] = dep_chan_data_15_16;
    assign token_in_vec_16[0] = token_15_16;
    assign in_chan_dep_vld_vec_16[1] = dep_chan_vld_21_16;
    assign in_chan_dep_data_vec_16[53 : 27] = dep_chan_data_21_16;
    assign token_in_vec_16[1] = token_21_16;
    assign in_chan_dep_vld_vec_16[2] = dep_chan_vld_24_16;
    assign in_chan_dep_data_vec_16[80 : 54] = dep_chan_data_24_16;
    assign token_in_vec_16[2] = token_24_16;
    assign dep_chan_vld_16_14 = out_chan_dep_vld_vec_16[0];
    assign dep_chan_data_16_14 = out_chan_dep_data_16;
    assign token_16_14 = token_out_vec_16[0];
    assign dep_chan_vld_16_15 = out_chan_dep_vld_vec_16[1];
    assign dep_chan_data_16_15 = out_chan_dep_data_16;
    assign token_16_15 = token_out_vec_16[1];
    assign dep_chan_vld_16_24 = out_chan_dep_vld_vec_16[2];
    assign dep_chan_data_16_24 = out_chan_dep_data_16;
    assign token_16_24 = token_out_vec_16[2];

    // Process: grp_compute_fu_306.process_U0.macs_122_U0
    pw_shared_hls_deadlock_detect_unit #(27, 17, 1, 5) pw_shared_hls_deadlock_detect_unit_17 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_17),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_17),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_17),
        .token_in_vec(token_in_vec_17),
        .dl_detect_in(dl_detect_out),
        .origin(origin[17]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_17),
        .out_chan_dep_data(out_chan_dep_data_17),
        .token_out_vec(token_out_vec_17),
        .dl_detect_out(dl_in_vec[17]));

    assign proc_17_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0.this_m_blocks_blk_n);
    assign proc_17_data_PIPO_blk[0] = 1'b0;
    assign proc_17_start_FIFO_blk[0] = 1'b0;
    assign proc_17_TLF_FIFO_blk[0] = 1'b0;
    assign proc_17_input_sync_blk[0] = 1'b0;
    assign proc_17_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_17[0] = dl_detect_out ? proc_dep_vld_vec_17_reg[0] : (proc_17_data_FIFO_blk[0] | proc_17_data_PIPO_blk[0] | proc_17_start_FIFO_blk[0] | proc_17_TLF_FIFO_blk[0] | proc_17_input_sync_blk[0] | proc_17_output_sync_blk[0]);
    assign proc_17_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.this_m_k_blk_n) | (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.grp_muls_129_Pipeline_VITIS_LOOP_152_2_fu_94.p_bIn_0_0_0_06_blk_n);
    assign proc_17_data_PIPO_blk[1] = 1'b0;
    assign proc_17_start_FIFO_blk[1] = 1'b0;
    assign proc_17_TLF_FIFO_blk[1] = 1'b0;
    assign proc_17_input_sync_blk[1] = 1'b0;
    assign proc_17_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_17[1] = dl_detect_out ? proc_dep_vld_vec_17_reg[1] : (proc_17_data_FIFO_blk[1] | proc_17_data_PIPO_blk[1] | proc_17_start_FIFO_blk[1] | proc_17_TLF_FIFO_blk[1] | proc_17_input_sync_blk[1] | proc_17_output_sync_blk[1]);
    assign proc_17_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.grp_muls_129_Pipeline_VITIS_LOOP_152_2_fu_94.p_aIn_0_0_0_03_blk_n);
    assign proc_17_data_PIPO_blk[2] = 1'b0;
    assign proc_17_start_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_macs_122_U0_U.if_empty_n & grp_compute_fu_306.process_U0.macs_122_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_macs_122_U0_U.if_write);
    assign proc_17_TLF_FIFO_blk[2] = 1'b0;
    assign proc_17_input_sync_blk[2] = 1'b0;
    assign proc_17_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_17[2] = dl_detect_out ? proc_dep_vld_vec_17_reg[2] : (proc_17_data_FIFO_blk[2] | proc_17_data_PIPO_blk[2] | proc_17_start_FIFO_blk[2] | proc_17_TLF_FIFO_blk[2] | proc_17_input_sync_blk[2] | proc_17_output_sync_blk[2]);
    assign proc_17_data_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.grp_adds30_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_03_blk_n);
    assign proc_17_data_PIPO_blk[3] = 1'b0;
    assign proc_17_start_FIFO_blk[3] = 1'b0;
    assign proc_17_TLF_FIFO_blk[3] = 1'b0;
    assign proc_17_input_sync_blk[3] = 1'b0;
    assign proc_17_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_17[3] = dl_detect_out ? proc_dep_vld_vec_17_reg[3] : (proc_17_data_FIFO_blk[3] | proc_17_data_PIPO_blk[3] | proc_17_start_FIFO_blk[3] | proc_17_TLF_FIFO_blk[3] | proc_17_input_sync_blk[3] | proc_17_output_sync_blk[3]);
    assign proc_17_data_FIFO_blk[4] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0.this_m_blocks_c12_blk_n);
    assign proc_17_data_PIPO_blk[4] = 1'b0;
    assign proc_17_start_FIFO_blk[4] = 1'b0;
    assign proc_17_TLF_FIFO_blk[4] = 1'b0;
    assign proc_17_input_sync_blk[4] = 1'b0;
    assign proc_17_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_17[4] = dl_detect_out ? proc_dep_vld_vec_17_reg[4] : (proc_17_data_FIFO_blk[4] | proc_17_data_PIPO_blk[4] | proc_17_start_FIFO_blk[4] | proc_17_TLF_FIFO_blk[4] | proc_17_input_sync_blk[4] | proc_17_output_sync_blk[4]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_17_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_17_reg <= proc_dep_vld_vec_17;
        end
    end
    assign in_chan_dep_vld_vec_17[0] = dep_chan_vld_4_17;
    assign in_chan_dep_data_vec_17[26 : 0] = dep_chan_data_4_17;
    assign token_in_vec_17[0] = token_4_17;
    assign dep_chan_vld_17_14 = out_chan_dep_vld_vec_17[0];
    assign dep_chan_data_17_14 = out_chan_dep_data_17;
    assign token_17_14 = token_out_vec_17[0];
    assign dep_chan_vld_17_15 = out_chan_dep_vld_vec_17[1];
    assign dep_chan_data_17_15 = out_chan_dep_data_17;
    assign token_17_15 = token_out_vec_17[1];
    assign dep_chan_vld_17_4 = out_chan_dep_vld_vec_17[2];
    assign dep_chan_data_17_4 = out_chan_dep_data_17;
    assign token_17_4 = token_out_vec_17[2];
    assign dep_chan_vld_17_24 = out_chan_dep_vld_vec_17[3];
    assign dep_chan_data_17_24 = out_chan_dep_data_17;
    assign token_17_24 = token_out_vec_17[3];
    assign dep_chan_vld_17_22 = out_chan_dep_vld_vec_17[4];
    assign dep_chan_data_17_22 = out_chan_dep_data_17;
    assign token_17_22 = token_out_vec_17[4];

    // Process: grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0
    pw_shared_hls_deadlock_detect_unit #(27, 18, 6, 3) pw_shared_hls_deadlock_detect_unit_18 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_18),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_18),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_18),
        .token_in_vec(token_in_vec_18),
        .dl_detect_in(dl_detect_out),
        .origin(origin[18]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_18),
        .out_chan_dep_data(out_chan_dep_data_18),
        .token_out_vec(token_out_vec_18),
        .dl_detect_out(dl_in_vec[18]));

    assign proc_18_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0.this_m_blocks_blk_n);
    assign proc_18_data_PIPO_blk[0] = 1'b0;
    assign proc_18_start_FIFO_blk[0] = 1'b0;
    assign proc_18_TLF_FIFO_blk[0] = 1'b0;
    assign proc_18_input_sync_blk[0] = 1'b0;
    assign proc_18_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_18[0] = dl_detect_out ? proc_dep_vld_vec_18_reg[0] : (proc_18_data_FIFO_blk[0] | proc_18_data_PIPO_blk[0] | proc_18_start_FIFO_blk[0] | proc_18_TLF_FIFO_blk[0] | proc_18_input_sync_blk[0] | proc_18_output_sync_blk[0]);
    assign proc_18_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0.this_m_blocks_c12_blk_n);
    assign proc_18_data_PIPO_blk[1] = 1'b0;
    assign proc_18_start_FIFO_blk[1] = 1'b0;
    assign proc_18_TLF_FIFO_blk[1] = 1'b0;
    assign proc_18_input_sync_blk[1] = 1'b0;
    assign proc_18_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_18[1] = dl_detect_out ? proc_dep_vld_vec_18_reg[1] : (proc_18_data_FIFO_blk[1] | proc_18_data_PIPO_blk[1] | proc_18_start_FIFO_blk[1] | proc_18_TLF_FIFO_blk[1] | proc_18_input_sync_blk[1] | proc_18_output_sync_blk[1]);
    assign proc_18_data_FIFO_blk[2] = 1'b0;
    assign proc_18_data_PIPO_blk[2] = 1'b0;
    assign proc_18_start_FIFO_blk[2] = 1'b0;
    assign proc_18_TLF_FIFO_blk[2] = 1'b0;
    assign proc_18_input_sync_blk[2] = 1'b0 | (grp_compute_fu_306.process_U0.macs_122_U0.ap_sync_entry_proc58_U0_ap_ready & grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs_122_U0.ap_sync_muls_129_U0_ap_ready);
    assign proc_18_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_18[2] = dl_detect_out ? proc_dep_vld_vec_18_reg[2] : (proc_18_data_FIFO_blk[2] | proc_18_data_PIPO_blk[2] | proc_18_start_FIFO_blk[2] | proc_18_TLF_FIFO_blk[2] | proc_18_input_sync_blk[2] | proc_18_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_18_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_18_reg <= proc_dep_vld_vec_18;
        end
    end
    assign in_chan_dep_vld_vec_18[0] = dep_chan_vld_13_18;
    assign in_chan_dep_data_vec_18[26 : 0] = dep_chan_data_13_18;
    assign token_in_vec_18[0] = token_13_18;
    assign in_chan_dep_vld_vec_18[1] = dep_chan_vld_14_18;
    assign in_chan_dep_data_vec_18[53 : 27] = dep_chan_data_14_18;
    assign token_in_vec_18[1] = token_14_18;
    assign in_chan_dep_vld_vec_18[2] = dep_chan_vld_19_18;
    assign in_chan_dep_data_vec_18[80 : 54] = dep_chan_data_19_18;
    assign token_in_vec_18[2] = token_19_18;
    assign in_chan_dep_vld_vec_18[3] = dep_chan_vld_20_18;
    assign in_chan_dep_data_vec_18[107 : 81] = dep_chan_data_20_18;
    assign token_in_vec_18[3] = token_20_18;
    assign in_chan_dep_vld_vec_18[4] = dep_chan_vld_21_18;
    assign in_chan_dep_data_vec_18[134 : 108] = dep_chan_data_21_18;
    assign token_in_vec_18[4] = token_21_18;
    assign in_chan_dep_vld_vec_18[5] = dep_chan_vld_22_18;
    assign in_chan_dep_data_vec_18[161 : 135] = dep_chan_data_22_18;
    assign token_in_vec_18[5] = token_22_18;
    assign dep_chan_vld_18_14 = out_chan_dep_vld_vec_18[0];
    assign dep_chan_data_18_14 = out_chan_dep_data_18;
    assign token_18_14 = token_out_vec_18[0];
    assign dep_chan_vld_18_22 = out_chan_dep_vld_vec_18[1];
    assign dep_chan_data_18_22 = out_chan_dep_data_18;
    assign token_18_22 = token_out_vec_18[1];
    assign dep_chan_vld_18_19 = out_chan_dep_vld_vec_18[2];
    assign dep_chan_data_18_19 = out_chan_dep_data_18;
    assign token_18_19 = token_out_vec_18[2];

    // Process: grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0
    pw_shared_hls_deadlock_detect_unit #(27, 19, 5, 4) pw_shared_hls_deadlock_detect_unit_19 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_19),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_19),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_19),
        .token_in_vec(token_in_vec_19),
        .dl_detect_in(dl_detect_out),
        .origin(origin[19]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_19),
        .out_chan_dep_data(out_chan_dep_data_19),
        .token_out_vec(token_out_vec_19),
        .dl_detect_out(dl_in_vec[19]));

    assign proc_19_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.this_m_k_blk_n) | (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.grp_muls_129_Pipeline_VITIS_LOOP_152_2_fu_94.p_bIn_0_0_0_06_blk_n);
    assign proc_19_data_PIPO_blk[0] = 1'b0;
    assign proc_19_start_FIFO_blk[0] = 1'b0;
    assign proc_19_TLF_FIFO_blk[0] = 1'b0;
    assign proc_19_input_sync_blk[0] = 1'b0;
    assign proc_19_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_19[0] = dl_detect_out ? proc_dep_vld_vec_19_reg[0] : (proc_19_data_FIFO_blk[0] | proc_19_data_PIPO_blk[0] | proc_19_start_FIFO_blk[0] | proc_19_TLF_FIFO_blk[0] | proc_19_input_sync_blk[0] | proc_19_output_sync_blk[0]);
    assign proc_19_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.grp_muls_129_Pipeline_VITIS_LOOP_152_2_fu_94.p_aIn_0_0_0_03_blk_n);
    assign proc_19_data_PIPO_blk[1] = 1'b0;
    assign proc_19_start_FIFO_blk[1] = 1'b0;
    assign proc_19_TLF_FIFO_blk[1] = 1'b0;
    assign proc_19_input_sync_blk[1] = 1'b0;
    assign proc_19_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_19[1] = dl_detect_out ? proc_dep_vld_vec_19_reg[1] : (proc_19_data_FIFO_blk[1] | proc_19_data_PIPO_blk[1] | proc_19_start_FIFO_blk[1] | proc_19_TLF_FIFO_blk[1] | proc_19_input_sync_blk[1] | proc_19_output_sync_blk[1]);
    assign proc_19_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.grp_muls_129_Pipeline_VITIS_LOOP_152_2_fu_94.l_mul1_blk_n) | (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.this_m_k_c_blk_n);
    assign proc_19_data_PIPO_blk[2] = 1'b0;
    assign proc_19_start_FIFO_blk[2] = 1'b0;
    assign proc_19_TLF_FIFO_blk[2] = 1'b0;
    assign proc_19_input_sync_blk[2] = 1'b0;
    assign proc_19_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_19[2] = dl_detect_out ? proc_dep_vld_vec_19_reg[2] : (proc_19_data_FIFO_blk[2] | proc_19_data_PIPO_blk[2] | proc_19_start_FIFO_blk[2] | proc_19_TLF_FIFO_blk[2] | proc_19_input_sync_blk[2] | proc_19_output_sync_blk[2]);
    assign proc_19_data_FIFO_blk[3] = 1'b0;
    assign proc_19_data_PIPO_blk[3] = 1'b0;
    assign proc_19_start_FIFO_blk[3] = 1'b0;
    assign proc_19_TLF_FIFO_blk[3] = 1'b0;
    assign proc_19_input_sync_blk[3] = 1'b0 | (grp_compute_fu_306.process_U0.macs_122_U0.ap_sync_muls_129_U0_ap_ready & grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs_122_U0.ap_sync_entry_proc58_U0_ap_ready);
    assign proc_19_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_19[3] = dl_detect_out ? proc_dep_vld_vec_19_reg[3] : (proc_19_data_FIFO_blk[3] | proc_19_data_PIPO_blk[3] | proc_19_start_FIFO_blk[3] | proc_19_TLF_FIFO_blk[3] | proc_19_input_sync_blk[3] | proc_19_output_sync_blk[3]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_19_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_19_reg <= proc_dep_vld_vec_19;
        end
    end
    assign in_chan_dep_vld_vec_19[0] = dep_chan_vld_4_19;
    assign in_chan_dep_data_vec_19[26 : 0] = dep_chan_data_4_19;
    assign token_in_vec_19[0] = token_4_19;
    assign in_chan_dep_vld_vec_19[1] = dep_chan_vld_13_19;
    assign in_chan_dep_data_vec_19[53 : 27] = dep_chan_data_13_19;
    assign token_in_vec_19[1] = token_13_19;
    assign in_chan_dep_vld_vec_19[2] = dep_chan_vld_15_19;
    assign in_chan_dep_data_vec_19[80 : 54] = dep_chan_data_15_19;
    assign token_in_vec_19[2] = token_15_19;
    assign in_chan_dep_vld_vec_19[3] = dep_chan_vld_18_19;
    assign in_chan_dep_data_vec_19[107 : 81] = dep_chan_data_18_19;
    assign token_in_vec_19[3] = token_18_19;
    assign in_chan_dep_vld_vec_19[4] = dep_chan_vld_20_19;
    assign in_chan_dep_data_vec_19[134 : 108] = dep_chan_data_20_19;
    assign token_in_vec_19[4] = token_20_19;
    assign dep_chan_vld_19_15 = out_chan_dep_vld_vec_19[0];
    assign dep_chan_data_19_15 = out_chan_dep_data_19;
    assign token_19_15 = token_out_vec_19[0];
    assign dep_chan_vld_19_4 = out_chan_dep_vld_vec_19[1];
    assign dep_chan_data_19_4 = out_chan_dep_data_19;
    assign token_19_4 = token_out_vec_19[1];
    assign dep_chan_vld_19_20 = out_chan_dep_vld_vec_19[2];
    assign dep_chan_data_19_20 = out_chan_dep_data_19;
    assign token_19_20 = token_out_vec_19[2];
    assign dep_chan_vld_19_18 = out_chan_dep_vld_vec_19[3];
    assign dep_chan_data_19_18 = out_chan_dep_data_19;
    assign token_19_18 = token_out_vec_19[3];

    // Process: grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0
    pw_shared_hls_deadlock_detect_unit #(27, 20, 3, 3) pw_shared_hls_deadlock_detect_unit_20 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_20),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_20),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_20),
        .token_in_vec(token_in_vec_20),
        .dl_detect_in(dl_detect_out),
        .origin(origin[20]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_20),
        .out_chan_dep_data(out_chan_dep_data_20),
        .token_out_vec(token_out_vec_20),
        .dl_detect_out(dl_in_vec[20]));

    assign proc_20_data_FIFO_blk[0] = 1'b0;
    assign proc_20_data_PIPO_blk[0] = 1'b0;
    assign proc_20_start_FIFO_blk[0] = 1'b0;
    assign proc_20_TLF_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.this_m_blocks_c_channel_U.if_empty_n & grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs_122_U0.this_m_blocks_c_channel_U.if_write);
    assign proc_20_input_sync_blk[0] = 1'b0;
    assign proc_20_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_20[0] = dl_detect_out ? proc_dep_vld_vec_20_reg[0] : (proc_20_data_FIFO_blk[0] | proc_20_data_PIPO_blk[0] | proc_20_start_FIFO_blk[0] | proc_20_TLF_FIFO_blk[0] | proc_20_input_sync_blk[0] | proc_20_output_sync_blk[0]);
    assign proc_20_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.this_m_k_blk_n) | (~grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.grp_adds30_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.l_mul1_blk_n);
    assign proc_20_data_PIPO_blk[1] = 1'b0;
    assign proc_20_start_FIFO_blk[1] = 1'b0;
    assign proc_20_TLF_FIFO_blk[1] = 1'b0;
    assign proc_20_input_sync_blk[1] = 1'b0;
    assign proc_20_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_20[1] = dl_detect_out ? proc_dep_vld_vec_20_reg[1] : (proc_20_data_FIFO_blk[1] | proc_20_data_PIPO_blk[1] | proc_20_start_FIFO_blk[1] | proc_20_TLF_FIFO_blk[1] | proc_20_input_sync_blk[1] | proc_20_output_sync_blk[1]);
    assign proc_20_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.grp_adds30_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_03_blk_n);
    assign proc_20_data_PIPO_blk[2] = 1'b0;
    assign proc_20_start_FIFO_blk[2] = 1'b0;
    assign proc_20_TLF_FIFO_blk[2] = 1'b0;
    assign proc_20_input_sync_blk[2] = 1'b0;
    assign proc_20_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_20[2] = dl_detect_out ? proc_dep_vld_vec_20_reg[2] : (proc_20_data_FIFO_blk[2] | proc_20_data_PIPO_blk[2] | proc_20_start_FIFO_blk[2] | proc_20_TLF_FIFO_blk[2] | proc_20_input_sync_blk[2] | proc_20_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_20_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_20_reg <= proc_dep_vld_vec_20;
        end
    end
    assign in_chan_dep_vld_vec_20[0] = dep_chan_vld_19_20;
    assign in_chan_dep_data_vec_20[26 : 0] = dep_chan_data_19_20;
    assign token_in_vec_20[0] = token_19_20;
    assign in_chan_dep_vld_vec_20[1] = dep_chan_vld_21_20;
    assign in_chan_dep_data_vec_20[53 : 27] = dep_chan_data_21_20;
    assign token_in_vec_20[1] = token_21_20;
    assign in_chan_dep_vld_vec_20[2] = dep_chan_vld_24_20;
    assign in_chan_dep_data_vec_20[80 : 54] = dep_chan_data_24_20;
    assign token_in_vec_20[2] = token_24_20;
    assign dep_chan_vld_20_18 = out_chan_dep_vld_vec_20[0];
    assign dep_chan_data_20_18 = out_chan_dep_data_20;
    assign token_20_18 = token_out_vec_20[0];
    assign dep_chan_vld_20_19 = out_chan_dep_vld_vec_20[1];
    assign dep_chan_data_20_19 = out_chan_dep_data_20;
    assign token_20_19 = token_out_vec_20[1];
    assign dep_chan_vld_20_24 = out_chan_dep_vld_vec_20[2];
    assign dep_chan_data_20_24 = out_chan_dep_data_20;
    assign token_20_24 = token_out_vec_20[2];

    // Process: grp_compute_fu_306.process_U0.merge_U0
    pw_shared_hls_deadlock_detect_unit #(27, 21, 1, 7) pw_shared_hls_deadlock_detect_unit_21 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_21),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_21),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_21),
        .token_in_vec(token_in_vec_21),
        .dl_detect_in(dl_detect_out),
        .origin(origin[21]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_21),
        .out_chan_dep_data(out_chan_dep_data_21),
        .token_out_vec(token_out_vec_21),
        .dl_detect_out(dl_in_vec[21]));

    assign proc_21_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.grp_merge17_Pipeline_VITIS_LOOP_83_1_fu_50.p_strs_0_0_0_0_blk_n);
    assign proc_21_data_PIPO_blk[0] = 1'b0;
    assign proc_21_start_FIFO_blk[0] = 1'b0;
    assign proc_21_TLF_FIFO_blk[0] = 1'b0;
    assign proc_21_input_sync_blk[0] = 1'b0;
    assign proc_21_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_21[0] = dl_detect_out ? proc_dep_vld_vec_21_reg[0] : (proc_21_data_FIFO_blk[0] | proc_21_data_PIPO_blk[0] | proc_21_start_FIFO_blk[0] | proc_21_TLF_FIFO_blk[0] | proc_21_input_sync_blk[0] | proc_21_output_sync_blk[0]);
    assign proc_21_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.grp_merge17_Pipeline_VITIS_LOOP_83_1_fu_50.p_strs_0_0_0_01_blk_n);
    assign proc_21_data_PIPO_blk[1] = 1'b0;
    assign proc_21_start_FIFO_blk[1] = 1'b0;
    assign proc_21_TLF_FIFO_blk[1] = 1'b0;
    assign proc_21_input_sync_blk[1] = 1'b0;
    assign proc_21_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_21[1] = dl_detect_out ? proc_dep_vld_vec_21_reg[1] : (proc_21_data_FIFO_blk[1] | proc_21_data_PIPO_blk[1] | proc_21_start_FIFO_blk[1] | proc_21_TLF_FIFO_blk[1] | proc_21_input_sync_blk[1] | proc_21_output_sync_blk[1]);
    assign proc_21_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.p_strs_0_0_0_02_blk_n);
    assign proc_21_data_PIPO_blk[2] = 1'b0;
    assign proc_21_start_FIFO_blk[2] = 1'b0;
    assign proc_21_TLF_FIFO_blk[2] = 1'b0;
    assign proc_21_input_sync_blk[2] = 1'b0;
    assign proc_21_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_21[2] = dl_detect_out ? proc_dep_vld_vec_21_reg[2] : (proc_21_data_FIFO_blk[2] | proc_21_data_PIPO_blk[2] | proc_21_start_FIFO_blk[2] | proc_21_TLF_FIFO_blk[2] | proc_21_input_sync_blk[2] | proc_21_output_sync_blk[2]);
    assign proc_21_data_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.p_strs_0_0_0_03_blk_n);
    assign proc_21_data_PIPO_blk[3] = 1'b0;
    assign proc_21_start_FIFO_blk[3] = 1'b0;
    assign proc_21_TLF_FIFO_blk[3] = 1'b0;
    assign proc_21_input_sync_blk[3] = 1'b0;
    assign proc_21_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_21[3] = dl_detect_out ? proc_dep_vld_vec_21_reg[3] : (proc_21_data_FIFO_blk[3] | proc_21_data_PIPO_blk[3] | proc_21_start_FIFO_blk[3] | proc_21_TLF_FIFO_blk[3] | proc_21_input_sync_blk[3] | proc_21_output_sync_blk[3]);
    assign proc_21_data_FIFO_blk[4] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.grp_merge_1_Pipeline_VITIS_LOOP_61_1_fu_38.c5_blk_n);
    assign proc_21_data_PIPO_blk[4] = 1'b0;
    assign proc_21_start_FIFO_blk[4] = 1'b0;
    assign proc_21_TLF_FIFO_blk[4] = 1'b0;
    assign proc_21_input_sync_blk[4] = 1'b0;
    assign proc_21_output_sync_blk[4] = 1'b0;
    assign proc_dep_vld_vec_21[4] = dl_detect_out ? proc_dep_vld_vec_21_reg[4] : (proc_21_data_FIFO_blk[4] | proc_21_data_PIPO_blk[4] | proc_21_start_FIFO_blk[4] | proc_21_TLF_FIFO_blk[4] | proc_21_input_sync_blk[4] | proc_21_output_sync_blk[4]);
    assign proc_21_data_FIFO_blk[5] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0.this_m_blocks_blk_n);
    assign proc_21_data_PIPO_blk[5] = 1'b0;
    assign proc_21_start_FIFO_blk[5] = 1'b0;
    assign proc_21_TLF_FIFO_blk[5] = 1'b0;
    assign proc_21_input_sync_blk[5] = 1'b0;
    assign proc_21_output_sync_blk[5] = 1'b0;
    assign proc_dep_vld_vec_21[5] = dl_detect_out ? proc_dep_vld_vec_21_reg[5] : (proc_21_data_FIFO_blk[5] | proc_21_data_PIPO_blk[5] | proc_21_start_FIFO_blk[5] | proc_21_TLF_FIFO_blk[5] | proc_21_input_sync_blk[5] | proc_21_output_sync_blk[5]);
    assign proc_21_data_FIFO_blk[6] = 1'b0;
    assign proc_21_data_PIPO_blk[6] = 1'b0;
    assign proc_21_start_FIFO_blk[6] = 1'b0 | (~grp_compute_fu_306.process_U0.start_for_merge_U0_U.if_empty_n & grp_compute_fu_306.process_U0.merge_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_merge_U0_U.if_write);
    assign proc_21_TLF_FIFO_blk[6] = 1'b0;
    assign proc_21_input_sync_blk[6] = 1'b0;
    assign proc_21_output_sync_blk[6] = 1'b0;
    assign proc_dep_vld_vec_21[6] = dl_detect_out ? proc_dep_vld_vec_21_reg[6] : (proc_21_data_FIFO_blk[6] | proc_21_data_PIPO_blk[6] | proc_21_start_FIFO_blk[6] | proc_21_TLF_FIFO_blk[6] | proc_21_input_sync_blk[6] | proc_21_output_sync_blk[6]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_21_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_21_reg <= proc_dep_vld_vec_21;
        end
    end
    assign in_chan_dep_vld_vec_21[0] = dep_chan_vld_5_21;
    assign in_chan_dep_data_vec_21[26 : 0] = dep_chan_data_5_21;
    assign token_in_vec_21[0] = token_5_21;
    assign dep_chan_vld_21_8 = out_chan_dep_vld_vec_21[0];
    assign dep_chan_data_21_8 = out_chan_dep_data_21;
    assign token_21_8 = token_out_vec_21[0];
    assign dep_chan_vld_21_12 = out_chan_dep_vld_vec_21[1];
    assign dep_chan_data_21_12 = out_chan_dep_data_21;
    assign token_21_12 = token_out_vec_21[1];
    assign dep_chan_vld_21_16 = out_chan_dep_vld_vec_21[2];
    assign dep_chan_data_21_16 = out_chan_dep_data_21;
    assign token_21_16 = token_out_vec_21[2];
    assign dep_chan_vld_21_20 = out_chan_dep_vld_vec_21[3];
    assign dep_chan_data_21_20 = out_chan_dep_data_21;
    assign token_21_20 = token_out_vec_21[3];
    assign dep_chan_vld_21_26 = out_chan_dep_vld_vec_21[4];
    assign dep_chan_data_21_26 = out_chan_dep_data_21;
    assign token_21_26 = token_out_vec_21[4];
    assign dep_chan_vld_21_18 = out_chan_dep_vld_vec_21[5];
    assign dep_chan_data_21_18 = out_chan_dep_data_21;
    assign token_21_18 = token_out_vec_21[5];
    assign dep_chan_vld_21_5 = out_chan_dep_vld_vec_21[6];
    assign dep_chan_data_21_5 = out_chan_dep_data_21;
    assign token_21_5 = token_out_vec_21[6];

    // Process: grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0
    pw_shared_hls_deadlock_detect_unit #(27, 22, 4, 1) pw_shared_hls_deadlock_detect_unit_22 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_22),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_22),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_22),
        .token_in_vec(token_in_vec_22),
        .dl_detect_in(dl_detect_out),
        .origin(origin[22]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_22),
        .out_chan_dep_data(out_chan_dep_data_22),
        .token_out_vec(token_out_vec_22),
        .dl_detect_out(dl_in_vec[22]));

    assign proc_22_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0.this_m_blocks_blk_n);
    assign proc_22_data_PIPO_blk[0] = 1'b0;
    assign proc_22_start_FIFO_blk[0] = 1'b0;
    assign proc_22_TLF_FIFO_blk[0] = 1'b0;
    assign proc_22_input_sync_blk[0] = 1'b0;
    assign proc_22_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_22[0] = dl_detect_out ? proc_dep_vld_vec_22_reg[0] : (proc_22_data_FIFO_blk[0] | proc_22_data_PIPO_blk[0] | proc_22_start_FIFO_blk[0] | proc_22_TLF_FIFO_blk[0] | proc_22_input_sync_blk[0] | proc_22_output_sync_blk[0]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_22_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_22_reg <= proc_dep_vld_vec_22;
        end
    end
    assign in_chan_dep_vld_vec_22[0] = dep_chan_vld_17_22;
    assign in_chan_dep_data_vec_22[26 : 0] = dep_chan_data_17_22;
    assign token_in_vec_22[0] = token_17_22;
    assign in_chan_dep_vld_vec_22[1] = dep_chan_vld_18_22;
    assign in_chan_dep_data_vec_22[53 : 27] = dep_chan_data_18_22;
    assign token_in_vec_22[1] = token_18_22;
    assign in_chan_dep_vld_vec_22[2] = dep_chan_vld_23_22;
    assign in_chan_dep_data_vec_22[80 : 54] = dep_chan_data_23_22;
    assign token_in_vec_22[2] = token_23_22;
    assign in_chan_dep_vld_vec_22[3] = dep_chan_vld_24_22;
    assign in_chan_dep_data_vec_22[107 : 81] = dep_chan_data_24_22;
    assign token_in_vec_22[3] = token_24_22;
    assign dep_chan_vld_22_18 = out_chan_dep_vld_vec_22[0];
    assign dep_chan_data_22_18 = out_chan_dep_data_22;
    assign token_22_18 = token_out_vec_22[0];

    // Process: grp_compute_fu_306.process_U0.merge_U0.merge17_U0
    pw_shared_hls_deadlock_detect_unit #(27, 23, 5, 4) pw_shared_hls_deadlock_detect_unit_23 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_23),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_23),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_23),
        .token_in_vec(token_in_vec_23),
        .dl_detect_in(dl_detect_out),
        .origin(origin[23]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_23),
        .out_chan_dep_data(out_chan_dep_data_23),
        .token_out_vec(token_out_vec_23),
        .dl_detect_out(dl_in_vec[23]));

    assign proc_23_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.grp_merge17_Pipeline_VITIS_LOOP_83_1_fu_50.p_strs_0_0_0_0_blk_n);
    assign proc_23_data_PIPO_blk[0] = 1'b0;
    assign proc_23_start_FIFO_blk[0] = 1'b0;
    assign proc_23_TLF_FIFO_blk[0] = 1'b0;
    assign proc_23_input_sync_blk[0] = 1'b0;
    assign proc_23_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_23[0] = dl_detect_out ? proc_dep_vld_vec_23_reg[0] : (proc_23_data_FIFO_blk[0] | proc_23_data_PIPO_blk[0] | proc_23_start_FIFO_blk[0] | proc_23_TLF_FIFO_blk[0] | proc_23_input_sync_blk[0] | proc_23_output_sync_blk[0]);
    assign proc_23_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.grp_merge17_Pipeline_VITIS_LOOP_83_1_fu_50.p_strs_0_0_0_01_blk_n);
    assign proc_23_data_PIPO_blk[1] = 1'b0;
    assign proc_23_start_FIFO_blk[1] = 1'b0;
    assign proc_23_TLF_FIFO_blk[1] = 1'b0;
    assign proc_23_input_sync_blk[1] = 1'b0;
    assign proc_23_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_23[1] = dl_detect_out ? proc_dep_vld_vec_23_reg[1] : (proc_23_data_FIFO_blk[1] | proc_23_data_PIPO_blk[1] | proc_23_start_FIFO_blk[1] | proc_23_TLF_FIFO_blk[1] | proc_23_input_sync_blk[1] | proc_23_output_sync_blk[1]);
    assign proc_23_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.grp_merge17_Pipeline_VITIS_LOOP_83_1_fu_50.l_left2_blk_n) | (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.this_m_blocks_c_blk_n);
    assign proc_23_data_PIPO_blk[2] = 1'b0;
    assign proc_23_start_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.start_for_merge_1_U0_U.if_full_n & grp_compute_fu_306.process_U0.merge_U0.merge17_U0.ap_start & ~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.real_start & (trans_in_cnt_3 == trans_out_cnt_3) & ~grp_compute_fu_306.process_U0.merge_U0.start_for_merge_1_U0_U.if_read);
    assign proc_23_TLF_FIFO_blk[2] = 1'b0;
    assign proc_23_input_sync_blk[2] = 1'b0;
    assign proc_23_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_23[2] = dl_detect_out ? proc_dep_vld_vec_23_reg[2] : (proc_23_data_FIFO_blk[2] | proc_23_data_PIPO_blk[2] | proc_23_start_FIFO_blk[2] | proc_23_TLF_FIFO_blk[2] | proc_23_input_sync_blk[2] | proc_23_output_sync_blk[2]);
    assign proc_23_data_FIFO_blk[3] = 1'b0;
    assign proc_23_data_PIPO_blk[3] = 1'b0;
    assign proc_23_start_FIFO_blk[3] = 1'b0;
    assign proc_23_TLF_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp_channel_U.if_empty_n & grp_compute_fu_306.process_U0.merge_U0.merge17_U0.ap_idle & ~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp_channel_U.if_write);
    assign proc_23_input_sync_blk[3] = 1'b0;
    assign proc_23_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_23[3] = dl_detect_out ? proc_dep_vld_vec_23_reg[3] : (proc_23_data_FIFO_blk[3] | proc_23_data_PIPO_blk[3] | proc_23_start_FIFO_blk[3] | proc_23_TLF_FIFO_blk[3] | proc_23_input_sync_blk[3] | proc_23_output_sync_blk[3]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_23_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_23_reg <= proc_dep_vld_vec_23;
        end
    end
    assign in_chan_dep_vld_vec_23[0] = dep_chan_vld_5_23;
    assign in_chan_dep_data_vec_23[26 : 0] = dep_chan_data_5_23;
    assign token_in_vec_23[0] = token_5_23;
    assign in_chan_dep_vld_vec_23[1] = dep_chan_vld_8_23;
    assign in_chan_dep_data_vec_23[53 : 27] = dep_chan_data_8_23;
    assign token_in_vec_23[1] = token_8_23;
    assign in_chan_dep_vld_vec_23[2] = dep_chan_vld_9_23;
    assign in_chan_dep_data_vec_23[80 : 54] = dep_chan_data_9_23;
    assign token_in_vec_23[2] = token_9_23;
    assign in_chan_dep_vld_vec_23[3] = dep_chan_vld_12_23;
    assign in_chan_dep_data_vec_23[107 : 81] = dep_chan_data_12_23;
    assign token_in_vec_23[3] = token_12_23;
    assign in_chan_dep_vld_vec_23[4] = dep_chan_vld_25_23;
    assign in_chan_dep_data_vec_23[134 : 108] = dep_chan_data_25_23;
    assign token_in_vec_23[4] = token_25_23;
    assign dep_chan_vld_23_8 = out_chan_dep_vld_vec_23[0];
    assign dep_chan_data_23_8 = out_chan_dep_data_23;
    assign token_23_8 = token_out_vec_23[0];
    assign dep_chan_vld_23_12 = out_chan_dep_vld_vec_23[1];
    assign dep_chan_data_23_12 = out_chan_dep_data_23;
    assign token_23_12 = token_out_vec_23[1];
    assign dep_chan_vld_23_25 = out_chan_dep_vld_vec_23[2];
    assign dep_chan_data_23_25 = out_chan_dep_data_23;
    assign token_23_25 = token_out_vec_23[2];
    assign dep_chan_vld_23_22 = out_chan_dep_vld_vec_23[3];
    assign dep_chan_data_23_22 = out_chan_dep_data_23;
    assign token_23_22 = token_out_vec_23[3];

    // Process: grp_compute_fu_306.process_U0.merge_U0.merge18_U0
    pw_shared_hls_deadlock_detect_unit #(27, 24, 5, 4) pw_shared_hls_deadlock_detect_unit_24 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_24),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_24),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_24),
        .token_in_vec(token_in_vec_24),
        .dl_detect_in(dl_detect_out),
        .origin(origin[24]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_24),
        .out_chan_dep_data(out_chan_dep_data_24),
        .token_out_vec(token_out_vec_24),
        .dl_detect_out(dl_in_vec[24]));

    assign proc_24_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.p_strs_0_0_0_02_blk_n);
    assign proc_24_data_PIPO_blk[0] = 1'b0;
    assign proc_24_start_FIFO_blk[0] = 1'b0;
    assign proc_24_TLF_FIFO_blk[0] = 1'b0;
    assign proc_24_input_sync_blk[0] = 1'b0;
    assign proc_24_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_24[0] = dl_detect_out ? proc_dep_vld_vec_24_reg[0] : (proc_24_data_FIFO_blk[0] | proc_24_data_PIPO_blk[0] | proc_24_start_FIFO_blk[0] | proc_24_TLF_FIFO_blk[0] | proc_24_input_sync_blk[0] | proc_24_output_sync_blk[0]);
    assign proc_24_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.p_strs_0_0_0_03_blk_n);
    assign proc_24_data_PIPO_blk[1] = 1'b0;
    assign proc_24_start_FIFO_blk[1] = 1'b0;
    assign proc_24_TLF_FIFO_blk[1] = 1'b0;
    assign proc_24_input_sync_blk[1] = 1'b0;
    assign proc_24_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_24[1] = dl_detect_out ? proc_dep_vld_vec_24_reg[1] : (proc_24_data_FIFO_blk[1] | proc_24_data_PIPO_blk[1] | proc_24_start_FIFO_blk[1] | proc_24_TLF_FIFO_blk[1] | proc_24_input_sync_blk[1] | proc_24_output_sync_blk[1]);
    assign proc_24_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.l_left2_blk_n);
    assign proc_24_data_PIPO_blk[2] = 1'b0;
    assign proc_24_start_FIFO_blk[2] = 1'b0;
    assign proc_24_TLF_FIFO_blk[2] = 1'b0;
    assign proc_24_input_sync_blk[2] = 1'b0;
    assign proc_24_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_24[2] = dl_detect_out ? proc_dep_vld_vec_24_reg[2] : (proc_24_data_FIFO_blk[2] | proc_24_data_PIPO_blk[2] | proc_24_start_FIFO_blk[2] | proc_24_TLF_FIFO_blk[2] | proc_24_input_sync_blk[2] | proc_24_output_sync_blk[2]);
    assign proc_24_data_FIFO_blk[3] = 1'b0;
    assign proc_24_data_PIPO_blk[3] = 1'b0;
    assign proc_24_start_FIFO_blk[3] = 1'b0;
    assign proc_24_TLF_FIFO_blk[3] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp1_channel_U.if_empty_n & grp_compute_fu_306.process_U0.merge_U0.merge18_U0.ap_idle & ~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp1_channel_U.if_write);
    assign proc_24_input_sync_blk[3] = 1'b0;
    assign proc_24_output_sync_blk[3] = 1'b0;
    assign proc_dep_vld_vec_24[3] = dl_detect_out ? proc_dep_vld_vec_24_reg[3] : (proc_24_data_FIFO_blk[3] | proc_24_data_PIPO_blk[3] | proc_24_start_FIFO_blk[3] | proc_24_TLF_FIFO_blk[3] | proc_24_input_sync_blk[3] | proc_24_output_sync_blk[3]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_24_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_24_reg <= proc_dep_vld_vec_24;
        end
    end
    assign in_chan_dep_vld_vec_24[0] = dep_chan_vld_13_24;
    assign in_chan_dep_data_vec_24[26 : 0] = dep_chan_data_13_24;
    assign token_in_vec_24[0] = token_13_24;
    assign in_chan_dep_vld_vec_24[1] = dep_chan_vld_16_24;
    assign in_chan_dep_data_vec_24[53 : 27] = dep_chan_data_16_24;
    assign token_in_vec_24[1] = token_16_24;
    assign in_chan_dep_vld_vec_24[2] = dep_chan_vld_17_24;
    assign in_chan_dep_data_vec_24[80 : 54] = dep_chan_data_17_24;
    assign token_in_vec_24[2] = token_17_24;
    assign in_chan_dep_vld_vec_24[3] = dep_chan_vld_20_24;
    assign in_chan_dep_data_vec_24[107 : 81] = dep_chan_data_20_24;
    assign token_in_vec_24[3] = token_20_24;
    assign in_chan_dep_vld_vec_24[4] = dep_chan_vld_25_24;
    assign in_chan_dep_data_vec_24[134 : 108] = dep_chan_data_25_24;
    assign token_in_vec_24[4] = token_25_24;
    assign dep_chan_vld_24_16 = out_chan_dep_vld_vec_24[0];
    assign dep_chan_data_24_16 = out_chan_dep_data_24;
    assign token_24_16 = token_out_vec_24[0];
    assign dep_chan_vld_24_20 = out_chan_dep_vld_vec_24[1];
    assign dep_chan_data_24_20 = out_chan_dep_data_24;
    assign token_24_20 = token_out_vec_24[1];
    assign dep_chan_vld_24_25 = out_chan_dep_vld_vec_24[2];
    assign dep_chan_data_24_25 = out_chan_dep_data_24;
    assign token_24_25 = token_out_vec_24[2];
    assign dep_chan_vld_24_22 = out_chan_dep_vld_vec_24[3];
    assign dep_chan_data_24_22 = out_chan_dep_data_24;
    assign token_24_22 = token_out_vec_24[3];

    // Process: grp_compute_fu_306.process_U0.merge_U0.merge_1_U0
    pw_shared_hls_deadlock_detect_unit #(27, 25, 3, 3) pw_shared_hls_deadlock_detect_unit_25 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_25),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_25),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_25),
        .token_in_vec(token_in_vec_25),
        .dl_detect_in(dl_detect_out),
        .origin(origin[25]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_25),
        .out_chan_dep_data(out_chan_dep_data_25),
        .token_out_vec(token_out_vec_25),
        .dl_detect_out(dl_in_vec[25]));

    assign proc_25_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.grp_merge_1_Pipeline_VITIS_LOOP_61_1_fu_38.l_left2_blk_n) | (~grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.p_n_blk_n);
    assign proc_25_data_PIPO_blk[0] = 1'b0;
    assign proc_25_start_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.start_for_merge_1_U0_U.if_empty_n & grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.ap_idle & ~grp_compute_fu_306.process_U0.merge_U0.start_for_merge_1_U0_U.if_write);
    assign proc_25_TLF_FIFO_blk[0] = 1'b0;
    assign proc_25_input_sync_blk[0] = 1'b0;
    assign proc_25_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_25[0] = dl_detect_out ? proc_dep_vld_vec_25_reg[0] : (proc_25_data_FIFO_blk[0] | proc_25_data_PIPO_blk[0] | proc_25_start_FIFO_blk[0] | proc_25_TLF_FIFO_blk[0] | proc_25_input_sync_blk[0] | proc_25_output_sync_blk[0]);
    assign proc_25_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.grp_merge_1_Pipeline_VITIS_LOOP_61_1_fu_38.l_right3_blk_n);
    assign proc_25_data_PIPO_blk[1] = 1'b0;
    assign proc_25_start_FIFO_blk[1] = 1'b0;
    assign proc_25_TLF_FIFO_blk[1] = 1'b0;
    assign proc_25_input_sync_blk[1] = 1'b0;
    assign proc_25_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_25[1] = dl_detect_out ? proc_dep_vld_vec_25_reg[1] : (proc_25_data_FIFO_blk[1] | proc_25_data_PIPO_blk[1] | proc_25_start_FIFO_blk[1] | proc_25_TLF_FIFO_blk[1] | proc_25_input_sync_blk[1] | proc_25_output_sync_blk[1]);
    assign proc_25_data_FIFO_blk[2] = 1'b0 | (~grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.grp_merge_1_Pipeline_VITIS_LOOP_61_1_fu_38.c5_blk_n);
    assign proc_25_data_PIPO_blk[2] = 1'b0;
    assign proc_25_start_FIFO_blk[2] = 1'b0;
    assign proc_25_TLF_FIFO_blk[2] = 1'b0;
    assign proc_25_input_sync_blk[2] = 1'b0;
    assign proc_25_output_sync_blk[2] = 1'b0;
    assign proc_dep_vld_vec_25[2] = dl_detect_out ? proc_dep_vld_vec_25_reg[2] : (proc_25_data_FIFO_blk[2] | proc_25_data_PIPO_blk[2] | proc_25_start_FIFO_blk[2] | proc_25_TLF_FIFO_blk[2] | proc_25_input_sync_blk[2] | proc_25_output_sync_blk[2]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_25_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_25_reg <= proc_dep_vld_vec_25;
        end
    end
    assign in_chan_dep_vld_vec_25[0] = dep_chan_vld_23_25;
    assign in_chan_dep_data_vec_25[26 : 0] = dep_chan_data_23_25;
    assign token_in_vec_25[0] = token_23_25;
    assign in_chan_dep_vld_vec_25[1] = dep_chan_vld_24_25;
    assign in_chan_dep_data_vec_25[53 : 27] = dep_chan_data_24_25;
    assign token_in_vec_25[1] = token_24_25;
    assign in_chan_dep_vld_vec_25[2] = dep_chan_vld_26_25;
    assign in_chan_dep_data_vec_25[80 : 54] = dep_chan_data_26_25;
    assign token_in_vec_25[2] = token_26_25;
    assign dep_chan_vld_25_23 = out_chan_dep_vld_vec_25[0];
    assign dep_chan_data_25_23 = out_chan_dep_data_25;
    assign token_25_23 = token_out_vec_25[0];
    assign dep_chan_vld_25_24 = out_chan_dep_vld_vec_25[1];
    assign dep_chan_data_25_24 = out_chan_dep_data_25;
    assign token_25_24 = token_out_vec_25[1];
    assign dep_chan_vld_25_26 = out_chan_dep_vld_vec_25[2];
    assign dep_chan_data_25_26 = out_chan_dep_data_25;
    assign token_25_26 = token_out_vec_25[2];

    // Process: grp_compute_fu_306.collect_U0
    pw_shared_hls_deadlock_detect_unit #(27, 26, 3, 2) pw_shared_hls_deadlock_detect_unit_26 (
        .reset(dl_reset),
        .clock(dl_clock),
        .proc_dep_vld_vec(proc_dep_vld_vec_26),
        .in_chan_dep_vld_vec(in_chan_dep_vld_vec_26),
        .in_chan_dep_data_vec(in_chan_dep_data_vec_26),
        .token_in_vec(token_in_vec_26),
        .dl_detect_in(dl_detect_out),
        .origin(origin[26]),
        .token_clear(token_clear),
        .out_chan_dep_vld_vec(out_chan_dep_vld_vec_26),
        .out_chan_dep_data(out_chan_dep_data_26),
        .token_out_vec(token_out_vec_26),
        .dl_detect_out(dl_in_vec[26]));

    assign proc_26_data_FIFO_blk[0] = 1'b0 | (~grp_compute_fu_306.collect_U0.grp_collect_Pipeline_VITIS_LOOP_30_1_VITIS_LOOP_30_2_fu_54.c5_blk_n);
    assign proc_26_data_PIPO_blk[0] = 1'b0;
    assign proc_26_start_FIFO_blk[0] = 1'b0;
    assign proc_26_TLF_FIFO_blk[0] = 1'b0;
    assign proc_26_input_sync_blk[0] = 1'b0;
    assign proc_26_output_sync_blk[0] = 1'b0;
    assign proc_dep_vld_vec_26[0] = dl_detect_out ? proc_dep_vld_vec_26_reg[0] : (proc_26_data_FIFO_blk[0] | proc_26_data_PIPO_blk[0] | proc_26_start_FIFO_blk[0] | proc_26_TLF_FIFO_blk[0] | proc_26_input_sync_blk[0] | proc_26_output_sync_blk[0]);
    assign proc_26_data_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.collect_U0.np_blk_n);
    assign proc_26_data_PIPO_blk[1] = 1'b0;
    assign proc_26_start_FIFO_blk[1] = 1'b0 | (~grp_compute_fu_306.start_for_collect_U0_U.if_empty_n & grp_compute_fu_306.collect_U0.ap_idle & ~grp_compute_fu_306.start_for_collect_U0_U.if_write);
    assign proc_26_TLF_FIFO_blk[1] = 1'b0;
    assign proc_26_input_sync_blk[1] = 1'b0;
    assign proc_26_output_sync_blk[1] = 1'b0;
    assign proc_dep_vld_vec_26[1] = dl_detect_out ? proc_dep_vld_vec_26_reg[1] : (proc_26_data_FIFO_blk[1] | proc_26_data_PIPO_blk[1] | proc_26_start_FIFO_blk[1] | proc_26_TLF_FIFO_blk[1] | proc_26_input_sync_blk[1] | proc_26_output_sync_blk[1]);
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            proc_dep_vld_vec_26_reg <= 'b0;
        end
        else begin
            proc_dep_vld_vec_26_reg <= proc_dep_vld_vec_26;
        end
    end
    assign in_chan_dep_vld_vec_26[0] = dep_chan_vld_2_26;
    assign in_chan_dep_data_vec_26[26 : 0] = dep_chan_data_2_26;
    assign token_in_vec_26[0] = token_2_26;
    assign in_chan_dep_vld_vec_26[1] = dep_chan_vld_21_26;
    assign in_chan_dep_data_vec_26[53 : 27] = dep_chan_data_21_26;
    assign token_in_vec_26[1] = token_21_26;
    assign in_chan_dep_vld_vec_26[2] = dep_chan_vld_25_26;
    assign in_chan_dep_data_vec_26[80 : 54] = dep_chan_data_25_26;
    assign token_in_vec_26[2] = token_25_26;
    assign dep_chan_vld_26_25 = out_chan_dep_vld_vec_26[0];
    assign dep_chan_data_26_25 = out_chan_dep_data_26;
    assign token_26_25 = token_out_vec_26[0];
    assign dep_chan_vld_26_2 = out_chan_dep_vld_vec_26[1];
    assign dep_chan_data_26_2 = out_chan_dep_data_26;
    assign token_26_2 = token_out_vec_26[1];


`include "pw_shared_hls_deadlock_report_unit.vh"
