#ifndef KEYS_H
#define KEYS_H
#include <stdbool.h>
#include <stdint.h>
enum { KEY_SHORT=1, KEY_LONG=2 };
typedef struct {bool raw,stable,armed,long_sent; uint32_t edge,pressed;} Key;
/* Call regularly, down=true for active-low hardware inputs. Initially require release. */
unsigned key_update(Key *, bool down, uint32_t now);
#endif
