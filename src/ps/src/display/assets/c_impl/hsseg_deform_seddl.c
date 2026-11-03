#include "hsseg_deform_seddl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

hsseg_model_t *g_model = NULL;

static inline float clampf(float x, float lo, float hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}

static inline float sigmoidf(float x) {
    return 1.0f / (1.0f + expf(-x));
}

static double get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1e6;
}

static int read_tensor(FILE *f, hsseg_tensor_t *t) {
    uint32_t ndim;
    if (fread(&ndim, 4, 1, f) != 1) return -1;
    t->ndim = (int)ndim;
    t->size = 1;
    for (int i = 0; i < (int)ndim; i++) {
        uint32_t d;
        if (fread(&d, 4, 1, f) != 1) return -1;
        t->shape[i] = (int)d;
        t->size *= (int)d;
    }
    if (t->size == 0) {
        t->data = NULL;
        return 0;
    }
    t->data = (float *)malloc(t->size * sizeof(float));
    if (!t->data) return -1;
    if (fread(t->data, sizeof(float), t->size, f) != (size_t)t->size) {
        free(t->data);
        return -1;
    }
    return 0;
}

int hsseg_model_load(hsseg_model_t *model, const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;
    char magic[4];
    uint32_t ver;
    fread(magic, 1, 4, f);
    fread(&ver, 4, 1, f);
    int ret = 0;
    if (read_tensor(f, &model->oc1_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->oc1_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->oc1_bn_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->oc1_bn_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->oc1_bn_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->oc1_bn_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->oc2_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->oc2_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->c1_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->c1_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->c1_bn_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->c1_bn_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->c1_bn_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->c1_bn_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_dw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_dw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_bn1_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_bn1_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_bn1_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_bn1_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_pw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_pw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_cheap_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_cheap_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_bn2_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_bn2_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_bn2_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s2_bn2_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_dw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_dw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_bn1_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_bn1_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_bn1_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_bn1_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_pw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_pw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_cheap_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_cheap_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_bn2_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_bn2_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_bn2_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s3_bn2_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_dw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_dw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_bn1_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_bn1_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_bn1_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_bn1_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_pw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_pw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_cheap_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_cheap_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_bn2_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_bn2_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_bn2_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s4_bn2_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_dw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_dw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_bn1_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_bn1_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_bn1_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_bn1_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_pw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_pw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_cheap_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_cheap_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_bn2_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_bn2_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_bn2_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s5_bn2_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_dw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_dw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_bn1_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_bn1_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_bn1_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_bn1_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_pw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_pw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_cheap_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_cheap_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_bn2_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_bn2_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_bn2_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s6_bn2_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_dw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_dw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_bn1_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_bn1_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_bn1_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_bn1_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_pw_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_pw_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_cheap_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_cheap_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_bn2_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_bn2_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_bn2_rm) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->s7_bn2_rv) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->wz_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->wz_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->wh_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->wh_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->proj_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->proj_b) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->fc_k_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->fc_beta_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->fc_v_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->read_w) < 0) { ret = -1; goto done; }
    if (read_tensor(f, &model->read_b) < 0) { ret = -1; goto done; }
done:
    fclose(f);
    return ret;
}

