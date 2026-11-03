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
// 0x20 : Data signal of x
//        bit 31~0 - x[31:0] (Read/Write)
// 0x24 : Data signal of x
//        bit 31~0 - x[63:32] (Read/Write)
// 0x28 : reserved
// 0x30 : Data signal of w
//        bit 31~0 - w[31:0] (Read/Write)
// 0x34 : Data signal of w
//        bit 31~0 - w[63:32] (Read/Write)
// 0x38 : reserved
// 0x40 : Data signal of y
//        bit 31~0 - y[31:0] (Read/Write)
// 0x44 : Data signal of y
//        bit 31~0 - y[63:32] (Read/Write)
// 0x48 : reserved
// 0x50 : Data signal of rows
//        bit 31~0 - rows[31:0] (Read/Write)
// 0x54 : reserved
// 0x58 : Data signal of ci
//        bit 31~0 - ci[31:0] (Read/Write)
// 0x5c : reserved
// 0x60 : Data signal of co
//        bit 31~0 - co[31:0] (Read/Write)
// 0x64 : reserved
// 0x68 : Data signal of xzero
//        bit 31~0 - xzero[31:0] (Read/Write)
// 0x6c : reserved
// 0x70 : Data signal of wzero
//        bit 31~0 - wzero[31:0] (Read/Write)
// 0x74 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XLUNG_LINEAR_INT8_CONTROL_ADDR_AP_CTRL    0x00
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_GIE        0x04
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_IER        0x08
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_ISR        0x0c
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_AP_RETURN  0x10
#define XLUNG_LINEAR_INT8_CONTROL_BITS_AP_RETURN  32
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_X_DATA     0x20
#define XLUNG_LINEAR_INT8_CONTROL_BITS_X_DATA     64
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_W_DATA     0x30
#define XLUNG_LINEAR_INT8_CONTROL_BITS_W_DATA     64
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_Y_DATA     0x40
#define XLUNG_LINEAR_INT8_CONTROL_BITS_Y_DATA     64
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_ROWS_DATA  0x50
#define XLUNG_LINEAR_INT8_CONTROL_BITS_ROWS_DATA  32
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_CI_DATA    0x58
#define XLUNG_LINEAR_INT8_CONTROL_BITS_CI_DATA    32
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_CO_DATA    0x60
#define XLUNG_LINEAR_INT8_CONTROL_BITS_CO_DATA    32
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_XZERO_DATA 0x68
#define XLUNG_LINEAR_INT8_CONTROL_BITS_XZERO_DATA 32
#define XLUNG_LINEAR_INT8_CONTROL_ADDR_WZERO_DATA 0x70
#define XLUNG_LINEAR_INT8_CONTROL_BITS_WZERO_DATA 32

