; --------------------------------------------------------------------
; boot引导程序，在ext2硬盘中寻找loader.bin
; 找到后将loader.bin加载到内存中并将控制权交给loader
; --------------------------------------------------------------------

org	07c00h                               ;提醒bios将引导加载到0x07c00处，由BIOS决定，与cpu无关，07c00即32KB最早IBM的PC
jmp boot

%include "boot.inc"
;Ext2磁盘相关信息
%include "Ext2.inc"
%include "boot_load.asm"

    ;硬盘数据结构
    ;规定loader名称
    Loader_Target db "loader.bin"
    Loader_Length equ $-Loader_Target

boot:
    ;读取栈底地址
    mov ax, cs
    mov ds, cs
    mov es ,cs
    mov ss, cs
    mov sp, Stack_Base

    ;输出配置信息
    call  clear_screen
    mov dh, 0
    call  display_str
    call  sector_reader

