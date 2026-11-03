#pragma once
// CHW, 3x3, stride 2, zero padding 1. Bias is added AFTER the ordered sum.
constexpr int LUNG_CONV_INPUT=479232,LUNG_CONV_WEIGHTS=294912,LUNG_CONV_ROW=8192;
constexpr int LUNG_CONV_OK=0x4c434e31;
extern "C" int lung_conv_fp32(const float*,const float*,const float*,float*,int,int,int,int);
