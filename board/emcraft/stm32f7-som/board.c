/*
 * (C) Copyright 2011-2015
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 * Pavel Boldin, Emcraft Systems, paboldin@emcraft.com
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

/*
 * Board specific code for the Emcraft STM32F7 SOM
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
#if defined(CONFIG_SYS_STM32F769I_DISCO)
	/* +L12, FMC_D0 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_14},
	/* +K13, FMC_D1 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_15},
	/* +B12, FMC_D2 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_0},
	/* +C12, FMC_D3 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_1},
	/* +R8, FMC_D4 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_7},
	/* +N9, FMC_D5 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_8},
	/* +P9, FMC_D6 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_9},
	/* +R9, FMC_D7 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_10},
	/* +P10, FMC_D8 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_11},
	/* +R10, FMC_D9 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_12},
	/* +R12, FMC_D10 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_13},
	/* +P11, FMC_D11 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_14},
	/* +R11, FMC_D12 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_15},
	/* +L15, FMC_D13 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_8},
	/* +L14, FMC_D14 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_9},
	/* +K15, FMC_D15 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_10},
	/* +P14, FMC_D16 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_8},
	/* +N14, FMC_D17 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_9},
	/* +P15, FMC_D18 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_10},
	/* +N15, FMC_D19 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_11},
	/* +M15, FMC_D20 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_12},
	/* +E12, FMC_D21 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_13},
	/* +E13, FMC_D22 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_14},
	/* +D13, FMC_D23 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_15},
	/* +E14, FMC_D24 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_0},
	/* +D14, FMC_D25 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_1},
	/* +C14, FMC_D26 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_2},
	/* +C13, FMC_D27 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_3},
	/* +D6, FMC_D28 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_6},
	/* +D4, FMC_D29 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_7},
	/* +E4, FMC_D30 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_9},
	/* +D5, FMC_D31 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_10},

	/* +D2, FMC_A0 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_0},
	/* +E2, FMC_A1 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_1},
	/* +G2, FMC_A2 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_2},
	/* +H2, FMC_A3 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_3},
	/* +J2, FMC_A4 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_4},
	/* +K3, FMC_A5 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_5},
	/* +M6, FMC_A6 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_12},
	/* +N6, FMC_A7 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_13},
	/* +P6, FMC_A8 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_14},
	/* +M8, FMC_A9 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_15},
	/* +N7, FMC_A10 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_0},
	/* +M7, FMC_A11 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_1},
	/* +M13, FMC_A12 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_2},

	/* +A6, FMC_NBL0 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_0},
	/* +A5, FMC_NBL1 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_1},
	/* +C3, FMC_NBL2 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_4},
	/* +D3, FMC_NBL3 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_5},

	/* +N12, FMC_BA0 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_4},
	/* +N11, FMC_BA1 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_5},

	/* SDRAM */
	/* +J4, FMC_SDNE0 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_3},
	/* +K4, FMC_SDCKE0 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_2},
	/* +P8, FMC_SDNRAS */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_11},
	/* +B7, FMC_SDNCAS */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_15},
	/* +J3, FMC_SDNWE */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_5},
	/* +H14, FMC_SDCLK */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_8},

#else /* CONFIG_SYS_STM32F769I_DISCO */

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

	/* D11, FMC_NOE */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_4},
	/* C10, FMC_NWE */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_5},

	/* B2, FMC_A22 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_6},
	/* B1, FMC_A21 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_5},
	/* A1, FMC_A20 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_4},
	/* A2, FMC_A19 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_3},
	/* M11, FMC_A18 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_13},
	/* M10, FMC_A17 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_12},
	/* N10, FMC_A16 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_11},

	/* N11, FMC_A15, BA1 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_5},
	/* N12, FMC_A14, BA0 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_4},
	/* M12, FMC_A13 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_3},
	/* M13, FMC_A12 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_2},
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
	{STM32F2_GPIO_PORT_C, STM32F2_GPIO_PIN_2},
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
#endif /* CONFIG_SYS_STM32F769I_DISCO */
};

