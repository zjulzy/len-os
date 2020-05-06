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

// 系统段描述符类型值说明
#define DA_LDT 0x82      /* 局部描述符表段类型值			*/
#define DA_TaskGate 0x85 /* 任务门类型值   */
#define DA_386TSS 0x89   /* 可用 386 任务状态段类型值		*/
#define DA_386CGate 0x8C /* 386 调用门类型值			*/
#define DA_386IGate 0x8E /* 386 中断门类型值			*/
#define DA_386TGate 0x8F /* 386 陷阱门类型值			*/

// 存储段描述符类型值说明
#define DAD_R 0x90  /* 存在的只读数据段类型值		*/
#define DAD_RW 0x92 /* 存在的可读写数据段属性值		*/
#define DAD_RWA 0x93 /* 存在的已访问可读写数据段类型值	*/
#define DAC_E 0x98  /* 存在的只执行代码段属性值		*/
#define DAC_ER 0x9A /* 存在的可执行可读代码段属性值		*/
#define DAC_ECO 0x9C /* 存在的只执行一致代码段属性值 */
#define DAC_ECRO 0x9E /* 存在的可执行可读一致代码段属性值	*/

#endif