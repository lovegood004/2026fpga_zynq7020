// Included only in the isolated production runtime after its existing helpers.
lung_schedule::Timeline lung_timeline;
lung_schedule::Budget lung_budget;
uint64_t lung_last_exit=0;
extern "C" {alignas(64) volatile lung_schedule::Report lung_timeline_report;}
extern "C" {uint32_t lung_schedule_enabled=1;}
uint32_t lung_report_sequence=0;
void lung_publish(){
    auto copy=lung_timeline.data;const uint32_t odd=lung_report_sequence+1,even=lung_report_sequence+2;
    lung_timeline_report.sequence=odd;lung_timeline_report.sequence_end=odd;
    Xil_DCacheFlushRange((UINTPTR)&lung_timeline_report,sizeof(lung_timeline_report));asm volatile("dsb sy":::"memory");
    copy.sequence=copy.sequence_end=odd;
    std::memcpy(const_cast<lung_schedule::Report*>(&lung_timeline_report),&copy,sizeof(copy));
    Xil_DCacheFlushRange((UINTPTR)&lung_timeline_report,sizeof(lung_timeline_report));asm volatile("dsb sy":::"memory");
    lung_timeline_report.sequence=even;lung_timeline_report.sequence_end=even;lung_report_sequence=even;
    Xil_DCacheFlushRange((UINTPTR)&lung_timeline_report,sizeof(lung_timeline_report));asm volatile("dsb sy":::"memory");
}
void lung_event(unsigned phase){lung_timeline.phase(phase,micros());}
void lung_begin_timing(){
    const auto at=micros();lung_timeline.begin(sink.pending_descriptor().session,at);
    if(sink.timing_complete&&sink.timing_upload){
        lung_timeline.data.last_pcm_us=lung_schedule::extend_ms(sink.last_data_ms,at);
        lung_timeline.data.crc_us=lung_schedule::extend_ms(sink.complete_ms,at);
        lung_timeline.data.flags|=lung_schedule::LastPcm|lung_schedule::CrcReady;
    }
    lung_set_event_hook(lung_event);lung_publish();
}
void lung_finish_timing(){
    lung_set_event_hook(nullptr);
    lung_timeline.finish(app.lung_ready&&app.view==View::LungResult&&app.lung_meta.session==lung_timeline.data.session&&!app.analysis_cancelled);
    lung_publish();
}
bool lung_same_result(){return app.lung_mode&&app.view==View::LungResult&&app.lung_meta.session==lung_timeline.data.session;}
void lung_chunk_submitted(){
    if(lung_same_result()&&!lcd_pl_report.error)lung_timeline.submit(micros());
}
void lung_paint_complete(){
    if(!lung_same_result()){lung_timeline.invalidate();return;}
    if(lung_timeline.data.status==lung_schedule::ResultReady||lung_timeline.scanning()){
        display.begin_view();lung_timeline.repaint(micros(),display.scan_steps());lung_display_begin();lung_publish();
    }
}
void lung_scan_poll(){
    if(!lung_timeline.scanning())return;
    if(!lung_same_result()){lung_timeline.invalidate();lung_publish();lung_display_end();return;}
    lung_timeline.step();
    if(lcd_pl_report.error||lcd_pl_report.timeouts){lung_timeline.data.status=lung_schedule::Failed;lung_timeline.data.error=1;lung_publish();lung_display_end();return;}
#ifdef RAW_RUNTIME_QEMU
    const bool idle=true; // Simulator has no panel; never a physical timing claim.
#else
    const bool idle=lcd_pl_idle()!=0;
#endif
    lung_timeline.complete(micros(),idle);
    if(lung_timeline.data.status==lung_schedule::Complete){lung_publish();lung_display_end();}
}
void lung_analysis_service(unsigned stage){
    static bool entered=false;if(entered)return;
    struct Guard {bool &flag;Guard(bool&v):flag(v){flag=true;}~Guard(){flag=false;}} guard(entered);
    const uint64_t at=micros();++analysis_report.callbacks;++lung_timeline.data.callbacks;
    if(!lung_schedule::Budget::due(at,lung_budget.service_at,1000))return;
    ++lung_timeline.data.services;
    const uint64_t gap=at-lung_last_exit;
    analysis_report.max_compute_gap_us=std::max(analysis_report.max_compute_gap_us,uint32_t(gap));
    lung_timeline.data.max_gap_us=analysis_report.max_compute_gap_us;
    service();
    app.analysis_stage=std::min(stage,unsigned(LUNG_HEAD));app.now=uint32_t(at/1000);
    app.analysis_elapsed_ms=uint32_t((at-analysis_began)/1000);
    if(lung_schedule::Budget::due(at,lung_budget.key_at,2000)){
        uint32_t ps=(1u<<18)|(1u<<19),pl=0x3c0u;
#ifdef RAW_RUNTIME_QEMU
        ps=analysis_ps_input;pl=analysis_pl_input;if(analysis_key_reader){ps=analysis_key_reader(false);pl=analysis_key_reader(true);}
#else
        ps=XGpioPs_ReadReg(0xe000a000u,XGPIOPS_DATA_RO_OFFSET+4u);
        pl=XGpioPs_ReadReg(0xe000a000u,XGPIOPS_DATA_RO_OFFSET+8u);
#endif
        dispatch_keys(ps,pl,app.now);++analysis_report.key_polls;++lung_timeline.data.keys;
    }
    if(!app.analysis_cancelled){
        if(lung_schedule::Budget::due(at,lung_budget.paint_at,100000)){paint();++lung_timeline.data.paints;}
        if(lung_schedule::Budget::due(at,lung_budget.scan_at,2000)){
            const auto began=micros();
            for(unsigned i=0;i<2;++i){
#if defined(UI_PL_LCD) && !defined(RAW_RUNTIME_QEMU)
                // The unchanged driver reserves at most 240 pixels+7 words.
                // Only the lung analysis path skips work while FIFO is full.
                if(Xil_In32(LCD_PL_BASE+0x14u)<247u)break;
#endif
                lung_display_step();++analysis_report.lcd_steps;++lung_timeline.data.steps;
                if(micros()-began>=1000)break;
            }
        }
    }
    analysis_report.stage=app.analysis_stage;analysis_report.elapsed_ms=app.analysis_elapsed_ms;
    analysis_report.cancelled=app.analysis_cancelled!=0;
    if(lung_schedule::Budget::due(at,lung_budget.publish_at,100000)){
        publish_reports(app.now);lung_publish();
        Xil_DCacheFlushRange((UINTPTR)&analysis_report,sizeof(analysis_report));
    }
    service();lung_last_exit=analysis_exit=micros();
}
