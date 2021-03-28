#include "tty.h"
#include "keyboard.h"
TTY tty_table[NR_CONSOLE];
// tty类方法===================================================================
void TTY::init(int index)
{

    id = index;
    tty_buffer.in_buffer_count = 0;
    tty_buffer.in_buffer_head = tty_buffer.in_buffer_tail = tty_buffer.in_buffer;
    //初始化console
    int v_size = V_MEM_SIZE;
    int con_v_size = v_size / NR_CONSOLE;
    console_buffer.current_addr = console_buffer.v_mem_addr = index * con_v_size;
    console_buffer.v_mem_limit = con_v_size;
    console_buffer.cursor = index ? console_buffer.current_addr : disp_pos;
    print('0' + index);
    print('>');
}
void TTY::tty_do_read()
{

    keyboard_read(&tty_buffer);
}
void TTY::tty_do_write()
{
    if (tty_buffer.in_buffer_count)
    {
        char out = *(tty_buffer.in_buffer_tail);
        tty_buffer.in_buffer_tail = (tty_buffer.in_buffer_tail - tty_buffer.in_buffer + 1) % TTY_IN_BYTES + tty_buffer.in_buffer;
        tty_buffer.in_buffer_count--;
        print(out);
    }
}
void TTY::print(char out)
{
    u8 *p_vmem = (u8 *)(V_MEM_BASE + console_buffer.cursor);
    *p_vmem++ = out;
    *p_vmem++ = 0x07; //设置黑底白字
    console_buffer.cursor += 2;
    set_cursor(console_buffer.cursor);
}
// ===============================================================================
bool is_current_console(TTY *tty)
{
    return tty - tty_table == current_console;
}
void task_tty()
{
    current_console = 0;
    for (TTY *tty = tty_table; tty < tty_table + NR_CONSOLE; tty++)
    {
        tty->init(tty - tty_table);
    }
    select_console(current_console, &(tty_table[current_console].console_buffer));
    while (1)
    {
        for (TTY *tty = tty_table; tty < tty_table + NR_CONSOLE; tty++)
        {
            if (is_current_console(tty))
            {
                tty->tty_do_read();
                tty->tty_do_write();
            }
        }
    }
}
