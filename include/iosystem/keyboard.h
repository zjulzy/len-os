#ifndef LENOS_KEYBOARD_H
#define LENOS_KEYBOARD_H
#ifdef __cplusplus
extern "C"
{
#endif
#include "const.h"
#include "type.h"
#include "protect.h"
#include "proto.h"
#include "global.h"
#include "display.h"
#include "console.h"

    void keyboard_read(S_TTY *p_tty);
    u8 get_byte_from_kbuf();
    void in_process(S_TTY *p_tty, u32 key);
#ifdef __cplusplus
}
#endif
#endif