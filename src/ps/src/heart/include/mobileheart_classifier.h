#pragma once

#include <stdint.h>

namespace mobileheart {

constexpr int kNumLabels = 5;
constexpr int kSegmentLen = 4096;

enum Label { kAS = 0, kAR = 1, kMR = 2, kMS = 3, kNormal = 4 };

struct Prediction {
    bool valid;
    int top_label;
    float mobile[kNumLabels];
    float tcn[kNumLabels];
    float ensemble[kNumLabels];
    uint32_t mobile_ms;
    uint32_t tcn_ms;
    uint32_t total_ms;
};

Prediction predict(const float *segment);
const char *label_short(int label);
const char *label_name(int label);

}  // namespace mobileheart
