#include "profile.hpp"
/* Compatibility target: release/infer_8600.py, not a newly designed Mel front end.
 * Fourier resample; periodic Tukey(.25); per-frame detrend; one-sided PSD;
 * log(PSD+1e-8); first 128 frequency rows; global z-score.
 * Full recording, no time crop: right-pad only the last incomplete hop. The
 * existing adaptive pool accepts variable time width without weight changes.
 */
#include "../../lung_model/lung_release.hpp"
extern "C" {unsigned lung_fft_optimized=1;}
#include <algorithm>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
namespace lung_release { namespace {
using Z=std::complex<double>;
constexpr double pi=3.1415926535897932384626433832795;
template<class T> using Buffer=std::unique_ptr<T[]>;
template<class T> Buffer<T> allocate(size_t n){return Buffer<T>(new(std::nothrow) T[n]{});}
const Work *fft_work=nullptr;
bool checkpoint(){return !fft_work||fft_work->poll();}
bool radix2(Z*,size_t,bool);
bool radix2_reference(Z *a,size_t n,bool inverse){
    unsigned butterflies=0;
    for(size_t i=1,j=0;i<n;i++){size_t b=n>>1;for(;j&b;b>>=1)j^=b;j^=b;if(i<j)std::swap(a[i],a[j]);if(i%2048==0&&!checkpoint())return false;}
    for(size_t len=2;len<=n;len<<=1){Z root=std::polar(1.,(inverse?2:-2)*pi/len);
        for(size_t i=0;i<n;i+=len){Z w=1.;for(size_t j=0;j<len/2;j++){
            Z u=a[i+j],v=w*a[i+j+len/2];a[i+j]=u+v;a[i+j+len/2]=u-v;w*=root;
            if(++butterflies==1024){butterflies=0;if(!checkpoint())return false;}}}
        if(!checkpoint())return false;}
    if(inverse)for(size_t i=0;i<n;i++){a[i]/=double(n);if(i%2048==0&&!checkpoint())return false;}
    return checkpoint();
}
bool bluestein(Z *out,const Z *in,size_t stride,size_t n,bool inverse){
    size_t m=1;while(m<2*n-1)m<<=1;
    auto a=allocate<Z>(m),b=allocate<Z>(m);if(!a||!b)return false;
    double sign=inverse?1.:-1.;
    for(size_t k=0;k<n;k++){
        double angle=sign*pi*double((uint64_t(k)*k)%(2*n))/n;Z w=std::polar(1.,angle);
        a.get()[k]=in[k*stride]*w;b.get()[k]=std::conj(w);if(k)b.get()[m-k]=std::conj(w);
    }
    if(!radix2(a.get(),m,false)||!radix2(b.get(),m,false))return false;
    for(size_t i=0;i<m;i++){a.get()[i]*=b.get()[i];if(i%2048==0&&!checkpoint())return false;}
    if(!radix2(a.get(),m,true))return false;
    for(size_t k=0;k<n;k++){out[k]=a.get()[k]*std::polar(1.,sign*pi*double((uint64_t(k)*k)%(2*n))/n);if(k%512==0&&!checkpoint())return false;}
    return true;
}
bool mixed(Z *out,const Z *in,size_t stride,size_t n,const Z *roots,size_t root_n,bool inverse){
    if(n==1){out[0]=in[0];return true;}
    unsigned p=2;while(p<=31&&n%p)++p;
    if(p>31)return bluestein(out,in,stride,n,inverse);
    size_t m=n/p;
    for(unsigned j=0;j<p;j++)if(!mixed(out+j*m,in+j*stride,stride*p,m,roots,root_n,inverse))return false;
    Z values[31],result[31];const size_t root_step=root_n/n,radix_step=root_n/p;
    for(size_t k=0;k<m;k++){
        for(unsigned j=0;j<p;j++)values[j]=out[j*m+k]*roots[j*k*root_step];
        for(unsigned q=0;q<p;q++){result[q]=0.;for(unsigned j=0;j<p;j++)result[q]+=values[j]*roots[((j*q)%p)*radix_step];}
        for(unsigned q=0;q<p;q++)out[q*m+k]=result[q];
        if(n>=256&&k%16==0&&!checkpoint())return false;
    }
    return true;
}
#include "../lung_fft/mixed_plan.inc"
bool fft_reference(Z *a,size_t n,bool inverse){
    if(!n)return false;
    if((n&(n-1))==0)return radix2(a,n,inverse);
    front_profile::Fft unused;auto&r=front_profile::record?*front_profile::record:unused;const auto at=front_profile::now();
    auto result=allocate<Z>(n),roots=allocate<Z>(n);if(!result||!roots)return false;
    r.alloc_us=front_profile::now()-at;const auto roots_at=front_profile::now();++front_profile::state.root_builds;
    for(size_t i=0;i<n;i++){
        roots.get()[i]=std::polar(1.,(inverse?2:-2)*pi*i/n);
        if(i%256==0&&!checkpoint())return false;
    }
    r.roots_us=front_profile::now()-roots_at;const auto transform_at=front_profile::now();
    if(lung_fft_optimized){
        MixedPlan plan;if(!plan.prepare(n,roots.get()))return false;
        if(!mixed_planned(result.get(),a,1,n,roots.get(),inverse,plan,0))return false;
    }else if(!mixed(result.get(),a,1,n,roots.get(),n,inverse))return false;
    r.transform_us=front_profile::now()-transform_at;const auto copy_at=front_profile::now();
    for(size_t i=0;i<n;i++){a[i]=result.get()[i]/double(inverse?n:1);if(i%2048==0&&!checkpoint())return false;}
    r.copy_us=front_profile::now()-copy_at;return true;
}
#include "cache.inc"
}
bool preprocess(const int16_t *pcm,size_t count,unsigned rate,float *features,unsigned *resampled,unsigned *frames,Service service,WaveCallback wave,void *context,const feature_lung_control *control){
    if(!pcm||!features||!resampled||!frames||rate<1000||rate>192000||count<2||count>661500||count>size_t(rate)*15)return false;
    Work work{service,control,LUNG_RESAMPLE};
    FrontSession session(work,count,rate);if(!session.entered)return false;
    const auto resample_at=front_profile::now();
    struct RestoreService {const Work *previous;RestoreService(const Work*w):previous(fft_work){fft_work=w;}~RestoreService(){fft_work=previous;}} restore(&work);
    if(!work.poll())return false;
    const size_t target=uint64_t(count)*4000/rate;
    if(!target||target>MaxSamples)return false;
    *resampled=unsigned(target);
    *frames=target<=256?1:1+unsigned((target-256+127)/128);
    const unsigned feature_count=FrequencyBins * *frames;
    const size_t padded=256+128*(*frames-1);
    if(!prepare_cache())return false;
    auto audio=allocate<float>(padded);if(!audio)return false;
    double total=0.;
    if(const auto*stats=work.stats(count))total=double(stats->sum);
    else for(size_t i=0;i<count;i++){total+=pcm[i];if(i%2048==0&&!work.poll())return false;}
    float mean=float(total/count),peak=0;
    if(rate==4000){
        for(size_t i=0;i<count;i++){peak=std::max(peak,std::fabs(float(pcm[i])-mean));if(i%2048==0&&!work.poll())return false;}
        for(size_t i=0;i<count;i++){audio.get()[i]=float(pcm[i])-mean;if(i%2048==0&&!work.poll())return false;}
    }else{
        auto spectrum=allocate<Z>(count),output=allocate<Z>(target);if(!spectrum||!output)return false;
        for(size_t i=0;i<count;i++){spectrum.get()[i]=Z(float(pcm[i])-mean,0.);if(i%2048==0&&!work.poll())return false;}
        if(!work.poll())return false;
        if(!fft(spectrum.get(),count,false))return false;
        if(!work.poll())return false;
        const size_t n=std::min(count,target);
        for(size_t k=0;k<=n/2;k++){output.get()[k]=spectrum.get()[k];if(k%2048==0&&!work.poll())return false;}
        if(n%2==0){if(target<count)output.get()[n/2]*=2.;else if(count<target)output.get()[n/2]*=.5;}
        for(size_t k=1;k<(target+1)/2;k++){
            // Resampled real signal is represented by a Hermitian spectrum.
            output.get()[target-k]=std::conj(output.get()[k]);if(k%2048==0&&!work.poll())return false;
        }
        if(!fft(output.get(),target,true))return false;
        float factor=float(double(target)/count);
        for(size_t i=0;i<target;i++){
            float value=float(output.get()[i].real())*factor;peak=std::max(peak,std::fabs(value));
            audio.get()[i]=value;if(i%2048==0&&!work.poll())return false;
        }
    }
    if(!std::isfinite(peak)||peak<1e-12f)return false;
    for(size_t i=0;i<target;i++){audio.get()[i]/=peak;if(i%2048==0&&!work.poll())return false;}
    front_profile::state.resample_us=front_profile::now()-resample_at;const auto stft_at=front_profile::now();
    work.stage=LUNG_SPECTROGRAM;if(!work.poll())return false;
    float window[256];double energy=0;
    for(unsigned i=0;i<256;i++){
        window[i]=float(i<32?.5*(1-std::cos(pi*i/32)):i>224?.5*(1+std::cos(pi*(i-224)/32)):1.);
        energy+=double(window[i])*window[i];
    }
    Z frame[512];
    for(unsigned t=0;t<*frames;t++){
        double sum=0;for(unsigned i=0;i<256;i++)sum+=audio.get()[t*128+i];float dc=float(sum/256);
        for(unsigned i=0;i<512;i++)frame[i]=i<256?Z(float((audio.get()[t*128+i]-dc)*window[i]),0.):Z{};
        if(!radix2(frame,512,false))return false;
        for(unsigned f=0;f<128;f++){
            float psd=float(std::norm(frame[f])/(4000*energy));if(f)psd*=2;
            features[f * *frames+t]=std::log(psd+1e-8f);
        }
        ++front_profile::state.stft_frames;
        if(!work.poll())return false;
    }
    front_profile::state.stft_us=front_profile::now()-stft_at;const auto normalize_at=front_profile::now();
    double sum=0;for(unsigned i=0;i<feature_count;i++) {sum+=features[i];if(i%2048==0&&!work.poll())return false;}float center=float(sum/feature_count);
    double var=0;for(unsigned i=0;i<feature_count;i++){float d=features[i]-center;var+=double(d)*d;if(i%2048==0&&!work.poll())return false;}
    float sd=float(std::sqrt(var/feature_count))+1e-8f;
    for(unsigned i=0;i<feature_count;i++){features[i]=(features[i]-center)/sd;if(!std::isfinite(features[i]))return false;if(i%2048==0&&!work.poll())return false;}
    // Borrowed normalized, 4kHz PCM, excluding artificial tail padding. This is
    // exactly the time-domain signal used for the classifier's spectrogram.
    front_profile::state.normalize_us=front_profile::now()-normalize_at;const auto wave_at=front_profile::now();
    work.stage=LUNG_WAVE;if(!work.poll())return false;
    if(wave)wave(audio.get(),target,4000,context);
    const bool okay=work.poll();front_profile::state.wave_us=front_profile::now()-wave_at;session.valid=okay;return okay;
}
// Optional respiratory metadata only. Not used to crop/replace infer_8600's input.
unsigned respiratory_spans(const int16_t *pcm,size_t count,unsigned rate,uint32_t *starts,uint32_t *ends,unsigned capacity){
    if(!pcm||!starts||!ends||!capacity||!rate||count<3||count>661500)return 0;
    auto zc=allocate<uint32_t>(count);auto gaps=allocate<uint32_t>(count);if(!zc||!gaps)return 0;
    double mean=0;for(size_t i=0;i<count;i++)mean+=pcm[i];mean/=count;
    unsigned n=0;for(size_t i=0;i+1<count;i++)if((pcm[i]<mean&&pcm[i+1]>mean)||(pcm[i]>mean&&pcm[i+1]<mean))zc.get()[n++]=uint32_t(i);
    if(n<2)return 0;
    for(unsigned i=0;i+1<n;i++)gaps.get()[i]=zc.get()[i+1]-zc.get()[i];
    std::sort(gaps.get(),gaps.get()+n-1);double pos=.96*(n-2);unsigned at=unsigned(pos);
    double threshold=gaps.get()[at]+(pos-at)*(gaps.get()[std::min(at+1,n-2)]-gaps.get()[at]);
    unsigned begin=0,used=0;uint32_t current_s=0,current_e=0;bool current=false;
    auto emit=[&](unsigned a,unsigned b){if(b-a<2)return;
        uint32_t s=zc.get()[a],e=zc.get()[b-1];
        if(current&&double(s-current_e)/rate<.15){current_e=e;return;}
        if(current&&used<capacity){starts[used]=current_s;ends[used++]=current_e;}
        current_s=s;current_e=e;current=true;
    };
    for(unsigned i=0;i+1<n;i++)if(double(zc.get()[i+1]-zc.get()[i])>threshold){emit(begin,i+1);begin=i+1;}
    emit(begin,n);if(current&&used<capacity){starts[used]=current_s;ends[used++]=current_e;}return used;
}
}
