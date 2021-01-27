#include "base.h"

void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute)
{
    p_desc->limit_low = limit & 0x0FFFF;     // 段界限 1		(2 字节)
    p_desc->base_low = base & 0x0FFFF;       // 段基址 1		(2 字节)
    p_desc->base_mid = (base >> 16) & 0x0FF; // 段基址 2		(1 字节)
    p_desc->attr1 = attribute & 0xFF;        // 属性 1
    p_desc->limit_high_attr2 =
        ((limit >> 16) & 0x0F) | (attribute >> 8) & 0xF0; // 段界限 2 + 属性 2
    p_desc->base_high = (base >> 24) & 0x0FF;             // 段基址 3		(1 字节)
}
//由段名求绝对地址
void seg2phys(u16 seg)
{
    DESCRIPTOR *p_dest = &gdt[seg >> 3];

    return (p_dest->base_high << 24) | (p_dest->base_mid << 16) | (p_dest->base_low);
}