/* POPULAR-3 device transport: the tested reference's BLE library and commands.
 * Session worker reused; RAW PCM retained without downsampling; v2 UART is separate.
 * No display, keys, filesystem, SD/TF, Wi-Fi or reference inference code. */
#include <Arduino.h>
#include "raw.hpp"
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>
#include <new>
#include <cstring>
#include <cstdio>
#include <inttypes.h>
extern "C" {
#include "ble_stethoscope.h"
#include "esp_heap_caps.h"

#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
}

static const char *TAG="STETH_BLE";
static constexpr char DEVICE_NAME[]="POPULAR-3";
static constexpr char SERVICE_UUID[]="6E400001-B5A3-F393-E0A9-E50E24DCCA9E";
static constexpr char RX_UUID[]="6E400002-B5A3-F393-E0A9-E50E24DCCA9E";
static constexpr char TX_UUID[]="6E400003-B5A3-F393-E0A9-E50E24DCCA9E";
static portMUX_TYPE mux=portMUX_INITIALIZER_UNLOCKED;
static ble_capture_status_t state;
static uint8_t *pcm;
static raw::PcmParser parser;
struct notification_t { uint32_t generation; uint16_t len; uint8_t data[256]; };
static QueueHandle_t notifications;
static BLEAdvertisedDevice *target;
static BLEClient *client;
static BLERemoteCharacteristic *rx_char,*tx_char;
static bool connected,start_requested,device_streaming,close_requested;
static unsigned notify_count;

static void capture_fail(uint32_t generation,const char *why) {
    portENTER_CRITICAL(&mux);
    if(state.generation!=generation||!state.active||state.stop_pending){portEXIT_CRITICAL(&mux);return;}
    state.active=false;state.recording=false;state.complete=false;
    state.failed=true;state.stop_pending=true;
    snprintf(state.error,sizeof(state.error),"%s",why);
    portEXIT_CRITICAL(&mux);
    ESP_LOGE(TAG,"CAPTURE_FAIL generation=%" PRIu32 " %s",generation,why);
}
static void link_lost(const char *why) {
    portENTER_CRITICAL(&mux);
    connected=false;state.ready=false;state.recording=false;
    device_streaming=false;
    if(state.active){state.active=false;state.failed=true;snprintf(state.error,sizeof(state.error),"BLE %s",why);}
    close_requested=true;
    portEXIT_CRITICAL(&mux);
    ESP_LOGW(TAG,"LINK_LOST %s; automatic reconnect; no automatic recording",why);
}
class PeerCallbacks final:public BLEClientCallbacks {
    void onDisconnect(BLEClient *) override {link_lost("disconnected");}
};
static PeerCallbacks peer_callbacks;
class ScanCallbacks final:public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertised) override {
        if(target||!advertised.haveName()||advertised.getName()!=DEVICE_NAME)return;
        target=new(std::nothrow) BLEAdvertisedDevice(advertised);
        if(target)BLEDevice::getScan()->stop();
    }
};
static ScanCallbacks scan_callbacks;

static void receive_notification(BLERemoteCharacteristic *characteristic,uint8_t *data,size_t len,bool) {
    if(notify_count++<3)ESP_LOGI(TAG,"GATT_NOTIFY handle=0x%04x bytes=%u",characteristic->getHandle(),(unsigned)len);
    portENTER_CRITICAL(&mux);
    bool accept=state.active&&state.recording&&!state.stop_pending;
    uint32_t generation=state.generation;
    portEXIT_CRITICAL(&mux);
    if(!accept)return;
    if(!len||len>sizeof(notification_t::data)){capture_fail(generation,"unexpected BLE notification size");return;}
    notification_t packet={};packet.generation=generation;packet.len=(uint16_t)len;
    memcpy(packet.data,data,len);
    if(xQueueSend(notifications,&packet,0)!=pdTRUE)capture_fail(generation,"BLE notification queue overflow");
}
/* Same CR suffix, write-without-response and 300 ms cooldown as the tested
 * device adapter. Unlike the old app, propagate a failed write to the owner. */
static bool command(const char *text) {
    if(!client||!client->isConnected()||!rx_char)return false;
    char bytes[80];int n=snprintf(bytes,sizeof(bytes),"%s\r",text);
    if(n<0||(size_t)n>=sizeof(bytes))return false;
    ESP_LOGI(TAG,"TX %s handle=0x%04x bytes=%d write=NO_RSP",text,rx_char->getHandle(),n);
    bool ok=rx_char->writeValue(reinterpret_cast<uint8_t *>(bytes),(size_t)n,false);
    ESP_LOGI(TAG,"GATT_WRITE_RETURN ok=%u",(unsigned)ok);
    vTaskDelay(pdMS_TO_TICKS(300));
    return ok&&client->isConnected();
}
/* BLE library dispatches disconnect before returning from its event handler.
 * Cleanup runs only in this worker, after that callback has had time to finish. */
