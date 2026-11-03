#pragma once
#define Display LegacyDisplay
#include "../../ui/display.hpp"
#undef Display
#include "profile.hpp"
struct Display:LegacyDisplay {
    bool lung_result=false,split=true,trace=false;
    lung_display::Counters counters{};
    struct Span {unsigned x,n;};
    // At most four transactions per scanline. Split only across gaps >=16
    // pixels; each extra command costs eleven wire bytes (5.5 RGB565 pixels).
    // The spare margin also limits MMIO/service overhead, subject to board AB.
    unsigned plan(const uint16_t*row,unsigned y,Span*out,unsigned &changed,unsigned &legacy)const{
        unsigned first=240,last=0,first_block=8,last_block=0;changed=0;
        unsigned gaps[3]{},ends[3]{};
        for(unsigned x=0;x<240;++x){
            const bool dirty=!valid[y][x/32]||shown[y][x]!=row[x];
            if(!dirty)continue;
            ++changed;first_block=std::min(first_block,x/32);last_block=x/32;
            if(first==240)first=x;
            else {
                const unsigned gap=x-last-1;
                if(gap>=16){
                    for(unsigned k=0;k<3;++k)if(gap>gaps[k]){
                        for(unsigned j=2;j>k;--j){gaps[j]=gaps[j-1];ends[j]=ends[j-1];}
                        gaps[k]=gap;ends[k]=x;break;
                    }
                }
            }
            last=x;
        }
        if(first==240){legacy=0;return 0;}
        legacy=std::min(240u,(last_block+1)*32)-first_block*32;
        if(!split){out[0]={first_block*32,legacy};return 1;}
        unsigned count=0;for(unsigned k=0;k<3;++k)if(gaps[k])++count;
        // Sort the selected gaps spatially; submissions retain the old order.
        for(unsigned i=0;i<count;++i)for(unsigned j=i+1;j<count;++j)
            if(ends[j]<ends[i]){std::swap(ends[j],ends[i]);std::swap(gaps[j],gaps[i]);}
        unsigned at=first;
        for(unsigned i=0;i<count;++i){out[i]={at,ends[i]-gaps[i]-at};at=ends[i];}
        out[count]={at,last-at+1};return count+1;
    }
    void step(Write write,void*opaque){
        if(!lung_result||!batch||(!split&&!trace)){LegacyDisplay::step(write,opaque);return;}
        const unsigned y=319-slot;uint16_t row[240];
        for(unsigned x=0;x<240;++x)row[x]=logical[x*320+slot];
        Span spans[4];unsigned changed=0,legacy=0;
        const unsigned count=plan(row,y,spans,changed,legacy);
        if(trace){++counters.rows;counters.changed_pixels+=changed;counters.legacy_pixels+=legacy;
            counters.legacy_wire_bytes+=legacy?2*legacy+11:0;
            counters.max_chunks_per_row=std::max<uint32_t>(counters.max_chunks_per_row,count);}
        for(unsigned i=0;i<count;++i){
            const auto s=spans[i];write(opaque,y,s.x,row+s.x,s.n);
            std::memcpy(shown[y]+s.x,row+s.x,s.n*2);++chunks;pixels+=s.n;
            if(trace){++counters.chunks;counters.sent_pixels+=s.n;counters.wire_bytes+=2*s.n+11;}
        }
        // Invalid blocks were treated as wholly dirty, so every unknown pixel
        // has been submitted; known clean blocks retain their exact contents.
        for(unsigned b=0;b<8;++b)valid[y][b]=true;
        if(++slot==320){slot=0;++scans;}
    }
};
