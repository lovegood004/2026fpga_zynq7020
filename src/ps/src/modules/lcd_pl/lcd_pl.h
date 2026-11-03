#ifndef LCD_PL_H
#define LCD_PL_H
#include <stdint.h>
enum { LCD_PL_BASE=0x43c20000u, LCD_PL_ID=0x4c434431u };
typedef struct {
    uint32_t magic,version,error,words,accepted_chunks,timeouts,max_submit_us;
} LcdPlReport;
extern volatile LcdPlReport lcd_pl_report;
void lcd_pl_service(void (*service)(unsigned));
int lcd_pl_idle(void);
int lcd_pl_ready(void);
#endif
