#include "raw.hpp"
extern "C" {
#include "ble_stethoscope.h"
#include "driver/uart.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
}
namespace {
raw::Source source;raw::Decoder decoder;
uint8_t wire[raw::MaxFrame];
QueueHandle_t uart_events;
uint32_t capture_generation;
bool start(void*){if(!ble_capture_start())return false;ble_capture_status_t s{};ble_capture_status(&s);
    capture_generation=s.generation;return true;}
void stop(void*){ble_capture_stop();}
bool copy(void*,uint32_t off,uint32_t n,uint8_t*p){return ble_capture_copy_generation(capture_generation,off,n,p);}
bool send(void*,raw::Op op,const uint8_t*p,size_t n){
    size_t bytes=raw::encode(wire,op,p,n);
    if(!bytes||uart_write_bytes(UART_NUM_1,wire,bytes)!=int(bytes))return false;
    return uart_wait_tx_done(UART_NUM_1,pdMS_TO_TICKS(500))==ESP_OK;
}
}
extern "C" void app_main(){
    // Same UART1 pins/levels as the existing adapter; no USB console reconfiguration.
    uart_config_t config{};config.baud_rate=raw::LinkBaud;config.data_bits=UART_DATA_8_BITS;
    config.parity=UART_PARITY_DISABLE;config.stop_bits=UART_STOP_BITS_1;
    config.flow_ctrl=UART_HW_FLOWCTRL_DISABLE;config.source_clk=UART_SCLK_DEFAULT;
    if(uart_param_config(UART_NUM_1,&config)!=ESP_OK||
       uart_set_pin(UART_NUM_1,17,18,UART_PIN_NO_CHANGE,UART_PIN_NO_CHANGE)!=ESP_OK||
       uart_driver_install(UART_NUM_1,8192,0,16,&uart_events,0)!=ESP_OK){ESP_LOGE("RAW","UART init failed");return;}
    uint32_t actual_baud=0;
    if(uart_get_baudrate(UART_NUM_1,&actual_baud)!=ESP_OK||
       actual_baud<raw::LinkBaud*99u/100u||actual_baud>raw::LinkBaud*101u/100u){
        ESP_LOGE("RAW","UART baud readback invalid: %lu",(unsigned long)actual_baud);return;
    }
    ESP_LOGI("RAW","J10 link 8N1 requested=%lu actual=%lu GPIO17/18 unchanged",
             (unsigned long)raw::LinkBaud,(unsigned long)actual_baud);
    if(!ble_stethoscope_init()){ESP_LOGE("RAW","BLE/raw RAM init failed");return;}
    source.backend={nullptr,start,stop,copy};source.send=send;
    for(;;){
        uart_event_t event;
        for(unsigned i=0;i<16&&xQueueReceive(uart_events,&event,0)==pdTRUE;i++){
            if(event.type==UART_FIFO_OVF||event.type==UART_BUFFER_FULL||event.type==UART_PARITY_ERR||event.type==UART_FRAME_ERR){
                decoder.used=0;uart_flush_input(UART_NUM_1);
                if(source.phase==raw::Phase::Recording||source.phase==raw::Phase::Uploading)source.fail(raw::Error::Transport);
                ESP_LOGE("RAW","UART input fault %d; recording invalidated",int(event.type));
            }
        }
        ble_capture_status_t s{};ble_capture_status(&s);
        raw::CaptureState cs{s.ready,s.recording,s.complete,s.failed,s.stop_pending,uint32_t(s.points),s.elapsed_ms};
        uint32_t now=uint32_t(esp_timer_get_time()/1000);source.tick(cs,now);
        uint8_t input[64];int got=uart_read_bytes(UART_NUM_1,input,sizeof(input),pdMS_TO_TICKS(2));
        if(got<0){if(source.phase==raw::Phase::Recording||source.phase==raw::Phase::Uploading)source.fail(raw::Error::Transport);continue;}
        for(int i=0;i<got;i++){raw::Op op;const uint8_t*p;size_t n;
            if(decoder.feed(input[i],uint32_t(esp_timer_get_time()/1000),op,p,n))
                source.frame(op,p,n,uint32_t(esp_timer_get_time()/1000));}
    }
}
