#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <setjmp.h>
#include "../display/assets/c_impl/hsseg_deform_seddl.h"
#ifndef FEATURE_CYCLE_ARENA_BYTES
#define FEATURE_CYCLE_ARENA_BYTES (4*1024*1024)
#endif
enum { ARENA_CAPACITY=FEATURE_CYCLE_ARENA_BYTES };
static union { uint64_t align; unsigned char bytes[ARENA_CAPACITY]; } arena;
static size_t used, highwater;
static jmp_buf allocation_failure;
static void (*cycle_service)(unsigned);
static const int *cycle_cancelled;
static void cycle_yield(void){
    if(cycle_service)cycle_service(9);
    if(cycle_cancelled&&*cycle_cancelled)longjmp(allocation_failure,2);
}
static void *bounded_alloc(size_t bytes) {
    size_t aligned=(bytes+7u)&~(size_t)7u;
    if(aligned<bytes || aligned>ARENA_CAPACITY-used) longjmp(allocation_failure,1);
    void *p=arena.bytes+used;used+=aligned;
    if(used>highwater)highwater=used;
    return p;
}
static void arena_free(void *p){(void)p;}
static void *bounded_calloc(size_t count,size_t size){
    if(size&&count>SIZE_MAX/size)longjmp(allocation_failure,1);
    size_t bytes=count*size;void *p=bounded_alloc(bytes);memset(p,0,bytes);return p;
}
#define malloc bounded_alloc
#define calloc bounded_calloc
#define free arena_free
#define getenv(x) ((char *)0)
#define clock_gettime(clock_id,ts) (((ts)->tv_sec=0),((ts)->tv_nsec=0),0)
#define hsseg_model_load feature_cycle_unused_file_loader
#define hsseg_model_free feature_cycle_unused_file_free
#define hsseg_segment feature_cycle_senior_segment
#define hsseg_segment_timed feature_cycle_unused_timed
#define hsseg_segment_profile feature_cycle_unused_profile
#define g_model feature_cycle_model_context
#define HSSEG_YIELD() cycle_yield()
#include "../display/assets/c_impl/hsseg_deform_seddl.c"
#undef malloc
#undef calloc
#undef free
#include "cycle_weights.inc"
int feature_cycle_infer_chunk(const float *audio,int *states,size_t *peak) {
    extern int feature_cycle_infer_chunk_serviced(const float*,int*,size_t*,void(*)(unsigned),const int*);
    return feature_cycle_infer_chunk_serviced(audio,states,peak,NULL,NULL);
}
int feature_cycle_infer_chunk_serviced(const float *audio,int *states,size_t *peak,void(*service)(unsigned),const int*cancelled) {
    hsseg_model_t model;memset(&model,0,sizeof(model));bind_original_weights(&model);
    used=0;highwater=0;
    if(setjmp(allocation_failure)){cycle_service=NULL;cycle_cancelled=NULL;if(peak)*peak=highwater;return 0;}
    cycle_service=service;cycle_cancelled=cancelled;cycle_yield();
    int ok=feature_cycle_senior_segment(&model,audio,10000,states,500)==0;
    if(peak)*peak=highwater;
    cycle_service=NULL;cycle_cancelled=NULL;
    return ok;
}
