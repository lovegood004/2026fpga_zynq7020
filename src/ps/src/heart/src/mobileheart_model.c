#include "mobileheart_model.h"
#include "weights_ghost_mobile.h"
#include "weights_ghost_tcn.h"
#include <esp_heap_caps.h>

/* Optimize the arithmetic-heavy inference kernels; fast-math remains disabled. */
#pragma GCC optimize ("O3")

static void *mh_alloc(size_t bytes, int clear) {
    void *p = clear
        ? heap_caps_calloc(1, bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT)
        : heap_caps_malloc(bytes, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!p) {
        p = clear ? calloc(1, bytes) : malloc(bytes);
    }
    return p;
}

/* Both networks execute sequentially and share these persistent work buffers. */
static float *g_mh_buf0, *g_mh_buf1, *g_mh_buf2, *g_mh_buf3;
static float *g_mh_buf4, *g_mh_buf5, *g_mh_work;

static void mh_release_workspace(void) {
    free(g_mh_buf0); g_mh_buf0 = NULL;
    free(g_mh_buf1); g_mh_buf1 = NULL;
    free(g_mh_buf2); g_mh_buf2 = NULL;
    free(g_mh_buf3); g_mh_buf3 = NULL;
    free(g_mh_buf4); g_mh_buf4 = NULL;
    free(g_mh_buf5); g_mh_buf5 = NULL;
    free(g_mh_work); g_mh_work = NULL;
}

static int mh_ensure_workspace(void) {
    if (g_mh_buf0 && g_mh_buf1 && g_mh_buf2 && g_mh_buf3 &&
        g_mh_buf4 && g_mh_buf5 && g_mh_work) return 1;
    mh_release_workspace();
    g_mh_buf0 = (float*)mh_alloc( 49152 * sizeof(float), 0);
    g_mh_buf1 = (float*)mh_alloc( 81920 * sizeof(float), 0);
    g_mh_buf2 = (float*)mh_alloc( 81920 * sizeof(float), 0);
    g_mh_buf3 = (float*)mh_alloc( 81920 * sizeof(float), 0);
    g_mh_buf4 = (float*)mh_alloc( 81920 * sizeof(float), 0);
    g_mh_buf5 = (float*)mh_alloc( 81920 * sizeof(float), 0);
    g_mh_work = (float*)mh_alloc(196608 * sizeof(float), 0);
    if (!g_mh_buf0 || !g_mh_buf1 || !g_mh_buf2 || !g_mh_buf3 ||
        !g_mh_buf4 || !g_mh_buf5 || !g_mh_work) {
        mh_release_workspace();
        return 0;
    }
    return 1;
}

/* ========== 辅助工具 ========== */
static inline int maxi(int a, int b) { return a > b ? a : b; }

/* SEBlock 前向: 输出gate(sigmoid)到buf[0..ch-1], 不乘回输入(SFM内部做) */
static void seblock_forward(const float *in, int ch, int len,
                            const float *w1, const float *b1,
                            const float *w3, const float *b3,
                            float *buf) {
    int r = maxi(1, ch / 4);
    for (int c = 0; c < ch; c++) {
        float s = 0;
        for (int i = 0; i < len; i++) s += in[c * len + i];
        buf[c] = s / len;
    }
    for (int oc = 0; oc < r; oc++) {
        float s = b1 ? b1[oc] : 0;
        for (int ic = 0; ic < ch; ic++)
            s += w1[oc * ch + ic] * buf[ic];
        buf[ch + oc] = s > 0 ? s : 0;
    }
    for (int oc = 0; oc < ch; oc++) {
        float s = b3 ? b3[oc] : 0;
        for (int ic = 0; ic < r; ic++)
            s += w3[oc * r + ic] * buf[ch + ic];
        buf[oc] = 1.0f / (1.0f + expf(-s));
    }
}

