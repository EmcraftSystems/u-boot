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

/*
 * Print the CPU specific information
 */
int print_cpuinfo(void)
{
	printf("CPU: %s\n", "STM32 F2 series (Cortex-M3)");

	return 0;
}

/*
 * Perform the low-level reset.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in eNMV.
 */
void __attribute__((section(".ramcode")))
		__attribute__ ((long_call))
		reset_cpu(ulong addr)
{
	/*
	 * TBD
	 */

	return;
}

/*
 * Dump the registers on an exception we don't know how to process.
 */
unsigned char cortex_m3_irq_vec_get(void)
{
	/*
	 * TBD
	 */

	return 0;
}
