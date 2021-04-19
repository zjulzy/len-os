#ifndef LENOS_TTY_H
#define LENOS_TTY_H

#include "console.h"
#include "display.h"
#include "global.h"
#include "interrupt.h"
#include "memory.h"

class TTY {
   public:
    // 控制台输出
    S_TTY tty_buffer;
    // 控制台输出前缀，即当前文件夹
    char prefix[TTY_IN_BYTES];
    // 当前目录所有目录项缓存区
    char dictionary_buffer[SIZE_OF_TTY_DIR];
    // 当前文件inode
    inode curr_file_inode;
    // 当前行的命令
    char command[TTY_IN_BYTES];
    int curr = 0;
    S_CONSOLE console_buffer;
    unsigned int id;
    void init(int index);
    void tty_do_read();
    void tty_do_write();
    void print(char out);
    void tty_command();
    void clear_command();
    int search_file(char* file_name, FILE_TYPE f);
    void pre_dir();
    void post_dir(char* dir_name);
};

bool is_current_console(TTY* tty);

extern TTY tty_table[NR_CONSOLE];
#ifdef __cplusplus
extern "C" {
#endif
void task_tty();
#ifdef __cplusplus
}
#endif
#endif