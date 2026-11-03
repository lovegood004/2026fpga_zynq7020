#ifndef LCD_H
#define LCD_H
#include <stdint.h>
void lcd_init(void);
void lcd_chunk(unsigned y, unsigned x, const uint16_t *, unsigned count);
#endif
