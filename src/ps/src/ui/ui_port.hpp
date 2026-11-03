#pragma once
#include "raw.hpp"
#include "analysis.hpp"
#include "../features/cycle.h"
#include "../features/lung.h"
#ifdef UI_PL_ACQ
#include "acq_preview.h"
#endif
// Preserve existing report values; append the new detail page.
enum class View {Menu,Connect,Capture,Move,Summary,Wave,Stft,Table,Status,Unavailable,Scores,NewExamConfirm,Cycle,LungResult,AnalyzeSaved};
enum class UiAction {Previous,Next,Confirm,Back,Measure,Menu,NextSite};
struct StageTiming {
    uint32_t session=0,record_wait_ms=0,upload_ms=0,capture_to_crc_ms=0;
    uint32_t preview_us=0,analysis_us=0,inference_ms=0,paint_us=0;
    uint32_t result_scan_ms=0,key_to_scan_ms=0;
    // In streaming mode upload_ms spans acquisition AND transfer; do not add
    // the device recording duration to it or call it post-record upload time.
    bool valid=false,scan_complete=false,stream_window=false;
    uint32_t first_data_ms=0,pcm_to_crc_ms=0,preprocessing_us=0,inference_us=0;
    uint32_t mobile_ms=0,tcn_ms=0,wave_us=0,stft_us=0;
    uint32_t operator_us[9]{};
};
static_assert(sizeof(StageTiming)==112,"Timing report v3 wire layout changed");
struct App {
    raw::Sink *sink=nullptr;View view=View::Menu;unsigned menu=0,site=0,page=0;
    unsigned analysis_request=0,seen_completed=0;uint32_t session=0,now=0;
    unsigned wave_window=0;
    unsigned lung_wave_window=0;
    LungWave lung_wave{},candidate_lung_wave{};
    bool analysing=false;const char*message="";
    int analysis_cancelled=0;bool analysis_to_menu=false;
    unsigned analysis_stage=0;uint32_t analysis_elapsed_ms=0;
    feature_lung_input_stats capture_stats{};uint32_t stats_session=0;
    feature_lung_timing lung_profile{},candidate_lung_profile{};
    Result results[5]{};Preview previews[5]{};
    StageTiming timings[5]{};
    // Fixed scratch, never placed on the small bare-metal stack. All committed
    // recording/result/plot fields remain intact during a replacement attempt.
    Result candidate_result{};Preview candidate_preview{};StageTiming candidate_timing{};
    uint32_t examination=1;
    // Cycle annotation is an explicit optional operation; never replaces the
    // original Ghost preprocessing. Lung data cannot overwrite heart sites.
    struct Cycle {feature_cycle_result result{};uint8_t states[750]{};bool attempted=false;uint32_t elapsed_ms=0;};
    Cycle cycles[5]{},candidate_cycle{};unsigned feature_request=0;bool cycle_marks_enabled=false,analysis_cycle=false;
    bool lung_mode=false,capture_lung=false;
    int16_t lung_pcm[raw::Samples]{};raw::Descriptor lung_meta{};
    feature_lung_result lung_result{};Preview lung_preview{};StageTiming lung_timing{};
    bool lung_ready=false;
    // A local reanalysis never sends START or commits the transport staging slot.
    bool reanalyse_saved=false;
#ifdef UI_PL_ACQ
    const AcqPreviewSnapshot *live_preview=nullptr;
#endif
    void key(unsigned which,unsigned event,uint32_t now);
    void action(UiAction action,uint32_t now);
    void key_batch(const unsigned events[6],uint32_t now);
    void tick(uint32_t now);
    void analyse(Progress service=nullptr);
    void analyse_feature(Progress service=nullptr);
    void paint(uint16_t *logical_framebuffer)const;
};
constexpr unsigned UI_W=320,UI_H=240;
void paint_startup(uint16_t *logical_framebuffer);