void hsseg_model_free(hsseg_model_t *model) {
    free(model->oc1_w.data); free(model->oc1_b.data);
    free(model->oc1_bn_w.data); free(model->oc1_bn_b.data);
    free(model->oc1_bn_rm.data); free(model->oc1_bn_rv.data);
    free(model->oc2_w.data); free(model->oc2_b.data);
    free(model->c1_w.data); free(model->c1_b.data);
    free(model->c1_bn_w.data); free(model->c1_bn_b.data);
    free(model->c1_bn_rm.data); free(model->c1_bn_rv.data);
    free(model->s2_dw_w.data); free(model->s2_dw_b.data);
    free(model->s2_bn1_w.data); free(model->s2_bn1_b.data);
    free(model->s2_bn1_rm.data); free(model->s2_bn1_rv.data);
    free(model->s2_pw_w.data); free(model->s2_pw_b.data);
    free(model->s2_cheap_w.data); free(model->s2_cheap_b.data);
    free(model->s2_bn2_w.data); free(model->s2_bn2_b.data);
    free(model->s2_bn2_rm.data); free(model->s2_bn2_rv.data);
    free(model->s3_dw_w.data); free(model->s3_dw_b.data);
    free(model->s3_bn1_w.data); free(model->s3_bn1_b.data);
    free(model->s3_bn1_rm.data); free(model->s3_bn1_rv.data);
    free(model->s3_pw_w.data); free(model->s3_pw_b.data);
    free(model->s3_cheap_w.data); free(model->s3_cheap_b.data);
    free(model->s3_bn2_w.data); free(model->s3_bn2_b.data);
    free(model->s3_bn2_rm.data); free(model->s3_bn2_rv.data);
    free(model->s4_dw_w.data); free(model->s4_dw_b.data);
    free(model->s4_bn1_w.data); free(model->s4_bn1_b.data);
    free(model->s4_bn1_rm.data); free(model->s4_bn1_rv.data);
    free(model->s4_pw_w.data); free(model->s4_pw_b.data);
    free(model->s4_cheap_w.data); free(model->s4_cheap_b.data);
    free(model->s4_bn2_w.data); free(model->s4_bn2_b.data);
    free(model->s4_bn2_rm.data); free(model->s4_bn2_rv.data);
    free(model->s5_dw_w.data); free(model->s5_dw_b.data);
    free(model->s5_bn1_w.data); free(model->s5_bn1_b.data);
    free(model->s5_bn1_rm.data); free(model->s5_bn1_rv.data);
    free(model->s5_pw_w.data); free(model->s5_pw_b.data);
    free(model->s5_cheap_w.data); free(model->s5_cheap_b.data);
    free(model->s5_bn2_w.data); free(model->s5_bn2_b.data);
    free(model->s5_bn2_rm.data); free(model->s5_bn2_rv.data);
    free(model->s6_dw_w.data); free(model->s6_dw_b.data);
    free(model->s6_bn1_w.data); free(model->s6_bn1_b.data);
    free(model->s6_bn1_rm.data); free(model->s6_bn1_rv.data);
    free(model->s6_pw_w.data); free(model->s6_pw_b.data);
    free(model->s6_cheap_w.data); free(model->s6_cheap_b.data);
    free(model->s6_bn2_w.data); free(model->s6_bn2_b.data);
    free(model->s6_bn2_rm.data); free(model->s6_bn2_rv.data);
    free(model->s7_dw_w.data); free(model->s7_dw_b.data);
    free(model->s7_bn1_w.data); free(model->s7_bn1_b.data);
    free(model->s7_bn1_rm.data); free(model->s7_bn1_rv.data);
    free(model->s7_pw_w.data); free(model->s7_pw_b.data);
    free(model->s7_cheap_w.data); free(model->s7_cheap_b.data);
    free(model->s7_bn2_w.data); free(model->s7_bn2_b.data);
    free(model->s7_bn2_rm.data); free(model->s7_bn2_rv.data);
    free(model->wz_w.data); free(model->wz_b.data);
    free(model->wh_w.data); free(model->wh_b.data);
    free(model->proj_w.data); free(model->proj_b.data);
    free(model->fc_k_w.data); free(model->fc_beta_w.data);
    free(model->fc_v_w.data);
    free(model->read_w.data); free(model->read_b.data);
}

#ifndef HSSEG_YIELD
#define HSSEG_YIELD() ((void)0)
#endif
static void bn_inference(float *out, const float *in, int C, int L,
                         const float *gamma, const float *beta,
                         const float *rm, const float *rv) {
    for (int c = 0; c < C; c++) {
        if((c & 31)==0)HSSEG_YIELD();
        float scale = gamma[c] / sqrtf(rv[c] + 1e-5f);
        float shift = beta[c] - rm[c] * scale;
        for (int l = 0; l < L; l++) {
        if((l & 31)==0)HSSEG_YIELD();
            out[c * L + l] = in[c * L + l] * scale + shift;
        }
    }
}

static void depthwise_conv1d(float *out, const float *in, int C, int L_in, int L_out,
                             int K, int stride, int dil, int pad,
                             const float *w, const float *b) {
    for (int c = 0; c < C; c++) {
        if((c & 31)==0)HSSEG_YIELD();
        const float *in_c = in + c * L_in;
        const float *w_c = w + c * K;
        float bias = b[c];
        for (int lo = 0; lo < L_out; lo++) {
        if((lo & 31)==0)HSSEG_YIELD();
            float sum = bias;
            int base = lo * stride - pad;
            for (int k = 0; k < K; k++) {
                int li = base + k * dil;
                if (li >= 0 && li < L_in) {
                    sum += in_c[li] * w_c[k];
                }
            }
            out[c * L_out + lo] = sum;
        }
    }
}

