#include "ui_port.hpp"
#include "events.hpp"
#include "../modules/lung_display/display.hpp"
#include "pw_profile.h"
#ifdef UI_PL_ACQ
#include "acq_preview_session.hpp"
#endif
#ifdef MH_PL_MODEL
#include "model_arm.h"
#endif
#include "rx_ring.hpp"
extern "C" {
#include "xparameters.h"
#include "xparameters_ps.h"
#include "xuartps.h"
#include "xgpiops_hw.h"
#include "xil_cache.h"
#include "xtime_l.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "keys.h"
#include "lcd.h"
#ifdef UI_PL_LCD
#include "lcd_pl.h"
#endif
}
namespace {
constexpr unsigned UART_BASE=0xe0000000u;
constexpr unsigned ERRORS=XUARTPS_IXR_OVER|XUARTPS_IXR_FRAMING|XUARTPS_IXR_PARITY;
struct Report {
    uint32_t magic=0x32525348,version=2,uptime=0,phase=0,error=0,view=0,site=0;
    uint32_t uart_errors=0,rx_dropped=0,frames_good=0,frames_bad=0,points=0,recorded=0;
    uint32_t valid_mask=0,complete=0,chunks=0,pixels=0,scans=0,max_chunk_us=0,pl_calls=0;
};
struct TimingReport {
    uint32_t magic=0x544d5231,version=3,sequence=0,baud_requested=raw::LinkBaud,baud_actual=0;
    StageTiming sites[5]{};
    uint32_t sequence_end=0;
};
static_assert(sizeof(TimingReport)==584,"Timing report v3 size");
}
struct UartDiagnostics {
 uint32_t magic=0x55445231,version=1,sequence=0,irq_count=0,overruns=0,framing=0,parity=0;
 uint32_t first_error_us=0,last_error_us=0,first_status=0,last_status=0;
 uint32_t phase=0,points=0,pw_phase=0,max_irq_gap_us=0,max_service_gap_us=0,max_ring=0;
 uint32_t bytes=0,sequence_end=0;
};
extern "C" {volatile Report raw_report;TimingReport raw_timing;volatile UartDiagnostics uart_diagnostics;}
namespace {
raw::Sink sink;raw::Decoder decoder;App app;Display display;
RxRing rx_ring;XUartPs uart;XScuGic gic;Key keys[6];
volatile uint32_t irq_errors=0,irq_faults=0;
uint8_t tx_queue[4096];unsigned tx_read=0,tx_write=0;
uint32_t last_hello=0;
#ifdef UI_PL_ACQ
AcqPreviewSession preview;
AcqPreviewSnapshot &live_snapshot=preview.snapshot;
#endif
uint64_t micros(){XTime t;XTime_GetTime(&t);return t/COUNTS_PER_SECOND*1000000u+(t%COUNTS_PER_SECOND)*1000000u/COUNTS_PER_SECOND;}
bool enqueue(void*,raw::Op op,const uint8_t*p,size_t n){
    uint8_t frame[raw::MaxFrame];size_t bytes=raw::encode(frame,op,p,n);
    if(!bytes||bytes>4095-((tx_write-tx_read)&4095))return false;
    for(size_t i=0;i<bytes;i++){tx_queue[tx_write]=frame[i];tx_write=(tx_write+1)&4095;}return true;
}
void uart_irq(void*){
    static uint32_t previous=0;uint32_t at=uint32_t(micros());
    ++uart_diagnostics.sequence;++uart_diagnostics.irq_count;
    if(previous&&at-previous>uart_diagnostics.max_irq_gap_us)uart_diagnostics.max_irq_gap_us=at-previous;
    previous=at;
    uint32_t status=XUartPs_ReadReg(UART_BASE,XUARTPS_ISR_OFFSET),error=status&ERRORS;
    if(error){irq_errors|=error;++irq_faults;
        if(!uart_diagnostics.overruns&&!uart_diagnostics.framing&&!uart_diagnostics.parity){
            uart_diagnostics.first_error_us=at;uart_diagnostics.first_status=status;
        }
        uart_diagnostics.overruns+=!!(error&XUARTPS_IXR_OVER);
        uart_diagnostics.framing+=!!(error&XUARTPS_IXR_FRAMING);
        uart_diagnostics.parity+=!!(error&XUARTPS_IXR_PARITY);
        uart_diagnostics.last_error_us=at;uart_diagnostics.last_status=status;
        uart_diagnostics.phase=unsigned(sink.phase);uart_diagnostics.points=sink.points;
        uart_diagnostics.pw_phase=pw_profile.phase;
    }
    while(XUartPs_IsReceiveData(UART_BASE)){
        rx_ring.push(uint8_t(XUartPs_ReadReg(UART_BASE,XUARTPS_FIFO_OFFSET)));++uart_diagnostics.bytes;
    }
    unsigned depth=(rx_ring.write-rx_ring.read)&(RxRing::N-1);
    if(depth>uart_diagnostics.max_ring)uart_diagnostics.max_ring=depth;
    ++uart_diagnostics.sequence;uart_diagnostics.sequence_end=uart_diagnostics.sequence;
    XUartPs_WriteReg(UART_BASE,XUARTPS_ISR_OFFSET,status);
    // Cadence UART requires restarting the timeout counter after a timeout IRQ.
    // Clearing ISR alone leaves the receive timeout asserted in the controller.
    if(status&XUARTPS_IXR_TOUT)
        XUartPs_WriteReg(UART_BASE,XUARTPS_CR_OFFSET,
            XUartPs_ReadReg(UART_BASE,XUARTPS_CR_OFFSET)|XUARTPS_CR_TORST);
}
int uart_init(){
    XUartPs_Config *cfg=nullptr;
    for(unsigned i=0;i<XPAR_XUARTPS_NUM_INSTANCES;i++){auto c=XUartPs_LookupConfig(i);if(c&&c->BaseAddress==UART_BASE){cfg=c;break;}}
    if(!cfg||XUartPs_CfgInitialize(&uart,cfg,UART_BASE)!=XST_SUCCESS)return 1;
    XUartPsFormat format={raw::LinkBaud,XUARTPS_FORMAT_8_BITS,XUARTPS_FORMAT_NO_PARITY,XUARTPS_FORMAT_1_STOP_BIT};
    if(XUartPs_SetDataFormat(&uart,&format)!=XST_SUCCESS)return 1;
    uint32_t generator=XUartPs_ReadReg(UART_BASE,XUARTPS_BAUDGEN_OFFSET)&XUARTPS_BAUDGEN_MASK;
    uint32_t divider=XUartPs_ReadReg(UART_BASE,XUARTPS_BAUDDIV_OFFSET)&XUARTPS_BAUDDIV_MASK;
    uint32_t clock=cfg->InputClockHz;
    if(XUartPs_ReadReg(UART_BASE,XUARTPS_MR_OFFSET)&XUARTPS_MR_CLKSEL)clock/=8;
    if(!generator)return 1;
    raw_timing.baud_actual=clock/(generator*(divider+1));
    if(raw_timing.baud_actual<raw::LinkBaud*99u/100u||raw_timing.baud_actual>raw::LinkBaud*101u/100u)return 1;
    XUartPs_DisableUart(&uart);XUartPs_SetInterruptMask(&uart,0);XUartPs_SetOperMode(&uart,XUARTPS_OPER_MODE_NORMAL);
    XUartPs_SetOptions(&uart,XUARTPS_OPTION_RESET_RX|XUARTPS_OPTION_RESET_TX);
    auto start=micros();while(XUartPs_ReadReg(UART_BASE,XUARTPS_CR_OFFSET)&(XUARTPS_CR_RXRST|XUARTPS_CR_TXRST))if(micros()-start>1000)return 1;
    XUartPs_WriteReg(UART_BASE,XUARTPS_ISR_OFFSET,XUARTPS_IXR_MASK);XUartPs_EnableUart(&uart);
    auto gc=XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);
    if(!gc||XScuGic_CfgInitialize(&gic,gc,gc->CpuBaseAddress)!=XST_SUCCESS)return 1;
    Xil_ExceptionInit();Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,&gic);
    if(XScuGic_Connect(&gic,XPS_UART0_INT_ID,uart_irq,nullptr)!=XST_SUCCESS)return 1;
    XScuGic_Enable(&gic,XPS_UART0_INT_ID);
    XUartPs_WriteReg(UART_BASE,XUARTPS_RXWM_OFFSET,8);XUartPs_WriteReg(UART_BASE,XUARTPS_RXTOUT_OFFSET,16);
    XUartPs_WriteReg(UART_BASE,XUARTPS_IER_OFFSET,XUARTPS_IXR_RXOVR|XUARTPS_IXR_TOUT|ERRORS);
    Xil_ExceptionEnable();return 0;
}
void service(unsigned=0){
    uint32_t at=uint32_t(micros());static uint32_t serviced=0;
    if(serviced&&at-serviced>uart_diagnostics.max_service_gap_us)uart_diagnostics.max_service_gap_us=at-serviced;
    serviced=at;
    uint32_t now=uint32_t(micros()/1000);static uint32_t faults=0,dropped=0;
    if(faults!=irq_faults||dropped!=rx_ring.dropped){faults=irq_faults;dropped=rx_ring.dropped;
        decoder.used=0;sink.fail(raw::Error::Transport,now);}
    uint8_t b;while(rx_ring.pop(b)){raw::Op op;const uint8_t*p;size_t n;
        if(decoder.feed(b,now,op,p,n))sink.frame(op,p,n,now);}
#ifdef UI_PL_ACQ
    preview.service(sink,now);
#endif
    if(now-last_hello>=500){last_hello=now;if(!enqueue(nullptr,raw::Op::Hello,nullptr,0))sink.fail(raw::Error::Transport,now);}
    while(tx_read!=tx_write&&!XUartPs_IsTransmitFull(UART_BASE)){
        XUartPs_WriteReg(UART_BASE,XUARTPS_FIFO_OFFSET,tx_queue[tx_read]);tx_read=(tx_read+1)&4095;}
}
void lung_chunk_submitted();void lung_paint_complete();void lung_scan_poll();
void lung_display_begin();void lung_display_end();void lung_display_step();void lung_display_submitted(uint64_t,uint32_t);
void write_chunk(void*,unsigned y,unsigned x,const uint16_t*p,unsigned n){
    auto begin=micros();
#ifdef RAW_RUNTIME_QEMU
    // QEMU has no physical LCD/EMIO. Keep rendering and the same chunk scheduler.
    (void)y;(void)x;(void)p;(void)n;
#else
    lcd_chunk(y,x,p,n);
#endif
    lung_chunk_submitted();
    uint32_t duration=uint32_t(micros()-begin);
    lung_display_submitted(begin,duration);
    if(duration>raw_report.max_chunk_us)raw_report.max_chunk_us=duration;
}
struct Stamp {
    unsigned view,site,menu,phase,error,points,recorded,valid,completed,online,ready,window,lung;
    const char *message;
#ifdef UI_PL_ACQ
    unsigned live_samples;
#endif
    bool operator==(const Stamp &o)const{return view==o.view&&site==o.site&&menu==o.menu&&phase==o.phase&&error==o.error&&
        points==o.points&&recorded==o.recorded&&valid==o.valid&&completed==o.completed&&online==o.online&&ready==o.ready&&
        window==o.window&&lung==o.lung&&message==o.message
#ifdef UI_PL_ACQ
        &&live_samples==o.live_samples
#endif
        ;}
};
Stamp stamp(uint32_t now){return {unsigned(app.view),app.site,app.menu,unsigned(sink.phase),unsigned(sink.error),sink.points/4096,
    sink.recorded/4000,sink.valid_mask,sink.completed,unsigned(sink.online(now)),unsigned(sink.ready),app.wave_window+(app.lung_wave_window<<1),unsigned(app.lung_mode),app.message
#ifdef UI_PL_ACQ
    ,live_snapshot.available?live_snapshot.samples:0u
#endif
    };}
