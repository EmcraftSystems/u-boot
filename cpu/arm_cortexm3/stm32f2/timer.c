/*
 * (C) Copyright 2011
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>

/* HCLK/8 */
#define SYSTICK_FREQ	(clock_get(CLOCK_HCLK)/8)

/* Internal tick units */
static unsigned long long timestamp;	/* Monotonic incrementing timer */
static ulong              lastdec;	/* Last decrementer snapshot */

/*
 * Init timer.
 */
int timer_init(void)
{
	volatile struct cm3_systick *systick =
		(volatile struct cm3_systick *)CM3_SYSTICK_BASE;

	systick->load = CM3_SYSTICK_LOAD_RELOAD_MSK - 1;
	systick->val = 0;
	/* Use external clock, no ints */
	systick->ctrl = CM3_SYSTICK_CTRL_EN;
	timestamp = 0;

	return 0;
}

/*
 * Return difference between timer ticks and 'base'.
 */
unsigned long get_timer(unsigned long base)
{
	volatile struct cm3_systick *systick =
		(volatile struct cm3_systick *)CM3_SYSTICK_BASE;
	ulong now = systick->val;

	if (lastdec >= now)
		timestamp += lastdec - now;
	else
		timestamp += lastdec + CM3_SYSTICK_LOAD_RELOAD_MSK - 1 - now;

	lastdec = now;

	return timestamp / (SYSTICK_FREQ / 1000) - base;
}

/*
 * Reset timer.
 */
void reset_timer(void)
{
	volatile struct cm3_systick *systick =
		(volatile struct cm3_systick *)(CM3_SYSTICK_BASE);
	lastdec = systick->val;
	timestamp = 0;
}

/*
 * Delay for 'usec' useconds.
 */
void __udelay(unsigned long usec)
{
	ulong clc, tmp;
	volatile struct cm3_systick *systick =
		(volatile struct cm3_systick *)(CM3_SYSTICK_BASE);


	clc = usec * (SYSTICK_FREQ / 1000000);

	/* get current timestamp */
	tmp = systick->val;

	if (tmp < clc) {
		/* loop till event */
		while (systick->val < tmp ||
			   systick->val > (CM3_SYSTICK_LOAD_RELOAD_MSK - 1 -
				clc + tmp)) ;	/* nop */
	} else {
		while (systick->val > (tmp - clc)) ;
	}
}

/*
 * This function is derived from PowerPC code (timebase clock frequency).
 * On ARM it returns the number of timer ticks per second.
 */
unsigned long get_tbclk(void)
{
	return SYSTICK_FREQ;
}
