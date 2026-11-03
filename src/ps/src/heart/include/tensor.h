#ifndef TENSOR_H_
#define TENSOR_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 1D 卷积参数 */
typedef struct {
    int in_c, in_len;
    int out_c;
    int ksize, dilation, pad, stride;
    const float *weight, *bias;
} Conv1DParams;

/* 批归一化参数 */
typedef struct {
    int channels;
    const float *gamma, *beta, *mean, *var;
    float eps;
} BNParams;

/* ===== 张量操作 ===== */
void relu_(float *x, int n);
void sigmoid_(float *x, int n);
float dot_product(const float *a, const float *b, int n);

/* 1D 卷积 (普通, weight shape: [out_c, in_c, ksize]) */
int conv1d(const float *in, int in_c, int in_len,
           const float *weight, int out_c, int ksize,
           int dilation, int pad, float *out);
int conv1d_strided(const float *in, int in_c, int in_len,
                   const float *weight, int out_c, int ksize,
                   int dilation, int pad, int stride, float *out);
void conv1d_with_bias(const float *in, int in_c, int in_len,
                      const float *weight, const float *bias,
                      int out_c, int ksize, int dilation, int pad, float *out);

/* 深度可分离 Conv1d (weight shape: [ch, 1, ksize]) */
int conv1d_dw(const float *in, int ch, int in_len,
              const float *weight, int ksize,
              int dilation, int pad, float *out);
int conv1d_dw_strided(const float *in, int ch, int in_len,
                      const float *weight, int ksize,
                      int dilation, int pad, int stride, float *out);

/* BN: in-place, x shape [ch, len] */
void batchnorm1d(float *x, int ch, int len,
                 const float *gamma, const float *beta,
                 const float *mean, const float *var, float eps);

/* 全连接 + bias */
void linear(const float *in, int in_dim, int out_dim,
            const float *weight, const float *bias, float *out);

/* AvgPool1d */
int avgpool1d(const float *in, int ch, int in_len,
              int kernel, int stride, float *out);

/* AdaptiveAvgPool1d -> 1 */
void adaptive_avgpool1d_to1(const float *in, int ch, int in_len, float *out);

/* 沿通道维拼接: a_ch + b_ch, 要求 a_len == b_len */
int cat_channels(const float *a, int a_ch, int a_len,
                 const float *b, int b_ch, int b_len, float *out);

/* 计算卷积输出长度 */
static inline int conv_out_len(int in_len, int ksize, int dilation, int pad, int stride) {
    return (in_len + 2 * pad - dilation * (ksize - 1) - 1) / stride + 1;
}
static inline int pool_out_len(int in_len, int kernel, int stride) {
    return (in_len - kernel) / stride + 1;
}

/* 安全 alloc/free */
float* float_alloc(int n);
void float_free(float *p);

#ifdef __cplusplus
}
#endif
#endif /* TENSOR_H_ */
