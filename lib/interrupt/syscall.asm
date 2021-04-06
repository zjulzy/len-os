%include "const.inc"

INDEX_GET_TICKS equ 0
INDEX_WRITE equ 1
INDEX_IPC_SEND equ 2
INDEX_IPC_RECEIVE equ 3
INT_VECTOR_SYS_CALL equ 0x90
bits 32
[SECTION .text]
extern p_proc_ready
; global get_ticks 
global write
global ipc
; get_ticks:
;     mov eax,INDEX_GET_TICKS
;     int INT_VECTOR_SYS_CALL
;     ret

write:
    mov eax,INDEX_WRITE
    mov ebx,[esp+4]
    mov ecx,[esp+8]
    mov edx,dword [p_proc_ready]
    int INT_VECTOR_SYS_CALL
    ret


ipc:
    mov eax,[esp+4]
    mov ebx,dword [p_proc_ready]
    mov ecx,[esp+8]
    mov edx,[esp+12]
    int INT_VECTOR_SYS_CALL
    ret

    