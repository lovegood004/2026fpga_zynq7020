#pragma once
#include "session.hpp"
#include "preprocess.h"
#include <cstddef>
#include <cstdint>
constexpr unsigned PLOT_W=208, PLOT_H=108;
constexpr unsigned WAVE_W=270;
struct WaveWindow {
    int16_t low[WAVE_W]{},high[WAVE_W]{};
    uint16_t envelope[WAVE_W]{};
    float first=0,seconds=0;
    uint32_t first_sample=0,samples=0,sample_rate=0;
};
struct LungWave {
    WaveWindow windows[11]{}; // 0..5, 1..6, ..., 10..15 seconds.
    unsigned count=0;
};
// Callback for the exact normalized 4kHz signal used by lung preprocessing.
void build_lung_wave(const float*,size_t,unsigned,void*);
struct WaveBuildContext {LungWave *wave;void(*service)(unsigned);const int *cancelled;};
void build_lung_wave_serviced(const float*,size_t,unsigned,void*);
struct Preview {
    bool valid=false;
    int16_t low[PLOT_W]={},high[PLOT_W]={};
    uint16_t envelope[PLOT_W]={};
    uint8_t stft[PLOT_H][PLOT_W]={};
    float seconds=0,low_hz=0,high_hz=0;
    WaveWindow wave[11]{};unsigned wave_count=0; // Processed 5s windows, 1s steps.
    uint32_t wave_us=0,stft_us=0;
};
struct Result {
    Position position;
    bool ready=false,failed=false,experimental=false,test_recording=false;
    uint32_t crc=0,inference_ms=0,total_ms=0,pl_calls=0;
    uint32_t preprocessing_us=0,inference_us=0,mobile_ms=0,tcn_ms=0;
    uint32_t operator_us[9]{};
    const char *failure="";
    const char *source="未分析";
};
using Progress=void (*)(unsigned);
bool build_preview(const int16_t*,size_t,uint32_t,uint32_t,Preview&,Progress=nullptr,bool heart_wave=true,const int *cancelled=nullptr);
bool run_analysis(const int16_t*,size_t,uint32_t,uint32_t,Result&,Progress=nullptr);
namespace chd {
PreprocessStats preprocess_pcm_rated(const int16_t*,size_t,uint32_t,uint32_t,
                                   SegmentCallback,void*,bool=false);
}
extern "C" void shared_arm_enable(int);
extern "C" unsigned integrated_pl_calls(void);
