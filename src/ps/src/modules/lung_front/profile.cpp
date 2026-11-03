#include "profile.hpp"
#include <cstring>
#ifdef __arm__
extern "C" {
#include "xil_cache.h"
}
#endif
extern "C" {unsigned lung_front_optimized=1,lung_front_trace=0;alignas(64) volatile front_profile::Report lung_front_report;}
namespace front_profile {
Report state;Fft*record=nullptr;static uint64_t(*clock_fn)()=nullptr;static uint32_t sequence=0;
uint64_t now(){return clock_fn?clock_fn():0;}
static void flush(){
#ifdef __arm__
 Xil_DCacheFlushRange((UINTPTR)&lung_front_report,sizeof(lung_front_report));asm volatile("dsb sy":::"memory");
#endif
}
void begin(uint64_t(*clock)()){state={};record=nullptr;clock_fn=lung_front_trace?clock:nullptr;publish(false,0,0);}
void end(){clock_fn=nullptr;record=nullptr;}
void publish(bool valid,unsigned ready,uint64_t bytes){
 auto r=state;r.optimized=lung_front_optimized;r.tracing=lung_front_trace;r.valid=valid;r.cache_ready=ready;r.retained_heap_bytes=bytes;
 const uint32_t odd=sequence+1,even=sequence+2;lung_front_report.sequence=odd;lung_front_report.sequence_end=odd;flush();
 r.sequence=r.sequence_end=odd;std::memcpy(const_cast<Report*>(&lung_front_report),&r,sizeof(r));flush();
 lung_front_report.sequence=even;lung_front_report.sequence_end=even;sequence=even;flush();
}
}
