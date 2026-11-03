#include "linear_pl.hpp"
#include "dot.hpp"
#include <algorithm>
#include <cstring>
#ifdef LINEAR_PL_MOCK
#include "linear_test_io.hpp"
#else
extern "C" {
#include "xil_cache.h"
#include "xil_io.h"
#include "esp_timer.h"
}
static uint32_t ll_read(unsigned r){return Xil_In32(0x43c70000u+r);}
static void ll_write(unsigned r,uint32_t v){Xil_Out32(0x43c70000u+r,v);}
static uint32_t ll_address(const void*p){return uint32_t(reinterpret_cast<uintptr_t>(p));}
static uint64_t ll_clock(){return uint64_t(esp_timer_get_time());}
static void ll_clean(void*p,size_t n){Xil_DCacheFlushRange((UINTPTR)p,n);}
static void ll_invalidate(void*p,size_t n){Xil_DCacheInvalidateRange((UINTPTR)p,n);}
static void ll_barrier(){asm volatile("dsb sy":::"memory");}
#endif
extern "C" {alignas(64) volatile LinearPlProfile linear_pl_profile={0x4c4c5031,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0};}
namespace {
alignas(64) uint8_t input_stage[LINEAR_INPUT];
alignas(64) int8_t weight_stage[LINEAR_WEIGHT];
alignas(64) int32_t output_stage[LINEAR_OUTPUT];
bool running=false;
bool cancelled(const int*p){return p&&*p;}
void publish(){ll_clean(const_cast<LinearPlProfile*>(&linear_pl_profile),sizeof(linear_pl_profile));ll_barrier();}
int fault(int status,bool timeout=false){linear_pl_profile.faulted=1;linear_pl_profile.enabled=0;++linear_pl_profile.errors;if(timeout)++linear_pl_profile.timeouts;linear_pl_profile.last_status=uint32_t(status);publish();return -1;}
bool safe_address(const void*p,size_t n){const uint32_t a=ll_address(p);return a>=0x100000&&a<0x20000000&&n<=0x20000000-a;}
bool copy_bytes(void*dst,const void*src,size_t n,void(*service)(unsigned),const int*cancel){
 for(size_t i=0;i<n;i+=4096){if(cancelled(cancel))return false;const size_t take=std::min(size_t(4096),n-i);std::memcpy(static_cast<uint8_t*>(dst)+i,static_cast<const uint8_t*>(src)+i,take);if(service)service(6);}
 return !cancelled(cancel);
}
void pointer(unsigned r,const void*p){ll_write(r,ll_address(p));ll_write(r+4,0);}
}
void linear_pl_enable(bool v){if(!linear_pl_profile.faulted)linear_pl_profile.enabled=v;publish();}
bool linear_pl_faulted(){return linear_pl_profile.faulted!=0;}
int linear_pl_compute(const uint8_t*x,const int8_t*w,float*y,const float*b,float multiplier,
 unsigned rows,unsigned ci,unsigned co,int xzero,int wzero,void(*service)(unsigned),const int*cancel){
 if(linear_pl_profile.faulted)return -1;
 if(!linear_pl_profile.enabled)return 0;
 if(running||!x||!w||!y||!b)return -1;
 if(rows<1||rows>256||ci<1||ci>512||co<1||co>512||size_t(rows)*ci>LINEAR_INPUT||size_t(rows)*co>LINEAR_OUTPUT||xzero<0||xzero>255||wzero< -128||wzero>127){++linear_pl_profile.unsupported;publish();return 0;}
 struct Guard {Guard(){running=true;}~Guard(){running=false;}} guard;
 ++linear_pl_profile.calls;
 if(cancelled(cancel)){++linear_pl_profile.cancellations;publish();return -2;}
 const size_t ni=size_t(rows)*ci,nw=size_t(co)*ci,no=size_t(rows)*co;auto at=ll_clock();
 if(!copy_bytes(input_stage,x,ni,service,cancel)||!copy_bytes(weight_stage,w,nw,service,cancel)){++linear_pl_profile.cancellations;publish();return -2;}
 if(!safe_address(input_stage,ni)||!safe_address(weight_stage,nw)||!safe_address(output_stage,no*4))return fault(-10);
 ll_clean(input_stage,ni);ll_clean(weight_stage,nw);ll_clean(output_stage,no*4);ll_invalidate(output_stage,no*4);ll_barrier();linear_pl_profile.staging_us+=uint32_t(ll_clock()-at);
 if(cancelled(cancel)){++linear_pl_profile.cancellations;publish();return -2;}
 if(!(ll_read(0)&4))return fault(-11);
 pointer(0x20,input_stage);pointer(0x30,weight_stage);pointer(0x40,output_stage);
 ll_write(0x50,rows);ll_write(0x58,ci);ll_write(0x60,co);ll_write(0x68,uint32_t(xzero));ll_write(0x70,uint32_t(wzero));
 ll_barrier();at=ll_clock();ll_write(0,1);ll_barrier();auto last_service=at;
 for(;;){const auto status=ll_read(0);if(status&2)break;const auto now=ll_clock();if(now-at>2000000)return fault(-12,true);if(now-last_service>=2000){last_service=now;if(service)service(6);}}
 linear_pl_profile.waiting_us+=uint32_t(ll_clock()-at);ll_barrier();const int result=int(ll_read(0x10));linear_pl_profile.last_status=uint32_t(result);if(result!=LINEAR_OK)return fault(result);
 if(cancelled(cancel)){++linear_pl_profile.cancellations;publish();return -2;}
 ll_invalidate(output_stage,no*4);ll_barrier();at=ll_clock();
 for(unsigned r=0;r<rows;++r)for(unsigned o=0;o<co;++o){
  if(cancelled(cancel)){++linear_pl_profile.cancellations;publish();return -2;}
  // Same conversion, multiply then add as the existing model (-ffp-contract=off).
  y[r*co+o]=float(output_stage[r*co+o])*multiplier+b[o];
  if(o%32==31&&service)service(6);
 }
 if(service)service(6);
 if(cancelled(cancel)){++linear_pl_profile.cancellations;publish();return -2;}
 linear_pl_profile.copy_us+=uint32_t(ll_clock()-at);++linear_pl_profile.completed;linear_pl_profile.input_bytes+=uint32_t(ni+nw);linear_pl_profile.output_bytes+=uint32_t(no*4);publish();return 1;
}
#ifdef LINEAR_PL_MOCK
void linear_pl_test_reset(){running=false;std::memset(const_cast<LinearPlProfile*>(&linear_pl_profile),0,sizeof(linear_pl_profile));linear_pl_profile.magic=0x4c4c5031;linear_pl_profile.version=1;}
#endif
