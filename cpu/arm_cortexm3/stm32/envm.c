/*
 * STM32 Embedded flash programming support.
 *
 * (C) Copyright 2011
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 * Sergei Poselenov, Emcraft Systems, sposelenov@emcraft.com
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
#include <errno.h>
#include "envm.h"

/*
 * Flash data area definitions
 */
#define STM32_FLASH_BASE		0x08000000
#define STM32_FLASH_SIZE	        (8*128*1024)

/*
 * Flash registers base
 */
#define STM32_FLASHREGS_BASE		(STM32_AHB1PERITH_BASE + 0x3C00)

/*
 * Flash register map
 */
struct stm32_flash_regs {
	u32	acr;				/* Access control		*/
	u32	keyr;				/* Key				*/
	u32	optkeyr;			/* Option key			*/
	u32	sr;				/* Status			*/
	u32	cr;				/* Control			*/
	u32	optcr;				/* Option control		*/
};
#define STM32_FLASH_REGS	((volatile struct stm32_flash_regs *)		\
			 STM32_FLASHREGS_BASE)

 /*
 * Flash CR definitions
 */
#define STM32_FLASH_CR_LOCK		(1 << 31)
#define STM32_FLASH_CR_ERRIE		(1 << 25)
#define STM32_FLASH_CR_EOPIE		(1 << 24)
#define STM32_FLASH_CR_START		(1 << 16)
#define STM32_FLASH_CR_PSIZE_SHIFT	8
#define STM32_FLASH_CR_PSIZE_MSK	0x3
#define STM32_FLASH_CR_PSIZE_32x	(2 << STM32_FLASH_CR_PSIZE_SHIFT)
#define STM32_FLASH_CR_SECT_SHIFT	3
#define STM32_FLASH_CR_SECT_MSK		0xf
#define STM32_FLASH_CR_MER		(1 << 2)
#define STM32_FLASH_CR_SER		(1 << 1)
#define STM32_FLASH_CR_PG		(1 << 0)

/*
 * Flash ACR definitions
 */
#define STM32_FLASH_ACR_LAT_BIT		0         /* Latency			*/
#define STM32_FLASH_ACR_LAT_MSK		0x3
#define STM32_FLASH_ACR_PRFTEN		(1 << 8)  /* Prefetch enable		*/
#define STM32_FLASH_ACR_ICEN		(1 << 9)  /* Instruction cache enable	*/

/*
 * Flash KEYR definitions
 */
#define STM32_FLASH_KEYR_KEY1		0x45670123  /* KEY1 value to unlock CR	*/
#define STM32_FLASH_KEYR_KEY2		0xCDEF89AB  /* KEY2 value to unlock CR	*/

/*
 * Flash SR definitions
 */
#define STM32_FLASH_SR_BSY		(1 << 16)
#define STM32_FLASH_SR_PGSERR		(1 << 7)
#define STM32_FLASH_SR_PGPERR		(1 << 6)
#define STM32_FLASH_SR_PGAERR		(1 << 5)
#define STM32_FLASH_SR_WRPERR		(1 << 4)
#define STM32_FLASH_SR_OPERR		(1 << 1)
#define STM32_FLASH_SR_EOP		(1 << 0)

/*
 * Unlock the Flash Command Register
 */
static void __attribute__((section(".ramcode")))
	     __attribute__ ((long_call))
stm32_flash_cr_unlock(void)
{
	if (STM32_FLASH_REGS->cr & STM32_FLASH_CR_LOCK) {
		STM32_FLASH_REGS->keyr = STM32_FLASH_KEYR_KEY1;
		STM32_FLASH_REGS->keyr = STM32_FLASH_KEYR_KEY2;
	}
}

/*
 * Lock the Flash Command Register
 */
static void __attribute__((section(".ramcode")))
	     __attribute__ ((long_call))
stm32_flash_cr_lock(void)
{
	STM32_FLASH_REGS->cr |= STM32_FLASH_CR_LOCK;
}

