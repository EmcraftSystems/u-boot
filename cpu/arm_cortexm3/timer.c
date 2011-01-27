/*
 * (C) Copyright 2010,2011
 *
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <common.h>

struct systick {
	uint32_t ctrl;		/* Control and Status Register */
	uint32_t load;		/* Reload Value Register       */
	uint32_t val;		/* Current Value Register      */
	uint32_t cal;		/* Calibration Register        */
};

/* system core clock /32 */
#define CONFIG_SYSTICK_FREQ		3125000

/* SysTick Base Address */
#define A2F_SYSTICK_BASE	(A2F_SCS_BASE +  0x0010)

#define A2F_SYSTICK		((volatile struct systick *)(A2F_SYSTICK_BASE))

#define SYSTICK_LOAD_RELOAD_POS		0
#define SYSTICK_CTRL_ENABLE_POS		0
#define SYSTICK_LOAD_RELOAD_MSK		(0xFFFFFFul << SYSTICK_LOAD_RELOAD_POS)
#define SYSTICK_CTRL_ENABLE_MSK		(1ul << SYSTICK_CTRL_ENABLE_POS)

/* Internal tick units */
static unsigned long long timestamp;	/* Monotonic incrementing timer */
static unsigned long lastdec;	/* Last decremneter snapshot */

int timer_init()
{
	/*
	 * Initialize timer
	 */
	A2F_SYSREG->soft_rst_cr &= ~(1 << 6); /* Release systimer from reset */
	A2F_TIMER->timer64_mode = 0;   /* enable 32bit timer1 */
	A2F_TIMER->timer1_ctrl = 0x03; /* timer1 is used by envm driver */

	A2F_SYSREG->systick_cr &= ~(1 << 25);	/* en noref */
	A2F_SYSREG->systick_cr |= (3 << 28);	/* div by 32 */
	A2F_SYSREG->systick_cr &= ~0xffffff;
	A2F_SYSREG->systick_cr |= 0x7a12;
	A2F_SYSTICK->load = SYSTICK_LOAD_RELOAD_MSK - 1;
	A2F_SYSTICK->val = 0;
	/* we don't want ints to be enabled */
	A2F_SYSTICK->ctrl = SYSTICK_CTRL_ENABLE_MSK;
	timestamp = 0;

	return 0;
}

unsigned long get_timer(unsigned long base)
{
	unsigned long now = A2F_SYSTICK->val;

	if (lastdec >= now)
		timestamp += lastdec - now;
	else
		timestamp += lastdec + SYSTICK_LOAD_RELOAD_MSK - 1 - now;

	lastdec = now;

	return timestamp / (CONFIG_SYSTICK_FREQ / 1000) - base;
}

void reset_timer(void)
{
	lastdec = A2F_SYSTICK->val;
	timestamp = 0;
}

/* delay x useconds */
void __udelay(unsigned long usec)
{
	unsigned long clc, tmp;

	clc = usec * (CONFIG_SYSTICK_FREQ / 1000000);

	/* get current timestamp */
	tmp = A2F_SYSTICK->val;

	if (tmp < clc) {
		/* loop till event */
		while (A2F_SYSTICK->val < tmp ||
			   A2F_SYSTICK->val > (SYSTICK_LOAD_RELOAD_MSK - 1 -
				clc + tmp)) ;	/* nop */
	} else {
		while (A2F_SYSTICK->val > (tmp - clc)) ;
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
