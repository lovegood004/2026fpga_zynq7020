// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xlung_conv_fp32.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XLung_conv_fp32_CfgInitialize(XLung_conv_fp32 *InstancePtr, XLung_conv_fp32_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XLung_conv_fp32_Start(XLung_conv_fp32 *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_AP_CTRL) & 0x80;
    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XLung_conv_fp32_IsDone(XLung_conv_fp32 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XLung_conv_fp32_IsIdle(XLung_conv_fp32 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XLung_conv_fp32_IsReady(XLung_conv_fp32 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XLung_conv_fp32_EnableAutoRestart(XLung_conv_fp32 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XLung_conv_fp32_DisableAutoRestart(XLung_conv_fp32 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_AP_CTRL, 0);
}

u32 XLung_conv_fp32_Get_return(XLung_conv_fp32 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_AP_RETURN);
    return Data;
}
void XLung_conv_fp32_Set_input_r(XLung_conv_fp32 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_INPUT_R_DATA, (u32)(Data));
    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_INPUT_R_DATA + 4, (u32)(Data >> 32));
}

u64 XLung_conv_fp32_Get_input_r(XLung_conv_fp32 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_INPUT_R_DATA);
    Data += (u64)XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_INPUT_R_DATA + 4) << 32;
    return Data;
}

void XLung_conv_fp32_Set_weight(XLung_conv_fp32 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_WEIGHT_DATA, (u32)(Data));
    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_WEIGHT_DATA + 4, (u32)(Data >> 32));
}

u64 XLung_conv_fp32_Get_weight(XLung_conv_fp32 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_WEIGHT_DATA);
    Data += (u64)XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_WEIGHT_DATA + 4) << 32;
    return Data;
}

void XLung_conv_fp32_Set_bias(XLung_conv_fp32 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_BIAS_DATA, (u32)(Data));
    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_BIAS_DATA + 4, (u32)(Data >> 32));
}

u64 XLung_conv_fp32_Get_bias(XLung_conv_fp32 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_BIAS_DATA);
    Data += (u64)XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_BIAS_DATA + 4) << 32;
    return Data;
}

void XLung_conv_fp32_Set_output_r(XLung_conv_fp32 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_OUTPUT_R_DATA, (u32)(Data));
    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_OUTPUT_R_DATA + 4, (u32)(Data >> 32));
}

u64 XLung_conv_fp32_Get_output_r(XLung_conv_fp32 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_OUTPUT_R_DATA);
    Data += (u64)XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_OUTPUT_R_DATA + 4) << 32;
    return Data;
}

void XLung_conv_fp32_Set_ci(XLung_conv_fp32 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_CI_DATA, Data);
}

u32 XLung_conv_fp32_Get_ci(XLung_conv_fp32 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_CI_DATA);
    return Data;
}

void XLung_conv_fp32_Set_co(XLung_conv_fp32 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_CO_DATA, Data);
}

u32 XLung_conv_fp32_Get_co(XLung_conv_fp32 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_CO_DATA);
    return Data;
}

void XLung_conv_fp32_Set_height(XLung_conv_fp32 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_HEIGHT_DATA, Data);
}

u32 XLung_conv_fp32_Get_height(XLung_conv_fp32 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_HEIGHT_DATA);
    return Data;
}

void XLung_conv_fp32_Set_width(XLung_conv_fp32 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_WIDTH_DATA, Data);
}

u32 XLung_conv_fp32_Get_width(XLung_conv_fp32 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_WIDTH_DATA);
    return Data;
}

void XLung_conv_fp32_InterruptGlobalEnable(XLung_conv_fp32 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_GIE, 1);
}

void XLung_conv_fp32_InterruptGlobalDisable(XLung_conv_fp32 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_GIE, 0);
}

void XLung_conv_fp32_InterruptEnable(XLung_conv_fp32 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_IER);
    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_IER, Register | Mask);
}

void XLung_conv_fp32_InterruptDisable(XLung_conv_fp32 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_IER);
    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_IER, Register & (~Mask));
}

void XLung_conv_fp32_InterruptClear(XLung_conv_fp32 *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_conv_fp32_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_ISR, Mask);
}

u32 XLung_conv_fp32_InterruptGetEnabled(XLung_conv_fp32 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_IER);
}

u32 XLung_conv_fp32_InterruptGetStatus(XLung_conv_fp32 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLung_conv_fp32_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_CONV_FP32_CONTROL_ADDR_ISR);
}

