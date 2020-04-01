; -----------------------------------------------------------
; loader.asm相关常量
; -----------------------------------------------------------

; loader加载基址
Loader_Base	equ 09000h		 
; loader加载偏移
Loader_Offset equ 00h
; 根据段基址和偏移计算loader加载物理地址
Loader_PLA equ Loader_Base * 0x10 + Loader_Offset