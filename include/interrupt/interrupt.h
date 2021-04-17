#ifndef LENOS_INTERRUPT_H
#define LENOS_INTERRUPT_H
#include "const.h"
#include "display.h"
#include "global.h"
#include "interrupt_option.h"
#include "proto.h"
#ifdef __cplusplus
extern "C" {
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