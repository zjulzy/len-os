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


; ------------------------------------------------------------------------
;	DispStr
; 	显示一个字符串
;	通过堆栈传递一个4字节字符串起始位置变量
;	字符串结尾用0来标识
; ------------------------------------------------------------------------
DispStr:
	push	ebp              ; 从堆栈中取出字符串
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi

	mov	esi, [ebp + 8]
	mov	edi, [Disp_Pos]
	mov	ah, 0Fh

.1:
	lodsb
	test	al, al					;test指令对符号位的影响和and指令相同
	jz	.2			;判断字符串是否已经显示完

	cmp	al, 0Ah					; 判断当前字符是否为回车
	jnz	.3

	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.1

.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1

.2:
	mov	[Disp_Pos], edi

	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret
; DispStr 结束------------------------------------------------------------

; **********************************************************
; 内存拷贝函数MemCpy
; 仿照void* MemCpy(void* es:pDest, void* ds:pSrc, int iSize);
; **********************************************************
MemCpy:
	push	ebp
	mov	ebp, esp

	push	esi
	push	edi
	push	ecx

	mov	edi, [ebp + 8]	; Destination
	mov	esi, [ebp + 12]	; Source
	mov	ecx, [ebp + 16]	; Counter
.1:
	cmp	ecx, 0		; 判断计数器
	jz	.2		; 计数器为零时跳出

    ; 逐字节移动
	mov	al, [ds:esi]	
	inc	esi		
	mov	byte [es:edi], al
	inc	edi			

	dec	ecx		; 计数器减一
	jmp	.1		; 循环
.2:
	mov	eax, [ebp + 8]	; 返回值

	pop	ecx
	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp

	ret			; 函数结束，返回
; MemCpy函数结束*****************************************


; 显示内存信息 --------------------------------------------------------------
DispMemInfo:
	push	esi
	push	edi
	push	ecx

	push	MemCheckTitle
	call	DispStr
	add	esp, 4

	mov	esi, MemCheckBuff
	mov	ecx, [MemCheckRes]	
.loop:					
	mov	edx, 5			
	mov	edi, ARDStruct	
.1:				
	push	dword [esi]		
	call	DispInt			
	pop	eax			
	stosd			
	add	esi, 4			
	dec	edx			
	cmp	edx, 0			
	jnz	.1			
	call	DispReturn		
	cmp	dword [dwType], 1	
	jne	.2			
	mov	eax, [dwBaseAddrLow]	
	add	eax, [dwLengthLow]	
	cmp	eax, [dwMemSize]	
	jb	.2			
	mov	[dwMemSize], eax	
.2:				
	loop	.loop			
	call	DispReturn	
	push	RAMSizeDisp
	call	DispStr			
	add	esp, 4			;
					;
	push	dword [RAMSize]	;
	call	DispInt		
	add	esp, 4		

	pop	ecx
	pop	edi
	pop	esi
	ret
; ---------------------------------------------------------------------------
; DispReturn换行
DispReturn:
	push	CharReturn
	call	DispStr			
	add	esp, 4

	ret

; ------------------------------------------------------------------------------
; DispInt:显示一个整形数
; ------------------------------------------------------------------------------
DispInt:
	mov	eax, [esp + 4]
	shr	eax, 24
	call	DispAL

	mov	eax, [esp + 4]
	shr	eax, 16
	call	DispAL

	mov	eax, [esp + 4]
	shr	eax, 8
	call	DispAL

	mov	eax, [esp + 4]
	call	DispAL

	mov	ah, 07h			; 0000b: 黑底    0111b: 灰字
	mov	al, 'h'
	push	edi
	mov	edi, [MemDispPos]
	mov	[gs:edi], ax
	add	edi, 4
	mov	[MemDispPos], edi
	pop	edi

	ret
; DispInt 结束------------------------------------------------------------

; ------------------------------------------------------------------------
; DispAL显示 AL 中的数字
; ------------------------------------------------------------------------
DispAL:
	push	ecx
	push	edx
	push	edi

	mov	edi, [Disp_Pos]

	mov	ah, 0Fh			; 0000b: 黑底    1111b: 白字
	mov	dl, al
	shr	al, 4
	mov	ecx, 2

