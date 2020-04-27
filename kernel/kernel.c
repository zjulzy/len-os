// ---------------------------------------------------------------
// 链接kernel.asm,定义在kernel.asm中使用的内核函数
// ---------------------------------------------------------------
#include "base.h"
void init_gdt(){
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
}
