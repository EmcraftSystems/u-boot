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
/* Configuration and Control Register */
#define _CCR		*((volatile u32*)(0xE000ED14))
/* CCR bis*/
/* Instruction cache enable bit */
#define CCR_IC		(1 << 17)
#define CCR_DC		(1 << 16)

/* Cache Level ID Register */
#define _CLIDR		*((volatile u32*)(0xE000ED78))
/* Cache Type Register */
#define _CTR		*((volatile u32*)(0xE000ED7C))
/* Cache Size ID Registers */
#define _CCSIDR		*((volatile u32*)(0xE000ED80))
/* Cache Size Selection Register */
#define _CSSELR		*((volatile u32*)(0xE000ED84))

/* decode CCSIDR bits */
/* Number of sets (0-based) */
#define CCSIDR_NSETS(x)		(((x) >> 13) & 0x7fff)
/* Associativity */
#define CCSIDR_ASC(x)		(((x) >> 3) & 0x3ff)
/* Line Size */
#define CCSIDR_LINESZ(x)	(((x) >> 0) & 0x7)

/* Instruction cache invalidate all to Point of Unification (PoU) */
#define _ICIALLU	*((volatile u32*)(0xE000EF50))
/* Data cache invalidate by set/way */
#define _DCISW		*((volatile u32*)(0xE000EF60))
/* Data cache clean by address to PoU */
#define _DCCMVAU	*((volatile u32*)(0xE000EF64))
/* Instruction cache invalidate by address to PoU */
#define _ICIMVAU	*((volatile u32*)(0xE000EF58))
/* Branch predictor invalidate all */
#define _BPIALL		*((volatile u32*)(0xE000EF78))

/*
 * MPU regions. The order below is important: if regions overlap, then
 * the region with high number is more prioritive (in the sense of settings
 * used for this region)
 */
enum mpu_rgn {
	MPU_RGN_4GB		= 0,	/* 4GB address space region */
	MPU_RGN_SDRAM_CA,		/* Cacheable SDRAM */
	MPU_RGN_SDRAM_NC,		/* Non-cacheable SDRAM */
	MPU_RGN_ENVM_CA,		/* Cacheable eNVM */

	MPU_RGN_MAX		= 15	/* RBAR[REGION] bits; actually - less */
};

#if defined(CONFIG_STM32F7_ICACHE_ON)
/* Invalidate Instruction cache */
static void invalidate_icache(void)
{
	_ICIALLU = 0;
	__asm__ volatile("dsb");
	__asm__ volatile("isb");
}
#endif

