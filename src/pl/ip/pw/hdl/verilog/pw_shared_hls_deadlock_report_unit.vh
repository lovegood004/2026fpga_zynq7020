   
    parameter PROC_NUM = 27;
    parameter ST_IDLE = 3'b000;
    parameter ST_FILTER_FAKE = 3'b001;
    parameter ST_DL_DETECTED = 3'b010;
    parameter ST_DL_REPORT = 3'b100;
   

    reg find_df_deadlock = 0;
    reg [2:0] CS_fsm;
    reg [2:0] NS_fsm;
    reg [PROC_NUM - 1:0] dl_detect_reg;
    reg [PROC_NUM - 1:0] dl_done_reg;
    reg [PROC_NUM - 1:0] origin_reg;
    reg [PROC_NUM - 1:0] dl_in_vec_reg;
    reg [31:0] dl_keep_cnt;
    integer i;
    integer fp;

    // FSM State machine
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            CS_fsm <= ST_IDLE;
        end
        else begin
            CS_fsm <= NS_fsm;
        end
    end
    always @ (CS_fsm or dl_in_vec or dl_detect_reg or dl_done_reg or dl_in_vec or origin_reg or dl_keep_cnt) begin
        NS_fsm = CS_fsm;
        case (CS_fsm)
            ST_IDLE : begin
                if (|dl_in_vec) begin
                    NS_fsm = ST_FILTER_FAKE;
                end
            end
            ST_FILTER_FAKE: begin
                if (dl_keep_cnt == 32'd1000) begin
                    NS_fsm = ST_DL_DETECTED;
                end
                else if (dl_detect_reg != (dl_detect_reg & dl_in_vec)) begin
                    NS_fsm = ST_IDLE;
                end
            end
            ST_DL_DETECTED: begin
                // has unreported deadlock cycle
                if (dl_detect_reg != dl_done_reg) begin
                    NS_fsm = ST_DL_REPORT;
                end
            end
            ST_DL_REPORT: begin
                if (|(dl_in_vec & origin_reg)) begin
                    NS_fsm = ST_DL_DETECTED;
                end
            end
        endcase
    end

    // dl_detect_reg record the procs that first detect deadlock
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            dl_detect_reg <= 'b0;
        end
        else begin
            if (CS_fsm == ST_IDLE) begin
                dl_detect_reg <= dl_in_vec;
            end
        end
    end

    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            dl_keep_cnt <= 32'h0;
        end
        else begin
            if (CS_fsm == ST_FILTER_FAKE && (dl_detect_reg == (dl_detect_reg & dl_in_vec))) begin
                dl_keep_cnt <= dl_keep_cnt + 32'h1;
            end
            else if (CS_fsm == ST_FILTER_FAKE && (dl_detect_reg != (dl_detect_reg & dl_in_vec))) begin
                dl_keep_cnt <= 32'h0;
            end
        end
    end

    // dl_detect_out keeps in high after deadlock detected
    assign dl_detect_out = (|dl_detect_reg) && (CS_fsm == ST_DL_DETECTED || CS_fsm == ST_DL_REPORT);

    // dl_done_reg record the cycles has been reported
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            dl_done_reg <= 'b0;
        end
        else begin
            if ((CS_fsm == ST_DL_REPORT) && (|(dl_in_vec & dl_detect_reg) == 'b1)) begin
                dl_done_reg <= dl_done_reg | dl_in_vec;
            end
        end
    end

    // clear token once a cycle is done
    assign token_clear = (CS_fsm == ST_DL_REPORT) ? ((|(dl_in_vec & origin_reg)) ? 'b1 : 'b0) : 'b0;

    // origin_reg record the current cycle start id
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            origin_reg <= 'b0;
        end
        else begin
            if (CS_fsm == ST_DL_DETECTED) begin
                origin_reg <= origin;
            end
        end
    end
   
    // origin will be valid for only one cycle
    always @ (CS_fsm or dl_detect_reg or dl_done_reg) begin
        if (CS_fsm == ST_DL_DETECTED) begin
            for (i = 0; i < PROC_NUM; i = i + 1) begin
                if (dl_detect_reg[i] & ~dl_done_reg[i] & ~(|origin)) begin
                    origin = 'b1 << i;
                end
            end
        end
        else begin
            origin = 'b0;
        end
    end
    
    // dl_in_vec_reg record the current cycle dl_in_vec
    always @ (negedge dl_reset or posedge dl_clock) begin
        if (~dl_reset) begin
            dl_in_vec_reg <= 'b0;
        end
        else begin
            if (CS_fsm == ST_DL_DETECTED) begin
                dl_in_vec_reg <= origin;
            end
            else if (CS_fsm == ST_DL_REPORT) begin
                dl_in_vec_reg <= dl_in_vec;
            end
        end
    end
    
    // get the first valid proc index in dl vector
    function integer proc_index(input [PROC_NUM - 1:0] dl_vec);
        begin
            proc_index = 0;
            for (i = 0; i < PROC_NUM; i = i + 1) begin
                if (dl_vec[i]) begin
                    proc_index = i;
                end
            end
        end
    endfunction

    // get the proc path based on dl vector
    function [656:0] proc_path(input [PROC_NUM - 1:0] dl_vec);
        integer index;
        begin
            index = proc_index(dl_vec);
            case (index)
                0 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.feed_w_U0";
                end
                1 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.feed_x_U0";
                end
                2 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.compute_Block_split36_proc_U0";
                end
                3 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0";
                end
                4 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0";
                end
                5 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0";
                end
                6 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0";
                end
                7 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0";
                end
                8 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0";
                end
                9 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0";
                end
                10 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0";
                end
                11 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0";
                end
                12 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0";
                end
                13 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0";
                end
                14 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0";
                end
                15 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0";
                end
                16 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0";
                end
                17 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0";
                end
                18 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0";
                end
                19 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0";
                end
                20 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0";
                end
                21 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0";
                end
                22 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0";
                end
                23 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0";
                end
                24 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0";
                end
                25 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge_1_U0";
                end
                26 : begin
                    proc_path = "pw_shared_pw_shared.grp_compute_fu_306.collect_U0";
                end
                default : begin
                    proc_path = "unknown";
                end
            endcase
        end
    endfunction

    // print the headlines of deadlock detection
    task print_dl_head;
        begin
            $display("\n//////////////////////////////////////////////////////////////////////////////");
            $display("// ERROR!!! DEADLOCK DETECTED at %0t ns! SIMULATION WILL BE STOPPED! //", $time);
            $display("//////////////////////////////////////////////////////////////////////////////");
            fp = $fopen("deadlock_db.dat", "w");
        end
    endtask

    // print the start of a cycle
    task print_cycle_start(input reg [656:0] proc_path, input integer cycle_id);
        begin
            $display("/////////////////////////");
            $display("// Dependence cycle %0d:", cycle_id);
            $display("// (1): Process: %0s", proc_path);
            $fdisplay(fp, "Dependence_Cycle_ID %0d", cycle_id);
            $fdisplay(fp, "Dependence_Process_ID 1");
            $fdisplay(fp, "Dependence_Process_path %0s", proc_path);
        end
    endtask

    // print the end of deadlock detection
    task print_dl_end(input integer num, input integer record_time);
        begin
            $display("////////////////////////////////////////////////////////////////////////");
            $display("// Totally %0d cycles detected!", num);
            $display("////////////////////////////////////////////////////////////////////////");
            $display("// ERROR!!! DEADLOCK DETECTED at %0t ns! SIMULATION WILL BE STOPPED! //", record_time);
            $display("//////////////////////////////////////////////////////////////////////////////");
            $fdisplay(fp, "Dependence_Cycle_Number %0d", num);
            $fclose(fp);
        end
    endtask

    // print one proc component in the cycle
    task print_cycle_proc_comp(input reg [656:0] proc_path, input integer cycle_comp_id);
        begin
            $display("// (%0d): Process: %0s", cycle_comp_id, proc_path);
            $fdisplay(fp, "Dependence_Process_ID %0d", cycle_comp_id);
            $fdisplay(fp, "Dependence_Process_path %0s", proc_path);
        end
    endtask

    // print one channel component in the cycle
    task print_cycle_chan_comp(input [PROC_NUM - 1:0] dl_vec1, input [PROC_NUM - 1:0] dl_vec2);
        reg [736:0] chan_path;
        integer index1;
        integer index2;
        begin
            index1 = proc_index(dl_vec1);
            index2 = proc_index(dl_vec2);
            case (index1)
                0 : begin
                    case(index2)
                    3: begin
                        if (~grp_compute_fu_306.feed_w_U0.a3_blk_n) begin
                            if (~grp_compute_fu_306.a_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.a_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.a_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.a_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.a_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.a_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    1: begin
                        if (grp_compute_fu_306.ap_sync_feed_w_U0_ap_ready & grp_compute_fu_306.feed_w_U0.ap_idle & ~grp_compute_fu_306.ap_sync_feed_x_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.feed_x_U0'");
                        end
                    end
                    2: begin
                        if (grp_compute_fu_306.ap_sync_feed_w_U0_ap_ready & grp_compute_fu_306.feed_w_U0.ap_idle & ~grp_compute_fu_306.ap_sync_compute_Block_split36_proc_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.compute_Block_split36_proc_U0'");
                        end
                    end
                    endcase
                end
                1 : begin
                    case(index2)
                    3: begin
                        if (~grp_compute_fu_306.feed_x_U0.b4_blk_n) begin
                            if (~grp_compute_fu_306.b_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.b_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.b_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.b_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.b_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.b_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    0: begin
                        if (grp_compute_fu_306.ap_sync_feed_x_U0_ap_ready & grp_compute_fu_306.feed_x_U0.ap_idle & ~grp_compute_fu_306.ap_sync_feed_w_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.feed_w_U0'");
                        end
                    end
                    2: begin
                        if (grp_compute_fu_306.ap_sync_feed_x_U0_ap_ready & grp_compute_fu_306.feed_x_U0.ap_idle & ~grp_compute_fu_306.ap_sync_compute_Block_split36_proc_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.compute_Block_split36_proc_U0'");
                        end
                    end
                    endcase
                end
                2 : begin
                    case(index2)
                    26: begin
                        if (~grp_compute_fu_306.compute_Block_split36_proc_U0.np_c_blk_n) begin
                            if (~grp_compute_fu_306.np_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.np_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.collect_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.np_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.np_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.np_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.collect_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.np_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.start_for_collect_U0_U.if_full_n & grp_compute_fu_306.compute_Block_split36_proc_U0.ap_start & ~grp_compute_fu_306.compute_Block_split36_proc_U0.real_start & (trans_in_cnt_4 == trans_out_cnt_4) & ~grp_compute_fu_306.start_for_collect_U0_U.if_read) begin
                            $display("//      Blocked by full output start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.start_for_collect_U0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.collect_U0',");
                        end
                    end
                    0: begin
                        if (grp_compute_fu_306.ap_sync_compute_Block_split36_proc_U0_ap_ready & grp_compute_fu_306.compute_Block_split36_proc_U0.ap_idle & ~grp_compute_fu_306.ap_sync_feed_w_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.feed_w_U0'");
                        end
                    end
                    1: begin
                        if (grp_compute_fu_306.ap_sync_compute_Block_split36_proc_U0_ap_ready & grp_compute_fu_306.compute_Block_split36_proc_U0.ap_idle & ~grp_compute_fu_306.ap_sync_feed_x_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.feed_x_U0'");
                        end
                    end
                    endcase
                end
                3 : begin
                    case(index2)
                    2: begin
                        if (~grp_compute_fu_306.array_m_blocks_1_channel_U.if_empty_n & grp_compute_fu_306.process_U0.tagAB_U0.ap_idle & ~grp_compute_fu_306.array_m_blocks_1_channel_U.if_write) begin
                            if (~grp_compute_fu_306.array_m_blocks_1_channel_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.array_m_blocks_1_channel_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.compute_Block_split36_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.array_m_blocks_1_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.array_m_blocks_1_channel_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.array_m_blocks_1_channel_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.compute_Block_split36_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.array_m_blocks_1_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.array_m_k_channel_U.if_empty_n & grp_compute_fu_306.process_U0.tagAB_U0.ap_idle & ~grp_compute_fu_306.array_m_k_channel_U.if_write) begin
                            if (~grp_compute_fu_306.array_m_k_channel_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.array_m_k_channel_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.compute_Block_split36_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.array_m_k_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.array_m_k_channel_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.array_m_k_channel_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.compute_Block_split36_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.array_m_k_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    0: begin
                        if (~grp_compute_fu_306.process_U0.tagAB_U0.grp_tagAB_Pipeline_VITIS_LOOP_270_1_VITIS_LOOP_271_2_fu_72.a3_blk_n) begin
                            if (~grp_compute_fu_306.a_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.a_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.feed_w_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.a_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.a_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.a_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.feed_w_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.a_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    1: begin
                        if (~grp_compute_fu_306.process_U0.tagAB_U0.grp_tagAB_Pipeline_VITIS_LOOP_270_1_VITIS_LOOP_271_2_fu_72.b4_blk_n) begin
                            if (~grp_compute_fu_306.b_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.b_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.feed_x_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.b_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.b_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.b_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.feed_x_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.b_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    4: begin
                        if (~grp_compute_fu_306.process_U0.tagAB_U0.grp_tagAB_Pipeline_VITIS_LOOP_270_1_VITIS_LOOP_271_2_fu_72.l_aStr1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_aStr_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_aStr_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_aStr_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_aStr_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_aStr_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_aStr_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.tagAB_U0.grp_tagAB_Pipeline_VITIS_LOOP_270_1_VITIS_LOOP_271_2_fu_72.l_bStr2_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_bStr_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_bStr_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_bStr_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_bStr_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_bStr_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_bStr_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.start_for_systolicArray_U0_U.if_full_n & grp_compute_fu_306.process_U0.tagAB_U0.ap_start & ~grp_compute_fu_306.process_U0.tagAB_U0.real_start & (trans_in_cnt_0 == trans_out_cnt_0) & ~grp_compute_fu_306.process_U0.start_for_systolicArray_U0_U.if_read) begin
                            $display("//      Blocked by full output start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_systolicArray_U0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0',");
                        end
                    end
                    6: begin
                        if (~grp_compute_fu_306.process_U0.tagAB_U0.this_m_blocks_c24_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c24_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c24_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    7: begin
                        if (~grp_compute_fu_306.process_U0.tagAB_U0.this_m_k_c27_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c27_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c27_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    5: begin
                        if (~grp_compute_fu_306.process_U0.start_for_macs19_U0_U.if_full_n & grp_compute_fu_306.process_U0.tagAB_U0.ap_start & ~grp_compute_fu_306.process_U0.tagAB_U0.real_start & (trans_in_cnt_0 == trans_out_cnt_0) & ~grp_compute_fu_306.process_U0.start_for_macs19_U0_U.if_read) begin
                            $display("//      Blocked by full output start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_macs19_U0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0',");
                        end
                    end
                    endcase
                end
                4 : begin
                    case(index2)
                    3: begin
                        if (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.l_aStr1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_aStr_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_aStr_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_aStr_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_aStr_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_aStr_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_aStr_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.l_bStr2_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_bStr_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_bStr_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_bStr_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_bStr_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_bStr_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_bStr_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.start_for_systolicArray_U0_U.if_empty_n & grp_compute_fu_306.process_U0.systolicArray_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_systolicArray_U0_U.if_write) begin
                            $display("//      Blocked by missing 'ap_start' from start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_systolicArray_U0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0',");
                        end
                    end
                    7: begin
                        if (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.p_aOut_0_0_0_0_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_0_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_0_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.p_bOut_0_0_0_0_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_0_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_0_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    11: begin
                        if (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.p_aOut_0_0_0_01_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    15: begin
                        if (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.p_aOut_0_0_0_02_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    19: begin
                        if (~grp_compute_fu_306.process_U0.systolicArray_U0.grp_systolicArray_Pipeline_VITIS_LOOP_246_1_fu_66.p_aOut_0_0_0_03_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    9: begin
                        if (~grp_compute_fu_306.process_U0.start_for_macs20_U0_U.if_full_n & grp_compute_fu_306.process_U0.systolicArray_U0.ap_start & ~grp_compute_fu_306.process_U0.systolicArray_U0.real_start & (trans_in_cnt_1 == trans_out_cnt_1) & ~grp_compute_fu_306.process_U0.start_for_macs20_U0_U.if_read) begin
                            $display("//      Blocked by full output start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_macs20_U0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0',");
                        end
                    end
                    13: begin
                        if (~grp_compute_fu_306.process_U0.start_for_macs21_U0_U.if_full_n & grp_compute_fu_306.process_U0.systolicArray_U0.ap_start & ~grp_compute_fu_306.process_U0.systolicArray_U0.real_start & (trans_in_cnt_1 == trans_out_cnt_1) & ~grp_compute_fu_306.process_U0.start_for_macs21_U0_U.if_read) begin
                            $display("//      Blocked by full output start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_macs21_U0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0',");
                        end
                    end
                    17: begin
                        if (~grp_compute_fu_306.process_U0.start_for_macs_122_U0_U.if_full_n & grp_compute_fu_306.process_U0.systolicArray_U0.ap_start & ~grp_compute_fu_306.process_U0.systolicArray_U0.real_start & (trans_in_cnt_1 == trans_out_cnt_1) & ~grp_compute_fu_306.process_U0.start_for_macs_122_U0_U.if_read) begin
                            $display("//      Blocked by full output start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_macs_122_U0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0',");
                        end
                    end
                    endcase
                end
                5 : begin
                    case(index2)
                    3: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0.this_m_blocks_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c24_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c24_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c27_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c27_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.start_for_macs19_U0_U.if_empty_n & grp_compute_fu_306.process_U0.macs19_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_macs19_U0_U.if_write) begin
                            $display("//      Blocked by missing 'ap_start' from start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_macs19_U0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0',");
                        end
                    end
                    4: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_0_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_0_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_0_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_0_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_0_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_0_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    11: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_07_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.this_m_k_c26_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c26_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c26_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    23: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.grp_adds1624_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_0_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_0_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_0_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    10: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0.this_m_blocks_c23_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c23_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c23_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    21: begin
                        if (~grp_compute_fu_306.process_U0.start_for_merge_U0_U.if_full_n & grp_compute_fu_306.process_U0.macs19_U0.ap_start & ~grp_compute_fu_306.process_U0.macs19_U0.real_start & (trans_in_cnt_2 == trans_out_cnt_2) & ~grp_compute_fu_306.process_U0.start_for_merge_U0_U.if_read) begin
                            $display("//      Blocked by full output start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_merge_U0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0',");
                        end
                    end
                    endcase
                end
                6 : begin
                    case(index2)
                    3: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0.this_m_blocks_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c24_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c24_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c24_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    10: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0.this_m_blocks_c23_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c23_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c23_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    7: begin
                        if (grp_compute_fu_306.process_U0.macs19_U0.ap_sync_entry_proc57_U0_ap_ready & grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs19_U0.ap_sync_muls23_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                        end
                    end
                    endcase
                end
                7 : begin
                    case(index2)
                    3: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c27_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c27_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.tagAB_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c27_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    4: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_0_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_0_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_0_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_0_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_0_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_0_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    11: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_07_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.this_m_k_c26_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c26_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c26_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    8: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.grp_muls23_Pipeline_VITIS_LOOP_100_2_fu_106.l_mul1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs19_U0.l_mul_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.l_mul_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs19_U0.l_mul_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.l_mul_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.this_m_k_c_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (ap_done_reg_8 & grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.ap_done & ~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.ap_done) begin
                            $display("//      Blocked by output sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0'");
                        end
                    end
                    6: begin
                        if (grp_compute_fu_306.process_U0.macs19_U0.ap_sync_muls23_U0_ap_ready & grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs19_U0.ap_sync_entry_proc57_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0'");
                        end
                    end
                    endcase
                end
                8 : begin
                    case(index2)
                    6: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.this_m_blocks_c_channel_U.if_empty_n & grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs19_U0.this_m_blocks_c_channel_U.if_write) begin
                            if (~grp_compute_fu_306.process_U0.macs19_U0.this_m_blocks_c_channel_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_blocks_c_channel_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_blocks_c_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs19_U0.this_m_blocks_c_channel_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_blocks_c_channel_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_blocks_c_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    7: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.grp_adds1624_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.l_mul1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs19_U0.l_mul_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.l_mul_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs19_U0.l_mul_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.l_mul_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (ap_done_reg_2 & grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.ap_done & ~grp_compute_fu_306.process_U0.macs19_U0.muls23_U0.ap_done) begin
                            $display("//      Blocked by output sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                        end
                    end
                    23: begin
                        if (~grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0.grp_adds1624_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_0_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_0_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_0_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                9 : begin
                    case(index2)
                    6: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0.this_m_blocks_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c23_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c23_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    7: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c26_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c26_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_04_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    4: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_01_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.start_for_macs20_U0_U.if_empty_n & grp_compute_fu_306.process_U0.macs20_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_macs20_U0_U.if_write) begin
                            $display("//      Blocked by missing 'ap_start' from start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_macs20_U0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0',");
                        end
                    end
                    15: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_08_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.this_m_k_c25_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c25_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c25_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    23: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.grp_adds1626_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_01_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    14: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0.this_m_blocks_c22_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c22_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c22_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                10 : begin
                    case(index2)
                    6: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0.this_m_blocks_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c23_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c23_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.entry_proc57_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c23_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    14: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0.this_m_blocks_c22_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c22_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c22_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    11: begin
                        if (grp_compute_fu_306.process_U0.macs20_U0.ap_sync_entry_proc56_U0_ap_ready & grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs20_U0.ap_sync_muls25_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                        end
                    end
                    endcase
                end
                11 : begin
                    case(index2)
                    7: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c26_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c26_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c26_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_04_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.muls23_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    4: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_01_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    15: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_08_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.this_m_k_c25_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c25_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c25_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    12: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.grp_muls25_Pipeline_VITIS_LOOP_100_2_fu_106.l_mul1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs20_U0.l_mul_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.l_mul_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs20_U0.l_mul_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.l_mul_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.this_m_k_c_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (ap_done_reg_9 & grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.ap_done & ~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.ap_done) begin
                            $display("//      Blocked by output sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0'");
                        end
                    end
                    10: begin
                        if (grp_compute_fu_306.process_U0.macs20_U0.ap_sync_muls25_U0_ap_ready & grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs20_U0.ap_sync_entry_proc56_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                        end
                    end
                    endcase
                end
                12 : begin
                    case(index2)
                    10: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.this_m_blocks_c_channel_U.if_empty_n & grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs20_U0.this_m_blocks_c_channel_U.if_write) begin
                            if (~grp_compute_fu_306.process_U0.macs20_U0.this_m_blocks_c_channel_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_blocks_c_channel_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_blocks_c_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs20_U0.this_m_blocks_c_channel_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_blocks_c_channel_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_blocks_c_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    11: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.grp_adds1626_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.l_mul1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs20_U0.l_mul_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.l_mul_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs20_U0.l_mul_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.l_mul_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (ap_done_reg_3 & grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.ap_done & ~grp_compute_fu_306.process_U0.macs20_U0.muls25_U0.ap_done) begin
                            $display("//      Blocked by output sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                        end
                    end
                    23: begin
                        if (~grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0.grp_adds1626_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_01_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                13 : begin
                    case(index2)
                    10: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0.this_m_blocks_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c22_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c22_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    11: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c25_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c25_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_05_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    4: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_02_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.start_for_macs21_U0_U.if_empty_n & grp_compute_fu_306.process_U0.macs21_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_macs21_U0_U.if_write) begin
                            $display("//      Blocked by missing 'ap_start' from start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_macs21_U0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0',");
                        end
                    end
                    19: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_09_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.this_m_k_c1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    24: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.grp_adds1628_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_02_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    18: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0.this_m_blocks_c21_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c21_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c21_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                14 : begin
                    case(index2)
                    10: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0.this_m_blocks_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c22_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c22_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.entry_proc56_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c22_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    18: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0.this_m_blocks_c21_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c21_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c21_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    15: begin
                        if (grp_compute_fu_306.process_U0.macs21_U0.ap_sync_entry_proc_U0_ap_ready & grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs21_U0.ap_sync_muls27_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                        end
                    end
                    endcase
                end
                15 : begin
                    case(index2)
                    11: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c25_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c25_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c25_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_bIn_0_0_0_05_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.muls25_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    4: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_aIn_0_0_0_02_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    19: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.p_bOut_0_0_0_09_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.this_m_k_c1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    16: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.grp_muls27_Pipeline_VITIS_LOOP_100_2_fu_106.l_mul1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs21_U0.l_mul_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.l_mul_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs21_U0.l_mul_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.l_mul_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.this_m_k_c_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (ap_done_reg_10 & grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.ap_done & ~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.ap_done) begin
                            $display("//      Blocked by output sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0'");
                        end
                    end
                    14: begin
                        if (grp_compute_fu_306.process_U0.macs21_U0.ap_sync_muls27_U0_ap_ready & grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs21_U0.ap_sync_entry_proc_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                        end
                    end
                    endcase
                end
                16 : begin
                    case(index2)
                    14: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.this_m_blocks_c_channel_U.if_empty_n & grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs21_U0.this_m_blocks_c_channel_U.if_write) begin
                            if (~grp_compute_fu_306.process_U0.macs21_U0.this_m_blocks_c_channel_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_blocks_c_channel_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_blocks_c_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs21_U0.this_m_blocks_c_channel_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_blocks_c_channel_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_blocks_c_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    15: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.grp_adds1628_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.l_mul1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs21_U0.l_mul_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.l_mul_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs21_U0.l_mul_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.l_mul_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (ap_done_reg_4 & grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.ap_done & ~grp_compute_fu_306.process_U0.macs21_U0.muls27_U0.ap_done) begin
                            $display("//      Blocked by output sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                        end
                    end
                    24: begin
                        if (~grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0.grp_adds1628_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_02_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                17 : begin
                    case(index2)
                    14: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0.this_m_blocks_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c21_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c21_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    15: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.grp_muls_129_Pipeline_VITIS_LOOP_152_2_fu_94.p_bIn_0_0_0_06_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    4: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.grp_muls_129_Pipeline_VITIS_LOOP_152_2_fu_94.p_aIn_0_0_0_03_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.start_for_macs_122_U0_U.if_empty_n & grp_compute_fu_306.process_U0.macs_122_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_macs_122_U0_U.if_write) begin
                            $display("//      Blocked by missing 'ap_start' from start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_macs_122_U0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0',");
                        end
                    end
                    24: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.grp_adds30_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_03_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    22: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0.this_m_blocks_c12_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                18 : begin
                    case(index2)
                    14: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0.this_m_blocks_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c21_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c21_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.entry_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c21_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    22: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0.this_m_blocks_c12_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    19: begin
                        if (grp_compute_fu_306.process_U0.macs_122_U0.ap_sync_entry_proc58_U0_ap_ready & grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs_122_U0.ap_sync_muls_129_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                        end
                    end
                    endcase
                end
                19 : begin
                    case(index2)
                    15: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.grp_muls_129_Pipeline_VITIS_LOOP_152_2_fu_94.p_bIn_0_0_0_06_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataB_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataB_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.muls27_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataB_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    4: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.grp_muls_129_Pipeline_VITIS_LOOP_152_2_fu_94.p_aIn_0_0_0_03_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_dataA_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_dataA_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.systolicArray_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_dataA_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    20: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.grp_muls_129_Pipeline_VITIS_LOOP_152_2_fu_94.l_mul1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.this_m_k_c_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    18: begin
                        if (grp_compute_fu_306.process_U0.macs_122_U0.ap_sync_muls_129_U0_ap_ready & grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs_122_U0.ap_sync_entry_proc58_U0_ap_ready) begin
                            $display("//      Blocked by input sync logic with process : 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                        end
                    end
                    endcase
                end
                20 : begin
                    case(index2)
                    18: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.this_m_blocks_c_channel_U.if_empty_n & grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.ap_idle & ~grp_compute_fu_306.process_U0.macs_122_U0.this_m_blocks_c_channel_U.if_write) begin
                            if (~grp_compute_fu_306.process_U0.macs_122_U0.this_m_blocks_c_channel_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_blocks_c_channel_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_blocks_c_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs_122_U0.this_m_blocks_c_channel_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_blocks_c_channel_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_blocks_c_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    19: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.this_m_k_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.this_m_k_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.grp_adds30_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.l_mul1_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.muls_129_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.l_mul_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    24: begin
                        if (~grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0.grp_adds30_Pipeline_VITIS_LOOP_192_1_VITIS_LOOP_193_2_fu_52.p_sum_0_0_0_03_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                21 : begin
                    case(index2)
                    8: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.grp_merge17_Pipeline_VITIS_LOOP_83_1_fu_50.p_strs_0_0_0_0_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_0_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_0_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    12: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.grp_merge17_Pipeline_VITIS_LOOP_83_1_fu_50.p_strs_0_0_0_01_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    16: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.p_strs_0_0_0_02_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    20: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.p_strs_0_0_0_03_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    26: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.grp_merge_1_Pipeline_VITIS_LOOP_61_1_fu_38.c5_blk_n) begin
                            if (~grp_compute_fu_306.c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.collect_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.collect_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    18: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0.this_m_blocks_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    5: begin
                        if (~grp_compute_fu_306.process_U0.start_for_merge_U0_U.if_empty_n & grp_compute_fu_306.process_U0.merge_U0.ap_idle & ~grp_compute_fu_306.process_U0.start_for_merge_U0_U.if_write) begin
                            $display("//      Blocked by missing 'ap_start' from start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.start_for_merge_U0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0',");
                        end
                    end
                    endcase
                end
                22 : begin
                    case(index2)
                    18: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0.this_m_blocks_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.this_m_blocks_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.this_m_blocks_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.entry_proc58_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                23 : begin
                    case(index2)
                    8: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.grp_merge17_Pipeline_VITIS_LOOP_83_1_fu_50.p_strs_0_0_0_0_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_0_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_0_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs19_U0.adds1624_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_0_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    12: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.grp_merge17_Pipeline_VITIS_LOOP_83_1_fu_50.p_strs_0_0_0_01_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_1_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_1_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs20_U0.adds1626_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_1_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    25: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.grp_merge17_Pipeline_VITIS_LOOP_83_1_fu_50.l_left2_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.merge_U0.l_left_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_left_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge_1_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_left_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.merge_U0.l_left_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_left_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge_1_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_left_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.this_m_blocks_c_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge_1_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge_1_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.merge_U0.start_for_merge_1_U0_U.if_full_n & grp_compute_fu_306.process_U0.merge_U0.merge17_U0.ap_start & ~grp_compute_fu_306.process_U0.merge_U0.merge17_U0.real_start & (trans_in_cnt_3 == trans_out_cnt_3) & ~grp_compute_fu_306.process_U0.merge_U0.start_for_merge_1_U0_U.if_read) begin
                            $display("//      Blocked by full output start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.start_for_merge_1_U0_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge_1_U0',");
                        end
                    end
                    22: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp_channel_U.if_empty_n & grp_compute_fu_306.process_U0.merge_U0.merge17_U0.ap_idle & ~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp_channel_U.if_write) begin
                            if (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp_channel_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp_channel_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp_channel_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp_channel_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                24 : begin
                    case(index2)
                    16: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.p_strs_0_0_0_02_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_2_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_2_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs21_U0.adds1628_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_2_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    20: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.p_strs_0_0_0_03_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.l_sum_V_V_3_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.l_sum_V_V_3_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.macs_122_U0.adds30_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.l_sum_V_V_3_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    25: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge18_U0.l_left2_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.merge_U0.l_right_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_right_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge_1_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_right_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.merge_U0.l_right_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_right_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge_1_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_right_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    22: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp1_channel_U.if_empty_n & grp_compute_fu_306.process_U0.merge_U0.merge18_U0.ap_idle & ~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp1_channel_U.if_write) begin
                            if (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp1_channel_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp1_channel_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp1_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp1_channel_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp1_channel_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.entry_proc59_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_tmp1_channel_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                25 : begin
                    case(index2)
                    23: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.grp_merge_1_Pipeline_VITIS_LOOP_61_1_fu_38.l_left2_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.merge_U0.l_left_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_left_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_left_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.merge_U0.l_left_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_left_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_left_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.p_n_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.this_m_blocks_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.process_U0.merge_U0.start_for_merge_1_U0_U.if_empty_n & grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.ap_idle & ~grp_compute_fu_306.process_U0.merge_U0.start_for_merge_1_U0_U.if_write) begin
                            $display("//      Blocked by missing 'ap_start' from start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.start_for_merge_1_U0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge17_U0',");
                        end
                    end
                    24: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.grp_merge_1_Pipeline_VITIS_LOOP_61_1_fu_38.l_right3_blk_n) begin
                            if (~grp_compute_fu_306.process_U0.merge_U0.l_right_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_right_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_right_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.process_U0.merge_U0.l_right_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_right_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge18_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.l_right_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    26: begin
                        if (~grp_compute_fu_306.process_U0.merge_U0.merge_1_U0.grp_merge_1_Pipeline_VITIS_LOOP_61_1_fu_38.c5_blk_n) begin
                            if (~grp_compute_fu_306.c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.collect_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.collect_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    endcase
                end
                26 : begin
                    case(index2)
                    25: begin
                        if (~grp_compute_fu_306.collect_U0.grp_collect_Pipeline_VITIS_LOOP_30_1_VITIS_LOOP_30_2_fu_54.c5_blk_n) begin
                            if (~grp_compute_fu_306.c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge_1_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.process_U0.merge_U0.merge_1_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                    end
                    2: begin
                        if (~grp_compute_fu_306.collect_U0.np_blk_n) begin
                            if (~grp_compute_fu_306.np_c_U.if_empty_n) begin
                                $display("//      Blocked by empty input FIFO 'pw_shared_pw_shared.grp_compute_fu_306.np_c_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.compute_Block_split36_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.np_c_U");
                                $fdisplay(fp, "Dependence_Channel_status EMPTY");
                            end
                            else if (~grp_compute_fu_306.np_c_U.if_full_n) begin
                                $display("//      Blocked by full output FIFO 'pw_shared_pw_shared.grp_compute_fu_306.np_c_U' read by process 'pw_shared_pw_shared.grp_compute_fu_306.compute_Block_split36_proc_U0'");
                                $fdisplay(fp, "Dependence_Channel_path pw_shared_pw_shared.grp_compute_fu_306.np_c_U");
                                $fdisplay(fp, "Dependence_Channel_status FULL");
                            end
                        end
                        if (~grp_compute_fu_306.start_for_collect_U0_U.if_empty_n & grp_compute_fu_306.collect_U0.ap_idle & ~grp_compute_fu_306.start_for_collect_U0_U.if_write) begin
                            $display("//      Blocked by missing 'ap_start' from start propagation FIFO 'pw_shared_pw_shared.grp_compute_fu_306.start_for_collect_U0_U' written by process 'pw_shared_pw_shared.grp_compute_fu_306.compute_Block_split36_proc_U0',");
                        end
                    end
                    endcase
                end
            endcase
        end
    endtask

    // report
    initial begin : report_deadlock
        integer cycle_id;
        integer cycle_comp_id;
        integer record_time;
        wait (dl_reset == 1);
        cycle_id = 1;
        record_time = 0;
        while (1) begin
            @ (negedge dl_clock);
            case (CS_fsm)
                ST_DL_DETECTED: begin
                    cycle_comp_id = 2;
                    if (dl_detect_reg != dl_done_reg) begin
                        if (dl_done_reg == 'b0) begin
                            print_dl_head;
                            record_time = $time;
                        end
                        print_cycle_start(proc_path(origin), cycle_id);
                        cycle_id = cycle_id + 1;
                    end
                    else begin
                        print_dl_end((cycle_id - 1),record_time);
                        find_df_deadlock = 1;
                        @(negedge dl_clock);
                        $finish;
                    end
                end
                ST_DL_REPORT: begin
                    if ((|(dl_in_vec)) & ~(|(dl_in_vec & origin_reg))) begin
                        print_cycle_chan_comp(dl_in_vec_reg, dl_in_vec);
                        print_cycle_proc_comp(proc_path(dl_in_vec), cycle_comp_id);
                        cycle_comp_id = cycle_comp_id + 1;
                    end
                    else begin
                        print_cycle_chan_comp(dl_in_vec_reg, dl_in_vec);
                    end
                end
            endcase
        end
    end
 
