#include "process.h"
#include "console.h"
// int init_proc()
// {
//     disp_clear();
//     // ======================初始化系统进程==================
//     // 指向系统为初始进程留下的栈空间
//     // 加上栈总长是为了使得该指针指向栈顶(最高地址)
//     char *p_task_stack = task_stack + BASE_TASKS_STACK_SIZE;
//     u16 selector_ldt = SELECTOR_LDT_FIRST;

//     // 用于暂存上一个节点的指针
//     // 临时指针
//     PROCESS *pre_proc;
//     //将任务加入优先级最高的队列
//     TASK *p_task = task_table;
//     PROCESS *temp_proc;

//     for (int i = 0; i < TASK_NUM; i++)
//     {
//         // 获取一个新的PCB
//         // u32 PID = get_pcb(&temp_proc);

//         // if (PID == -1) {
//         //     disp_str("point proc.c start_proc, no enough pcb");
//         //     pause();
//         //     while (1)
//         //         ;
//         // }
//         // 不需要从空pcb栈中获取的空pcb了,直接使用系统预留的pid及对应的pcb初始化
//         u32 PID = p_task[i].pid;
//         temp_proc = &PCB_stack[PID];
//         PCB_stack_status[PID] = 1;

//         // 初始化PCB
//         init_pcb(&p_task[i], temp_proc, PID, p_task_stack, selector_ldt);

//         // 初始化就绪队列
//         if (i == 0)
//         {
//             p_proc_ready_head = temp_proc;
//             pre_proc = temp_proc;
//         }
//         else
//         {
//             temp_proc->pre_pcb = pre_proc;
//             pre_proc->next_pcb = temp_proc;
//             pre_proc = temp_proc;
//         }

//         // 从内核栈中为初始进程分配堆
//         p_task_stack -= p_task[i].stacksize;
//         // 指向GDT中下一个空描述符
//         selector_ldt += 1 << 3;
//         // 修改PID
//         PID++;
//     }
//     pre_proc->next_pcb = &p_proc_ready_tail;
//     p_proc_ready_tail.pre_pcb = pre_proc;
// }

void init_proc()
{
    TASK user_proc_table[NR_USER_PROCESS] = {
        //{process_proto, STACK_SIZE_PROTO, "process_proto", 0},
        {process_A, STACK_SIZE_A, "process_A", 1},
        {process_B, STACK_SIZE_B, "process_B", 2},
        {process_C, STACK_SIZE_C, "process_C", 3}};
    TASK task_table[NR_TASK] = {{task_tty, STACK_SIZE_TTY, "process_tty", 4}};
    PROCESS *p_process = proc_table;
    TASK *p_task = task_table;
    u16 selector_ldt = SELECTOR_LDT_FIRST;
    u32 remain_stack_size = STACK_SIZE_TOTAL;
    int eflags;
    u8 privilege, rpl;
    //分别对用户进程和任务进程分配PCB
    for (int i = 0; i < NR_TASK + NR_USER_PROCESS; i++)
    {
        if (i < NR_TASK)
        {
            p_task = task_table + i;
            privilege = PRIVILEGE_TASK;
            rpl = RPL_TASK;
            eflags = 0x1202;
        }
        else
        {
            p_task = user_proc_table + i - NR_TASK;
            privilege = PRIVILEGE_USER;
            rpl = RPL_USER;
            eflags = 0x202; //无IO权限
        }
        p_process->pid = p_task->pid;
        p_process->ldt_sel = selector_ldt;

        memcpy(&p_process->ldts[0], &gdt[SELECTOT_KERNEL_C >> 3], sizeof(DESCRIPTOR));
        p_process->ldts[0].attr1 = DAC_E | privilege << 5;
        memcpy(&p_process->ldts[1], &gdt[SELECTOR_KERNEL_RW >> 3], sizeof(DESCRIPTOR));
        p_process->ldts[1].attr1 = DAD_RW | privilege << 5;

        p_process->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_process->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_process->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_process->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_process->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
        p_process->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
        p_process->regs.eip = (u32)(p_task->initial_eip);
        p_process->regs.esp = (u32)task_stack + remain_stack_size;
        p_process->regs.eflags = eflags; // IF=1, IOPL=1, bit 2 is always 1.+

        p_process++;
        p_task++;
        selector_ldt += 1 << 3;
        remain_stack_size -= p_task->stacksize;
    }

    p_process = proc_table;
    p_process->next_pcb = p_process;
    p_process->ticks = LAST_QUENE_SLICE;
    process_tail = proc_table;
    process_queen1_head = p_process + 1;
    for (int i = 1; i < NR_TASK + NR_USER_PROCESS; i++)
    {
        p_process++;
        p_process->ticks = FIRST_QUENE_SLICE;
        p_process->next_pcb = i == NR_TASK + NR_USER_PROCESS - 1 ? process_tail : p_process + 1;
    }
    process_queen1_tail = p_process;
    p_proc_ready = process_queen1_head;
    //为进程指定终端
    proc_table[1].tty = 0;
    proc_table[2].tty = 0;
    proc_table[3].tty = 0;
}
//通过task初始化一个pcb,传入pcb地址,初始化pcb结构数据
//task确定pcb将要插入的优先级队列
// void init_pcb(TASK *task, PROCESS *proc, u32 pid, char *stack, u16 selector_ldt)
// {

