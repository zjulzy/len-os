; ----------------------------------------------------------
; 记录boot.asm相关参数
; 包含超级块和inode table的位置
; ----------------------------------------------------------

Stack_Base equ 0x7c00

; loader加载的基址和偏移
Loader_Base equ 0x9000
Loader_Offset equ 0x00

; 超级块加载的内存地址
SuperBlock_Base	equ  0x070
SuperBlock_Offset equ 0x00

; GroupDescriptors目标内存地址(超级块本身不到512字节,不会冲突)
GroupDescriptors_Base equ 0x090
GroupDescriptors_Offset equ 0x00

; inode table 目标内存地址(60m的硬盘GroupDescriptors只有一个block)
InodeTable_Base	equ	0xd0
InodeTable_Offset equ 0x00

; 根目录目标内存地址(inode table目前我们只需要第一个block就行了,留下1k空间足够了)
RootDir_Base    equ 110h
RootDir_Offset  equ 00h

; inode相关的数据结构
; 长度为128字节
; 记录数据block号的起始偏移为40
Inode_Length    equ 128
Inode_Blocks    equ 28
Inode_Block equ 40