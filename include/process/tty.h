#ifndef LENOS_TTY_H
#define LENOS_TTY_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "global.h"
#include "display.h"
#include "keyboard.h"
#include "interrupt.h"
#include "console.h"
#include "memory.h"

    class TTY
    {
    public:
        // 控制台输出
        S_TTY tty_buffer;
        S_CONSOLE console_buffer;
        void init()
        {
            tty_buffer.in_buffer_count = 0;
            tty_buffer.in_buffer_head = tty_buffer.in_buffer_tail = tty_buffer.in_buffer;
        }
        void tty_do_read()
        {

            keyboard_read(&tty_buffer);
        }
        void tty_do_write()
        {
            if (tty_buffer.in_buffer_count)
            {
                char out = *(tty_buffer.in_buffer_tail);
                tty_buffer.in_buffer_tail = (tty_buffer.in_buffer_tail - tty_buffer.in_buffer + 1) % TTY_IN_BYTES + tty_buffer.in_buffer;
                tty_buffer.in_buffer_count--;
                u8 *p_vmem = (u8 *)(V_MEM_BASE + disp_pos);
                *p_vmem++ = out;
                *p_vmem++ = 0x07; //设置黑底白字
                disp_pos += 2;
                set_cursor(disp_pos);
            }
        }
    };

    bool is_current_console(TTY *tty);
    void task_tty();
    TTY tty_table[NR_CONSOLE];

#ifdef __cplusplus
}
#endif

#endif