
##############################
# Makefile for len-os                              #
##############################

# kernel重新放置后的位置
# 与/boot/include/loader.inc中保持一致
ENTRYPOINT = 0X10400
# kernel入口偏移
# 在对kernel.bin进行反汇编的时候会用到
# 取决于 ENTRYPOINT
ENTRYOFFSET	=   0x400
# 指令变量宏定义
ASM = nasm
DASM = ndisasm

GCC = gcc
LD = ld
INCLUDE_PATH = -I include/essential/ -I include/interrupt/  \
 -I include/iosystem/   -I include/process/

# boot和kernel汇编flag
BOOT_ASM_FLAG = -I boot/include
KERNEL_ASM_FLAG = -I kernel/ -f elf
OBJS_ASM_FLAG = -f elf
# -c指定只编译不链接
# -fno-stack-protector指定不需要调用栈检查
C_FLAGS		= $(INCLUDE_PATH) -c -fno-builtin -fno-stack-protector -m32  -g
LD_FLAGS  = -s -Ttext $(ENTRYPOINT) -m elf_i386 

# 使用gdb进行调试时，需要打开gcc -g并关闭ld -s选项，同时使用gdb在编译好的kernel.bin文件中设置断点
# 在bochsrc文件中设置使用gdb调试，开启bochs之后使用gdbgui等工具连接1234端口即可进行调试
LD_GDB_FLAGS = -Ttext $(ENTRYPOINT) -m elf_i386 

# 目标文件定义
LENBOOT= boot/boot.bin boot/loader.bin
LENKERNEL = kernel/kernel.bin
#中间文件定义
OBJS = kernel/kernel.o kernel/kernel_cpp.o lib/essential/base.o lib/essential/display.o lib/essential/global.o lib/essential/memory.o \
			lib/interrupt/interrupt.o lib/essential/proto.o
KERNEL = kernel/kernel.bin
# 本makefile支持的所有操作
.PHONY : initialize everything clean buildimg realclean image disasm

# make默认从此开始执行,使用bochs开始加载系统
initialize : image
	bochs -f bochsrc

# 删除所有文件
realclean : 
	rm -f $(OBJS) $(LENBOOT) $(LENKERNEL)

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

# 生成内核
kernel/kernel.o : kernel/kernel.asm kernel/kernel.inc
	$(ASM)  $(KERNEL_ASM_FLAG) -o $@ $< 

kernel/kernel_cpp.o : kernel/kernel.cc
	$(GCC)  $(C_FLAGS) -o $@ $<

$(KERNEL):$(OBJS)
	$(LD)  $(LD_FLAGS) -o $@ $^

# 生成中间文件

lib/essential/global.o :lib/essential/global.cc
	$(GCC) $(C_FLAGS) -o $@ $<

lib/essential/proto.o: lib/essential/proto.asm
	$(ASM) $(KERNEL_ASM_FLAG ) -o $@ $<

lib/essential/memory.o: lib/essential/memory.asm
	$(ASM) $(OBJS_ASM_FLAG) -o $@ $<

lib/essential/display.o :lib/essential/display.asm
	$(ASM) $(OBJS_ASM_FLAG) -o $@ $<

lib/essential/base.o : lib/essential/base.cc
	$(GCC) $(C_FLAGS) -o $@ $<

lib/interrupt/interrupt.o:lib/interrupt/interrupt.cc
	$(GCC) $(C_FLAGS) -o $@ $<

lib/essential/proto.o : lib/essential/proto.asm
	$(ASM) $(OBJS_ASM_FLAG) -o $@ $<

