#include "tty.h"

#include "keyboard.h"
#include "syscall.h"
TTY tty_table[NR_CONSOLE];
// tty类方法===================================================================
void TTY::init(int index) {
    //为了和光标地址统一，控制台缓存区单位为字节
    id = index;
    tty_buffer.in_buffer_count = 0;
    tty_buffer.in_buffer_head = tty_buffer.in_buffer_tail =
        tty_buffer.in_buffer;
    //初始化console
    int v_size = V_MEM_SIZE;
    int con_v_size = v_size / NR_CONSOLE;
    console_buffer.current_addr = console_buffer.v_mem_addr =
        index * con_v_size;
    console_buffer.v_mem_limit = con_v_size;
    console_buffer.cursor = index ? console_buffer.current_addr : disp_pos;
    clear_command();
    memset(prefix, '\0', 256);
    prefix[0] = '0' + index;
    print('0' + index, true);
    print('>', true);
}
//将键盘输入读入到缓存区
void TTY::tty_do_read() { keyboard_read(&tty_buffer); }

//将缓存中的按键读取到显存中
void TTY::tty_do_write() {
    if (tty_buffer.in_buffer_count) {
        char out = *(tty_buffer.in_buffer_tail);
        tty_buffer.in_buffer_tail =
            (tty_buffer.in_buffer_tail - tty_buffer.in_buffer + 1) %
                TTY_IN_BYTES +
            tty_buffer.in_buffer;
        tty_buffer.in_buffer_count--;
        if (out == '\n') {
            command[curr] = '\0';
            tty_command();
            clear_command();
        } else if (out == '\b') {
            curr--;
            command[curr] = '\0';
            print(out, true);
        } else {
            command[curr] = out;
            curr++;
            print(out, true);
        }
    }
}
void TTY::print(char out, bool isPrefix = true) {
    //获取当前光标的显存位置
    u8 *p_vmem = (u8 *)(V_MEM_BASE + console_buffer.cursor);

    switch (out) {
        // 如果下一个字符是换行，调整光标位置
        // 重新输出前缀
        case '\n':
            if (console_buffer.cursor < console_buffer.v_mem_addr +
                                            console_buffer.v_mem_limit -
                                            SCREEN_WIDTH * 2) {
                console_buffer.cursor =
                    console_buffer.v_mem_addr +
                    SCREEN_WIDTH * 2 *
                        ((console_buffer.cursor - console_buffer.v_mem_addr) /
                             SCREEN_WIDTH / 2 +
                         1);
                if (isPrefix) {
                    char *p;
                    for (p = prefix; *p; p++) {
                        print(*p, false);
                    }
                    print('>', false);
                }
            }
            break;
        case '\b':
            if (console_buffer.cursor > console_buffer.v_mem_addr) {
                console_buffer.cursor -= 2;
                *(p_vmem - 2) = ' ';
                *(p_vmem - 1) = 0x07;
            }
            break;
        default:
            if (console_buffer.cursor <
                console_buffer.v_mem_addr + console_buffer.v_mem_limit - 1) {
                *p_vmem++ = out;
                *p_vmem++ = 0x07;  //设置黑底白字
                console_buffer.cursor += 2;
            }
    }

    set_cursor(console_buffer.cursor);
    set_video_addr(console_buffer.current_addr);
}

