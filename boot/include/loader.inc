; -----------------------------------------------------------
; loader.asm相关常量
; -----------------------------------------------------------

; loader加载基址
LoaderBase	equ 0x9000h		 
; loader加载偏移
LoaderOffset equ 00h
; 根据段基址和偏移计算loader加载物理地址
LoaderPLA equ Loader_Base * 0x10 + Loader_Offset

; 实模式下的堆栈定义为0x20000h~0x20400h
RealModeStackBase equ 0x2000h

RealModeStackTop equ 0x400h


; ++++++++++++++++++++++++++++++++++++++++++++++++++
; 定义内核文件名称及加载位置
; kernel被loader加载时放置在0x80000~0x90000h
; 重新部署后分配的空间为0x30000h~0x80000h
; kernel代码入口为0x30000h

KernelName   db   "kernel.bin"
KernelNameLength equ $ - Kernel_Name
KernelPLA	equ	0x70000h
KernelEntrypoint equ 0x30000h
; ++++++++++++++++++++++++++++++++++++++++++++++++++

; 页目录和页表基址
PageDirBase		equ	0x100000
PageTableBASE		equ	0x101000

; 超级块内存地址(0x050h ~ 0x0d0h)

SuperBlockBase   equ  0x50h
SuperBlockIffset   equ   00h

; GroupDescriptions内存地址(0x090h ~ 0x0d0h)
GroupDescriptorsBase   equ 090h
GroupDescriptorsOffset equ 00h

; Inode内存地址(0x0d0h ~ 0x0110h)

InodeTableBase	equ	0d0h		
InodeTableOffset	equ 00h
; 根目录内存地址
RootDirBase    equ 0110h
RootDirOffset  equ 00h

;一级索引临时block存放区
FirstIndexBlockBase    equ   0150h
FirstIndexBlockOffset    equ 00h
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
InodeNumberOffset    equ 0
RecordLengthOffset   equ 4
NameLenOffset    equ 6
FileTypeOffset   equ 7
FileNameOffset   equ 8