#if defined(CONFIG_STM32F7_DCACHE_ON)
/* Invalidate Data cache */
static void invalidate_dcache(void)
{
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

#if defined(CONFIG_STM32F7_DCACHE_ON) || defined(CONFIG_STM32F7_ICACHE_ON)
/*
 * To be able to program eNVM we must reconfigure the appropriate MPU region to
 * 'Strongly-ordered' or 'Device' memory type. Switching cache off isn't enough:
 * we've got garbage in eNVM when programming it as a 'Normal' memory.
 * So:
 * - when 'dev' = 0, then configure 1MB eNVM as Normal memory with 'Write-back;
 *   write and read allocate' cache attributes;
 * - when 'dev' = 0, then configure eNVM as 'Stringly ordered' mem.
 */
static void stm32f7_envm_mpu_cfg(u8 dev, u8 mpu_run)
{
	if (mpu_run)
		cortex_m3_mpu_enable(0);

	cortex_m3_mpu_set_region(MPU_RGN_ENVM_CA,
		CONFIG_MEM_NVM_BASE | 1 << 4 | MPU_RGN_ENVM_CA << 0,
		0 << 28 |  3 << 24 |
		(dev ? (0 << 19 |  0 << 18 | 0 << 17 | 0 << 16) :
		       (1 << 19 |  0 << 18 | 1 << 17 | 1 << 16)) |
		0 <<  8 | 19 <<  1 | 1 <<  0);

	if (mpu_run)
		cortex_m3_mpu_enable(1);

	__asm__ volatile("dsb");
	__asm__ volatile("isb");
}

/*
 * Configure eNVM as 'Strongly ordered' memory type
 */
void stm32f7_envm_as_dev(void)
{
	stm32f7_envm_mpu_cfg(1, 1);
}

/*
 * Configure eNVM as 'Normal' memory type
 */
void stm32f7_envm_as_mem(void)
{
	stm32f7_envm_mpu_cfg(0, 1);
}
#endif

#if defined(CONFIG_SYS_STM32F7)
/*
 * Configure MPU.
 * We don't want to use common cortex_m3_mpu_full_access() to avoid messing
 * with region indexes in case if some changes to cortex_m3_mpu_full_access()
 * in the future.
 */
static void stm32f7_mpu_config(void)
{
	/*
	 * Make the whole 4GB space as 'Strongly-ordered'
	 */
	cortex_m3_mpu_set_region(MPU_RGN_4GB,
		0x00000000 | 1 << 4 | MPU_RGN_4GB << 0,
		0 << 28 |  3 << 24 |
		0 << 19 |  0 << 18 | 0 << 17 | 0 << 16 |
		0 <<  8 | 31 <<  1 | 1 <<  0);

	/*
	 * Configure 64MB SDRAM region as Normal memory with the appropriate
	 * cacheability attributes
	 */
	cortex_m3_mpu_set_region(MPU_RGN_SDRAM_CA,
		CONFIG_SYS_RAM_BASE | 1 << 4 | MPU_RGN_SDRAM_CA << 0,
		0 << 28 |  3 << 24 |
#if defined(CONFIG_STM32F7_DCACHE_ON) || defined(CONFIG_STM32F7_ICACHE_ON)
		/* Write-back; write and read allocate */
		1 << 19 |  0 << 18 | 1 << 17 | 1 << 16 |
#else
		/* Non-cacheable */
		1 << 19 |  0 << 18 | 0 << 17 | 0 << 16 |
#endif
		0 <<  8 | 25 <<  1 | 1 <<  0);

#if defined(CONFIG_STM32F7_DCACHE_ON) || defined(CONFIG_STM32F7_ICACHE_ON)
	stm32f7_envm_mpu_cfg(0, 0);
#endif

	/*
	 * We don't enable cache for SRAM because some device drivers
	 * put buffer descriptors and DMA buffers there.
	 */
	cortex_m3_mpu_enable(1);
}
#endif

#if defined(CONFIG_STM32F7_ICACHE_ON) || defined(CONFIG_STM32F7_DCACHE_ON)
/*
 * Enable Data and/or Instruction caches
 */
static void stm32f7_enable_cache(void)
{
	u32 ccr;

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

#if defined(CONFIG_STM32F7_DCACHE_ON) && defined(CONFIG_STM32F7_ICACHE_ON)
/*
 * Clean D-cache, invalidate I-cache, and invalidate B-cache within
 * the specified region.
 * - s - start address of region
 * - e - end address of region
 */
void stm32f7_cache_sync_range(u32 s, u32 e)
{
	for ( ; s < e; s += CONFIG_SYS_CACHELINE_SIZE) {
		_DCCMVAU = s;
		_ICIMVAU = s;
	}

	_BPIALL = 0;

	__asm__ volatile("dsb");
	__asm__ volatile("isb");
}
#endif

#if defined(CONFIG_DMAMEM)
/*
 * Configure the specified area as DMA memory
 * - base_adr - start address of the area
 * - size - size of the area
 */
void dmamem_init(unsigned long base_adr, unsigned long size)
{
#if defined(CONFIG_STM32F7_DCACHE_ON)
	/*
	 * Configure DMAMEM as non-cacheable
	 */
	cortex_m3_mpu_set_region(MPU_RGN_SDRAM_NC,
		base_adr | 1 << 4 | MPU_RGN_SDRAM_NC << 0,
		0 << 28 | 3 << 24 |
		1 << 19 | 0 << 18 | 0 << 17 | 0 << 16 |
		0 <<  8 | ((ffs(size) - 2) <<  1) | 1 <<  0);
#endif
}
#endif

/*
 * SoC configuration code that cannot be put into drivers
 */
#ifdef CONFIG_ARMCORTEXM3_SOC_INIT
void cortex_m3_soc_init(void)
{
	stm32f7_mpu_config();

#if defined(CONFIG_STM32F7_ICACHE_ON) || defined(CONFIG_STM32F7_DCACHE_ON)
	stm32f7_enable_cache();
#endif

#if defined(CONFIG_DMAMEM)
	/*
	 * Set the default values for now (so that e.g. LCD FB operates
	 * correctly). The dmamem region may be then reprogrammed after
	 * obtaining the 'dmamem' params from dtb
	 */
	dmamem_init(CONFIG_DMAMEM_BASE, CONFIG_DMAMEM_SZ_ALL);
#endif
}
#endif
