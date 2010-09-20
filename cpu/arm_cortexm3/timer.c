#include "CMSIS/core_cm3.h"

/* Internal tick units */
static unsigned long long timestamp;	/* Monotonic incrementing timer */
static unsigned long lastdec;		/* Last decremneter snapshot */

void init()
{
    SysTick_Config(SysTick_LOAD_RELOAD_Msk); 
    /* we don't want ints to be enabled */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    timestamp = 0;
}

unsigned long get_timer(unsigned long base)
{
	unsigned long now = SysTick->VAL;

	if (lastdec >= now)
		timestamp += lastdec - now;
	else
		timestamp += lastdec + SysTick_LOAD_RELOAD_Msk - 1 - now;

	lastdec = now;

	return timestamp;
}

void reset_timer(void)
{
    lastdec = SysTick->VAL;
    timestamp = 0;
}

/* delay x useconds */
void __udelay(unsigned long usec)
{
	unsigned long tmo, tmp;

	if (usec >= 1000) {
		/*
		 * if "big" number, spread normalization
		 * to seconds
		 * 1. start to normalize for usec to ticks per sec
		 * 2. find number of "ticks" to wait to achieve target
		 * 3. finish normalize.
		 */
		tmo = usec / 1000;
		tmo *= (CONFIG_SYS_HZ * (SysTick_LOAD_RELOAD_Pos - 1) / 10);
		tmo /= 1000;
	} else {
		/* else small number, don't kill it prior to HZ multiply */
		tmo = usec * CONFIG_SYS_HZ * (SysTick_LOAD_RELOAD_Pos - 1) / 10;
		tmo /= (1000 * 1000);
	}

	/* get current timestamp */
	tmp = get_timer(0);

	/* if setting this fordward will roll time stamp */
	/* reset "advancing" timestamp to 0, set lastdec value */
	/* else, set advancing stamp wake up time */
	if ((tmo + tmp + 1) < tmp)
		reset_timer();
	else
		tmo += tmp;

	/* loop till event */
	while (get_timer(0) < tmo)
		;	/* nop */
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
unsigned long get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}