#if defined(CONFIG_VIDEO_STM32F4_LTDC) && !defined(CONFIG_SYS_STM32F769I_DISCO)
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
# ifdef CONFIG_SYS_STM32F769I_DISCO
static const struct stm32f2_gpio_dsc qspi_af9_iomux[] = {
	{STM32F2_GPIO_PORT_C, STM32F2_GPIO_PIN_9},	/* D0 */
	{STM32F2_GPIO_PORT_C, STM32F2_GPIO_PIN_10},	/* D1 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_2},	/* D2 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_13},	/* D3 */
	{STM32F2_GPIO_PORT_B, STM32F2_GPIO_PIN_2},	/* CLK */
};
static const struct stm32f2_gpio_dsc qspi_af10_iomux[] = {
	{STM32F2_GPIO_PORT_B, STM32F2_GPIO_PIN_6},	/* NCS */
};
# else
#  error "QSPI is not defined for this platform"
# endif
#endif /* CONFIG_STM32_QSPI */

#ifdef CONFIG_SYS_BOARD_UCL_BSB
/*
 * Configure GPIOs
 */
static int pwr_setup_gpio(void)
{
	struct stm32f2_gpio_dsc pwr_en_gpio[] = {
		{ STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_10 },	/* PWR_EN  */
		{ STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_0  },	/* LCD_DIM */
	};
	int i, rv;

	for (i = 0; i < ARRAY_SIZE(pwr_en_gpio); i++) {
		rv = stm32f2_gpio_config(&pwr_en_gpio[i],
					 STM32F2_GPIO_ROLE_GPOUT);
		if (rv)
			goto out;

		rv = stm32f2_gpout_set(&pwr_en_gpio[i], 1);
		if (rv)
			goto out;
	}
out:
	return rv;
}
#endif /* CONFIG_SYS_BOARD_UCL_BSB */

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

#if defined(CONFIG_VIDEO_STM32F4_LTDC) && !defined(CONFIG_SYS_STM32F769I_DISCO)
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

static int qspi_setup_iomux(void)
{
	int rv = 0;
#ifdef CONFIG_STM32_QSPI
	int i;

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
#endif /* CONFIG_STM32_QSPI */
	return rv;
}

/*
 * Early hardware init.
 */
int board_init(void)
{
	int rv;

#if defined(CONFIG_SYS_BOARD_UCL_BSB)
	rv = pwr_setup_gpio();
	if (rv)
		printf("WARN: pwr_setup_gpio() error %d\n", rv);
#endif

	rv = fmc_fsmc_setup_gpio();
	if (rv)
		return rv;

#if !defined(CONFIG_SYS_NO_FLASH)
# if defined(CONFIG_ENV_IS_IN_FLASH)
	/*
	 * We may sometimes got fixed garbage from NOR flash if access it after
	 * a sw reset and before external SDRAM initializion complete.
	 * Erratas say nothing about this, so we just initialize SDRAM earlier
	 * if want to access NOR to get environment.
	 */
	dram_init();
# endif /* CONFIG_ENV_IS_IN_FLASH */

	if ((rv = fsmc_nor_psram_init(CONFIG_SYS_FLASH_CS,
			CONFIG_SYS_FSMC_FLASH_BCR,
			CONFIG_SYS_FSMC_FLASH_BTR,
			CONFIG_SYS_FSMC_FLASH_BWTR))) {
		goto Done;
	}
#endif

#if defined(CONFIG_VIDEO_STM32F4_LTDC) && !defined(CONFIG_SYS_STM32F769I_DISCO)
	rv = ltdc_setup_iomux();
	if (rv)
		return rv;
#endif /* CONFIG_VIDEO_STM32F4_LTDC */

	rv = qspi_setup_iomux();
	if (rv)
		return rv;

Done:
	return 0;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
#if defined(CONFIG_SYS_STM32F769I_DISCO)
	printf("Board: STM32F769I-DISCO %s, www.emcraft.com\n",
	       CONFIG_SYS_BOARD_REV_STR);
#else
	printf("Board: STM32F7 SOM Rev %s, www.emcraft.com\n",
		CONFIG_SYS_BOARD_REV_STR);
#endif /* CONFIG_SYS_STM32F769I_DISCO */

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

#if defined(CONFIG_SYS_STM32F769I_DISCO)

/*
 * Following are timings for MT48LC4M32B2, from corresponding datasheet
 */
#define SDRAM_CAS	3
#define SDRAM_NB	1	/* Number of banks: 0 -> two, 1 -> four */
#define SDRAM_MWID	2	/* 32 bit memory */

#define SDRAM_NR	0x1	/* 12-bit row */
#define SDRAM_NC	0x0	/* 8-bit col */

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

#define SDRAM_TXSR	NS2CLK(70)
#define SDRAM_TMRD	(3 - 1)

#else /* CONFIG_SYS_STM32F769I_DISCO */

/*
 * Following are timings for M12L2561616A-6BI, from corresponding datasheet
 */
#define SDRAM_CAS	3
#define SDRAM_NB	1	/* Number of banks */
#define SDRAM_MWID	1	/* 16 bit memory */

#define SDRAM_NR	0x2	/* 13-bit row */
#if (CONFIG_SYS_RAM_SIZE == (32 * 1024 * 1024))
#define SDRAM_NC	0x1	/* 9-bit col */
#endif
#if (CONFIG_SYS_RAM_SIZE == (64 * 1024 * 1024))
#define SDRAM_NC	0x2	/* 10-bit col */
#endif

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
#define SDRAM_TMRD	(3 - 1)		/* Page 10, Mode Register Set */

#endif /* CONFIG_SYS_STM32F769I_DISCO */

/* Last data in to row precharge, need also comply ineq on page 1648 */
#define SDRAM_TWR	max(\
	(int)max((int)SDRAM_TRDL, (int)(SDRAM_TRAS - SDRAM_TRCD)), \
	(int)(SDRAM_TRC - SDRAM_TRCD - SDRAM_TRP)\
)

