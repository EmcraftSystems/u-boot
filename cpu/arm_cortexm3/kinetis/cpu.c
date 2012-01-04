/*
 * (C) Copyright 2011, 2012
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

#include <asm/arch/kinetis.h>
#include "clock.h"

/*
 * Print the CPU specific information
 */
int print_cpuinfo(void)
{
	char buf[4][32];

	printf("CPU  : %s\n", "Freescale Kinetis series (Cortex-M4)");

	strmhz(buf[0], clock_get(CLOCK_SYSTICK));
	strmhz(buf[1], clock_get(CLOCK_CCLK));
	strmhz(buf[2], clock_get(CLOCK_PCLK));
	strmhz(buf[3], clock_get(CLOCK_MACCLK));
	printf("Freqs: SYSTICK=%sMHz,CCLK=%sMHz,PCLK=%sMHz,MACCLK=%sMHz\n",
		buf[0], buf[1], buf[2], buf[3]);

	return 0;
}
