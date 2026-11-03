#pragma once
#include <cstddef>
#include <cstdint>
struct LinearPlProfile {uint32_t magic,version,enabled,faulted,calls,completed,unsupported,errors,timeouts,cancellations,staging_us,waiting_us,copy_us,input_bytes,output_bytes,last_status;};
extern "C" volatile LinearPlProfile linear_pl_profile;
void linear_pl_enable(bool);
bool linear_pl_faulted();
// 1 computed, 0 disabled/unsupported before hardware, -1 fault, -2 cancelled.
int linear_pl_compute(const uint8_t*,const int8_t*,float*,const float*,float,
                      unsigned,unsigned,unsigned,int,int,void(*)(unsigned),const int*);
#ifdef LINEAR_PL_MOCK
void linear_pl_test_reset();
#endif
