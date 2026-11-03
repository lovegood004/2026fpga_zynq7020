#include "raw.hpp"
namespace raw {
uint32_t crc_update(uint32_t c,const uint8_t*p,size_t n){
    while(n--){c^=*p++;for(unsigned k=0;k<8;k++)c=(c>>1)^(0xedb88320u&(0u-(c&1u)));}return c;
}
void pack(uint8_t*p,const Descriptor&d){
    put32(p,d.session);put32(p+4,d.rate);put32(p+8,d.den);put32(p+12,d.samples);
    put32(p+16,d.checksum);put32(p+20,d.elapsed_ms);p[24]=d.site;p[25]=d.bits;
    p[26]=d.channels;p[27]=d.format;p[28]=d.rate_verified;p[29]=d.streaming;p[30]=p[31]=0;
}
bool unpack(const uint8_t*p,size_t n,Descriptor&d){
    if(n!=32||p[29]>1||p[30]||p[31])return false;
    d.session=get32(p);d.rate=get32(p+4);d.den=get32(p+8);d.samples=get32(p+12);
    d.checksum=get32(p+16);d.elapsed_ms=get32(p+20);d.site=p[24];d.bits=p[25];
    d.channels=p[26];d.format=p[27];d.rate_verified=p[28];d.streaming=p[29];return true;
}
bool valid(const Descriptor&d){return d.session&&d.rate==Rate&&d.den==1&&d.samples==Samples&&
    d.site<5&&d.bits==16&&d.channels==1&&d.format==1&&d.rate_verified<=1&&d.streaming<=1;}
size_t encode(uint8_t*out,Op op,const uint8_t*p,size_t n){
    if(!out||n>MaxPayload||(!p&&n))return 0;
    std::memcpy(out,"HSR2",4);out[4]=2;out[5]=uint8_t(op);put16(out+6,uint16_t(n));
    if(n){std::memcpy(out+8,p,n);}
    put32(out+8+n,crc(out,8+n));return n+12;
}
bool Decoder::feed(uint8_t b,uint32_t now,Op&op,const uint8_t*&p,size_t&n){
    if(used&&now-last>1500){used=0;++bad;}last=now;
    if(used<4){if(b==uint8_t("HSR2"[used]))bytes[used++]=b;else used=b=='H'?1:0;return false;}
    bytes[used++]=b;
    if(used==8&&(bytes[4]!=2||get16(bytes+6)>MaxPayload)){used=0;++bad;return false;}
    if(used<8)return false;
    n=get16(bytes+6);if(used<12+n)return false;
    used=0;if(get32(bytes+8+n)!=crc(bytes,8+n)){++bad;return false;}
    ++good;op=Op(bytes[5]);p=bytes+8;return true;
}
void PcmParser::feed(const uint8_t*p,size_t n){
    if(!p||failed||complete)return;
    while(n--&&!failed&&!complete){
        if(used==408){std::memmove(pending,pending+1,407);used=407;}
        pending[used++]=*p++;
        if(used==8&&!std::memcmp(pending,"PCM_LOSF",8)){++lost;failed=true;break;}
        if(used!=408)continue;
        if(!std::memcmp(pending+400,"PCM_LOSF",8)){++lost;failed=true;break;}
        if(std::memcmp(pending+400,"PCM_SYNC",8)){
            if(synchronized)failed=true;
            continue;
        }
        size_t take=std::min(size_t(200),capacity-points);
        std::memcpy(pcm+2*points,pending,2*take);points+=take;++packets;
        used=0;synchronized=true;complete=points==capacity;
    }
}
bool Sink::commit_pending(){
    if(!pending_ready||phase!=Phase::Complete||!valid(staged_meta)||points!=Samples)return false;
    const unsigned site=staged_meta.site;
    std::memcpy(pcm[site],staging,sizeof(staging));meta[site]=staged_meta;
    valid_mask|=uint8_t(1u<<site);++completed;pending_ready=false;return true;
}
bool Sink::commit_pending_external(int16_t *destination,Descriptor &metadata){
    if(!destination||!pending_ready||phase!=Phase::Complete||!valid(staged_meta)||points!=Samples)return false;
    std::memcpy(destination,staging,sizeof(staging));metadata=staged_meta;
    ++completed;pending_ready=false;return true;
}
void Sink::discard_pending(){pending_ready=false;timing_complete=false;}
bool Sink::clear_committed(){
    if(busy()||pending_ready)return false;
    valid_mask=0;for(auto &d:meta)d=Descriptor{};return true;
}
void Sink::ack(Op op,uint32_t off){uint8_t a[9];put32(a,wanted.session);put32(a+4,off);a[8]=uint8_t(op);
    if(!send(opaque,Op::Ack,a,9)){phase=Phase::Error;error=Error::Transport;discard_pending();}}
bool Sink::start(uint32_t id,unsigned site,uint32_t now,bool streaming){
    if(busy()||pending_ready||site>=5||!id||id==peer_session)return false;
    if(!online(now)||!ready){error=Error::Offline;return false;}
    wanted=Descriptor{};wanted.session=id;wanted.site=uint8_t(site);wanted.streaming=uint8_t(streaming);
    uint8_t a[32];pack(a,wanted);
    if(!send(opaque,Op::Start,a,32)){error=Error::Transport;return false;}
    staged_meta=Descriptor{};pending_ready=false;points=recorded=0;checksum=~0u;last=now;
    started_ms=now;upload_ms=complete_ms=first_data_ms=last_data_ms=0;timing_upload=timing_complete=false;
    phase=Phase::Recording;error=Error::None;return true;
}
void Sink::stop(uint32_t now){discard_pending();if(!busy())return;uint8_t a[4];put32(a,wanted.session);
    phase=Phase::Stopping;last=stop_sent_ms=now;
    if(!send(opaque,Op::Stop,a,4)){phase=Phase::Error;error=Error::Transport;}}
void Sink::fail(Error e,uint32_t now){stop(now);phase=Phase::Error;error=e;}
void Sink::frame(Op op,const uint8_t*p,size_t n,uint32_t now){
    if(op==Op::Status&&n==6){peer_at=now;peer_seen=true;ready=p[0]&&p[1];peer_session=get32(p+2);
        if(busy()&&!p[1]){fail(Error::Offline,now);}
        return;}
    if(n<4||get32(p)!=wanted.session)return;
    if(op==Op::Ack&&n==9&&p[8]==uint8_t(Op::Stop)&&phase==Phase::Stopping){phase=Phase::Idle;error=Error::None;return;}
    if(!busy()&&phase!=Phase::Complete)return;
    if(op==Op::Fault){fail(Error::Peer,now);return;}
    if(op==Op::Progress&&n==8&&(phase==Phase::Recording||phase==Phase::Uploading)){uint32_t value=get32(p+4);
        if(value>Samples||value<recorded){fail(Error::Format,now);return;}recorded=value;last=now;return;}
    if(op==Op::Ack&&n==9&&p[8]==uint8_t(Op::Start)&&phase==Phase::Recording){last=now;return;}
    if(op==Op::Describe){Descriptor d;
        if(!unpack(p,n,d)||!valid(d)||d.site!=wanted.site||d.streaming!=wanted.streaming||
           (d.streaming&&(d.checksum||d.elapsed_ms))){fail(Error::Format,now);return;}
        if(phase==Phase::Recording){staged_meta=d;upload_ms=now;timing_upload=true;phase=Phase::Uploading;last=now;ack(op,0);}
        else if(phase==Phase::Uploading&&points==0){uint8_t expected[32];pack(expected,staged_meta);
            if(std::memcmp(expected,p,32))fail(Error::Format,now);else{last=now;ack(op,0);}}
        return;
    }
    if(op==Op::Data&&phase==Phase::Uploading){
        if(n<10){fail(Error::Format,now);return;}uint32_t off=get32(p+4),count=get16(p+8);
        if(!count||count>Chunk||n!=10+2*count||off>Samples||count>Samples-off){fail(Error::Format,now);return;}
        auto *dst=reinterpret_cast<uint8_t*>(staging);
        if(off==points&&count==std::min(Chunk,Samples-points)){
            if(!points)first_data_ms=now;last_data_ms=now;
            std::memcpy(dst+2*off,p+10,count*2);checksum=crc_update(checksum,p+10,count*2);points+=count;
        }else if(off<points&&off%Chunk==0&&off+count==points&&count==std::min(Chunk,Samples-off)&&
                 !std::memcmp(dst+2*off,p+10,count*2)){++duplicates;}
        else{fail(Error::Sequence,now);return;}
        last=now;ack(op,points);return;
    }
    if(op==Op::End&&(phase==Phase::Uploading||phase==Phase::Complete)){
        if(n!=16||points!=Samples||get32(p+4)!=Samples||get32(p+8)!=(checksum^~0u)||
           (!staged_meta.streaming&&(get32(p+8)!=staged_meta.checksum||get32(p+12)!=staged_meta.elapsed_ms))||
           (phase==Phase::Complete&&(get32(p+8)!=staged_meta.checksum||get32(p+12)!=staged_meta.elapsed_ms))){fail(Error::Crc,now);return;}
        if(phase!=Phase::Complete){staged_meta.checksum=get32(p+8);staged_meta.elapsed_ms=get32(p+12);
            pending_ready=true;phase=Phase::Complete;
            complete_ms=now;timing_complete=true;}
        last=now;ack(op,points);
    }
}
void Sink::tick(uint32_t now){
    if(!busy())return;
    if(!online(now)){fail(Error::Offline,now);return;}
    if(phase==Phase::Stopping&&now-stop_sent_ms>=700){
        uint8_t a[4];put32(a,wanted.session);stop_sent_ms=now;
        if(!send(opaque,Op::Stop,a,4)){phase=Phase::Error;error=Error::Transport;return;}
    }
    uint32_t timeout=phase==Phase::Recording?24000:phase==Phase::Stopping?2500:5000;
    if(now-last>timeout)fail(Error::Timeout,now);
}
bool Source::transmit(Op op,const uint8_t*p,size_t n,uint32_t now){
    if(n>sizeof(packet)){return false;}
    std::memcpy(packet,p,n);packet_size=n;waiting_op=op;
    waiting=true;attempts=1;deadline=now+1500;
    if(!send(opaque,op,packet,n)){fail(Error::Transport);return false;}return true;
}
void Source::fail(Error e){
    if(backend.stop){backend.stop(backend.opaque);}
    phase=Phase::Error;waiting=false;
    uint8_t a[5];put32(a,desc.session);a[4]=uint8_t(e);send(opaque,Op::Fault,a,5);
}
void Source::frame(Op op,const uint8_t*p,size_t n,uint32_t now){
    if(op==Op::Hello&&n==0){peer_at=now;uint8_t a[6]={uint8_t(capture.ready&&!capture.stop_pending),uint8_t(capture.ready),0,0,0,0};
        put32(a+2,desc.session);send(opaque,Op::Status,a,6);return;}
    if(op==Op::Start){Descriptor d;
        if(!unpack(p,n,d)||!valid(d))return;
        peer_at=now;
        if(d.session==desc.session){if(phase==Phase::Recording){uint8_t a[9];put32(a,d.session);put32(a+4,0);a[8]=uint8_t(Op::Start);send(opaque,Op::Ack,a,9);}return;}
        if(phase==Phase::Recording||phase==Phase::Uploading||phase==Phase::Stopping)return;
        desc=d;desc.rate_verified=0;desc.checksum=desc.elapsed_ms=0;
        offset=pending_end=0;rolling_crc=~0u;waiting=described=false; // START cannot certify the device clock.
        if(!capture.ready||capture.stop_pending||!backend.start(backend.opaque)){fail(Error::Offline);return;}
        phase=Phase::Recording;began=progress_at=now;deadline=now;uint8_t a[9];put32(a,d.session);put32(a+4,0);a[8]=uint8_t(Op::Start);
        send(opaque,Op::Ack,a,9);return;
    }
    if(n<4||get32(p)!=desc.session)return;
    if(op==Op::Stop&&n==4){
        if(phase==Phase::Stopping)return; // Do not restart device STOP on a retry.
        if(phase==Phase::Idle&&!capture.stop_pending){uint8_t a[9];put32(a,desc.session);put32(a+4,0);a[8]=uint8_t(Op::Stop);
            send(opaque,Op::Ack,a,9);return;}
        backend.stop(backend.opaque);waiting=false;phase=Phase::Stopping;return;
    }
    if(op!=Op::Ack||n!=9||!waiting||p[8]!=uint8_t(waiting_op))return;
    uint32_t expected=waiting_op==Op::Describe?0:waiting_op==Op::Data?pending_end:Samples;
    if(get32(p+4)!=expected)return;
    waiting=false;if(waiting_op==Op::Describe)described=true;
    if(waiting_op==Op::Data){
        rolling_crc=crc_update(rolling_crc,packet+10,(pending_end-offset)*2);offset=pending_end;
    }
    if(waiting_op==Op::End)phase=Phase::Complete;
}
void Source::tick(const CaptureState&s,uint32_t now){
    capture=s;
    if(phase==Phase::Stopping){if(!s.stop_pending){uint8_t a[9];put32(a,desc.session);put32(a+4,0);a[8]=uint8_t(Op::Stop);
        send(opaque,Op::Ack,a,9);phase=Phase::Idle;}return;}
    if(phase!=Phase::Recording&&phase!=Phase::Uploading)return;
    if(s.failed||!s.ready||now-peer_at>4000){fail(Error::Offline);return;}
    if(s.points>Samples||(s.complete&&s.points!=Samples)){fail(Error::Format);return;}
    if(!s.complete&&now-began>24000){fail(Error::Timeout);return;}
    if(now-progress_at>=500){uint8_t a[8];put32(a,desc.session);put32(a+4,s.points);
        if(!send(opaque,Op::Progress,a,8)){fail(Error::Transport);return;}progress_at=now;}
    if(waiting){if(int32_t(now-deadline)>=0){if(attempts>=3){fail(Error::Timeout);return;}
        ++attempts;deadline=now+1500;if(!send(opaque,waiting_op,packet,packet_size))fail(Error::Transport);}return;}
    if(!described){
        if(!desc.streaming){
            if(!s.complete||s.stop_pending)return;
            uint32_t c=~0u;
            for(uint32_t i=0;i<Samples;i+=Chunk){uint32_t count=std::min(Chunk,Samples-i);
                if(!backend.copy(backend.opaque,i,count,packet)){fail(Error::Peer);return;}c=crc_update(c,packet,count*2);}
            desc.checksum=c^~0u;desc.elapsed_ms=s.elapsed_ms;
        }
        phase=Phase::Uploading;uint8_t a[32];pack(a,desc);transmit(Op::Describe,a,32,now);return;
    }
    if(offset<Samples){uint32_t count=std::min(Chunk,Samples-offset);uint8_t a[MaxPayload];
        // Published PCM is immutable for this backend generation; full chunks only.
        // Retain all samples in PSRAM, so temporary link backpressure loses no audio.
        if(s.points<offset+count)return;
        put32(a,desc.session);put32(a+4,offset);put16(a+8,uint16_t(count));
        if(!backend.copy(backend.opaque,offset,count,a+10)){fail(Error::Peer);return;}
        pending_end=offset+count;transmit(Op::Data,a,10+count*2,now);
    }else{
        if(!s.complete||s.stop_pending)return; // Never publish final before device STOP finishes.
        if(desc.streaming){desc.checksum=rolling_crc^~0u;desc.elapsed_ms=s.elapsed_ms;}
        uint8_t a[16];put32(a,desc.session);put32(a+4,Samples);put32(a+8,desc.checksum);put32(a+12,desc.elapsed_ms);
        transmit(Op::End,a,16,now);}
}
}
