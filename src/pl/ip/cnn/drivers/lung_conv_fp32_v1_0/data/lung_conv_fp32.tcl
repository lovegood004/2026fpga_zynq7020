# ==============================================================
# Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2021.1 (64-bit)
# Copyright 1986-2021 Xilinx, Inc. All Rights Reserved.
# ==============================================================
proc generate {drv_handle} {
    xdefine_include_file $drv_handle "xparameters.h" "XLung_conv_fp32" \
        "NUM_INSTANCES" \
        "DEVICE_ID" \
        "C_S_AXI_CONTROL_BASEADDR" \
        "C_S_AXI_CONTROL_HIGHADDR"

    xdefine_config_file $drv_handle "xlung_conv_fp32_g.c" "XLung_conv_fp32" \
        "DEVICE_ID" \
        "C_S_AXI_CONTROL_BASEADDR"

    xdefine_canonical_xpars $drv_handle "xparameters.h" "XLung_conv_fp32" \
        "DEVICE_ID" \
        "C_S_AXI_CONTROL_BASEADDR" \
        "C_S_AXI_CONTROL_HIGHADDR"
}

