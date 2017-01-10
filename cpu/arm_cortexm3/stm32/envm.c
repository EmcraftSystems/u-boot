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
#include <asm/cache.h>

#include "envm.h"

/*
 * This array defines the layout of the Embedded Flash on the STM32 chips
 */
static u32 flash_bsize[] = {
#if defined(CONFIG_SYS_STM32F7)
	[0 ... 3]	=  32 * 1024,
	[4]		= 128 * 1024,
	[5 ... 7]	= 256 * 1024,
#if defined(CONFIG_SYS_STM32F76xxx) || defined(CONFIG_SYS_STM32F77xxx)
	[8 ... 11]	= 256 * 1024,
#endif
#else
	[0 ... 3]	=  16 * 1024,
	[4]		=  64 * 1024,
	[5 ... 11]	= 128 * 1024
# if defined(CONFIG_SYS_STM32F43X)
	,
	[12 ... 15]	=  16 * 1024,
	[16]		=  64 * 1024,
	[17 ... 23]	= 128 * 1024
# endif
#endif
	};

/*
 * Number of flash blocks for STM32F2x chips
 */
#define STM32_FLASH_BLOCKS		ARRAY_SIZE(flash_bsize)

/*
 * Flash registers base
 */
#define STM32_FLASHREGS_BASE		(STM32_AHB1PERIPH_BASE + 0x3C00)

/*
 * Flash register map
 */
struct stm32_flash_regs {
	u32	acr;			/* Access control		*/
	u32	keyr;			/* Key				*/
	u32	optkeyr;		/* Option key			*/
	u32	sr;			/* Status			*/
	u32	cr;			/* Control			*/
	u32	optcr;			/* Option control		*/
};
#define STM32_FLASH_REGS		((volatile struct stm32_flash_regs *) \
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
#define STM32_FLASH_CR_BLCK(v)		((v) << 7)
#define STM32_FLASH_CR_MER		(1 << 2)
#define STM32_FLASH_CR_SER		(1 << 1)
#define STM32_FLASH_CR_PG		(1 << 0)

/*
 * Flash ACR definitions
 */
#define STM32_FLASH_ACR_LAT_BIT		0		/* Latency           */
#define STM32_FLASH_ACR_LAT_MSK		0x7
#define STM32_FLASH_ACR_PRFTEN		(1 << 8)	/* Prefetch enable   */
#define STM32_FLASH_ACR_ICEN		(1 << 9)	/* Icache enable     */

/*
 * Flash KEYR definitions
 */
#define STM32_FLASH_KEYR_KEY1		0x45670123	/* KEY1 to unlock CR */
#define STM32_FLASH_KEYR_KEY2		0xCDEF89AB	/* KEY2 to unlock CR */

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
 * Given the flash address, return the block number.
 * Return error if the address is not a start block address.
 */

static s32 stm32_flash_get_block(u8 *addr)
{
	s32 i = 0;
	u8 *base = (u8 *)CONFIG_MEM_NVM_BASE;

	while (i < STM32_FLASH_BLOCKS) {
		if (addr == base)
			break;
		base += flash_bsize[i];
		i++;
	}

	if (i == STM32_FLASH_BLOCKS)
		i = -EINVAL;

	return i;
}

/*
 * Erase the embedded flash of the STM32. Start block is calculated from the
 * given offset, end block - from size.
 */
static s32 __attribute__((section(".ramcode")))
	     __attribute__ ((long_call))
stm32_flash_erase(u32 offset, u32 size)
{
	s32 ret;
	s32 n, k, num;
#if defined(CONFIG_SYS_STM32F43X)
	s32 v;
#endif
	u32 erasesize;

	if ((n = stm32_flash_get_block((u8 *)offset)) < 0) {
		printf("%s: Address %#x is not block-aligned\n", __func__,
			offset);
		ret = n;
		goto xit;
	}

	/* Calculate the number of blocks to erase */
	erasesize = 0;
	num = n;
	while (erasesize < size) {
		erasesize += flash_bsize[num];
		num++;
	}

	/* Check there is no pending operations */
	if (STM32_FLASH_REGS->sr & STM32_FLASH_SR_BSY) {
		printf("%s: Flash is busy\n", __func__);
		ret = -EBUSY;
		goto xit;
	}

	stm32_flash_cr_unlock();

	while (n < num) {
		STM32_FLASH_REGS->cr &= ~(STM32_FLASH_CR_SECT_MSK <<
					STM32_FLASH_CR_SECT_SHIFT);
		k = n;
#if defined(CONFIG_SYS_STM32F43X)
		v = k >= 12;
		if (v) {
			k -= 12;
		}
#endif
		STM32_FLASH_REGS->cr |= ((k << STM32_FLASH_CR_SECT_SHIFT) |
#if defined(CONFIG_SYS_STM32F43X)
					STM32_FLASH_CR_BLCK(v) |
#endif
					STM32_FLASH_CR_SER);
		STM32_FLASH_REGS->cr |= STM32_FLASH_CR_START;
		/*
		 * Warning! As soon as the erase operation starts, you can't
		 * access U-Boot functions except of marked as ".ramcode"!
		 */
		while (STM32_FLASH_REGS->sr & STM32_FLASH_SR_BSY)
			;
		n++;
	}

	STM32_FLASH_REGS->cr &= ~(STM32_FLASH_CR_SER |
			(STM32_FLASH_CR_SECT_MSK << STM32_FLASH_CR_SECT_SHIFT));
	stm32_flash_cr_lock();

	ret = 0;
xit:
	return ret;
}

static s32 __attribute__((section(".ramcode")))
	     __attribute__ ((long_call))
stm32_flash_program(u32 offset, void *buf, u32 size)
{
	u32 *src = (u32 *)buf;
	u32 *dst = (u32 *)offset;
	/* I can read 1-3 bytes beyond the input buffer, but this is OK */
	u32 words = (size + sizeof(u32) - 1) / sizeof(u32);
	s32 ret;

	/* No sanity check on flash address here, proceed to program */

	/* Check there is no pending operations */
	if (STM32_FLASH_REGS->sr & STM32_FLASH_SR_BSY) {
		ret = -EBUSY;
		goto xit;
	}
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
u32 __attribute__((section(".ramcode")))
	     __attribute__ ((long_call))
envm_write(u32 offset, void * buf, u32 size)
{
	s32 ret = 0;

#if defined(CONFIG_STM32F7_DCACHE_ON) || defined(CONFIG_STM32F7_ICACHE_ON)
	stm32f7_envm_as_dev();
#endif
	/* Basic sanity check. More checking in the "get_block" routine */
	if ((offset < CONFIG_MEM_NVM_BASE) ||
		((offset + size) > (CONFIG_MEM_NVM_BASE + CONFIG_MEM_NVM_LEN))) {
		printf("%s: Address %#x is not in flash"
			" or size %d is too big\n",
			__func__, offset, size);
		goto xit;
	}

	if (stm32_flash_erase(offset, size) < 0 ||
		(stm32_flash_program(offset, buf, size) < 0))
		goto xit;

	ret = size;
xit:
#if defined(CONFIG_STM32F7_DCACHE_ON) || defined(CONFIG_STM32F7_ICACHE_ON)
	stm32f7_envm_as_dev();
#endif
	return ret;
}
