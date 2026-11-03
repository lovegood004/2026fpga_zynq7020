#include "pw_mmio.h"
#include "pw_profile.h"
#include "xparameters.h"
#include "xpw_shared_hw.h"
#include "xil_io.h"
#include "xil_cache.h"
#include "xtime_l.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>
extern "C" int ref_conv1d_strided(const float*,int,int,const float*,int,int,int,int,int,float*);
extern "C" uint32_t pw_hal_read(uintptr_t p){return Xil_In32(p);}
extern "C" void pw_hal_write(uintptr_t p,uint32_t v){Xil_Out32(p,v);}
extern "C" uint64_t pw_hal_ticks(){XTime t;XTime_GetTime(&t);return t;}
extern "C" void pw_hal_barrier(){dsb();}
// Dedicated cache-line aligned output: invalidation never discards another
// application's dirty data. Input/weights are read-only during synchronous call.
static float output_scratch[96*2048] __attribute__((aligned(64)));
#ifndef XPAR_PW_SHARED_0_S_AXI_CONTROL_BASEADDR
#error "Build with the new PS+PL platform, not the old PS-only BSP"
#endif
static PwDevice device={XPAR_PW_SHARED_0_S_AXI_CONTROL_BASEADDR,0};
static bool use_pl=false;
static bool scratch_clean=false;
extern "C" {volatile PwProfile pw_profile={0x50575031,1,0,0,0,0,COUNTS_PER_SECOND,0,0,0,0,0,0,0,0,0};}
static_assert(PW_CTRL==XPW_SHARED_CONTROL_ADDR_AP_CTRL && PW_RETURN==XPW_SHARED_CONTROL_ADDR_AP_RETURN,
              "Control map differs from the generated platform");
static_assert(PW_W==XPW_SHARED_CONTROL_ADDR_WEIGHTS_DATA && PW_X==XPW_SHARED_CONTROL_ADDR_INPUT_R_DATA &&
              PW_Y==XPW_SHARED_CONTROL_ADDR_OUTPUT_R_DATA && PW_CI==XPW_SHARED_CONTROL_ADDR_CI_DATA &&
              PW_CO==XPW_SHARED_CONTROL_ADDR_CO_DATA && PW_LEN==XPW_SHARED_CONTROL_ADDR_LENGTH_R_DATA,
              "Argument map differs from the generated platform");
// BSP range maintenance masks IRQ for its whole call. Bound each call so UART
// can drain between blocks; keep original barriers and cache-line semantics.
static void clean_range(UINTPTR p,unsigned bytes){
 while(bytes){unsigned n=bytes>1024?1024:bytes;Xil_DCacheFlushRange(p,n);p+=n;bytes-=n;}
}
static void invalidate_output(unsigned bytes){
 for(unsigned offset=0;offset<bytes;offset+=1024)
  Xil_DCacheInvalidateRange((UINTPTR)output_scratch+offset,bytes-offset>1024?1024:bytes-offset);
}
extern "C" void shared_arm_enable(int enabled){use_pl=enabled!=0;}
extern "C" int shared_model(const float*w,const float*x,float*y,int ci,int co,int len){
 if(!use_pl)return ref_conv1d_strided(x,ci,len,w,co,1,1,0,1,y);
 // Validate BEFORE cache ranges and unsigned size arithmetic.
 if(!w||!x||!y||ci<1||ci>96||co<1||co>96||len<1||len>2048){
  ++pw_profile.errors;pw_profile.phase=99;std::abort();
 }
 uint64_t began=pw_hal_ticks(),mark=began;
 ++pw_profile.sequence;pw_profile.phase=10;
 unsigned bytes=co*len*sizeof(float);
 clean_range((UINTPTR)w,ci*co*sizeof(float));
 clean_range((UINTPTR)x,ci*len*sizeof(float));
 pw_profile.input_clean+=pw_hal_ticks()-mark;mark=pw_hal_ticks();pw_profile.phase=11;
 // BSS may be dirty on first use. Clean the WHOLE dedicated buffer once,
 // including ranges used only by later larger layers. CPU never stores to it.
 if(!scratch_clean){clean_range((UINTPTR)output_scratch,sizeof(output_scratch));scratch_clean=true;}
 pw_hal_barrier();
 pw_profile.output_prepare+=pw_hal_ticks()-mark;pw_profile.phase=12;
 PwSubmitTiming timing{};
 int status=pw_submit_timed(&device,(uint32_t)(uintptr_t)w,(uint32_t)(uintptr_t)x,
                      (uint32_t)(uintptr_t)output_scratch,ci,co,len,COUNTS_PER_SECOND*2ULL,&timing);
 pw_profile.setup+=timing.setup_ticks;pw_profile.wait+=timing.wait_ticks;
 if(status!=PW_OK){
  ++pw_profile.errors;pw_profile.phase=99;++pw_profile.sequence;pw_profile.sequence_end=pw_profile.sequence;
  // Fail closed. Never race CPU fallback against a possibly running AXI master.
  std::printf("{\"type\":\"pl_error\",\"code\":%d,\"action\":\"stop_reset_required\"}\n",status);
  std::fflush(stdout);std::abort();
 }
 mark=pw_hal_ticks();pw_profile.phase=13;
 invalidate_output((bytes+63)&~63U);
 pw_hal_barrier();pw_profile.output_invalidate+=pw_hal_ticks()-mark;
 mark=pw_hal_ticks();pw_profile.phase=14;std::memcpy(y,output_scratch,bytes);
 pw_profile.copy+=pw_hal_ticks()-mark;pw_profile.total+=pw_hal_ticks()-began;
 ++pw_profile.calls;pw_profile.phase=0;++pw_profile.sequence;pw_profile.sequence_end=pw_profile.sequence;return len;
}
