// ---------------------------------------------------------------
// 链接kernel.asm,定义在kernel.asm中使用的内核函数
// ---------------------------------------------------------------

#include "base.h"

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
    disp_str("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"
             "gdb change");
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

    init_8259A();
    init_idt_description(INT_VECTOR_DIVIDE, DA_386IGate,
                         divide_error, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_DEBUG, DA_386IGate,
                         single_step_exception, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_NMI, DA_386IGate,
                         nmi, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_BREAKPOINT, DA_386IGate,
                         breakpoint_exception, PRIVILEGE_USER);

    init_idt_description(INT_VECTOR_OVERFLOW, DA_386IGate,
                         overflow, PRIVILEGE_USER);

    init_idt_description(INT_VECTOR_BOUNDS, DA_386IGate,
                         bounds_check, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_INVAL_OP, DA_386IGate,
                         invalid_opcode, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_COPROC_NOT, DA_386IGate,
                         copr_not_available, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_DOUBLE_FAULT, DA_386IGate,
                         double_fault, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_COPROC_SEG, DA_386IGate,
                         copr_seg_overrun, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_INVAL_TSS, DA_386IGate,
                         invalid_tss, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_SEG_NOT, DA_386IGate,
                         segment_not_present, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_STACK_FAULT, DA_386IGate,
                         stack_exception, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_PROTECTION, DA_386IGate,
                         general_protection, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_PAGE_FAULT, DA_386IGate,
                         page_fault, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_COPROC_ERR, DA_386IGate,
                         copr_error, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ0 + 0, DA_386IGate,
                         hwint00, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ0 + 1, DA_386IGate,
                         hwint01, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ0 + 2, DA_386IGate,
                         hwint02, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ0 + 3, DA_386IGate,
                         hwint03, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ0 + 4, DA_386IGate,
                         hwint04, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ0 + 5, DA_386IGate,
                         hwint05, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ0 + 6, DA_386IGate,
                         hwint06, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ0 + 7, DA_386IGate,
                         hwint07, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ8 + 0, DA_386IGate,
                         hwint08, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ8 + 1, DA_386IGate,
                         hwint09, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ8 + 2, DA_386IGate,
                         hwint10, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ8 + 3, DA_386IGate,
                         hwint11, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ8 + 4, DA_386IGate,
                         hwint12, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ8 + 5, DA_386IGate,
                         hwint13, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ8 + 6, DA_386IGate,
                         hwint14, PRIVILEGE_KRNL);

    init_idt_description(INT_VECTOR_IRQ8 + 7, DA_386IGate,
                         hwint15, PRIVILEGE_KRNL);
    disp_str("\n idt change\n");
}

// void init_tss()
// {
//     memset(&tss, 0, sizeof(tss));
//     tss.ss0 = SELECTOR_KERNEL_DS;
//     init_descriptor(&gdt[INDEX_TSS],
//                     vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss),
//                     sizeof(tss) - 1,
//                     //段属性
//                     DA_386TSS);
//     tss.iobase = sizeof(tss);
// }
