%include "kernel.inc"



; 导入全局变量
    extern gdt_ptr
    extern idt_ptr
    extern disp_pos
    extern p_proc_ready
    extern tss
    extern int_reenter
    

; 导入kernel.cpp函数
    extern init_gdt       ;将gdt_ptr指向新的GDT
    extern init_idt
    extern init_tss
    extern init_proc
    extern init_8259A
    extern exception_handler
    extern i8259_handler
    extern kernel_main
    extern disp_str
    extern delay
    extern clock_handler

    global restart
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
    global divide_error;
    global single_step_exception;
    global nmi;
    global breakpoint_exception;
    global overflow;
    global bounds_check;
    global invalid_opcode;
    global copr_not_available;
    global double_fault;
    global copr_seg_overrun;
    global invalid_tss;
    global segment_not_present;
    global stack_exception;
    global general_protection;
    global page_fault;
    global copr_error;

[SECTION .data]
; pcb中寄存器存储位置定义
    P_STACKBASE	equ	0
    GSREG		equ	P_STACKBASE
    FSREG		equ	GSREG		+ 4
    ESREG		equ	FSREG		+ 4
    DSREG		equ	ESREG		+ 4
    EDIREG		equ	DSREG		+ 4
    ESIREG		equ	EDIREG		+ 4
    EBPREG		equ	ESIREG		+ 4
    KERNELESPREG	equ	EBPREG		+ 4
    EBXREG		equ	KERNELESPREG	+ 4
    EDXREG		equ	EBXREG		+ 4
    ECXREG		equ	EDXREG		+ 4
    EAXREG		equ	ECXREG		+ 4
    RETADR		equ	EAXREG		+ 4
    EIPREG		equ	RETADR		+ 4
    CSREG		equ	EIPREG		+ 4
    EFLAGSREG	equ	CSREG		+ 4
    ESPREG		equ	EFLAGSREG	+ 4
    SSREG		equ	ESPREG		+ 4
    P_STACKTOP	equ	SSREG		+ 4
    P_LDT_SEL	equ	P_STACKTOP
    P_LDT		equ	P_LDT_SEL	+ 4

    TSS3_S_SP0	equ	4

    ; 定义主从中断芯片的控制端口
    INT_MASTER_CTL    equ 0x20
    INT_MASTER_CTLMASK   equ  0x21
    INT_SLAVE_CTL   equ  0xA0
    INT_SLAVE_CTLMASK   equ  0xA1
    EOI  equ  0x20
    ClockMessage  db   "z"

; 数据段结束----------------------------------------------------------------------------
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
    call init_tss
    ; 强制使用刚刚初始化的结构
    jmp SELECTOR_KERNEL_CS:kernel_start
; --------------------------------------------------------------------------------

kernel_start:
    xor	eax, eax
	mov	ax, SELECTOR_TSS
	ltr	ax

    jmp kernel_main
    ; 初始化tss
;     call init_tss
;     xor	eax, eax
; 	mov	ax, SELECTOR_TSS
; 	ltr	ax

;     ; 调用process文件夹下函数初始化PCB队列
;     call init_proc
;     jmp$
;     ; 系统内核初始化结束
; -------------------------------------------------------------
; sys_call:

; 中断返回函数,完成特权级的切换
ALIGN 16
hwint00:
    ; 保存现场
    sub esp,4
    pushad
    push ds
    push es
    push fs 
    push gs

    
    ; 令es和ds指向ss同样的位置
    mov dx,ss 
    mov ds,dx 
    mov es,dx 
    
    mov  al ,  EOI
    out   INT_MASTER_CTL , al 
    ; 如果当前由同一中断正在处理，就不进入内核栈
    inc dword[int_reenter]
    cmp  dword[int_reenter],0
    jnz reenter
    ; 切换到内核栈
    mov esp, StackTop
    sti
    

    call clock_handler
    cli
    mov esp,[p_proc_ready]
    lldt [esp+P_LDT_SEL]
    ; 在tss中指定下一次由ring0切换到ring1的esp
    lea eax,[esp+P_STACKTOP]
    mov dword  [tss+TSS3_S_SP0],eax

; 多次相同中断重入
reenter:
    dec dword[int_reenter]
    
    ;恢复寄存器的值
    pop gs 
    pop fs 
    pop es
    pop ds 
    popad
    add esp,4

    iretd


hwint01:
    push 1
    call i8259_handler
    add esp, 2
    hlt
hwint02:
    push 2
    call i8259_handler
    add esp, 2
    hlt
hwint03:
    push 3
    call i8259_handler
    add esp, 2
    hlt
hwint04:
    push 4
    call i8259_handler
    add esp, 2
    hlt
hwint05:
    push 5
    call i8259_handler
    add esp, 2
    hlt
hwint06:
    push 6
    call i8259_handler
    add esp, 2
    hlt
hwint07:
    push 7
    call i8259_handler
    add esp, 2
    hlt
hwint08:
    push 8
    call i8259_handler
    add esp, 2
    hlt
hwint09:
    push 9
    call i8259_handler
    add esp, 2
    hlt
hwint10:
    push 10
    call i8259_handler
    add esp, 2
    hlt
hwint11:
    push 11
    call i8259_handler
    add esp, 2
    hlt
hwint12:
    push 12
    call i8259_handler
    add esp, 2
    hlt
hwint13:
    push 13
    call i8259_handler
    add esp, 2
    hlt
hwint14:
    push 14
    call i8259_handler
    add esp, 2
    hlt
hwint15:
    push 15
    call i8259_handler
    add esp, 2
    hlt

; 支持的异常列表
; 没有错误码需要压栈0xFFFFFFFF
divide_error:
    push 0xFFFFFFFF
    push 0
    jmp exception
single_step_exception:
    push 0xFFFFFFFF
    push 1
    jmp exception
nmi:
    push 0xFFFFFFFF
    push 2
    jmp exception
breakpoint_exception:
    push 0xFFFFFFFF
    push 3
    jmp exception
overflow:
    push 0xFFFFFFFF
    push 4
    jmp exception
bounds_check:
    push 0xFFFFFFFF
    push 5
    jmp exception
invalid_opcode:
    push 0xFFFFFFFF
    push 6
    jmp exception
copr_not_available:
    push 0xFFFFFFFF
    push 7
    jmp exception
double_fault:
    push 8
    jmp exception
copr_seg_overrun:
    push 0xFFFFFFFF
    push 9
    jmp exception
invalid_tss:
    push 10
    jmp exception
segment_not_present:
    push 11
    jmp exception
stack_exception:
    push 12
    jmp exception
general_protection:

    push 13
    jmp exception
page_fault:
    push 14
    jmp exception
copr_error:
    push 0xFFFFFFFF
    push 16
    jmp exception
exception:
    call exception_handler
    add esp,  4*2
    hlt

restart:
	mov	esp, [p_proc_ready]
	lldt	[esp + P_LDT_SEL] 
	lea	eax, [esp + P_STACKTOP]
	mov	dword [tss + TSS3_S_SP0], eax

	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad

	add	esp, 4

	iretd


; restart:
;     mov  esp , [proc_queen1_head]
;     lldt	[esp + P_LDT_SEL]
; 	lea	eax, [esp + P_STACKTOP]
; 	mov	dword [tss + TSS3_S_SP0], eax
;     dec	dword [k_reenter].
    
; 	pop	gs
; 	pop	fs
; 	pop	es
; 	pop	ds
; 	popad
; 	add	esp, 4
; 	iretd
