#include "profile.hpp"
extern "C" {
unsigned lung_display_optimized=1,lung_display_trace=0;
volatile unsigned lung_display_trace_active=0,lung_display_reserve_calls=0,lung_display_wait_calls=0;
volatile uint64_t lung_display_fifo_wait_us=0;
alignas(64) volatile lung_display::Report lung_display_report;
}