static void close_link(void) {
    portENTER_CRITICAL(&mux);close_requested=true;state.ready=false;portEXIT_CRITICAL(&mux);
    if(client&&client->isConnected()){
        client->disconnect();
        for(unsigned i=0;i<200&&client->isConnected();++i)vTaskDelay(pdMS_TO_TICKS(10));
        if(client->isConnected())return;
    }
    vTaskDelay(pdMS_TO_TICKS(200));
    rx_char=tx_char=nullptr;
    if(client){delete client;client=nullptr;}
    if(target){delete target;target=nullptr;}
    portENTER_CRITICAL(&mux);
    connected=false;state.ready=false;state.recording=false;device_streaming=false;
    state.stop_pending=false;close_requested=false;
    portEXIT_CRITICAL(&mux);
    xQueueReset(notifications);
}
static bool connect_device(void) {
    if(target){delete target;target=nullptr;}
    BLEScan *scan=BLEDevice::getScan();
    scan->setAdvertisedDeviceCallbacks(&scan_callbacks,true);
    scan->setActiveScan(true);
    ESP_LOGI(TAG,"SCANNING name=%s",DEVICE_NAME);
    scan->start(5,false);
    scan->clearResults();
    if(!target)return false;
    ESP_LOGI(TAG,"FOUND %s address=%s type=%u",DEVICE_NAME,target->getAddress().toString().c_str(),(unsigned)target->getAddressType());
    client=BLEDevice::createClient();
    if(!client)return false;
    client->setClientCallbacks(&peer_callbacks);
    if(!client->connect(target))return false;
    portENTER_CRITICAL(&mux);connected=true;portEXIT_CRITICAL(&mux);
    notify_count=0;
    BLERemoteService *service=client->getService(BLEUUID(SERVICE_UUID));
    if(!service)return false;
    rx_char=service->getCharacteristic(BLEUUID(RX_UUID));
    tx_char=service->getCharacteristic(BLEUUID(TX_UUID));
    if(!rx_char||!tx_char||!rx_char->canWriteNoResponse()||!tx_char->canNotify())return false;
    ESP_LOGI(TAG,"NUS_HANDLES rx=0x%04x tx=0x%04x mtu=%u",rx_char->getHandle(),tx_char->getHandle(),client->getMTU());
    tx_char->registerForNotify(receive_notification);
    if(!command("UserLogin 0000"))return false;
    portENTER_CRITICAL(&mux);state.ready=connected&&!close_requested;bool ready=state.ready;portEXIT_CRITICAL(&mux);
    if(ready)ESP_LOGI(TAG,"BLE_READY %s; waiting UART START; RAM only, no TF",DEVICE_NAME);
    return ready;
}

