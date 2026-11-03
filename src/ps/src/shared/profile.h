#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void profile_reset(void);
void profile_report(const char *name);
void profile_set_capture(int enable);
void profile_snapshot_us(uint32_t out[9]);
#ifdef __cplusplus
}
#endif
