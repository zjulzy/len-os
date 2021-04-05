#include "syscall.h"
#include "tty.h"
#include "process.h"
int sys_call_handler(int syc_code, char *buffer, int len, PROCESS *p_proc)
{
    if (syc_code == 0)
    {
        return ticks;
    }
    else if (syc_code == 1)
    {
        while (len)
        {
            tty_table[p_proc->tty].print(*buffer);
            len--;
        }
    }
    else if (syc_code == 2)
    {
        if (int(buffer) == SENDING)
        {
            send_msg(p_proc, len, p_proc->message);
        }
        else
        {
            receive_msg(p_proc, len, p_proc->message);
        }
    }
}
