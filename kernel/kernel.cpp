// ---------------------------------------------------------------
// 链接kernel.asm,定义在kernel.asm中使用的内核函数
// ---------------------------------------------------------------
#include "base.h"

//layer=1
void init_gdt()
{
    //初始化新的gdt
    //将原来在loader中的gdt复制到新的gdt中
    //通过memory.asm定义的memcpy函数实现
    memcpy(&gdt, (void *)(*((u32 *)(&gdt_ptr[2]))),
           *((u16 *)(&gdt_ptr[0])) + 1);

    //将新的gdt传递给kernel.asm
    u16 *p_gdt_limit = (u16 *)(&gdt_ptr[0]);
    u32 *p_gdt_base = (u32 *)(&gdt_ptr[2]);
    *p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
    *p_gdt_base = (u32)&gdt;
    //gdt初始化结束
}

void init_idt()
{
    //初始化中断开始---------------------------------------------------------------------

    //初始化中断结束---------------------------------------------------------------------

    //类似gdt的初始化方法,初始化idt
    // idt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base
    u16 *p_idt_limit = (u16 *)(&idt_ptr[0]);
    u32 *p_idt_base = (u32 *)(&idt_ptr[2]);
    *p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
    *p_idt_base = (u32)&idt;
}

void init_tss()
{
    memset(&tss, 0, sizeof(tss));
    tss.ss0 = SELECTOR_KERNEL_DS;
    init_descriptor(&gdt[INDEX_TSS],
                    vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss),
                    sizeof(tss) - 1,
                    //段属性
                    DA_386TSS);
    tss.iobase = sizeof(tss);
}

void cstart()
{
}