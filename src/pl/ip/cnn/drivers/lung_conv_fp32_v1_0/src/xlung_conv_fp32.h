// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XLUNG_CONV_FP32_H
#define XLUNG_CONV_FP32_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xlung_conv_fp32_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u32 Control_BaseAddress;
} XLung_conv_fp32_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XLung_conv_fp32;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XLung_conv_fp32_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XLung_conv_fp32_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XLung_conv_fp32_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XLung_conv_fp32_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XLung_conv_fp32_Initialize(XLung_conv_fp32 *InstancePtr, u16 DeviceId);
XLung_conv_fp32_Config* XLung_conv_fp32_LookupConfig(u16 DeviceId);
int XLung_conv_fp32_CfgInitialize(XLung_conv_fp32 *InstancePtr, XLung_conv_fp32_Config *ConfigPtr);
#else
int XLung_conv_fp32_Initialize(XLung_conv_fp32 *InstancePtr, const char* InstanceName);
int XLung_conv_fp32_Release(XLung_conv_fp32 *InstancePtr);
#endif

void XLung_conv_fp32_Start(XLung_conv_fp32 *InstancePtr);
u32 XLung_conv_fp32_IsDone(XLung_conv_fp32 *InstancePtr);
u32 XLung_conv_fp32_IsIdle(XLung_conv_fp32 *InstancePtr);
u32 XLung_conv_fp32_IsReady(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_EnableAutoRestart(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_DisableAutoRestart(XLung_conv_fp32 *InstancePtr);
u32 XLung_conv_fp32_Get_return(XLung_conv_fp32 *InstancePtr);

void XLung_conv_fp32_Set_input_r(XLung_conv_fp32 *InstancePtr, u64 Data);
u64 XLung_conv_fp32_Get_input_r(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_Set_weight(XLung_conv_fp32 *InstancePtr, u64 Data);
u64 XLung_conv_fp32_Get_weight(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_Set_bias(XLung_conv_fp32 *InstancePtr, u64 Data);
u64 XLung_conv_fp32_Get_bias(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_Set_output_r(XLung_conv_fp32 *InstancePtr, u64 Data);
u64 XLung_conv_fp32_Get_output_r(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_Set_ci(XLung_conv_fp32 *InstancePtr, u32 Data);
u32 XLung_conv_fp32_Get_ci(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_Set_co(XLung_conv_fp32 *InstancePtr, u32 Data);
u32 XLung_conv_fp32_Get_co(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_Set_height(XLung_conv_fp32 *InstancePtr, u32 Data);
u32 XLung_conv_fp32_Get_height(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_Set_width(XLung_conv_fp32 *InstancePtr, u32 Data);
u32 XLung_conv_fp32_Get_width(XLung_conv_fp32 *InstancePtr);

void XLung_conv_fp32_InterruptGlobalEnable(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_InterruptGlobalDisable(XLung_conv_fp32 *InstancePtr);
void XLung_conv_fp32_InterruptEnable(XLung_conv_fp32 *InstancePtr, u32 Mask);
void XLung_conv_fp32_InterruptDisable(XLung_conv_fp32 *InstancePtr, u32 Mask);
void XLung_conv_fp32_InterruptClear(XLung_conv_fp32 *InstancePtr, u32 Mask);
u32 XLung_conv_fp32_InterruptGetEnabled(XLung_conv_fp32 *InstancePtr);
u32 XLung_conv_fp32_InterruptGetStatus(XLung_conv_fp32 *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
