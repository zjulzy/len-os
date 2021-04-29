//定义结构体
#ifndef LENOS_PROTECT_H
#define LENOS_PROTECT_H

// layer==-2
#include "const.h"
#include "type.h"
typedef void (*task_f)();
//存储段描述符/系统段描述符
typedef struct s_descriptor {
    u16 limit_low;        // Limit
    u16 base_low;         // Base
    u8 base_mid;          // Base
    u8 attr1;             // P(1) DPL(2) DT(1) TYPE(4)
    u8 limit_high_attr2;  // G(1) D(1) 0(1) AVL(1) LimitHigh(4)
    u8 base_high;         // Base
} DESCRIPTOR;

//门描述符(/调用门中断门/陷阱门/任务门)
typedef struct s_gate {
    u16 offset_low;  // Offset Low
    u16 selector;    // Selector
    u8 dcount;  //调用门中调用子程序时,如果引起特权级的改变,切换堆栈时需要复制的双字参数数量
    u8 attr;    // P(1) DPL(2) DT(1) TYPE(4)
    u16 offset_high;  // Offset High
} GATE;
typedef struct s_keyrmap_result {
    key_type type;   //键值的类型
    key_value data;  //键值的数据
} KEYMAP_RESULT;
// 以下内容改自Minix
// output子系统信息结构体, 信息类型为OUTPUT_SYSTEM (0)
struct OUTPUT_MESSAGE {
    char function;      // 执行的功能 --> 显示字符==0 特殊功能==1
    int console_index;  // 要输出的控制台指针
    u32 pid;  // 发送进程的pid,用于确定要显示字符的内存地址
    char *data;      // 要显示字符的指针
    char disp_func;  // 执行的具体功能
};
// input子系统信息
struct INPUT_MESSAGE {
    int input_source;  // input输入源
    KEYMAP_RESULT keyboard_result;
};
// 硬盘操作信息
struct DISK_MESSAGE {
    u8 function;      // 执行的操作类型
    u32 pid;          // 信息来源进程
    char *buffer;     // 缓冲区指针
    u32 sector_head;  // 操作的起始位置
    int bytes_count;  //读取的字节数
    u8 result;        // 磁盘操作的结果
};
struct FS_MESSAGE {
    u8 function;      // 执行的操作类型
    u32 pid;          // 信息来源 进程
    u32 inode_index;  // 文件描述符指针
    char *buffer;     // 数据缓冲区
    u32 count;        // 读取的大小
    u8 result;        //返回值结果
    char *file_name;  // 文件名字
};
struct MEM_MESSAGE {
    u8 function;         // 执行的操作类型
    u32 pid;             // 信息来源进程
    struct inode *file;  // 目标文件
    // 创建子进程: -1: 创建失败  0~MAX_PRO_NUM : 子进程pid
    int result;  //返回值结果
};

typedef struct mess {
    int source;
    int type;
    int value;
    union {
        struct OUTPUT_MESSAGE output_message;
        struct INPUT_MESSAGE input_message;
        struct DISK_MESSAGE disk_message;
        struct FS_MESSAGE fs_message;
        struct MEM_MESSAGE mem_message;
    } u;
} MESSAGE;

// 进程相关---------------------------------------------------------------------------------
// 进程控制块PCB中存放的关于程序运行状态的信息
typedef struct s_stackframe {
    //==========================================
    // 在进程切换的过程中被 save() 保存的进程状态
    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;
    u32 edi;
    u32 esi;
    u32 ebp;
    u32 kernel_esp;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    //=============================================
    // 在发生中断的时候esp会指向这里
    // save函数的返回地址
    u32 retaddr;
    //============================
    // 在中断发生的时候会被CPU自动保存的寄存器
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;
    //============================
} STACK_FRAME;

// 进程控制块PCB组成单元
// 存放了程序运行状态信息,ldt选择子,进程调度信息,进程id,进程名字
typedef struct s_proc {
    STACK_FRAME regs;

    //虽然下面接着就是局部描述符表,但是这个ldt_sel依然有存在的意义
    //其指出了ldt的界限,同时简化了加载ldt的步骤
    u16 ldt_sel;
    DESCRIPTOR ldts[LDT_SIZE];

    int ticks;
    int priority;

    // 为进程指定终端进行输出
    int tty;
    u32 pid;
    char p_name[16];
    u8 queen_number;

    // 进程调度队列内部指针,暂定为双向
    struct s_proc *pre_pcb;
    struct s_proc *next_pcb;

    // 进程间通信需要用到的数据，进程调度根据这个标志进行阻塞
    // 进程状态标志
    // 0 --> 正常执行
    // 2 --> 发送信息阻塞中
    // 4 --> 接收信息阻塞中
    int flags;

    // 本进程准备发送的信息或等待接收的信息
    // 尝试发送信息的对象
    // 尝试接收信息的源
    struct mess *message;
    int send_to_record;
    int recv_from_record;

    // 0 --> ready to handle an interupt
    // 1 --> handling an interupt
    // 操作系统通过把这一位从0设为1来通知中断的发生
    int has_int_msg;

    // 该进程的接收消息队列
    s_proc *receive_quene;
    // 用于产生链表结构的指针
    // 一个进程只能在一个接受队列中，因此使用指针用来表示接受队列中下一个进程
    s_proc *next_sending;

} PROCESS;

