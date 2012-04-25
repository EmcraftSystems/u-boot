/*
 * (C) Copyright 2012
 *
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
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

#include <asm/arch/lpc18xx.h>
#include "clock.h"

/*
 * Print the CPU specific information
 */
int print_cpuinfo(void)
{
	char buf[2][32];

#if defined(CONFIG_SYS_ARMCORTEXM4)
	printf("CPU  : %s\n", "LPC43xx series (Cortex-M4/M0)");
#else
	printf("CPU  : %s\n", "LPC18xx series (Cortex-M3)");
#endif

	strmhz(buf[0], clock_get(CLOCK_SYSTICK));
	strmhz(buf[1], clock_get(CLOCK_CCLK));
	printf("Freqs: SYSTICK=%sMHz,CCLK=%sMHz\n",
		buf[0], buf[1]);

	return 0;
}
