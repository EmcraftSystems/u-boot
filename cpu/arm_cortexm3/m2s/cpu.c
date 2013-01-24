/*
 * (C) Copyright 2012
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
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
#include "clock.h"

/*
 * Print the CPU specific information
 */
int print_cpuinfo(void)
{
	char buf[CLOCK_END][32];

	printf("CPU  : %s\n", "SmartFusion2 SoC (Cortex-M3 Hard IP)");

	strmhz(buf[CLOCK_SYSTICK], clock_get(CLOCK_SYSTICK));
	strmhz(buf[CLOCK_DDR], clock_get(CLOCK_DDR));
	strmhz(buf[CLOCK_PCLK0], clock_get(CLOCK_PCLK0));
	strmhz(buf[CLOCK_PCLK1], clock_get(CLOCK_PCLK1));
	printf("Freqs: CORTEX-M3=%sMHz,PCLK0=%sMHz,PCLK1=%sMHz\n",
		buf[CLOCK_SYSTICK], buf[CLOCK_PCLK0], buf[CLOCK_PCLK1]);

	return 0;
}
