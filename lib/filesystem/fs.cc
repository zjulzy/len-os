#include "fs.h"
#include "harddrive.h"
#include "const.h"
#include "console.h"
#include "syscall.h"
// 文件系统任务
void task_fs()
{
    init_fs();
}
void init_fs()
{
    printf("start FS");
    MESSAGE msg;
    msg.type = MSG_TYPE_HD;
    msg.u.disk_message.function = FUNTION_DEV_OPEN;
    if (ipc(INDEX_SYSCALL_IPC_SEND, PID_HD, &msg) == 0)
    {
        ipc(INDEX_SYSCALL_IPC_RECEIVE, PID_HD, &msg);
    }
}
