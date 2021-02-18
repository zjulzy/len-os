// ---------------------------------------------------------------
// 链接kernel.asm,定义在kernel.asm中使用的内核函数
// ---------------------------------------------------------------

#include "base.h"
void kernel_main()
{
    int_reenter = 0;
    enable_irq(CLOCK_IRQ);
    delay(10);
    disp_clear();
    disp_str("-----\"kernel_main\" begins-----\n");
    init_ldt();
    init_proc();

    restart();
    while (1)
    {
    }
}

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
//初始化进程ldt，循环放入gdt
void init_ldt()
{
    u32 index_selector_ldt = INDEX_LDT_FIRST;
    for (int i = 0; i < NR_TASK; i++)
    {
        init_descriptor(&gdt[index_selector_ldt], vir2phy(seg2phys(SELECTOR_KERNEL_RW), proc_table[i].ldts), LDT_SIZE * sizeof(DESCRIPTOR) - 1, DA_LDT);
        index_selector_ldt += 1;
    }
}
//初始化idt
//包含异常和硬件中断
void init_idt()
{
    //初始化中断开始---------------------------------------------------------------------
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

void init_tss()
{
    memset(&tss, 0, sizeof(tss));
    tss.ss0 = SELECTOR_KERNEL_RW;
    init_descriptor(&gdt[INDEX_TSS],
                    vir2phy(seg2phys(SELECTOR_KERNEL_RW), &tss),
                    sizeof(tss) - 1,
                    //段属性
                    DA_386TSS);
    tss.iobase = sizeof(tss); //没有io许可位图
    disp_str("tss initialized\n");
}

void delay(int time)
{
    for (int i = 0; i < time; i++)
    {
        for (int j = 0; j < 100; j++)
        {
            for (int k = 0; k < 1000; k++)
            {
            }
        }
    }
}