/* SFM - 匹配Python: blur + (x-blur)*s, blur=AvgPool1d(3,1,1)零填充 */
static void sfm_forward(const float *in, int ch, int len, const float *gate,
                        float *out) {
    for (int c = 0; c < ch; c++) {
        for (int i = 0; i < len; i++) {
            float xv = in[c * len + i];
            float l = (i > 0) ? in[c * len + i - 1] : 0.0f;
            float r = (i < len - 1) ? in[c * len + i + 1] : 0.0f;
            float blur = (l + xv + r) * (1.0f / 3.0f);
            float s = xv * gate[c];
            out[c * len + i] = blur + (xv - blur) * s;
        }
    }
}

/* Stem: Conv1d + BN + ReLU */
void stem_forward(const float *in, int in_c, int in_len,
                  int out_c, int ksize, int stride,
                  const float *w_conv,
                  const float *bn_w, const float *bn_b,
                  const float *bn_mean, const float *bn_var,
                  float *out, int *out_len) {
    int conv_len = conv_out_len(in_len, ksize, 1, ksize/2, stride);
    conv1d_strided(in, in_c, in_len, w_conv, out_c, ksize, 1, ksize/2, stride, out);
    batchnorm1d(out, out_c, conv_len, bn_w, bn_b, bn_mean, bn_var, 1e-5f);
    relu_(out, out_c * conv_len);
    *out_len = conv_len;
}

/* DSConv 前向 */
int dsconv_forward(const float *in, int in_c, int in_len,
                   int out_c, int ksize, int dilation, int stride,
                   const float *dw_w, const float *pw_w,
                   const float *bn_w, const float *bn_b,
                   const float *bn_mean, const float *bn_var,
                   const float *se_w1, const float *se_b1,
                   const float *se_w3, const float *se_b3,
                   float *buf, float *out, int *out_len) {
    int pad = (ksize - 1) * dilation / 2;
    /* DW Conv: in -> buf (DW output has in_c channels) */
    int dw_len = conv1d_dw(in, in_c, in_len, dw_w, ksize, dilation, pad, buf);
    if (dw_len < 1) return -1;
    /* PW Conv: buf(in_c) -> out(out_c), k=1 */
    int pw_len = conv1d(buf, in_c, dw_len, pw_w, out_c, 1, 1, 0, out);
    /* BN + ReLU */
    batchnorm1d(out, out_c, pw_len, bn_w, bn_b, bn_mean, bn_var, 1e-5f);
    relu_(out, out_c * pw_len);
    /* SE: gate到buf, sfm输出到buf+out_c避免重叠 */
    seblock_forward(out, out_c, pw_len, se_w1, se_b1, se_w3, se_b3, buf);
    sfm_forward(out, out_c, pw_len, buf, buf + out_c);
    *out_len = avgpool1d(buf + out_c, out_c, pw_len, stride, stride, out);
    return *out_len;
}

