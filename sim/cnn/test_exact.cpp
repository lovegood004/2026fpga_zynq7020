#include "conv.hpp"
#include <cassert>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <string>
#include <cstdlib>
extern "C" int lung_conv_reference(const float*,const float*,const float*,float*,int,int,int,int);
static uint32_t rng=0x717aa31;
static float next(){rng=rng*1664525u+1013904223u;return float(int(rng>>16)-32768)/65536.f;}
static void test(int ci,int co,int h,int w,bool captured=false){
 int oh=(h+1)/2,ow=(w+1)/2;size_t n=size_t(co)*oh*ow;
 std::vector<float>x(size_t(ci)*h*w),weight(size_t(co)*ci*9),bias(co),old(n+2,731.25f),fresh(n+2,731.25f);
 for(auto&v:x)v=next()*4;for(auto&v:weight)v=next();for(auto&v:bias)v=next();
 if(captured){const char*dir=std::getenv("LUNG_REPLAY_DIR");assert(dir);const char*names[]={"input_a.bin","weight_a.bin","bias_a.bin"};
  float*ptr[]={x.data(),weight.data(),bias.data()};size_t count[]={x.size(),weight.size(),bias.size()};
  for(unsigned i=0;i<3;++i){std::string path=std::string(dir)+"/"+names[i];FILE*f=std::fopen(path.c_str(),"rb");assert(f&&std::fread(ptr[i],4,count[i],f)==count[i]);std::fclose(f);}}
 assert(lung_conv_reference(x.data(),weight.data(),bias.data(),old.data()+1,ci,co,h,w)==LUNG_CONV_OK);
 assert(lung_conv_fp32(x.data(),weight.data(),bias.data(),fresh.data()+1,ci,co,h,w)==LUNG_CONV_OK);
 assert(!std::memcmp(old.data(),fresh.data(),fresh.size()*4)&&fresh.front()==731.25f&&fresh.back()==731.25f);
 std::printf("FLOW_BIT_EXACT ci=%d co=%d h=%d w=%d captured=%d\n",ci,co,h,w,int(captured));
}
int main(){setvbuf(stdout,nullptr,_IONBF,0);
 const int cases[][4]={{1,32,128,468},{32,64,64,234},{64,128,32,117},{128,256,16,59},
 {1,32,128,269},{32,64,64,135},{64,128,32,68},{128,256,16,34},
 {1,1,1,1},{1,3,3,3},{3,2,5,17},{32,2,3,65},{64,2,3,33},{128,2,3,59},
 {3,3,7,61},{2,3,5,63},{3,3,7,65},{2,3,5,129},{1,2,5,257},
 {128,4,3,59},{32,8,7,65},{128,8,5,59},{128,3,3,64},{127,3,3,64},{17,5,5,467}};
 for(const auto&c:cases)test(c[0],c[1],c[2],c[3]);test(128,2,16,59,true);
 float sentinel=731.25f;
 const int bad[][4]={{0,1,1,1},{129,1,1,1},{1,257,1,1},{1,1,129,1},{1,1,1,469},{128,1,128,468},{1,256,128,468}};
 for(const auto&c:bad){assert(lung_conv_fp32(&sentinel,&sentinel,&sentinel,&sentinel,c[0],c[1],c[2],c[3])==-1);assert(sentinel==731.25f);}
 puts("FLOW_HOST_PASS bit exact complete layers/odd shapes/captured input/invalid guards; not RTL timing");
}
