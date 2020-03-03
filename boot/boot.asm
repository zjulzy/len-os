; --------------------------------------------------------------------
; boot引导程序，在ext2硬盘中寻找loader.bin
; 找到后将loader.bin加载到内存中并将控制权交给loader
; --------------------------------------------------------------------

org	07c00h                               ;提醒bios将引导加载到0x07c00处，由BIOS决定，与cpu无关，07c00即32KB最早IBM的PC
jmp boot_start
%include "boot.inc"
%include "Ext2.inc"
%include "boot_load.asm"