#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
    /* active reserves a UART request; recording starts only when BLE sends START. */
    bool ready, active, recording, complete, failed, stop_pending;
    size_t points, packets;
    unsigned lost;
    uint32_t generation, elapsed_ms;
    char error[80];
} ble_capture_status_t;
bool ble_stethoscope_init(void);
bool ble_capture_start(void);
/* Immediately gate notifications; STOP ACK must wait until stop_pending clears. */
void ble_capture_stop(void);
void ble_capture_status(ble_capture_status_t *status);
bool ble_capture_copy(size_t offset, size_t points, uint8_t *pcm_le16);
/* Reject stale generation both before and after reading published immutable PCM. */
bool ble_capture_copy_generation(uint32_t generation, size_t offset, size_t points, uint8_t *pcm_le16);
bool ble_capture_crc(uint32_t *crc);
