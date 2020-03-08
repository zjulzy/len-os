; ---------------------------------------------------------------------------------------
; 记录MBR引导过程中的一些操作
; ---------------------------------------------------------------------------------------
disk_address_packet:
				db	0x10		; [ 0 ] 数据结构的长度,单位为byte
				db	0				; [ 1 ] 保留位,取0
				db	2				; [ 2 ] 读取的块数,超级块1k占用两个扇区(块)
				db	0				; [ 3 ] 保留位,取0
				dw	SuperBlock_Offset			   ; [ 4 ] 写入内存位置的偏移
				dw	SuperBlock_Base				    ; [ 6 ] 写入内存位置的段
				dd	SuperBlock_LBA_L			   ; [ 8 ] LBA. Low  32-bits.
				dd	SuperBlock_LBA_H			  ; [12] LBA. High 32-bits.

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