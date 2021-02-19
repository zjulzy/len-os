#ifndef LENOS_SYSCALL_H
#define LENOS_SYSCALL_H
#include "global.h"
#ifdef __cplusplus
extern "C"
{
#endif
    int get_ticks();
    int sys_call_handler(int syc_code);
    void sys_call();
#ifdef __cplusplus
}
#endif
#endif