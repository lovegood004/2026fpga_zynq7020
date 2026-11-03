// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
// 0x00 : Control signals
//        bit 0  - ap_start (Read/Write/COH)
//        bit 1  - ap_done (Read/COR)
//        bit 2  - ap_idle (Read)
//        bit 3  - ap_ready (Read/COR)
//        bit 7  - auto_restart (Read/Write)
//        others - reserved
// 0x04 : Global Interrupt Enable Register
//        bit 0  - Global Interrupt Enable (Read/Write)
//        others - reserved
// 0x08 : IP Interrupt Enable Register (Read/Write)
//        bit 0  - enable ap_done interrupt (Read/Write)
//        bit 1  - enable ap_ready interrupt (Read/Write)
//        others - reserved
// 0x0c : IP Interrupt Status Register (Read/TOW)
//        bit 0  - ap_done (COR/TOW)
//        bit 1  - ap_ready (COR/TOW)
//        others - reserved
// 0x10 : Data signal of ap_return
//        bit 31~0 - ap_return[31:0] (Read)
// 0x20 : Data signal of input_r
//        bit 31~0 - input_r[31:0] (Read/Write)
// 0x24 : Data signal of input_r
//        bit 31~0 - input_r[63:32] (Read/Write)
// 0x28 : reserved
// 0x30 : Data signal of weight
//        bit 31~0 - weight[31:0] (Read/Write)
// 0x34 : Data signal of weight
//        bit 31~0 - weight[63:32] (Read/Write)
// 0x38 : reserved
// 0x40 : Data signal of bias
//        bit 31~0 - bias[31:0] (Read/Write)
// 0x44 : Data signal of bias
//        bit 31~0 - bias[63:32] (Read/Write)
// 0x48 : reserved
// 0x50 : Data signal of output_r
//        bit 31~0 - output_r[31:0] (Read/Write)
// 0x54 : Data signal of output_r
//        bit 31~0 - output_r[63:32] (Read/Write)
// 0x58 : reserved
// 0x60 : Data signal of ci
//        bit 31~0 - ci[31:0] (Read/Write)
// 0x64 : reserved
// 0x68 : Data signal of co
//        bit 31~0 - co[31:0] (Read/Write)
// 0x6c : reserved
// 0x70 : Data signal of height
//        bit 31~0 - height[31:0] (Read/Write)
// 0x74 : reserved
// 0x78 : Data signal of width
//        bit 31~0 - width[31:0] (Read/Write)
// 0x7c : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XLUNG_CONV_FP32_CONTROL_ADDR_AP_CTRL       0x00
#define XLUNG_CONV_FP32_CONTROL_ADDR_GIE           0x04
#define XLUNG_CONV_FP32_CONTROL_ADDR_IER           0x08
#define XLUNG_CONV_FP32_CONTROL_ADDR_ISR           0x0c
#define XLUNG_CONV_FP32_CONTROL_ADDR_AP_RETURN     0x10
#define XLUNG_CONV_FP32_CONTROL_BITS_AP_RETURN     32
#define XLUNG_CONV_FP32_CONTROL_ADDR_INPUT_R_DATA  0x20
#define XLUNG_CONV_FP32_CONTROL_BITS_INPUT_R_DATA  64
#define XLUNG_CONV_FP32_CONTROL_ADDR_WEIGHT_DATA   0x30
#define XLUNG_CONV_FP32_CONTROL_BITS_WEIGHT_DATA   64
#define XLUNG_CONV_FP32_CONTROL_ADDR_BIAS_DATA     0x40
#define XLUNG_CONV_FP32_CONTROL_BITS_BIAS_DATA     64
#define XLUNG_CONV_FP32_CONTROL_ADDR_OUTPUT_R_DATA 0x50
#define XLUNG_CONV_FP32_CONTROL_BITS_OUTPUT_R_DATA 64
#define XLUNG_CONV_FP32_CONTROL_ADDR_CI_DATA       0x60
#define XLUNG_CONV_FP32_CONTROL_BITS_CI_DATA       32
#define XLUNG_CONV_FP32_CONTROL_ADDR_CO_DATA       0x68
#define XLUNG_CONV_FP32_CONTROL_BITS_CO_DATA       32
#define XLUNG_CONV_FP32_CONTROL_ADDR_HEIGHT_DATA   0x70
#define XLUNG_CONV_FP32_CONTROL_BITS_HEIGHT_DATA   32
#define XLUNG_CONV_FP32_CONTROL_ADDR_WIDTH_DATA    0x78
#define XLUNG_CONV_FP32_CONTROL_BITS_WIDTH_DATA    32

