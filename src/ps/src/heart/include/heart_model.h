#pragma once

#include <stdint.h>

namespace chd {

constexpr int kSegmentLen = 4096;
constexpr int kNumClasses = 2;

struct Prediction {
    bool valid;
    int label;
    float logits[kNumClasses];
    float probability[kNumClasses];
};

Prediction predict_segment(const float *segment);

}  // namespace chd
