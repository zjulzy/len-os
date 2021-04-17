//定义内存操作
#ifndef LENOS_MEMORY_H
#define LENOS_MEMORY_H
#ifdef __cplusplus
extern "C" {
#endif
void *memcpy(void *pDst, void *pSrc, int iSize);
void memset(void *p_dst, char ch, int size);

#ifdef __cplusplus
}
#endif
#endif