static void pointwise_conv1d(float *out, const float *in, int C_in, int C_out, int L,
                             const float *w, const float *b) {
    for (int co = 0; co < C_out; co++) {
        if((co & 31)==0)HSSEG_YIELD();
        const float *w_co = w + co * C_in;
        float bias = b[co];
        for (int lo = 0; lo < L; lo++) {
        if((lo & 31)==0)HSSEG_YIELD();
            float sum = bias;
            for (int ci = 0; ci < C_in; ci++) {
                sum += in[ci * L + lo] * w_co[ci];
            }
            out[co * L + lo] = sum;
        }
    }
}

static void ghost_sep_forward(float *out, const float *in, int C_in, int C_out, int L_in,
                              int K, int stride, int dil,
                              const hsseg_tensor_t *dw_w, const hsseg_tensor_t *dw_b,
                              const hsseg_tensor_t *bn1_w, const hsseg_tensor_t *bn1_b,
                              const hsseg_tensor_t *bn1_rm, const hsseg_tensor_t *bn1_rv,
                              const hsseg_tensor_t *pw_w, const hsseg_tensor_t *pw_b,
                              const hsseg_tensor_t *cheap_w, const hsseg_tensor_t *cheap_b,
                              const hsseg_tensor_t *bn2_w, const hsseg_tensor_t *bn2_b,
                              const hsseg_tensor_t *bn2_rm, const hsseg_tensor_t *bn2_rv) {
    int C_mid = C_out / 2;
    int L_out = (L_in + 2 * (dil * (K / 2)) - dil * (K - 1) - 1) / stride + 1;
    float *dw_out = (float *)malloc(C_in * L_out * sizeof(float));
    float *bn1_out = (float *)malloc(C_in * L_out * sizeof(float));
    float *pw_out = (float *)malloc(C_mid * L_out * sizeof(float));
    float *cheap_out = (float *)malloc(C_mid * L_out * sizeof(float));
    int pad = dil * (K / 2);
    depthwise_conv1d(dw_out, in, C_in, L_in, L_out, K, stride, dil, pad, dw_w->data, dw_b->data);
    bn_inference(bn1_out, dw_out, C_in, L_out, bn1_w->data, bn1_b->data, bn1_rm->data, bn1_rv->data);
    for (int i = 0; i < C_in * L_out; i++) bn1_out[i] = fmaxf(bn1_out[i], 0.0f);
    pointwise_conv1d(pw_out, bn1_out, C_in, C_mid, L_out, pw_w->data, pw_b->data);
    depthwise_conv1d(cheap_out, pw_out, C_mid, L_out, L_out, 3, 1, 1, 1, cheap_w->data, cheap_b->data);
    float *cat = (float *)calloc(C_out * L_out, sizeof(float));
    for (int lo = 0; lo < L_out; lo++) {
        if((lo & 31)==0)HSSEG_YIELD();
        for (int c = 0; c < C_mid; c++) {
        if((c & 31)==0)HSSEG_YIELD();
            cat[c * L_out + lo] = pw_out[c * L_out + lo];
            cat[(C_mid + c) * L_out + lo] = cheap_out[c * L_out + lo];
        }
    }
    bn_inference(out, cat, C_out, L_out, bn2_w->data, bn2_b->data, bn2_rm->data, bn2_rv->data);
    for (int i = 0; i < C_out * L_out; i++) out[i] = fmaxf(out[i], 0.0f);
    free(dw_out); free(bn1_out); free(pw_out); free(cheap_out); free(cat);
}

