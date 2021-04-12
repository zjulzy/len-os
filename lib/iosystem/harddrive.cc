#include "harddrive.h"
#include "proto.h"
#include "console.h"
#include "interrupt.h"
#include "syscall.h"
#include "const.h"

// 运行在特权级ring1
u8 hd_status;
u8 hdbuf[SECTOR_SIZE * 2];
void hd_cmd_out(HD_CMD *cmd)
{
    //判断status寄存器的bsy位是否为0，为1时磁盘正在工作
    while (in_byte(REG_STATUS) & STATUS_BSY != 0)
    {
    }

    out_byte(REG_DEV_CTRL, 0); //开启中断
    // 写寄存器命令块
    out_byte(REG_FEATURES, cmd->features);
    out_byte(REG_NSECTOR, cmd->count);
    out_byte(REG_LBA_LOW, cmd->lba_low);
    out_byte(REG_LBA_MID, cmd->lba_mid);
    out_byte(REG_LBA_HIGH, cmd->lba_high);
    out_byte(REG_DEVICE, cmd->device);
    // 写寄存器控制块
    out_byte(REG_CMD, cmd->command);
}
// 磁盘驱动部分=======================================================================
// 磁盘驱动任务
void task_hd()
{
    MESSAGE msg;
    // 初始化磁盘
    // 从bios数据区域获取驱动数量
    u8 *numOfDrives = (u8 *)(0x475);
    enable_irq(CASCADE_IRQ);
    enable_irq(AT_WINI_IRQ);
    while (1)
    {
        ipc(INDEX_SYSCALL_IPC_RECEIVE, ANY, &msg);
        int src = msg.source;
        switch (msg.type)
        {
        case MSG_TYPE_DEV_OPEN:
            char str[10];
            printf(itoa(str, hd_info()));
            break;

        default:
            break;
        }
    }
}
int hd_info()
{
    HD_CMD cmd{
        0, 0, 0, 0, 0, MAKE_DEVICE_REG(0, 0, 0), ATA_IDENTIFY};
    hd_cmd_out(&cmd);
    // 等待中断发生
    MESSAGE msg;
    ipc(INDEX_SYSCALL_IPC_RECEIVE, INTERRUPT, &msg);
    // 从端口读入数据
    port_in(REG_DATA, hdbuf, SECTOR_SIZE);
    u16 *info = (u16 *)hdbuf;
    // 计算有多少个sector，注意这里u16可能溢出
    int sectors = ((int)info[61] << 16) + info[60];
    return sectors;
}
// =================================================================================

// 文件系统任务
void task_fs()
{

    printf("start FS");
    MESSAGE msg;
    msg.type = MSG_TYPE_DEV_OPEN;
    if (ipc(INDEX_SYSCALL_IPC_SEND, PID_HD, &msg) == 0)
    {
        ipc(INDEX_SYSCALL_IPC_RECEIVE, PID_HD, &msg);
    }
}
