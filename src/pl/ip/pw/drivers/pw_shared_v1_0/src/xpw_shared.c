// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xpw_shared.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XPw_shared_CfgInitialize(XPw_shared *InstancePtr, XPw_shared_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XPw_shared_Start(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_AP_CTRL) & 0x80;
    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XPw_shared_IsDone(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XPw_shared_IsIdle(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XPw_shared_IsReady(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XPw_shared_EnableAutoRestart(XPw_shared *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XPw_shared_DisableAutoRestart(XPw_shared *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_AP_CTRL, 0);
}

u32 XPw_shared_Get_return(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_AP_RETURN);
    return Data;
}
void XPw_shared_Set_weights(XPw_shared *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_WEIGHTS_DATA, Data);
}

u32 XPw_shared_Get_weights(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_WEIGHTS_DATA);
    return Data;
}

void XPw_shared_Set_input_r(XPw_shared *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_INPUT_R_DATA, Data);
}

u32 XPw_shared_Get_input_r(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_INPUT_R_DATA);
    return Data;
}

void XPw_shared_Set_output_r(XPw_shared *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_OUTPUT_R_DATA, Data);
}

u32 XPw_shared_Get_output_r(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_OUTPUT_R_DATA);
    return Data;
}

void XPw_shared_Set_ci(XPw_shared *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_CI_DATA, Data);
}

u32 XPw_shared_Get_ci(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_CI_DATA);
    return Data;
}

void XPw_shared_Set_co(XPw_shared *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_CO_DATA, Data);
}

u32 XPw_shared_Get_co(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_CO_DATA);
    return Data;
}

void XPw_shared_Set_length_r(XPw_shared *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_LENGTH_R_DATA, Data);
}

u32 XPw_shared_Get_length_r(XPw_shared *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_LENGTH_R_DATA);
    return Data;
}

void XPw_shared_InterruptGlobalEnable(XPw_shared *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_GIE, 1);
}

void XPw_shared_InterruptGlobalDisable(XPw_shared *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_GIE, 0);
}

void XPw_shared_InterruptEnable(XPw_shared *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_IER);
    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_IER, Register | Mask);
}

void XPw_shared_InterruptDisable(XPw_shared *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_IER);
    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_IER, Register & (~Mask));
}

void XPw_shared_InterruptClear(XPw_shared *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XPw_shared_WriteReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_ISR, Mask);
}

u32 XPw_shared_InterruptGetEnabled(XPw_shared *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_IER);
}

u32 XPw_shared_InterruptGetStatus(XPw_shared *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XPw_shared_ReadReg(InstancePtr->Control_BaseAddress, XPW_SHARED_CONTROL_ADDR_ISR);
}

