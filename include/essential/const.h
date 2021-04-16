//宏定义
#ifndef LENOS_CONST_H
#define LENOS_CONST_H

//idt相关宏定义=========================================
//系统调用中断向量
#define INT_VECTOR_SYS_CALL 0X90
//异常中断向量
#define INT_VECTOR_DIVIDE 0x0
#define INT_VECTOR_DEBUG 0x1
#define INT_VECTOR_NMI 0x2
#define INT_VECTOR_BREAKPOINT 0x3
#define INT_VECTOR_OVERFLOW 0x4
#define INT_VECTOR_BOUNDS 0x5
#define INT_VECTOR_INVAL_OP 0x6
#define INT_VECTOR_COPROC_NOT 0x7
#define INT_VECTOR_DOUBLE_FAULT 0x8
#define INT_VECTOR_COPROC_SEG 0x9
#define INT_VECTOR_INVAL_TSS 0xA
#define INT_VECTOR_SEG_NOT 0xB
#define INT_VECTOR_STACK_FAULT 0xC
#define INT_VECTOR_PROTECTION 0xD
#define INT_VECTOR_PAGE_FAULT 0xE
#define INT_VECTOR_COPROC_ERR 0x10
//8259A芯片端口定义
#define INT_M_CTL 0x20
#define INT_M_CTLMASK 0x21
#define INT_S_CTL 0xA0
#define INT_S_CTLMASK 0xA1
//8259A中断入口
#define INT_VECTOR_IRQ0 0x20
#define INT_VECTOR_IRQ8 0x28
//8253相关定义
#define TIMER0 0x40
#define TIMER_MODE 0x43
#define RATE_GENERATOR 0x34
#define TIMER_FREQ 1193182L
#define HZ 100
//权限
#define PRIVILEGE_KRNL 0
#define PRIVILEGE_TASK 1
#define PRIVILEGE_USER 3
//RPL
#define RPL_KRNL SA_RPL0
#define RPL_TASK SA_RPL1
#define RPL_USER SA_RPL3
//===================================================
//定义函数类型
#define PUBLIC
#define PRIVATE static

// GDT 和 IDT 中描述符的个数
#define GDT_SIZE 128
#define IDT_SIZE 256
//每个进程允许使用的ldt数目
#define LDT_SIZE 2
//定义任务以及用户进程数目
#define NR_TASK 4
#define NR_USER_PROCESS 3
//===========================================================
//GDT相关
//描述符索引,用于在c文件中确定选择子对应的描述符数组下标
//部分由loader确定
#define INDEX_DUMMY 0
#define INDEX_FLAT_C 1
#define INDEX_FLAT_RW 2
#define INDEX_VIDEO 3
#define INDEX_TSS 4
#define INDEX_LDT_FIRST 5
//选择子
#define SELECTOR_DUMMY 0
#define SELECTOR_FLAT_C 0x08
#define SELECTOR_FLAT_RW 0x10
#define SELECTOR_VIDEO (0x18 + 3)
#define SELECTOR_TSS 0x20
#define SELECTOR_LDT_FIRST 0x28

#define SELECTOT_KERNEL_C SELECTOR_FLAT_C
#define SELECTOR_KERNEL_RW SELECTOR_FLAT_RW
#define SELECTOR_KERNEL_GS SELECTOR_VIDEO

/* 每个任务有一个单独的 LDT, 每个 LDT 中的描述符个数: */
#define LDT_SIZE 2
/* descriptor indices in LDT */
#define INDEX_LDT_C 0
#define INDEX_LDT_RW 1
//===========================================================
//线性地址转为物理地址
#define vir2phy(seg_base, vir) (u32)(((u32)seg_base) + (u32)vir)

