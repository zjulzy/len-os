//实现基本功能的函数,统一essential文件夹下各头文件
#ifndef LENOS_BASE_H
#define LENOS_BASE_H
//layer = 0
#include "global.h"
#include "display.h"
#include "memory.h"
#include "interrupt_opinion.h"

//初始化描述符
void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute);
void hwint00();
void hwint01();
void hwint02();
void hwint03();
void hwint04();
void hwint05();
void hwint06();
void hwint07();
void hwint08();
void hwint09();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();

u32 seg2phys(u16 seg);
#endif