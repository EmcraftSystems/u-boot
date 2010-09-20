#include "CMSIS/core_cm3.h"

void init()
{
    unsigned int ticks;
    /* we want the timer to decrement every 1 ms, so 
       let's calculate amount of ticks per 1 ms */
    if (!(SysTick->CALIB & 0xFFFFFFFF)) {
	ticks = SysTick->CALIB / 10;
    } else {
	ticks = g_FrequencyPCLK0 / 1000;
    }
    SysTick_Config(ticks); 
    /* we don't want ints to be enabled */
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
}


/* delay x useconds */
void __udelay(unsigned long usec)
{
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
unsigned long get_tbclk(void)
{
	return CONFIG_SYS_HZ;
}
