#include "process.h"
int init_proc(){

}

//通过task初始化一个pcb,传入pcb地址,初始化pcb结构数据
//task确定pcb将要插入的优先级队列
void init_pcb(TASK *task,PROCESS* proc,u32 pid,char* stack,u16 selector_ldt){

    memcpy(proc-> p_name,task->name,16);
    proc->pid = pid;
    //以下为初始化寄存器,LDT SELECTOR 和LDT
    proc->ldt_sel = selector-ldt;

    //初始为内核代码段
    memcpy(&proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
    //改变dpl
    proc->ldts[0].attr1 = DESEC_ATTR_CODE_E | PRIVILEGE_TASK << 5;
    //初始为内核数据段
    memcpy(&proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
    //改变dpl
    proc->ldts[1].attr1 = DESEC_ATTR_DATA_RW | PRIVILEGE_TASK << 5;  
    //初始化保存状态的寄存器
    proc->regs.cs = ((8 * 0) & SELEC_ATTR_RPL_MASK & SELEC_TI_MASK) |
                    SELEC_TI_LOCAL | RPL_TASK;
    proc->regs.ds = ((8 * 1) & SELEC_ATTR_RPL_MASK & SELEC_TI_MASK) |
                    SELEC_TI_LOCAL | RPL_TASK;
    proc->regs.es = ((8 * 1) & SELEC_ATTR_RPL_MASK & SELEC_TI_MASK) |
                    SELEC_TI_LOCAL | RPL_TASK;
    proc->regs.fs = ((8 * 1) & SELEC_ATTR_RPL_MASK & SELEC_TI_MASK) |
                    SELEC_TI_LOCAL | RPL_TASK;
    proc->regs.ss = ((8 * 1) & SELEC_ATTR_RPL_MASK & SELEC_TI_MASK) |
                    SELEC_TI_LOCAL | RPL_TASK;
    proc->regs.gs = (SELECTOR_KERNEL_GS & SELEC_ATTR_RPL_MASK) | RPL_TASK;
    
    //设置进程运行入口
    proc->regs.eip = (u32)task->initial_eip;
    //设置进程使用栈
    proc->regs.esp = (u32)stack;
    //设置eflages,将IF和IOPL置1,在中断返回时开中断
    proc->regs.eflags = 0x1202;
}