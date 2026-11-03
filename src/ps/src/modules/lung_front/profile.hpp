#pragma once
#include <cstdint>
namespace front_profile {
struct Fft {uint64_t alloc_us=0,roots_us=0,transform_us=0,copy_us=0,total_us=0;uint32_t n=0,root_hit=0,scratch_reused=0,error=0;};
struct Report {uint32_t magic=0x46524e31,version=1,sequence=0,optimized=0,tracing=0,valid=0,cache_ready=0,sequence_end=0;
 Fft fft[2];uint64_t resample_us=0,stft_us=0,normalize_us=0,wave_us=0,service_us=0,total_us=0;
 uint32_t fft_calls=0,stft_frames=0,root_builds=0,stft_plan_builds=0;uint64_t retained_heap_bytes=0,samples=0,rate=0,prepare_us=0,reserved[2]{};};
static_assert(sizeof(Report)==256,"Front profile ABI");
extern Report state;extern Fft*record;
uint64_t now();void begin(uint64_t(*clock)());void publish(bool valid,unsigned ready,uint64_t bytes);void end();
}
extern "C" {extern unsigned lung_front_optimized,lung_front_trace;extern volatile front_profile::Report lung_front_report;void lung_front_cache_clear();uint64_t lung_front_cache_bytes();}
