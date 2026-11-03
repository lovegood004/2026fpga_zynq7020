#include "esp_timer.h"
#include "xtime_l.h"

int64_t esp_timer_get_time(void) {
    XTime ticks;
    XTime_GetTime(&ticks);
    const uint64_t rate = (uint64_t)COUNTS_PER_SECOND;
    /* Split the conversion to avoid overflowing ticks * 1000000. */
    return (int64_t)(((uint64_t)ticks / rate) * 1000000u +
                     ((uint64_t)ticks % rate) * 1000000u / rate);
}