num_trans:
	; 把al中的数字转化成ascii码
	and	al, 01111b
	cmp	al, 9
	ja	big_num
	add	al, '0'
	jmp	display

big_num:
	; 对大于9的数字用A加上超过十的部分
	sub	al, 0Ah
	add	al, 'A'

display:
	;对显存进行操作
	mov	[gs:edi], ax
	add	edi, 2

	mov	al, dl
	loop	num_trans

	mov	[Disp_Pos], edi

	pop	edi
	pop	edx
	pop	ecx

	ret
; DispAL 结束-------------------------------------------------------------

; 启动分页机制 --------------------------------------------------------------
SetupPaging:
	; 根据内存大小计算应初始化多少PDE以及多少页表
	xor	edx, edx
	mov	eax, [dwMemSize]
	mov	ebx, 400000h	; 400000h = 4M = 4096 * 1024, 一个页表对应的内存大小
	div	ebx
	mov	ecx, eax	; 此时 ecx 为页表的个数，也即 PDE 应该的个数
	test	edx, edx
	jz	.no_remainder
	inc	ecx		; 如果余数不为 0 就需增加一个页表
.no_remainder:
	push	ecx		; 暂存页表个数

	; 为简化处理, 所有线性地址对应相等的物理地址. 并且不考虑内存空洞.

	; 首先初始化页目录
	mov	ax, SelectorFlatRW
	mov	es, ax
	mov	edi, PAGE_DIR_BASE	; 此段首地址为 PAGE_DIR_BASE
	xor	eax, eax
	mov	eax, PAGE_TBL_BASE | PG_P  | PG_USU | PG_RWW
.1:
	stosd
	add	eax, 4096		; 为了简化, 所有页表在内存中是连续的.
	loop	.1

	; 再初始化所有页表
	pop	eax			; 页表个数
	mov	ebx, 1024		; 每个页表 1024 个 PTE
	mul	ebx
	mov	ecx, eax		; PTE个数 = 页表个数 * 1024
	mov	edi, PAGE_TBL_BASE	; 此段首地址为 PAGE_TBL_BASE
	xor	eax, eax
	mov	eax, PG_P  | PG_USU | PG_RWW
.2:
	stosd
	add	eax, 4096		; 每一页指向 4K 的空间
	loop	.2

	mov	eax, PAGE_DIR_BASE
	mov	cr3, eax
	mov	eax, cr0
	or	eax, 80000000h
	mov	cr0, eax
	jmp	short .3
.3:
	nop

	ret
; 分页机制启动完毕 ----------------------------------------------------------

; -----------------------------------------------------------------------------------
; InitKernel重新放置内核并对齐
; -----------------------------------------------------------------------------------
InitKernel:
    ; 遍历每一个Program Header,根据Program Header决定相关段的内存存放位置
    xor esi , esi
    mov	cx, word [KernelPLA+ ELFHeader_e_phnum];	;Program Header Table中entry个数
	movzx	ecx, cx
	mov	esi, [KernelPLA + ELFHeader_e_phoff]				;Program Header Table 在文件中的偏移
	add	esi, KernelPLA					

deal_with_one_program_header_table_entry:


	mov	eax, [esi]
	cmp	eax, 0																										  ; 0 -> unused  1 -> loadable
	jz	init_next_section
	push	dword [esi + ELF_Program_Header_p_size]							   ; 获取section的大小
	mov	eax, [esi + ELF_Program_Header_p_offset]								; 获取section在文件中的偏移
	add	eax, Kernel_File_Phy_Addr																 ; 获取section在内存中的偏移
	push	eax																										   ; section在内存中的源地址
	push	dword [esi +ELF_Program_Header_p_vaddr]						    ; section在内存中的目的地址
	call	MemCpy
	add	esp, 12																										 ; 恢复堆栈

init_next_section:
	add	esi, 020h																								   ; 指向下一个 Program Header Table entry
	dec	ecx
	jnz	deal_with_one_program_header_table_entry


	ret
