#pragma once

#include <stddef.h>
#include <stdint.h>

namespace chd {

constexpr int kPcmSampleRate = 11025;
constexpr int kAnalysisSampleRate = 1000;
constexpr int kCaptureSeconds = 15;
constexpr int kMaxAutoSegments = 16;

enum class PreprocessFailure : uint8_t {
    kNone = 0,
    kTooShort,
    kNoMemory,
    kWhtFailed,
    kHeartRateNotFound,
};

struct PreprocessStats {
    bool valid;
    float heart_rate_bpm;
    float snr;
    int input_samples;
    int resampled_samples;
    int segment_count;
    PreprocessFailure failure;
    uint8_t heart_rate_method;  // 1=autocorrelation, 2=peak fallback
    uint32_t elapsed_ms;
};

using SegmentCallback = bool (*)(const float *segment, int index, void *user);

PreprocessStats preprocess_pcm_to_segments(const int16_t *pcm,
                                           size_t sample_count,
                                           SegmentCallback callback,
                                           void *user,
                                           bool skip_gating = false);

const char *preprocess_failure_name(PreprocessFailure failure);

}  // namespace chd
