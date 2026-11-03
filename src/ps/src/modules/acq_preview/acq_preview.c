#include "acq_preview.h"
#ifdef ACQ_PREVIEW_TEST
#include "acq_preview_test_io.h"
#else
#include "xil_io.h"
#include "xtime_l.h"
static uint32_t acq_read(unsigned r){return Xil_In32(ACQ_PREVIEW_BASE+r);}
static void acq_write(unsigned r,uint32_t v){Xil_Out32(ACQ_PREVIEW_BASE+r,v);}
static uint32_t acq_micros(void){XTime t;XTime_GetTime(&t);return (uint32_t)(t/COUNTS_PER_SECOND*1000000u+(t%COUNTS_PER_SECOND)*1000000u/COUNTS_PER_SECOND);}
#endif
volatile AcqPreviewReport acq_preview_report={.magic=0x41505231,.version=1};
static uint32_t identity=0,position=0;
static int live=0;
static int failure(uint32_t error){++acq_preview_report.errors;acq_preview_report.last_error=error;return 0;}
int acq_preview_probe(void){return acq_read(0)==ACQ_PREVIEW_ID&&acq_read(0x4c)==0x00fa01b9;}
void acq_preview_abort(void){
 if(live){acq_write(4,2);++acq_preview_report.aborted;}
 live=0;identity=0;position=0;
}
int acq_preview_begin(uint32_t session,uint32_t samples){
 if(!session||!samples||samples>165375)return failure(0x100);
 acq_preview_abort();
 if(!acq_preview_probe())return failure(0x101);
 acq_write(8,session);acq_write(12,samples);acq_write(4,1);
 if(acq_read(8)!=session||acq_read(4)!=1||acq_read(0x18)!=0){acq_write(4,2);return failure(0x102);}
 identity=session;position=0;live=1;++acq_preview_report.sessions;return 1;
}
int acq_preview_feed(uint32_t session,uint32_t offset,const int16_t *pcm,unsigned count){
 // MMIO belongs only to the foreground producer; never call from an IRQ.
 if(!live||session!=identity||offset!=position||!pcm||!count||count>256)return failure(0x103);
 if(acq_read(4)!=1||acq_read(0x18)!=position||count>acq_read(12)-position)return failure(0x104);
 uint32_t began=acq_micros();
 acq_write(0x10,position);acq_write(0x14,count);
 for(unsigned i=0;i<count;i++)acq_write(0x50,(uint16_t)pcm[i]);
 uint32_t elapsed=acq_micros()-began;
 ++acq_preview_report.feed_calls;acq_preview_report.fed_samples+=count;
 acq_preview_report.feed_us+=elapsed;if(elapsed>acq_preview_report.max_feed_us)acq_preview_report.max_feed_us=elapsed;
 if(acq_read(0x1c)||acq_read(0x18)!=position+count||acq_read(0x48))return failure(0x105);
 position+=count;return 1;
}
int acq_preview_snapshot(AcqPreviewSnapshot *o){
 if(!o||!identity)return 0;
 uint32_t began=acq_micros(),status=acq_read(4);
 if(acq_read(8)!=identity||(status&4))return failure(0x106);
 // No producer runs during these reads: single clock, no DMA or ISR feed.
 o->available=0;o->session=identity;o->samples=acq_read(0x18);o->errors=acq_read(0x1c);
 o->abs_sum=(uint64_t)acq_read(0x20)|((uint64_t)acq_read(0x24)<<32);
 o->square_sum=(uint64_t)acq_read(0x28)|((uint64_t)acq_read(0x2c)<<32);
 o->clips=acq_read(0x30);uint32_t range=acq_read(0x34);
 o->minimum=(int16_t)range;o->maximum=(int16_t)(range>>16);o->peak=acq_read(0x38);
 o->bins_completed=acq_read(0x3c);uint32_t partial=acq_read(0x40),cursor=acq_read(0x44);
 o->partial_count=cursor&511;o->complete=(status&2)!=0;
 if(o->samples>165375||o->partial_count>=441||o->bins_completed!=o->samples/441||
    o->partial_count!=o->samples%441||((cursor>>9)&255)!=o->bins_completed%250)return failure(0x10a);
 unsigned full=o->bins_completed<250?o->bins_completed:250;
 // Reserve one column for the live partial bin once the ring is full.
 if(o->partial_count&&full==250)--full;
 uint32_t first=o->bins_completed-full;
 for(unsigned i=0;i<full;i++){
  uint32_t v=acq_read(0x400+4*((first+i)%250));o->low[i]=(int16_t)v;o->high[i]=(int16_t)(v>>16);
 }
 o->columns=full;o->first_sample=first*441;
 if(o->partial_count){o->low[full]=(int16_t)partial;o->high[full]=(int16_t)(partial>>16);++o->columns;}
 if(acq_read(0x18)!=o->samples||acq_read(8)!=identity)return failure(0x107);
 o->available=o->samples!=0;
 uint32_t elapsed=acq_micros()-began;++acq_preview_report.snapshots;acq_preview_report.snapshot_us+=elapsed;
 if(elapsed>acq_preview_report.max_snapshot_us)acq_preview_report.max_snapshot_us=elapsed;
 return 1;
}
int acq_preview_finish(uint32_t session){
 if(!live||session!=identity||position!=acq_read(12)||acq_read(0x1c))return failure(0x108);
 acq_write(4,4);if(acq_read(4)!=2)return failure(0x109);live=0;return 1;
}
