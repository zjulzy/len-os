extern disp_pos //导入显示位置,定义在globe.h

[SECTION .text]
;导出显示函数
global	disp_str
; =============================================================
; void disp_str(char *info)
; 显示一个字符串
; 字符串起始通过堆栈传递,结尾使用0来表示
; =============================================================


disp_str:
	push	ebp
	mov	ebp, esp
	push	ebx
	push	esi
	push	edi

	mov	esi, [ebp + 8]
	mov	edi, [disp_pos]
	mov	ah, 0Fh

.1:
	lodsb
	test	al, al					;test指令对符号位的影响和and指令相同
	jz	.2			                    ; 检测是否为0,为0则字符串结束

	cmp	al, 0Ah					; 判断当前字符是否为回车
	jnz	.3

	push	eax
	mov	eax, edi
	mov	bl, 160
	div	bl
	and	eax, 0FFh
	inc	eax

	; 实现滚屏操作,如果越界则回滚到第一行
	cmp eax, 20
	jbe .4
	xor eax,eax
.4:
	mov	bl, 160
	mul	bl
	mov	edi, eax
	pop	eax
	jmp	.2

.3:
	mov	[gs:edi], ax
	add	edi, 2
	jmp	.1
//显示结束
.2:
	mov	[disp_pos], edi

	pop	edi
	pop	esi
	pop	ebx
	pop	ebp
	ret