//实现基本功能的函数,统一essential文件夹下各头文件
//声明在base.h和kernel中定义的函数
#ifndef LENOS_BASE_H
#define LENOS_BASE_H
#ifdef __cplusplus
extern "C" {
#endif
#include "display.h"
#include "global.h"
#include "interrupt.h"
#include "memory.h"
#include "process.h"
#include "protect.h"
#include "syscall.h"

//初始化描述符
void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute);
void init_idt_description(unsigned char vector, u8 desc_type,
                          int_handler handler, unsigned char privilege);
void restart();
//中断芯片相关的外中断
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

void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void invalid_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void invalid_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void page_fault();
void copr_error();

u32 seg2phys(u16 seg);
void init_gdt();
void init_ldt();
void init_idt();
void init_tss();
void exception_handler(int err_vec, int err_code, int eip, int cs, int eflags);
void i8259_handler(int code);

void kernel_main();

#ifdef __cplusplus
}
#endif
#endif