// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xlung_linear_int8.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XLung_linear_int8_CfgInitialize(XLung_linear_int8 *InstancePtr, XLung_linear_int8_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XLung_linear_int8_Start(XLung_linear_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_AP_CTRL) & 0x80;
    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XLung_linear_int8_IsDone(XLung_linear_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XLung_linear_int8_IsIdle(XLung_linear_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XLung_linear_int8_IsReady(XLung_linear_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XLung_linear_int8_EnableAutoRestart(XLung_linear_int8 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XLung_linear_int8_DisableAutoRestart(XLung_linear_int8 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_AP_CTRL, 0);
}

u32 XLung_linear_int8_Get_return(XLung_linear_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_AP_RETURN);
    return Data;
}
void XLung_linear_int8_Set_x(XLung_linear_int8 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_X_DATA, (u32)(Data));
    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_X_DATA + 4, (u32)(Data >> 32));
}

u64 XLung_linear_int8_Get_x(XLung_linear_int8 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_X_DATA);
    Data += (u64)XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_X_DATA + 4) << 32;
    return Data;
}

void XLung_linear_int8_Set_w(XLung_linear_int8 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_W_DATA, (u32)(Data));
    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_W_DATA + 4, (u32)(Data >> 32));
}

u64 XLung_linear_int8_Get_w(XLung_linear_int8 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_W_DATA);
    Data += (u64)XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_W_DATA + 4) << 32;
    return Data;
}

void XLung_linear_int8_Set_y(XLung_linear_int8 *InstancePtr, u64 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_Y_DATA, (u32)(Data));
    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_Y_DATA + 4, (u32)(Data >> 32));
}

u64 XLung_linear_int8_Get_y(XLung_linear_int8 *InstancePtr) {
    u64 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_Y_DATA);
    Data += (u64)XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_Y_DATA + 4) << 32;
    return Data;
}

void XLung_linear_int8_Set_rows(XLung_linear_int8 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_ROWS_DATA, Data);
}

u32 XLung_linear_int8_Get_rows(XLung_linear_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_ROWS_DATA);
    return Data;
}

void XLung_linear_int8_Set_ci(XLung_linear_int8 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_CI_DATA, Data);
}

u32 XLung_linear_int8_Get_ci(XLung_linear_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_CI_DATA);
    return Data;
}

void XLung_linear_int8_Set_co(XLung_linear_int8 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_CO_DATA, Data);
}

u32 XLung_linear_int8_Get_co(XLung_linear_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_CO_DATA);
    return Data;
}

void XLung_linear_int8_Set_xzero(XLung_linear_int8 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_XZERO_DATA, Data);
}

u32 XLung_linear_int8_Get_xzero(XLung_linear_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_XZERO_DATA);
    return Data;
}

void XLung_linear_int8_Set_wzero(XLung_linear_int8 *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_WZERO_DATA, Data);
}

u32 XLung_linear_int8_Get_wzero(XLung_linear_int8 *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_WZERO_DATA);
    return Data;
}

void XLung_linear_int8_InterruptGlobalEnable(XLung_linear_int8 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_GIE, 1);
}

void XLung_linear_int8_InterruptGlobalDisable(XLung_linear_int8 *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_GIE, 0);
}

void XLung_linear_int8_InterruptEnable(XLung_linear_int8 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_IER);
    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_IER, Register | Mask);
}

void XLung_linear_int8_InterruptDisable(XLung_linear_int8 *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_IER);
    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_IER, Register & (~Mask));
}

void XLung_linear_int8_InterruptClear(XLung_linear_int8 *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XLung_linear_int8_WriteReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_ISR, Mask);
}

u32 XLung_linear_int8_InterruptGetEnabled(XLung_linear_int8 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_IER);
}

u32 XLung_linear_int8_InterruptGetStatus(XLung_linear_int8 *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XLung_linear_int8_ReadReg(InstancePtr->Control_BaseAddress, XLUNG_LINEAR_INT8_CONTROL_ADDR_ISR);
}

