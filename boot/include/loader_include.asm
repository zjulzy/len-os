; ------------------------------------------------------------------
; loader_include.asm
; 定义loader中使用的函数,
; 分为实模式下所用的函数和保护模式下使用的函数
; 同时定义只能在实模式使用的变量和宏定义
; ------------------------------------------------------------------



; *****************************************************************************************************************
; GDT相关数据结构
; GDT段描述符表
; 用于生成描述符的宏定义在pm.inc
;                                                                          段基址               段界限         属性
LABEL_GDT:			Descriptor          0,                    	0, 					0																	; 空描述符
LABEL_DESC_FLAT_C:		Descriptor      0,              0fffffh, 			DA_CR  | DA_32 | DA_LIMIT_4K				; 0 ~ 4G代码段(靠偏移寻址)
LABEL_DESC_FLAT_RW:	 Descriptor         0,              0fffffh, 			DA_DRW | DA_32 | DA_LIMIT_4K			; 0 ~ 4G数据段(靠偏移寻址)
LABEL_DESC_VIDEO:		 Descriptor	 0B8000h,           0ffffh, 			DA_DRW | DA_DPL3									; 显存,首地址为0B8000h
GdtLen		equ	$ - LABEL_GDT                        ;GDT长度

GdtPtr:
		dw	GdtLen - 1														; GDT段界限
		dd	Loader_PLA + LABEL_GDT		  ; GDT段基址,即GDTR
; 全局段选择子
SelectorFlatC		equ	LABEL_DESC_FLAT_C	- LABEL_GDT								; 全局 32位 可执行可读 0级 代码段描述符 范围为0 ~ 4G
SelectorFlatRW		equ	LABEL_DESC_FLAT_RW	- LABEL_GDT						  ; 全局 32位 可读可写 	   0级 数据段描述符 范围为0 ~ 4G
SelectorVideo		equ	LABEL_DESC_VIDEO	- LABEL_GDT + SA_RPL3	    ; 全局 32位 可读可写 	 3级 显存段描述符 段长度为1Mb
; *****************************************************************************************************************

; 运行状态字符串
Message_Length equ  13
Message:
    db "loading...   "
    db "kernel loaded"
    db "kernel found "
    db "No kernel    "
    db "memery failed"

; 相关数据定义,其中以"_"开头的在实模式下访问;
; 以字母开头的表示相关变量的偏移,在保护模式下访问




; 实模式下使用的函数

;======================================
; DispStr_In_RealMode
; dh 表示要显示行数
; dl 表示要显示的信息的编号
; 需要注意的是boot程序已经在前两行显示了东西了
;======================================
RealModeDisp:
	
	pusha 
	mov	cx, Message_Length	; CX = 串长度
	mov	ax, Message_Length
	mul	dl
	add	ax, Message
	mov	bp, ax			; ┓
	mov	ax, ds			; ┣ ES:BP = 串地址
	mov	es, ax			; ┛
	mov	ax, 01301h		; AH = 13,  AL = 01h
	mov	bx, 0007h		; 页号为0(BH = 0) 黑底白字(BL = 07h)
	mov	dl, 0
	int	10h			; int 10h
	popa
	ret
; +++++++++++++++++++++++++++++++++++++++++++++
; 和boot一样使用getinode和readsector找到内核文件
; get_inode
; 获取inode号对应的inode
; eax = 目标inode号
; 结束后es:bx指向目标inode
; read_sector
; 调用前需要设置好Disk_Address_Packet结构体
; 使用前后寄存器会发生改变,为了节省空间不再管这个了
; +++++++++++++++++++++++++++++++++++++++++++++
GetInode:
	;把获取到的inode所在的block放在InodeTable处
	;修改es和bx使其指向目标inode
	mov word	[_Disk_Address_Packet + 4],	InodeTable_Offset
	mov	word	[_Disk_Address_Packet + 6],	InodeTable_Base

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
	mov	dword	[_Disk_Address_Packet + 8],	eax

	call ReadSector

	mov bx, InodeTable_Base
	mov es, bx
	mov al, cl
	mov bl, Inode_Length
	mul bl
	mov bx,ax

	ret
ReadSector:
    pusha
	xor	ebx, ebx
	mov	ah, 042h
	mov	dl, 080h			; c 盘盘符
	mov	si, _Disk_Address_Packet
	int	0x13
	popa
	ret

LoadFile:
    ;保存原始的bx
	push bx

	; 读取直接索引的block
	mov ecx, dword [es:bx + Inode_Blocks]
	cmp ecx, 24																;直接索引最大支持24个扇区
	jbe	ReadDirectBlock
	mov	ecx,24

ReadDirectBlock:
    mov byte [_Disk_Address_Packet + 2], 2
    mov eax, dword [es:bx + Inode_Block]
	add bx, 4
	add eax, eax
	mov	dword	[_Disk_Address_Packet + 8],	eax
	
	;读取磁盘并使内存目的地址指向下一个block
	call ReadSector
	add	word	[_Disk_Address_Packet + 4], 0x400
	sub ecx, 2
	ja	ReadDirectBlock
    ;暂存下一个block的位置
	mov ax, bx
	; 恢复原始的bx
	pop bx

	;判断有没有一级索引的block没有读
	mov ecx, dword [es:bx + Inode_Blocks]
	cmp ecx, 24
	jbe	Return
	sub ecx, 24											;获取还需要读的扇区数
	;恢复指向下一个block的位置
	mov bx, ax

	;保存当前文件的内存写入位置
	push dword [_Disk_Address_Packet + 4]
	push dword [_Disk_Address_Packet + 6]

	; 读取直接索引的block
	; 读取索引block
	mov eax, dword [es:bx + Inode_Block]
	add eax, eax
	mov	dword	[_Disk_Address_Packet + 8],	eax
	mov word	[_Disk_Address_Packet + 4],	First_Index_Block_Offset
	mov	word	[_Disk_Address_Packet + 6],	First_Index_Block_Base
	call ReadSector

	; 根据索引block读取
	;调整es和bx
	mov ax, First_Index_Block_Base
	mov es, ax
	mov bx, First_Index_Block_Offset
	;恢复原本文件的内存写入位置
	pop dword [_Disk_Address_Packet + 6]
	pop dword [_Disk_Address_Packet + 4]
ReadFirstBlock:
	mov eax, dword [es:bx]
	add bx, 4
	add eax, eax
	mov	dword	[_Disk_Address_Packet + 8],	eax

	;读取磁盘并使内存目的地址指向下一个block
	call ReadSector
	add	word	[_Disk_Address_Packet + 4], 0x400
	sub ecx, 2
	ja	ReadFirstBlock

Return:

	ret




