#include "tty.h"

bool is_current_console(TTY *tty)
{
    return tty - tty_table == current_console;
}
void task_tty()
{
    current_console = 0;
    for (TTY *tty = tty_table; tty < tty_table + NR_CONSOLE; tty++)
    {
        tty->init();
    }
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
