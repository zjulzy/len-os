//定义内存操作
#ifndef LENOS_MEMORY_H
#define LENOS_MEMORY_H
#ifdef __cplusplus
extern "C"
{
#endif
    void *memcpy(void *pDst, void *pSrc, int iSize);

#ifdef __cplusplus
}
#endif
#endif