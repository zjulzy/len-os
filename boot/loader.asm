; +++++++++++++++++++++++++++++++++++++++++++++++++++++++
; loader.asm
; 由boot引导加载入内存后，进行以下步骤：
; １．为了分页,从BIOS获取内存信息
; ２．搜索并加载kernrl.bin
; ３．由实模式进入保护模式
; ４．启动内存分页机制
; ５．重新部署kernel
; ６．跳转进入kernel
; +++++++++++++++++++++++++++++++++++++++++++++++++++++++

jmp loader_initial

;载入配置信息
%include "pm.inc"
%include "loader.inc"
%include "loader_include.asm"

; *****************************************************************************************************************
; GDT相关数据结构
; GDT段描述符表
; 用于生成描述符的宏定义在pm.inc
;                                                                          段基址               段界限         属性
LABEL_GDT:			Descriptor            				   0,                    	0, 					0																	; 空描述符
LABEL_DESC_FLAT_C:		Descriptor  		  0,              0fffffh, 			DA_CR  | DA_32 | DA_LIMIT_4K				; 0 ~ 4G(靠偏移寻址)
LABEL_DESC_FLAT_RW:	 Descriptor             0,              0fffffh, 			DA_DRW | DA_32 | DA_LIMIT_4K			; 0 ~ 4G(靠偏移寻址)
LABEL_DESC_VIDEO:		 Descriptor	 0B8000h,              0ffffh, 			DA_DRW | DA_DPL3									; 显存,首地址为0B8000h
GdtLen		equ	$ - LABEL_GDT                        ;GDT长度

GdtPtr:
		dw	GdtLen - 1														; GDT段界限
		dd	Loader_Phy_Address + LABEL_GDT		  ; GDT段基址,即GDTR
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

; 相关数据定义,其中以"_"开头的在实模式下访问;
; 以字母开头的表示相关变量的偏移,在保护模式下访问

[SECTION .data]

ALIGN	32
Label_Data:
    ; 实模式下的符号
    ; 打印需要的字符串
    _MemCheckTitle:  db  "BaseAddrL BaseAddrH LengthLow LengthHigh   Type", 0Ah, 0
    _RAMSizeDisp:    db  "RAM size:",0   
    _CharReturn: db 0Ah, 0     
    _PageReady:     db  "Page divided",0
    _KernelReady:  db "Kerenel Replaced Ready",0
    _ProtectModeReady: db "Protec Mode initialized",0                
	_DiskAddressPacket:
			db	0x10		; [ 0 ] 数据结构的长度,单位为byte
			db	0				; [ 1 ] 保留位,取0
			db	2				; [ 2 ] 读取的块数,超级块1k占用两个扇区(块)
			db	0				; [ 3 ] 保留位,取0
			dw	RootDir_Offset			  ; [ 4 ] 写入内存位置的偏移
			dw	RootDir_Base			   ; [ 6 ] 写入内存位置的段
			dd	 0			    ; [ 8 ] LBA. Low  32-bits.
			dd	0			    ; [12] LBA. High 32-bits.    
    _ARDStruct:	; 地址范围描述符结构定义

        _dwBaseAddrLow:		dd	0
        _dwBaseAddrHigh:		dd	0
        _dwLengthLow:			dd	0
        _dwLengthHigh:		dd	0 
        _dwType:			dd	0
    
    
    _MemDispPos:	dd (80 * 6 + 0) * 2	; 打印在屏幕第 6 行, 第 0 列
    _RAMSize:  dd   0                                       ; 内存大小,通过bios得到
    _MemCheckRes:  dd 0                        ; 检查需要描述符的数量
    _MemCheckBuff: times  256  db 0            ; 检查内存时使用的缓存区

    ; 保护模式下的符号
    MemCheckTitle          equ   LoaderPLA+_Mem_Check_Title
    RAMSizeDisp     equ   LoaderPLA+_RAMSizeDisp
    CharReturn      equ    LoaderPLA+_CharReturn
    PageReady       equ   LoaderPLA+_PageReady
    KernelReady    equ   LoaderPLA+_KernelReady
    ProtectModeReady   equ   LoaderPLA+_ProtectModeReady
    ARDStruct		equ	BaseOfLoaderPhyAddr + _ARDStruct
        dwBaseAddrLow	equ	BaseOfLoaderPhyAddr + _dwBaseAddrLow
        dwBaseAddrHigh	equ	BaseOfLoaderPhyAddr + _dwBaseAddrHigh
        dwLengthLow	equ	BaseOfLoaderPhyAddr + _dwLengthLow
        dwLengthHigh	equ	BaseOfLoaderPhyAddr + _dwLengthHigh
        dwType		equ	BaseOfLoaderPhyAddr + _dwType

    MemDispPos     equ   LoaderPLA+_MemDispPos
    RAMSize      equ   LoaderPLA+  _RAMSize
    MemCheckRes    equ  LoaderPLA+_MemCheckRes
    MemCheckBuff    equ  LoaderPLA+_MemCheckBuff
    DiskAddressPacket equ LoaderPLA+_DiskAddressPacket

