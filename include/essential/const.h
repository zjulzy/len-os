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
//每个进程允许使用的ldt数目
#define LDT_SIZE 2

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

//===========================================================
//线性地址转为物理地址
# define vir2phy(seg_base,vir) (u32)(((u32)seg_base)+(u32)vir)

#endif