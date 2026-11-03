#include "profile.hpp"
#include <cstring>
#ifdef __arm__
extern "C" {
#include "xil_cache.h"
}
#endif
extern "C" {unsigned lung_gru_optimized=1,lung_gru_trace=0;alignas(64) volatile gru_profile::Report lung_gru_report;}
namespace gru_profile {
Report state;static uint32_t sequence=0;
static void flush(){
#ifdef __arm__
 Xil_DCacheFlushRange((UINTPTR)&lung_gru_report,sizeof(lung_gru_report));asm volatile("dsb sy":::"memory");
#endif
}
void publish(bool valid){
 auto r=state;r.optimized=lung_gru_optimized;r.tracing=lung_gru_trace;r.valid=valid;
 const auto odd=sequence+1,even=sequence+2;
 lung_gru_report.sequence=odd;lung_gru_report.sequence_end=odd;flush();
 r.sequence=r.sequence_end=odd;std::memcpy(const_cast<Report*>(&lung_gru_report),&r,sizeof(r));flush();
 lung_gru_report.sequence=even;lung_gru_report.sequence_end=even;sequence=even;flush();
}
void reset(){state={};publish(false);}
}
