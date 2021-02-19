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
    void clock_handler();
    void interrupt_request(int irq);
    void disable_irq(int irq);
    void enable_irq(int irq);
#ifdef __cplusplus
}
#endif
#endif