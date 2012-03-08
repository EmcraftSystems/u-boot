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

#include "soc.h"

/*
 * Memory Protection Unit (MPU) registers map
 *
 * See Chapter 18 of the K60 Reference Manual (page 409)
 */
struct kinetis_mpu_regs {
	u32	cesr;
};

/*
 * MPU registers base
 */
#define KINETIS_MPU_BASE	(KINETIS_AIPS0PERIPH_BASE + 0x0000D000)
#define KINETIS_MPU		((volatile struct kinetis_mpu_regs *) \
				KINETIS_MPU_BASE)

/*
 * Cache interface registers map
 *
 * See Chapter 24 of the K70 Reference Manual
 */
struct kinetis_cache_regs {
	u32	ccr;	
	u32	clcr;	
	u32	csar;	
	u32	ccvr;	
	u32	unused[4];
	u32	crmr;	
};

/*
 * Cache interface registers base
 */
#define KINETIS_PC_CACHE_BASE	0xE0082000
#define KINETIS_PC_CACHE	((volatile struct kinetis_cache_regs *) \
				KINETIS_PC_CACHE_BASE)
#define KINETIS_PS_CACHE_BASE	0xE0082800
#define KINETIS_PS_CACHE	((volatile struct kinetis_cache_regs *) \
				KINETIS_PS_CACHE_BASE)

/*
 * Enable the caches if so configured
 */
static void kinetis_cache_init(void)
{
#if defined(CONFIG_KINETIS_PC_CACHE_ON)
	/*
	 * This sets up the following cache modes for the I/D bus cache:
	 * - on-chip Flash => write-through
	 * - DRAM controller (0x08000000) => write-through
	 * - FlexNVM => non-cacheable
	 * - FlexBus (Alias) => non-cacheable
	 */
	KINETIS_PC_CACHE->crmr = 0xA0000000;

	/*
	 * This invalidates the I/D bus cache and 
	 * and then enables the cache and write buffer
	 */
	KINETIS_PC_CACHE->ccr = 0x85000003;
	while (KINETIS_PC_CACHE->ccr & 0x80000000);
#endif

#if defined(CONFIG_KINETIS_PS_CACHE_ON)
	/*
	 * This sets up the following cache modes for the System bus cache:
	 * - FlexBus (external memory - write-back) => non-cacheable
	 * - DRAM controller (0x70000000) => write-back
	 * - DRAM controller (0x80000000) => non-cacheable
	 * - FlexBus (external memory - write-through) => non-cacheable
	 */
	KINETIS_PS_CACHE->crmr = 0x00030000;

	/*
	 * This invalidates the System bus cache and 
	 * and then enables the cache and write buffer
	 */
	KINETIS_PS_CACHE->ccr = 0x85000003;
	while (KINETIS_PS_CACHE->ccr & 0x80000000);
#endif
}

/*
 * SoC configuration code that cannot be put into drivers
 */
#ifdef CONFIG_ARMCORTEXM3_SOC_INIT
void cortex_m3_soc_init(void)
{
#ifdef CONFIG_MCFFEC
	/*
	 * Enable the clock on the Ethernet module of the MCU
	 */
	kinetis_periph_enable(KINETIS_CG_ENET, 1);
#endif /* CONFIG_MCFFEC */

#ifdef CONFIG_CMD_NAND
	/*
	 * Enable the clock on the NAND Flash Controller module of the MCU
	 */
	kinetis_periph_enable(KINETIS_CG_NFC, 1);
#endif /* CONFIG_CMD_NAND */

	/*
	 * Disable the MPU to let the Ethernet module access the SRAM
	 */
	KINETIS_MPU->cesr = 0;

	/*
	 * Enable the caches if so configured
	 */
	kinetis_cache_init();
}
#endif