// 系统段描述符类型值说明
#define DA_LDT 0x82      /* 局部描述符表段类型值			*/
#define DA_TaskGate 0x85 /* 任务门类型值   */
#define DA_386TSS 0x89   /* 可用 386 任务状态段类型值		*/
#define DA_386CGate 0x8C /* 386 调用门类型值			*/
#define DA_386IGate 0x8E /* 386 中断门类型值			*/
#define DA_386TGate 0x8F /* 386 陷阱门类型值			*/

// 存储段描述符类型值说明
#define DAD_R 0x90    /* 存在的只读数据段类型值		*/
#define DAD_RW 0x92   /* 存在的可读写数据段属性值		*/
#define DAD_RWA 0x93  /* 存在的已访问可读写数据段类型值	*/
#define DAC_E 0x98    /* 存在的只执行代码段属性值		*/
#define DAC_ER 0x9A   /* 存在的可执行可读代码段属性值		*/
#define DAC_ECO 0x9C  /* 存在的只执行一致代码段属性值 */
#define DAC_ECOR 0x9E /* 存在的可执行可读一致代码段属性值	*/

//选择子属性定义
#define SA_RPL_MASK 0xFFFC
#define SA_RPL0 0
#define SA_RPL1 1
#define SA_RPL2 2
#define SA_RPL3 3

#define SA_TI_MASK 0xFFFB
#define SA_TIG 0
#define SA_TIL 4

//定义中断芯片相关的硬件中断
#define NR_IRQ 16 /* Number of IRQs */
#define CLOCK_IRQ 0
#define KEYBOARD_IRQ 1
#define CASCADE_IRQ 2   /* cascade enable for 2nd AT controller */
#define ETHER_IRQ 3     /* default ethernet interrupt vector */
#define SECONDARY_IRQ 3 /* RS232 interrupt vector for port 2 */
#define RS232_IRQ 4     /* RS232 interrupt vector for port 1 */
#define XT_WINI_IRQ 5   /* xt winchester */
#define FLOPPY_IRQ 6    /* floppy disk */
#define PRINTER_IRQ 7
#define AT_WINI_IRQ 14 /* at winchester */

//多级反馈队列进程调度时间片限制
#define FIRST_QUENE_SLICE 2
#define SECOND_QUENE_SLICE 4
#define LAST_QUENE_SLICE 8
// tty最大数量.
#define NR_CONSOLE 3
// tty最大输入长度
#define TTY_IN_BYTES 256

#define SCREEN_SIZE (80 * 25)
#define SCREEN_WIDTH 80
//键盘相关io宏定义=============================================================================
/* AT keyboard */
/* 8042 ports */
#define KB_DATA 0x60 /* I/O port for keyboard data \
             Read : Read Output Buffer             \
             Write: Write Input Buffer(8042 Data&8048 Command) */
#define KB_CMD 0x64  /* I/O port for keyboard command \
             Read : Read Status Register              \
             Write: Write Input Buffer(8042 Command) */

#define KB_IN_BYTES 32     /* size of keyboard input buffer */
#define MAP_COLS 3         /* Number of columns in keymap */
#define NR_SCAN_CODES 0x80 /* Number of scan codes (rows in keymap) */

#define FLAG_BREAK 0x0080   /* Break Code			*/
#define FLAG_EXT 0x0100     /* Normal function keys		*/
#define FLAG_SHIFT_L 0x0200 /* Shift key			*/
#define FLAG_SHIFT_R 0x0400 /* Shift key			*/
#define FLAG_CTRL_L 0x0800  /* Control key			*/
#define FLAG_CTRL_R 0x1000  /* Control key			*/
#define FLAG_ALT_L 0x2000   /* Alternate key		*/
#define FLAG_ALT_R 0x4000   /* Alternate key		*/
#define FLAG_PAD 0x8000     /* keys in num pad		*/

#define MASK_RAW 0x01FF /* raw key value = code passed to tty & MASK_RAW \
               the value can be found either in the keymap column 0      \
               or in the list below */

