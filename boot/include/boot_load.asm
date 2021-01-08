; ---------------------------------------------------------------------------------------
; 记录MBR引导过程中的一些操作
; 读取磁盘块所用数据结构: disk_address_packet
; 清屏操作:           clear_screen
; 输出状态信息：       display_str 
; 读扇区:             sector_reader
; 加载inode:          inode_loader
; ---------------------------------------------------------------------------------------

;读取磁盘块所用数据结构
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
        db  "Loader Not Found"

    Booting_Message_Length equ 16               ;打印状态信息长度相同


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
    mov ax, 0x0130
    mov bx, 0x07
    mov dl, 0
    int 10h
    popa                  ;出栈
    ret

; ---------------------------------------------------------------------------
; sector_reader
; 通过int 0x13中断调用读扇区功能
; ah=0x42h代表读取n个sector到buffer
; ---------------------------------------------------------------------------
sector_reader:
    pusha
    mov ah , 042h
    mov dl , 080h
    mov si , disk_address_packet
    int 0x13
    popa
    ret

; ---------------------------------------------------------------------------
; inode_loader
; 获取inode并将相应块放到inodetable处
; 修改es和bx使其指向inode
; ---------------------------------------------------------------------------
inode_loader:
    mov word	[disk_address_packet + 4],	InodeTable_Offset
	mov	word	[disk_address_packet + 6],	InodeTable_Base
	dec eax		;需要注意的是inode的编号是从1开始的
	mov bl,8	;每个block可以存放8个inode
	div	bl			; al => 商,单位为block
	mov cl, ah ; cl => 余数,为block内偏移
	xor ah, ah

	mov bx, GroupDescriptors_Base
	mov	es, bx
	mov	ebx,	dword [es:GroupDescriptors_Offset + 8]				;获取inode table首地址,单位为block
	add eax, ebx		; eax => 所在的总的block
	add eax, eax		; eax => 所在的总的扇区号
	mov	dword	[disk_address_packet + 8],	eax

	call sector_reader

	mov bx, InodeTable_Base
	mov es, bx
	mov al, cl
	mov bl, Inode_Length
	mul bl
	mov bx,ax

	ret

