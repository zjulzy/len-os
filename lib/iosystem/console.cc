#include "console.h"
void set_cursor(unsigned int position)
{
    __asm__ __volatile__("cli");
    out_byte(CRTC_ADDR_REG, CURSOR_H);
    out_byte(CRTC_DATA_REG, ((position / 2) >> 8) & 0xFF);
    out_byte(CRTC_ADDR_REG, CURSOR_L);
    out_byte(CRTC_DATA_REG, (position / 2) & 0xFF);
    __asm__ __volatile__("sti");
}