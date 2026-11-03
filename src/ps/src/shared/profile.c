#include "tensor.h"
#include "profile.h"
#include "esp_timer.h"
#include <stdio.h>
#include <stdint.h>
#ifdef MH_PL_MODEL
#include "model_arm.h"
#define active_dw model_dw
#define active_bn model_bn
#else
#define active_dw ref_conv1d_dw_strided
#define active_bn ref_batchnorm1d
#endif
static uint64_t elapsed[9],calls[9],macs[9];
static int capture=0;
enum { PW,CONV,DW,BN,RELU,LINEAR,POOL,COPY,SIGMOID };
static const char *names[]={"pointwise","conv","depthwise","batchnorm","relu","linear","pool","copy","sigmoid"};
void profile_reset(void){memset(elapsed,0,sizeof(elapsed));memset(calls,0,sizeof(calls));memset(macs,0,sizeof(macs));}
void profile_set_capture(int enable){capture=enable;}
void profile_snapshot_us(uint32_t out[9]){for(int k=0;k<9;++k)out[k]=(uint32_t)elapsed[k];}
static void record(int kind,int64_t start,uint64_t operations){elapsed[kind]+=esp_timer_get_time()-start;++calls[kind];macs[kind]+=operations;}
void profile_report(const char *name){for(int k=0;k<9;++k) printf("{\"type\":\"profile\",\"name\":\"%s\",\"op\":\"%s\",\"us\":%llu,\"calls\":%llu,\"macs\":%llu}\n",name,names[k],(unsigned long long)elapsed[k],(unsigned long long)calls[k],(unsigned long long)macs[k]);}
extern int ref_conv1d(const float*,int,int,const float*,int,int,int,int,float*);
extern int ref_conv1d_strided(const float*,int,int,const float*,int,int,int,int,int,float*);
extern int ref_conv1d_dw(const float*,int,int,const float*,int,int,int,float*);
extern int ref_conv1d_dw_strided(const float*,int,int,const float*,int,int,int,int,float*);
extern void ref_batchnorm1d(float*,int,int,const float*,const float*,const float*,const float*,float);
extern void ref_relu_(float*,int);
extern void ref_sigmoid_(float*,int);
extern void ref_linear(const float*,int,int,const float*,const float*,float*);
extern int ref_avgpool1d(const float*,int,int,int,int,float*);
extern void ref_adaptive_avgpool1d_to1(const float*,int,int,float*);
extern int ref_cat_channels(const float*,int,int,const float*,int,int,float*);
#ifdef MH_BLAS_MODEL
extern int shared_model(const float*,const float*,float*,int,int,int);
#endif
#ifdef MH_CAPTURE
static void dump(const char *name,const float *v,size_t n){FILE*f=fopen(name,"wb");if(!f){perror(name);return;}if(fwrite(v,4,n,f)!=n)perror("fwrite");fclose(f);}
#endif
int conv1d_strided(const float*x,int ci,int len,const float*w,int co,int k,int d,int p,int s,float*y){
 int kind=k==1&&d==1&&p==0&&s==1?PW:CONV;
 int64_t t=esp_timer_get_time();int n;
#ifdef MH_BLAS_MODEL
 if(kind==PW) n=shared_model(w,x,y,ci,co,len);
 else
#endif
 n=ref_conv1d_strided(x,ci,len,w,co,k,d,p,s,y);
 record(kind,t,(uint64_t)ci*co*k*(n>0?n:0));
#ifdef MH_CAPTURE
 if(capture&&kind==PW){
  static int layer=0;char path[128];
  printf("{\"type\":\"layer\",\"id\":%d,\"ci\":%d,\"co\":%d,\"length\":%d}\n",layer,ci,co,len);
  sprintf(path,"data/layer_%02d_x.bin",layer);dump(path,x,ci*len);
  sprintf(path,"data/layer_%02d_w.bin",layer);dump(path,w,ci*co);
  sprintf(path,"data/layer_%02d_y.bin",layer);dump(path,y,co*n);++layer;
 }

#endif
 return n;
}
int conv1d(const float*x,int ci,int l,const float*w,int co,int k,int d,int p,float*y){return conv1d_strided(x,ci,l,w,co,k,d,p,1,y);}
int conv1d_dw_strided(const float*x,int c,int len,const float*w,int k,int d,int p,int s,float*y){int64_t t=esp_timer_get_time();int n=active_dw(x,c,len,w,k,d,p,s,y);record(DW,t,(uint64_t)c*k*(n>0?n:0));return n;}
int conv1d_dw(const float*x,int c,int len,const float*w,int k,int d,int p,float*y){return conv1d_dw_strided(x,c,len,w,k,d,p,1,y);}
void batchnorm1d(float*x,int c,int n,const float*g,const float*b,const float*m,const float*v,float e){int64_t t=esp_timer_get_time();active_bn(x,c,n,g,b,m,v,e);record(BN,t,0);}
void relu_(float*x,int n){int64_t t=esp_timer_get_time();ref_relu_(x,n);record(RELU,t,0);}
void sigmoid_(float*x,int n){int64_t t=esp_timer_get_time();ref_sigmoid_(x,n);record(SIGMOID,t,0);}
void linear(const float*x,int ci,int co,const float*w,const float*b,float*y){int64_t t=esp_timer_get_time();ref_linear(x,ci,co,w,b,y);record(LINEAR,t,(uint64_t)ci*co);}
int avgpool1d(const float*x,int c,int l,int k,int s,float*y){int64_t t=esp_timer_get_time();int n=ref_avgpool1d(x,c,l,k,s,y);record(POOL,t,0);return n;}
void adaptive_avgpool1d_to1(const float*x,int c,int l,float*y){int64_t t=esp_timer_get_time();ref_adaptive_avgpool1d_to1(x,c,l,y);record(POOL,t,0);}
int cat_channels(const float*a,int ac,int al,const float*b,int bc,int bl,float*y){int64_t t=esp_timer_get_time();int n=ref_cat_channels(a,ac,al,b,bc,bl,y);record(COPY,t,0);return n;}
