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