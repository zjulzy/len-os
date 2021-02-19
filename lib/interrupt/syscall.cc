#include "syscall.h"
int sys_call_handler(int syc_code)
{
    if (syc_code == 0)
    {
        return ticks;
    }
}