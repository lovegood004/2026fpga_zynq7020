// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
// Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xlung_conv_fp32.h"

extern XLung_conv_fp32_Config XLung_conv_fp32_ConfigTable[];

XLung_conv_fp32_Config *XLung_conv_fp32_LookupConfig(u16 DeviceId) {
	XLung_conv_fp32_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XLUNG_CONV_FP32_NUM_INSTANCES; Index++) {
		if (XLung_conv_fp32_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XLung_conv_fp32_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XLung_conv_fp32_Initialize(XLung_conv_fp32 *InstancePtr, u16 DeviceId) {
	XLung_conv_fp32_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XLung_conv_fp32_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XLung_conv_fp32_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

