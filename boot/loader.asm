; +++++++++++++++++++++++++++++++++++++++++++++++++++++++
; loader.asm
; 由boot引导加载入内存后，进行以下步骤：
; １．为了分页,从BIOS获取内存信息
; ２．搜索并加载kernel.bin
; ３．由实模式进入保护模式
; ４．启动内存分页机制
; ５．重新部署kernel
; ６．跳转进入kernel
; +++++++++++++++++++++++++++++++++++++++++++++++++++++++

jmp loader_start

;载入配置信息
%include "pm.inc"
%include "loader.inc"
%include "loader_include.asm"


; section .data的开始-----------------------------------------------------------------------------
; 这个section中的变量和宏定义可以在实模式和保护模式下访问
[SECTION .data]

ALIGN	32
Label_Data:
    ; 实模式下的符号
    ; 打印需要的字符串
    _Mem_Check_Title:  db  "BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0
    _RAM_Size_Disp:    db  "RAM size:",0   
    _Char_Return: db 0Ah, 0     
    _Page_Ready:     db  "Page divided",0
    _Kernel_Ready:  db "Kerenel Replaced Ready",0
    _Protect_Mode_Ready: db "Protect Mode initialized",0                
	_Disk_Address_Packet:
			db	0x10		; [ 0 ] 数据结构的长度,单位为byte
			db	0				; [ 1 ] 保留位,取0
			db	2				; [ 2 ] 读取的块数,超级块1k占用两个扇区(块)
			db	0				; [ 3 ] 保留位,取0
			dw	Root_Dir_Offset			  ; [ 4 ] 写入内存位置的偏移
			dw	Root_Dir_Base			   ; [ 6 ] 写入内存位置的段
			dd	0			    ; [ 8 ] LBA. Low  32-bits.
			dd	0			    ; [12] LBA. High 32-bits.    
    _ARD_Struct:	; 地址范围描述符结构定义

        _dwBaseAddrLow:		dd	0
        _dwBaseAddrHigh:		dd	0
        _dwLengthLow:			dd	0
        _dwLengthHigh:		dd	0 
        _dwType:			dd	0
    
    
    _Mem_Disp_Pos:	dd (80 * 6 + 0) * 2	; 打印在屏幕第 6 行, 第 0 列
    _RAM_Size:  dd   0                                       ; 内存大小,通过bios得到
    _Mem_Check_Res:  dd 0                        ; 检查需要描述符的数量
    _Mem_Check_Buff: times  256  db 0            ; 检查内存时使用的缓存区

    ; 保护模式下的符号
    Mem_Check_Title          equ   Loader_PLA+_Mem_Check_Title
    RAM_Size_Disp     equ   Loader_PLA+_RAM_Size_Disp
    Char_Return      equ    Loader_PLA+_Char_Return
    Page_Ready       equ   Loader_PLA+_Page_Ready
    Kernel_Ready    equ   Loader_PLA+_Kernel_Ready
    Protect_Mode_Ready   equ   Loader_PLA+_Protect_Mode_Ready
    ARD_Struct		equ	Loader_PLA + _ARD_Struct
    dwBaseAddrLow	equ	Loader_PLA + _dwBaseAddrLow
    dwBaseAddrHigh	equ	Loader_PLA + _dwBaseAddrHigh
    dwLengthLow	equ	Loader_PLA + _dwLengthLow
    dwLengthHigh	equ	Loader_PLA + _dwLengthHigh
    dwType		equ Loader_PLA + _dwType

    Mem_Disp_Pos     equ   Loader_PLA+_Mem_Disp_Pos
    RAM_Size      equ   Loader_PLA+  _RAM_Size
    Mem_Check_Res    equ  Loader_PLA+_Mem_Check_Res
    Mem_Check_Buff    equ  Loader_PLA+_Mem_Check_Buff
    Disk_Address_Packet equ Loader_PLA+_Disk_Address_Packet
; section .data的结尾----------------------------------------------------------
loader_start:                                    ; loader代码入口
    mov  ax, cs
    mov  ds, ax
    mov  es, ax
    ; 实模式堆栈范围是020000h到020400h
    mov  ax, Real_Mode_Stack_Base
    mov  ss, ax
    mov  sp, Real_Mode_Stack_Top
    ;输出"loading..."   
    mov dh, 03h             ;  显示行数
    mov  dl ,0                   ; 选择显示哪条信息     
    call RealModeDisp

    ; 使用int 15h得到内存数,需要将ax赋值为0E820h
    ; 使用中断会将地址范围描述符填充在指定的缓存空间中
	mov	ebx, 0			; ebx = 后续值, 开始时需为 0
	mov	di, _Mem_Check_Buff		; es:di 指向一个地址范围描述符结构(ARDS)

