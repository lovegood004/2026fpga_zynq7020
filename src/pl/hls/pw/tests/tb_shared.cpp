#include "pw_shared.hpp"
#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <cstdlib>
static int calls=0,failures=0;static unsigned long long checked=0;static float maxscaled=0;
static std::vector<float> read(const std::string &p,size_t count){
 FILE*f=fopen(p.c_str(),"rb");if(!f){perror(p.c_str());std::exit(2);}std::vector<float>v(count);
 if(fread(v.data(),4,count,f)!=count){fclose(f);std::exit(2);}fclose(f);return v;
}
static void test(const std::vector<float>&w,const std::vector<float>&x,const std::vector<float>&gold,int ci,int co,int len,const char*name){
 // HLS co-sim snapshots every declared m_axi depth, even for shorter requests.
 // Provide full backing allocations so its instrumentation cannot overread.
 std::vector<float>wb(9216,0),xb(196608,0),out(196608+2,-9999.0f);
 std::copy(w.begin(),w.end(),wb.begin());std::copy(x.begin(),x.end(),xb.begin());
 int status=pw_shared(wb.data(),xb.data(),out.data()+1,ci,co,len);++calls;
 if(status||out.front()!=-9999.0f||out[co*len+1]!=-9999.0f||out.back()!=-9999.0f){++failures;return;}
 for(int i=0;i<co*len;++i){float d=std::fabs(out[i+1]-gold[i]);
  // Forward error bound for two FP32 dot products with different summation
  // orders. Cancellation makes relative-to-result alone misleading.
  double sumabs=0;for(int k=0;k<ci;++k)sumabs+=std::fabs(double(w[(i/len)*ci+k])*x[k*len+i%len]);
  double gamma=(ci*5.960464477539063e-8)/(1-ci*5.960464477539063e-8);
  double lim=2e-6+2*gamma*sumabs;
  if(!std::isfinite(out[i+1])||!std::isfinite(gold[i])||d>lim){if(failures<5)std::fprintf(stderr,"FAIL %s [%d] %g vs %g\n",name,i,out[i+1],gold[i]);++failures;}
  maxscaled=std::max(maxscaled,d/(1+std::fabs(gold[i])));++checked;
 }
 // Save independently checkable expected outputs for raw RTL transaction review.
 const char*export_dir=std::getenv("MH_EXPECTED_DIR");
 if(export_dir){char p[512];sprintf(p,"%s/call_%02d.bin",export_dir,calls-1);FILE*f=fopen(p,"wb");if(!f)std::exit(2);fwrite(gold.data(),4,gold.size(),f);fclose(f);}
}
int main(int argc,char**argv){
 if(argc<2)return 2;std::string root=argv[1];bool rtl=argc>2;
 FILE*f=fopen((root+"/layers.txt").c_str(),"r");if(!f)return 2;int id,ci,co,len;char p[128];
 // All captured layers in C; RTL uses full first layer and up to 65 samples of
 // every other layer, preserving real values/strides and exercising tile tails.
 while(fscanf(f,"%d %d %d %d",&id,&ci,&co,&len)==4){
  sprintf(p,"/layer_%02d_w.bin",id);auto w=read(root+p,ci*co);
  sprintf(p,"/layer_%02d_x.bin",id);auto original_x=read(root+p,ci*len);
  sprintf(p,"/layer_%02d_y.bin",id);auto original_y=read(root+p,co*len);
  int n=rtl&&id!=0?std::min(len,65):len;
  std::vector<float>x(ci*n),gold(co*n);
  for(int k=0;k<ci;++k)for(int j=0;j<n;++j)x[k*n+j]=original_x[k*len+j];
  for(int m=0;m<co;++m)for(int j=0;j<n;++j)gold[m*n+j]=original_y[m*len+j];
  sprintf(p,"real_%02d",id);test(w,x,gold,ci,co,n,p);
 }
 fclose(f);
 const int shapes[][3]={{1,1,1},{3,5,63},{7,9,65},{95,95,67},{96,96,64},{18,19,5}};
 for(int mode=0;mode<6;++mode){ci=shapes[mode][0];co=shapes[mode][1];len=shapes[mode][2];
  std::vector<float>w(ci*co),x(ci*len),gold(co*len);
  for(int i=0;i<ci*co;++i)w[i]=mode==0?0.0f:((i*7%19)-9)*0.0625f;
  for(int i=0;i<ci*len;++i)x[i]=mode==1?0.0f:((i*13%31)-15)*(mode==5?0.0001f:0.03125f);
  for(int m=0;m<co;++m)for(int n=0;n<len;++n){float s=0;for(int k=0;k<ci;++k)s+=w[m*ci+k]*x[k*len+n];gold[m*len+n]=s;}
  test(w,x,gold,ci,co,len,"synthetic");
 }
 std::vector<float> iw(9216,-123.0f),ix(196608,-123.0f),iy(196608,-123.0f);
 const int invalid[][3]={{0,4,64},{97,4,64},{4,0,64},{4,97,64},{4,4,0},{4,4,2049},{-1,4,64}};
 for(auto &s:invalid){if(pw_shared(iw.data(),ix.data(),iy.data(),s[0],s[1],s[2])!=-1||!std::all_of(iy.begin(),iy.end(),[](float v){return v==-123.0f;}))++failures;++calls;}
 printf("{\"test\":\"pw_shared\",\"rtl\":%s,\"calls\":%d,\"outputs_checked\":%llu,\"failures\":%d,\"max_scaled_error\":%.9g}\n",rtl?"true":"false",calls,checked,failures,maxscaled);
 return failures?1:0;
}
