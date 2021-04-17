#ifndef LENOS_DISPLAY_H
#define LENOS_DISPLAY_H
#ifdef __cplusplus
extern "C" {
#endif
void disp_clear();
void disp_str(char *info);
void disp_int(int n);
void disp_char(char ch);
#ifdef __cplusplus
}
#endif
#endif