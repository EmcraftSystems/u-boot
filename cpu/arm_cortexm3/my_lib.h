#include <stddef.h>

extern void *my_memcpy(void *dst, const void *src, size_t size);
extern void *my_memset(void *s, int c, size_t size);
extern void my_udelay(unsigned long usec);
extern void my_memory_dump(unsigned char *addr, int len);