/* GhostDSConv 前向 */
int ghostdsconv_forward(const float *in, int in_c, int in_len,
                        int out_c, int ksize, int dilation, int stride,
                        const float *dw_w,
                        const float *pw_primary_w, const float *pw_cheap_w,
                        const float *bn_w, const float *bn_b,
                        const float *bn_mean, const float *bn_var,
                        const float *se_w1, const float *se_b1,
                        const float *se_w3, const float *se_b3,
                        float *buf, float *out, int *out_len) {
    int pad = (ksize - 1) * dilation / 2;
    int half = out_c / 2;
    /* DW: in -> buf (in_c channels) */
    int dw_len = conv1d_dw(in, in_c, in_len, dw_w, ksize, dilation, pad, buf);
    if (dw_len < 1) return -1;
    /* PW primary: buf(in_c) -> out(half), k=1 */
    int p_len = conv1d(buf, in_c, dw_len, pw_primary_w, half, 1, 1, 0, out);
    /* 保存primary预ReLU值到work尾部（长度p_len==c_len, k=1 always） */
    float *primary_save = buf + half * p_len;
    memcpy(primary_save, out, half * p_len * sizeof(float));
    /* cheap path: ReLU(out) -> conv1d(half->half, k=1) -> buf (前半部分) */
    relu_(out, half * p_len);
    int c_len = conv1d(out, half, p_len, pw_cheap_w, half, 1, 1, 0, buf);
    if (c_len < 1) return -1;
    /* Concat: [primary_save(pre-ReLU) | buf(cheap)] -> out */
    int cat_ch = cat_channels(primary_save, half, p_len, buf, half, c_len, out);
    if (cat_ch < 1) return -1;
    /* BN + ReLU */
    batchnorm1d(out, cat_ch, p_len, bn_w, bn_b, bn_mean, bn_var, 1e-5f);
    relu_(out, cat_ch * p_len);
    /* SE: gate存buf+half*c_len, sfm输出到buf避免重叠 */
    float *gate = buf + half * c_len;
    float *sfm_out = buf + half * c_len + cat_ch;
    seblock_forward(out, cat_ch, p_len, se_w1, se_b1, se_w3, se_b3, gate);
    sfm_forward(out, cat_ch, p_len, gate, sfm_out);
    *out_len = avgpool1d(sfm_out, cat_ch, p_len, stride, stride, out);
    return *out_len;
}

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
                         float *buf, float *out, int *out_len) {
    int pad = (3 - 1) * dilation;
    /* Path 1 */
    conv1d_dw(in, c, len, dw1_w, 3, dilation, pad, buf);
    int l1 = conv_out_len(len, 3, dilation, pad, 1);
    conv1d(buf, c, l1, pw1_w, c, 1, 1, 0, out);
    batchnorm1d(out, c, l1, bn1_w, bn1_b, bn1_mean, bn1_var, 1e-5f);
    relu_(out, c * l1);
    /* Path 2 */
    conv1d_dw(out, c, l1, dw2_w, 3, dilation, pad, buf);
    int l2 = conv_out_len(l1, 3, dilation, pad, 1);
    conv1d(buf, c, l2, pw2_w, c, 1, 1, 0, out);
    batchnorm1d(out, c, l2, bn2_w, bn2_b, bn2_mean, bn2_var, 1e-5f);
    /* Shortcut */
    float *shortcut = buf;
    int sl = conv1d(in, c, len, ds_w, c, 1, 1, 0, shortcut);
    /* Compact bn2 from stride l2(=516) to stride len(=512) before adding */
    /* Compact bn2 from stride l2 to stride len, then add + ReLU */
    for (int ch = 0; ch < c; ch++)
        for (int i = 0; i < len; i++)
            out[ch * len + i] = out[ch * l2 + i];
    int min_len = l2 < sl ? l2 : sl;
    for (int i = 0; i < c * min_len; i++)
        out[i] += shortcut[i];
    relu_(out, c * min_len);
    *out_len = min_len;
    return *out_len;
}

