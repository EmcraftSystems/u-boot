/*
 * (C) Copyright 2011, 2012, 2013
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 * Pavel Boldin, Emcraft Systems, paboldin@emcraft.com
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

/*
 * Board specific code for the STmicro STM32F429 Discovery board
 */

#include <common.h>
#include <netdev.h>
#include <ili932x.h>

#include <asm/arch/stm32.h>
#include <asm/arch/stm32f2_gpio.h>

#include <asm/arch/fmc.h>
#include <flash.h>
#include <asm/io.h>
#include <asm/system.h>

#include <asm/arch/fsmc.h>

DECLARE_GLOBAL_DATA_PTR;

static const struct stm32f2_gpio_dsc ext_ram_fsmc_fmc_gpio[] = {
	/* Chip is LQFP144, see DM00077036.pdf for details */
	/* 79, FMC_D15 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_10},
	/* 78, FMC_D14 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_9},
	/* 77, FMC_D13 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_8},
	/* 68, FMC_D12 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_15},
	/* 67, FMC_D11 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_14},
	/* 66, FMC_D10 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_13},
	/* 65, FMC_D9 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_12},
	/* 64, FMC_D8 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_11},
	/* 63, FMC_D7 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_10},
	/* 60, FMC_D6 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_9},
	/* 59, FMC_D5 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_8},
	/* 58, FMC_D4 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_7},
	/* 115, FMC_D3 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_1},
	/* 114, FMC_D2 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_0},
	/* 86, FMC_D1 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_15},
	/* 85, FMC_D0 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_14},
	/* 142, FMC_NBL1 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_1},
	/* 141, FMC_NBL0 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_0},
	/* 90, FMC_A15, BA1 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_5},
	/* 89, FMC_A14, BA0 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_4},
	/* K15, FMC_A13 */
	/* 57, FMC_A11 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_1},
	/* 56, FMC_A10 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_0},
	/* 55, FMC_A9 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_15},
	/* 54, FMC_A8 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_14},
	/* 53, FMC_A7 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_13},
	/* 50, FMC_A6 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_12},
	/* 15, FMC_A5 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_5},
	/* 14, FMC_A4 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_4},
	/* 13, FMC_A3 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_3},
	/* 12, FMC_A2 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_2},
	/* 11, FMC_A1 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_1},
	/* 10, FMC_A0 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_0},
	/* 136, SDRAM_NE */
	{STM32F2_GPIO_PORT_B, STM32F2_GPIO_PIN_6},
	/* 49, SDRAM_NRAS */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_11},
	/* 132, SDRAM_NCAS */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_15},
	/* 26, SDRAM_NWE */
	{STM32F2_GPIO_PORT_C, STM32F2_GPIO_PIN_0},
	/* 135, SDRAM_CKE */
	{STM32F2_GPIO_PORT_B, STM32F2_GPIO_PIN_5},
	/* 93, SDRAM_CLK */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_8},
};

/*
 * Init FMC/FSMC GPIOs based
 */
static int fmc_fsmc_setup_gpio(void)
{
	int rv = 0;
	int i;

	/*
	 * Connect GPIOs to FMC controller
	 */
	for (i = 0; i < ARRAY_SIZE(ext_ram_fsmc_fmc_gpio); i++) {
		rv = stm32f2_gpio_config(&ext_ram_fsmc_fmc_gpio[i],
				STM32F2_GPIO_ROLE_FMC);
		if (rv)
			goto out;
	}

	fsmc_gpio_init_done = 1;
out:
	return rv;
}

/*
 * Early hardware init.
 */
int board_init(void)
{
	int rv;

	rv = fmc_fsmc_setup_gpio();
	if (rv)
		return rv;

	return 0;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
	printf("Board: STM32F429-DISCOVERY Rev %s\n", CONFIG_SYS_BOARD_REV_STR);

	return 0;
}

/*
 * STM32 RCC FMC specific definitions
 */
#define STM32_RCC_ENR_FMC		(1 << 0)	/* FMC module clock  */

static int dram_initialized = 0;

static inline u32 _ns2clk(u32 ns, u32 freq)
{
	uint32_t tmp = freq/1000000;
	return (tmp * ns) / 1000;
}

#define NS2CLK(ns) (_ns2clk(ns, freq))

/*
 * Following are timings for IS42S16400J, from corresponding datasheet
 */
#define SDRAM_CAS	3
#define SDRAM_NB	1	/* Number of banks */
#define SDRAM_MWID	1	/* 16 bit memory */

#define SDRAM_NR	0x1	/* 12-bit row */
#define SDRAM_NC	0x0	/* 8-bit col */

#define SDRAM_TRRD	(NS2CLK(14) - 1)
#define SDRAM_TRCD	(NS2CLK(15) - 1)
#define SDRAM_TRP	(NS2CLK(15) - 1)
#define SDRAM_TRAS	(NS2CLK(42) - 1)
#define SDRAM_TRC	(NS2CLK(63) - 1)
#define SDRAM_TRFC	(NS2CLK(63) - 1)
#define SDRAM_TCDL	(1 - 1)
#define SDRAM_TRDL	(2 - 1)
#define SDRAM_TBDL	(1 - 1)
#define SDRAM_TREF	1386
#define SDRAM_TCCD	(1 - 1)

