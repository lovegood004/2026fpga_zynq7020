#pragma once
#include <cstdint>
/* Single producer ISR, single consumer main. No overwriting unread data. */
struct RxRing {
    static constexpr unsigned N=65536;
    uint8_t data[N];volatile uint32_t write=0,read=0,dropped=0;
    void push(uint8_t b){unsigned w=write,next=(w+1)&(N-1);if(next==read){++dropped;return;}
        data[w]=b;barrier();write=next;}
    bool pop(uint8_t &b){unsigned r=read;if(r==write)return false;barrier();b=data[r];barrier();read=(r+1)&(N-1);return true;}
    static void barrier(){
#if defined(__arm__)
        asm volatile("dmb sy":::"memory");
#else
        asm volatile("":::"memory");
#endif
    }
};
