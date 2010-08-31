#include "my_lib.h"
#include "my_printf.h"
#include "CMSIS/a2fxxxm3.h"

#if 0

void *my_memcpy(void *dst, const void *src, size_t size)
{
    void *savedst = dst;

    while (size-- > 0) {
	*(char *)(dst++) = *(char *)(src++);
    }

    return savedst;
}

void *my_memset(void *s, int c, size_t size)
{
    void *saves = s;

    while (size-- > 0) {
	*(char *)(s++) = c;
    }

    return saves;
}

#endif 

void my_udelay(unsigned long usec)
{
    TIMER->TIM1_LOADVAL = usec * (g_FrequencyPCLK0 / 1000000);
    while (TIMER->TIM1_VAL)
	;
}

void my_memory_dump(unsigned char *addr, int len)
{
    int i;
    my_printf("Dumping %d bytes @ 0x%08x\n", len, (unsigned int)addr);
    for (i = 0; i < len; i++) {
	my_printf("%02x ", addr[i]);
	if (((i + 1) % 16) == 0) {
	    my_printf("\n");
	}
    }
}
