#include "harddrive.h"
#include "proto.h"
#include "console.h"
#include "interrupt.h"
#include "syscall.h"
#include "const.h"
#include "process.h"
#include "memory.h"
// 运行在特权级ring1

u8 hd_status;
bool open;
u8 hdbuf[SECTOR_SIZE * 2];
constexpr int drv_of_dev(int device)
{
    return (device <= MAX_PRIM ? device / NR_PRIM_PER_DRIVE : (device - logic_start) / NR_SUB_PER_DRIVE);
}
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
        switch (msg.u.disk_message.function)
        {
        case FUNTION_DEV_OPEN:
            hd_open();
            break;

        case FUNTION_DEV_READ:
            hd_read(&msg);
            break;
        case FUNTION_DEV_WRITE:
            hd_write(&msg);
            break;
        default:
            break;
        }
    }
}
int hd_sectors()
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
void hd_open()
{
    // 目前默认只有一个硬盘,没有进行分区
    char str[10];
    printf(itoa(str, hd_sectors()));
    open = true;
}
void hd_close()
{
    if (not open)
        panic("");
    open = false;
    return;
}

void hd_write(MESSAGE *msg)
{
    u32 sector_start = msg->u.disk_message.sector_head;

    HD_CMD cmd;
    cmd.features = 0;
    cmd.count = (msg->u.disk_message.bytes_count - 1) / SECTOR_SIZE + 1;
    cmd.lba_low = sector_start & 0xFF;
    cmd.lba_mid = (sector_start >> 8) & 0xFF;
    cmd.lba_high = (sector_start >> 16) & 0xFF;
    cmd.device = MAKE_DEVICE_REG(0, 0, (sector_start >> 24) & 0xF);
    cmd.command = ATA_WRITE;
    hd_cmd_out(&cmd);
    int bytes = ((msg->u.disk_message.bytes_count + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
    void *la = (void *)vir2line(proc_table + msg->u.disk_message.pid, msg->u.disk_message.buffer);
    while (bytes)
    {
        while ((in_byte(REG_STATUS) & STATUS_DRQ) != STATUS_DRQ)
        {
        }
        port_out(REG_DATA, la, bytes);
        // 等待中断发生
        MESSAGE msg;
        ipc(INDEX_SYSCALL_IPC_RECEIVE, INTERRUPT, &msg);
        bytes -= SECTOR_SIZE;
        la += SECTOR_SIZE;
    }
}
void hd_read(MESSAGE *msg)
{
    u32 sector_start = msg->u.disk_message.sector_head;

    HD_CMD cmd;
    cmd.features = 0;
    cmd.count = (msg->u.disk_message.bytes_count - 1) / SECTOR_SIZE + 1;
    cmd.lba_low = sector_start & 0xFF;
    cmd.lba_mid = (sector_start >> 8) & 0xFF;
    cmd.lba_high = (sector_start >> 16) & 0xFF;
    cmd.device = MAKE_DEVICE_REG(0, 0, (sector_start >> 24) & 0xF);
    cmd.command = ATA_READ;
    hd_cmd_out(&cmd);
    //防止长度不够一个扇区
    int bytes = ((msg->u.disk_message.bytes_count + SECTOR_SIZE - 1) / SECTOR_SIZE) * SECTOR_SIZE;
    void *la = (void *)vir2line(proc_table + msg->u.disk_message.pid, msg->u.disk_message.buffer);
    while (bytes)
    {
        // 等待中断发生
        MESSAGE msg;
        ipc(INDEX_SYSCALL_IPC_RECEIVE, INTERRUPT, &msg);
        port_in(REG_DATA, hdbuf, SECTOR_SIZE);
        memcpy(la, (void *)vir2line(proc_table + PID_HD, hdbuf), bytes);
        bytes -= SECTOR_SIZE;
        la += SECTOR_SIZE;
    }
}
// =================================================================================
