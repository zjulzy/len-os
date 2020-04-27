//宏定义
#ifndef LENOS_CONST_H
#define LENOS_CONST_H
//8259A芯片端口定义
#define INT_M_CTL 0x20
#define INT_M_CTLMASK 0x21
#define INT_S_CTL 0xA0
#define INT_S_CTLMASK 0xA1  
//8259A中断入口
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28
//定义函数类型
#define PUBLIC
#define PRIVATE static


// GDT 和 IDT 中描述符的个数
#define GDT_SIZE 128
#define IDT_SIZE 256

#endif