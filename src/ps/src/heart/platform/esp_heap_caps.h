#ifndef MH_POSIX_HEAP_CAPS_H
#define MH_POSIX_HEAP_CAPS_H

/* Uses the standalone C runtime heap. Put the heap in board DDR.
 * ESP32 capability bits have no meaning on this platform. */
#include <stdlib.h>
#define MALLOC_CAP_SPIRAM 1u
#define MALLOC_CAP_8BIT 2u
static inline void *heap_caps_malloc(size_t bytes, unsigned caps) {
    (void)caps;
    return malloc(bytes);
}
static inline void *heap_caps_calloc(size_t count, size_t bytes, unsigned caps) {
    (void)caps;
    return calloc(count, bytes);
}
static inline void heap_caps_free(void *pointer) { free(pointer); }
#endif
