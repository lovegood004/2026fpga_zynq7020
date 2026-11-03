#include "lung_pl.hpp"
#include "conv.hpp"
#include <algorithm>
#include <cstring>
#ifdef LUNG_PL_MOCK
#include "test_io.hpp"
#else
extern "C" {
#include "xil_cache.h"
#include "xil_io.h"
#include "esp_timer.h"
}
static uint32_t lp_read(unsigned offset){return Xil_In32(0x43c60000u+offset);}
static void lp_write(unsigned offset,uint32_t value){Xil_Out32(0x43c60000u+offset,value);}
static uint32_t lp_address(const void*p){return uint32_t(reinterpret_cast<uintptr_t>(p));}
static uint64_t lp_clock(){return uint64_t(esp_timer_get_time());}
static void lp_clean(void*p,size_t n){Xil_DCacheFlushRange((UINTPTR)p,n);}
static void lp_invalidate(void*p,size_t n){Xil_DCacheInvalidateRange((UINTPTR)p,n);}
static void lp_barrier(){asm volatile("dsb sy":::"memory");}
#endif
extern "C" {alignas(64) volatile LungPlProfile lung_pl_profile={0x4c504c31,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};}
namespace {
// Dedicated buffers remain allocated forever. A timed-out AXI master can never
// access freed model scratch or overwrite a published waveform/result.
alignas(64) float input_stage[LUNG_CONV_INPUT];
alignas(64) float weight_stage[LUNG_CONV_WEIGHTS];
alignas(64) float bias_stage[256];
alignas(64) float output_stage[LUNG_CONV_INPUT];
bool running=false;
void publish(){lp_clean(const_cast<LungPlProfile*>(&lung_pl_profile),sizeof(lung_pl_profile));lp_barrier();}
bool cancelled(const int*p){return p&&*p;}
bool copy_serviced(float*dst,const float*src,size_t count,void(*service)(unsigned),const int*cancel){
    for(size_t off=0;off<count;off+=4096){
        if(cancelled(cancel))return false;
        const auto take=std::min(size_t(4096),count-off);std::memcpy(dst+off,src+off,take*4);
        if(service)service(5);
    }
    return !cancelled(cancel);
}
int fault(int status,bool timeout=false){
    lung_pl_profile.faulted=1;lung_pl_profile.enabled=0;++lung_pl_profile.errors;
    if(timeout)++lung_pl_profile.timeouts;
    lung_pl_profile.last_status=uint32_t(status);publish();return -1;
}
bool safe_address(const void*p,size_t bytes){
    const uint32_t a=lp_address(p);return a>=0x100000u&&a<0x20000000u&&bytes<=0x20000000u-a;
}
void pointer(unsigned reg,const void*p){lp_write(reg,lp_address(p));lp_write(reg+4,0);}
}
void lung_pl_enable(bool enabled){if(!lung_pl_profile.faulted)lung_pl_profile.enabled=enabled;publish();}
int lung_pl_conv(const float*x,const float*w,const float*b,float*y,unsigned ci,unsigned co,unsigned h,unsigned width,
                 void(*service)(unsigned),const int*cancel){
    if(lung_pl_profile.faulted)return -1;
    if(!lung_pl_profile.enabled)return 0;
    if(running)return -1;
    if(!x||!w||!b||!y)return -1;
    if(ci<1||ci>128||co<1||co>256||h<1||h>128||width<1||width>468||ci*width>LUNG_CONV_ROW){++lung_pl_profile.unsupported;publish();return 0;}
    const size_t ni=size_t(ci)*h*width,nw=size_t(co)*ci*9,no=size_t(co)*((h+1)/2)*((width+1)/2);
    if(ni>LUNG_CONV_INPUT||no>LUNG_CONV_INPUT||nw>LUNG_CONV_WEIGHTS){++lung_pl_profile.unsupported;publish();return 0;}
    struct Guard {Guard(){running=true;}~Guard(){running=false;}} guard;
    ++lung_pl_profile.calls;
    if(cancelled(cancel)){++lung_pl_profile.cancellations;publish();return -2;}
    auto began=lp_clock();
    if(!copy_serviced(input_stage,x,ni,service,cancel)||!copy_serviced(weight_stage,w,nw,service,cancel)||
       !copy_serviced(bias_stage,b,co,service,cancel)){++lung_pl_profile.cancellations;publish();return -2;}
    if(!safe_address(input_stage,ni*4)||!safe_address(weight_stage,nw*4)||!safe_address(bias_stage,co*4)||!safe_address(output_stage,no*4))return fault(-10);
    lp_clean(input_stage,ni*4);lp_clean(weight_stage,nw*4);lp_clean(bias_stage,co*4);
    lp_clean(output_stage,no*4);lp_invalidate(output_stage,no*4);lp_barrier();
    lung_pl_profile.staging_us+=uint32_t(lp_clock()-began);
    if(cancelled(cancel)){++lung_pl_profile.cancellations;publish();return -2;}
    if(!(lp_read(0)&4))return fault(-11);
    pointer(0x20,input_stage);pointer(0x30,weight_stage);pointer(0x40,bias_stage);pointer(0x50,output_stage);
    lp_write(0x60,ci);lp_write(0x68,co);lp_write(0x70,h);lp_write(0x78,width);
    lp_barrier();began=lp_clock();lp_write(0,1);lp_barrier();
    auto last_service=began;
    for(;;){
        const auto status=lp_read(0);if(status&2)break;
        const auto now=lp_clock();
        if(now-began>2000000u)return fault(-12,true);
        if(now-last_service>=2000u){last_service=now;if(service)service(5);}
        // Cancellation does not reset the fabric or release/reuse active AXI
        // buffers. Drain the bounded operation, then discard its result.
    }
    lung_pl_profile.waiting_us+=uint32_t(lp_clock()-began);
    lp_barrier();const int result=int(lp_read(0x10));lung_pl_profile.last_status=uint32_t(result);
    if(result!=LUNG_CONV_OK)return fault(result);
    if(cancelled(cancel)){++lung_pl_profile.cancellations;publish();return -2;}
    lp_invalidate(output_stage,no*4);lp_barrier();began=lp_clock();
    if(!copy_serviced(y,output_stage,no,service,cancel)){++lung_pl_profile.cancellations;publish();return -2;}
    lung_pl_profile.copy_us+=uint32_t(lp_clock()-began);++lung_pl_profile.completed;
    lung_pl_profile.input_bytes+=uint32_t((ni+nw+co)*4);lung_pl_profile.output_bytes+=uint32_t(no*4);
    publish();return 1;
}
#ifdef LUNG_PL_MOCK
void lung_pl_test_reset(){
    running=false;std::memset(const_cast<LungPlProfile*>(&lung_pl_profile),0,sizeof(lung_pl_profile));
    lung_pl_profile.magic=0x4c504c31;lung_pl_profile.version=1;
}
#endif