static void deform_conv1d_forward(float *out, const float *in, int L_in, int C_out,
                                  int K, int stride,
                                  const hsseg_tensor_t *oc1_w, const hsseg_tensor_t *oc1_b,
                                  const hsseg_tensor_t *oc1_bn_w, const hsseg_tensor_t *oc1_bn_b,
                                  const hsseg_tensor_t *oc1_bn_rm, const hsseg_tensor_t *oc1_bn_rv,
                                  const hsseg_tensor_t *oc2_w, const hsseg_tensor_t *oc2_b,
                                  const hsseg_tensor_t *c1_w, const hsseg_tensor_t *c1_b,
                                  const hsseg_tensor_t *bn_w, const hsseg_tensor_t *bn_b,
                                  const hsseg_tensor_t *bn_rm, const hsseg_tensor_t *bn_rv) {
    float *oc1_out = (float *)malloc(L_in * sizeof(float));
    float *oc1_bn = (float *)malloc(L_in * sizeof(float));
    float *oc2_out = (float *)malloc(L_in * sizeof(float));
    for (int l = 0; l < L_in; l++) {
        if((l & 31)==0)HSSEG_YIELD();
        float sum = oc1_b->data[0];
        for (int k = 0; k < 3; k++) {
            int li = l + k - 1;
            if (li >= 0 && li < L_in) sum += in[li] * oc1_w->data[k];
        }
        oc1_out[l] = sum;
    }
    float scale = oc1_bn_w->data[0] / sqrtf(oc1_bn_rv->data[0] + 1e-5f);
    float shift = oc1_bn_b->data[0] - oc1_bn_rm->data[0] * scale;
    for (int l = 0; l < L_in; l++) oc1_bn[l] = fmaxf(oc1_out[l] * scale + shift, 0.0f);
    for (int l = 0; l < L_in; l++) {
        if((l & 31)==0)HSSEG_YIELD();
        float v = oc1_bn[l] * oc2_w->data[0] + oc2_b->data[0];
        oc2_out[l] = tanhf(v) * 2.0f;
    }
    float *x_deformed = (float *)malloc(L_in * sizeof(float));
    for (int l = 0; l < L_in; l++) {
        if((l & 31)==0)HSSEG_YIELD();
        int pos = (int)((float)l + oc2_out[l]); /* 与torch .long()截断一致 */
        pos = pos < 0 ? 0 : (pos >= L_in ? L_in - 1 : pos);
        x_deformed[l] = in[pos];
    }
    int L_out = (L_in + 2 * 20 - 41) / stride + 1;
    float *conv_out = (float *)malloc(C_out * L_out * sizeof(float));
    for (int co = 0; co < C_out; co++) {
        if((co & 31)==0)HSSEG_YIELD();
        const float *w_co = c1_w->data + co * K;
        float bias = c1_b->data[co];
        for (int lo = 0; lo < L_out; lo++) {
        if((lo & 31)==0)HSSEG_YIELD();
            float sum = bias;
            int base = lo * stride - 20;
            for (int k = 0; k < K; k++) {
                int li = base + k;
                if (li >= 0 && li < L_in) sum += x_deformed[li] * w_co[k];
            }
            conv_out[co * L_out + lo] = sum;
        }
    }
    bn_inference(out, conv_out, C_out, L_out, bn_w->data, bn_b->data, bn_rm->data, bn_rv->data);
    for (int i = 0; i < C_out * L_out; i++) out[i] = fmaxf(out[i], 0.0f);
    free(oc1_out); free(oc1_bn); free(oc2_out); free(x_deformed); free(conv_out);
}

static void seddl_mini_gru_forward(float *out, const float *in, int T, int d_model, int width) {
    hsseg_model_t *model = g_model;
    const float *wz_w = model->wz_w.data;
    const float *wz_b = model->wz_b.data;
    const float *wh_w = model->wh_w.data;
    const float *wh_b = model->wh_b.data;
    const float *proj_w = model->proj_w.data;
    const float *proj_b = model->proj_b.data;
    const float *fc_k_w = model->fc_k_w.data;
    const float *fc_beta_w = model->fc_beta_w.data;
    const float *fc_v_w = model->fc_v_w.data;

    /* 输入 in 为 channel-major [d_model, T], 与前端输出布局一致 */
    float *x_squeeze = (float *)calloc(d_model, sizeof(float));
    for (int d = 0; d < d_model; d++) {
        const float *in_d = in + (size_t)d * T;
        float s = 0.0f;
        for (int t = 0; t < T; t++) s += in_d[t];
        x_squeeze[d] = s / T;
    }

    float *k = (float *)malloc(width * sizeof(float));
    float *v = (float *)malloc(width * sizeof(float));
    float beta_val = 0.0f;
    for (int w = 0; w < width; w++) {
        float kv = 0.0f, vv = 0.0f;
        for (int d = 0; d < d_model; d++) {
            kv += x_squeeze[d] * fc_k_w[w * d_model + d];
            vv += x_squeeze[d] * fc_v_w[w * d_model + d];
        }
        k[w] = kv;
        v[w] = vv;
    }
    for (int d = 0; d < d_model; d++) beta_val += x_squeeze[d] * fc_beta_w[d];
    beta_val = sigmoidf(beta_val) * 2.0f;

    float k_norm = 0.0f;
    for (int w = 0; w < width; w++) k_norm += k[w] * k[w];
    k_norm = sqrtf(k_norm);
    if (k_norm < 1e-6f) k_norm = 1e-6f;
    for (int w = 0; w < width; w++) k[w] /= k_norm;
    float k_dot_v = 0.0f;
    for (int w = 0; w < width; w++) k_dot_v += k[w] * v[w];

    float *h = (float *)calloc(width, sizeof(float));
    for (int t = 0; t < T; t++) {
        if((t & 31)==0)HSSEG_YIELD();
        float *z_arr = (float *)malloc(width * sizeof(float));
        float *ht_arr = (float *)malloc(width * sizeof(float));
        for (int w = 0; w < width; w++) {
            float wz = wz_b[w], wh = wh_b[w];
            for (int d = 0; d < d_model; d++) {
                float xv = in[(size_t)d * T + t];
                wz += xv * wz_w[w * d_model + d];
                wh += xv * wh_w[w * d_model + d];
            }
            z_arr[w] = sigmoidf(wz);
            ht_arr[w] = wh;
        }
        float k_dot_ht = 0.0f;
        for (int w = 0; w < width; w++) k_dot_ht += k[w] * ht_arr[w];
        for (int w = 0; w < width; w++) {
            ht_arr[w] = ht_arr[w] - beta_val * k[w] * k_dot_ht + beta_val * k[w] * k_dot_v;
        }
        for (int w = 0; w < width; w++) {
            float a = 1.0f - z_arr[w];
            float b_val = z_arr[w] * ht_arr[w];
            h[w] = (t == 0) ? b_val : a * h[w] + b_val;
        }
        for (int d = 0; d < d_model; d++) {
            float proj_val = proj_b[d];
            for (int w = 0; w < width; w++) {
                proj_val += h[w] * proj_w[d * width + w];
            }
            out[t * d_model + d] = in[(size_t)d * T + t] + proj_val;
        }
        free(z_arr); free(ht_arr);
    }
    free(x_squeeze); free(k); free(v); free(h);
}

