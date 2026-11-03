#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <algorithm>

// Deliberately incompatible with legacy AA55 / 15034-point transport.
namespace raw {
constexpr uint32_t Rate=11025, Samples=Rate*15, Chunk=512;
// ESP UART1 <-> Zynq PS UART0 only; USB debug consoles keep their own baud.
constexpr uint32_t LinkBaud=460800;
constexpr size_t MaxPayload=10+Chunk*2, MaxFrame=12+MaxPayload;
enum class Op:uint8_t {Hello=1,Status,Start,Stop,Describe,Data,End,Ack,Fault,Progress};
enum class Phase:uint8_t {Idle,Recording,Uploading,Complete,Stopping,Error};
enum class Error:uint8_t {None,Offline,Format,Sequence,Crc,Timeout,Stopped,Transport,Peer};
using Send=bool(*)(void*,Op,const uint8_t*,size_t);
uint32_t crc_update(uint32_t,const uint8_t*,size_t);
inline uint32_t crc(const uint8_t*p,size_t n){return crc_update(~0u,p,n)^~0u;}
inline uint16_t get16(const uint8_t*p){return uint16_t(p[0]|uint16_t(p[1])<<8);}
inline uint32_t get32(const uint8_t*p){return uint32_t(get16(p))|uint32_t(get16(p+2))<<16;}
inline void put16(uint8_t*p,uint16_t n){p[0]=uint8_t(n);p[1]=uint8_t(n>>8);}
inline void put32(uint8_t*p,uint32_t n){put16(p,uint16_t(n));put16(p+2,uint16_t(n>>16));}
struct Descriptor {
    uint32_t session=0,rate=Rate,den=1,samples=Samples,checksum=0,elapsed_ms=0;
    uint8_t site=0,bits=16,channels=1,format=1,rate_verified=0,streaming=0;
};
void pack(uint8_t*,const Descriptor&);
bool unpack(const uint8_t*,size_t,Descriptor&);
bool valid(const Descriptor&);
size_t encode(uint8_t*,Op,const uint8_t*,size_t);
struct Decoder {
    uint8_t bytes[MaxFrame]{};size_t used=0;uint32_t last=0,good=0,bad=0;
    bool feed(uint8_t,uint32_t,Op&,const uint8_t*&,size_t&);
};

// Device framing is 400 PCM bytes followed by PCM_SYNC. Never reduce samples.
struct PcmParser {
    uint8_t pending[408]{};size_t used=0,points=0,packets=0,capacity=0;
    uint8_t *pcm=nullptr;unsigned lost=0;
    bool synchronized=false,failed=false,complete=false;
    void reset(uint8_t*p,size_t n){*this=PcmParser{};pcm=p;capacity=n;if(!p||!n)failed=true;}
    void feed(const uint8_t*,size_t);
};

struct Sink {
    Send send=nullptr;void *opaque=nullptr;Phase phase=Phase::Idle;Error error=Error::None;
    Descriptor wanted{},meta[5]{};int16_t pcm[5][Samples]{};
    uint32_t points=0,recorded=0,peer_session=0,checksum=~0u,last=0,peer_at=0,completed=0,duplicates=0;
    uint8_t valid_mask=0;bool peer_seen=false,ready=false;
    // Local monotonic times, not transmitted metadata. Completion is CRC-gated.
    uint32_t started_ms=0,upload_ms=0,complete_ms=0;
    uint32_t first_data_ms=0,last_data_ms=0;
    bool timing_upload=false,timing_complete=false;
    // Only one in-flight recording. Committed slots remain intact until analysis succeeds.
    int16_t staging[Samples]{};Descriptor staged_meta{};bool pending_ready=false;uint32_t stop_sent_ms=0;
    const int16_t* pending_data()const{return pending_ready?staging:nullptr;}
    const Descriptor& pending_descriptor()const{return staged_meta;}
    const int16_t* staging_data()const{return staging;}
    bool commit_pending();
    // Publish to a separate modality without changing the five heart slots.
    bool commit_pending_external(int16_t *destination,Descriptor &metadata);
    void discard_pending();
    bool clear_committed();
    bool online(uint32_t now)const{return peer_seen&&now-peer_at<4000;}
    bool busy()const{return phase==Phase::Recording||phase==Phase::Uploading||phase==Phase::Stopping;}
    bool start(uint32_t session,unsigned site,uint32_t now,bool streaming=true);
    void stop(uint32_t now);
    void fail(Error,uint32_t now);
    void frame(Op,const uint8_t*,size_t,uint32_t);
    void tick(uint32_t);
    void ack(Op,uint32_t);
};

struct CaptureState {bool ready=false,recording=false,complete=false,failed=false,stop_pending=false;
    uint32_t points=0,elapsed_ms=0;};
struct Backend {
    void *opaque=nullptr;
    bool(*start)(void*)=nullptr;void(*stop)(void*)=nullptr;
    bool(*copy)(void*,uint32_t,uint32_t,uint8_t*)=nullptr;
};
struct Source {
    Backend backend{};Send send=nullptr;void*opaque=nullptr;CaptureState capture{};
    Descriptor desc{};Phase phase=Phase::Idle;Op waiting_op=Op::Hello;
    uint8_t packet[MaxPayload]{};size_t packet_size=0;
    uint32_t offset=0,pending_end=0,deadline=0,began=0,peer_at=0,progress_at=0,rolling_crc=~0u;
    unsigned attempts=0;bool waiting=false,described=false;
    void frame(Op,const uint8_t*,size_t,uint32_t);
    void tick(const CaptureState&,uint32_t);
    void fail(Error);
    bool transmit(Op,const uint8_t*,size_t,uint32_t);
};
}