/* Special keys */
#define ESC (0x01 + FLAG_EXT)       /* Esc		*/
#define TAB (0x02 + FLAG_EXT)       /* Tab		*/
#define ENTER (0x03 + FLAG_EXT)     /* Enter	*/
#define BACKSPACE (0x04 + FLAG_EXT) /* BackSpace	*/

#define GUI_L (0x05 + FLAG_EXT) /* L GUI	*/
#define GUI_R (0x06 + FLAG_EXT) /* R GUI	*/
#define APPS (0x07 + FLAG_EXT)  /* APPS	*/

/* Shift, Ctrl, Alt */
#define SHIFT_L (0x08 + FLAG_EXT) /* L Shift	*/
#define SHIFT_R (0x09 + FLAG_EXT) /* R Shift	*/
#define CTRL_L (0x0A + FLAG_EXT)  /* L Ctrl	*/
#define CTRL_R (0x0B + FLAG_EXT)  /* R Ctrl	*/
#define ALT_L (0x0C + FLAG_EXT)   /* L Alt	*/
#define ALT_R (0x0D + FLAG_EXT)   /* R Alt	*/

/* Lock keys */
#define CAPS_LOCK (0x0E + FLAG_EXT)   /* Caps Lock	*/
#define NUM_LOCK (0x0F + FLAG_EXT)    /* Number Lock	*/
#define SCROLL_LOCK (0x10 + FLAG_EXT) /* Scroll Lock	*/

/* Function keys */
#define F1 (0x11 + FLAG_EXT)  /* F1		*/
#define F2 (0x12 + FLAG_EXT)  /* F2		*/
#define F3 (0x13 + FLAG_EXT)  /* F3		*/
#define F4 (0x14 + FLAG_EXT)  /* F4		*/
#define F5 (0x15 + FLAG_EXT)  /* F5		*/
#define F6 (0x16 + FLAG_EXT)  /* F6		*/
#define F7 (0x17 + FLAG_EXT)  /* F7		*/
#define F8 (0x18 + FLAG_EXT)  /* F8		*/
#define F9 (0x19 + FLAG_EXT)  /* F9		*/
#define F10 (0x1A + FLAG_EXT) /* F10		*/
#define F11 (0x1B + FLAG_EXT) /* F11		*/
#define F12 (0x1C + FLAG_EXT) /* F12		*/

/* Control Pad */
#define PRINTSCREEN (0x1D + FLAG_EXT) /* Print Screen	*/
#define PAUSEBREAK (0x1E + FLAG_EXT)  /* Pause/Break	*/
#define INSERT (0x1F + FLAG_EXT)      /* Insert	*/
#define DELETE (0x20 + FLAG_EXT)      /* Delete	*/
#define HOME (0x21 + FLAG_EXT)        /* Home		*/
#define END (0x22 + FLAG_EXT)         /* End		*/
#define PAGEUP (0x23 + FLAG_EXT)      /* Page Up	*/
#define PAGEDOWN (0x24 + FLAG_EXT)    /* Page Down	*/
#define UP (0x25 + FLAG_EXT)          /* Up		*/
#define DOWN (0x26 + FLAG_EXT)        /* Down		*/
#define LEFT (0x27 + FLAG_EXT)        /* Left		*/
#define RIGHT (0x28 + FLAG_EXT)       /* Right	*/

/* ACPI keys */
#define POWER (0x29 + FLAG_EXT) /* Power	*/
#define SLEEP (0x2A + FLAG_EXT) /* Sleep	*/
#define WAKE (0x2B + FLAG_EXT)  /* Wake Up	*/

