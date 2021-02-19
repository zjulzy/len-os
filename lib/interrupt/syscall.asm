%include "const.inc"

INDEX_GET_TICKS equ 0
INT_VECTOR_SYS_CALL equ 0x90
bits 32
[SECTION .text]
global get_ticks 

get_ticks:
    mov eax,INDEX_GET_TICKS
    int INT_VECTOR_SYS_CALL
    ret