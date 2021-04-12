%include "kernel.inc"
%include "const.inc"


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
    extern interrupt_request
    extern sys_call_handler
    extern p_proc_ready

    global restart
    global sys_call
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


; -------------------------------------------------------------------------------------
; 堆栈段
[SECTION .bss]

StackSpace          resb        20*1024
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
; 系统内核初始化结束
; -------------------------------------------------------------
sys_call:
    call save 
    sti 
    push edx
    push ecx
    push ebx
    push eax 
    call sys_call_handler
    add esp,4*4
    mov [esi+EAXREG-P_STACKBASE],eax 
    cli 
    ret

%macro hwint_master 1
    call save 
    ; 屏蔽当前中断
    in al,INT_MASTER_CTLMASK
    or al,(%1<<1)
    out INT_MASTER_CTLMASK,al 
    ; 置EOI
    mov al,EOI 
    out INT_MASTER_CTL,al
    sti 
    push %1
    call interrupt_request
    pop ecx 
    cli 
    ; 重新开启当前中断
    in al,INT_MASTER_CTLMASK
    and al,~(%1<<1)
    out INT_MASTER_CTLMASK,al 
    ret
%endmacro
; 8259A从片中断调用宏定义
%macro hwint_slave 1
    call save 
    ; 屏蔽当前中断
    in al,INT_MASTER_CTLMASK
    or al,(%1<<1)
    out INT_MASTER_CTLMASK,al 
    ; 置EOI
    ; 与主片不同，此处要同时给中片和从片同时置EOI
    mov al,EOI 
    out INT_MASTER_CTL,al
    nop
    out INT_SLAVE_CTL,al
    sti 
    push %1
    call interrupt_request
    pop ecx 
    cli 
    ; 重新开启当前中断
    in al,INT_MASTER_CTLMASK
    and al,~(%1<<1)
    out INT_MASTER_CTLMASK,al 
    ret
%endmacro
ALIGN 16
hwint00:
    hwint_master 0

hwint01:
    hwint_master 1
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
    hwint_slave 14
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

save:
    ; 保存现场
    pushad
    push ds
    push es
    push fs 
    push gs
    ; 令es和ds指向ss同样的位置
    ; 注意此时可能有的寄存器保存着系统调用的参数
    mov esi, edx


    mov dx,ss 
    mov ds,dx 
    mov es,dx 
    mov fs,dx
    mov gs,dx

    mov edx,esi
    mov esi,esp
    
    ; 如果当前由同一中断正在处理，就执行中断重入
    inc dword[int_reenter]
    cmp  dword[int_reenter],0
    jne .2
.1:
    mov esp,StackTop
    push restart
    jmp [esi+RETADR-P_STACKBASE]
.2:
    push reenter
    jmp [esi+RETADR-P_STACKBASE]





restart:
	mov	esp, [p_proc_ready]
	lldt	[esp + P_LDT_SEL] 
	lea	eax, [esp + P_STACKTOP]
	mov	dword [tss + TSS3_S_SP0], eax
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