#define SDRAM_TXSR	(NS2CLK(70)-1) 	/* Row cycle time after precharge */
#define SDRAM_TMRD	(3 - 1)		/* Page 10, Mode Register Set */

/* Last data in to row precharge, need also comply ineq on page 1648 */
#define SDRAM_TWR	max(\
	(int)max((int)SDRAM_TRDL, (int)(SDRAM_TRAS - SDRAM_TRCD - 1)), \
	(int)(SDRAM_TRC - SDRAM_TRCD - SDRAM_TRP - 2)\
)

int dram_init(void)
{
	u32 freq;
	int rv;

	/*
	 * Enable FMC interface clock
	 */
	STM32_RCC->ahb3enr |= STM32_RCC_ENR_FMC;

	/*
	 * Get frequency for NS2CLK calculation.
	 */
	freq = clock_get(CLOCK_HCLK) / CONFIG_SYS_RAM_FREQ_DIV;

	STM32_SDRAM_FMC->sdcr1 = (
		CONFIG_SYS_RAM_FREQ_DIV << FMC_SDCR_SDCLK_SHIFT |
		0 << FMC_SDCR_RPIPE_SHIFT |
		1 << FMC_SDCR_RBURST_SHIFT
	);
	STM32_SDRAM_FMC->sdcr2 = (
		CONFIG_SYS_RAM_FREQ_DIV << FMC_SDCR_SDCLK_SHIFT |
		SDRAM_CAS << FMC_SDCR_CAS_SHIFT |
		SDRAM_NB << FMC_SDCR_NB_SHIFT |
		SDRAM_MWID << FMC_SDCR_MWID_SHIFT |
		SDRAM_NR << FMC_SDCR_NR_SHIFT |
		SDRAM_NC << FMC_SDCR_NC_SHIFT |
		0 << FMC_SDCR_RPIPE_SHIFT |
		1 << FMC_SDCR_RBURST_SHIFT
	);

	STM32_SDRAM_FMC->sdtr2 = (
		SDRAM_TRP << FMC_SDTR_TRP_SHIFT |
		SDRAM_TRC << FMC_SDTR_TRC_SHIFT
	);
	STM32_SDRAM_FMC->sdtr2 = (
		SDRAM_TRCD << FMC_SDTR_TRCD_SHIFT |
		SDRAM_TRP << FMC_SDTR_TRP_SHIFT |
		SDRAM_TWR << FMC_SDTR_TWR_SHIFT |
		SDRAM_TRC << FMC_SDTR_TRC_SHIFT |
		SDRAM_TRAS << FMC_SDTR_TRAS_SHIFT |
		SDRAM_TXSR << FMC_SDTR_TXSR_SHIFT |
		SDRAM_TMRD << FMC_SDTR_TMRD_SHIFT
	);

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_2 | FMC_SDCMR_MODE_START_CLOCK;

	udelay(200);	/* 200 us delay, page 10, "Power-Up" */
	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_2 | FMC_SDCMR_MODE_PRECHARGE;

	udelay(100);
	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = (
		FMC_SDCMR_BANK_2 | FMC_SDCMR_MODE_AUTOREFRESH |
		7 << FMC_SDCMR_NRFS_SHIFT
	);

	udelay(100);
	FMC_BUSY_WAIT();

#define SDRAM_MODE_BL_SHIFT		0
#define SDRAM_MODE_CAS_SHIFT		4

#define SDRAM_MODE_BL			0
#define SDRAM_MODE_CAS			SDRAM_CAS

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_2 |
	(
		SDRAM_MODE_BL << SDRAM_MODE_BL_SHIFT |
		SDRAM_MODE_CAS << SDRAM_MODE_CAS_SHIFT
	) << FMC_SDCMR_MODE_REGISTER_SHIFT | FMC_SDCMR_MODE_WRITE_MODE;

	udelay(100);

	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_2 | FMC_SDCMR_MODE_NORMAL;

	FMC_BUSY_WAIT();

	/* Refresh timer */
	STM32_SDRAM_FMC->sdrtr = SDRAM_TREF;

	/*
	 * Fill in global info with description of SRAM configuration
	 */
	gd->bd->bi_dram[0].start = CONFIG_SYS_RAM_BASE;
	gd->bd->bi_dram[0].size  = CONFIG_SYS_RAM_SIZE;

	rv = 0;

	cortex_m3_mpu_full_access();

	dram_initialized = 1;

	return rv;
}

#ifdef CONFIG_STM32_ETH
/*
 * Register ethernet driver
 */
int board_eth_init(bd_t *bis)
{
	return stm32_eth_init(bis);
}
#endif

