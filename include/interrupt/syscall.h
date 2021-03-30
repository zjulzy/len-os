#ifndef LENOS_SYSCALL_H
#define LENOS_SYSCALL_H
#include "global.h"
#ifdef __cplusplus
extern "C"
{
#endif
    int get_ticks();
    int sys_call_handler(int syc_code, char *buffer, int len, PROCESS *p_proc);
    void sys_call();
    void write(char *buffer, int length);
#ifdef __cplusplus
}
#endif
#endif