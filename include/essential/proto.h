//定义端口操作函数

#ifndef LENOS_PROTO_H
#define LENOS_PROTO_H
#ifdef __cplusplus
extern "C" {
#endif
#include "type.h"
void out_byte(u16 port, u8 value);
u8 in_byte(u16 port);
void port_in(u16 port, void *buf, int n);
void port_out(u16 port, void *buf, int n);
#ifdef __cplusplus
}
#endif
#endif