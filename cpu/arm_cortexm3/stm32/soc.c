/*
 * (C) Copyright 2015
 *
 * Vladimir Skvortsov, Emcraft Systems, vskvortsov@emcraft.com
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


/* ARMv7-M registers */
#define _CCR		*((volatile u32*)(0xE000ED14))	/* Configuration and Control Register*/
/* CCR bis*/
#define CCR_IC		(1 << 17)	/* Instruction cache enable bit */
#define CCR_DC		(1 << 16)	/* Data cache enable bit */

#define _CLIDR		*((volatile u32*)(0xE000ED78))	/* Cache Level ID Register */
#define _CTR		*((volatile u32*)(0xE000ED7C))	/* Cache Type Register */
#define _CCSIDR		*((volatile u32*)(0xE000ED80))	/* Cache Size ID Registers */
#define _CSSELR		*((volatile u32*)(0xE000ED84))	/* Cache Size Selection Register */

/* decode CCSIDR bits */
#define CCSIDR_NSETS(x)		(((x) >> 13) & 0x7fff)	/* Number of sets (0-based) */
#define CCSIDR_ASC(x)		(((x) >> 3) & 0x3ff)	/* Associativity */
#define CCSIDR_LINESZ(x)	(((x) >> 0) & 0x7)	/* Line Size */

#define _ICIALLU	*((volatile u32*)(0xE000EF50))	/* Instruction cache invalidate all to Point of Unification (PoU) */
#define _DCISW		*((volatile u32*)(0xE000EF60))	/* Data cache invalidate by set/way */


#if defined(CONFIG_STM32F7_ICACHE_ON)
/* Invalidate Instruction cache */
static void invalidate_icache(void ) {
	_ICIALLU = 0;
	__asm__ volatile("dsb");
	__asm__ volatile("isb");
}
#endif

#if defined(CONFIG_STM32F7_DCACHE_ON)
/* Invalidate Data cache */
static void invalidate_dcache(void ) {
	u32 ccsidr;
	u32 nsets, asc, linesz;
	int wshift, sshift;
	u32 set, way;

	_CSSELR = 0;		/* select D-cache*/
	__asm__ volatile("dsb");
	ccsidr = _CCSIDR;	/* read size of D-cache */

	/* Parse CCSIDR */
	nsets = CCSIDR_NSETS(ccsidr);
	asc = CCSIDR_ASC(ccsidr);
	linesz = CCSIDR_LINESZ(ccsidr);

	/* Calculate way and set shift offsets in DCISW */
	sshift = linesz + 4;
	__asm__ volatile("clz %0, %1" : "=r" (wshift) : "r" (asc));

	for (set = 0; set <= nsets; set++) {
		for (way = 0; way <= asc; way++) {
			u32 sw = (way << wshift) | (set << sshift) | (0 << 1);
			/* Invalidate D-cache line */
			_DCISW = sw;
		}
	}
	__asm__ volatile("dsb");
	__asm__ volatile("isb");
}
#endif

#if defined(CONFIG_STM32F7_DCACHE_ON)
static void config_cached_regions(void) {

	cortex_m3_mpu_enable(0);
	/* Whole 4GB space */
	cortex_m3_mpu_add_region(0,
				 0x00000000 | 1 << 4,
				 0<<28 | 3 <<24 | 0<<19 | 0<<18 | 0<<17 | 0<<16 | 0<<8 | 31<<1 | 1<<0);
	/* SDRAM (cached) */
	cortex_m3_mpu_add_region(1,
				 CONFIG_SYS_RAM_BASE | 1 << 4 | 1 << 0,
				 0<<28 | 3 <<24 | 0<<19 | 0<<18 | 1<<17 | 0<<16 | 0<<8 | 24<<1 | 1<<0);
	cortex_m3_mpu_enable(1);

}
#endif


#if defined(CONFIG_STM32F7_ICACHE_ON) || defined(CONFIG_STM32F7_DCACHE_ON)
/* Enable Data and Instruction caches */
static void stm32f7_enable_cache(void)
{
	u32 ccr;
#if defined(CONFIG_STM32F7_DCACHE_ON)
	/* Configure regions prior to enable cache. */
	config_cached_regions();
#endif

	/* Invalidate caches prior to enable */
#if defined(CONFIG_STM32F7_ICACHE_ON)
	invalidate_icache();
#endif

#if defined(CONFIG_STM32F7_DCACHE_ON)
	invalidate_dcache();
#endif

	/* Enable caches. */
	ccr = _CCR;
#if defined(CONFIG_STM32F7_ICACHE_ON)
	ccr |= CCR_IC;
#endif
#if defined(CONFIG_STM32F7_DCACHE_ON)
	ccr |= CCR_DC;
#endif
	_CCR = ccr;
	__asm__ volatile("dsb");
	__asm__ volatile("isb");
}
#endif

/*
 * SoC configuration code that cannot be put into drivers
 */
#ifdef CONFIG_ARMCORTEXM3_SOC_INIT
void cortex_m3_soc_init(void)
{
#if defined(CONFIG_STM32F7_ICACHE_ON) || defined(CONFIG_STM32F7_DCACHE_ON)
	stm32f7_enable_cache();
#endif
}
#endif
