#include <common.h>
#include "CMSIS/a2fxxxm3.h"
#include "CMSIS/core_cm3.h"

/* Internal tick units */
static unsigned long long timestamp;	/* Monotonic incrementing timer */
static unsigned long lastdec;		/* Last decremneter snapshot */

int timer_init()
{
    SYSREG->SYSTICK_CR &= ~(1<<25); /* en noref */
    SYSREG->SYSTICK_CR |= (3 << 28); /* div by 32 */
    SYSREG->SYSTICK_CR &= ~0xffffff;
    SYSREG->SYSTICK_CR |= 0x7a12;
    SysTick->LOAD  = SysTick_LOAD_RELOAD_Msk - 1; 
    SysTick->VAL = 0;
    /* we don't want ints to be enabled */
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk;
    timestamp = 0;

    return 0;
}

unsigned long get_timer(unsigned long base)
{
	unsigned long now = SysTick->VAL;

	if (lastdec >= now)
		timestamp += lastdec - now;
	else
		timestamp += lastdec + SysTick_LOAD_RELOAD_Msk - 1 - now;

	lastdec = now;

	return timestamp / (CONFIG_SYSTICK_FREQ / 1000) - base;
}

void reset_timer(void)
{
    lastdec = SysTick->VAL;
    timestamp = 0;
}

/* delay x useconds */
void __udelay(unsigned long usec)
{
	unsigned long clc, tmp;

	clc = usec * (CONFIG_SYSTICK_FREQ / 1000000);

	/* get current timestamp */
	tmp = SysTick->VAL;

	if (tmp < clc) {
    	    /* loop till event */
    	    while (SysTick->VAL < tmp ||
		    SysTick->VAL > (SysTick_LOAD_RELOAD_Msk - 1 - clc + tmp))
    		    ;	/* nop */
	} else {
	    while (SysTick->VAL > (tmp - clc));
	}
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
unsigned long get_tbclk(void)
{
	return CONFIG_SYSTICK_FREQ;
}
