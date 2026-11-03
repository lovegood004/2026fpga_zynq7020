#include "keys.h"
unsigned key_update(Key *k, bool down, uint32_t now) {
    if (k->raw!=down) {k->raw=down; k->edge=now;}
    if (now-k->edge<20) return 0;
    if (!k->armed) {
        if (!down) {k->armed=true; k->stable=false;}
        return 0;
    }
    if (k->stable!=down) {
        k->stable=down;
        if (down) {k->pressed=now; k->long_sent=false;}
        else if (!k->long_sent) return KEY_SHORT;
    }
    if (down && !k->long_sent && now-k->pressed>=1000) {k->long_sent=true; return KEY_LONG;}
    return 0;
}
