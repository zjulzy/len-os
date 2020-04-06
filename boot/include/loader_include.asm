; ------------------------------------------------------------------
; loader_include.asm
; 定义loader中使用的函数,
; 分为实模式下所用的函数和保护模式下使用的函数
; ------------------------------------------------------------------


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

	call read_sector

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
    mov byte [_DiskAddressPacket + 2], 2
    mov eax, dword [es:bx + Inode_Block]
	add bx, 4
	add eax, eax
	mov	dword	[_DiskAddressPacket + 8],	eax
	
	;读取磁盘并使内存目的地址指向下一个block
	call ReadSector
	add	word	[_DiskAddressPacket + 4], 0x400
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
	push dword [_DiskAddressPacket + 4]
	push dword [_DiskAddressPacket + 6]

	; 读取直接索引的block
	; 读取索引block
	mov eax, dword [es:bx + Inode_Block]
	add eax, eax
	mov	dword	[_DiskAddressPacket + 8],	eax
	mov word	[_DiskAddressPacket + 4],	First_Index_Block_Offset
	mov	word	[_DiskAddressPacket + 6],	First_Index_Block_Base
	call ReadSector

	; 根据索引block读取
	;调整es和bx
	mov ax, FirstIndexBlockBase
	mov es, ax
	mov bx, FirstIndexBlockOffset
	;恢复原本文件的内存写入位置
	pop dword [_DiskAddressPacket + 6]
	pop dword [_DiskAddressPacket + 4]
ReadFirstBlock:
	mov eax, dword [es:bx]
	add bx, 4
	add eax, eax
	mov	dword	[_DiskAddressPacket + 8],	eax

	;读取磁盘并使内存目的地址指向下一个block
	call ReadSector
	add	word	[_DiskAddressPacket + 4], 0x400
	sub ecx, 2
	ja	ReadFirstBlock

Return:

	ret




; 保护模式下使用的函数
