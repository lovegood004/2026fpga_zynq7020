#include "../modules/lung_gru/profile.hpp"
#include "lung_pl.hpp"
#include "linear_pl.hpp"
/* Portable eval-mode CNN/Mixer/BiGRU. Linear uses checkpoint INT8 weights and
 * whole-input dynamic uint8 activation quantization (FBGEMM reduce_range).
 * No dependency on libtorch, filesystem, host SIMD, PL or Python on the board.
 */
#include "lung_release.hpp"
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
namespace lung_release {
namespace {
Trace tracer=nullptr;void *trace_context=nullptr;
void trace(const char*n,const float*x,unsigned size){if(tracer)tracer(n,x,size,trace_context);}
struct Workspace {
    float a[32*64*((MaxFrames+1)/2)],b[32*64*((MaxFrames+1)/2)];
    float x[16384],norm[16384],trans[16384],hidden[32768],tmp[16384];
    float sequence[16384],gates[64*384],state[128],rh[384],out[16384];
    uint8_t quant[32768];
};
bool active=false;
const float* floats(const Weight*w){return w&&!w->quantized?reinterpret_cast<const float*>(weight_blob+w->offset):nullptr;}
const float* value(const char*n){return floats(find(n));}
bool conv(const float *x,float*y,unsigned ci,unsigned co,unsigned height,unsigned width,const char *prefix,const Work &work){
    char n[80];std::snprintf(n,sizeof(n),"%s.weight",prefix);const float*w=value(n);
    std::snprintf(n,sizeof(n),"%s.bias",prefix);const float*b=value(n);if(!w||!b)return false;
    const int pl=lung_pl_conv(x,w,b,y,ci,co,height,width,work.service,work.control?work.control->cancelled:nullptr);
    if(pl!=0)return pl==1;
    const unsigned oh=(height+1)/2,ow=(width+1)/2;unsigned budget=0;
    for(unsigned o=0;o<co;o++)for(unsigned yy=0;yy<oh;yy++){
        unsigned xx=0;
        while(xx<ow){
            // Four independent output sums share weights. Each sum retains the
            // original channel/ky/kx order; no fast-math, FMA or reassociation.
            if(xx>0&&xx+3<ow&&(xx+3)*2+1<width){
                float s0=0,s1=0,s2=0,s3=0;
                for(unsigned c=0;c<ci;c++)for(int ky=0;ky<3;ky++){
                    const int sy=int(yy*2)+ky-1;if(sy<0||sy>=int(height))continue;
                    const float *p=x+(c*height+unsigned(sy))*width+xx*2-1;
                    const float *v=w+((o*ci+c)*3+unsigned(ky))*3;
                    for(unsigned k=0;k<3;k++){
                        const float weight=v[k];s0+=p[k]*weight;s1+=p[k+2]*weight;
                        s2+=p[k+4]*weight;s3+=p[k+6]*weight;
                    }
                }
                float *p=y+(o*oh+yy)*ow+xx;p[0]=s0+b[o];p[1]=s1+b[o];p[2]=s2+b[o];p[3]=s3+b[o];
                xx+=4;budget+=ci*36;
            }else{
                float sum=0;
                for(unsigned c=0;c<ci;c++)for(int ky=0;ky<3;ky++){
                    const int sy=int(yy*2)+ky-1;if(sy<0||sy>=int(height))continue;
                    for(int kx=0;kx<3;kx++){
                        const int sx=int(xx*2)+kx-1;if(sx<0||sx>=int(width))continue;
                        sum+=x[(c*height+unsigned(sy))*width+unsigned(sx)]*w[((o*ci+c)*3+unsigned(ky))*3+unsigned(kx)];
                    }
                }
                y[(o*oh+yy)*ow+xx]=sum+b[o];++xx;budget+=ci*9;
            }
            if(budget>=32768){budget=0;if(!work.poll())return false;}
        }
    }
    return work.poll();
}
bool bn_relu(float*x,unsigned channels,unsigned pixels,const char *prefix,const Work &work){
    char n[80];const float *p[4];const char *suffix[]={"weight","bias","running_mean","running_var"};
    for(unsigned i=0;i<4;i++){std::snprintf(n,sizeof(n),"%s.%s",prefix,suffix[i]);p[i]=value(n);if(!p[i])return false;}
    for(unsigned c=0;c<channels;c++){
        const float inverse=1.f/std::sqrt(p[3][c]+1e-5f);
        for(unsigned i=0;i<pixels;i++){
            float z=(x[c*pixels+i]-p[2][c])*inverse*p[0][c]+p[1][c];x[c*pixels+i]=std::max(0.f,z);
            if(i%2048==0&&!work.poll())return false;
        }
    }
    return true;
}
bool norm(const float*x,float*y,unsigned rows,unsigned width,const char*prefix){
    char n[96];std::snprintf(n,sizeof(n),"%s.weight",prefix);const float*w=value(n);
    std::snprintf(n,sizeof(n),"%s.bias",prefix);const float*b=value(n);if(!w||!b)return false;
    for(unsigned r=0;r<rows;r++){
        double mean=0,var=0;for(unsigned i=0;i<width;i++)mean+=x[r*width+i];mean/=width;
        for(unsigned i=0;i<width;i++){double d=x[r*width+i]-mean;var+=d*d;}
        float inv=float(1/std::sqrt(var/width+1e-5));
        for(unsigned i=0;i<width;i++)y[r*width+i]=(x[r*width+i]-float(mean))*inv*w[i]+b[i];
    }
    return true;
}
void qparams(const float*x,unsigned count,double &scale,int &zero){
    float low=0,high=0;for(unsigned i=0;i<count;i++){low=std::min(low,x[i]);high=std::max(high,x[i]);}
    scale=(double(high)-low)/127;
    if(float(scale)==0||!std::isfinite(1.f/float(scale)))scale=.1;
    constexpr float minimum_scale=6.1e-5f;
    if(scale<minimum_scale){float old=float(scale);scale=minimum_scale;
        if(low==0)high=minimum_scale*127;else if(high==0)low=-minimum_scale*127;
        else {float ratio=minimum_scale/old;low*=ratio;high*=ratio;}}
    double a=-low/scale,b=127-high/scale;
    double z=(-std::fabs(low/scale)<127-std::fabs(high/scale))?a:b;
    zero=int(std::nearbyint(std::max(0.,std::min(127.,z))));
}
bool linear(const float*x,float*y,unsigned rows,unsigned ci,unsigned co,const char*prefix,Workspace&s,const Work &work){
    char n[96];std::snprintf(n,sizeof(n),"%s.weight",prefix);const Weight*w=find(n);
    std::snprintf(n,sizeof(n),"%s.bias",prefix);const float*b=value(n);
    if(!w||!w->quantized||!b||w->shape[0]!=int(co)||w->shape[1]!=int(ci)||rows*ci>32768)return false;
    std::snprintf(n,sizeof(n),"%s.input",prefix);trace(n,x,rows*ci);
    const int8_t *q=reinterpret_cast<const int8_t*>(weight_blob+w->offset);
    double scale;int zero;qparams(x,rows*ci,scale,zero);float inv=1.f/float(scale);
    for(unsigned i=0;i<rows*ci;i++)s.quant[i]=uint8_t(std::max(0.f,std::min(255.f,std::nearbyint(x[i]*inv+float(zero)))));
    float multiplier=float(scale)*float(w->scale);
    if(!std::strncmp(prefix,"mixer.",6)){
        const int pl=linear_pl_compute(s.quant,q,y,b,multiplier,rows,ci,co,zero,w->zero,work.service,work.control?work.control->cancelled:nullptr);
        if(pl!=0){if(pl<0)return false;trace(prefix,y,rows*co);return true;}
    }
    for(unsigned r=0;r<rows;r++){for(unsigned o=0;o<co;o++){
        int32_t sum=0;for(unsigned i=0;i<ci;i++)sum+=(int(s.quant[r*ci+i])-zero)*(int(q[o*ci+i])-w->zero);
        y[r*co+o]=float(sum)*multiplier+b[o];
        if(o%32==31&&!work.poll())return false;
    }
    }if(!work.poll())return false;
    trace(prefix,y,rows*co);
    return true;
}
bool gelu(float*x,unsigned n,const Work &work){for(unsigned i=0;i<n;i++){x[i]=.5f*x[i]*(1.f+std::erf(x[i]*.7071067811865475244f));if(i%256==0&&!work.poll())return false;}return true;}
float sigmoid(float x){if(x>=0)return 1.f/(1.f+std::exp(-x));float e=std::exp(x);return e/(1.f+e);}
#include "kernels.inc"
bool gru_poll(const Work&work,gru_profile::Direction&d){
 if(!lung_gru_trace)return work.poll();const auto at=work.clock();
 const bool okay=work.poll();d.service_us+=work.clock()-at;++d.polls;return okay;
}
bool gru_direction(const float*x,float*out,unsigned layer,bool reverse,Workspace&s,const Work &work){
    auto &profile=gru_profile::state.d[layer*2+unsigned(reverse)];const bool probe=lung_gru_trace!=0;const auto began=probe?work.clock():0;
    char n[96];const char*end=reverse?"_reverse":"";
    std::snprintf(n,sizeof(n),"gru.weight_ih_l%u%s",layer,end);const float*wi=value(n);
    std::snprintf(n,sizeof(n),"gru.weight_hh_l%u%s",layer,end);const float*wh=value(n);
    std::snprintf(n,sizeof(n),"gru.bias_ih_l%u%s",layer,end);const float*bi=value(n);
    std::snprintf(n,sizeof(n),"gru.bias_hh_l%u%s",layer,end);const float*bh=value(n);
    if(!wi||!wh||!bi||!bh)return false;
    for(unsigned t=0;t<64;t++){
        const auto at=probe?work.clock():0,service_before=profile.service_us;
        if(lung_gru_optimized){
            for(unsigned g=0;g<384;g+=4){
                float a=0,b=0,c=0,d=0;const float *w=wi+g*256;
                for(unsigned k=0;k<256;++k){const float v=x[t*256+k];a+=v*w[k];b+=v*w[256+k];c+=v*w[512+k];d+=v*w[768+k];}
                float*y=s.gates+t*384+g;y[0]=a+bi[g];y[1]=b+bi[g+1];y[2]=c+bi[g+2];y[3]=d+bi[g+3];
                if(g%64==60&&!gru_poll(work,profile))return false;
            }
        }else {for(unsigned g=0;g<384;g++){float sum=0;for(unsigned k=0;k<256;k++)sum+=x[t*256+k]*wi[g*256+k];s.gates[t*384+g]=sum+bi[g];if(g%64==63&&!gru_poll(work,profile))return false;}}
        if(!gru_poll(work,profile))return false;
        if(probe){profile.input_us+=work.clock()-at-(profile.service_us-service_before);++profile.input_steps;}
    }
    std::fill(s.state,s.state+128,0.f);
    for(unsigned step=0;step<64;step++){
        unsigned t=reverse?63-step:step;
        const auto at=probe?work.clock():0;
        if(lung_gru_optimized)gru_dot4<128>(s.state,wh,bh,s.rh);else for(unsigned g=0;g<384;g++){float sum=0;for(unsigned k=0;k<128;k++)sum+=s.state[k]*wh[g*128+k];s.rh[g]=sum+bh[g];}
        if(probe){profile.recurrent_us+=work.clock()-at;++profile.recurrent_steps;}
        const auto activated=probe?work.clock():0;
        for(unsigned i=0;i<128;i++){
            float r=sigmoid(s.gates[t*384+i]+s.rh[i]);
            float z=sigmoid(s.gates[t*384+128+i]+s.rh[128+i]);
            float candidate=std::tanh(s.gates[t*384+256+i]+r*s.rh[256+i]);
            s.state[i]=(1-z)*candidate+z*s.state[i];
            out[t*256+(reverse?128:0)+i]=s.state[i];
        }
        if(probe){profile.activation_us+=work.clock()-activated;++profile.activation_steps;}
        if(!gru_poll(work,profile))return false;
    }
    if(probe){profile.total_us=work.clock()-began;++gru_profile::state.directions;}
    return true;
}
}
const Weight* find(const char*name){for(unsigned i=0;i<weight_count;i++)if(!std::strcmp(weights[i].name,name))return weights+i;return nullptr;}
void set_trace(Trace cb,void*context){tracer=cb;trace_context=context;}
bool forward(const float*features,unsigned frames,float probabilities[5],Service service,const feature_lung_control *control){
    if(!features||!probabilities||!frames||frames>MaxFrames||active)return false;
    gru_profile::reset();
    Work work{service,control,LUNG_CNN};if(!work.poll())return false;
    auto started=work.clock();
    if(linear_pl_faulted())return false;
    const unsigned feature_count=FrequencyBins*frames;
    for(unsigned i=0;i<feature_count;i++)if(!std::isfinite(features[i]))return false;
    struct Guard {Guard(){active=true;}~Guard(){active=false;}} guard;
    std::unique_ptr<Workspace> storage(new(std::nothrow) Workspace);if(!storage)return false;
    auto &s=*storage;std::memcpy(s.a,features,feature_count*sizeof(float));
    unsigned ci=1,height=128,width=frames;float*input=s.a,*output=s.b;
    for(unsigned block=0;block<4;block++){
        unsigned co=32u<<block;char name[64];std::snprintf(name,sizeof(name),"cnn.%u",block*3);
        if(!conv(input,output,ci,co,height,width,name,work))return false;
        height=(height+1)/2;width=(width+1)/2;std::snprintf(name,sizeof(name),"cnn.%u",block*3+1);
        if(!bn_relu(output,co,height*width,name,work))return false;
        std::snprintf(name,sizeof(name),"cnn%u",block);trace(name,output,co*height*width);
        ci=co;std::swap(input,output);
    }
    // PyTorch adaptive pool may expand an axis: 8x30 -> 1x64 for 15s. Preserve its
    // floor(start)/ceil(end) bins, not linear interpolation or reshape.
    for(unsigned t=0;t<64;t++)for(unsigned c=0;c<256;c++){
        unsigned first=t*width/64,last=((t+1)*width+63)/64;float sum=0;
        for(unsigned y=0;y<height;y++)for(unsigned x=first;x<last;x++)sum+=input[(c*height+y)*width+x];
        s.x[t*256+c]=sum/float(height*(last-first));
    }
    trace("pool",s.x,16384);
    if(control&&control->timing)control->timing->cnn_us=work.elapsed(started);
    work.stage=LUNG_MIXER;started=work.clock();if(!work.poll())return false;
    for(unsigned block=0;block<4;block++){
        char name[96];std::snprintf(name,sizeof(name),"mixer.%u.norm1",block);
        if(!norm(s.x,s.norm,64,256,name))return false;
        for(unsigned t=0;t<64;t++)for(unsigned c=0;c<256;c++)s.trans[c*64+t]=s.norm[t*256+c];
        std::snprintf(name,sizeof(name),"mixer.%u.token_mix.0",block);
        if(!linear(s.trans,s.hidden,256,64,128,name,s,work))return false;
        if(!gelu(s.hidden,32768,work))return false;
        std::snprintf(name,sizeof(name),"mixer.%u.token_mix.3",block);
        if(!linear(s.hidden,s.trans,256,128,64,name,s,work))return false;
        for(unsigned t=0;t<64;t++)for(unsigned c=0;c<256;c++)s.x[t*256+c]+=s.trans[c*64+t];
        std::snprintf(name,sizeof(name),"mixer.%u.norm2",block);
        if(!norm(s.x,s.norm,64,256,name))return false;
        std::snprintf(name,sizeof(name),"mixer.%u.channel_mix.0",block);
        if(!linear(s.norm,s.hidden,64,256,512,name,s,work))return false;
        if(!gelu(s.hidden,32768,work))return false;
        std::snprintf(name,sizeof(name),"mixer.%u.channel_mix.3",block);
        if(!linear(s.hidden,s.tmp,64,512,256,name,s,work))return false;
        for(unsigned i=0;i<16384;i++)s.x[i]+=s.tmp[i];
        std::snprintf(name,sizeof(name),"mixer%u",block);trace(name,s.x,16384);if(!work.poll())return false;
    }
    if(control&&control->timing)control->timing->mixer_us=work.elapsed(started);
    work.stage=LUNG_GRU;started=work.clock();if(!work.poll())return false;
    if(!gru_direction(s.x,s.sequence,0,false,s,work)||!gru_direction(s.x,s.sequence,0,true,s,work))return false;
    trace("gru0",s.sequence,16384);
    if(!gru_direction(s.sequence,s.out,1,false,s,work)||!gru_direction(s.sequence,s.out,1,true,s,work))return false;
    trace("gru1",s.out,16384);gru_profile::publish(true);
    if(control&&control->timing)control->timing->gru_us=work.elapsed(started);
    work.stage=LUNG_HEAD;started=work.clock();if(!work.poll())return false;
    float average[256]{},h1[128],h2[64],logits[5];
    for(unsigned t=0;t<64;t++)for(unsigned c=0;c<256;c++)average[c]+=s.out[t*256+c]/64.f;
    trace("average",average,256);
    if(!linear(average,h1,1,256,128,"head.0",s,work))return false;
    for(float &v:h1)v=std::max(0.f,v);
    if(!linear(h1,h2,1,128,64,"head.3",s,work))return false;
    for(float &v:h2)v=std::max(0.f,v);
    if(!linear(h2,logits,1,64,5,"head.5",s,work))return false;
    trace("logits",logits,5);
    float max=*std::max_element(logits,logits+5),sum=0;
    for(unsigned i=0;i<5;i++){if(!std::isfinite(logits[i]))return false;probabilities[i]=std::exp(logits[i]-max);sum+=probabilities[i];}
    for(unsigned i=0;i<5;i++)probabilities[i]/=sum;
    trace("probabilities",probabilities,5);
    if(control&&control->timing)control->timing->head_us=work.elapsed(started);
    return work.poll();
}
}
