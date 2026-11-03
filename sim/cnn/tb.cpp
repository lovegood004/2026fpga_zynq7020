#include "conv.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <cstdlib>
#include <string>
static uint32_t state=0x29f3a1u;
static float value(){state=1664525u*state+1013904223u;return float(int(state>>16)-32768)/65536.f;}
static bool run(int ci,int co,int h,int w,bool replay=false){
    int oh=(h+1)/2,ow=(w+1)/2;
    std::vector<float>x(std::max(ci*h*w,131072)),k(std::max(co*ci*9,32768)),b(256),y(std::max(co*oh*ow,32768)+2,321.25f);
    for(auto &v:x)v=value()*4;for(auto &v:k)v=value();for(auto &v:b)v=value();
    if(replay){
        const char*dir=std::getenv("LUNG_REPLAY_DIR");if(!dir){puts("MISSING_REQUIRED_REAL_REPLAY");return false;}
        const char*names[]={"input_a.bin","weight_a.bin","bias_a.bin"};
        float*dest[]={x.data(),k.data(),b.data()};const unsigned counts[]={unsigned(ci*h*w),unsigned(co*ci*9),unsigned(co)};
        for(unsigned i=0;i<3;i++){const std::string file=std::string(dir)+"/"+names[i];FILE*f=std::fopen(file.c_str(),"rb");if(!f||std::fread(dest[i],4,counts[i],f)!=counts[i])return false;std::fclose(f);}
    }
    int code=lung_conv_fp32(x.data(),k.data(),b.data(),y.data()+1,ci,co,h,w);
    if(code!=LUNG_CONV_OK||y.front()!=321.25f||y.back()!=321.25f||y[co*oh*ow+1]!=321.25f)return false;
    float maximum=0;
    for(int o=0;o<co;o++)for(int yy=0;yy<oh;yy++)for(int xx=0;xx<ow;xx++){
        float sum=0;
        for(int c=0;c<ci;c++)for(int ky=0;ky<3;ky++){
            int sy=yy*2+ky-1;if(sy<0||sy>=h)continue;
            for(int kx=0;kx<3;kx++){
                int sx=xx*2+kx-1;if(sx<0||sx>=w)continue;
                sum+=x[(c*h+sy)*w+sx]*k[((o*ci+c)*3+ky)*3+kx];
            }
        }
        float expected=sum+b[o],actual=y[1+(o*oh+yy)*ow+xx];
        maximum=std::max(maximum,std::fabs(expected-actual));
        if(!std::isfinite(actual)||std::fabs(actual-expected)>2e-5f+2e-5f*std::fabs(expected))return false;
    }
    std::printf("CONV_CASE ci=%d co=%d h=%d w=%d max_abs=%.9g\n",ci,co,h,w,maximum);
    return true;
}
int main(){
#ifdef LUNG_FULL_CSIM
    const int cases[][4]={{1,32,128,468},{32,64,64,234},{64,128,32,117},{128,256,16,59},
                         {1,32,128,269},{32,64,64,135},{64,128,32,68},{128,256,16,34}};
#else
    const int cases[][4]={{1,1,1,1},{1,3,3,3},{3,2,5,17},{32,2,3,65},{64,2,3,33},{128,2,3,59},{3,3,7,61},{2,3,5,63},{3,3,7,65},{2,3,5,129},{1,2,5,257},{128,4,3,59},{32,8,7,65},{128,8,5,59},{128,3,3,64},{127,3,3,64},{17,5,5,467}};
#endif
    for(const auto &c:cases)if(!run(c[0],c[1],c[2],c[3])){puts("LUNG_CONV_FAIL numerical");return 1;}
#ifndef LUNG_FULL_CSIM
    if(!run(128,2,16,59,true)){puts("REAL_STAGE_REPLAY_FAIL");return 4;}
    puts("REAL_STAGE_REPLAY_PASS captured 128x16x59, first two channels, all 8 output rows");
#endif
    std::vector<float>x(131072),k(32768),b(256),output(32768,731.25f);
    const int bad[][4]={{0,1,1,1},{129,1,1,1},{1,257,1,1},{1,1,129,1},{1,1,1,469},{128,1,128,468},{1,256,128,468}};
    for(const auto &c:bad)if(lung_conv_fp32(x.data(),k.data(),b.data(),output.data(),c[0],c[1],c[2],c[3])!=-1||output.front()!=731.25f||output.back()!=731.25f)return 2;
    puts("LUNG_CONV_PASS ordered FP32, padding, tails, all channel stages, invalid geometry, output guards");return 0;
}
