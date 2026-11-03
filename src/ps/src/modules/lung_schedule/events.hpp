#pragma once
#include <cstdint>
namespace lung_schedule {
enum Status : uint32_t {Idle,Computing,ResultReady,Scanning,Complete,Failed,Superseded};
enum Flag : uint32_t {LastPcm=1,CrcReady=2,Entered=4,Preprocessed=8,Inferred=16,Painted=32,Submitted=64,Displayed=128};
struct Report {
 uint32_t magic=0x4c545332,version=1,sequence=0,session=0,flags=0,status=Idle;
 uint32_t callbacks=0,services=0,keys=0,steps=0,paints=0,max_gap_us=0;
 uint64_t last_pcm_us=0,crc_us=0,entered_us=0,preprocessed_us=0,inferred_us=0,painted_us=0,submitted_us=0,displayed_us=0;
 uint32_t sequence_end=0,repaints=0,error=0,reserved=0;
};
static_assert(sizeof(Report)==128,"Lung timing ABI");
struct Budget {
 uint64_t service_at=0,key_at=0,paint_at=0,scan_at=0,publish_at=0;
 void reset(uint64_t now){service_at=key_at=paint_at=scan_at=publish_at=now;}
 static bool due(uint64_t now,uint64_t &last,uint64_t interval){if(now-last<interval)return false;last=now;return true;}
};
struct Timeline {
 Report data{};unsigned left=0;
 bool scanning()const{return data.status==Scanning;}
 void begin(uint32_t session,uint64_t now){data=Report{};data.session=session;data.entered_us=now;data.flags=Entered;data.status=Computing;left=0;}
 void phase(unsigned phase,uint64_t now){if(data.status!=Computing)return;if(phase==1){data.preprocessed_us=now;data.flags|=Preprocessed;}if(phase==2){data.inferred_us=now;data.flags|=Inferred;}}
 void finish(bool okay){if(data.status!=Computing)return;data.status=okay?ResultReady:Failed;}
 void repaint(uint64_t now,unsigned steps){if(data.status!=ResultReady&&!scanning())return;data.status=Scanning;data.painted_us=now;data.flags|=Painted;data.flags&=~Displayed;data.displayed_us=0;left=steps;++data.repaints;}
 void submit(uint64_t now){if(scanning()&&!(data.flags&Submitted)){data.submitted_us=now;data.flags|=Submitted;}}
 void step(){if(scanning()&&left)--left;}
 void complete(uint64_t now,bool idle){if(scanning()&&!left&&idle){data.displayed_us=now;data.flags|=Displayed;data.status=Complete;}}
 void invalidate(){if(data.status==ResultReady||scanning()){data.status=Superseded;left=0;}}
};
// Extend a transport's existing millisecond timestamp into the current clock
// epoch. Input endpoints keep their 1ms resolution; no fictitious precision.
inline uint64_t extend_ms(uint32_t stamp,uint64_t now){const uint32_t age=uint32_t(now/1000)-stamp;const uint64_t delta=uint64_t(age)*1000+now%1000;return delta<=now?now-delta:0;}
}
extern "C" void lung_set_event_hook(void(*)(unsigned));
extern "C" void lung_phase_complete(unsigned);
