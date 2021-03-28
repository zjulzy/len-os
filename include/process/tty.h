#ifndef LENOS_TTY_H
#define LENOS_TTY_H

#include "global.h"
#include "display.h"
#include "interrupt.h"
#include "console.h"
#include "memory.h"

class TTY
{
public:
    // 控制台输出
    S_TTY tty_buffer;
    S_CONSOLE console_buffer;
    unsigned int id;
    void init(int index);
    void tty_do_read();
    void tty_do_write();
    void print(char out);
};

bool is_current_console(TTY *tty);

extern TTY tty_table[NR_CONSOLE];
#ifdef __cplusplus
extern "C"
{
#endif
    void task_tty();
#ifdef __cplusplus
}
#endif
#endif