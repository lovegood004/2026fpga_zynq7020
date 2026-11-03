// Included after the existing lung timeline helpers; no UI/model changes.
lung_display::Report lung_display_state;
uint32_t lung_display_sequence=0;
uint64_t lung_display_began=0;
void lung_display_submitted(uint64_t at,uint32_t duration){
    if(!lung_display_trace_active)return;
    if(!lung_display_state.first_submit_us)lung_display_state.first_submit_us=at;
    lung_display_state.chunk_submit_us+=duration;
}
void lung_display_step(){
    if(!lung_display_trace_active){display.step(write_chunk,nullptr);return;}
    const auto began=micros(),before=lung_display_state.chunk_submit_us;
    display.step(write_chunk,nullptr);
    const auto elapsed=micros()-began,submitted=lung_display_state.chunk_submit_us-before;
    if(elapsed>=submitted)lung_display_state.compare_us+=elapsed-submitted;
}
void lung_display_publish(){
    auto r=lung_display_state;const auto &c=display.counters;
    r.session=lung_timeline.data.session;r.status=lung_timeline.data.status;
    r.flags=lung_timeline.data.flags;r.optimized=lung_display_optimized;r.tracing=lung_display_trace;
    if(r.tracing){
        r.rows=c.rows;r.chunks=c.chunks;r.changed_pixels=c.changed_pixels;r.sent_pixels=c.sent_pixels;
        r.legacy_pixels=c.legacy_pixels;r.wire_bytes=c.wire_bytes;r.legacy_wire_bytes=c.legacy_wire_bytes;
        r.max_chunks_per_row=c.max_chunks_per_row;r.reserve_calls=lung_display_reserve_calls;
        r.wait_calls=lung_display_wait_calls;r.fifo_wait_us=lung_display_fifo_wait_us;
    }
    r.error=lcd_pl_report.error|lcd_pl_report.timeouts;
    const uint32_t odd=lung_display_sequence+1,even=lung_display_sequence+2;
    lung_display_report.sequence=odd;lung_display_report.sequence_end=odd;
    Xil_DCacheFlushRange((UINTPTR)&lung_display_report,sizeof(lung_display_report));asm volatile("dsb sy":::"memory");
    r.sequence=r.sequence_end=odd;
    std::memcpy(const_cast<lung_display::Report*>(&lung_display_report),&r,sizeof(r));
    Xil_DCacheFlushRange((UINTPTR)&lung_display_report,sizeof(lung_display_report));asm volatile("dsb sy":::"memory");
    lung_display_report.sequence=even;lung_display_report.sequence_end=even;lung_display_sequence=even;
    Xil_DCacheFlushRange((UINTPTR)&lung_display_report,sizeof(lung_display_report));asm volatile("dsb sy":::"memory");
}
void lung_display_begin(){
    display.lung_result=lung_same_result();display.split=lung_display_optimized!=0;
    display.trace=lung_display_trace!=0;display.counters={};lung_display_state={};
    lung_display_reserve_calls=lung_display_wait_calls=0;lung_display_fifo_wait_us=0;
    lung_display_began=micros();lung_display_trace_active=display.lung_result&&display.trace;
    lung_display_publish();
}
void lung_display_end(){
    lung_display_trace_active=0;
    if(lung_timeline.data.status==lung_schedule::Complete){
        lung_display_state.final_idle_us=lung_timeline.data.displayed_us;
        lung_display_state.coverage_us=lung_timeline.data.displayed_us-lung_display_began;
    }
    lung_display_publish();display.trace=false;
}