//     memcpy(proc->p_name, task->name, 16);
//     proc->pid = pid;
//     //以下为初始化寄存器,LDT SELECTOR 和LDT
//     proc->ldt_sel = selector_ldt;

//     //初始为内核代码段
//     memcpy(&proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3], sizeof(DESCRIPTOR));
//     //改变dpl
//     proc->ldts[0].attr1 = DESEC_ATTR_CODE_E | PRIVILEGE_TASK << 5;
//     //初始为内核数据段
//     memcpy(&proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3], sizeof(DESCRIPTOR));
//     //改变dpl
//     proc->ldts[1].attr1 = DESEC_ATTR_DATA_RW | PRIVILEGE_TASK << 5;
//     init_descriptor(&gdt[selector_ldt >> 3],
//                     vir2phys(seg2phys(SELECTOR_KERNEL_DS), proc->ldts),
//                     LDT_SIZE * sizeof(DESCRIPTOR) - 1,
//                     DA_LDT);
//     //初始化保存状态的寄存器
//     proc->regs.cs = ((8 * 0) & SELEC_ATTR_RPL_MASK & SELEC_TI_MASK) |
//                     SELEC_TI_LOCAL | RPL_TASK;
//     proc->regs.ds = ((8 * 1) & SELEC_ATTR_RPL_MASK & SELEC_TI_MASK) |
//                     SELEC_TI_LOCAL | RPL_TASK;
//     proc->regs.es = ((8 * 1) & SELEC_ATTR_RPL_MASK & SELEC_TI_MASK) |
//                     SELEC_TI_LOCAL | RPL_TASK;
//     proc->regs.fs = ((8 * 1) & SELEC_ATTR_RPL_MASK & SELEC_TI_MASK) |
//                     SELEC_TI_LOCAL | RPL_TASK;
//     proc->regs.ss = ((8 * 1) & SELEC_ATTR_RPL_MASK & SELEC_TI_MASK) |
//                     SELEC_TI_LOCAL | RPL_TASK;
//     proc->regs.gs = (SELECTOR_KERNEL_GS & SELEC_ATTR_RPL_MASK) | RPL_TASK;

//     //设置进程运行入口
//     proc->regs.eip = (u32)task->initial_eip;
//     //设置进程使用栈
//     proc->regs.esp = (u32)stack;
//     //设置eflages,将IF和IOPL置1,在中断返回时开中断
//     proc->regs.eflags = 0x1202;
// }

void process_proto()
{
    while (1)
    {

        delay(10);
    }
}

void process_A()
{
    while (1)
    {

        printf("A");
        delay(10);
    }
}
void process_B()
{
    while (1)
    {

        //disp_str("B");
        delay(10);
    }
}
void process_C()
{
    while (1)
    {

        //disp_str("C");
        delay(10);
    }
}

void delay(int time)
{
    for (int i = 0; i < time; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            for (int k = 0; k < 1000; k++)
            {
            }
        }
    }
}