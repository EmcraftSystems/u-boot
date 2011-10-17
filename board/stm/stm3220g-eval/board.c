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

/*
 * Board specific code for the STMicroelectronic STM3220G-EVAL board
 */

#include <common.h>
#include <netdev.h>

#include <asm/arch/stm32.h>
#include <asm/arch/stm32f2_gpio.h>

#if (CONFIG_NR_DRAM_BANKS > 0)
/*
 * Check if RAM configured
 */
# if !defined(CONFIG_SYS_RAM_CS) || !defined(CONFIG_SYS_FSMC_PSRAM_BCR) ||     \
     !defined(CONFIG_SYS_FSMC_PSRAM_BTR)
#  error "Incorrect FSMC configuration."
# endif
#endif /* CONFIG_NR_DRAM_BANKS */

/*
 * STM32 RCC FSMC specific definitions
 */
#define STM32_RCC_ENR_FSMC		(1 << 0)	/* FSMC module clock  */

DECLARE_GLOBAL_DATA_PTR;

#if (CONFIG_NR_DRAM_BANKS > 0) || !defined(CONFIG_SYS_NO_FLASH)
/*
 * External SRAM GPIOs for FSMC:
 *
 * D0..D15, A0..A23, NE2/1, NOE, NWE, NBL1/0, CLK, NL, NWAIT
 */
static struct stm32f2_gpio_dsc fsmc_gpio[] = {
	{STM32F2_GPIO_PORT_B, STM32F2_GPIO_PIN_7},

	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_0},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_1},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_3},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_4},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_5},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_6},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_7},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_8},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_9},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_10},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_11},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_12},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_13},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_14},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_15},

	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_0},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_1},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_2},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_3},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_4},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_5},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_6},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_7},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_8},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_9},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_10},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_11},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_12},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_13},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_14},
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_15},

	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_0},
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_1},
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_2},
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_3},
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_4},
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_5},
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_12},
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_13},
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_14},
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_15},

	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_0},
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_1},
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_2},
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_3},
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_4},
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_5},
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_9}
};
#endif

/*
 * Early hardware init.
 */
int board_init(void)
{
	int	rv = 0;

#if (CONFIG_NR_DRAM_BANKS > 0) || !defined(CONFIG_SYS_NO_FLASH)
	/*
	 * Some external memory is used. Connect GPIOs to FSMC controller
	 */
	int	i;

	for (i = 0; i < sizeof(fsmc_gpio)/sizeof(fsmc_gpio[0]); i++) {
		rv = stm32f2_gpio_config(&fsmc_gpio[i],
					 STM32F2_GPIO_ROLE_FSMC);
		if (rv != 0)
			break;
	}

	/*
	 * Enable FSMC interface clock
	 */
	if (rv == 0)
		STM32_RCC->ahb3enr |= STM32_RCC_ENR_FSMC;
#endif

	return rv;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
	printf("Board: STM3220G-EVAL board %s\n",
		CONFIG_SYS_BOARD_REV_STR);

	return 0;
}

/*
 * Configure board specific parts.
 */
int misc_init_r(void)
{
#if !defined(CONFIG_SYS_NO_FLASH)
	int	i, rv;

	/*
	 * Configure FSMC Flash block
	 */
	i = CONFIG_SYS_FLASH_CS - 1;

	/*
	 * FIXME: not sure if this fake read is necessary here
	 */
	rv = STM32_FSMC->cs[i].bcr;

	STM32_FSMC->cs[i].bcr = CONFIG_SYS_FSMC_FLASH_BCR;
	STM32_FSMC->cs[i].btr = CONFIG_SYS_FSMC_FLASH_BTR;
# if defined(CONFIG_SYS_FSMC_FLASH_BWR)
	STM32_FSMC->wt[i].wtr = CONFIG_SYS_FSMC_FLASH_BWR;
# endif
#endif /* CONFIG_SYS_NO_FLASH */

	return 0;
}

/*
 * Setup external RAM.
 */
int dram_init(void)
{
	int	rv = 0;

#if (CONFIG_NR_DRAM_BANKS > 0)
	static struct stm32f2_gpio_dsc	ctrl_gpio = {STM32F2_GPIO_PORT_I,
						     STM32F2_GPIO_PIN_9};
	int				i;

	/*
	 * Configure FSMC PSRAM block
	 */
	i = CONFIG_SYS_RAM_CS - 1;

	/*
	 * Fake BCR read; if don't do this, then BCR remains configured
	 * with defaults.
	 */
	rv = STM32_FSMC->cs[i].bcr;

	/* Step.1 */
	STM32_FSMC->cs[i].bcr = CONFIG_SYS_FSMC_PSRAM_BCR;
	STM32_FSMC->cs[i].btr = CONFIG_SYS_FSMC_PSRAM_BTR;
# if defined(CONFIG_SYS_FSMC_PSRAM_BWR)
	STM32_FSMC->wt[i].wtr = CONFIG_SYS_FSMC_PSRAM_BWR;
# endif

	rv = stm32f2_gpio_config(&ctrl_gpio, STM32F2_GPIO_ROLE_GPOUT);
	if (rv != 0)
		goto out;

# if defined(CONFIG_SYS_RAM_BURST)
	/*
	 * FIXME: all this hardcoded stuff, and wiki 'Step.X' remarks...
	 */

	/* Step.2 */
	stm32f2_gpout_set(&ctrl_gpio, 1);

	/* Step.3 */
	*(volatile u8 *)(CONFIG_SYS_RAM_BASE + 0x4101F) = 0;

	/* Step.4-5 */
	stm32f2_gpout_set(&ctrl_gpio, 0);

	/* Step.6 */
	STM32_FSMC->cs[i].bcr = 0x00087959;
	STM32_FSMC->cs[i].btr = 0x0010FFFF;

	/* Step.7 */
	rv = *(volatile u8 *)(CONFIG_SYS_RAM_BASE + 0x000000);

	/* Step.8 */
	STM32_FSMC->cs[i].bcr = 0x00005059;
	STM32_FSMC->cs[i].btr = 0x100106F2;
	STM32_FSMC->wt[i].wtr = 0x100107F2;

	/* Step.9 */
	stm32f2_gpout_set(&ctrl_gpio, 1);

	/* Step.10 */
	*(volatile u8 *)(CONFIG_SYS_RAM_BASE + 0x4101F) = 0;

	/* Step.11 */
	stm32f2_gpout_set(&ctrl_gpio, 0);

	/* Step.12 */
	STM32_FSMC->cs[i].bcr = 0x00087959;
	STM32_FSMC->cs[i].btr = 0x0010FFFF;
# else
	/*
	 * Switch PSRAM in the Asyncronous Read/Write Mode
	 */
	stm32f2_gpout_set(&ctrl_gpio, 0);
# endif /* CONFIG_SYS_RAM_BURST */

	/*
	 * Fill in global info with description of SRAM configuration
	 */
	gd->bd->bi_dram[0].start = CONFIG_SYS_RAM_BASE;
	gd->bd->bi_dram[0].size  = CONFIG_SYS_RAM_SIZE;

	rv = 0;
out:
#endif /* CONFIG_NR_DRAM_BANKS */

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

