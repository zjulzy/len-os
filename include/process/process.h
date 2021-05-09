#ifndef LENOS_PROC_H
#define LENOS_PROC_H

#ifdef __cplusplus
extern "C" {
#endif
#include "display.h"
#include "global.h"
#include "keyboard.h"
#include "protect.h"
void process_proto();
void process_A();
void process_B();
void process_C();
void empty_process();
void task_system();
void delay(int time);
void task_tty();
void init_proc();
void unblock(PROCESS *p);
void block(PROCESS *P, int value);
bool is_deadlock(PROCESS *src, PROCESS *dest);
// void init_pcb(TASK *task, PROCESS *proc, u32 pid, char *stack, u16
// selector_ldt);

int send_msg(PROCESS *src, int dest, MESSAGE *msg);
int receive_msg(PROCESS *dest, int src, MESSAGE *msg);
void panic(const char *problem);
void *vir2line(PROCESS *p, void *va);

// fork.cc
int fork();
#ifdef __cplusplus
}
#endif

#endif