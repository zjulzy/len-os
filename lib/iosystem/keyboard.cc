#include "keyboard.h"
void keyboard_input()
{
    u8 scan_code;
    if (kb_buffer.count > 0)
    {
        __asm__ __volatile__("cli");

        scan_code = *kb_buffer.p_tail;
        kb_buffer.p_tail++;
        if (kb_buffer.p_tail == kb_buffer.buffer + KB_IN_BYTES)
        {
            kb_buffer.p_tail = kb_buffer.buffer;
        }
        kb_buffer.count--;
        __asm__ __volatile__("sti");
    }
}