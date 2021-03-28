#include "console.h"
//通过开关中断操作VGA寄存器的值改变光标位置和当前控制台在显存中的位置================================
void set_cursor(unsigned int position)
{
    __asm__ __volatile__("cli");
    out_byte(CRTC_ADDR_REG, CURSOR_H);
    out_byte(CRTC_DATA_REG, ((position / 2) >> 8) & 0xFF);
    out_byte(CRTC_ADDR_REG, CURSOR_L);
    out_byte(CRTC_DATA_REG, (position / 2) & 0xFF);
    __asm__ __volatile__("sti");
}
void set_video_addr(u32 addr)
{
    __asm__ __volatile__("cli");
    out_byte(CRTC_ADDR_REG, START_ADDR_H);
    out_byte(CRTC_DATA_REG, ((addr / 2) >> 8) & 0xFF);
    out_byte(CRTC_ADDR_REG, START_ADDR_L);
    out_byte(CRTC_DATA_REG, (addr / 2) & 0xFF);
    __asm__ __volatile__("sti");
}
//======================================================================
void select_console(int console_index, S_CONSOLE *console)
{
    console_index = console_index % NR_CONSOLE;
    current_console = console_index;
    set_cursor(console->cursor);
    set_video_addr(console->current_addr);
}
void scroll_screen(S_CONSOLE *console, bool up)
{
    if (up)
    {
        if (console->current_addr > console->v_mem_addr)
        {
            console->current_addr -= SCREEN_WIDTH;
            console->cursor -= SCREEN_WIDTH;
        }
    }
    else
    {
        if (console->current_addr + SCREEN_SIZE < (console->v_mem_addr + console->v_mem_limit))
        {
            console->current_addr += SCREEN_WIDTH;
            console->cursor += SCREEN_WIDTH;
        }
    }
    set_video_addr(console->current_addr);
    set_cursor(console->cursor);
}