; ---------------------------------------------------------------------------------------
; 记录MBR引导过程中的一些操作
; ---------------------------------------------------------------------------------------

Booting_Message:
        db  "Booting...      "
        db  "Fa Xian Loader  "
        db "Loader Not Found"

    Booting_Message_Length equ 16               ;打印状态信息长度相同

read_section:

;清屏操作
clear_screen:
    pusha                  ;把通用寄存器 AX, CX, DX, BX, SP, BP, SI 及 DI 压入栈中
    mov	ax,	0600h
	mov	bx,	0700h
	mov	cx,	0
	mov	dx,	0184fh
	int 10h
	popa                    ;出栈
	ret

;输出当前状态信息
display_str:
    pusha                ;入栈
    mov ax , Booting_Message_Length
    mul dh
    add ax , Booting_Message
    mov bp ,ax 
    mov ax , ds
    mov es , ax
    mov cx , Booting_Message_Length
    mov ax, 0x0130h
    mov bx, 0x07h
    mov dl, 0
    int 10h
    popa                  ;出栈
    ret

; --------------------------------------------------------------------------
; 根据inode加载file
; --------------------------------------------------------------------------