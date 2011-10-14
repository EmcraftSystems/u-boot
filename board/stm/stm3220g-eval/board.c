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
 * Warn if DRAM configuration isn't set, and use the following defaults:
 *
 * IS61WV102416BLL at Bank1 SRAM 2:
 * - 16 bit data bus, SRAM, Mode 1, Enabled.
 * - CLK period is HCLK (8.4s);
 * - ADDSET  = 0 x CLK;
 * - DATASET = 2 x CLK (16.8ns);
 * - BUSTURN = 1 x CLK (8.4ns).
 * - these timings used both for read & write accesses (not-extended
 *   mode - WTR register isn't used).
 */
# if !defined(CONFIG_SYS_RAM_CS) || !defined(CONFIG_SYS_FSMC_BCR) ||	       \
     !defined(CONFIG_SYS_FSMC_BTR)
#  warning "Incorrect FSMC configuration. Using defaults."
#  undef CONFIG_SYS_RAM_CS
#  undef CONFIG_SYS_FSMC_BCR
#  undef CONFIG_SYS_FSMC_BTR
#  undef CONFIG_SYS_FSMC_BWR
#  define CONFIG_SYS_RAM_CS	2
#  define CONFIG_SYS_FSMC_BCR	(STM32_FSMC_BCR_WREN |			       \
				 (STM32_FSMC_BCR_MWID_16 <<		       \
				  STM32_FSMC_BCR_MWID_BIT) |		       \
				 STM32_FSMC_BCR_MBKEN)
#  define CONFIG_SYS_FSMC_BTR	(1 << STM32_FSMC_BTR_BUSTURN_BIT) |	       \
				(2 << STM32_FSMC_BTR_DATAST_BIT)
# endif /* !CONFIG_SYS_RAM_CS || !CONFIG_SYS_FSMC_BCR || !CONFIG_SYS_FSMC_BTR */
#endif /* CONFIG_NR_DRAM_BANKS */

/*
 * STM32 RCC FSMC specific definitions
 */
#define STM32_RCC_ENR_FSMC		(1 << 0)	/* FSMC module clock  */

DECLARE_GLOBAL_DATA_PTR;

/*
 * External SRAM GPIOs for FSMC:
 *
 * D0..D15, A0..A23, NE2/1, NOE, NWE, NBL1/0, CLK, NL, NWAIT
 */
static struct stm32f2_gpio_dsc fsmc_gpio[] = {
	/* Port B: */
	{1,  7},
	/* Port D: */
	{3,  0}, {3,  1}, {3,  3}, {3,  4}, {3,  5}, {3,  6}, {3,  7}, {3,  8},
	{3,  9}, {3, 10}, {3, 11}, {3, 12}, {3, 13}, {3, 14}, {3, 15},
	/* Port E: */
	{4,  0}, {4,  1}, {4,  2}, {4,  3}, {4,  4}, {4,  5}, {4,  6}, {4,  7},
	{4,  8}, {4,  9}, {4, 10}, {4, 11}, {4, 12}, {4, 13}, {4, 14}, {4, 15},
	/* Port F: */
	{5,  0}, {5,  1}, {5,  2}, {5,  3}, {5,  4}, {5,  5}, {5, 12}, {5, 13},
	{5, 14}, {5, 15},
	/* Port G: */
	{6,  0}, {6,  1}, {6,  2}, {6,  3}, {6,  4}, {6,  5}, {6,  9}
};

/*
 * Early hardware init.
 */
int board_init(void)
{
	/*
	 * TBD
	 */

	return 0;
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
	/*
	 * TBD
	 */

	return 0;
}

/*
 * Setup external RAM.
 */
int dram_init(void)
{
	int	rv = 0;

#if (CONFIG_NR_DRAM_BANKS > 0)
	int	i;

	/*
	 * Connect GPIOs to FSMC controller
	 */
	for (i = 0; i < sizeof(fsmc_gpio)/sizeof(fsmc_gpio[0]); i++) {
		rv = stm32f2_gpio_config(&fsmc_gpio[i],
					 STM32F2_GPIO_ROLE_FSMC);
		if (rv != 0)
			goto out;
	}

	/*
	 * Enable FSMC interface clock
	 */
	STM32_RCC->ahb3enr |= STM32_RCC_ENR_FSMC;

	/*
	 * Configure FSMC
	 */
	i = CONFIG_SYS_RAM_CS - 1;

	/*
	 * Fake BCR read; if don't do this, then BCR remains configured
	 * with defaults.
	 */
	rv = STM32_FSMC->cs[i].bcr;

	STM32_FSMC->cs[i].bcr = CONFIG_SYS_FSMC_BCR;
	STM32_FSMC->cs[i].btr = CONFIG_SYS_FSMC_BTR;
#if defined(CONFIG_SYS_FSMC_BWR)
	STM32_FSMC->wt[i].wtr = CONFIG_SYS_FSMC_BWR;
#endif

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

