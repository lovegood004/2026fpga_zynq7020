#ifndef HSSEG_DEFORM_SEDDL_H
#define HSSEG_DEFORM_SEDDL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define HSSEG_FS        1000
#define HSSEG_FS_FRAME  50
#define HSSEG_SEG_LEN   10000
#define HSSEG_N_CLASSES 5

typedef struct {
    float *data;
    int ndim;
    int shape[4];
    int size;
} hsseg_tensor_t;

typedef struct {
    /* c1: DeformConv1d */
    hsseg_tensor_t oc1_w, oc1_b, oc1_bn_w, oc1_bn_b, oc1_bn_rm, oc1_bn_rv;
    hsseg_tensor_t oc2_w, oc2_b;
    hsseg_tensor_t c1_w, c1_b, c1_bn_w, c1_bn_b, c1_bn_rm, c1_bn_rv;

    /* s2: GhostSep(16, 24, k=21, stride=2, dil=1) */
    hsseg_tensor_t s2_dw_w, s2_dw_b, s2_bn1_w, s2_bn1_b, s2_bn1_rm, s2_bn1_rv;
    hsseg_tensor_t s2_pw_w, s2_pw_b, s2_cheap_w, s2_cheap_b;
    hsseg_tensor_t s2_bn2_w, s2_bn2_b, s2_bn2_rm, s2_bn2_rv;

    /* s3: GhostSep(24, 32, k=5, stride=2, dil=2) */
    hsseg_tensor_t s3_dw_w, s3_dw_b, s3_bn1_w, s3_bn1_b, s3_bn1_rm, s3_bn1_rv;
    hsseg_tensor_t s3_pw_w, s3_pw_b, s3_cheap_w, s3_cheap_b;
    hsseg_tensor_t s3_bn2_w, s3_bn2_b, s3_bn2_rm, s3_bn2_rv;

    /* s4: GhostSep(32, 32, k=5, dil=4) */
    hsseg_tensor_t s4_dw_w, s4_dw_b, s4_bn1_w, s4_bn1_b, s4_bn1_rm, s4_bn1_rv;
    hsseg_tensor_t s4_pw_w, s4_pw_b, s4_cheap_w, s4_cheap_b;
    hsseg_tensor_t s4_bn2_w, s4_bn2_b, s4_bn2_rm, s4_bn2_rv;

    /* s5: GhostSep(32, 32, k=5, dil=8) */
    hsseg_tensor_t s5_dw_w, s5_dw_b, s5_bn1_w, s5_bn1_b, s5_bn1_rm, s5_bn1_rv;
    hsseg_tensor_t s5_pw_w, s5_pw_b, s5_cheap_w, s5_cheap_b;
    hsseg_tensor_t s5_bn2_w, s5_bn2_b, s5_bn2_rm, s5_bn2_rv;

    /* s6: GhostSep(32, 32, k=5, dil=16) */
    hsseg_tensor_t s6_dw_w, s6_dw_b, s6_bn1_w, s6_bn1_b, s6_bn1_rm, s6_bn1_rv;
    hsseg_tensor_t s6_pw_w, s6_pw_b, s6_cheap_w, s6_cheap_b;
    hsseg_tensor_t s6_bn2_w, s6_bn2_b, s6_bn2_rm, s6_bn2_rv;

    /* s7: GhostSep(32, 32, k=5, dil=32) */
    hsseg_tensor_t s7_dw_w, s7_dw_b, s7_bn1_w, s7_bn1_b, s7_bn1_rm, s7_bn1_rv;
    hsseg_tensor_t s7_pw_w, s7_pw_b, s7_cheap_w, s7_cheap_b;
    hsseg_tensor_t s7_bn2_w, s7_bn2_b, s7_bn2_rm, s7_bn2_rv;

    /* seq: SEDDLMiniGRU(32, 8, rank=8) */
    hsseg_tensor_t wz_w, wz_b, wh_w, wh_b;
    hsseg_tensor_t proj_w, proj_b;
    hsseg_tensor_t fc_k_w, fc_beta_w, fc_v_w;

    /* read: Linear(32, 5) */
    hsseg_tensor_t read_w, read_b;
} hsseg_model_t;

int hsseg_model_load(hsseg_model_t *model, const char *path);
void hsseg_model_free(hsseg_model_t *model);
int hsseg_segment(hsseg_model_t *model, const float *audio, int n_samples,
                  int *pred_states, int n_frames);
int hsseg_segment_timed(hsseg_model_t *model, const float *audio, int n_samples,
                        int *pred_states, int n_frames, double *elapsed_ms);
int hsseg_segment_profile(hsseg_model_t *model, const float *audio, int n_samples,
                          int *pred_states, int n_frames,
                          double *t_frontend, double *t_gru, double *t_classifier);

#ifdef __cplusplus
}
#endif

#endif
