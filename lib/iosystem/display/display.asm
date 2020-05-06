extern disp_pos //导入显示位置,定义在globe.h

[SECTION .text]
;导出显示函数
    global	disp_str
    global  disp_char
    global  disp_int
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
; ==============================================================
; void disp_char(char ch)
; 通过堆栈传递要显示的ascii码
; ==============================================================

disp_char:
    mov eax , [esp+4]
    moc edi  ,[disp_pos]
    cmp  al, 0dh
    jnz  .1

    ;显示可见字符
    mov	eax, edi
	mov	bl, 160
	div	bl
    and eax 0FFh
    inc  eax
    mov  bl  160
    mul bl 
    mov  edi , eax 
    mov  [disp_pos] , edi 
    jmp  .3
;处理回车
.1:             
    cmp  al, 08h
    jnz  .2

    sub  edi , 2
    mov  ah, 0Fh
    mov  al, 020h
    mov  [gs:edi] , ax
    mov  [disp_pos] , edi 
    jmp  .3
;处理退格符
.2:
    mov	ah, 0Fh
	mov	[gs:edi], ax

	; 修改指针位置
	add	edi, 2
	mov	[disp_pos], edi
;打印结束
.3:
    ret
; ================================================================
; disp_int
; 打印一个整数
; ================================================================
