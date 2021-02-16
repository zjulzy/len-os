#include "interrupt.h"

// 初始化时钟中断
void interruptInitialize()
{
}

//初始化8259A,操作顺序不可逆转
//依次向主片和从片中写入ICW1,ICW2,ICW3,ICW4
void init_8259A()
{
    // Master 8259, ICW1
    out_byte(INT_M_CTL, 0x11);

    // Slave  8259, ICW1
    out_byte(INT_S_CTL, 0x11);

    // Master 8259, ICW2. 设置 '主8259' 的中断入口地址为 0x20
    out_byte(INT_M_CTLMASK, INT_VECTOR_IRQ0);

    // Slave  8259, ICW2. 设置 '从8259' 的中断入口地址为 0x28
    out_byte(INT_S_CTLMASK, INT_VECTOR_IRQ8);

    // Master 8259, ICW3
    //意思是从芯片接在主芯片的IR2位置
    out_byte(INT_M_CTLMASK, 0x4);

    // Slave  8259, ICW3
    //意思是从芯片接在主芯片的IR2位置
    out_byte(INT_S_CTLMASK, 0x2);

    // Master 8259, ICW4
    // 设置芯片工作在80x86模式
    out_byte(INT_M_CTLMASK, 0x1);

    // Slave  8259, ICW4
    // 设置芯片工作在80x86模式
    out_byte(INT_S_CTLMASK, 0x1);

    // Master 8259, OCW1
    // 主芯片关闭所有中断,除了时钟中断
    out_byte(INT_M_CTLMASK, 0xFE);

    // Slave  8259, OCW1
    //从芯片关闭所有中断
    out_byte(INT_S_CTLMASK, 0xFF);
}

//时钟中断处理函数
void clock_handler()
{

    p_proc_ready = (p_proc_ready - proc_table + 1) % NR_TASK + proc_table;
    disp_str("#");
}