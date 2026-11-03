#pragma once
// Channel-major FP32 tensors, no bias/activation. Bounds match both current models.
enum { PW_MAX_C=96, PW_MAX_N=2048, PW_TILE_N=64 };
extern "C" int pw_shared(const float *weights,const float *input,float *output,
                         int ci,int co,int length);
