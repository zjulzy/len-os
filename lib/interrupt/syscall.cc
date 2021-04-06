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
            tty_table[p_proc->tty].print(*buffer++);
            len--;
        }
    }
    else if (syc_code == INDEX_SYSCALL_IPC_SEND)
    {
        send_msg((PROCESS *)buffer, len, (MESSAGE *)p_proc);
    }
    else if (syc_code == INDEX_SYSCALL_IPC_RECEIVE)
    {
        receive_msg((PROCESS *)buffer, len, (MESSAGE *)p_proc);
    }
}
