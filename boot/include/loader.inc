; -----------------------------------------------------------
; loader.asm相关常量
; -----------------------------------------------------------

; loader加载基址
Loader_Base	equ 09000h		 
; loader加载偏移
Loader_Offset equ 00h
; 根据段基址和偏移计算loader加载物理地址
Loader_PLA equ Loader_Base * 0x10 + Loader_Offset

; 实模式下的堆栈定义为0x20000h~0x20400h
Real_Mode_Stack_Base equ 02000h

Real_Mode_Stack_Top equ 0400h


; ++++++++++++++++++++++++++++++++++++++++++++++++++
; 定义内核文件名称及加载位置
; kernel被loader加载时放置在0x70000~0x90000h
; 重新部署后分配的空间为0x10000h~0x70000h
; kernel代码入口为0x30000h

Kernel_Name   db   "kernel.bin"
Kernel_Name_Length equ $ - Kernel_Name

Kernel_Base equ 07000h
Kernel_Offset equ 0
Kernel_PLA	equ	Kernel_Base*0x10
Kernel_Entry_Point equ 010400h
; ++++++++++++++++++++++++++++++++++++++++++++++++++

; 页目录和页表基址
PageDir_Base		equ	0x100000
PageTable_Base		equ	0x101000

; 超级块内存地址(0x050h ~ 0x0d0h)

SuperBlock_Base   equ  050h
SuperBlock_Offset   equ   00h

; GroupDescriptions内存地址(0x090h ~ 0x0d0h)
GroupDescriptors_Base   equ 090h
GroupDescriptors_Offset equ 00h

; Inode内存地址(0x0d0h ~ 0x0110h)

InodeTable_Base	equ	0d0h		
InodeTable_Offset	equ 00h
; 根目录内存地址
Root_Dir_Base    equ 0110h
Root_Dir_Offset  equ 00h

;一级索引临时block存放区
First_Index_Block_Base    equ   0150h
First_Index_Block_Offset    equ 00h
;inode相关的数据结构
;长度为128字节
;记录数据block号的起始偏移为40
Inode_Length    equ 128
Inode_Blocks    equ 28
Inode_Block equ 40


; ELF文件相关的常量
ELFHeader_e_phnum      equ 0x2c
ELFHeader_e_phoff      equ 0x1c
ELFProgramHeader_p_size   equ 0x10
ELFProgramHeader_p_offset equ 0x4
ELFProgramHeader_p_vaddr  equ 0x8

; 记录目录项中的数据结构偏移
Inode_Number_Offset    equ 0
Record_Length_Offset   equ 4
Name_Len_Offset    equ 6
File_Type_Offset   equ 7
File_Name_Offset   equ 8