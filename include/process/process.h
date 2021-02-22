#ifndef LENOS_PROC_H
#define LENOS_PROC_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "global.h"
#include "display.h"
#include "keyboard.h"
#include "memory.h"
    void process_proto();
    void process_A();
    void process_B();
    void process_C();
    void delay(int time);
    void task_tty();
    void init_proc();
    //void init_pcb(TASK *task, PROCESS *proc, u32 pid, char *stack, u16 selector_ldt);

#ifdef __cplusplus
}
#endif

#endif