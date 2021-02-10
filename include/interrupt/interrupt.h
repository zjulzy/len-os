#include "interrupt_option.h"
#include "proto.h"
#ifdef __cplusplus
extern "C"
{
#endif
    void interruptInitialize();

    void init_8259A();
#ifdef __cplusplus
}
#endif