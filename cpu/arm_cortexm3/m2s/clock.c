/*
 * Copyright (C) 2012
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <common.h>
#include <asm/io.h>
#include "clock.h"

/*
 * Array of various reference clocks
 */
static unsigned long clock[CLOCK_END];

/*
 * Kick-off the MPLL of SmartFusion2
 */
static void clock_mss_init(void)
{

	/*
	 * Analog voltage = 3.3v. Libero appears to ignore this
	 * setting and defines this as 2.5v regardless.
	 * We change it dynamically.
	 */
	M2S_SYSREG->mssddr_pll_status_high_cr &= ~(1<<2);

	/*
	 * Wait for fabric PLL to lock. MPPL is getting clock from FPGA PLL.
	 */
	while (!(M2S_SYSREG->mssddr_pll_status & (1<<0)));

	/*
	 * Negate MPLL bypass.
	 */
	M2S_SYSREG->mssddr_pll_status_high_cr &= ~(1<<0);

	/*
	 * Wait for MPLL to lock.
	 */
	while (!(M2S_SYSREG->mssddr_pll_status & (1<<1)));

	/*
	 * Drive M3, PCLK0, PCLK1 from stage 2 dividers.
	 * This is what enables the MPLL.
	 */
	M2S_SYSREG->mssddr_facc1_cr &= ~(1<<12);
}

/*
 * Calculate the divisor for a specified FACC1 field
 * @param r		FACC1 value
 * @param s		FACC1 divisor field
 * @returns		divisor
 */
static unsigned int clock_mss_divisor(unsigned int r, unsigned int s)
{
	unsigned int v, ret;

	/*
 	 * Get a 3-bit field that defines the divisor
 	 */
	v = (r & (0x7<<s)) >> s;

	/*
	 * Translate the bit representation of the divisor to 
	 * a value ready to be used in calculation of a clock.
	 */
	switch (v) {
	case 0: ret = 1; break;
	case 1: ret = 2; break;
	case 2: ret = 4; break;
	case 4: ret = 8; break;
	case 5: ret = 16; break;
	case 6: ret = 32; break;
	default: ret = 1; break;
	}

	return ret;
}

/*
 * Perform reference clocks learning
 */
static void clock_mss_learn(void)
{
	unsigned int r1 = M2S_SYSREG->mssddr_facc1_cr;
	unsigned int r2 = M2S_SYSREG->mssddr_pll_status_low_cr;

	/*
	 * System reference clock is defined as a build-time constant.
	 * This clock comes from the FPGA PLL and we can't determine
	 * its value at run time. All clocks derived from CLK_BASE
	 * can be calculated at run time (and we do just that).
	 */
	clock[CLOCK_SYSREF] = CONFIG_SYS_M2S_SYSREF;

	/*
	 * Respectively:
	 * M3_CLK_DIVISOR
	 * DDR
	 * APB0_DIVISOR
	 * APB1_DIVISOR
	 * FIC32_0_DIVISOR
	 */
	clock[CLOCK_SYSTICK] = clock[CLOCK_SYSREF] / clock_mss_divisor(r1, 9);
	clock[CLOCK_DDR] = clock[CLOCK_SYSREF] / clock_mss_divisor(r2, 16);
	clock[CLOCK_PCLK0] = clock[CLOCK_SYSREF] / clock_mss_divisor(r1, 2);
	clock[CLOCK_PCLK1] = clock[CLOCK_SYSREF] / clock_mss_divisor(r1, 5);
	clock[CLOCK_FPGA] = clock[CLOCK_SYSREF] / clock_mss_divisor(r1, 13);
}

/*
 * Initialize the various clocks
 */
void clock_init(void)
{
	/*
	 * Initialize the MSS MPLL/CCC
	 */
	clock_mss_init();

	/*
	 * Perform clock learning.
	 */
	clock_mss_learn();
}

/*
 * Get the value of a specified reference clock
 * @param clck		reference clock
 * @returns		value of the clock
 */
ulong clock_get(enum clock clck)
{
	ulong res = 0;

	if (clck >= 0 && clck < CLOCK_END) {
		res = clock[clck];
	}

	return res;
}
