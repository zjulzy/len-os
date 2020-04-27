%include "kernel.inc"

; 导入全局变量
extern gdt_ptr
extern idt_ptr
extern disp_pos

; 导入kernel.c函数
extern init_gdt       //将gdt_ptr指向新的GDT



; -------------------------------------------------------------------------------------
; 堆栈段
[SECTION .bss]

StackSpace          resb        2*1024
StackTop:
; -------------------------------------------------------------------------------------
; 代码段----------------------------------------------------------------------------------------
[SECTION .text]

global _start                 ; 导出_start_
_start:
    ;把esp从loader挪到kernel
    mov     esp , StackTop

    ;切换gdt
    sgdt [gdt_ptr]
    call cstart                       ; kernel.c中改变gdt_ptr
    lgdt [gdt_ptr]
    ; lidt  [idt_ptr]
; --------------------------------------------------------------------------------------------