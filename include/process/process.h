#ifndef LENOS_PROC_H
#define LENOS_PROC_H


#include"base.h"

int init_proc();
void init_pcb(TASK *task,PROCESS* proc,u32 pid,char* stack,u16 selector_ldt);


#endif