/* ========== GhostMobile 前向 ========== */
int ghost_mobile_forward(const float *input, float *output) {
    /* 中间缓冲区: 每个输出buffer = out_c * intermediate_len (pool前长度) */
    if (!mh_ensure_workspace()) {
        memset(output, 0, 5 * sizeof(float));
        return 0;
    }
    float *buf_stem = g_mh_buf0;
    float *buf_s1   = g_mh_buf1;
    float *buf_s2   = g_mh_buf2;
    float *buf_s3   = g_mh_buf3;
    float *buf_s4   = g_mh_buf4;
    float *buf_s5   = g_mh_buf5;
    float *work     = g_mh_work;
    int len;

    stem_forward(input, 1, 4096, 24, 15, 2,
                 ghost_mobile_stem_0_weight, ghost_mobile_stem_1_weight,
                 ghost_mobile_stem_1_bias, ghost_mobile_stem_1_running_mean,
                 ghost_mobile_stem_1_running_var,
                 buf_stem, &len);

    dsconv_forward(buf_stem, 24, len, 40, 7, 1, 2,
        ghost_mobile_s1_dw_weight, ghost_mobile_s1_pw_weight,
        ghost_mobile_s1_bn_weight, ghost_mobile_s1_bn_bias,
        ghost_mobile_s1_bn_running_mean, ghost_mobile_s1_bn_running_var,
        ghost_mobile_s1_se_se_1_weight, ghost_mobile_s1_se_se_1_bias,
        ghost_mobile_s1_se_se_3_weight, ghost_mobile_s1_se_se_3_bias,
        work, buf_s1, &len);

    ghostdsconv_forward(buf_s1, 40, len, 64, 7, 2, 2,
        ghost_mobile_s2_dw_weight, ghost_mobile_s2_pw_primary_weight,
        ghost_mobile_s2_pw_cheap_weight,
        ghost_mobile_s2_bn_weight, ghost_mobile_s2_bn_bias,
        ghost_mobile_s2_bn_running_mean, ghost_mobile_s2_bn_running_var,
        ghost_mobile_s2_se_se_1_weight, ghost_mobile_s2_se_se_1_bias,
        ghost_mobile_s2_se_se_3_weight, ghost_mobile_s2_se_se_3_bias,
        work, buf_s2, &len);

    ghostdsconv_forward(buf_s2, 64, len, 80, 5, 4, 2,
        ghost_mobile_s3_dw_weight, ghost_mobile_s3_pw_primary_weight,
        ghost_mobile_s3_pw_cheap_weight,
        ghost_mobile_s3_bn_weight, ghost_mobile_s3_bn_bias,
        ghost_mobile_s3_bn_running_mean, ghost_mobile_s3_bn_running_var,
        ghost_mobile_s3_se_se_1_weight, ghost_mobile_s3_se_se_1_bias,
        ghost_mobile_s3_se_se_3_weight, ghost_mobile_s3_se_se_3_bias,
        work, buf_s3, &len);

    ghostdsconv_forward(buf_s3, 80, len, 96, 5, 4, 2,
        ghost_mobile_s4_dw_weight, ghost_mobile_s4_pw_primary_weight,
        ghost_mobile_s4_pw_cheap_weight,
        ghost_mobile_s4_bn_weight, ghost_mobile_s4_bn_bias,
        ghost_mobile_s4_bn_running_mean, ghost_mobile_s4_bn_running_var,
        ghost_mobile_s4_se_se_1_weight, ghost_mobile_s4_se_se_1_bias,
        ghost_mobile_s4_se_se_3_weight, ghost_mobile_s4_se_se_3_bias,
        work, buf_s4, &len);

    ghostdsconv_forward(buf_s4, 96, len, 96, 3, 8, 2,
        ghost_mobile_s5_dw_weight, ghost_mobile_s5_pw_primary_weight,
        ghost_mobile_s5_pw_cheap_weight,
        ghost_mobile_s5_bn_weight, ghost_mobile_s5_bn_bias,
        ghost_mobile_s5_bn_running_mean, ghost_mobile_s5_bn_running_var,
        ghost_mobile_s5_se_se_1_weight, ghost_mobile_s5_se_se_1_bias,
        ghost_mobile_s5_se_se_3_weight, ghost_mobile_s5_se_se_3_bias,
        work, buf_s5, &len);

    adaptive_avgpool1d_to1(buf_s5, 96, len, work);
    linear(work, 96, 5, ghost_mobile_cls_3_weight, ghost_mobile_cls_3_bias, output);
    sigmoid_(output, 5);

    return 1;
}

