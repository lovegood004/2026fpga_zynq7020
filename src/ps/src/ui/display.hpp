#pragma once
#include <cstdint>
#include <cstring>
#include <algorithm>
// Driver remains in native 240x320 orientation. Original UI is 320x240 landscape.
struct Display {
    uint16_t logical[320*240]{},shown[320][240]{};
    bool valid[320][8]{};unsigned slot=0,scans=0;
    uint32_t chunks=0,pixels=0;
    bool batch=false;
    unsigned scan_steps()const{return batch?320:320*8;}
    // New views always begin at landscape top-left. In-page updates keep
    // advancing so frequent recording progress cannot starve the right side.
    void begin_view(){slot=0;}
    using Write=void(*)(void*,unsigned,unsigned,const uint16_t*,unsigned);
    void step(Write write,void *opaque){
        if(batch){
            unsigned y=319-slot,first=8,last=0;uint16_t row[240];
            for(unsigned i=0;i<240;i++)row[i]=logical[i*320+slot];
            for(unsigned b=0;b<8;b++){
                unsigned x=b*32,n=std::min(32u,240-x);
                if(!valid[y][b]||std::memcmp(shown[y]+x,row+x,n*2)){
                    first=std::min(first,b);last=b;
                }
            }
            if(first<8){
                unsigned x=first*32,n=std::min(240u,(last+1)*32)-x;
                write(opaque,y,x,row+x,n);std::memcpy(shown[y]+x,row+x,n*2);
                for(unsigned b=first;b<=last;b++)valid[y][b]=true;
                ++chunks;pixels+=n;
            }
            if(++slot==320){slot=0;++scans;}return;
        }
        unsigned y=319-slot/8,x=(slot%8)*32,n=std::min(32u,240-x);uint16_t row[32];
        for(unsigned i=0;i<n;i++)row[i]=logical[(x+i)*320+(319-y)];
        if(!valid[y][slot%8]||std::memcmp(shown[y]+x,row,n*2)){
            write(opaque,y,x,row,n);std::memcpy(shown[y]+x,row,n*2);valid[y][slot%8]=true;
            ++chunks;pixels+=n;
        }
        if(++slot==320*8){slot=0;++scans;}
    }
};
