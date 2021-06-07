extern main
extern exit 
global _start
bits 32
[SECTION .text]
_start:
    push eax 
    push ecx 
    call main 
    push eax 
    call exit 
    hlt