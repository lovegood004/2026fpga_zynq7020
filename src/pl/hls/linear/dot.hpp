#pragma once
#include <cstdint>
constexpr int LINEAR_OK=0x4c4d4d31;
constexpr unsigned LINEAR_INPUT=32768,LINEAR_WEIGHT=262144,LINEAR_OUTPUT=32768;
extern "C" int lung_linear_int8(const uint8_t*,const int8_t*,int32_t*,int,int,int,int,int);
