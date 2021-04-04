#ifndef LENOS_PROC_H
#define LENOS_PROC_H

#ifdef __cplusplus
extern "C"
{
#endif
#include "global.h"
#include "display.h"
#include "keyboard.h"
#include "interrupt.h"
#include "syscall.h"
#include "memory.h"
    void process_proto();
    void process_A();
    void process_B();
    void process_C();
    void delay(int time);
    void task_tty();
    void init_proc();
    void unblock(PROCESS *p);
    void block(PROCESS *P, int value);
    bool is_deadlock(PROCESS *src, PROCESS *dest);
    //void init_pcb(TASK *task, PROCESS *proc, u32 pid, char *stack, u16 selector_ldt);

    // 实现定义在protect.h中的pcb对象的方法-------------------------------------------------
    // 进程收发消息时，通过ipc系统调用
    int PROCESS::send_msg(int dest)
    {
        ipc(dest);
    }
    int PROCESS::receive_msg(int src)
    {
        ipc(src);
    }
    // ----------------------------------------------------------------------------------
    int send_msg(PROCESS *src, int dest, MESSAGE *msg);
    int receive_msg(PROCESS *dest, int src, MESSAGE *msg);
#ifdef __cplusplus
}
#endif

#endif