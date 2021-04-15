
##############################
# Makefile for len-os                              #
##############################
# makefile中编译的中间文件和内核都会放在build/中

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

GCC = g++
LD = ld
INCLUDE_PATH = -I include/essential/ -I include/interrupt/  \
 -I include/iosystem/   -I include/process/

# boot和kernel汇编flag
BOOT_ASM_FLAG = -I boot/include
KERNEL_ASM_FLAG = -I kernel/ -f elf
OBJS_ASM_FLAG =-I kernel/ -f elf
# -c指定只编译不链接
# -fno-stack-protector指定不需要调用栈检查
C_FLAGS		= $(INCLUDE_PATH) -c -fno-builtin -fno-stack-protector -m32  -g -O0
LD_FLAGS  = -s -Ttext $(ENTRYPOINT) -m elf_i386 

# 使用gdb进行调试时，需要打开gcc -g并关闭ld -s选项，同时使用gdb在编译好的kernel.bin文件中设置断点
# 在bochsrc文件中设置使用gdb调试，开启bochs之后使用gdbgui等工具连接1234端口即可进行调试
LD_GDB_FLAGS = -Ttext $(ENTRYPOINT) -m elf_i386 

# 目标文件定义
LENBOOT= build/boot/boot.bin build/boot/loader.bin
LENKERNEL = build/kernel/kernel.bin
LENKERNELGDB = build/kernel/kernel_gdb.bin
#中间文件定义
OBJS = build/kernel/kernel.o build/kernel/kernel_cpp.o build/essential/base.o build/essential/display.o\
 			build/essential/global.o build/essential/memory.o \
			build/interrupt/interrupt.o build/interrupt/interrupt_asm.o build/essential/proto.o \
			build/interrupt/syscall.o build/interrupt/syscall_asm.o\
			build/iosystem/keyboard.o build/process/tty.o build/iosystem/console.o\
			build/process/process.o build/process/systask.o build/essential/type.o build/iosystem/harddrive.o build/filesystem/fs.o
KERNEL = build/kernel/kernel.bin
# 本makefile支持的所有操作
.PHONY : initialize everything clean buildimg realclean image disasm


# =======================================================================================
# make默认从此开始执行,使用bochs开始加载系统
# gdb使用调试用的内核，bochs运行的是不带符号表的内核
initialize : run gdb
	bochs -f bochsrc

# 删除所有文件
realclean : 
	rm -f $(OBJS) $(LENBOOT) $(LENKERNEL) $(LENKERNELGDB)

# 删除中间文件
clean:
	rm -f $(OBJS)
# 生成引导文件和内核文件
everything : $(LENBOOT) $(LENKERNEL)
# 生成镜像文件
image : everything realclean buildimg

run : $(LENKERNEL) $(LENBOOT)
	dd if=build/boot/boot.bin of=c.img bs=512 count=1 conv=notrunc 
	sudo mount ./c.img /mnt/floppy
	sudo cp build/boot/loader.bin /mnt/floppy
	sudo cp build/kernel/kernel.bin /mnt/floppy
	sudo umount /mnt/floppy


# 生成boot和loader===================================================================
build/boot/boot.bin: boot/boot.asm boot/include/boot.inc boot/include/Ext2.inc boot/include/boot_include.asm
	$(ASM) $(BOOT_ASM_FLAG) -o $@ $<
build/boot/loader.bin:boot/loader.asm boot/include/loader.inc boot/include/pm.inc boot/include/loader_include.asm
	$(ASM) $(BOOT_ASM_FLAG) -o $@ $<

# 生成内核=======================================================================
build/kernel/kernel.o : kernel/kernel.asm kernel/kernel.inc kernel/const.inc
	$(ASM)  $(KERNEL_ASM_FLAG) -o $@ $< 

build/kernel/kernel_cpp.o : kernel/kernel.cc
	$(GCC)  $(C_FLAGS) -o $@ $<

$(KERNEL):$(OBJS)
	$(LD)  $(LD_FLAGS) -o $@ $^

# 生成供调试的内核，含有符号表
gdb : $(OBJS)
	$(LD)  $(LD_GDB_FLAGS) -o $(LENKERNELGDB) $^

# 生成中间文件----------------------------------------------------------------

build/essential/global.o :lib/essential/global.cc
	$(GCC) $(C_FLAGS) -o $@ $<


build/essential/memory.o: lib/essential/memory.asm
	$(ASM) $(OBJS_ASM_FLAG) -o $@ $<

build/essential/display.o :lib/essential/display.asm
	$(ASM) $(OBJS_ASM_FLAG) -o $@ $<

build/essential/base.o : lib/essential/base.cc
	$(GCC) $(C_FLAGS) -o $@ $<

build/interrupt/interrupt.o:lib/interrupt/interrupt.cc
	$(GCC) $(C_FLAGS) -o $@ $<

build/essential/proto.o : lib/essential/proto.asm
	$(ASM) $(OBJS_ASM_FLAG) -o $@ $<

build/process/process.o : lib/process/process.cc
	$(GCC) $(C_FLAGS) -o $@ $<

build/interrupt/interrupt_asm.o: lib/interrupt/interrupt.asm
	$(ASM) $(OBJS_ASM_FLAG) -o $@ $<

build/interrupt/syscall.o : lib/interrupt/syscall.cc
	$(GCC) $(C_FLAGS) -o $@ $<

build/interrupt/syscall_asm.o : lib/interrupt/syscall.asm
	$(ASM) $(OBJS_ASM_FLAG) -o $@ $<

build/iosystem/keyboard.o : lib/iosystem/keyboard.cc
	$(GCC) $(C_FLAGS) -o $@ $<

build/process/tty.o:lib/process/tty.cc
	$(GCC) $(C_FLAGS) -o $@ $<

build/iosystem/console.o : lib/iosystem/console.cc
	$(GCC) $(C_FLAGS) -o $@ $<

build/process/systask.o:lib/process/systask.cc
	$(GCC) $(C_FLAGS) -o $@ $<

build/essential/type.o :lib/essential/type.cc
	$(GCC) $(C_FLAGS) -o $@ $<

build/iosystem/harddrive.o:lib/iosystem/harddrive.cc
	$(GCC) $(C_FLAGS) -o $@ $<

build/filesystem/fs.o:lib/filesystem/fs.cc:
	$(GCC) $(C_FLAGS) -o $@ $<

