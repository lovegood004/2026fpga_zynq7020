#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
#define ACQ_PREVIEW_BASE 0x43c50000u
#define ACQ_PREVIEW_BINS 250u
#define ACQ_PREVIEW_BIN_SAMPLES 441u
#define ACQ_PREVIEW_ID 0x41505631u
typedef struct {
 uint32_t session,samples,bins_completed,partial_count,errors;
 uint64_t abs_sum,square_sum;
 uint32_t clips,peak;
 int16_t minimum,maximum;
 // Chronological full bins, followed by an optional partial bin. Fixed 10s.
 int16_t low[ACQ_PREVIEW_BINS],high[ACQ_PREVIEW_BINS];
 uint32_t columns,first_sample;
 uint8_t available,complete;
} AcqPreviewSnapshot;
typedef struct {
 uint32_t magic,version,sessions,feed_calls,fed_samples,feed_us,max_feed_us;
 uint32_t snapshots,snapshot_us,max_snapshot_us,aborted,errors,last_error;
} AcqPreviewReport;
extern volatile AcqPreviewReport acq_preview_report;
int acq_preview_probe(void);
int acq_preview_begin(uint32_t session,uint32_t samples);
int acq_preview_feed(uint32_t session,uint32_t offset,const int16_t *pcm,unsigned count);
int acq_preview_snapshot(AcqPreviewSnapshot *out);
int acq_preview_finish(uint32_t session);
void acq_preview_abort(void);
#ifdef __cplusplus
}
#endif
