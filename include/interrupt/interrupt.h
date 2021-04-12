#ifndef LENOS_INTERRUPT_H
#define LENOS_INTERRUPT_H
#include "interrupt_option.h"
#include "proto.h"
#include "const.h"
#include "global.h"
#include "display.h"
#ifdef __cplusplus
extern "C"
{
#endif
    void interruptInitialize();
    void init_8259A();
    void schedule();
    void interrupt_request(int irq);
    void disable_irq(int irq);
    void enable_irq(int irq);
    void init_clock();
    void init_keyboard();
    void clock_handler();
    void keyboard_handler();
    void hd_handler();
#ifdef __cplusplus
}
#endif
#endif