static void ble_task(void *) {
    notification_t packet;
    uint32_t recording_generation=0;
    int64_t capture_begin=0,last_audio=0;
    bool first_notification=false,first_pcm=false;
    for(;;){
        portENTER_CRITICAL(&mux);bool close=close_requested,up=connected;portEXIT_CRITICAL(&mux);
        if(close){close_link();vTaskDelay(pdMS_TO_TICKS(500));continue;}
        if(!up){
            if(!connect_device()){close_link();vTaskDelay(pdMS_TO_TICKS(1000));}
            continue;
        }
        portENTER_CRITICAL(&mux);
        bool stop=state.stop_pending,start=start_requested;
        start_requested=false;uint32_t generation=state.generation;
        bool streaming=device_streaming;
        portEXIT_CRITICAL(&mux);
        if(stop){
            bool stopped=!streaming||command("RealTimeStop");
            xQueueReset(notifications);
            portENTER_CRITICAL(&mux);
            device_streaming=false;
            /* A newer STOP may arrive during the blocking device write. A single
             * device STOP drains both, but never clear a new START reservation. */
            state.stop_pending=false;
            if(!stopped){state.ready=false;close_requested=true;}
            portEXIT_CRITICAL(&mux);
            ESP_LOGI(TAG,"CAPTURE_STOPPED device_stop=%u ok=%u",(unsigned)streaming,(unsigned)stopped);
            continue;
        }
        if(start){
            xQueueReset(notifications);
            parser.reset(pcm,raw::Samples);
            portENTER_CRITICAL(&mux);
            bool can=connected&&state.ready&&state.active&&!state.stop_pending&&state.generation==generation;
            if(can){state.recording=true;device_streaming=true;recording_generation=generation;}
            portEXIT_CRITICAL(&mux);
            if(can){
                first_notification=first_pcm=true;
                capture_begin=last_audio=esp_timer_get_time();
                if(!command("RealTimeStartHEART"))capture_fail(generation,"device start write failed");
                else ESP_LOGI(TAG,"CAPTURE_START generation=%" PRIu32 " target_raw=%u (~15s), no TF",generation,raw::Samples);
            }
        }
        if(xQueueReceive(notifications,&packet,pdMS_TO_TICKS(10))==pdTRUE){
            portENTER_CRITICAL(&mux);
            bool accept=state.active&&state.recording&&!state.stop_pending&&packet.generation==state.generation;
            portEXIT_CRITICAL(&mux);
            if(accept){
                if(first_notification){ESP_LOGI(TAG,"FIRST_NOTIFY generation=%" PRIu32 " bytes=%u",packet.generation,packet.len);first_notification=false;}
                size_t old_packets=parser.packets;
                parser.feed(packet.data,packet.len);
                if(parser.packets!=old_packets)last_audio=esp_timer_get_time();
                if(first_pcm&&parser.packets){ESP_LOGI(TAG,"FIRST_PCM raw=%u raw_points=%u",(unsigned)parser.points,(unsigned)parser.points);first_pcm=false;}
                portENTER_CRITICAL(&mux);
                bool current=state.generation==packet.generation&&state.active&&state.recording&&!state.stop_pending;
                if(current){
                    state.points=parser.points;state.packets=parser.packets;state.lost=parser.lost;state.elapsed_ms=(uint32_t)((esp_timer_get_time()-capture_begin)/1000);
                    if(parser.complete){state.active=false;state.recording=false;state.complete=true;state.stop_pending=true;}
                }
                portEXIT_CRITICAL(&mux);
                if(current&&parser.failed)capture_fail(packet.generation,parser.lost?"device PCM_LOSF":"PCM framing failed");
                if(current&&parser.complete)ESP_LOGI(TAG,"CAPTURE_COMPLETE raw=%u raw_points=%u BLE_packets=%u",(unsigned)parser.points,(unsigned)parser.points,(unsigned)parser.packets);
            }
        }
        ble_capture_status_t s;ble_capture_status(&s);
        int64_t now=esp_timer_get_time();
        if(s.active&&s.recording&&s.generation==recording_generation&&capture_begin>0&&
           (now-last_audio>3000000||now-capture_begin>22000000))
            capture_fail(recording_generation,"PCM timeout (3s idle / 22s capture)");
    }
}
extern "C" bool ble_stethoscope_init(void) {
    /* Check NVS first so Arduino init cannot erase a full/incompatible NVS. */
    esp_err_t err=nvs_flash_init();
    if(err!=ESP_OK){ESP_LOGE(TAG,"NVS init failed: %s",esp_err_to_name(err));return false;}
    pcm=(uint8_t*)heap_caps_malloc(raw::Samples*2u,MALLOC_CAP_SPIRAM|MALLOC_CAP_8BIT);
    if(!pcm)pcm=(uint8_t*)heap_caps_malloc(raw::Samples*2u,MALLOC_CAP_8BIT);
    if(!pcm){ESP_LOGE(TAG,"No RAM for raw PCM; refuse capture");return false;}
    notifications=xQueueCreate(64,sizeof(notification_t));
    if(!notifications)return false;
    initArduino();
    if(!BLEDevice::init("ATK-CHD-TinyML"))return false;
    BLEDevice::setMTU(247);
    ESP_LOGI(TAG,"TRANSPORT reference BLE library; fixed PCM RAM=%u bytes; no filesystem",(unsigned)(raw::Samples*2u));
    return xTaskCreate(ble_task,"steth_ble",8192,nullptr,5,nullptr)==pdPASS;
}
extern "C" bool ble_capture_start(void) {
    portENTER_CRITICAL(&mux);
    bool ok=state.ready&&!state.active&&!state.stop_pending&&!start_requested;
    if(ok){++state.generation;state.points=state.packets=state.lost=0;state.complete=state.failed=false;state.elapsed_ms=0;state.error[0]=0;state.active=true;state.recording=false;start_requested=true;}
    portEXIT_CRITICAL(&mux);return ok;
}
extern "C" void ble_capture_stop(void) {
    portENTER_CRITICAL(&mux);
    state.active=false;state.recording=false;state.complete=false;state.failed=false;state.points=0;
    start_requested=false;++state.generation;
    /* Idle STOP is local, including while scanning. No 5-second scan delay
     * is allowed to hold B's STOP synchronization acknowledgement. */
    state.stop_pending=device_streaming;
    portEXIT_CRITICAL(&mux);
}
extern "C" void ble_capture_status(ble_capture_status_t *out) {
    portENTER_CRITICAL(&mux);*out=state;portEXIT_CRITICAL(&mux);
}
extern "C" bool ble_capture_copy(size_t offset,size_t points,uint8_t *out) {
    ble_capture_status_t s;ble_capture_status(&s);
    return ble_capture_copy_generation(s.generation,offset,points,out);
}
extern "C" bool ble_capture_copy_generation(uint32_t generation,size_t offset,size_t points,uint8_t *out) {
    ble_capture_status_t s;ble_capture_status(&s);
    if(!out||s.generation!=generation||s.failed||offset>s.points||points>s.points-offset)return false;
    // Only UART task requests START; BLE worker appends beyond published points.
    // Do not hold the interrupt critical section while copying from PSRAM.
    memcpy(out,pcm+offset*2,points*2);
    ble_capture_status(&s);return s.generation==generation&&!s.failed&&offset<=s.points&&points<=s.points-offset;
}
extern "C" bool ble_capture_crc(uint32_t *crc) {
    ble_capture_status_t s;ble_capture_status(&s);
    if(!crc||!s.complete||s.failed||s.points!=raw::Samples)return false;
    *crc=raw::crc(pcm,raw::Samples*2u);return true;
}