/* ========== GhostTCN 前向 ========== */
int ghost_tcn_forward(const float *input, float *output) {
    if (!mh_ensure_workspace()) {
        memset(output, 0, 5 * sizeof(float));
        return 0;
    }
    float *buf_stem = g_mh_buf0;
    float *buf_s1   = g_mh_buf1;
    float *buf_s2   = g_mh_buf2;
    float *buf_s3   = g_mh_buf3;
    float *buf_tcnA = g_mh_buf4;
    float *buf_tcnB = g_mh_buf5;
    float *work     = g_mh_work;
    memset(buf_stem, 0, 20 * 2048 * sizeof(float));
    memset(buf_s1,   0, 36 * 2048 * sizeof(float));
    memset(buf_s2,   0, 40 * 2048 * sizeof(float));
    memset(buf_s3,   0, 40 * 2048 * sizeof(float));
    memset(buf_tcnA, 0, 40 * 2048 * sizeof(float));
    memset(buf_tcnB, 0, 40 * 2048 * sizeof(float));
    memset(work,     0, 60 * 2048 * sizeof(float));
    int len;

    stem_forward(input, 1, 4096, 20, 15, 2,
                 ghost_tcn_stem_0_weight, ghost_tcn_stem_1_weight,
                 ghost_tcn_stem_1_bias, ghost_tcn_stem_1_running_mean,
                 ghost_tcn_stem_1_running_var,
                 buf_stem, &len);

    ghostdsconv_forward(buf_stem, 20, len, 36, 7, 1, 2,
        ghost_tcn_s1_dw_weight, ghost_tcn_s1_pw_primary_weight,
        ghost_tcn_s1_pw_cheap_weight,
        ghost_tcn_s1_bn_weight, ghost_tcn_s1_bn_bias,
        ghost_tcn_s1_bn_running_mean, ghost_tcn_s1_bn_running_var,
        ghost_tcn_s1_se_se_1_weight, ghost_tcn_s1_se_se_1_bias,
        ghost_tcn_s1_se_se_3_weight, ghost_tcn_s1_se_se_3_bias,
        work, buf_s1, &len);

    ghostdsconv_forward(buf_s1, 36, len, 40, 7, 2, 2,
        ghost_tcn_s2_dw_weight, ghost_tcn_s2_pw_primary_weight,
        ghost_tcn_s2_pw_cheap_weight,
        ghost_tcn_s2_bn_weight, ghost_tcn_s2_bn_bias,
        ghost_tcn_s2_bn_running_mean, ghost_tcn_s2_bn_running_var,
        ghost_tcn_s2_se_se_1_weight, ghost_tcn_s2_se_se_1_bias,
        ghost_tcn_s2_se_se_3_weight, ghost_tcn_s2_se_se_3_bias,
        work, buf_s2, &len);

    ghostdsconv_forward(buf_s2, 40, len, 40, 5, 4, 1,
        ghost_tcn_s3_dw_weight, ghost_tcn_s3_pw_primary_weight,
        ghost_tcn_s3_pw_cheap_weight,
        ghost_tcn_s3_bn_weight, ghost_tcn_s3_bn_bias,
        ghost_tcn_s3_bn_running_mean, ghost_tcn_s3_bn_running_var,
        ghost_tcn_s3_se_se_1_weight, ghost_tcn_s3_se_se_1_bias,
        ghost_tcn_s3_se_se_3_weight, ghost_tcn_s3_se_se_3_bias,
        work, buf_s3, &len);

    /* TCN x5: 用buf_tcnA/B双缓冲 */
    memcpy(buf_tcnA, buf_s3, 40 * len * sizeof(float));
    float *tcn_in = buf_tcnA, *tcn_out = buf_tcnB;
    int tcn_len = len;

    #define TCN_BLOCK(n, d) do { \
        tcn_resblock_forward(tcn_in, 40, tcn_len, d, \
            ghost_tcn_tcn_##n##_dw1_weight, ghost_tcn_tcn_##n##_pw1_weight, \
            ghost_tcn_tcn_##n##_bn1_weight, ghost_tcn_tcn_##n##_bn1_bias, \
            ghost_tcn_tcn_##n##_bn1_running_mean, ghost_tcn_tcn_##n##_bn1_running_var, \
            ghost_tcn_tcn_##n##_dw2_weight, ghost_tcn_tcn_##n##_pw2_weight, \
            ghost_tcn_tcn_##n##_bn2_weight, ghost_tcn_tcn_##n##_bn2_bias, \
            ghost_tcn_tcn_##n##_bn2_running_mean, ghost_tcn_tcn_##n##_bn2_running_var, \
            ghost_tcn_tcn_##n##_ds_weight, \
            work, tcn_out, &tcn_len); \
        { float *tmp = tcn_in; tcn_in = tcn_out; tcn_out = tmp; } \
 \
    } while(0)

    TCN_BLOCK(0, 1); TCN_BLOCK(1, 2); TCN_BLOCK(2, 4);
    TCN_BLOCK(3, 8); TCN_BLOCK(4, 16);
    #undef TCN_BLOCK

    adaptive_avgpool1d_to1(tcn_in, 40, tcn_len, work);
    linear(work, 40, 5, ghost_tcn_cls_3_weight, ghost_tcn_cls_3_bias, output);
    sigmoid_(output, 5);

    return 1;
}
