// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XPW_SHARED_H
#define XPW_SHARED_H

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
#include "xpw_shared_hw.h"

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
} XPw_shared_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XPw_shared;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XPw_shared_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XPw_shared_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XPw_shared_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XPw_shared_ReadReg(BaseAddress, RegOffset) \
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
int XPw_shared_Initialize(XPw_shared *InstancePtr, u16 DeviceId);
XPw_shared_Config* XPw_shared_LookupConfig(u16 DeviceId);
int XPw_shared_CfgInitialize(XPw_shared *InstancePtr, XPw_shared_Config *ConfigPtr);
#else
int XPw_shared_Initialize(XPw_shared *InstancePtr, const char* InstanceName);
int XPw_shared_Release(XPw_shared *InstancePtr);
#endif

void XPw_shared_Start(XPw_shared *InstancePtr);
u32 XPw_shared_IsDone(XPw_shared *InstancePtr);
u32 XPw_shared_IsIdle(XPw_shared *InstancePtr);
u32 XPw_shared_IsReady(XPw_shared *InstancePtr);
void XPw_shared_EnableAutoRestart(XPw_shared *InstancePtr);
void XPw_shared_DisableAutoRestart(XPw_shared *InstancePtr);
u32 XPw_shared_Get_return(XPw_shared *InstancePtr);

void XPw_shared_Set_weights(XPw_shared *InstancePtr, u32 Data);
u32 XPw_shared_Get_weights(XPw_shared *InstancePtr);
void XPw_shared_Set_input_r(XPw_shared *InstancePtr, u32 Data);
u32 XPw_shared_Get_input_r(XPw_shared *InstancePtr);
void XPw_shared_Set_output_r(XPw_shared *InstancePtr, u32 Data);
u32 XPw_shared_Get_output_r(XPw_shared *InstancePtr);
void XPw_shared_Set_ci(XPw_shared *InstancePtr, u32 Data);
u32 XPw_shared_Get_ci(XPw_shared *InstancePtr);
void XPw_shared_Set_co(XPw_shared *InstancePtr, u32 Data);
u32 XPw_shared_Get_co(XPw_shared *InstancePtr);
void XPw_shared_Set_length_r(XPw_shared *InstancePtr, u32 Data);
u32 XPw_shared_Get_length_r(XPw_shared *InstancePtr);

void XPw_shared_InterruptGlobalEnable(XPw_shared *InstancePtr);
void XPw_shared_InterruptGlobalDisable(XPw_shared *InstancePtr);
void XPw_shared_InterruptEnable(XPw_shared *InstancePtr, u32 Mask);
void XPw_shared_InterruptDisable(XPw_shared *InstancePtr, u32 Mask);
void XPw_shared_InterruptClear(XPw_shared *InstancePtr, u32 Mask);
u32 XPw_shared_InterruptGetEnabled(XPw_shared *InstancePtr);
u32 XPw_shared_InterruptGetStatus(XPw_shared *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
