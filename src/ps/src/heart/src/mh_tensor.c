#include "tensor.h"
extern int ref_conv1d_strided(const float*,int,int,const float*,int,int,int,int,int,float*);
extern int ref_conv1d_dw_strided(const float*,int,int,const float*,int,int,int,int,float*);
#include <esp_heap_caps.h>

#pragma GCC optimize ("O3")

float* float_alloc(int n) {
    float *p = (float*)heap_caps_malloc(n * sizeof(float), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!p) p = (float*)malloc(n * sizeof(float));
    return p;
}
void float_free(float *p) { free(p); }

void ref_relu_(float *x, int n) {
    for (int i = 0; i < n; i++)
        if (x[i] < 0) x[i] = 0;
}

void ref_sigmoid_(float *x, int n) {
    for (int i = 0; i < n; i++)
        x[i] = 1.0f / (1.0f + expf(-x[i]));
}

float dot_product(const float *a, const float *b, int n) {
    float s = 0;
    for (int i = 0; i < n; i++) s += a[i] * b[i];
    return s;
}

int ref_conv1d(const float *in, int in_c, int in_len,
           const float *weight, int out_c, int ksize,
           int dilation, int pad, float *out) {
    return ref_conv1d_strided(in, in_c, in_len, weight, out_c, ksize,
                          dilation, pad, 1, out);
}

int ref_conv1d_strided(const float *in, int in_c, int in_len,
                   const float *weight, int out_c, int ksize,
                   int dilation, int pad, int stride, float *out) {
    int out_len = conv_out_len(in_len, ksize, dilation, pad, stride);
    if (out_len < 1) return -1;

    memset(out, 0, out_c * out_len * sizeof(float));

    /* Pointwise convolution dominates both Ghost networks. Reorder it as
     * contiguous SAXPY-style channel accumulation so the LX7 compiler can
     * schedule its single-precision MAC pipeline efficiently. For every
     * output position, channel additions remain in the original order. */
    if (ksize == 1 && dilation == 1 && pad == 0 && stride == 1) {
        for (int oc = 0; oc < out_c; ++oc) {
            float *dst = out + oc * out_len;
            for (int ic = 0; ic < in_c; ++ic) {
                const float w = weight[oc * in_c + ic];
                const float *src = in + ic * in_len;
                for (int pos = 0; pos < out_len; ++pos) dst[pos] += w * src[pos];
            }
        }
        return out_len;
    }

    for (int oc = 0; oc < out_c; oc++) {
        for (int pos = 0; pos < out_len; pos++) {
            float sum = 0.0f;
            for (int ic = 0; ic < in_c; ic++) {
                for (int k = 0; k < ksize; k++) {
                    int idx = pos * stride + k * dilation - pad;
                    if (idx >= 0 && idx < in_len) {
                        int widx = oc * (in_c * ksize) + ic * ksize + k;
                        sum += weight[widx] * in[ic * in_len + idx];
                    }
                }
            }
            out[oc * out_len + pos] = (float)sum;
        }
    }
    return out_len;
}

void conv1d_with_bias(const float *in, int in_c, int in_len,
                      const float *weight, const float *bias,
                      int out_c, int ksize, int dilation, int pad, float *out) {
    int out_len = ref_conv1d(in, in_c, in_len, weight, out_c, ksize, dilation, pad, out);
    if (out_len < 1) return;
    for (int oc = 0; oc < out_c; oc++) {
        float b = bias ? bias[oc] : 0;
        for (int pos = 0; pos < out_len; pos++)
            out[oc * out_len + pos] += b;
    }
}

int ref_conv1d_dw(const float *in, int ch, int in_len,
              const float *weight, int ksize,
              int dilation, int pad, float *out) {
    return ref_conv1d_dw_strided(in, ch, in_len, weight, ksize, dilation, pad, 1, out);
}

int ref_conv1d_dw_strided(const float *in, int ch, int in_len,
                      const float *weight, int ksize,
                      int dilation, int pad, int stride, float *out) {
    int out_len = conv_out_len(in_len, ksize, dilation, pad, stride);
    if (out_len < 1) return -1;

    /* 先清零输出缓冲区，避免残留数据影响 */
    memset(out, 0, (size_t)ch * out_len * sizeof(float));

    for (int c = 0; c < ch; c++) {
        for (int pos = 0; pos < out_len; pos++) {
            float sum = 0.0f;
            for (int k = 0; k < ksize; k++) {
                int idx = pos * stride + k * dilation - pad;
                if (idx >= 0 && idx < in_len)
                    sum += weight[c * ksize + k] * in[c * in_len + idx];
            }
            out[c * out_len + pos] = (float)sum;
        }
    }
    return out_len;
}

void ref_batchnorm1d(float *x, int ch, int len,
                 const float *gamma, const float *beta,
                 const float *mean, const float *var, float eps) {
    for (int c = 0; c < ch; c++) {
        float g = gamma[c], b = beta[c], m = mean[c], v = var[c];
        float denom = 1.0f / sqrtf(v + eps);
        for (int i = 0; i < len; i++)
            x[c * len + i] = g * (x[c * len + i] - m) * denom + b;
    }
}

void ref_linear(const float *in, int in_dim, int out_dim,
            const float *weight, const float *bias, float *out) {
    for (int oc = 0; oc < out_dim; oc++) {
        float s = dot_product(&weight[oc * in_dim], in, in_dim);
        out[oc] = s + (bias ? bias[oc] : 0);
    }
}

int ref_avgpool1d(const float *in, int ch, int in_len,
              int kernel, int stride, float *out) {
    int out_len = pool_out_len(in_len, kernel, stride);
    if (out_len < 1) return -1;

    for (int c = 0; c < ch; c++) {
        for (int pos = 0; pos < out_len; pos++) {
            float s = 0;
            int start = pos * stride;
            for (int k = 0; k < kernel; k++)
                s += in[c * in_len + start + k];
            out[c * out_len + pos] = s / kernel;
        }
    }
    return out_len;
}

void ref_adaptive_avgpool1d_to1(const float *in, int ch, int in_len, float *out) {
    for (int c = 0; c < ch; c++) {
        float s = 0;
        for (int i = 0; i < in_len; i++)
            s += in[c * in_len + i];
        out[c] = s / in_len;
    }
}

int ref_cat_channels(const float *a, int a_ch, int a_len,
                 const float *b, int b_ch, int b_len, float *out) {
    if (a_len != b_len) return -1;
    memcpy(out, a, a_ch * a_len * sizeof(float));
    memcpy(out + a_ch * a_len, b, b_ch * b_len * sizeof(float));
    return a_ch + b_ch;
}
