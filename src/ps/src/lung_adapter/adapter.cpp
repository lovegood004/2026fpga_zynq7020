#include "lung.h"
#include "events.hpp"
#include "lung_release.hpp"
#include <algorithm>
#include <memory>
#include <new>
namespace {bool running=false;}
extern "C" feature_lung_result feature_lung_pcm16_serviced(const int16_t *pcm,size_t count,int rate,void(*service)(unsigned)){
    return feature_lung_pcm16_with_wave(pcm,count,rate,service,nullptr,nullptr);
}
extern "C" feature_lung_result feature_lung_pcm16_with_wave(const int16_t *pcm,size_t count,int rate,void(*service)(unsigned),feature_lung_wave_callback wave,void *context){
    return feature_lung_pcm16_controlled(pcm,count,rate,service,wave,context,nullptr);
}
extern "C" feature_lung_result feature_lung_pcm16_controlled(const int16_t *pcm,size_t count,int rate,void(*service)(unsigned),feature_lung_wave_callback wave,void *context,const feature_lung_control *control){
    feature_lung_result result{};result.label=-1;result.reason="invalid input";
    if(!pcm||count<2||count>661500||rate<1000||rate>192000||count>size_t(rate)*15)return result;
    if(running){result.failed=1;result.reason="lung model busy";return result;}
    struct Guard {Guard(){running=true;}~Guard(){running=false;}} guard;
    lung_release::Work work{service,control,LUNG_STATS};
    if(control&&control->timing)*control->timing=feature_lung_timing{};
    if(!work.poll()){result.reason="analysis cancelled";return result;}
    int minimum=32767,maximum=-32768;
    if(const auto*stats=work.stats(count)){
        minimum=stats->minimum;maximum=stats->maximum;
    }else for(size_t i=0;i<count;i++){
        minimum=std::min(minimum,int(pcm[i]));maximum=std::max(maximum,int(pcm[i]));
        if(i%2048==0&&!work.poll()){result.reason="analysis cancelled";return result;}
    }
    if(maximum-minimum<4){result.reason="silence or constant: unknown";return result;}
    std::unique_ptr<float[]> features(new(std::nothrow) float[lung_release::FeatureSize]);
    if(!features){result.failed=1;result.reason="lung feature memory";return result;}
    unsigned resampled=0,frames=0;auto started=work.clock();
    if(!lung_release::preprocess(pcm,count,unsigned(rate),features.get(),&resampled,&frames,service,wave,context,control)){
        result.failed=!work.cancelled();result.reason=work.cancelled()?"analysis cancelled":"release preprocessing failed";return result;
    }
    if(control&&control->timing)control->timing->preprocessing_us=work.elapsed(started);
    lung_phase_complete(1);
    started=work.clock();
    result.resampled_samples=int(resampled);
    if(!lung_release::forward(features.get(),frames,result.mean_probability,service,control)){
        result.failed=!work.cancelled();result.reason=work.cancelled()?"analysis cancelled":"release inference failed";return result;
    }
    if(control&&control->timing)control->timing->inference_us=work.elapsed(started);
    lung_phase_complete(2);
    if(work.cancelled()){result.reason="analysis cancelled";return result;}
    result.label=int(std::max_element(result.mean_probability,result.mean_probability+5)-result.mean_probability);
    result.segments=1;result.votes[result.label]=1;result.vote_fraction=1;result.valid=1;
    result.reason="release CNN-Mixer-BiGRU; accuracy unverified";return result;
}
extern "C" const char *feature_lung_label(int label){
    static const char*names[]={"COPD","Asthma","Pneumonia","Other","Healthy"};
    return label>=0&&label<5?names[label]:"Unknown";
}
// Inference owns and frees its scratch on every return path. Embedded weights
// are read-only, so there is no retained model allocation to release here.
extern "C" void feature_lung_release(void){}
