// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XLUNG_LINEAR_INT8_H
#define XLUNG_LINEAR_INT8_H

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
#include "xlung_linear_int8_hw.h"

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
} XLung_linear_int8_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XLung_linear_int8;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XLung_linear_int8_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XLung_linear_int8_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XLung_linear_int8_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XLung_linear_int8_ReadReg(BaseAddress, RegOffset) \
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
int XLung_linear_int8_Initialize(XLung_linear_int8 *InstancePtr, u16 DeviceId);
XLung_linear_int8_Config* XLung_linear_int8_LookupConfig(u16 DeviceId);
int XLung_linear_int8_CfgInitialize(XLung_linear_int8 *InstancePtr, XLung_linear_int8_Config *ConfigPtr);
#else
int XLung_linear_int8_Initialize(XLung_linear_int8 *InstancePtr, const char* InstanceName);
int XLung_linear_int8_Release(XLung_linear_int8 *InstancePtr);
#endif

void XLung_linear_int8_Start(XLung_linear_int8 *InstancePtr);
u32 XLung_linear_int8_IsDone(XLung_linear_int8 *InstancePtr);
u32 XLung_linear_int8_IsIdle(XLung_linear_int8 *InstancePtr);
u32 XLung_linear_int8_IsReady(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_EnableAutoRestart(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_DisableAutoRestart(XLung_linear_int8 *InstancePtr);
u32 XLung_linear_int8_Get_return(XLung_linear_int8 *InstancePtr);

void XLung_linear_int8_Set_x(XLung_linear_int8 *InstancePtr, u64 Data);
u64 XLung_linear_int8_Get_x(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_Set_w(XLung_linear_int8 *InstancePtr, u64 Data);
u64 XLung_linear_int8_Get_w(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_Set_y(XLung_linear_int8 *InstancePtr, u64 Data);
u64 XLung_linear_int8_Get_y(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_Set_rows(XLung_linear_int8 *InstancePtr, u32 Data);
u32 XLung_linear_int8_Get_rows(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_Set_ci(XLung_linear_int8 *InstancePtr, u32 Data);
u32 XLung_linear_int8_Get_ci(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_Set_co(XLung_linear_int8 *InstancePtr, u32 Data);
u32 XLung_linear_int8_Get_co(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_Set_xzero(XLung_linear_int8 *InstancePtr, u32 Data);
u32 XLung_linear_int8_Get_xzero(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_Set_wzero(XLung_linear_int8 *InstancePtr, u32 Data);
u32 XLung_linear_int8_Get_wzero(XLung_linear_int8 *InstancePtr);

void XLung_linear_int8_InterruptGlobalEnable(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_InterruptGlobalDisable(XLung_linear_int8 *InstancePtr);
void XLung_linear_int8_InterruptEnable(XLung_linear_int8 *InstancePtr, u32 Mask);
void XLung_linear_int8_InterruptDisable(XLung_linear_int8 *InstancePtr, u32 Mask);
void XLung_linear_int8_InterruptClear(XLung_linear_int8 *InstancePtr, u32 Mask);
u32 XLung_linear_int8_InterruptGetEnabled(XLung_linear_int8 *InstancePtr);
u32 XLung_linear_int8_InterruptGetStatus(XLung_linear_int8 *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
