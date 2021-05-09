#include "process.h"
#include "syscall.h"

int fork() {
    MESSAGE msg;
    msg.type = MSG_TYPE_MEM;
    msg.u.mem_message.function = FUNTION_FORK;

    // 向内存管理器发请求fork
    ipc(INDEX_SYSCALL_IPC_SEND, PID_MEM, &msg);

    // 通过内存管理器的返回值可以看到是否为子进程
    ipc(INDEX_SYSCALL_IPC_RECEIVE, PID_MEM, &msg);
    return msg.u.mem_message.result;
}