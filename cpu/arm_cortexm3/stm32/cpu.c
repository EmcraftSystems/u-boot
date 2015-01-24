/*
 * (C) Copyright 2011,2012
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

#include <asm/arch/stm32.h>
#include "clock.h"

/*
 * Print the CPU specific information
 */
int print_cpuinfo(void)
{
	char	buf[4][32];

#if defined(CONFIG_SYS_STM32F7)
	printf("CPU  : %s\n", "STM32F7 (Cortex-M7)");
#else
#if defined(CONFIG_SYS_ARMCORTEXM4)
	printf("CPU  : %s\n", "STM32F4 (Cortex-M4)");
#else
	printf("CPU  : %s\n", "STM32F2 (Cortex-M3)");
#endif
#endif

	strmhz(buf[0], clock_get(CLOCK_SYSCLK));
	strmhz(buf[1], clock_get(CLOCK_HCLK));
	strmhz(buf[2], clock_get(CLOCK_PCLK1));
	strmhz(buf[3], clock_get(CLOCK_PCLK2));
	printf("Freqs: SYSCLK=%sMHz,HCLK=%sMHz,PCLK1=%sMHz,PCLK2=%sMHz\n",
		buf[0], buf[1], buf[2], buf[3]);

	return 0;
}
