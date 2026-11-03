// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xpw_shared.h"

extern XPw_shared_Config XPw_shared_ConfigTable[];

XPw_shared_Config *XPw_shared_LookupConfig(u16 DeviceId) {
	XPw_shared_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XPW_SHARED_NUM_INSTANCES; Index++) {
		if (XPw_shared_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XPw_shared_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XPw_shared_Initialize(XPw_shared *InstancePtr, u16 DeviceId) {
	XPw_shared_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XPw_shared_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XPw_shared_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

