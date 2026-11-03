#pragma once
#include "acq_preview.h"
#include "raw.hpp"
#include <algorithm>
// Foreground-only side branch: never commits/discards PCM or changes capture.
struct AcqPreviewSession {
    AcqPreviewSnapshot snapshot{};
    uint32_t session=0,position=0,snapshot_at=0;
    bool active=false,failed=false,verified=false;
    void service(const raw::Sink &sink,uint32_t now){
        const bool receiving=sink.phase==raw::Phase::Recording||sink.phase==raw::Phase::Uploading;
        if(sink.phase==raw::Phase::Complete&&sink.timing_complete)verified=true;
        const bool finished=sink.phase==raw::Phase::Complete&&verified;
        if((receiving||finished)&&sink.wanted.session!=session){
            session=sink.wanted.session;position=0;snapshot_at=now;snapshot={};
            verified=sink.phase==raw::Phase::Complete&&sink.timing_complete;
            failed=!acq_preview_begin(session,raw::Samples);active=!failed;
        }
        if(!receiving&&!finished){
            if(active)acq_preview_abort();
            active=false;snapshot.available=0;return;
        }
        if(!active||failed)return;
        if(sink.points<position||sink.points>raw::Samples){fail();return;}
        const unsigned count=std::min<uint32_t>(128u,sink.points-position);
        if(count){
            if(!acq_preview_feed(session,position,sink.staging_data()+position,count)){fail();return;}
            position+=count;
        }
        const bool final=finished&&position==raw::Samples;
        if(final&&!acq_preview_finish(session)){fail();return;}
        if(now-snapshot_at>=250||final){
            snapshot_at=now;
            if(!acq_preview_snapshot(&snapshot)){fail();return;}
        }
        if(final)active=false;
    }
    void fail(){failed=true;active=false;snapshot.available=0;acq_preview_abort();}
};
