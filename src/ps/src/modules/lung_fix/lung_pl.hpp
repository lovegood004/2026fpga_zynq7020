#pragma once
#include <cstddef>
#include <cstdint>
struct LungPlProfile {
    uint32_t magic,version,enabled,faulted,calls,completed,unsupported,errors,timeouts,cancellations;
    uint32_t staging_us,waiting_us,copy_us,input_bytes,output_bytes,last_status;
};
extern "C" volatile LungPlProfile lung_pl_profile;
void lung_pl_enable(bool);
// 1: computed; 0: disabled/unsupported before touching hardware; -1: fault;
// -2: cancelled (started requests are drained or permanently quarantined).
int lung_pl_conv(const float*,const float*,const float*,float*,unsigned,unsigned,unsigned,unsigned,
                 void(*)(unsigned),const int*);
#ifdef LUNG_PL_MOCK
void lung_pl_test_reset();
#endif
