#ifndef LENOS_FS_H
#define LENOS_FS_H
#include "protect.h"
#ifdef __cplusplus
extern "C" {
#endif
void task_fs();
void init_fs();
int fs_op_disk(int sector_head, char *buffer, int bytes, u8 funtion);
int fs_get_inode(int index, inode *target);
#ifdef __cplusplus
}
#endif
#endif