.MemCheckLoop:
    mov	eax, 0E820h		             ; eax = 0000E820h
	mov	ecx, 20			                   ; ecx = 地址范围描述符结构的大小
	mov	edx, 0534D4150h		  ; edx = 'SMAP'
	int	15h			                                ; int 15h
    jc  .MemCheckFail                   ; 出错时CF置一
    ; 每次循环使di加20,描述符个数加1
    add	di, 20                                
	inc	dword [_Mem_Check_Res]   	; dwMCRNumber = ARDS 的个数
	cmp	ebx, 0                               ; ebx为0表示为最后一个地址描述符
	jne	.MemCheckLoop
    ; 终止循环
	jmp	.MemCheckCompleted

; 检测内存失败
.MemCheckFail:
    mov dword[_Mem_Check_Res],  0
    mov dh , 04h
    mov dl , 4
    call RealModeDisp
    jmp $
.MemCheckCompleted:
    ; 在根目录中寻找内核
    mov eax , 2
    call GetInode

    mov word	[_Disk_Address_Packet + 4],	Root_Dir_Offset
	mov	word	[_Disk_Address_Packet + 6],	Root_Dir_Base
	mov	eax, [es:bx + Inode_Block]
	add eax,eax
	mov	dword	[_Disk_Address_Packet + 8],	eax
	mov	dword	[_Disk_Address_Packet + 12],	0
	mov	eax, [es:bx + Inode_Blocks]
	shr eax, 1
	mov	cx,	ax
    ; 如果根目录占用超过12个block,则只搜索前12个
	cmp cx, 12
	jle .RootLoad
	mov cx, 12

.RootLoad:
    cmp cx , 0
    je   .NoKernel
    call  ReadSector
    dec  cx            ; cx表示还剩多少个block未被搜索
    add bx,4
	mov	eax,[es:bx]
	add eax,eax
	mov	dword	[_Disk_Address_Packet + 8],	eax
.SearchKernel:
    pusha
	mov si,Kernel_Name
	mov eax, Root_Dir_Base
	mov gs,eax
	mov bx, Root_Dir_Offset
; ********************************************************
; 对比文件名长度
.FileLengthCmp:
    xor cx,cx
	mov cl, Kernel_Name_Length
    push bx
	cmp cl, byte [gs:bx+Name_Len_Offset]
	jnz .NextMatch
	mov si,Kernel_Name
	
; ********************************************************
; 以db为单位对比文件名
.FileNameCmp:
    lodsb				; ds:si -> al
	;只有bx能当做基址寄存器
	cmp al,  byte[gs:bx+File_Name_Offset]
	jnz .NextMatch
	dec cl
	jz .KernelFound
	inc bx
	jmp .FileNameCmp
.NextMatch:
    pop bx
    add bx, word [gs:bx+Record_Length_Offset]
	cmp bx,1024
	jl .FileLengthCmp
	popa
	jmp .RootLoad
.NoKernel:
    pop  bx
    mov dx, 0403h
	call RealModeDisp
	jmp $
.KernelFound:
    pop  bx
	mov dx, 0402h
	call RealModeDisp

.KernelLoad:
    mov eax, dword [gs:bx + Inode_Number_Offset]
	call	GetInode
	
	mov word	[_Disk_Address_Packet + 4],	Kernel_Offset
	mov	word	[_Disk_Address_Packet + 6],	Kernel_Base
	
	call	LoadFile
    mov dx, 0501h
	call RealModeDisp

; 准备进入保护模式 -----------------------------------------------------------------
.ToProtectedMode:
    ; 加载GDT
    lgdt [GdtPtr]

    ; 关中断
    cli

    ; 打开地址线A20,增加地址线位数
    in al, 92h
    or al , 00000010b
    out 92h,al

    ; 切换到保护模式
    mov  eax , cr0
    or  eax , 1
    mov  cr0 , eax
    jmp dword SelectorFlatC:(Loader_PLA+Label_PM_Start)
; 实模式结束-------------------------------------------------------------------------

; 从此以后的代码在保护模式下执行 ----------------------------------------------------
; 32 位代码段. 由实模式跳入 ---------------------------------------------------------
[SECTION .s32]

ALIGN	32

[BITS	32]

