; +++++++++++++++++++++++++++++++++++++++++++++++++++++++
; loader.asm
; 由boot引导加载入内存后，进行以下步骤：
; １．获取内存信息
; ２．搜索加载kernrl.bin
; ３．由实模式进入保护模式
; ４．启动内存分页
; ５．重新部署kernel
; ６．跳转进入kernel
; +++++++++++++++++++++++++++++++++++++++++++++++++++++++

jmp loader_initial

;载入配置信息
%include "pm.inc"
%include "loader.inc"
%include "gdt.asm"

; **********************************************************************
; GDT相关数据结构
; GDT描述符表
;                                                                          段基址               段界限         属性
LABEL_GDT:			Descriptor            				   0,                    	0, 					0																	; 空描述符
LABEL_DESC_FLAT_C:		Descriptor  		  0,              0fffffh, 			DA_CR  | DA_32 | DA_LIMIT_4K				; 0 ~ 4G(靠偏移寻址)
LABEL_DESC_FLAT_RW:	 Descriptor             0,              0fffffh, 			DA_DRW | DA_32 | DA_LIMIT_4K			; 0 ~ 4G(靠偏移寻址)
LABEL_DESC_VIDEO:		 Descriptor	 0B8000h,              0ffffh, 			DA_DRW | DA_DPL3									; 显存首地址
GdtLen		equ	$ - LABEL_GDT                        ;GDT长度

GdtPtr:
		dw	GdtLen - 1														; GDT段界限
		dd	Loader_Phy_Address + LABEL_GDT		  ; GDT段基址


; **********************************************************************