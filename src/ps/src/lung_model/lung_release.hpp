#pragma once
#include <cstddef>
#include <cstdint>
#include "../features/lung.h"
namespace lung_release {
constexpr unsigned MaxSamples=4000*15, FrequencyBins=128;
constexpr unsigned MaxFrames=1+(MaxSamples-256+127)/128;
constexpr unsigned FeatureSize=FrequencyBins*MaxFrames;
using Service=void(*)(unsigned);
using WaveCallback=void(*)(const float*,size_t,unsigned,void*);
struct Work {
    Service service;const feature_lung_control *control;unsigned stage;
    bool cancelled()const{return control&&control->cancelled&&*control->cancelled;}
    bool poll()const{if(cancelled())return false;if(service)service(stage);return !cancelled();}
    uint64_t clock()const{return control&&control->clock_us?control->clock_us():0;}
    uint32_t elapsed(uint64_t start)const{const auto n=clock()-start;return uint32_t(n>UINT32_MAX?UINT32_MAX:n);}
    const feature_lung_input_stats *stats(size_t n)const{
        const auto*s=control?control->input_stats:nullptr;
        return s&&s->samples==n&&s->minimum>=-32768&&s->maximum<=32767&&s->minimum<=s->maximum&&
            s->sum>=int64_t(s->minimum)*int64_t(n)&&s->sum<=int64_t(s->maximum)*int64_t(n)?s:nullptr;
    }
};
struct Weight {const char *name;unsigned offset;int shape[4],rank;double scale;int zero,quantized;};
extern const Weight weights[];
extern const unsigned weight_count;
extern const unsigned char weight_blob[];
const Weight *find(const char *name);
using Trace=void(*)(const char*,const float*,unsigned,void*);
// Each trace pointer is borrowed only for the duration of the callback.
void set_trace(Trace callback,void *context);
bool preprocess(const int16_t *pcm,size_t count,unsigned rate,float *features,
                unsigned *resampled,unsigned *frames,Service service=nullptr,
                WaveCallback wave=nullptr,void *context=nullptr,const feature_lung_control *control=nullptr);
bool forward(const float *features,unsigned frames,float probabilities[5],Service service=nullptr,const feature_lung_control *control=nullptr);
// P96 and 150 ms merge from release's separate ZC experiment, optional metadata.
unsigned respiratory_spans(const int16_t *pcm,size_t count,unsigned rate,
                           uint32_t *starts,uint32_t *ends,unsigned capacity);
}
