#pragma once
#include <stdint.h>
// Register offsets are checked against the actual generated HLS RTL.
enum { PW_CTRL=0x00,PW_RETURN=0x10,PW_W=0x18,PW_X=0x20,PW_Y=0x28,
       PW_CI=0x30,PW_CO=0x38,PW_LEN=0x40 };
typedef struct {uintptr_t base;int fault;} PwDevice;
typedef struct {uint64_t setup_ticks,wait_ticks;} PwSubmitTiming;
enum {PW_OK=0,PW_BAD_ARGUMENT=-1,PW_BUSY=-2,PW_TIMEOUT=-3,PW_BAD_RESULT=-4,PW_FAULT=-5};
#ifdef __cplusplus
extern "C" {
#endif
uint32_t pw_hal_read(uintptr_t address);
void pw_hal_write(uintptr_t address,uint32_t value);
uint64_t pw_hal_ticks(void);
void pw_hal_barrier(void);
// Non-reentrant polling driver. A timeout latches a fault: reset the PL system
// before reinitializing, since outstanding AXI writes may still be in flight.
int pw_submit(PwDevice*d,uint32_t w,uint32_t x,uint32_t y,int ci,int co,int len,uint64_t timeout);
int pw_submit_timed(PwDevice*d,uint32_t w,uint32_t x,uint32_t y,int ci,int co,int len,uint64_t timeout,PwSubmitTiming*);
#ifdef __cplusplus
}
#endif