// 这个结构体用来定义系统初始进程
// 这个设计最早来自minix
typedef struct s_task {
    task_f initial_eip;
    int stacksize;
    char name[16];
    // int priority;
    u32 pid;
} TASK;
//-----------------------------------------------------------------------------------
// tss结构体
typedef struct s_tss {
    u32 backlink;
    u32 esp0;  //内核段栈指针
    u32 ss0;   //内核段栈基址
    u32 esp1;
    u32 ss1;
    u32 esp2;
    u32 ss2;
    u32 cr3;
    u32 eip;
    u32 flags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    u32 ldt;
    u16 trap;
    u16 iobase;  // I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图
} TSS;

//定义键盘输入缓存区，具体实现类似于循环队列
typedef struct s_keyboard {
    char *p_head;
    char *p_tail;
    int count;
    char buffer[KB_IN_BYTES];
} KB_INPUT;

// tty的输入缓存区
typedef struct s_tty {
    u32 in_buffer[TTY_IN_BYTES];
    u32 *in_buffer_head; /*指向缓存区的下一个空闲位置*/
    u32 *in_buffer_tail; /*指向键盘任务应该处理的键值*/
    int in_buffer_count;
} S_TTY;

//保存tty在显存中的各种位置
typedef struct s_console {
    u32 current_addr;
    u32 v_mem_addr; /*当前控制台在显存中的位置以及所占大小*/
    u32 v_mem_limit;
    u32 cursor; /*当前光标位置*/
} S_CONSOLE;
// 文件系统相关结构体============================================================
typedef struct hd_cmd {
    u8 features;
    u8 count;
    u8 lba_low;
    u8 lba_mid;
    u8 lba_high;
    u8 device;
    u8 command;
} HD_CMD;

struct part_info {
    u32 base; /* # of start sector (NOT byte offset, but SECTOR) */
    u32 size; /* how many sectors in this partition (NOT byte size, but SECTOR
                 number) */
};

/* main drive struct, one entry per drive */
struct hd_info {
    /* int			cylinders; */
    /* int			heads; */
    /* int			sectors; */
    /* int			precomp; */
    /* int			lzone; */
    /* int			ctl; */
    int open_cnt;
    // struct part_info primary[NR_PRIM_PER_DRIVE];
    // struct part_info logical[NR_SUB_PER_DRIVE];
};

// 文件系统相关数据结构
// 超级块，位于分区的第二个块
struct super_block {
    u32 s_inodes_count;       //文件系统中inode的总数
    u32 s_blocks_count;       //文件系统中块的总数
    u32 s_r_blocks_count;     //保留块的总数
    u32 s_free_blocks_count;  //未使用的块的总数,包含保留块
    u32 s_free_inodes_count;  //未使用的inode的总数
    u32 s_first_data_block;
    u32 s_log_block_size;    // 1024左移，计算块的大小
    u32 s_log_frag_size;     // 1024左移或者右移，计算段的大小
    u32 s_blocks_per_group;  //每个块组中块的个数
    u32 s_frags_per_group;   //每个块组中段的个数
    u32 s_inodes_per_group;  //每个块组中的inode的总数
    u32 s_mtime;
    u32 s_wtime;
    u16 s_mnt_count;
    u16 s_max_mnt_count;
    u16 s_magic;  //文件系统标志
    u16 s_state;  //文件系统的状态
    u16 s_errors;
    u16 s_minor_rev_level;
    u32 s_lastcheck;
    u32 s_checkinterval;
    u32 s_creator_os;
    u32 s_rev_level;
    u16 s_def_resuid;
    u16 s_def_resgid;
    // 仅用于使用动态inode大小的修订版（EXT2_DYNAMIC_REV）
    u32 s_first_ino;
    u16 s_inode_size;
    u16 s_block_group_nr;
    u32 s_feature_compat;
    u32 s_feature_incompat;
    u32 s_feature_ro_compat;
    u8 s_uuid[16];
    char s_volume_name[16];
    char s_last_mounted[64];
    u32 s_algorithm_usage_bitmap;
    u8 s_prealloc_blocks;
    u8 s_prealloc_dir_blocks;
    u16 s_padding;
    u32 s_reserved[204];
};
// 组描述符
struct group_descriptor {
    u32 bg_block_bitmap;       //块位图所在的第一个块的块id
    u32 bg_inode_bitmap;       // inode位图所在的第一个块的块id
    u32 bg_inode_table;        // inode表所在的第一个块的块id
    u16 bg_free_blocks_count;  //块组中未使用的块数
    u16 bg_free_inodes_count;  // 块组中未使用的inode数
    u16 bg_used_dirs_count;    // 块组分配的目录的inode数
    u16 bg_pad;
    u32 bg_reserved[3];
};
//文件目录入口项
typedef struct directory_entry {
    u32 inode;       //文件入口的inode号，0表示该项未使用
    u16 rec_len;     //目录项长度
    u8 name_len;     //文件名包含的字符数
    u8 file_type;    //文件类型
    char name[255];  // 文件名
} DIR_ENTRY;
struct inode {
    u16 i_mode;  //文件格式和访问权限
    u16 i_uid;   //文件所有者id后16位
    u32 i_size;  //文件字节数
    u32 i_atime;
    u32 i_ctime;
    u32 i_mtime;
    u32 i_dtime;
    u16 i_gid;
    u16 i_links_count;
    u32 i_blocks;
    u32 i_flags;
    u32 osd1;
    u32 i_block[15];
    u32 i_generation;
    u32 i_file_acl;
    u32 i_dir_acl;
    u32 i_faddr;
    u32 osd2[3];
};
#endif