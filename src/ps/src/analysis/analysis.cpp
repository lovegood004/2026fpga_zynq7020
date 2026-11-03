#include "analysis.hpp"
#include "esp_timer.h"
#include "profile.h"
#include <cmath>
#include <algorithm>
#include <vector>
#ifdef MH_MODEL_REPLAY
extern void model_replay_observe(const mobileheart::Prediction&);
#endif
namespace {
struct Context {Result *r;Progress progress;bool failed=false;uint64_t callbacks_us=0;};
bool infer(const float*x,int index,void*opaque) {
    auto &c=*static_cast<Context*>(opaque);
    const auto callback_start=esp_timer_get_time();
    if(c.progress)c.progress(static_cast<unsigned>(index));
    const auto model_start=esp_timer_get_time();
    auto p=mobileheart::predict(x);
    c.r->inference_us+=uint32_t(esp_timer_get_time()-model_start);
#ifdef MH_MODEL_REPLAY
    model_replay_observe(p);
#endif
    c.r->mobile_ms+=p.mobile_ms;c.r->tcn_ms+=p.tcn_ms;
    c.r->inference_ms+=p.total_ms;
    const bool ok=c.r->position.add(p);if(!ok)c.failed=true;
    c.callbacks_us+=uint64_t(esp_timer_get_time()-callback_start);
    return ok;
}
}
bool run_analysis(const int16_t*pcm,size_t n,uint32_t num,uint32_t den,Result&r,Progress cb) {
    bool test=r.test_recording;const char *source=r.source;r=Result{};r.test_recording=test;r.source=source;
    r.experimental=!(num==11025 && den==1);
    uint64_t start=esp_timer_get_time();unsigned calls=integrated_pl_calls();
    uint32_t before_ops[9];profile_snapshot_us(before_ops);
    Context c{&r,cb};
    auto s=chd::preprocess_pcm_rated(pcm,n,num,den,infer,&c);
    const auto elapsed=uint64_t(esp_timer_get_time()-start);
    r.preprocessing_us=uint32_t(elapsed>=c.callbacks_us?elapsed-c.callbacks_us:0);
    profile_snapshot_us(r.operator_us);
    for(unsigned i=0;i<9;i++)r.operator_us[i]-=before_ops[i];
    r.position.bpm=s.heart_rate_bpm;
    r.ready=s.valid&&!c.failed&&r.position.total==s.segment_count;
    r.failed=!r.ready;r.failure=c.failed?"MODEL_ERROR":chd::preprocess_failure_name(s.failure);
    r.pl_calls=integrated_pl_calls()-calls;
    r.total_ms=(esp_timer_get_time()-start)/1000;
    return r.ready;
}
namespace {
bool build_windows(const float*pcm,size_t n,unsigned rate,WaveWindow*windows,unsigned &count,Progress service,const int*cancelled){
    count=0;if(!pcm||!windows||!rate||!n||n>size_t(rate)*15)return false;
    for(size_t i=0;i<n;i++){
        if(!std::isfinite(pcm[i])||std::fabs(pcm[i])>1.00001f)return false;
        if(i%2048==0){if(service)service(4);if(cancelled&&*cancelled)return false;}
    }
    const size_t span=std::min(n,size_t(rate)*5),last=n-span;
    const unsigned number=unsigned((last+rate-1)/rate)+1;if(number>11)return false;
    for(unsigned k=0;k<number;k++){
        auto &w=windows[k];const size_t first=std::min(size_t(k)*rate,last);
        w.first=float(first)/rate;w.seconds=float(span)/rate;
        w.first_sample=uint32_t(first);w.samples=uint32_t(span);w.sample_rate=rate;
        for(unsigned x=0;x<WAVE_W;x++){
            const size_t a=first+uint64_t(x)*span/WAVE_W;
            const size_t b=std::min(first+span,std::max(a+1,first+size_t(uint64_t(x+1)*span/WAVE_W)));
            int lo=32767,hi=-32768;uint64_t envelope=0;
            for(size_t i=a;i<b;i++){
                const int v=int(std::lround(std::max(-1.f,std::min(1.f,pcm[i]))*32767.f));
                lo=std::min(lo,v);hi=std::max(hi,v);envelope+=unsigned(std::abs(v));
            }
            w.low[x]=int16_t(lo);w.high[x]=int16_t(hi);w.envelope[x]=uint16_t(envelope/(b-a));
            if(x%8==0){if(service)service(4);if(cancelled&&*cancelled)return false;}
        }
    }
    count=number;return true;
}
}
bool build_preview(const int16_t*pcm,size_t n,uint32_t num,uint32_t den,Preview&out,Progress service,bool heart_wave,const int*cancelled) {
    out=Preview{};
    if(!pcm||!num||!den||n<PLOT_W||num>192000||den>1024||n>1000000)return false;
    out.seconds=static_cast<float>(n)*den/num;
    const auto wave_start=esp_timer_get_time();
    if(heart_wave){
        const size_t count=(uint64_t(n)*1000*den+num/2)/num;
        if(count<2||count>15000)return false;
        std::vector<float> audio(count);const float scale=float(n-1)/float(count-1);float mean=0,peak=0;
        // Same continuous 1kHz resampling/DC removal as the heart training path.
        // Display-only peak scaling does not change either heart model input.
        for(size_t i=0;i<count;i++){
            const float pos=i*scale;const size_t a=size_t(pos),b=std::min(a+1,n-1);const float f=pos-a;
            audio[i]=((1.f-f)*pcm[a]+f*pcm[b])/32768.f;mean+=audio[i];
            if(i%1024==0){if(service)service(0);if(cancelled&&*cancelled)return false;}
        }
        mean/=count;for(float &v:audio){v-=mean;peak=std::max(peak,std::fabs(v));}
        if(peak>0)for(float&v:audio)v/=peak;
        if(!build_windows(audio.data(),count,1000,out.wave,out.wave_count,service,cancelled))return false;
    }
    for(unsigned x=0;x<PLOT_W;x++) {
        size_t a=static_cast<uint64_t>(x)*n/PLOT_W,b=static_cast<uint64_t>(x+1)*n/PLOT_W;
        int lo=32767,hi=-32768;uint64_t env=0;
        for(size_t i=a;i<b;i++){int v=pcm[i];lo=std::min(lo,v);hi=std::max(hi,v);env+=v<0?-v:v;}
        out.low[x]=lo;out.high[x]=hi;out.envelope[x]=env/(b-a);
        if(x%8==0){if(service)service(0);if(cancelled&&*cancelled)return false;}
    }
    out.wave_us=uint32_t(esp_timer_get_time()-wave_start);
    const auto stft_start=esp_timer_get_time();
    unsigned count=static_cast<uint64_t>(n)*1000*den/num;
    if(count<256||count>60000)return false;
    float fs=static_cast<float>(num)/den;
    // Same local averaging structure as extracted signal_visualization.cpp;
    // current low-rate input uses radius0 and is not downsampled a second time.
    std::vector<float> ds(count);int radius=fs>2000?5:0;
    for(unsigned i=0;i<count;i++) {
        double pos=static_cast<double>(i)*num/(1000.0*den);
        int center=static_cast<int>(pos),a=std::max(0,center-radius),b=std::min(static_cast<int>(n)-1,center+radius);
        if(!radius){unsigned j=std::min(static_cast<unsigned>(center),static_cast<unsigned>(n)-1);
            double f=pos-j;ds[i]=static_cast<float>(pcm[j]*(1.0-f)+pcm[std::min(j+1,static_cast<unsigned>(n)-1)]*f);}
        else {float sum=0;for(int j=a;j<=b;j++)sum+=pcm[j];ds[i]=sum/(b-a+1);}
    }
    std::vector<float> db(PLOT_H*PLOT_W);float peak=-1e30f;
    float re[256],im[256],window[256];
    constexpr float pi=3.14159265358979323846f;
    for(int i=0;i<256;i++)window[i]=.5f-.5f*std::cos(2*pi*i/255);
    for(unsigned x=0;x<PLOT_W;x++) {
        unsigned first=static_cast<uint64_t>(x)*(count-256)/(PLOT_W-1);float mean=0;
        for(int i=0;i<256;i++)mean+=ds[first+i];
        mean/=256;
        for(int i=0;i<256;i++){re[i]=(ds[first+i]-mean)*window[i];im[i]=0;}
        for(int i=1,j=0;i<256;i++){int bit=128;for(;j&bit;bit>>=1)j^=bit;j^=bit;
            if(i<j){std::swap(re[i],re[j]);std::swap(im[i],im[j]);}}
        for(int len=2;len<=256;len<<=1){float angle=-2*pi/len,wr0=std::cos(angle),wi0=std::sin(angle);
            for(int i=0;i<256;i+=len){float wr=1,wi=0;
                for(int j=0;j<len/2;j++){int a=i+j,b=a+len/2;
                    float vr=re[b]*wr-im[b]*wi,vi=re[b]*wi+im[b]*wr,ur=re[a],ui=im[a];
                    re[a]=ur+vr;im[a]=ui+vi;re[b]=ur-vr;im[b]=ui-vi;
                    float nw=wr*wr0-wi*wi0;wi=wr*wi0+wi*wr0;wr=nw;}}}
        if(x%4==0){if(service)service(0);if(cancelled&&*cancelled)return false;}
        for(unsigned y=0;y<PLOT_H;y++){unsigned bin=115-y*109/(PLOT_H-1);
            float v=10*std::log10(re[bin]*re[bin]+im[bin]*im[bin]+1);
            db[y*PLOT_W+x]=v;peak=std::max(peak,v);}
    }
    for(unsigned y=0;y<PLOT_H;y++)for(unsigned x=0;x<PLOT_W;x++)
        out.stft[y][x]=static_cast<uint8_t>(std::max(0.0f,std::min(255.0f,(db[y*PLOT_W+x]-(peak-60))*255/60)));
    out.stft_us=uint32_t(esp_timer_get_time()-stft_start);
    out.low_hz=6*1000.0f/256;out.high_hz=115*1000.0f/256;out.valid=true;return true;
}
void build_lung_wave(const float*pcm,size_t n,unsigned rate,void*context){
    if(!context)return;auto &out=*static_cast<LungWave*>(context);
    if(rate!=4000){out.count=0;return;}
    build_windows(pcm,n,rate,out.windows,out.count,nullptr,nullptr);
}
void build_lung_wave_serviced(const float*pcm,size_t n,unsigned rate,void*context){
    if(!context)return;auto &c=*static_cast<WaveBuildContext*>(context);
    if(!c.wave)return;
    if(rate!=4000){c.wave->count=0;return;}
    build_windows(pcm,n,rate,c.wave->windows,c.wave->count,c.service,c.cancelled);
}
