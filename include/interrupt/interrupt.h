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
#ifdef __cplusplus
}
#endif