/* 四状态循环约束解码。
   标签结构: S1(1) -> systole(2) -> S2(3) -> diastole(4) -> S1(1) 循环,
   允许驻留与前进一档, 禁止回退、跳档与类别0。
   以类别logits为发射分数, 在4状态格上做维特比解码, 复杂度 O(T*4)。 */
static void decode_states_constrained(const float *logits, int T, int *pred) {
    const float NEG = -1e30f;
    if (T <= 0) return;
    float *dp_prev = (float *)malloc(4 * sizeof(float));
    float *dp_cur = (float *)malloc(4 * sizeof(float));
    int *bp = (int *)malloc((size_t)T * 4 * sizeof(int));
    if (!dp_prev || !dp_cur || !bp) {
        /* 回退: 逐帧在1..4中取极大 */
        for (int t = 0; t < T; t++) {
        if((t & 31)==0)HSSEG_YIELD();
            float best = NEG; int bi = 1;
            for (int c = 1; c <= 4; c++) {
        if((c & 31)==0)HSSEG_YIELD();
                if (logits[t * 5 + c] > best) { best = logits[t * 5 + c]; bi = c; }
            }
            pred[t] = bi;
        }
        free(dp_prev); free(dp_cur); free(bp);
        return;
    }
    /* t=0: 任意合法状态均可作为起点 */
    for (int i = 0; i < 4; i++) dp_prev[i] = logits[1 + i];
    for (int t = 1; t < T; t++) {
        if((t & 31)==0)HSSEG_YIELD();
        for (int i = 0; i < 4; i++) {
            /* 前驱: 自身驻留 i, 或前一状态 (i+3)%4 前进一档 */
            int self_p = i;
            int fwd_p = (i + 3) % 4;
            float best = dp_prev[self_p];
            int arg = self_p;
            if (dp_prev[fwd_p] > best) { best = dp_prev[fwd_p]; arg = fwd_p; }
            dp_cur[i] = best + logits[t * 5 + (i + 1)];
            bp[(size_t)t * 4 + i] = arg;
        }
        float *tmp = dp_prev; dp_prev = dp_cur; dp_cur = tmp;
    }
    int bi = 0;
    for (int i = 1; i < 4; i++) if (dp_prev[i] > dp_prev[bi]) bi = i;
    pred[T - 1] = bi + 1;
    for (int t = T - 1; t > 0; t--) {
        if((t & 31)==0)HSSEG_YIELD();
        bi = bp[(size_t)t * 4 + bi];
        pred[t - 1] = bi + 1;
    }
    free(dp_prev); free(dp_cur); free(bp);
}

