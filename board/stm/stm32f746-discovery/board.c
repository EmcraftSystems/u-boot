/*
 * (C) Copyright 2011-2015
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 * Pavel Boldin, Emcraft Systems, paboldin@emcraft.com
 * Vladimir Skvortsov, Emcraft Systems, vskvortsov@emcraft.com
 * Anton Protopopov, Emcraft Systems, antonp@emcraft.com
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
 * Board specific code for the STmicro STM32F746 Discovery board
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
#include <linux/mtd/stm32_qspi.h>

DECLARE_GLOBAL_DATA_PTR;

static const struct stm32f2_gpio_dsc ext_ram_fsmc_fmc_gpio[] = {
	/* K15, FMC_D15 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_10},
	/* L14, FMC_D14 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_9},
	/* L15, FMC_D13 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_8},
	/* R11, FMC_D12 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_15},
	/* P11, FMC_D11 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_14},
	/* R12, FMC_D10 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_13},
	/* R10, FMC_D9 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_12},
	/* P10, FMC_D8 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_11},
	/* R9, FMC_D7 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_10},
	/* P9, FMC_D6 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_9},
	/* N9, FMC_D5 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_8},
	/* R8, FMC_D4 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_7},
	/* C12, FMC_D3 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_1},
	/* B12, FMC_D2 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_0},
	/* K13, FMC_D1 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_15},
	/* L12, FMC_D0 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_14},

	/* A5, FMC_NBL1 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_1},
	/* A6, FMC_NBL0 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_0},

	/* FMC_BA1 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_5},
	/* FMC_BA0 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_4},

	/* M7, FMC_A11 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_1},
	/* N7, FMC_A10 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_0},
	/* M8, FMC_A9 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_15},
	/* P6, FMC_A8 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_14},
	/* N6, FMC_A7 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_13},
	/* M6, FMC_A6 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_12},
	/* K3, FMC_A5 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_5},
	/* J2, FMC_A4 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_4},
	/* H2, FMC_A3 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_3},
	/* G2, FMC_A2 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_2},
	/* E2, FMC_A1 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_1},
	/* D2, FMC_A0 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_0},

	/* SDRAM */
	/* M4, SDRAM_NE */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_3},
	/* P8, SDRAM_NRAS */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_11},
	/* B7, SDRAM_NCAS */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_15},
	/* J3, SDRAM_NWE */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_5},
	/* L4, SDRAM_CKE */
	{STM32F2_GPIO_PORT_C, STM32F2_GPIO_PIN_3},

	/* H14, SDRAM_CLK */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_8},

#ifdef CONFIG_FSMC_NOR_PSRAM_CS1_ENABLE
	/* A11, FMC_NE1 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_7},
#endif
#ifdef CONFIG_FSMC_NOR_PSRAM_CS2_ENABLE
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_9},
#endif
#ifdef CONFIG_FSMC_NOR_PSRAM_CS3_ENABLE
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_10},
#endif
#ifdef CONFIG_FSMC_NOR_PSRAM_CS4_ENABLE
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_12},
#endif
};

#ifdef CONFIG_VIDEO_STM32F4_LTDC
static const struct stm32f2_gpio_dsc ltdc_iomux[] = {
	/* PI14 = LCD_CLK */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_14},
	/* PK7  = LCD_DE */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_7},
	/* PI12 = LCD_HSYNC */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_12},
	/* PI13 = LCD_VSYNC */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_13},
	/* PJ12 = LCD_B0 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_12},
	/* PJ13 = LCD_B1 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_13},
	/* PJ14 = LCD_B2 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_14},
	/* PJ15 = LCD_B3 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_15},
	/* PK3  = LCD_B4 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_3},
	/* PK4  = LCD_B5 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_4},
	/* PK5  = LCD_B6 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_5},
	/* PK6  = LCD_B7 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_6},
	/* PJ7  = LCD_G0 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_7},
	/* PJ8  = LCD_G1 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_8},
	/* PJ9  = LCD_G2 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_9},
	/* PJ10 = LCD_G3 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_10},
	/* PJ11 = LCD_G4 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_11},
	/* PK0  = LCD_G5 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_0},
	/* PK1  = LCD_G6 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_1},
	/* PK2  = LCD_G7 */
	{STM32F2_GPIO_PORT_K, STM32F2_GPIO_PIN_2},
	/* PI15 = LCD_R0 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_15},
	/* PJ0  = CD_R1 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_0},
	/* PJ1  = LCD_R2 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_1},
	/* PJ2  = LCD_R3 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_2},
	/* PJ3  = LCD_R4 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_3},
	/* PJ4  = LCD_R5 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_4},
	/* PJ5  = LCD_R6 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_5},
	/* PJ6  = LCD_R7 */
	{STM32F2_GPIO_PORT_J, STM32F2_GPIO_PIN_6},
};
#endif /* CONFIG_VIDEO_STM32F4_LTDC */

