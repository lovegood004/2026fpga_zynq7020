#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    int valid,label,segments,failed,resampled_samples;
    int votes[5];
    float mean_probability[5],vote_fraction;
    const char *reason;
} feature_lung_result;
enum feature_lung_stage { LUNG_PREVIEW=0,LUNG_STATS,LUNG_RESAMPLE,LUNG_SPECTROGRAM,
    LUNG_WAVE,LUNG_CNN,LUNG_MIXER,LUNG_GRU,LUNG_HEAD };
typedef struct {size_t samples;int64_t sum;int minimum,maximum;} feature_lung_input_stats;
typedef struct {uint32_t preprocessing_us,inference_us,cnn_us,mixer_us,gru_us,head_us;} feature_lung_timing;
typedef struct {
    const int *cancelled; /* Same-thread cooperative cancellation; never frees PCM. */
    const feature_lung_input_stats *input_stats; /* CRC-validated recording only. */
    feature_lung_timing *timing;
    uint64_t (*clock_us)(void);
} feature_lung_control;
/* release CNN-Mixer-BiGRU: full <=15s, 4kHz PCM -> log PSD 128 x variable time.
   Label order: COPD/Asthma/Pneumonia/Other/Healthy. One infer_8600-compatible
   prediction per recording; legacy votes/segments fields describe that one input.
   Separate from the Ghost heart and cycle models. Non-reentrant, CPU only. */
feature_lung_result feature_lung_pcm16(const int16_t *,size_t,int rate);
feature_lung_result feature_lung_pcm16_serviced(const int16_t *,size_t,int rate,
                                               void (*service)(unsigned));
/* Borrowed processed PCM is valid only inside this callback; no tail padding. */
typedef void (*feature_lung_wave_callback)(const float *,size_t,unsigned,void *);
feature_lung_result feature_lung_pcm16_with_wave(const int16_t *,size_t,int rate,
    void (*service)(unsigned),feature_lung_wave_callback,void *context);
feature_lung_result feature_lung_pcm16_controlled(const int16_t *,size_t,int rate,
    void (*service)(unsigned),feature_lung_wave_callback,void *,const feature_lung_control *);
const char *feature_lung_label(int label);
void feature_lung_release(void);
#ifdef __cplusplus
}
#endif
