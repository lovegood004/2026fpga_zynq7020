/* Matching lcd_spi_axil ABI only. No GPIO output / no automatic fallback. */
#include "lcd.h"
#include "lcd_pl.h"
#include "xil_io.h"
#include "xtime_l.h"
#include "sleep.h"
#define CTRL 0x00u
#define HALF 0x04u
#define STATUS 0x08u
#define ERROR 0x0cu
#define DATA 0x10u
#define FREE 0x14u
#define ID 0x20u
volatile LcdPlReport lcd_pl_report={LCD_PL_ID,1,0,0,0,0,0};
static void (*service_fn)(unsigned);
static int ready;
static uint32_t control;
static uint64_t usec(void){XTime t;XTime_GetTime(&t);return t/COUNTS_PER_SECOND*1000000u+(t%COUNTS_PER_SECOND)*1000000u/COUNTS_PER_SECOND;}
static uint32_t read_reg(unsigned r){return Xil_In32(LCD_PL_BASE+r);}
static void write_reg(unsigned r,uint32_t v){Xil_Out32(LCD_PL_BASE+r,v);}
void lcd_pl_service(void (*fn)(unsigned)){service_fn=fn;}
static void service(void){if(service_fn)service_fn(0);}
static void fail(uint32_t error){
    lcd_pl_report.error|=error;ready=0;
    /* Abort queue, keep reset high and backlight off. No automatic retry. */
    control=2;write_reg(CTRL,control);
}
int lcd_pl_ready(void){return ready;}
int lcd_pl_idle(void){
    if(!ready)return 0;
    uint32_t error=read_reg(ERROR);
    if(error){fail(error<<8);return 0;}
    return !(read_reg(STATUS)&0x20000u);
}
static int wait_idle(void){
    uint64_t begin=usec();
    while(ready&&!lcd_pl_idle()){
        service();
        if(usec()-begin>100000u){++lcd_pl_report.timeouts;fail(2);return 0;}
    }
    return ready;
}
extern volatile unsigned lung_display_trace_active,lung_display_reserve_calls,lung_display_wait_calls;
extern volatile uint64_t lung_display_fifo_wait_us;
static void trace_wait(uint64_t began,int waited){
    if(lung_display_trace_active&&waited){++lung_display_wait_calls;lung_display_fifo_wait_us+=usec()-began;}
}
static int reserve(unsigned words){
    if(!ready)return 0;
    uint64_t begin=usec();
    int waited=0;
    if(lung_display_trace_active)++lung_display_reserve_calls;
    while(read_reg(FREE)<words){
        waited=1;
        if(read_reg(ERROR)){trace_wait(begin,waited);fail(read_reg(ERROR)<<8);return 0;}
        service();
        if(usec()-begin>100000u){trace_wait(begin,waited);++lcd_pl_report.timeouts;fail(2);return 0;}
    }
    if(read_reg(ERROR)){trace_wait(begin,waited);fail(read_reg(ERROR)<<8);return 0;}
    trace_wait(begin,waited);
    return 1;
}
static void send(uint16_t value,unsigned dc,unsigned last,unsigned wide){
    write_reg(DATA,value|(dc<<16)|(last<<17)|(wide<<18));
    ++lcd_pl_report.words;
}
static int command(uint8_t cmd,const uint8_t *p,unsigned n){
    if(!reserve(n+1))return 0;
    send(cmd,0,n==0,0);
    for(unsigned i=0;i<n;i++)send(p[i],1,i+1==n,0);
    return 1;
}
void lcd_chunk(unsigned y,unsigned x,const uint16_t *p,unsigned n){
    if(y>=320||x>=240||!n||n>240||x+n>240||!p)return;
    uint64_t begin=usec();
    if(!reserve(n+7))return;
    // Three commands, four packed coordinates, one packed word per RGB565 pixel.
    send(0x2a,0,0,0);send((uint16_t)x,1,0,1);send((uint16_t)(x+n-1),1,1,1);
    send(0x2b,0,0,0);send((uint16_t)y,1,0,1);send((uint16_t)y,1,1,1);
    send(0x2c,0,0,0);
    for(unsigned i=0;i<n;i++)send(p[i],1,i+1==n,1);
    ++lcd_pl_report.accepted_chunks;
    uint32_t elapsed=(uint32_t)(usec()-begin);
    if(elapsed>lcd_pl_report.max_submit_us)lcd_pl_report.max_submit_us=elapsed;
}
void lcd_init(void){
    ready=0;
    if(read_reg(ID)!=LCD_PL_ID){lcd_pl_report.error=1;return;}
    write_reg(CTRL,0);write_reg(ERROR,255);write_reg(HALF,50);
    if(read_reg(HALF)!=50||read_reg(ERROR)){fail(4);return;}
    ready=1;control=1;write_reg(CTRL,control);
    usleep(20000);control=3;write_reg(CTRL,control);usleep(120000);
    if(!command(0x01,0,0)||!wait_idle())return;
    usleep(150000);
    if(!command(0x11,0,0)||!wait_idle())return;
    usleep(120000);
    const uint8_t madctl[]={0},rgb565[]={0x55},porch[]={0x0c,0x0c,0,0x33,0x33};
    const uint8_t gate[]={0x35},vcom[]={0x19},lcm[]={0x2c},vdv_en[]={1};
    const uint8_t vrh[]={0x12},vdv[]={0x20},fr[]={0x0f},power[]={0xa4,0xa1};
    const uint8_t pos[]={0xd0,0x04,0x0d,0x11,0x13,0x2b,0x3f,0x54,0x4c,0x18,0x0d,0x0b,0x1f,0x23};
    const uint8_t neg[]={0xd0,0x04,0x0c,0x11,0x13,0x2c,0x3f,0x44,0x51,0x2f,0x1f,0x1f,0x20,0x23};
    command(0x36,madctl,1);command(0x3a,rgb565,1);command(0xb2,porch,5);
    command(0xb7,gate,1);command(0xbb,vcom,1);command(0xc0,lcm,1);
    command(0xc2,vdv_en,1);command(0xc3,vrh,1);command(0xc4,vdv,1);
    command(0xc6,fr,1);command(0xd0,power,2);command(0xe0,pos,14);
    command(0xe1,neg,14);command(0x21,0,0);
    uint16_t black[32]={0};
    for(unsigned y=0;y<320&&ready;y++)for(unsigned x=0;x<240&&ready;x+=32)
        lcd_chunk(y,x,black,x+32<=240?32:240-x);
    if(!wait_idle()||!command(0x29,0,0)||!wait_idle())return;
    usleep(20000);control=7;write_reg(CTRL,control);
}
