#include "fs.h"

#include "console.h"
#include "const.h"
#include "global.h"
#include "harddrive.h"
#include "memory.h"
#include "process.h"
#include "syscall.h"
// 文件系统任务
void task_fs() {
    init_fs();
    MESSAGE msg;
    int src, result;
    while (ipc(INDEX_SYSCALL_IPC_RECEIVE, ANY, &msg) == 0) {
        src = msg.source;
        switch (msg.u.fs_message.function) {
            case FUNTION_FS_ROOT:
                msg.u.fs_message.inode_index = 2;
                result = fs_read_file(&msg);
                msg.u.fs_message.result = 0;
                break;
            case FUNTION_FS_CD:
                result = fs_read_file(&msg);
                break;
            case FUNTION_DEV_READ:
                result = fs_read_file(&msg);
                break;
            case FUNTION_DEV_WRITE:
                break;
            default:
                break;
        }
        //向发送message的进程反信
        msg.source = PID_FS;
        msg.type = MSG_TYPE_FS;
        msg.u.fs_message.result = result;
        ipc(INDEX_SYSCALL_IPC_SEND, src, &msg);
    }
}

// 根据进程发来的信息体中的inode等读取数据
int fs_read_file(MESSAGE *msg) {
    inode curr;
    memcpy((void *)&curr,
           (void *)(fs_inode_table + msg->u.fs_message.inode_index - 1),
           INODE_SIZE);
    // 判断读取字节数是否大于文件大小
    // if (curr.i_size < msg->u.fs_message.count) {
    //     return 1;
    // }
    int bytes = curr.i_size;
    char *buffer = (char *)vir2line(proc_table + msg->u.fs_message.pid,
                                    msg->u.fs_message.buffer);
    //首先读取12个直接索引，再读取一个一级索引，一共256+12个块
    //由于目前文件较小，不对后面的二级索引进行处理
    for (int i = 0; (i < 12) and (bytes > 0); i++) {
        int bytes_read = min(bytes, BLOCK_SIZE);
        fs_op_disk(curr.i_block[i] * 2, buffer, bytes_read, FUNTION_DEV_READ);
        buffer += bytes_read;
        bytes -= bytes_read;
    }
    if (bytes > 0) {
        //读取一级索引指向的块
        u32 inode_index[BLOCK_SIZE / 4];
        fs_op_disk(curr.i_block[12] * 2, (char *)inode_index, BLOCK_SIZE,
                   FUNTION_DEV_READ);
        for (int i = 0; (i < BLOCK_SIZE / 4) and (bytes > 0); i++) {
            int bytes_read = min(bytes, BLOCK_SIZE);
            fs_op_disk(inode_index[i] * 2, buffer, bytes_read,
                       FUNTION_DEV_READ);
            buffer += bytes_read;
            bytes -= bytes_read;
        }
    }
    return 0;
}

void init_fs() {
    MESSAGE msg;
    // msg.type = MSG_TYPE_HD;
    // msg.u.disk_message.function = FUNTION_DEV_OPEN;
    // if (ipc(INDEX_SYSCALL_IPC_SEND, PID_HD, &msg) == 0) {
    //     ipc(INDEX_SYSCALL_IPC_RECEIVE, PID_HD, &msg);
    // }

    //读取超级块
    fs_op_disk(2, (char *)&fs_super_block, BLOCK_SIZE, FUNTION_DEV_READ);

    //计算总的group数
    group_num =
        (fs_super_block.s_blocks_count / fs_super_block.s_blocks_per_group);
    //读取组描述符表
    fs_op_disk(4, (char *)&fs_group_descriptors, BLOCK_SIZE, FUNTION_DEV_READ);
    //遍历读取位图和inode表
    for (int i = 0; i < group_num; i++) {
        fs_op_disk(fs_group_descriptors[i].bg_inode_bitmap * 2,
                   (char *)&inode_bitmap[i], BLOCK_SIZE, FUNTION_DEV_READ);
        fs_op_disk(fs_group_descriptors[i].bg_block_bitmap * 2,
                   (char *)&block_bitmap[i], BLOCK_SIZE, FUNTION_DEV_READ);
        fs_op_disk(
            fs_group_descriptors[i].bg_inode_table * 2,
            (char *)&fs_inode_table[i * fs_super_block.s_inodes_per_group],
            fs_super_block.s_inodes_per_group * INODE_SIZE, FUNTION_DEV_READ);
    }
    //初始化根目录inode，节点序号为2
    fs_get_inode(2, &root_dictionary);
    printf("start FS\n");
}

// 文件系统对磁盘操作，function代表对硬盘的读操作和写操作
int fs_op_disk(int sector_head, char *buffer, int bytes, u8 funtion) {
    MESSAGE msg;
    msg.source = PID_FS;
    msg.type = MSG_TYPE_HD;
    msg.u.disk_message.buffer = buffer;
    msg.u.disk_message.function = funtion;
    msg.u.disk_message.bytes_count = bytes;
    msg.u.disk_message.pid = PID_FS;
    msg.u.disk_message.sector_head = sector_head;
    if (ipc(INDEX_SYSCALL_IPC_SEND, PID_HD, &msg) == 0) {
        ipc(INDEX_SYSCALL_IPC_RECEIVE, PID_HD, &msg);
        return msg.u.disk_message.result;
    } else {
        return 2;
    }
}

//根据inode序号获取inode
int fs_get_inode(int index, inode *target) {
    if (index >= fs_super_block.s_inodes_count) return 1;
    memcpy((void *)target, (void *)&fs_inode_table[index - 1], INODE_SIZE);
    return 0;
}