#ifdef CONFIG_STM32_QSPI
static const struct stm32f2_gpio_dsc qspi_af9_iomux[] = {
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_11},	/* D0 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_12},	/* D1 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_2},	/* D2 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_13},	/* D3 */
	{STM32F2_GPIO_PORT_B, STM32F2_GPIO_PIN_2},	/* CLK */
};
static const struct stm32f2_gpio_dsc qspi_af10_iomux[] = {
	{STM32F2_GPIO_PORT_B, STM32F2_GPIO_PIN_6},	/* NCS */
};
#endif /* CONFIG_STM32_QSPI */

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

#ifdef CONFIG_VIDEO_STM32F4_LTDC
/*
 * Initialize LCD pins
 */
static int ltdc_setup_iomux(void)
{
	int rv = 0;
	int i;

	/*
	 * Connect GPIOs to FMC controller
	 */
	for (i = 0; i < ARRAY_SIZE(ltdc_iomux); i++) {
		rv = stm32f2_gpio_config(&ltdc_iomux[i],
				STM32F2_GPIO_ROLE_LTDC);
		if (rv)
			break;
	}

	return rv;
}
#endif /* CONFIG_VIDEO_STM32F4_LTDC */

#ifdef CONFIG_STM32_QSPI
static int qspi_setup_iomux(void)
{
	int i, rv = 0;

	for (i = 0; i < ARRAY_SIZE(qspi_af9_iomux); i++) {
		rv = stm32f2_gpio_config(&qspi_af9_iomux[i],
					 STM32F2_GPIO_ROLE_QSPI_AF9);
		if (rv)
			break;
	}

	for (i = 0; i < ARRAY_SIZE(qspi_af10_iomux); i++) {
		rv = stm32f2_gpio_config(&qspi_af10_iomux[i],
					 STM32F2_GPIO_ROLE_QSPI_AF10);
		if (rv)
			break;
	}

	return rv;
}
#endif /* CONFIG_STM32_QSPI */

/*
 * Early hardware init.
 */
