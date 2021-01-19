; ---------------------------------------------------------------------------------------
; 记录MBR引导过程中的一些操作
; 同时其中函数在loader实模式阶段也可以使用
; 读取磁盘块所用数据结构: disk_address_packet
; 清屏操作:           clear_screen
; 输出状态信息：       display_str 
; 读扇区:             sector_reader
; 加载inode:          inode_loader
; 加载文件数据block:     block_loader
; ---------------------------------------------------------------------------------------

;读取磁盘块所用数据结构
; 使用int 0x42读n个sector到内存中的指定缓冲位置
; struct buffer_packet
; {
;     short buffer_packet_size;         /* struct's size（可以为 0x10 或 0x18）*/
;     short sectors;                    /* 读多少个 sectors */
;     char *buffer;                     /* buffer address */
;     long long start_sectors;          /* 从哪个 sector 开始读 */
;     long long *l_buffer;              /* 64 位的 buffer address */
; } buffer;
disk_address_packet:
				db	0x10		; [ 0 ] 数据结构的长度,单位为byte
				db	0				; [ 1 ] 保留位,取0
				db	2				; [ 2 ] 读取的扇区数,超级块1k占用两个扇区
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

;---------------------------------------------------------------------
; 根据dh记录的偏移显示当前状态信息
; int 0x10中断在ah为13时为显示字符功能,es:bp为显示串的地址
; cx为串的长度,bh为页号,al可取0,1,2,3
;---------------------------------------------------------------------
display_str:
    pusha                ;入栈
    mov ax , Booting_Message_Length
    mul dh
    add ax , Booting_Message
    mov bp ,ax 
    mov ax , ds
    mov es , ax
    mov cx , Booting_Message_Length
    mov ax, 0x1301
    mov bx, 0x07
    mov dl, 0
    int 10h
    popa                  ;出栈
    ret

; ---------------------------------------------------------------------------
; sector_reader
; 通过int 0x13中断调用读扇区功能
; int 0x13 的 0x42 号功能从磁盘读 n 个 sectors 到 buffer 中。
; 入口参数：ah = 0x42, dl = 磁盘号（0x80 为硬盘）， ds:si = buffer
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
	add eax, ebx		; eax => loader的Inode所在的总的block
	add eax, eax		; eax => loader的Inode所在的总的扇区号
	mov	dword	[disk_address_packet + 8],	eax

	call sector_reader

	mov bx, InodeTable_Base
	mov es, bx
	mov al, cl
	mov bl, Inode_Length
	mul bl
	mov bx,ax

	ret
; --------------------------------------------------------------------------------------------------------
; block_loader读取指定文件的数据,通过已经读入的inode获取文件的block编号
; 再通过sector_loader读取
; --------------------------------------------------------------------------------------------------------
block_loader:
	; 判断文件大小,最大只加载12个block (12kb)
	; 默认block大小为1kb,得到的数字不是block数,而是扇区数
	mov ecx, dword [es:bx + Inode_Blocks_Offset]
	cmp ecx, 24				;最大支持24个扇区
	jbe	valid_size
	mov	ecx,24
valid_size:
	mov byte [disk_address_packet + 2], 2		;分多次读取,每次读取一个block

read_block:
	mov eax, dword [es:bx + Inode_Block_Offset]
	add bx, 4
	add eax, eax
	mov	dword	[disk_address_packet + 8],	eax
	
	;读取磁盘并使内存目的地址指向下一个block
	call sector_reader
	add	word	[disk_address_packet + 4], 0x400

	sub ecx, 2
	ja	read_block
	ret