// 识别命令区并执行
void TTY::tty_command() {
    MESSAGE msg;
    // 判断是否为root命令------------------------------------------------------------
    if (not strcmp(command, "root")) {
        msg.source = PID_TTY;
        msg.type = MSG_TYPE_FS;
        msg.u.fs_message.buffer = dictionary_buffer;
        msg.u.fs_message.function = FUNTION_FS_ROOT;
        msg.u.fs_message.pid = PID_TTY;
        ipc(INDEX_SYSCALL_IPC_SEND, PID_FS, &msg);
        ipc(INDEX_SYSCALL_IPC_RECEIVE, PID_FS, &msg);
        if (msg.u.fs_message.result == 0) {
            char str[] = "root/";
            memcpy(prefix, str, sizeof(str));
            curr = 2;
        }
        print('\n', true);

    }
    // 判断是否为cd命令--------------------------------------------------------------
    else if (command[0] == 'c' and command[1] == 'd') {
        char *dir_name = command + 3;
        u32 index = search_file(dir_name, EXT2_FT_DIR);
        if (index > 0) {
            msg.u.fs_message.inode_index = index;
            msg.type = MSG_TYPE_FS;
            msg.source = PID_FS;
            msg.u.fs_message.pid = PID_FS;
            msg.u.fs_message.buffer = dictionary_buffer;
            ipc(INDEX_SYSCALL_IPC_SEND, PID_FS, &msg);
            ipc(INDEX_SYSCALL_IPC_RECEIVE, PID_FS, &msg);
            curr = index;

            // 在面包屑导航中加一级
            post_dir(dir_name);
            print('\n', true);
        }
    }
    // 判断是否为open命令------------------------------------------------------------
    else if (command[0] == 'o' and command[1] == 'p' and command[2] == 'o' and
             command[3] == 'n') {
        char *file_name = command + 5;
        u32 index = search_file(file_name, EXT2_FT_REG_FILE);
        if (index > 0) {
            curr = index;
        }
    }
    // 判断是否为ls命令--------------------------------------------------------------
    else if (command[0] == 'l' and command[1] == 's') {
        char *buffer = dictionary_buffer;

        // 换行并且不显示前缀
        print('\n', false);
        while (buffer < dictionary_buffer + SIZE_OF_TTY_DIR) {
            directory_entry *curr_entry = (directory_entry *)buffer;
            if (curr_entry->rec_len == 0) break;
            char *out = curr_entry->name;
            while (*out) {
                print(*out, false);
                out++;
            }
            print(' ', false);

            // ext2中目录项有长度各不相同，目录项在删除时会被前一个目录项覆盖
            // 因此在遍历时要使用char*指针配合rec_len使用
            buffer += curr_entry->rec_len;
        }
        print('\n', true);
    } else {
        print('\n', true);
    }
}

// 将命令区清空
void TTY::clear_command() {
    memset(command, '\0', TTY_IN_BYTES);
    curr = 0;
}

// 在目录缓存中查找目标文件或者文件夹
int TTY::search_file(char *file_name, FILE_TYPE f) {
    char *buffer = dictionary_buffer;
    while (buffer < dictionary_buffer + SIZE_OF_TTY_DIR) {
        directory_entry *curr_entry = (directory_entry *)buffer;
        if (curr_entry->rec_len == 0) break;

        if (not strcmp(file_name, curr_entry->name) and
            curr_entry->file_type == f) {
            return curr_entry->inode;
        }
        buffer += curr_entry->rec_len;
    }
    return 0;
}

// cd命令执行后在终端前缀中加入一个文件夹名
void TTY::post_dir(char *dir_name) {
    char *ch = prefix;
    while (*ch) {
        ch++;
    }
    while (*dir_name) {
        *ch = *dir_name;
        dir_name++;
        ch++;
    }
    *ch++ = '/';
    *ch = '\0';
}

// 在会退到上一文件夹时，面包屑导航减少一级
void TTY::pre_dir() {
    char *ch = prefix;
    char *last;
    while (*ch) {
        if (*ch == '/') last = ch;
        ch++;
    }
    if (*(last + 1) != '\0') {
        *(last + 1) = '\0';
    }
}
// ===============================================================================
bool is_current_console(TTY *tty) { return tty - tty_table == current_console; }

// 终端任务进程
void task_tty() {
    current_console = 0;
    for (TTY *tty = tty_table; tty < tty_table + NR_CONSOLE; tty++) {
        tty->init(tty - tty_table);
    }
    select_console(current_console,
                   &(tty_table[current_console].console_buffer));
    while (1) {
        // 选择当前焦点控制台进行输入输出
        for (TTY *tty = tty_table; tty < tty_table + NR_CONSOLE; tty++) {
            if (is_current_console(tty)) {
                tty->tty_do_read();
                tty->tty_do_write();
            }
        }
    }
}
