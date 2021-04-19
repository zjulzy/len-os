//定义数据类型
#ifndef LENOS_TYPE_H
#define LENOS_TYPE_H
#ifdef __cplusplus
extern "C" {
#endif
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef void (*int_handler)();
//读取键盘相关
// key_type表明读取键盘得到的数据的类型
// key_type = 0  ==> ascii码
// key_type = 1  ==> 特殊信息
typedef char key_type;
typedef char key_value;
char* itoa(char* str, int num);
bool strcmp(char* str1, char* str2);
#ifdef __cplusplus
}
#endif
#endif