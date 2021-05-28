#include "process.h"
#include "protect.h"
#include "string.h"
#include "syscall.h"
// EXEC函数根据输入的参数表构建进程的初始栈
// 栈的格式为首先是所有n个参数的指针，然后是n个参数
int exec(int file, char* arg[]) {
    char** args = (char**)(&arg);
    //将参数先入栈保存
    char stack_arg[PROC_ORIGIN_STACK];

    // 参数计数
    int arg_count = 0;
    char** p = arg;
    char** q = (char**)stack_arg;

    while (*(p++)) {
        arg_count++;
    }
    // 参数指针后填一个0
    int curr = sizeof(char*) * (arg_count + 1);
    *((int*)&stack_arg[arg_count]) = 0;
    for (p = arg; *p != 0; p++) {
        *(q++) = &stack_arg[curr];
        strcpy(&stack_arg[curr], *p);
        curr += strlen(*p);
        stack_arg[curr] = 0;
        curr++;
    }
    MESSAGE exec_msg;
    exec_msg.type = MSG_TYPE_MEM;
    exec_msg.u.mem_message.inode_index = file;

    exec_msg.u.mem_message.function = FUNTION_EXEC;
    exec_msg.u.mem_message.arg_buffer = stack_arg;
    exec_msg.u.mem_message.arg_buffer_length = curr;

    // 传入进程代码地址

    ipc(INDEX_SYSCALL_IPC_SEND, PID_MEM, &exec_msg);
    ipc(INDEX_SYSCALL_IPC_RECEIVE, PID_MEM, &exec_msg);
    return exec_msg.u.mem_message.result;
}