int dram_init(void)
{
	u32 freq;
	int rv;

	if (dram_initialized)
		return 0;

	STM32_RCC->ahb3enr |= RCC_AHB3ENR_QSPIEN;
	STM32_RCC->ahb1enr |= RCC_AHB1ENR_DMA1EN | RCC_AHB1ENR_DMA2EN;

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

/*
 * STM32 Flash bug workaround.
 */
extern char	_mem_ram_buf_base, _mem_ram_buf_size;

#define SOC_RAM_BUFFER_BASE	(ulong)(&_mem_ram_buf_base)
#define SOC_RAM_BUFFER_SIZE	(ulong)((&_mem_ram_buf_size) - 0x100)

void stop_ram(void)
{
	if (!dram_initialized)
		return;

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_SELFREFRESH;

	FMC_BUSY_WAIT();
}

void start_ram(void)
{
	if (!dram_initialized)
		return;

	/*
	 * Precharge according to chip requirement, page 12.
	 */

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_PRECHARGE;
	FMC_BUSY_WAIT();


	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_NORMAL;
	FMC_BUSY_WAIT();

	udelay(60);
}

#if !defined(CONFIG_SYS_NO_FLASH)
#define NOP10()		do {	nop(); nop(); nop(); nop(); nop(); \
				nop(); nop(); nop(); nop(); nop(); \
			} while(0);

u16 flash_read16(void *addr)
{
	u16 value;
	stop_ram();
	value = __raw_readw(addr);
	NOP10();
	start_ram();
	return value;
}

void flash_write16(u16 value, void *addr)
{
	stop_ram();
	__raw_writew(value, addr);
	NOP10();
	NOP10();
	start_ram();
}

__attribute__((noinline)) void copy_one(volatile u16* src, volatile u16* dst)
{
	*dst = *src;
}

u32 flash_write_buffer(void *src, void *dst, int cnt, int portwidth)
{
	u32 retval = 0;

	if (portwidth != FLASH_CFI_16BIT) {
		retval = ERR_INVAL;
		goto out;
	}

	memcpy((void*)SOC_RAM_BUFFER_BASE, (void*)src, cnt * portwidth);

	stop_ram();
	__asm__ __volatile__("": : :"memory");

	src = (void*) SOC_RAM_BUFFER_BASE;

	while(cnt-- > 0) {
		copy_one(src, dst);
		src += 2, dst += 2;
		NOP10();
		NOP10();
	}

	__asm__ __volatile__("": : :"memory");
	start_ram();
out:
	return retval;
}

u32 flash_check_flag(void *src, void *dst, int cnt, int portwidth)
{
	u32 flag = 1;

	if (portwidth != FLASH_CFI_16BIT) {
		flag = 0;
		goto out;
	}

	stop_ram();

	while((cnt-- > 0) && (flag == 1)) {
		flag = *(u16*)dst == 0xFFFF;
		dst += 2;
	}

	start_ram();

out:
	return flag;
}
#endif /* !CONFIG_SYS_NO_FLASH */

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
	int rv = 0;

#ifdef CONFIG_STM32_QSPI
	rv = stm32_qspi_init();
#endif

	return rv;
}
#endif /* BOARD_LATE_INIT */
