; --------------------------------------------------------------------
; 由于一个扇区大小有限制，故选择在loader中进入保护模式
; boot引导程序，在ext2硬盘中寻找loader.bin
; 找到后将loader.bin加载到内存中并将控制权交给loader
; --------------------------------------------------------------------

org	07c00h                      ;提醒bios将引导加载到0x07c00处，由BIOS决定，与cpu无关，07c00即32KB最早IBM的PC
jmp boot

%include "boot.inc"
;Ext2磁盘相关信息
%include "Ext2.inc"
%include "boot_load.asm"


    ;硬盘数据结构
    ;规定loader名称
    Loader_Target db "loader.bin"
    Loader_Length equ $-Loader_Target

boot:
    ;读取栈底地址
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, Stack_Base

    ;输出配置信息
    call  clear_screen
    mov dh, 0
    call  display_str
    call  sector_reader            ;读取超级块

    ; mov word 初始化读取磁盘使用的数据结构
    mov word	[disk_address_packet + 4],	GroupDescriptors_Offset
	mov	word	[disk_address_packet + 6],	GroupDescriptors_Base
	mov	dword	[disk_address_packet + 8],	GroupDescriptors_LBA_L
	mov	dword	[disk_address_packet + 12],	GroupDescriptors_LBA_H
	call sector_reader


    ; 对inode结构体的操作
    ;inode是文件对应的索引节点,存放文件元数据,存储在磁盘的inode table中
    mov eax , 2
    ; ext2 inode结构体可以理解为权限+block定位,共描述15个block;
    ; 其中0到11个是direct block,第十二个是indirect block
    ; 第十三个是double block,第十四个是triple block
    call inode_loader

    ; 根据inode读取根目录

    mov word [disk_address_packet + 4], RootDir_Offset
    mov word [disk_address_packet + 6], RootDir_Base
    mov eax , [es:bx+Inode_Blocks]
    add eax , eax
    mov dword [disk_address_packet + 8], eax
    mov dword [disk_address_packet+ 12],0        ;高32位为0

    mov	eax, [es:bx + Inode_Blocks]
	shr eax, 1
	mov	cx,	ax
	cmp cx, 12
	jle root_loader
	mov cx, 12
; ----------------------------------------------------------------------------
; 根目录加载操作
; ----------------------------------------------------------------------------
root_loader:
    cmp	cx, 0
	je	loader_not_found
	call	sector_reader
	dec cx
	add bx,4
	mov	eax,[es:bx]
	add eax,eax
	mov	dword	[disk_address_packet + 8],	eax

; 在当前block搜索loader
root_search:
    pusha
	mov si,Loader_Name
	mov eax, RootDir_Base
	mov gs,eax
	mov bx, RootDir_Offset
; 对比名字寻找loader
loader_cmp:
    xor cx,cx
	mov cl, Loader_Name_Length
	cmp cl, byte [gs:bx+Name_Len_Offset]
	jnz file_not_match
	mov si,Loader_Name
	push bx
    ; lodsb指令将ds:si的一个字节装入al中,
    ; 并根据DF标志位增减si
loop_cmp:
    lodsb
    ; 以bx作为基址寄存器
    cmp al,  byte[gs:bx+File_Name_Offset]
	jnz cmp_result
	dec cl
	jz cmp_result
	inc bx
	jmp loop_cmp
cmp_result:
    pop bx
	cmp cl, 0
	jnz loader_not_match
	jmp loader_found

loader_not_match:
    add bx, word [gs:bx+Record_Length_Offset]
	cmp bx,1024
	jl root_search
	popa
	jmp root_loader

loader_not_found:
	mov	dh,	2
	call disp_str
	jmp	$

loader_found:
	;这个堆栈以后就不用了,没必要恢复栈顶指针
	mov	dh,	1
	call	disp_str

	; 好了终于能加载Loader了!!!!!!!
	; 此时寄存器情况
	; gs:bx => 当前目录项的指针
	mov eax, dword [gs:bx + Inode_Number_Offset]
	call get_inode
	
	mov word	[disk_address_packet + 4],	Loader_Offset
	mov	word	[disk_address_packet + 6],	Loader_Base
	
	call	loade_file

	jmp	Loader_Base:Loader_Offset





times 	510-($-$$)	db	0					; 填充剩下的空间，使生成的二进制代码恰好为512字节
dw 	0xaa55											; 结束标志