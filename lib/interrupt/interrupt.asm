%include "const.inc"
[SECTION .text]
; 设置i8259中断屏蔽和开启，摘自minix
global enable_irq
global disable_irq
; ========================================================================
;                  void disable_irq(int irq);
; ========================================================================
; Disable an interrupt request line by setting an 8259 bit.
; Equivalent code:
;	if(irq < 8)
;		out_byte(INT_MASTER_CTLMASK, in_byte(INT_MASTER_CTLMASK) | (1 << irq));
;	else
;		out_byte(INT_SLAVE_CTLMASK, in_byte(INT_SLAVE_CTLMASK) | (1 << irq));
disable_irq:
        mov     ecx, [esp + 4]          ; irq
        pushf
        cli
        mov     ah, 1
        rol     ah, cl                  ; ah = (1 << (irq % 8))
        cmp     cl, 8
        jae     disable_8               ; disable irq >= 8 at the slave 8259
disable_0:
        in      al, INT_MASTER_CTLMASK
        test    al, ah
        jnz     dis_already             ; already disabled?
        or      al, ah
        out     INT_MASTER_CTLMASK, al       ; set bit at master 8259
        popf
        mov     eax, 1                  ; disabled by this function
        ret
disable_8:
        in      al, INT_SLAVE_CTLMASK
        test    al, ah
        jnz     dis_already             ; already disabled?
        or      al, ah
        out     INT_SLAVE_CTLMASK, al       ; set bit at slave 8259
        popf
        mov     eax, 1                  ; disabled by this function
        ret
dis_already:
        popf
        xor     eax, eax                ; already disabled
        ret

; ========================================================================
;                  void enable_irq(int irq);
; ========================================================================
; Enable an interrupt request line by clearing an 8259 bit.
; Equivalent code:
;       if(irq < 8)
;               out_byte(INT_MASTER_CTLMASK, in_byte(INT_MASTER_CTLMASK) & ~(1 << irq));
;       else
;               out_byte(INT_SLAVE_CTLMASK, in_byte(INT_SLAVE_CTLMASK) & ~(1 << irq));
;
enable_irq:
    mov     ecx, [esp + 4]          ; irq
    pushf
    cli
    mov     ah, ~1
    rol     ah, cl                  ; ah = ~(1 << (irq % 8))
    cmp     cl, 8
    jae     enable_8                ; enable irq >= 8 at the slave 8259
enable_0:
    in      al, INT_MASTER_CTLMASK
    and     al, ah
    out     INT_MASTER_CTLMASK, al       ; clear bit at master 8259
    popf
    ret
enable_8:
    in      al, INT_SLAVE_CTLMASK
    and     al, ah
    out     INT_SLAVE_CTLMASK, al       ; clear bit at slave 8259
    popf
    ret