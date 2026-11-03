#include "dot.hpp"
#include <algorithm>
#include <cassert>
#include <cstdio>
#include <vector>
static uint32_t state=1729;
static unsigned rng(){state=1664525*state+1013904223;return state;}
static void run(int rows,int ci,int co,int xz,int wz,bool extreme=false){
 std::vector<uint8_t>x(LINEAR_INPUT,0xa5);std::vector<int8_t>w(LINEAR_WEIGHT,37);
 std::vector<int32_t>y(LINEAR_OUTPUT+2,0x13579bdf),ref(rows*co);
 for(int i=0;i<rows*ci;++i)x[i]=extreme?255:uint8_t(rng()>>16);
 for(int i=0;i<co*ci;++i)w[i]=extreme?-128:int8_t(rng()>>16);
 const auto before_x=x;const auto before_w=w;
 for(int r=0;r<rows;++r)for(int o=0;o<co;++o){int64_t sum=0;for(int k=0;k<ci;++k)sum+=int64_t(int(x[r*ci+k])-xz)*(int(w[o*ci+k])-wz);assert(sum>=INT32_MIN&&sum<=INT32_MAX);ref[r*co+o]=int32_t(sum);}
 assert(lung_linear_int8(x.data(),w.data(),y.data()+1,rows,ci,co,xz,wz)==LINEAR_OK);
 assert(x==before_x&&w==before_w);
 for(unsigned i=0;i<ref.size();++i)assert(y[i+1]==ref[i]);
 assert(y[0]==0x13579bdf);for(unsigned i=ref.size()+1;i<y.size();++i)assert(y[i]==0x13579bdf);
 printf("LINEAR_CASE rows=%d ci=%d co=%d xz=%d wz=%d exact=1\n",rows,ci,co,xz,wz);
}
int main(){
#ifdef LINEAR_FULL_CSIM
 run(64,256,512,71,0);run(64,512,256,127,-3);
 run(256,64,128,0,0);run(256,128,64,100,4);
 run(64,512,512,0,127,true);
#endif
 run(1,1,1,0,0);run(3,17,9,123,-7);run(7,65,17,255,127);
 run(64,256,9,71,0);run(64,512,9,127,-3);
 run(256,64,9,0,0);run(256,128,9,100,4);run(1,512,9,0,127,true);
 run(1,512,512,19,-7);run(64,1,512,255,-128);
 const int bad[][5]={{0,1,1,0,0},{257,1,1,0,0},{1,0,1,0,0},{1,513,1,0,0},{1,1,0,0,0},{1,1,513,0,0},{256,512,1,0,0},{256,1,512,0,0},{1,1,1,-1,0},{1,1,1,256,0},{1,1,1,0,-129},{1,1,1,0,128}};
 for(const auto &v:bad){std::vector<uint8_t>x(LINEAR_INPUT,0xa5);std::vector<int8_t>w(LINEAR_WEIGHT,37);std::vector<int32_t>y(LINEAR_OUTPUT,1729);assert(lung_linear_int8(x.data(),w.data(),y.data(),v[0],v[1],v[2],v[3],v[4])==-1);assert(std::all_of(y.begin(),y.end(),[](int32_t v){return v==1729;}));}
 puts("LINEAR_C_RTL_PASS exact integer dot, tails, zero points, overflow bound, guards, invalid sizes");
}
