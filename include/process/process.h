#ifndef LENOS_PROC_H
#define LENOS_PROC_H
#define STACK_SIZE_PROTO 0x8000
#define STACK_SIZE_TOTAL STACK_SIZE_PROTO

#ifdef __cplusplus
extern "C"
{
#endif
#include "base.h"

    void init_proc();
    //void init_pcb(TASK *task, PROCESS *proc, u32 pid, char *stack, u16 selector_ldt);

#ifdef __cplusplus
}
#endif

#endif