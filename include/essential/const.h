//宏定义
#ifndef LENOS_CONST_H
#define LENOS_CONST_H

//idt相关宏定义=========================================
//系统调用中断向量
#define INT_VECTOR_SYS_CALL 0X90
//异常中断向量
#define INT_VECTOR_DIVIDE 0x0
#define INT_VECTOR_DEBUG 0x1
#define INT_VECTOR_NMI 0x2
#define INT_VECTOR_BREAKPOINT 0x3
#define INT_VECTOR_OVERFLOW 0x4
#define INT_VECTOR_BOUNDS 0x5
#define INT_VECTOR_INVAL_OP 0x6
#define INT_VECTOR_COPROC_NOT 0x7
#define INT_VECTOR_DOUBLE_FAULT 0x8
#define INT_VECTOR_COPROC_SEG 0x9
#define INT_VECTOR_INVAL_TSS 0xA
#define INT_VECTOR_SEG_NOT 0xB
#define INT_VECTOR_STACK_FAULT 0xC
#define INT_VECTOR_PROTECTION 0xD
#define INT_VECTOR_PAGE_FAULT 0xE
#define INT_VECTOR_COPROC_ERR 0x10
//8259A芯片端口定义
#define INT_M_CTL 0x20
#define INT_M_CTLMASK 0x21
#define INT_S_CTL 0xA0
#define INT_S_CTLMASK 0xA1
//8259A中断入口
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28
//8253相关定义
#define TIMER0 0x40
#define TIMER_MODE 0x43
#define RATE_GENERATOR 0x34
#define TIMER_FREQ 1193182L
#define HZ 100
//权限
#define PRIVILEGE_KRNL 0
#define PRIVILEGE_TASK 1
#define PRIVILEGE_USER 3
//RPL
#define RPL_KRNL SA_RPL0
#define RPL_TASK SA_RPL1
#define RPL_USER SA_RPL3
//===================================================
//定义函数类型
#define PUBLIC
#define PRIVATE static

// GDT 和 IDT 中描述符的个数
#define GDT_SIZE 128
#define IDT_SIZE 256
//每个进程允许使用的ldt数目
#define LDT_SIZE 2

#define NR_TASK 3
//===========================================================
//GDT相关
//描述符索引,用于在c文件中确定选择子对应的描述符数组下标
//部分由loader确定
#define INDEX_DUMMY 0
#define INDEX_FLAT_C 1
#define INDEX_FLAT_RW 2
#define INDEX_VIDEO 3
#define INDEX_TSS 4
#define INDEX_LDT_FIRST 5
//选择子
#define SELECTOR_DUMMY 0
#define SELECTOR_FLAT_C 0x08
#define SELECTOR_FLAT_RW 0x10
#define SELECTOR_VIDEO (0x18 + 3)
#define SELECTOR_TSS 0x20
#define SELECTOR_LDT_FIRST 0x28

#define SELECTOT_KERNEL_C SELECTOR_FLAT_C
#define SELECTOR_KERNEL_RW SELECTOR_FLAT_RW
#define SELECTOR_KERNEL_GS SELECTOR_VIDEO
//===========================================================
//线性地址转为物理地址
#define vir2phy(seg_base, vir) (u32)(((u32)seg_base) + (u32)vir)

// 系统段描述符类型值说明
#define DA_LDT 0x82      /* 局部描述符表段类型值			*/
#define DA_TaskGate 0x85 /* 任务门类型值   */
#define DA_386TSS 0x89   /* 可用 386 任务状态段类型值		*/
#define DA_386CGate 0x8C /* 386 调用门类型值			*/
#define DA_386IGate 0x8E /* 386 中断门类型值			*/
#define DA_386TGate 0x8F /* 386 陷阱门类型值			*/

// 存储段描述符类型值说明
#define DAD_R 0x90    /* 存在的只读数据段类型值		*/
#define DAD_RW 0x92   /* 存在的可读写数据段属性值		*/
#define DAD_RWA 0x93  /* 存在的已访问可读写数据段类型值	*/
#define DAC_E 0x98    /* 存在的只执行代码段属性值		*/
#define DAC_ER 0x9A   /* 存在的可执行可读代码段属性值		*/
#define DAC_ECO 0x9C  /* 存在的只执行一致代码段属性值 */
#define DAC_ECOR 0x9E /* 存在的可执行可读一致代码段属性值	*/

//选择子属性定义
#define SA_RPL_MASK 0xFFFC
#define SA_RPL0 0
#define SA_RPL1 1
#define SA_RPL2 2
#define SA_RPL3 3

#define SA_TI_MASK 0xFFFB
#define SA_TIG 0
#define SA_TIL 4

//定义中断芯片相关的硬件中断
#define NR_IRQ 16 /* Number of IRQs */
#define CLOCK_IRQ 0
#define KEYBOARD_IRQ 1
#define CASCADE_IRQ 2   /* cascade enable for 2nd AT controller */
#define ETHER_IRQ 3     /* default ethernet interrupt vector */
#define SECONDARY_IRQ 3 /* RS232 interrupt vector for port 2 */
#define RS232_IRQ 4     /* RS232 interrupt vector for port 1 */
#define XT_WINI_IRQ 5   /* xt winchester */
#define FLOPPY_IRQ 6    /* floppy disk */
#define PRINTER_IRQ 7
#define AT_WINI_IRQ 14 /* at winchester */

//多级反馈队列进程调度时间片限制
#define FIRST_QUENE_SLICE 2
#define SECOND_QUENE_SLICE 4
#define LAST_QUENE_SLICE 8
#endif