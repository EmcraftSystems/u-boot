/*
 * (C) Copyright 2011
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

#include <asm/arch/lpc178x.h>
#include <asm/arch/lpc178x_eth.h>
#include "clock.h"

/*
 * Print the CPU specific information
 */
int print_cpuinfo(void)
{
	char buf[3][32];

	printf("CPU  : %s\n", "LPC178x/7x series (Cortex-M3)");

	strmhz(buf[0], clock_get(CLOCK_SYSTICK));
	strmhz(buf[1], clock_get(CLOCK_EMCCLK));
	strmhz(buf[2], clock_get(CLOCK_PCLK));
	printf("Freqs: SYSTICK=%sMHz,EMCCLK=%sMHz,PCLK=%sMHz\n",
		buf[0], buf[1], buf[2]);

	return 0;
}

/*
 * Prepare for software reset
 *
 * This function will be called from `reset_cpu()`, therefore this should also
 * be in `.ramcode`.
 */
void __attribute__((section(".ramcode")))
     __attribute__ ((long_call))
lpc178x_pre_reset_cpu(void)
{
#ifdef CONFIG_LPC178X_ETH
	/*
	 * If we do not perform a PHY reset immediately before SYSRESET
	 * (the `cpu_reset()` call), then the Ethernet block will hang
	 * after this software reset.
	 */
	lpc178x_phy_final_reset();
#endif
}