loader_start:                                    ; loader代码入口
    mov  ax, cs
    mov  ds, ax
    mov  ax, RealModeStackBase
    mov  ss, ax
    mov  sp, RealModeStackTop
    ;输出"loading..."   
    mov dh, 0     
    call RealModeDisp

    ; 得到内存数
	mov	ebx, 0			; ebx = 后续值, 开始时需为 0
	mov	di, _MemCheckBuff		; es:di 指向一个地址范围描述符结构(ARDS)

.MemCheck:
    mov	eax, 0E820h		; eax = 0000E820h
	mov	ecx, 20			; ecx = 地址范围描述符结构的大小
	mov	edx, 0534D4150h		; edx = 'SMAP'
	int	15h			; int 15h
    jc  .MemCheckFail
    ; 每次循环使di加20,描述符个数加1
    add	di, 20
	inc	dword [_MemCheckRes]	; dwMCRNumber = ARDS 的个数
	cmp	ebx, 0
	jne	.MemCheck
    ; 终止循环
	jmp	.MemChkOK

; 检测内存失败
.MemCheckFail:
    mov dword[_MemCheckRes],  0
    jmp $
.MemCheckCompleted:
    mov eax , 2
    call GetInode

    mov word	[_Disk_Address_Packet + 4],	RootDir_Offset
	mov	word	[_Disk_Address_Packet + 6],	RootDir_Base
	mov	eax, [es:bx + Inode_Block]
	add eax,eax
	mov	dword	[_Disk_Address_Packet + 8],	eax
	mov	dword	[_Disk_Address_Packet + 12],	0
	mov	eax, [es:bx + Inode_Blocks]
	shr eax, 1
	mov	cx,	ax
    ; 如果根目录占用超过12个block,则只搜索前12个
	cmp cx, 12
	jle root_dir_read
	mov cx, 12

RootLoad:
    cmp cx , 0
    je   NoKernel
    call  ReadSector
    dec  cx            ; cx表示还剩多少个block未被搜索
    add bx,4
	mov	eax,[es:bx]
	add eax,eax
	mov	dword	[_DiskAddressPacket + 8],	eax
SearchKernel:
    pusha
	mov si,KernelName
	mov eax, RootDirBase
	mov gs,eax
	mov bx, RootDirOffset
    ; ********************************************************
    ; 对比文件名长度
    xor cx,cx
	mov cl, Kernel_File_Name_Length
	cmp cl, byte [gs:bx+Name_Len_Offset]
	jnz file_not_match
	mov si,Kernel_File_Name
	push bx
    ; ********************************************************
    ; 以db为单位对比文件名
FileNameCmp
    lodsb				; ds:si -> al
	;只有bx能当做基址寄存器
	cmp al,  byte[gs:bx+File_Name_Offset]
	jnz NoKernel
	dec cl
	jz KernelFound
	inc bx
	jmp filenamecmp
NextMatch:
    pop   bx
    add bx, word [gs:bx+RecordLengthOffset]
	cmp bx,1024
	jl SearchKernel
	popa
	jmp RootLoad
NoKernel:
    pop  bx
    mov dx, 0403h
	call DispStr_In_RealMode
	jmp $
KernelFound:
    pop  bx
	mov dx, 0401h
	call DispStr_In_RealMode

KernelLoad:
    mov eax, dword [gs:bx + InodeNumberOffset]
	call	get_inode
	
	mov word	[_DiskAddressPacket + 4],	KernelFileOffset
	mov	word	[_DiskAddressPacket + 6],	KernelFileBase
	
	call	LoadFile

; 进入保护模式