int board_init(void)
{
	int rv;

	rv = fmc_fsmc_setup_gpio();
	if (rv)
		return rv;

#if !defined(CONFIG_SYS_NO_FLASH)

	if ((rv = fsmc_nor_psram_init(CONFIG_SYS_FLASH_CS,
			CONFIG_SYS_FSMC_FLASH_BCR,
			CONFIG_SYS_FSMC_FLASH_BTR,
			CONFIG_SYS_FSMC_FLASH_BWTR))) {
		return rv;
	}

#endif

#ifdef CONFIG_VIDEO_STM32F4_LTDC
	rv = ltdc_setup_iomux();
	if (rv)
		return rv;
#endif /* CONFIG_VIDEO_STM32F4_LTDC */

#ifdef CONFIG_STM32_QSPI
	rv = qspi_setup_iomux();
	if (rv)
		return rv;
#endif

	return 0;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
	printf("Board: STM32F746 Discovery Rev %s, www.emcraft.com\n",
		CONFIG_SYS_BOARD_REV_STR);

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
 * Following are timings for MT48LC4M32B2B5-6A, from corresponding datasheet
 */
#define SDRAM_CAS	3
#define SDRAM_NB	1	/* Number of banks */
#define SDRAM_MWID	1	/* 16 bit memory */

#define SDRAM_NR	1	/* 12-bit row */
#define SDRAM_NC	0	/* 8-bit col */

#define SDRAM_TRRD	NS2CLK(12)
#define SDRAM_TRCD	NS2CLK(18)
#define SDRAM_TRP	NS2CLK(18)
#define SDRAM_TRAS	NS2CLK(42)
#define SDRAM_TRC	NS2CLK(60)
#define SDRAM_TRFC	NS2CLK(60)
#define SDRAM_TCDL	(1 - 1)
#define SDRAM_TRDL	NS2CLK(12)
#define SDRAM_TBDL	(1 - 1)
#define SDRAM_TREF	(NS2CLK(64000000 / 8192) - 20)
#define SDRAM_TCCD	(1 - 1)

#define SDRAM_TXSR	SDRAM_TRFC	/* Row cycle time after precharge */
#define SDRAM_TMRD	1		/* Page 10, Mode Register Set */

/* Last data in to row precharge, need also comply ineq on page 1648 */
#define SDRAM_TWR	max(\
	(int)max((int)SDRAM_TRDL, (int)(SDRAM_TRAS - SDRAM_TRCD)), \
	(int)(SDRAM_TRC - SDRAM_TRCD - SDRAM_TRP)\
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
		SDRAM_CAS << FMC_SDCR_CAS_SHIFT |
		SDRAM_NB << FMC_SDCR_NB_SHIFT |
		SDRAM_MWID << FMC_SDCR_MWID_SHIFT |
		SDRAM_NR << FMC_SDCR_NR_SHIFT |
		SDRAM_NC << FMC_SDCR_NC_SHIFT |
		0 << FMC_SDCR_RPIPE_SHIFT |
		1 << FMC_SDCR_RBURST_SHIFT
	);

	STM32_SDRAM_FMC->sdtr1 = (
		SDRAM_TRCD << FMC_SDTR_TRCD_SHIFT |
		SDRAM_TRP << FMC_SDTR_TRP_SHIFT |
		SDRAM_TWR << FMC_SDTR_TWR_SHIFT |
		SDRAM_TRC << FMC_SDTR_TRC_SHIFT |
		SDRAM_TRAS << FMC_SDTR_TRAS_SHIFT |
		SDRAM_TXSR << FMC_SDTR_TXSR_SHIFT |
		SDRAM_TMRD << FMC_SDTR_TMRD_SHIFT
	);

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_START_CLOCK;

	udelay(200);	/* 200 us delay, page 10, "Power-Up" */
	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_PRECHARGE;

	udelay(100);
	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = (
		FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_AUTOREFRESH |
		7 << FMC_SDCMR_NRFS_SHIFT
	);

	udelay(100);
	FMC_BUSY_WAIT();


#define SDRAM_MODE_BL_SHIFT		0
#define SDRAM_MODE_CAS_SHIFT		4

#define SDRAM_MODE_BL			0
#define SDRAM_MODE_CAS			SDRAM_CAS

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 |
	(
		SDRAM_MODE_BL << SDRAM_MODE_BL_SHIFT |
		SDRAM_MODE_CAS << SDRAM_MODE_CAS_SHIFT
	) << FMC_SDCMR_MODE_REGISTER_SHIFT | FMC_SDCMR_MODE_WRITE_MODE;

	udelay(100);

	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_NORMAL;

	FMC_BUSY_WAIT();

	/* Refresh timer */
	STM32_SDRAM_FMC->sdrtr = SDRAM_TREF;

	/*
	 * Fill in global info with description of SRAM configuration
	 */
	gd->bd->bi_dram[0].start = CONFIG_SYS_RAM_BASE;
	gd->bd->bi_dram[0].size  = CONFIG_SYS_RAM_SIZE;

	rv = 0;

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

#ifdef BOARD_LATE_INIT
int board_late_init(void)
{
	int	rv = 0;

#ifdef CONFIG_STM32_QSPI
	STM32_RCC->ahb3enr |= RCC_AHB3ENR_QSPIEN;
	STM32_RCC->ahb1enr |= RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMA2EN;

	rv = stm32_qspi_init();
#endif

	return rv;
}
#endif /* BOARD_LATE_INIT */
