%include "kernel.inc"


; 导入全局变量
    extern gdt_ptr
    extern idt_ptr
    extern disp_pos

; 导入kernel.cpp函数
    extern init_gdt       ;将gdt_ptr指向新的GDT
    extern init_idt
    extern init_tss
    extern init_proc

; 导出中断处理函数
    global  hwint00
    global  hwint01
    global  hwint02
    global  hwint03
    global  hwint04
    global  hwint05
    global  hwint06
    global  hwint07
    global  hwint08
    global  hwint09
    global  hwint10
    global  hwint11
    global  hwint12
    global  hwint13
    global  hwint14
    global  hwint15
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
    call init_gdt                      ; kernel.cpp中改变gdt_ptr
    lgdt [gdt_ptr]
    ;打开中断
    call init_idt
    lidt  [idt_ptr]
    jmp $
    ; 强制使用刚刚初始化的结构
    ; jmp SELECTOR_KERNEL_CS:kernel_start
; --------------------------------------------------------------------------------------------

; kernel_start:
;     ; 初始化tss
;     call init_tss
;     xor	eax, eax
; 	mov	ax, SELECTOR_TSS
; 	ltr	ax

;     ; 调用process文件夹下函数初始化PCB队列
;     call init_proc
;     jmp$
;     ; 系统内核初始化结束
; sys_call:

; ALIGN 16
; hwint00:
;     mov  al ,  EOI
;     out   INT_MASTER_CTL , al 
    
;     iretd

; ; 中断返回函数,完成特权级的切换
; hwint01:
; hwint02:
; hwint03:
; hwint04:
; hwint05:
; hwint06:
; hwint07:
; hwint08:
; hwint09:
; hwint10:
; hwint11:
; hwint12:
; hwint13:
; hwint14:
; hwint15:

; restart:
;     mov  esp , [proc_queen1_head]
;     lldt	[esp + P_LDT_SEL]
; 	lea	eax, [esp + P_STACKTOP]
; 	mov	dword [tss + TSS3_S_SP0], eax
;     dec	dword [k_reenter]
; 	pop	gs
; 	pop	fs
; 	pop	es
; 	pop	ds
; 	popad
; 	add	esp, 4
; 	iretd