int hsseg_segment(hsseg_model_t *model, const float *audio, int n_samples,
                  int *pred_states, int n_frames) {
    if (n_samples != HSSEG_SEG_LEN) return -1;
    g_model = model;

    int L0 = n_samples;
    int L1 = (L0 + 2 * 20 - 41) / 5 + 1;
    int L2 = (L1 + 2 * 10 - 21) / 2 + 1;
    int L3 = (L2 + 2 * 4 - 5 * 2) / 2 + 1;
    int T = L3;

    float *c1_out = (float *)malloc(16 * L1 * sizeof(float));
    float *s2_out = (float *)malloc(24 * L2 * sizeof(float));
    float *s3_out = (float *)malloc(32 * L3 * sizeof(float));
    float *s4_out = (float *)malloc(32 * L3 * sizeof(float));
    float *s5_out = (float *)malloc(32 * L3 * sizeof(float));
    float *s6_out = (float *)malloc(32 * L3 * sizeof(float));
    float *s7_out = (float *)malloc(32 * T * sizeof(float));

    deform_conv1d_forward(c1_out, audio, L0, 16, 41, 5,
                          &model->oc1_w, &model->oc1_b, &model->oc1_bn_w, &model->oc1_bn_b,
                          &model->oc1_bn_rm, &model->oc1_bn_rv, &model->oc2_w, &model->oc2_b,
                          &model->c1_w, &model->c1_b, &model->c1_bn_w, &model->c1_bn_b,
                          &model->c1_bn_rm, &model->c1_bn_rv);
    ghost_sep_forward(s2_out, c1_out, 16, 24, L1, 21, 2, 1,
                      &model->s2_dw_w, &model->s2_dw_b, &model->s2_bn1_w, &model->s2_bn1_b,
                      &model->s2_bn1_rm, &model->s2_bn1_rv, &model->s2_pw_w, &model->s2_pw_b,
                      &model->s2_cheap_w, &model->s2_cheap_b, &model->s2_bn2_w, &model->s2_bn2_b,
                      &model->s2_bn2_rm, &model->s2_bn2_rv);
    ghost_sep_forward(s3_out, s2_out, 24, 32, L2, 5, 2, 2,
                      &model->s3_dw_w, &model->s3_dw_b, &model->s3_bn1_w, &model->s3_bn1_b,
                      &model->s3_bn1_rm, &model->s3_bn1_rv, &model->s3_pw_w, &model->s3_pw_b,
                      &model->s3_cheap_w, &model->s3_cheap_b, &model->s3_bn2_w, &model->s3_bn2_b,
                      &model->s3_bn2_rm, &model->s3_bn2_rv);
    ghost_sep_forward(s4_out, s3_out, 32, 32, L3, 5, 1, 4,
                      &model->s4_dw_w, &model->s4_dw_b, &model->s4_bn1_w, &model->s4_bn1_b,
                      &model->s4_bn1_rm, &model->s4_bn1_rv, &model->s4_pw_w, &model->s4_pw_b,
                      &model->s4_cheap_w, &model->s4_cheap_b, &model->s4_bn2_w, &model->s4_bn2_b,
                      &model->s4_bn2_rm, &model->s4_bn2_rv);
    ghost_sep_forward(s5_out, s4_out, 32, 32, L3, 5, 1, 8,
                      &model->s5_dw_w, &model->s5_dw_b, &model->s5_bn1_w, &model->s5_bn1_b,
                      &model->s5_bn1_rm, &model->s5_bn1_rv, &model->s5_pw_w, &model->s5_pw_b,
                      &model->s5_cheap_w, &model->s5_cheap_b, &model->s5_bn2_w, &model->s5_bn2_b,
                      &model->s5_bn2_rm, &model->s5_bn2_rv);
    ghost_sep_forward(s6_out, s5_out, 32, 32, L3, 5, 1, 16,
                      &model->s6_dw_w, &model->s6_dw_b, &model->s6_bn1_w, &model->s6_bn1_b,
                      &model->s6_bn1_rm, &model->s6_bn1_rv, &model->s6_pw_w, &model->s6_pw_b,
                      &model->s6_cheap_w, &model->s6_cheap_b, &model->s6_bn2_w, &model->s6_bn2_b,
                      &model->s6_bn2_rm, &model->s6_bn2_rv);
    ghost_sep_forward(s7_out, s6_out, 32, 32, L3, 5, 1, 32,
                      &model->s7_dw_w, &model->s7_dw_b, &model->s7_bn1_w, &model->s7_bn1_b,
                      &model->s7_bn1_rm, &model->s7_bn1_rv, &model->s7_pw_w, &model->s7_pw_b,
                      &model->s7_cheap_w, &model->s7_cheap_b, &model->s7_bn2_w, &model->s7_bn2_b,
                      &model->s7_bn2_rm, &model->s7_bn2_rv);

    float *gru_out = (float *)malloc(T * 32 * sizeof(float));
    seddl_mini_gru_forward(gru_out, s7_out, T, 32, 8);

    /* 调试: HSSEG_DUMP指定目录时导出各层激活 */
    {
        const char *dump_dir = getenv("HSSEG_DUMP");
        if (dump_dir) {
            char path[512];
            FILE *df;
            #define DUMP(buf, n, nm) do { \
                snprintf(path, sizeof(path), "%s/%s.f32", dump_dir, nm); \
                df = fopen(path, "wb"); \
                if (df) { fwrite((buf), sizeof(float), (n), df); fclose(df); } \
            } while (0)
            DUMP(c1_out, 16 * L1, "c1");
            DUMP(s2_out, 24 * L2, "s2");
            DUMP(s3_out, 32 * L3, "s3");
            DUMP(s4_out, 32 * L3, "s4");
            DUMP(s5_out, 32 * L3, "s5");
            DUMP(s6_out, 32 * L3, "s6");
            DUMP(s7_out, 32 * T, "s7");
            DUMP(gru_out, 32 * T, "gru");
            #undef DUMP
        }
    }

    int n_dec = T < n_frames ? T : n_frames;
    float *logits = (float *)malloc((size_t)n_dec * 5 * sizeof(float));
    for (int t = 0; t < n_dec; t++) {
        if((t & 31)==0)HSSEG_YIELD();
        for (int c = 0; c < 5; c++) {
        if((c & 31)==0)HSSEG_YIELD();
            float val = model->read_b.data[c];
            for (int d = 0; d < 32; d++) {
                val += gru_out[t * 32 + d] * model->read_w.data[c * 32 + d];
            }
            logits[t * 5 + c] = val;
        }
    }
    decode_states_constrained(logits, n_dec, pred_states);
    free(logits);
    free(c1_out); free(s2_out); free(s3_out);
    free(s4_out); free(s5_out); free(s6_out);
    free(s7_out); free(gru_out);
    return 0;
}

