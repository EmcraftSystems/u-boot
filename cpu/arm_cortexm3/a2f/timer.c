/*
 * (C) Copyright 2010,2011
 * Sergei Poselenov, Emcraft Systems, sposelenov@emcraft.com
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <common.h>

/* system core clock /32 */
#define CONFIG_SYSTICK_FREQ	3125000

/* Internal tick units */
static unsigned long long timestamp;	/* Monotonic incrementing timer */
static ulong              lastdec;	/* Last decrementer snapshot */

int timer_init()
{
	volatile struct a2f_sysreg *a2f_sysreg =
		(volatile struct a2f_sysreg *)A2F_SYSREG_BASE;
	volatile struct a2f_timer *a2f_timer =
		(volatile struct a2f_timer *)A2F_TIMER_BASE;
	volatile struct cm3_systick *systick =
		(volatile struct cm3_systick *)CM3_SYSTICK_BASE;

	/* Release systimer from reset */
	a2f_sysreg->soft_rst_cr &= ~A2F_SOFT_RST_TIMER_SR;
	/* enable 32bit timer1 */
	a2f_timer->timer64_mode &= ~A2F_TIM64_64MODE_EN;
	/* timer1 is used by envm driver */
	a2f_timer->timer1_ctrl = A2F_TIM_CTRL_MODE_ONESHOT|A2F_TIM_CTRL_EN;
	/* No reference clock */
	a2f_sysreg->systick_cr &= ~A2F_SYSTICK_NOREF;
	/* div by 32 */
	a2f_sysreg->systick_cr |= (A2F_SYSTICK_STCLK_DIV_32 <<
				A2F_SYSTICK_STCLK_DIV_SHIFT);
	a2f_sysreg->systick_cr &= ~A2F_SYSTICK_TENMS_MSK;
	a2f_sysreg->systick_cr |= 0x7a12;

	systick->load = CM3_SYSTICK_LOAD_RELOAD_MSK - 1;
	systick->val = 0;
	/* we don't want ints to be enabled */
	systick->ctrl = CM3_SYSTICK_CTRL_EN;
	timestamp = 0;

	return 0;
}

ulong get_timer(ulong base)
{
	volatile struct cm3_systick *systick =
		(volatile struct cm3_systick *)CM3_SYSTICK_BASE;
	ulong now = systick->val;

	if (lastdec >= now)
		timestamp += lastdec - now;
	else
		timestamp += lastdec + CM3_SYSTICK_LOAD_RELOAD_MSK - 1 - now;

	lastdec = now;

	return timestamp / (CONFIG_SYSTICK_FREQ / 1000) - base;
}

void reset_timer(void)
{
	volatile struct cm3_systick *systick =
		(volatile struct cm3_systick *)(CM3_SYSTICK_BASE);
	lastdec = systick->val;
	timestamp = 0;
}

/* delay x useconds */
void __udelay(ulong usec)
{
	ulong clc, tmp;
	volatile struct cm3_systick *systick =
		(volatile struct cm3_systick *)(CM3_SYSTICK_BASE);


	clc = usec * (CONFIG_SYSTICK_FREQ / 1000000);

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
ulong get_tbclk(void)
{
	return CONFIG_SYSTICK_FREQ;
}
