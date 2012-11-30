/*
 * (C) Copyright 2012
 *
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
#include "soc.h"
#include "wdt.h"

#ifdef CONFIG_M2S_CACHE_ON
/*
 * Enable the on-cache chip of SF2
 */
static void soc_cache_enable(void)
{
	/*
	 * Enable cache
	 */
	M2S_SYSREG->cc_cr = 1 << 0;
}
#endif

/*
 * SoC configuration code that cannot be put into drivers
 */
void cortex_m3_soc_init(void)
{

#ifdef CONFIG_M2S_CACHE_ON

	/*
	 * Enable the on-chip cache
	 */
	soc_cache_enable();
#endif

#ifdef CONFIG_SYS_NS16550

	/*
	 * Reset UART0 and take it out of reset
	 */
	M2S_SYSREG->soft_reset_cr |= (1 << 7);
	M2S_SYSREG->soft_reset_cr &= ~(1 << 7);

	/* Initialize iomux for MSS UART0 */
	*(volatile int *)0x4003827C = 0xc;
	*(volatile int *)0x40038280 = 0x361;
#endif

#ifdef CONFIG_M2S_SPI
	/* Initialize iomux for SPI0 */
	*(volatile int *)0x400381F4 = 0x36C;
#endif
	/*
	 * Configure the memory protection unit (MPU) to allow full access to
	 * the whole 4GB address space.
	 *
	 * This is required, because in the default configuration code
	 * execution is not permitted at the addresses above 0xA0000000
	 * (including external DDR memory.)
	 */
	cortex_m3_mpu_full_access();
}
