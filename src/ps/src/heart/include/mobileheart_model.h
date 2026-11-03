#ifndef GHOSTNET_H_
#define GHOSTNET_H_
#include "tensor.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========== GhostMobile (39,315 params) ========== */
/* Input: [1, 4096] float32 (mono audio, 1000Hz, 5 heartbeats) */
/* Output: [5] float32 (sigmoid logits: AS, AR, MR, MS, Normal) */
/* Returns 1 on success, 0 when the inference workspace cannot be allocated. */
int ghost_mobile_forward(const float *input, float *output);

/* ========== GhostTCN (32,766 params) ========== */
int ghost_tcn_forward(const float *input, float *output);

/* ========== 辅助函数 ========== */
/* Stem: Conv1d + BN + ReLU */
void stem_forward(const float *in, int in_c, int in_len,
                  int out_c, int ksize, int stride,
                  const float *w_conv,          /* [out_c, in_c, ksize] */
                  const float *bn_w, const float *bn_b,
                  const float *bn_mean, const float *bn_var,
                  float *out, int *out_len);

/* DSConv (普通深度可分离) */
int dsconv_forward(const float *in, int in_c, int in_len,
                   int out_c, int ksize, int dilation, int stride,
                   const float *dw_w, const float *pw_w,
                   const float *bn_w, const float *bn_b,
                   const float *bn_mean, const float *bn_var,
                   const float *se_w1, const float *se_b1,
                   const float *se_w3, const float *se_b3,
                   float *buf, /* workspace, size >= max(out_c*out_len, in_c*out_len_se) */
                   float *out, int *out_len);

/* GhostDSConv (鬼影深度可分离) */
int ghostdsconv_forward(const float *in, int in_c, int in_len,
                        int out_c, int ksize, int dilation, int stride,
                        const float *dw_w,
                        const float *pw_primary_w, const float *pw_cheap_w,
                        const float *bn_w, const float *bn_b,
                        const float *bn_mean, const float *bn_var,
                        const float *se_w1, const float *se_b1,
                        const float *se_w3, const float *se_b3,
                        float *buf, float *out, int *out_len);

/* TCN 残差块 */
int tcn_resblock_forward(const float *in, int c, int len,
                         int dilation,
                         const float *dw1_w, const float *pw1_w,
                         const float *bn1_w, const float *bn1_b,
                         const float *bn1_mean, const float *bn1_var,
                         const float *dw2_w, const float *pw2_w,
                         const float *bn2_w, const float *bn2_b,
                         const float *bn2_mean, const float *bn2_var,
                         const float *ds_w,
                         float *buf, float *out, int *out_len);

#ifdef __cplusplus
}
#endif
#endif /* GHOSTNET_H_ */
