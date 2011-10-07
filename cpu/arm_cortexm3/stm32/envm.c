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
#include <asm/arch/stm32.h>

#include "envm.h"

/*
 * Flash registers base
 */
#define STM32F2_FLASH_BASE		(STM32F2_AHB1PERITH_BASE + 0x3C00)

/*
 * Flash ACR definitions
 */
#define STM32F2_FLASH_ACR_LAT_BIT	0	/* Latency		     */
#define STM32F2_FLASH_ACR_LAT_MSK	0x3

#define STM32F2_FLASH_ACR_PRFTEN	(1 << 8)  /* Prefetch enable	     */
#define STM32F2_FLASH_ACR_ICEN		(1 << 9)  /* Instruction cache enable*/

/*
 * Flash register map
 */
struct stm32f2_flash_regs {
	u32	acr;				/* Access control	      */
	u32	keyr;				/* Key			      */
	u32	optkeyr;			/* Option key		      */
	u32	sr;				/* Status		      */
	u32	cr;				/* Control		      */
	u32	optcr;				/* Option control	      */
};
#define STM32F2_FLASH	((volatile struct stm32f2_flash_regs *)		       \
			 STM32F2_FLASH_BASE)

/*
 * Enable instruction cache, prefetch and set the Flash wait latency
 * according to the clock configuration used (HCLK value).
 * We _must_ do this before changing System clock source (or will crash on
 * fetching instructions of while() wait cycle).
 * In case of HSI clock - no Sys clock change happens, but, for consistency,
 * we configure Flash this way as well.
 */
void envm_config(u32 wait_states)
{
	if (wait_states > STM32F2_FLASH_ACR_LAT_MSK)
		wait_states = STM32F2_FLASH_ACR_LAT_MSK;

	STM32F2_FLASH->acr = STM32F2_FLASH_ACR_PRFTEN |
			     STM32F2_FLASH_ACR_ICEN |
			     (wait_states << STM32F2_FLASH_ACR_LAT_BIT);
}

/*
 * Initialize internal Flash interface
 */
void envm_init(void)
{
	/*
	 * TBD
	 */

	return;
}

/*
 * Write a data buffer to internal Flash.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in internal Flash.
 */
unsigned int __attribute__((section(".ramcode")))
	     __attribute__ ((long_call))
  envm_write(unsigned int offset, void * buf, unsigned int size)
{
	/*
	 * TBD
	 */

	return 0;
}
