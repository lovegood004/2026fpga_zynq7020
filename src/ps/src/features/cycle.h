#pragma once
#include <stddef.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { FEATURE_CYCLE_FRAME_HZ=50, FEATURE_CYCLE_UNKNOWN=0,
       FEATURE_CYCLE_S1=1, FEATURE_CYCLE_SYSTOLE=2,
       FEATURE_CYCLE_S2=3, FEATURE_CYCLE_DIASTOLE=4 };
typedef struct {
    int valid; /* numeric model output, NOT verified medical annotation */
    int frames, s1_events, s2_events, transitions_invalid;
    float duration_s, bpm_from_s1;
    size_t arena_highwater;
    const char *reason;
} feature_cycle_result;
/* Non-reentrant. Nominal input rate is metadata supplied by the caller.
   State zero means unknown. Bounded output capacity (<=3000) is zeroed before validation.
   No calibrated per-frame confidence is available from the original decoder.
   This is a separate DeformSEDDLGRU model, not Ghost classification preprocessing. */
feature_cycle_result feature_cycle_pcm16(const int16_t *pcm,size_t count,
                                        int rate,uint8_t *states,size_t capacity);
feature_cycle_result feature_cycle_pcm16_serviced(const int16_t *,size_t,int,
    uint8_t *,size_t,void (*service)(unsigned),const int *cancelled);
const char *feature_cycle_state_name(int state);
#ifdef __cplusplus
}
#endif
