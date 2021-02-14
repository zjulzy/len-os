// ------------------------------------------------------------------------
// 帮助编译器找到include文件夹中的头文件,即该头文件中的extern声明变量定义在此处
// ------------------------------------------------------------------------

#include "global.h"
//当前屏幕指针位置
int disp_pos = 0;

//kernel中使用的gdt指针
//描述符
// 0~15:Limit  16~47:Base 共48位
//DESCRIPTOR和GATE定义在protect.h中
u8 gdt_ptr[6];
DESCRIPTOR gdt[GDT_SIZE];
// 0~15:Limit  16~47:Base 共48位
u8 idt_ptr[6];
GATE idt[IDT_SIZE];
TSS tss;
PROCESS proc_table[NR_TASK];
PROCESS *p_proc_ready;
char task_stack[STACK_SIZE_TOTAL];