/* 带计时的推理接口 */
int hsseg_segment_timed(hsseg_model_t *model, const float *audio, int n_samples,
                        int *pred_states, int n_frames, double *elapsed_ms) {
    double t0 = get_time_ms();
    int ret = hsseg_segment(model, audio, n_samples, pred_states, n_frames);
    double t1 = get_time_ms();
    if (elapsed_ms) *elapsed_ms = t1 - t0;
    return ret;
}

/* 获取各阶段耗时 */
int hsseg_segment_profile(hsseg_model_t *model, const float *audio, int n_samples,
                          int *pred_states, int n_frames,
                          double *t_frontend, double *t_gru, double *t_classifier) {
    if (n_samples != HSSEG_SEG_LEN) return -1;
    g_model = model;

    int L0 = n_samples;
    int L1 = (L0 + 2 * 20 - 41) / 5 + 1;
    int L2 = (L1 + 2 * 10 - 21) / 2 + 1;
    int L3 = (L2 + 2 * 4 - 5 * 2) / 2 + 1;
    int T = L3;

    float *c1_out = (float *)malloc(16 * L1 * sizeof(float));
    float *s2_out = (float *)malloc(24 * L2 * sizeof(float));
    float *s3_out = (float *)malloc(32 * L3 * sizeof(float));
    float *s4_out = (float *)malloc(32 * L3 * sizeof(float));
    float *s5_out = (float *)malloc(32 * L3 * sizeof(float));
    float *s6_out = (float *)malloc(32 * L3 * sizeof(float));
    float *s7_out = (float *)malloc(32 * T * sizeof(float));
    float *gru_out = (float *)malloc(T * 32 * sizeof(float));

    double t_start = get_time_ms();

    deform_conv1d_forward(c1_out, audio, L0, 16, 41, 5,
                          &model->oc1_w, &model->oc1_b, &model->oc1_bn_w, &model->oc1_bn_b,
                          &model->oc1_bn_rm, &model->oc1_bn_rv, &model->oc2_w, &model->oc2_b,
                          &model->c1_w, &model->c1_b, &model->c1_bn_w, &model->c1_bn_b,
                          &model->c1_bn_rm, &model->c1_bn_rv);
    ghost_sep_forward(s2_out, c1_out, 16, 24, L1, 21, 2, 1,
                      &model->s2_dw_w, &model->s2_dw_b, &model->s2_bn1_w, &model->s2_bn1_b,
                      &model->s2_bn1_rm, &model->s2_bn1_rv, &model->s2_pw_w, &model->s2_pw_b,
                      &model->s2_cheap_w, &model->s2_cheap_b, &model->s2_bn2_w, &model->s2_bn2_b,
                      &model->s2_bn2_rm, &model->s2_bn2_rv);
    ghost_sep_forward(s3_out, s2_out, 24, 32, L2, 5, 2, 2,
                      &model->s3_dw_w, &model->s3_dw_b, &model->s3_bn1_w, &model->s3_bn1_b,
                      &model->s3_bn1_rm, &model->s3_bn1_rv, &model->s3_pw_w, &model->s3_pw_b,
                      &model->s3_cheap_w, &model->s3_cheap_b, &model->s3_bn2_w, &model->s3_bn2_b,
                      &model->s3_bn2_rm, &model->s3_bn2_rv);
    ghost_sep_forward(s4_out, s3_out, 32, 32, L3, 5, 1, 4,
                      &model->s4_dw_w, &model->s4_dw_b, &model->s4_bn1_w, &model->s4_bn1_b,
                      &model->s4_bn1_rm, &model->s4_bn1_rv, &model->s4_pw_w, &model->s4_pw_b,
                      &model->s4_cheap_w, &model->s4_cheap_b, &model->s4_bn2_w, &model->s4_bn2_b,
                      &model->s4_bn2_rm, &model->s4_bn2_rv);
    ghost_sep_forward(s5_out, s4_out, 32, 32, L3, 5, 1, 8,
                      &model->s5_dw_w, &model->s5_dw_b, &model->s5_bn1_w, &model->s5_bn1_b,
                      &model->s5_bn1_rm, &model->s5_bn1_rv, &model->s5_pw_w, &model->s5_pw_b,
                      &model->s5_cheap_w, &model->s5_cheap_b, &model->s5_bn2_w, &model->s5_bn2_b,
                      &model->s5_bn2_rm, &model->s5_bn2_rv);
    ghost_sep_forward(s6_out, s5_out, 32, 32, L3, 5, 1, 16,
                      &model->s6_dw_w, &model->s6_dw_b, &model->s6_bn1_w, &model->s6_bn1_b,
                      &model->s6_bn1_rm, &model->s6_bn1_rv, &model->s6_pw_w, &model->s6_pw_b,
                      &model->s6_cheap_w, &model->s6_cheap_b, &model->s6_bn2_w, &model->s6_bn2_b,
                      &model->s6_bn2_rm, &model->s6_bn2_rv);
    ghost_sep_forward(s7_out, s6_out, 32, 32, L3, 5, 1, 32,
                      &model->s7_dw_w, &model->s7_dw_b, &model->s7_bn1_w, &model->s7_bn1_b,
                      &model->s7_bn1_rm, &model->s7_bn1_rv, &model->s7_pw_w, &model->s7_pw_b,
                      &model->s7_cheap_w, &model->s7_cheap_b, &model->s7_bn2_w, &model->s7_bn2_b,
                      &model->s7_bn2_rm, &model->s7_bn2_rv);

    double t_frontend_end = get_time_ms();

    seddl_mini_gru_forward(gru_out, s7_out, T, 32, 8);
    double t_gru_end = get_time_ms();

    int n_dec = T < n_frames ? T : n_frames;
    float *logits = (float *)malloc((size_t)n_dec * 5 * sizeof(float));
    for (int t = 0; t < n_dec; t++) {
        if((t & 31)==0)HSSEG_YIELD();
        for (int c = 0; c < 5; c++) {
        if((c & 31)==0)HSSEG_YIELD();
            float val = model->read_b.data[c];
            for (int d = 0; d < 32; d++) {
                val += gru_out[t * 32 + d] * model->read_w.data[c * 32 + d];
            }
            logits[t * 5 + c] = val;
        }
    }
    decode_states_constrained(logits, n_dec, pred_states);
    free(logits);
    double t_classifier_end = get_time_ms();

    if (t_frontend) *t_frontend = t_frontend_end - t_start;
    if (t_gru) *t_gru = t_gru_end - t_frontend_end;
    if (t_classifier) *t_classifier = t_classifier_end - t_gru_end;

    free(c1_out); free(s2_out); free(s3_out);
    free(s4_out); free(s5_out); free(s6_out);
    free(s7_out); free(gru_out);
    return 0;
}