Label_PM_Start:
    ; 初始化堆栈
    mov	ax, SelectorVideo
	mov	gs, ax
	mov	ax, SelectorFlatRW
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	ss, ax
	mov	esp, Protect_Mode_Stack_Top

    ; 输出保护模式初始化完成
    push Protect_Mode_Ready
    call DispStr
    call DispReturn
    add esp ,4


    ; 启动分页机制
    call DispMemInfo
    call SetupPaging
    push Page_Ready
    call   DispStr
    call   DispReturn
    add  esp ,4

    ; 重新放置内核
    call	InitKernel
	push Kernel_Ready
	call 	DispStr
	call	DispReturn
	add esp,	4


    mov ax,SelectorVideo
	mov gs,ax

    	
	;***************************************************************
	jmp	SelectorFlatC:Kernel_Entry_Point	; 正式进入内核 
	;***************************************************************

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
	mov	edi, [Mem_Disp_Pos]
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
	mov	[Mem_Disp_Pos], edi

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

	push	Mem_Check_Title
	call	DispStr
	add	esp, 4

	mov	esi, Mem_Check_Buff
	mov	ecx, [Mem_Check_Res]	
.loop:					
	mov	edx, 5			
	mov	edi, ARD_Struct	
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
	cmp	eax, [RAM_Size ]	
	jb	.2			
	mov	[RAM_Size], eax	
.2:				
	loop	.loop			
	call	DispReturn	
	push	RAM_Size_Disp
	call	DispStr			
	add	esp, 4			
	push	dword [RAM_Size]	
	call	DispInt		
	add	esp, 4		

	pop	ecx
	pop	edi
	pop	esi
	ret
; ---------------------------------------------------------------------------
; DispReturn换行
DispReturn:
	push	Char_Return
	call	DispStr			
	add	esp, 4

	ret
; ---------------------------------------------------------------------------

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
	mov	edi, [Mem_Disp_Pos]
	mov	[gs:edi], ax
	add	edi, 4
	mov	[Mem_Disp_Pos], edi
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

	mov	edi, [Mem_Disp_Pos]

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

	mov	[Mem_Disp_Pos], edi

	pop	edi
	pop	edx
	pop	ecx

	ret
; DispAL 结束-------------------------------------------------------------

; 启动分页机制 --------------------------------------------------------------
SetupPaging:
	; 根据内存大小计算应初始化多少PDE以及多少页表
	xor	edx, edx
	mov	eax, [RAM_Size]
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
	mov	edi, PageDir_Base	; 此段首地址为 PageDir_Base
	xor	eax, eax
	mov	eax, PageTable_Base | PG_P  | PG_USU | PG_RWW
.1:
	stosd
	add	eax, 4096		; 为了简化, 所有页表在内存中是连续的.
	loop	.1

	; 再初始化所有页表
	pop	eax			; 页表个数
	mov	ebx, 1024		; 每个页表 1024 个 PTE
	mul	ebx
	mov	ecx, eax		; PTE个数 = 页表个数 * 1024
	mov	edi, PageTable_Base	; 此段首地址为 PageTable_Base
	xor	eax, eax
	mov	eax, PG_P  | PG_USU | PG_RWW
.2:
	stosd
	add	eax, 4096		; 每一页指向 4K 的空间
	loop	.2

	mov	eax, PageDir_Base
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
    mov	cx, word [Kernel_PLA+ ELFHeader_e_phnum];	;Program Header Table中entry个数
	movzx	ecx, cx
	mov	esi, [Kernel_PLA + ELFHeader_e_phoff]				;Program Header Table 在文件中的偏移
	add	esi, Kernel_PLA					

deal_with_one_program_header_table_entry:


	mov	eax, [esi]
	cmp	eax, 0																										  ; 0 -> unused  1 -> loadable
	jz	init_next_section
	push	dword [esi + ELFProgramHeader_p_size]							   ; 获取section的大小
	mov	eax, [esi + ELFProgramHeader_p_offset]								; 获取section在文件中的偏移
	add	eax, Kernel_PLA														 ; 获取section在内存中的偏移
	push	eax																										   ; section在内存中的源地址
	push	dword [esi +ELFProgramHeader_p_vaddr]						    ; section在内存中的目的地址
	call	MemCpy
	add	esp, 12																										 ; 恢复堆栈

init_next_section:
	add	esi, 020h																								   ; 指向下一个 Program Header Table entry
	dec	ecx
	jnz	deal_with_one_program_header_table_entry


	ret

; 堆栈就在32位代码段的末尾
Protect_Mode_Stack_Space:	times	1000h	db	0
Protect_Mode_Stack_Top	equ	Loader_PLA + $
