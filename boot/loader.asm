org 0x9000
mov ax, 0B800h
mov gs, ax
mov al, "L"
mov [gs:((80*10+39)*2)],ax

jmp $