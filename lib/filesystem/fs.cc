#include "fs.h"
#include "harddrive.h"
#include "const.h"
#include "console.h"
#include "syscall.h"
#include "global.h"
#include "memory.h"
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
    //读取超级块
    fs_op_disk(2, (char *)&fs_super_block, BLOCK_SIZE, FUNTION_DEV_READ);

    //计算总的group数
    group_num = (fs_super_block.s_blocks_count / fs_super_block.s_blocks_per_group);
    //读取组描述符表
    fs_op_disk(4, (char *)&fs_group_descriptors, BLOCK_SIZE, FUNTION_DEV_READ);
    //遍历读取位图和inode表
    for (int i = 0; i < group_num; i++)
    {
        fs_op_disk(fs_group_descriptors[i].bg_inode_bitmap * 2, (char *)&inode_bitmap[i], BLOCK_SIZE, FUNTION_DEV_READ);
        fs_op_disk(fs_group_descriptors[i].bg_block_bitmap * 2, (char *)&block_bitmap[i], BLOCK_SIZE, FUNTION_DEV_READ);
        fs_op_disk(fs_group_descriptors[i].bg_inode_table * 2, (char *)&fs_inode_table[i * fs_super_block.s_inodes_per_group], fs_super_block.s_inodes_count * INODE_SIZE, FUNTION_DEV_READ);
    }
    //初始化根目录inode，节点序号为2
    fs_get_inode(2, &root_dictionary);
    //进入根目录
}

int fs_op_disk(int sector_head, char *buffer, int bytes, u8 funtion)
{
    MESSAGE msg;
    msg.source = PID_FS;
    msg.type = MSG_TYPE_HD;
    msg.u.disk_message.buffer = buffer;
    msg.u.disk_message.function = funtion;
    msg.u.disk_message.bytes_count = bytes;
    msg.u.disk_message.pid = PID_FS;
    msg.u.disk_message.sector_head = sector_head;
    if (ipc(INDEX_SYSCALL_IPC_SEND, PID_HD, &msg) == 0)
    {
        ipc(INDEX_SYSCALL_IPC_RECEIVE, PID_HD, &msg);
        return msg.u.disk_message.result;
    }
    else
    {
        return 2;
    }
}
//根据inode序号获取inode
int fs_get_inode(int index, inode *target)
{
    if (index >= fs_super_block.s_inodes_count)
        return 1;
    memcpy((void *)target, (void *)&fs_inode_table[index], INODE_SIZE);
    return 0;
}