/* Num Pad */
#define PAD_SLASH (0x2C + FLAG_EXT) /* /		*/
#define PAD_STAR (0x2D + FLAG_EXT)  /* *		*/
#define PAD_MINUS (0x2E + FLAG_EXT) /* -		*/
#define PAD_PLUS (0x2F + FLAG_EXT)  /* +		*/
#define PAD_ENTER (0x30 + FLAG_EXT) /* Enter	*/
#define PAD_DOT (0x31 + FLAG_EXT)   /* .		*/
#define PAD_0 (0x32 + FLAG_EXT)     /* 0		*/
#define PAD_1 (0x33 + FLAG_EXT)     /* 1		*/
#define PAD_2 (0x34 + FLAG_EXT)     /* 2		*/
#define PAD_3 (0x35 + FLAG_EXT)     /* 3		*/
#define PAD_4 (0x36 + FLAG_EXT)     /* 4		*/
#define PAD_5 (0x37 + FLAG_EXT)     /* 5		*/
#define PAD_6 (0x38 + FLAG_EXT)     /* 6		*/
#define PAD_7 (0x39 + FLAG_EXT)     /* 7		*/
#define PAD_8 (0x3A + FLAG_EXT)     /* 8		*/
#define PAD_9 (0x3B + FLAG_EXT)     /* 9		*/
#define PAD_UP PAD_8                /* Up		*/
#define PAD_DOWN PAD_2              /* Down		*/
#define PAD_LEFT PAD_4              /* Left		*/
#define PAD_RIGHT PAD_6             /* Right	*/
#define PAD_HOME PAD_7              /* Home		*/
#define PAD_END PAD_1               /* End		*/
#define PAD_PAGEUP PAD_9            /* Page Up	*/
#define PAD_PAGEDOWN PAD_3          /* Page Down	*/
#define PAD_INS PAD_0               /* Ins		*/
#define PAD_MID PAD_5               /* Middle key	*/
#define PAD_DEL PAD_DOT             /* Del		*/

/*CRT Controller 相关定义*/
#define CRTC_ADDR_REG 0x3D4
#define CRTC_DATA_REG 0x3D5
#define START_ADDR_H 0xC
#define START_ADDR_L 0xD
#define CURSOR_H 0xE
#define CURSOR_L 0xF
#define V_MEM_BASE 0xB8000
#define V_MEM_SIZE 0x8000
//=====================================================================================
#define RUNNING 0
#define SENDING 0x2
#define RECEIVING 0x4
#define ANY -1
#define NO_TASK -2
#define INTERRUPT -3

// 系统调用
#define INDEX_SYSCALL_GET_TICKS 0
#define INDEX_SYSCALL_WRITE 1
#define INDEX_SYSCALL_IPC_SEND 2
#define INDEX_SYSCALL_IPC_RECEIVE 3
// pid
#define PID_TTY 0
#define PID_SYSTEM 1
#define PID_HD 2
#define PID_FS 3
// 信息类型
#define MSG_TYPE_HD 2
#define MSG_TYPE_FS 3
#define MSG_TYPE_GET_TICKS 0
#define MSG_TYPE_INT 1
// 系统调用信息操作
#define FUNTION_DEV_OPEN 1001
#define FUNTION_DEV_WRITE 1002
#define FUNTION_DEV_READ 1003
#define FUNTION_DEV_CLOSE 1004
// 磁盘
#define SECTOR_SIZE 512
#define BLOCK_SIZE 1024
#define INODE_SIZE 0x80

// 主通道最多有两个驱动
#define MAX_DRIVES 2
#define NR_PART_PER_DRIVE 4
#define NR_SUB_PER_PART 16
#define NR_SUB_PER_DRIVE (NR_SUB_PER_PART * NR_PART_PER_DRIVE)
#define NR_PRIM_PER_DRIVE (NR_PART_PER_DRIVE + 1)
// 占用的最大设备号
#define MAX_PRIM (MAX_DRIVES * NR_PRIM_PER_DRIVE - 1)
#define MAX_SUBPARTITIONS (NR_SUB_PER_DRIVE * MAX_DRIVES)
// 逻辑分区的第一个设备号
#define logic_start (MAX_PRIM + 1)
#endif
