#pragma once
#include <stdint.h>
// Cumulative CPU global-timer ticks. Wait includes AXI transfers and PL compute.
typedef struct {
 uint32_t magic,version,sequence,calls,errors,phase;
 uint64_t ticks_per_second,input_clean,output_prepare,setup,wait,output_invalidate,copy,total;
 uint32_t sequence_end,reserved;
} PwProfile;
#ifdef __cplusplus
extern "C" {
#endif
extern volatile PwProfile pw_profile;
#ifdef __cplusplus
}
#endif
