#include "process.h"
#include "protect.h"
#include "syscall.h"

// 预留的系统进程
void task_system() {
    MESSAGE msg;
    while (1) {
        ipc(INDEX_SYSCALL_IPC_RECEIVE, ANY, &msg);
        int src = msg.source;
        switch (msg.type) {
            case INDEX_SYSCALL_GET_TICKS:
                msg.value = ticks;
                ipc(INDEX_SYSCALL_IPC_SEND, src, &msg);
                break;

            default:
                break;
        }
    }
}