//定义内存操作
#ifndef LENOS_MEMORY_H
#define LENOS_MEMORY_H
#ifdef __cplusplus
extern "C" {
#endif

// memory.asm===============================================
void *memcpy(void *pDst, void *pSrc, int iSize);
void memset(void *p_dst, char ch, int size);

// memory.cc===============================================
void task_mem();
int init_memory_manager();
int do_fork();
int alloc_mem(int pid, int mem_size);
void do_exit(int pid, int status);
void do_wait(int pid);
int do_exec(MESSAGE *msg);

#ifdef __cplusplus
}
#endif
#endif