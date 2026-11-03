#include "mobileheart_classifier.h"

#include <cstring>
#include "esp_timer.h"

extern "C" {
#include "mobileheart_model.h"
}

namespace mobileheart {

Prediction predict(const float *segment) {
    Prediction result{};
    if (!segment) return result;

    const int64_t total_start = esp_timer_get_time();
    // Run sequentially so both networks never hold their large workspaces at once.
    int64_t stage_start = esp_timer_get_time();
    if (!ghost_mobile_forward(segment, result.mobile)) return result;
    result.mobile_ms = static_cast<uint32_t>((esp_timer_get_time() - stage_start) / 1000);
    stage_start = esp_timer_get_time();
    if (!ghost_tcn_forward(segment, result.tcn)) return result;
    result.tcn_ms = static_cast<uint32_t>((esp_timer_get_time() - stage_start) / 1000);

    result.valid = true;
    result.top_label = 0;
    for (int i = 0; i < kNumLabels; ++i) {
        result.ensemble[i] = 0.5f * (result.mobile[i] + result.tcn[i]);
        if (result.ensemble[i] > result.ensemble[result.top_label]) result.top_label = i;
    }
    result.total_ms = static_cast<uint32_t>((esp_timer_get_time() - total_start) / 1000);
    return result;
}

const char *label_short(int label) {
    static const char *names[kNumLabels] = {"AS", "AR", "MR", "MS", "Normal"};
    return (label >= 0 && label < kNumLabels) ? names[label] : "ERR";
}

const char *label_name(int label) {
    static const char *names[kNumLabels] = {
        "Aortic stenosis", "Aortic regurg", "Mitral regurg", "Mitral stenosis", "Normal"
    };
    return (label >= 0 && label < kNumLabels) ? names[label] : "Inference error";
}

}  // namespace mobileheart
