#ifndef LENOS_CONSOLE_H
#define LENOS_CONSOLE_H
#ifdef __cplusplus
extern "C" {
#endif
#include "const.h"
#include "global.h"
#include "proto.h"
void set_cursor(unsigned int position);
void select_console(int console_index, S_CONSOLE *console);
void set_video_addr(u32 addr);
void scroll_screen(S_CONSOLE *console, bool up);
int printf(const char *fmt);
#ifdef __cplusplus
}
#endif
#endif