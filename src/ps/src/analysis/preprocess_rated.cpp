#include "preprocess.h"

#include <math.h>
#include <string.h>

#include <esp_heap_caps.h>
#include <esp_timer.h>

#include "heart_model.h"

namespace chd {
namespace {

constexpr int kWhtSize = 8192;
constexpr int kDecimate = 4;
constexpr float kHrMin = 30.0f;
constexpr float kHrMax = 160.0f;
constexpr int kCyclesPerSegment = 5;

float *alloc_float(size_t count) {
    void *p = heap_caps_malloc(count * sizeof(float), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!p) p = heap_caps_malloc(count * sizeof(float), MALLOC_CAP_8BIT);
    return static_cast<float *>(p);
}

void free_mem(void *p) { if (p) heap_caps_free(p); }

void resample_training_path(const int16_t *pcm, size_t count, float *out, int out_count) {
    const float scale = static_cast<float>(count - 1) / static_cast<float>(out_count - 1);
    float mean = 0.0f;
    for (int i = 0; i < out_count; ++i) {
        const float pos = i * scale;
        const int i0 = static_cast<int>(pos);
        const int i1 = i0 + 1 < static_cast<int>(count) ? i0 + 1 : i0;
        const float f = pos - i0;
        out[i] = ((1.0f - f) * pcm[i0] + f * pcm[i1]) / 32768.0f;
        mean += out[i];
    }
    mean /= out_count;
    for (int i = 0; i < out_count; ++i) out[i] -= mean;
}

void fwht(float *x, int n) {
    for (int step = 1; step < n; step <<= 1) {
        for (int base = 0; base < n; base += step << 1) {
            for (int j = 0; j < step; ++j) {
                const float u = x[base + j];
                const float v = x[base + j + step];
                x[base + j] = u + v;
                x[base + j + step] = u - v;
            }
        }
    }
}

// The desktop reference obtains this permutation through an O(N^2)
// argsort. bit-reversed Gray code is already a permutation, so construct
// its inverse directly in O(N), which is practical on the ESP32-S3.
void build_sequency_lut(uint16_t *seq_lut, uint16_t *inverse_lut) {
    constexpr int bits = 13;
    for (int i = 0; i < kWhtSize; ++i) {
        const int gray = i ^ (i >> 1);
        int reversed = 0;
        for (int b = 0; b < bits; ++b) reversed = (reversed << 1) | ((gray >> b) & 1);
        seq_lut[reversed] = static_cast<uint16_t>(i);
    }
    for (int i = 0; i < kWhtSize; ++i) inverse_lut[seq_lut[i]] = static_cast<uint16_t>(i);
}

bool wht_envelope(const float *signal, int count, float *envelope) {
    float *x = alloc_float(kWhtSize);
    float *tmp = alloc_float(kWhtSize);
    auto *seq = static_cast<uint16_t *>(heap_caps_malloc(kWhtSize * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    auto *inv = static_cast<uint16_t *>(heap_caps_malloc(kWhtSize * sizeof(uint16_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (!x || !tmp || !seq || !inv) { free_mem(x); free_mem(tmp); free_mem(seq); free_mem(inv); return false; }
    memset(x, 0, kWhtSize * sizeof(float));
    memcpy(x, signal, count * sizeof(float));
    build_sequency_lut(seq, inv);
    fwht(x, kWhtSize);
    for (int i = 0; i < kWhtSize; ++i) tmp[i] = x[seq[i]];
    memset(tmp + kWhtSize / 2, 0, (kWhtSize / 2) * sizeof(float));
    for (int i = 0; i < kWhtSize; ++i) x[i] = tmp[inv[i]];
    fwht(x, kWhtSize);
    constexpr float inv_n = 1.0f / kWhtSize;
    for (int i = 0; i < count; ++i) envelope[i] = fabsf(x[i]) * inv_n;
    free_mem(x); free_mem(tmp); free_mem(seq); free_mem(inv);
    return true;
}

float kmeans_gate(float *env, int n) {
    float mn = env[0], mx = env[0];
    for (int i = 1; i < n; ++i) { if (env[i] < mn) mn = env[i]; if (env[i] > mx) mx = env[i]; }
    if (mx - mn < 1e-10f) return 0.0f;
    const float inv_range = 1.0f / (mx - mn);
    float c0 = 0.0f, c1 = 1.0f;
    auto *labels = static_cast<uint8_t *>(heap_caps_malloc(n, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (!labels) return 0.0f;
    memset(labels, 0, n);
    for (int iter = 0; iter < 30; ++iter) {
        float s0 = 0.0f, s1 = 0.0f; int n0 = 0, n1 = 0, changed = 0;
        for (int i = 0; i < n; ++i) {
            const float v = (env[i] - mn) * inv_range;
            const uint8_t label = fabsf(v - c0) < fabsf(v - c1) ? 0 : 1;
            changed += label != labels[i]; labels[i] = label;
            if (label == 0) { s0 += v; ++n0; } else { s1 += v; ++n1; }
        }
        if (n0) c0 = s0 / n0; if (n1) c1 = s1 / n1;
        if (!changed) break;
    }
    const uint8_t keep = c1 > c0 ? 1 : 0;
    for (int i = 0; i < n; ++i) if (labels[i] != keep) env[i] = 0.0f;
    free_mem(labels);
    const float high = keep ? c1 : c0, low = keep ? c0 : c1;
    return low > 1e-6f ? high / low : 999.0f;
}

float autocorrelation_hr(const float *gated, int n) {
    const int fs = kAnalysisSampleRate / kDecimate;
    const int count = n / kDecimate;
    float *ds = alloc_float(count);
    if (!ds) return 0.0f;
    float mean = 0.0f;
    for (int i = 0; i < count; ++i) { ds[i] = gated[i * kDecimate]; mean += ds[i]; }
    mean /= count;
    float power = 0.0f;
    for (int i = 0; i < count; ++i) { ds[i] -= mean; power += ds[i] * ds[i]; }
    if (power < 1e-12f) { free_mem(ds); return 0.0f; }
    int lo = static_cast<int>(0.30f * fs), hi = static_cast<int>(1.71f * fs);
    if (hi >= count) hi = count - 1;
    int best_lag = lo; float best = -1.0f;
    for (int lag = lo; lag <= hi; ++lag) {
        float sum = 0.0f;
        for (int i = 0; i < count - lag; ++i) sum += ds[i] * ds[i + lag];
        sum /= power;
        if (sum > best) { best = sum; best_lag = lag; }
    }
    free_mem(ds);
    if (best <= 0.0f) return 0.0f;
    float hr = 60.0f * fs / best_lag;
    if (hr > 200.0f) hr *= 0.5f;
    return hr;
}

float peak_fallback_hr(const float *env, int n) {
    int peaks[64], count = 0;
    float max_value = 0.0f;
    for (int i = 0; i < n; ++i) if (env[i] > max_value) max_value = env[i];
    const float threshold = 0.15f * max_value;
    int last = -10000;
    for (int i = 1; i < n - 1 && count < 64; ++i) {
        if (env[i] >= threshold && env[i] > env[i - 1] && env[i] >= env[i + 1] && i - last >= 300) {
            int best = i;
            const int end = i + 250 < n ? i + 250 : n;
            for (int j = i + 1; j < end; ++j) if (env[j] > env[best]) best = j;
            peaks[count++] = best; last = best; i = best + 249;
        }
    }
    if (count < 2) return 0.0f;
    const float spacing = static_cast<float>(peaks[count - 1] - peaks[0]) / (count - 1);
    return spacing > 0.0f ? 60.0f * kAnalysisSampleRate / spacing : 0.0f;
}

}  // namespace

PreprocessStats preprocess_pcm_rated(const int16_t *pcm, size_t sample_count,
                                           uint32_t rate_num, uint32_t rate_den,
                                           SegmentCallback callback, void *user,
                                           bool skip_gating) {
    PreprocessStats stats{};
    const int64_t started = esp_timer_get_time();
    stats.input_samples = static_cast<int>(sample_count);
    if (!pcm || !rate_num || !rate_den || sample_count > 1000000u ||
        rate_num > 192000u || rate_den > 1024u ||
        static_cast<uint64_t>(sample_count)*rate_den < static_cast<uint64_t>(rate_num)*4u || !callback) {
        stats.failure = PreprocessFailure::kTooShort;
        stats.elapsed_ms = static_cast<uint32_t>((esp_timer_get_time() - started) / 1000);
        return stats;
    }

    const int n = static_cast<int>((static_cast<uint64_t>(sample_count) * kAnalysisSampleRate * rate_den + rate_num / 2) / rate_num);
    stats.resampled_samples = n;
    if (n < 2 || n > 60000) {stats.failure=PreprocessFailure::kTooShort;return stats;}
    float *signal = alloc_float(n);
    float *env = alloc_float(n < kWhtSize ? n : kWhtSize);
    float *segment = alloc_float(kSegmentLen);
    if (!signal || !env || !segment) {
        stats.failure = PreprocessFailure::kNoMemory;
        free_mem(signal); free_mem(env); free_mem(segment);
        stats.elapsed_ms = static_cast<uint32_t>((esp_timer_get_time() - started) / 1000);
        return stats;
    }
    resample_training_path(pcm, sample_count, signal, n);

    const int env_count = n < kWhtSize ? n : kWhtSize;
    if (!wht_envelope(signal, env_count, env)) {
        stats.failure = PreprocessFailure::kWhtFailed;
        free_mem(signal); free_mem(env); free_mem(segment);
        stats.elapsed_ms = static_cast<uint32_t>((esp_timer_get_time() - started) / 1000);
        return stats;
    }
    // Match the reference pipeline exactly: autocorrelation uses the
    // K-means-gated envelope, while the peak fallback sees the untouched WHT
    // envelope so weak periodic peaks are not lost on strong-murmur samples.
    float *gated = alloc_float(env_count);
    if (!gated) {
        stats.failure = PreprocessFailure::kNoMemory;
        free_mem(signal); free_mem(env); free_mem(segment);
        stats.elapsed_ms = static_cast<uint32_t>((esp_timer_get_time() - started) / 1000);
        return stats;
    }
    memcpy(gated, env, env_count * sizeof(float));
    stats.snr = kmeans_gate(gated, env_count);
    float hr = autocorrelation_hr(gated, env_count);
    free_mem(gated);
    stats.heart_rate_method = 1;
    if (hr < kHrMin || hr > kHrMax) { hr = peak_fallback_hr(env, env_count); stats.heart_rate_method = 2; }
    stats.heart_rate_bpm = hr;
    if (hr < kHrMin || hr > kHrMax) {
        stats.failure = PreprocessFailure::kHeartRateNotFound;
        free_mem(signal); free_mem(env); free_mem(segment);
        stats.elapsed_ms = static_cast<uint32_t>((esp_timer_get_time() - started) / 1000);
        return stats;
    }

    const int cycle = static_cast<int>(kAnalysisSampleRate * 60.0f / hr + 0.5f);
    const int window = cycle * kCyclesPerSegment;
    int windows = window > 0 ? n / window : 0;
    if (windows < 1 && n > 0) windows = 1;
    int emitted = 0;
    for (int w = 0; w < windows && emitted < kMaxAutoSegments; ++w) {
        memset(segment, 0, kSegmentLen * sizeof(float));
        const int start = w * window;
        int copy = window;
        if (copy > kSegmentLen) copy = kSegmentLen;
        if (copy > n - start) copy = n - start;
        if (copy <= 0) break;
        memcpy(segment, signal + start, copy * sizeof(float));
        if (!callback(segment, emitted + 1, user)) break;
        ++emitted;
    }
    stats.segment_count = emitted;
    stats.valid = emitted > 0;
    stats.failure = emitted > 0 ? PreprocessFailure::kNone : PreprocessFailure::kHeartRateNotFound;
    (void)skip_gating;  // Original MobileHeart pipeline always applies HR gating.
    free_mem(signal); free_mem(env); free_mem(segment);
    stats.elapsed_ms = static_cast<uint32_t>((esp_timer_get_time() - started) / 1000);
    return stats;
}

const char *preprocess_failure_name(PreprocessFailure failure) {
    switch (failure) {
        case PreprocessFailure::kNone: return "OK";
        case PreprocessFailure::kTooShort: return "TOO_SHORT";
        case PreprocessFailure::kNoMemory: return "NO_MEMORY";
        case PreprocessFailure::kWhtFailed: return "WHT_FAILED";
        case PreprocessFailure::kHeartRateNotFound: return "HR_NOT_FOUND";
    }
    return "UNKNOWN";
}

}  // namespace chd
