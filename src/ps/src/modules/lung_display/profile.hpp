#pragma once
#include <cstdint>
namespace lung_display {
struct Counters {
    uint32_t rows=0,chunks=0,changed_pixels=0,sent_pixels=0,legacy_pixels=0;
    uint32_t wire_bytes=0,legacy_wire_bytes=0,max_chunks_per_row=0;
};
struct Report {
    uint32_t magic=0x4c445331,version=1,sequence=0,session=0;
    uint32_t flags=0,status=0,optimized=0,tracing=0;
    uint32_t rows=0,chunks=0,changed_pixels=0,sent_pixels=0,legacy_pixels=0;
    uint32_t wire_bytes=0,legacy_wire_bytes=0,reserve_calls=0;
    uint64_t fifo_wait_us=0,chunk_submit_us=0,compare_us=0;
    uint64_t first_submit_us=0,final_idle_us=0,coverage_us=0;
    uint32_t max_chunks_per_row=0,wait_calls=0,error=0,sequence_end=0;
};
static_assert(sizeof(Report)==128,"Display report ABI");
}
extern "C" {
extern unsigned lung_display_optimized,lung_display_trace;
extern volatile unsigned lung_display_trace_active,lung_display_reserve_calls,lung_display_wait_calls;
extern volatile uint64_t lung_display_fifo_wait_us;
extern volatile lung_display::Report lung_display_report;
}
