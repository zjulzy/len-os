//定义结构体
#ifndef LENOS_PROTECT_H
#define LENOS_PROTECT_H

//layer==-2
#include "type.h"
#include "const.h"

//存储段描述符/系统段描述符
typedef struct s_descriptor
{
    u16 limit_low;       // Limit
    u16 base_low;        // Base
    u8 base_mid;         // Base
    u8 attr1;            // P(1) DPL(2) DT(1) TYPE(4)
    u8 limit_high_attr2; // G(1) D(1) 0(1) AVL(1) LimitHigh(4)
    u8 base_high;        // Base
} DESCRIPTOR;

//门描述符(/调用门中断门/陷阱门/任务门)
typedef struct s_gate
{
    u16 offset_low;  // Offset Low
    u16 selector;    // Selector
    u8 dcount;       //调用门中调用子程序时,如果引起特权级的改变,切换堆栈时需要复制的双字参数数量
    u8 attr;         // P(1) DPL(2) DT(1) TYPE(4)
    u16 offset_high; // Offset High
} GATE;

// 进程控制块PCB中存放的关于程序运行状态的信息
typedef struct s_stackframe
{
    //==========================================
    // 在进程切换的过程中被 save() 保存的进程状态
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 kernel_esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    //=============================================
    // 在发生中断的时候esp会指向这里
    // save函数的返回地址
    u32 retaddr;
    //============================
    // 在中断发生的时候会被CPU自动保存的寄存器
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;
    //============================
} STACK_FRAME;

// 进程控制块PCB组成单元
// 存放了程序运行状态信息,ldt选择子,进程调度信息,进程id,进程名字
typedef struct s_proc
{
    STACK_FRAME regs;

    //虽然下面接着就是局部描述符表,但是这个ldt_sel依然有存在的意义
    //其指出了ldt的界限,同时简化了加载ldt的步骤
    u16 ldt_sel;
    DESCRIPTOR ldts[LDT_SIZE];

    int ticks;
    int priority;

    u32 pid;
    char p_name[16];

    // 进程调度队列内部指针,暂定为双向
    struct s_proc *pre_pcb;
    struct s_proc *next_pcb;

    // 进程间通信需要用到的数据
    // 进程状态标志
    // 0 --> 正常执行
    // 2 --> 发送信息阻塞中
    // 4 --> 接收信息阻塞中
    int flags;

    // 本进程准备发送的信息或等待接收的信息
    // 尝试发送信息的对象
    // 尝试接收信息的源
    struct mess *message;
    int recv_from;
    int send_to;

    // 0 --> ready to handle an interupt
    // 1 --> handling an interupt
    // 操作系统通过把这一位从0设为1来通知中断的发生
    int has_int_msg;

    // 希望发信息给该进程的进程的链表
    struct s_proc *sending_to_this;
    // 用于产生链表结构的指针
    struct s_proc *next_sending;

} PROCESS;

// 这个结构体用来定义系统初始进程
// 这个设计最早来自minix
typedef struct s_task
{
    //task_f initial_eip;
    int stacksize;
    char name[16];
    int priority;
    u32 pid;
} TASK;

//tss结构体
typedef struct s_tss
{
    u32 backlink;
    u32 esp0; //内核段栈指针
    u32 ss0;  //内核段栈基址
    u32 esp1;
    u32 ss1;
    u32 esp2;
    u32 ss2;
    u32 cr3;
    u32 eip;
    u32 flags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    u32 ldt;
    u16 trap;
    u16 iobase; // I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图
} TSS;

#endif