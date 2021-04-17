#include "base.h"

void init_descriptor(DESCRIPTOR *p_desc, u32 base, u32 limit, u16 attribute) {
  p_desc->limit_low = limit & 0x0FFFF;      // 段界限 1		(2 字节)
  p_desc->base_low = base & 0x0FFFF;        // 段基址 1		(2 字节)
  p_desc->base_mid = (base >> 16) & 0x0FF;  // 段基址 2		(1 字节)
  p_desc->attr1 = attribute & 0xFF;         // 属性 1
  p_desc->limit_high_attr2 =
      ((limit >> 16) & 0x0F) | (attribute >> 8) & 0xF0;  // 段界限 2 + 属性 2
  p_desc->base_high = (base >> 24) & 0x0FF;  // 段基址 3		(1 字节)
}

//由段名求绝对地址
u32 seg2phys(u16 seg) {
  DESCRIPTOR *p_dest = &gdt[seg >> 3];

  return (p_dest->base_high << 24) | (p_dest->base_mid << 16) |
         (p_dest->base_low);
}
//初始化中断门描述符
void init_idt_description(unsigned char vector, u8 desc_type,
                          int_handler handler, unsigned char privilege) {
  GATE *p_gate = &idt[vector];
  u32 base = (u32)handler;
  p_gate->offset_low = base & 0xFFFF;
  p_gate->selector = SELECTOT_KERNEL_C;
  p_gate->dcount = 0;
  p_gate->attr = desc_type | (privilege << 5);
  p_gate->offset_high = (base >> 16) & 0xFFFF;
}

// exception_handler,异常处理函数,用于输出异常信息,包含eip,cs,eflags================
void exception_handler(int err_vec, int err_code, int eip, int cs, int eflags) {
  int i;
  int text_color = 0x74; /* 灰底红字 */

  char *err_msg[] = {"#DE Divide Error",
                     "#DB RESERVED",
                     "—  NMI Interrupt",
                     "#BP Breakpoint",
                     "#OF Overflow",
                     "#BR BOUND Range Exceeded",
                     "#UD Invalid Opcode (Undefined Opcode)",
                     "#NM Device Not Available (No Math Coprocessor)",
                     "#DF Double Fault",
                     "    Coprocessor Segment Overrun (reserved)",
                     "#TS Invalid TSS",
                     "#NP Segment Not Present",
                     "#SS Stack-Segment Fault",
                     "#GP General Protection",
                     "#PF Page Fault",
                     "—  (Intel reserved. Do not use.)",
                     "#MF x87 FPU Floating-Point Error (Math Fault)",
                     "#AC Alignment Check",
                     "#MC Machine Check",
                     "#XF SIMD Floating-Point Exception"};

  /* 通过打印空格的方式清空屏幕的前五行，并把 disp_pos 清零 */
  disp_pos = 0;
  for (i = 0; i < 80 * 5; i++) {
    disp_str(" ");
  }
  disp_pos = 0;

  disp_str("Exception! --> ");
  disp_str(err_msg[err_vec]);
  disp_str("\n\n");
  disp_str("EFLAGS:");
  disp_int(eflags);
  disp_str("CS:");
  disp_int(cs);
  disp_str("EIP:");
  disp_int(eip);

  if (err_code != 0xFFFFFFFF) {
    disp_str("Error code:");
    disp_int(err_code);
  }
}
//============================================================================

// i8259芯片相关
void i8259_handler(int code) {
  disp_str("zhongduan ");
  disp_int(code);
  disp_str("\n");
}