
##############################
# Makefile for len-os                              #
##############################

# kernel重新放置后的位置
# 与/boot/include/loader.inc中保持一致
ENTRYPOINT = 0X10000
# kernel入口偏移
# 在对kernel.bin进行反汇编的时候会用到
# 取决于 ENTRYPOINT
ENTRYOFFSET	=   0x400
# 指令变量宏定义
ASM = nasm
DASM = ndisasm

GCC = gcc
LD = ld
INCLUDE_PATH = -I include/essential/ -l include/display
# boot和kernel汇编flag
BOOT_ASM_FLAG = -I boot/include
KERNEL_ASM_FLAG = -I kernel/include
C_FLAGS		= $(INCLUDE_PATH) -c -fno-builtin -m32 -fno-stack-protector
# 目标文件定义
LENBOOT= boot/boot.bin boot/loader.bin
LENKERNEL = kernel/kernel.bin
#中间文件定义
OBJS = kernel/kernel.o  lib/essential/proto.os lib/interrupt/intertupt.o lib/interrupt/interrupt_option.o
# 本makefile支持的所有操作
.PHONY : initialize everything clean buildimg realclean image disasm

# make默认从此开始执行,使用bochs开始加载系统
initialize : image
	bochs -f bochsrc

# 删除所有文件
realclean : 
	rm -f $(OBJ) $(LENBOOT) $(LENKERNEL)

# 删除中间文件
clean:
	rm -f $(OBJS)
# 生成引导文件和内核文件
everything : $(LENBOOT) $(LENKERNEL)
# 生成镜像文件
image : everything clean buildimg

buildimg : boot/boot.bin  boot/loader.bin kernel/kernel.bin
	dd if=boot/boot.bin of=c.img bs=512 count=1 conv=notrunc 
	sudo mount ./c.img /mnt/floppy
	sudo cp boot/loader.bin /mnt/floppy
	sudo cp kernel/kernel.bin /mnt/floppy
	sudo umount /mnt/floppy

# 生成boot和loader
boot/boot.bin: boot/boot.asm boot/include/boot.inc boot/include/Ext2.inc boot/include/boot_include.asm
	$(ASM) $(BOOT_ASM_FLAG) -o $@ $<
boot/loader.bin:boot/loader.asm boot/include/loader.inc boot/include/pm.inc boot/include/loader_include.asm
	$(ASM) $(BOOT_ASM_FLAG) -o $@ $<

lib/essential/proto.o: lib/essential/proto.asm
	$(ASM) $(KERNEL_ASM_FLAG ) -o $@ $<