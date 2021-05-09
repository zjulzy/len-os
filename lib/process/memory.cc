#include "memory.h"

#include "base.h"
#include "const.h"
#include "protect.h"
#include "syscall.h"

// memory manager主任务循环
void task_mem() {
    init_memory_manager();
    MESSAGE msg;
    while (1) {
        ipc(INDEX_SYSCALL_IPC_RECEIVE, ANY, &msg);
        switch (msg.u.mem_message.function) {
            default:
                break;
        }
    }
}

int init_memory_manager() { return 0; }

// ***************************************************************************
//                                  do_fork
// 第一步：在进程表中找到空位填充，将父进程表中的内容分配给子进程块
// 第二步：读取父进程ldt，获取父进程地址空间
// 第三步：为子进程分配内存空间，将父进程内存复制给子进程，更新子空间ldt
// 第四步：通过文件系统为子进程共享父进程打开的文件
// 第五步：解除子进程阻塞，函数返回
// ***************************************************************************
int do_fork(int father) {
    PROCESS* p;
    for (p = proc_table; p < proc_table + NR_TASK + NR_PROCESS; p++) {
        if (p->flags == FREE) {
            break;
        }
    }
    int child_pid = p - proc_table;
    if (p - proc_table == NR_TASK + NR_PROCESS) return -1;

    // 为子进程复制父进程的进程表
    int father_pid = father;
    u16 child_seg_sel = p->ldt_sel;
    *p = proc_table[father_pid];
    p->ldt_sel = child_seg_sel;
    p->p_parent = father_pid;

    // 父进程代码段
    DESCRIPTOR* descriptor = &proc_table[father_pid].ldts[INDEX_LDT_C];
    int father_T_base =
        reassembly(descriptor->base_high, 24, descriptor->base_mid, 16,
                   descriptor->base_low);
    int father_T_limit = reassembly(0, 0, descriptor->limit_high_attr2 & 0xF,
                                    16, descriptor->limit_low);
    int father_T_size =
        (father_T_limit + 1) *
        ((descriptor->limit_high_attr2 & DA_LIMIT_4K >> 8) ? 4096 : 1);

    // 父进程堆栈段
    descriptor = &proc_table[father_pid].ldts[INDEX_LDT_RW];
    int father_D_base =
        reassembly(descriptor->base_high, 24, descriptor->base_mid, 16,
                   descriptor->base_low);
    int father_D_limit = reassembly(0, 0, descriptor->limit_high_attr2 & 0xF,
                                    16, descriptor->limit_low);
    int father_D_size =
        (father_D_limit + 1) *
        ((descriptor->limit_high_attr2 & DA_LIMIT_4K >> 8) ? 4096 : 1);

    int child_base = alloc_mem(child_pid, father_T_size);
    // 将父进程内存空间拷贝给子进程
    memcpy((void*)child_base, (void*)father_T_base, father_T_size);

    //初始化子进程LDT
    init_descriptor(&p->ldts[INDEX_LDT_C], child_base,
                    (PROC_IMAGE_SIZE_DEFAULT - 1) >> LIMIT_4K_SHIFT,
                    DA_LIMIT_4K | DA_32 | DAC_E | PRIVILEGE_USER << 5);
    init_descriptor(&p->ldts[INDEX_LDT_RW], child_base,
                    (PROC_IMAGE_SIZE_DEFAULT - 1) >> LIMIT_4K_SHIFT,
                    DA_LIMIT_4K | DA_32 | DAD_RW | PRIVILEGE_USER << 5);

    // 告知文件系统
    MESSAGE msg;
    msg.type = MSG_TYPE_FS;
    msg.u.fs_message.pid = child_pid;
    msg.u.fs_message.function = FUNTION_FORK;
    msg.source = father_pid;
    ipc(INDEX_SYSCALL_IPC_SEND, PID_FS, &msg);
    ipc(INDEX_SYSCALL_IPC_RECEIVE, PID_FS, &msg);

    MESSAGE m;
    m.type = MSG_TYPE_SYSCALL_RET;
    m.source = 0;
    ipc(INDEX_SYSCALL_IPC_SEND, child_pid, &m);

    return child_pid;
}

// 为进程分配内存
int alloc_mem(int pid, int mem_size) {
    return PROCS_BASE +
           (pid - NR_TASK - NR_USER_PROCESS) * PROC_IMAGE_SIZE_DEFAULT;
}