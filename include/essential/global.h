#ifndef LENOS_GLOBAL_H
#define LENOS_GLOBAL_H
#ifdef __cplusplus
extern "C"
{
#endif
//定义全局变量
#include "process.h"
#include "protect.h"

    extern int disp_pos;
    //kernel中使用的gdt指针
    //描述符
    // 0~15:Limit  16~47:Base 共48位
    //DESCRIPTOR和GATE定义在protect.h中
    extern u8 gdt_ptr[6];
    extern DESCRIPTOR gdt[GDT_SIZE];
    // 0~15:Limit  16~47:Base 共48位
    extern u8 idt_ptr[6];
    extern GATE idt[IDT_SIZE];

    //tss定义
    extern TSS tss;
    extern int int_reenter;
    extern PROCESS proc_table[];
    extern char task_stack[];
    extern PROCESS *p_proc_ready;
    extern TASK task_table[];
    // //使用多级循环队列来实现进程调度
    // extern PROCESS *process_queen1_head;
    // extern PROCESS process_queen1_tail;
    // extern PROCESS *process_queen2_head;
    // extern PROCESS process_queen2_tail;
    // extern PROCESS *process_queen3_head;
    // extern PROCESS process_queen3_tail;
    // //阻塞状态队列
    // extern PROCESS *process_blocked_head;
    // extern PROCESS process_blocked_tail;

#ifdef __cplusplus
}
#endif
#endif