// One shared iteration for the production entry and the UART/GIC integration test.
// bank is a GPIO sample, not a direct UI event: the same debounce code always runs.
unsigned result_scan_left=0,result_scan_site=0;
uint32_t result_scan_started=0,result_scan_session=0;
void paint(){
    display.lung_result=app.lung_mode&&app.view==View::LungResult;
    display.split=lung_display_optimized!=0;
    if(!display.lung_result){display.trace=false;lung_display_trace_active=0;}
    static bool painted=false,lung=false,analysing=false;
    static View view;static unsigned site=0,menu=0,window=0;
    bool preview_changed=false;
#ifdef UI_PL_ACQ
    static bool was_live=false;
    const bool is_live=app.view==View::Capture&&sink.busy()&&sink.phase!=raw::Phase::Stopping&&
        live_snapshot.available&&live_snapshot.session==sink.wanted.session;
    preview_changed=is_live!=was_live;was_live=is_live;
#endif
    if(!painted||view!=app.view||site!=app.site||menu!=app.menu||lung!=app.lung_mode||
       window!=app.wave_window+(app.lung_wave_window<<1)||analysing!=app.analysing||preview_changed)display.begin_view();
    painted=true;view=app.view;site=app.site;menu=app.menu;lung=app.lung_mode;
    window=app.wave_window+(app.lung_wave_window<<1);analysing=app.analysing;
    auto began=micros();app.paint(display.logical);lung_paint_complete();
    if(result_scan_left){
        if(app.view==View::Summary&&app.site==result_scan_site){
            // A repaint restarts coverage: wait a full scan of the latest framebuffer.
            app.timings[result_scan_site].paint_us+=uint32_t(micros()-began);
            result_scan_left=display.scan_steps();
        }else result_scan_left=0; // A superseded result is not reported as fully shown.
    }
}
void dispatch_keys(uint32_t bank,uint32_t pl_bank,uint32_t now){
    unsigned events[6]{};
    for(unsigned i=0;i<2;i++)events[i]=key_update(&keys[i],!(bank&(1u<<(18+i))),now);
#ifdef UI_PL_KEYS
    // Matching keys_ui bit: EMIO[6+i] is PL KEY(i+1), read-only in fabric.
    uint32_t pl=pl_bank;
    for(unsigned i=0;i<4;i++)events[i+2]=key_update(&keys[i+2],!(pl&(1u<<(6+i))),now);
#endif
    app.key_batch(events,now);
}
void publish_reports(uint32_t now){
    static uint32_t last_report=0;
    if(now-last_report>=100){last_report=now;
            raw_report.uptime=now;raw_report.phase=unsigned(sink.phase);raw_report.error=unsigned(sink.error);
            raw_report.view=unsigned(app.view);raw_report.site=app.site;raw_report.uart_errors=irq_errors;raw_report.rx_dropped=rx_ring.dropped;
            raw_report.frames_good=decoder.good;raw_report.frames_bad=decoder.bad;raw_report.points=sink.points;raw_report.recorded=sink.recorded;
            raw_report.valid_mask=sink.valid_mask;raw_report.complete=sink.completed;raw_report.chunks=display.chunks;
            raw_report.pixels=display.pixels;raw_report.scans=display.scans;raw_report.pl_calls=integrated_pl_calls();
            Xil_DCacheFlushRange((UINTPTR)&raw_report,sizeof(raw_report));asm volatile("dsb sy":::"memory");
#ifdef UI_PL_LCD
            Xil_DCacheFlushRange((UINTPTR)&lcd_pl_report,sizeof(lcd_pl_report));
#endif
            Xil_DCacheFlushRange((UINTPTR)&pw_profile,sizeof(pw_profile));
#ifdef UI_PL_ACQ
            Xil_DCacheFlushRange((UINTPTR)&acq_preview_report,sizeof(acq_preview_report));
#endif
#ifdef MH_PL_MODEL
            Xil_DCacheFlushRange((UINTPTR)&model_dw_profile,sizeof(model_dw_profile));
            Xil_DCacheFlushRange((UINTPTR)&model_bn_profile,sizeof(model_bn_profile));
#endif
            Xil_DCacheFlushRange((UINTPTR)&uart_diagnostics,sizeof(uart_diagnostics));
            Xil_DCacheFlushRange((UINTPTR)&app.lung_timing,sizeof(app.lung_timing));
            Xil_DCacheFlushRange((UINTPTR)&app.lung_profile,sizeof(app.lung_profile));
            // A reader accepts only identical even sequence/end and a stable re-read.
            ++raw_timing.sequence;Xil_DCacheFlushRange((UINTPTR)&raw_timing,sizeof(raw_timing));
            asm volatile("dsb sy":::"memory");
            for(unsigned i=0;i<5;i++)raw_timing.sites[i]=app.timings[i];
            ++raw_timing.sequence;raw_timing.sequence_end=raw_timing.sequence;
            Xil_DCacheFlushRange((UINTPTR)&raw_timing,sizeof(raw_timing));asm volatile("dsb sy":::"memory");
        }
}
struct AnalysisReport {
    uint32_t magic=0x414e5231,version=1,stage=0,elapsed_ms=0;
    uint32_t callbacks=0,key_polls=0,lcd_steps=0,max_compute_gap_us=0,cancelled=0;
};
extern "C" {AnalysisReport analysis_report;}
uint64_t analysis_began=0,analysis_exit=0,analysis_keys=0,analysis_scan=0,analysis_paint=0;
#ifdef RAW_RUNTIME_QEMU
uint32_t (*analysis_key_reader)(bool)=nullptr;
uint32_t analysis_ps_input=(1u<<18)|(1u<<19),analysis_pl_input=0x3c0u;
#endif
void lung_reset_budget();
void begin_lung_analysis(){
    analysis_report=AnalysisReport{};analysis_began=analysis_exit=micros();
    analysis_keys=analysis_scan=analysis_paint=0;
    if(app.capture_lung&&app.lung_mode&&!app.analysis_cycle)lung_reset_budget();
}
#include "runtime_lung.inc"
#include "../modules/lung_display/runtime.inc"
void lung_reset_budget(){lung_budget.reset(micros());lung_last_exit=micros();}
void analysis_service(unsigned stage){
    if(lung_schedule_enabled&&app.capture_lung&&app.lung_mode&&!app.analysis_cycle&&app.analysing){lung_analysis_service(stage);return;}
    static bool entered=false;if(entered)return;
    struct Guard {bool &flag;Guard(bool&f):flag(f){flag=true;}~Guard(){flag=false;}} guard(entered);
    service();if(!app.analysing||(!app.capture_lung&&!app.analysis_cycle))return;
    uint64_t at=micros();++analysis_report.callbacks;
    if(at-analysis_exit>analysis_report.max_compute_gap_us)analysis_report.max_compute_gap_us=uint32_t(at-analysis_exit);
    app.analysis_stage=std::min(stage,unsigned(LUNG_HEAD));app.now=uint32_t(at/1000);
    app.analysis_elapsed_ms=uint32_t((at-analysis_began)/1000);
    if(at-analysis_keys>=2000){
        analysis_keys=at;
        uint32_t ps=(1u<<18)|(1u<<19),pl=0x3c0u;
#ifdef RAW_RUNTIME_QEMU
        ps=analysis_ps_input;pl=analysis_pl_input;
        if(analysis_key_reader){ps=analysis_key_reader(false);pl=analysis_key_reader(true);}
#else
        ps=XGpioPs_ReadReg(0xe000a000u,XGPIOPS_DATA_RO_OFFSET+4u);
#ifdef UI_PL_KEYS
        pl=XGpioPs_ReadReg(0xe000a000u,XGPIOPS_DATA_RO_OFFSET+8u);
#endif
#endif
        dispatch_keys(ps,pl,app.now);++analysis_report.key_polls;
    }
    if(at-analysis_paint>=100000){analysis_paint=at;paint();}
    if(at-analysis_scan>=500){
        analysis_scan=at;const auto began=micros();
        // Bounded scan work; low-level FIFO waits only re-enter UART service.
        for(unsigned i=0;i<4;i++){lung_display_step();++analysis_report.lcd_steps;
            if(micros()-began>=1000)break;}
    }
    analysis_report.stage=app.analysis_stage;analysis_report.elapsed_ms=app.analysis_elapsed_ms;
    analysis_report.cancelled=app.analysis_cancelled!=0;
    publish_reports(app.now);
    Xil_DCacheFlushRange((UINTPTR)&analysis_report,sizeof(analysis_report));
    service();analysis_exit=micros();
}
void runtime_step(uint32_t bank,uint32_t pl_bank=0x3c0u){
        static Stamp previous=stamp(0);
#ifdef RAW_RUNTIME_QEMU
        analysis_ps_input=bank;analysis_pl_input=pl_bank;
#endif
        service();uint32_t now=uint32_t(micros()/1000);app.tick(now);
        dispatch_keys(bank,pl_bank,now);
        if(app.analysis_request&&!sink.busy()){
            const bool lung_run=app.capture_lung&&app.lung_mode&&!app.analysis_cycle;
            if(lung_run)lung_begin_timing();
            if(app.capture_lung){app.analysis_stage=LUNG_PREVIEW;app.analysis_elapsed_ms=0;}
            app.analysing=true;paint();app.analysing=false;
            service();if(app.capture_lung)begin_lung_analysis();
            app.analyse(app.capture_lung?analysis_service:service);
            if(lung_run)lung_finish_timing();
            result_scan_site=app.site;result_scan_session=app.timings[app.site].session;
            result_scan_started=uint32_t(micros()/1000);
            result_scan_left=app.view==View::Summary?display.scan_steps():0;paint();
        }
        if(app.feature_request&&!sink.busy()){
            app.analysing=true;paint();app.analysing=false;
            begin_lung_analysis();app.analyse_feature(analysis_service);paint();
        }
        now=uint32_t(micros()/1000);app.now=now;
        auto current=stamp(now);if(!(current==previous)){previous=current;paint();}
        service();lung_display_step();lung_scan_poll();service();
        if(result_scan_left){
            if(result_scan_left>1)--result_scan_left;
            else {
#ifdef UI_PL_LCD
                // Queue acceptance is not panel completion. Wait for the wire.
                if(lcd_pl_idle()){
#else
                {
#endif
                result_scan_left=0;auto &t=app.timings[result_scan_site];
            if(t.valid&&t.session==result_scan_session){uint32_t ended=uint32_t(micros()/1000);
                t.result_scan_ms=ended-result_scan_started;t.key_to_scan_ms=ended-sink.started_ms;t.scan_complete=true;}
                }
            }
        }
        publish_reports(now);
}
}
#ifndef RAW_RUNTIME_QEMU
int main(){
    Xil_ICacheEnable();Xil_DCacheEnable();sink.send=enqueue;app.sink=&sink;
#ifdef UI_PL_ACQ
    app.live_preview=&live_snapshot;
#endif
    // Same GPIO pins/UART routing; LCD half-cycle timing validated separately.
    XUartPs_WriteReg(UART_BASE,XUARTPS_CR_OFFSET,XUARTPS_CR_RX_DIS|XUARTPS_CR_TX_DIS);
#ifdef MH_MODEL_REPLAY
    // Saved PCM only, before UART/LCD startup. Failure leaves the candidate stopped.
    extern bool model_replay_run();
    if(!model_replay_run())for(;;){}
#endif
    lcd_init();
#ifdef UI_PL_LCD
    if(!lcd_pl_ready()){
        Xil_DCacheFlushRange((UINTPTR)&lcd_pl_report,sizeof(lcd_pl_report));
        asm volatile("dsb sy":::"memory");
        for(;;){}
    }
#endif
    if(uart_init()){raw_report.error=unsigned(raw::Error::Transport);for(;;){}}
#ifdef UI_PL_LCD
    lcd_pl_service(service);
#endif
#ifdef UI_PL_KEYS
    // EMIO[6:9] are input-only in fabric; explicitly keep PS direction/OE inputs.
    for(unsigned offset:{XGPIOPS_DIRM_OFFSET,XGPIOPS_OUTEN_OFFSET}){
        const unsigned reg=offset+2*XGPIOPS_REG_MASK_OFFSET;
        XGpioPs_WriteReg(0xe000a000u,reg,XGpioPs_ReadReg(0xe000a000u,reg)&~0x3c0u);
    }
#endif
    #ifdef UI_PL_LCD
    display.batch=true;
#endif
    shared_arm_enable(1);
#ifdef MH_PL_MODEL
    // First physical replay was slower with DW/BN. Keep the measured PW path
    // as the menu default until a new candidate earns a performance acceptance.
#ifndef MH_MODEL_DEFAULT_ENABLE
#define MH_MODEL_DEFAULT_ENABLE 0
#endif
    model_arm_enable(MH_MODEL_DEFAULT_ENABLE);
#endif
    paint_startup(display.logical);
    const auto boot_began=micros();
    // Original five-second splash, with UART serviced throughout and no START.
    while(micros()-boot_began<5000000u){service();lung_display_step();}
    paint();
    for(;;){
        const uint32_t ps=XGpioPs_ReadReg(0xe000a000u,XGPIOPS_DATA_RO_OFFSET+4u);
#ifdef UI_PL_KEYS
        runtime_step(ps,XGpioPs_ReadReg(0xe000a000u,XGPIOPS_DATA_RO_OFFSET+8u));
#else
        runtime_step(ps);
#endif
    }
}
#endif

