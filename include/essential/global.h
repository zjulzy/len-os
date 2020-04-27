#ifndef LENOS_GLOBAL_H
#define LENOS_GLOBAL_H
//layer = -1
//定义全局变量
#include "type.h"
#include "const.h"
//kernel中使用的gdt指针
//描述符
// 0~15:Limit  16~47:Base 共48位
extern u8 gdt_ptr[6];
extern DESCRIPTOR gdt[GDT_SIZE];

//定义各种结构体----------------------------------------------------------------------------
//存储段描述符/系统段描述符
typedef struct s_descriptor {
    u16 limit_low;        // Limit
    u16 base_low;         // Base
    u8 base_mid;          // Base
    u8 attr1;             // P(1) DPL(2) DT(1) TYPE(4)
    u8 limit_high_attr2;  // G(1) D(1) 0(1) AVL(1) LimitHigh(4)
    u8 base_high;         // Base
} DESCRIPTOR;












#endif