/*
 * Erase the whole embedded flash of the STM32.
 */
static int __attribute__((section(".ramcode")))
	     __attribute__ ((long_call))
stm32_flash_erase(uint32_t offset, uint32_t size)
{
	int32_t ret = -EBUSY;
	/* No sanity check of address here, proceed to erase */

	/* Check there is no pending operations */
	if (STM32_FLASH_REGS->sr & STM32_FLASH_SR_BSY) {
		printf("%s: Flash is busy\n", __func__);
		goto xit;
	}
	stm32_flash_cr_unlock();

	STM32_FLASH_REGS->cr |= STM32_FLASH_CR_MER;
	STM32_FLASH_REGS->cr |= STM32_FLASH_CR_START;

	/*
	 * Warning! As soon as the erase operation starts, you can't access
	 * U-Boot functions except of marked as ".ramcode".
	 */
	while (STM32_FLASH_REGS->sr & STM32_FLASH_SR_BSY)
		;

	STM32_FLASH_REGS->cr &= ~STM32_FLASH_CR_MER;
	stm32_flash_cr_lock();

	ret = 0;
xit:
	return ret;
}

static int __attribute__((section(".ramcode")))
	     __attribute__ ((long_call))
stm32_flash_program(uint32_t offset, void *buf, uint32_t size)
{
	uint32_t *src = (uint32_t *)buf;
	uint32_t *dst = (uint32_t *)offset;
	/* I know I can read 1-3 bytes beyond the input buffer, but this is OK */
	uint32_t words = (size + sizeof(uint32_t) - 1)/ sizeof(uint32_t);
	int32_t ret = -EBUSY;

	/* No sanity check on flash address here, proceed to program */

	/* Check there is no pending operations */
	if (STM32_FLASH_REGS->sr & STM32_FLASH_SR_BSY)
		goto xit;

	stm32_flash_cr_unlock();
	STM32_FLASH_REGS->cr |= STM32_FLASH_CR_PG;

	/* Since we are using 32x parallelism (set in CR), copy by 4 bytes */
	while (words--) {
		*dst++ = *src++;

		while (STM32_FLASH_REGS->sr & STM32_FLASH_SR_BSY)
			;
	}
	STM32_FLASH_REGS->cr &= ~STM32_FLASH_CR_PG;
	stm32_flash_cr_lock();

	ret = 0;
xit:
	return ret;
}

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
	if (wait_states > STM32_FLASH_ACR_LAT_MSK)
		wait_states = STM32_FLASH_ACR_LAT_MSK;

	STM32_FLASH_REGS->acr = STM32_FLASH_ACR_PRFTEN |
			  STM32_FLASH_ACR_ICEN |
			  (wait_states << STM32_FLASH_ACR_LAT_BIT);
}

/*
 * Initialize internal Flash interface
 */
void envm_init(void)
{
	stm32_flash_cr_unlock();
	/* Clear ints, set the parallelism */
	STM32_FLASH_REGS->cr &= ~(STM32_FLASH_CR_PSIZE_MSK |
			STM32_FLASH_CR_ERRIE | STM32_FLASH_CR_EOPIE);
	STM32_FLASH_REGS->cr |= STM32_FLASH_CR_PSIZE_32x;
	stm32_flash_cr_lock();
}

/*
 * Write a data buffer to internal Flash.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in internal Flash.
 */
unsigned int __attribute__((section(".ramcode")))
	     __attribute__ ((long_call))
envm_write(uint32_t offset, void * buf, uint32_t size)
{
	int32_t ret = 0;

	/* Sanity check */
	if ((offset < STM32_FLASH_BASE) ||
		((offset + size) > (STM32_FLASH_BASE + STM32_FLASH_SIZE))) {
		printf("Offset %#x is not in flash or size %d is too big\n", offset, size);
		goto xit;
	}

	if (stm32_flash_erase(offset, size) < 0 ||
		(stm32_flash_program(offset, buf, size) < 0))
		goto xit;

	ret = size;
xit:
	return ret;
}
