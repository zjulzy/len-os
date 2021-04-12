;========================================
;	内存操作函数
;========================================

global out_byte
global in_byte
global port_in
global port_out

; ========================================================================
;                  void out_byte(u16 port, u8 value);
; ========================================================================
out_byte:
	mov	edx, [esp + 4]		; port
	mov	al, [esp + 4 + 4]	; value
	out	dx, al
	nop	; 一点延迟
	nop
	ret

; ========================================================================
;                  u8 in_byte(u16 port);
; ========================================================================
in_byte:
	mov	edx, [esp + 4]		; port
	xor	eax, eax
	in	al, dx
	nop	; 一点延迟
	nop
	ret

; ========================================================================
;                  void port_in(u16 port, void* buf, int n);
; ========================================================================
port_in:
	mov	edx, [esp + 4]		; port
	mov	edi, [esp + 8]	; buf
	mov	ecx, [esp + 12]	; n
	shr	ecx, 1
    ; DF置0
	cld
    ; 重复执行insw
	rep	insw
	ret

; ========================================================================
;                  void port_out(u16 port, void* buf, int n);
; ========================================================================
port_out:
	mov	edx, [esp + 4]		; port
	mov	esi, [esp + 8]	; buf
	mov	ecx, [esp + 12]	; n
	shr	ecx, 1
	cld
	rep	outsw
	ret