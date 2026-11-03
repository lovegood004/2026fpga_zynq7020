// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xlung_linear_int8.h"

extern XLung_linear_int8_Config XLung_linear_int8_ConfigTable[];

XLung_linear_int8_Config *XLung_linear_int8_LookupConfig(u16 DeviceId) {
	XLung_linear_int8_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XLUNG_LINEAR_INT8_NUM_INSTANCES; Index++) {
		if (XLung_linear_int8_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XLung_linear_int8_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XLung_linear_int8_Initialize(XLung_linear_int8 *InstancePtr, u16 DeviceId) {
	XLung_linear_int8_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XLung_linear_int8_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XLung_linear_int8_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

