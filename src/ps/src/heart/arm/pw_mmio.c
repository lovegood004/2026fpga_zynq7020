#include "pw_mmio.h"
int pw_submit(PwDevice*d,uint32_t w,uint32_t x,uint32_t y,int ci,int co,int len,uint64_t timeout){
 return pw_submit_timed(d,w,x,y,ci,co,len,timeout,0);
}
int pw_submit_timed(PwDevice*d,uint32_t w,uint32_t x,uint32_t y,int ci,int co,int len,uint64_t timeout,PwSubmitTiming*t){
 uint64_t began=pw_hal_ticks();if(t){t->setup_ticks=0;t->wait_ticks=0;}
 if(!d||!d->base||!w||!x||!y||((w|x|y)&3)||ci<1||ci>96||co<1||co>96||len<1||len>2048||!timeout)return PW_BAD_ARGUMENT;
 if(d->fault)return PW_FAULT;
 if(!(pw_hal_read(d->base+PW_CTRL)&4))return PW_BUSY;
 pw_hal_write(d->base+PW_W,w);pw_hal_write(d->base+PW_X,x);pw_hal_write(d->base+PW_Y,y);
 pw_hal_write(d->base+PW_CI,(uint32_t)ci);pw_hal_write(d->base+PW_CO,(uint32_t)co);pw_hal_write(d->base+PW_LEN,(uint32_t)len);
 pw_hal_barrier();uint64_t start=pw_hal_ticks();pw_hal_write(d->base+PW_CTRL,1);
 if(t)t->setup_ticks=start-began;
 for(;;){
  uint32_t status=pw_hal_read(d->base+PW_CTRL);
  if(status&2){if(t)t->wait_ticks=pw_hal_ticks()-start;pw_hal_barrier();int result=(int32_t)pw_hal_read(d->base+PW_RETURN);if(result){d->fault=1;return PW_BAD_RESULT;}return PW_OK;}
  uint64_t elapsed=pw_hal_ticks()-start;
  if(elapsed>=timeout){if(t)t->wait_ticks=elapsed;d->fault=1;return PW_TIMEOUT;}
 }
}
