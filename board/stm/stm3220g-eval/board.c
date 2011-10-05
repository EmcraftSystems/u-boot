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

#include <asm/arch/stm32f2.h>
#include <asm/arch/stm32f2_gpio.h>

/*
 * STM32F2 RCC FSMC specific definitions
 */
#define STM32F2_RCC_ENR_FSMC		(1 << 0)	/* FSMC module clock  */

DECLARE_GLOBAL_DATA_PTR;

/*
 * External SRAM GPIOs:
 *
 * +-----------------+------------------+----------------+----------------+
 * | PD0 <->FSMC_D2  | PE0 <->FSMC_NBL0 | PF0 <->FSMC_A0 | PG0<->FSMC_A10 |
 * | PD1 <->FSMC_D3  | PE1 <->FSMC_NBL1 | PF1 <->FSMC_A1 | PG1<->FSMC_A11 |
   | PD4 <->FSMC_NOE | PE3 <->FSMC_A19  | PF2 <->FSMC_A2 | PG2<->FSMC_A12 |
   | PD5 <->FSMC_NWE | PE4 <->FSMC_A20  | PF3 <->FSMC_A3 | PG3<->FSMC_A13 |
 * | PD8 <->FSMC_D13 | PE7 <->FSMC_D4   | PF5 <->FSMC_A5 | PG5<->FSMC_A15 |
 * | PD9 <->FSMC_D14 | PE8 <->FSMC_D5   | PF12<->FSMC_A6 | PG9<->FSMC_NE2 |
 * | PD10<->FSMC_D15 | PE9 <->FSMC_D6   | PF13<->FSMC_A7 +----------------+
 * | PD11<->FSMC_A16 | PE10<->FSMC_D7   | PF14<->FSMC_A8 |
 * | PD12<->FSMC_A17 | PE11<->FSMC_D8   | PF15<->FSMC_A9 |
 * | PD13<->FSMC_A18 | PE12<->FSMC_D9   +----------------+
 * | PD14<->FSMC_D0  | PE13<->FSMC_D10  |
 * | PD15<->FSMC_D1  | PE14<->FSMC_D11  |
 * +-----------------+ PE15<->FSMC_D12  |
 *                   +------------------+
 */
static struct stm32f2_gpio_dsc fsmc_gpio[] = {
	{3,  0}, {3,  1}, {3,  4}, {3,  5}, {3,  8}, {3,  9}, {3, 10}, {3, 11},
	{3, 12}, {3, 13}, {3, 14}, {3, 15},
	{4,  0}, {4,  1}, {4,  3}, {4,  4}, {4,  7}, {4,  8}, {4,  9}, {4, 10},
	{4, 11}, {4, 12}, {4, 13}, {4, 14}, {4, 15},
	{5,  0}, {5,  1}, {5,  2}, {5,  3}, {5,  4}, {5,  5}, {5, 12}, {5, 13},
	{5, 14}, {5, 15},
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
	int					rv = 0;

#if (CONFIG_NR_DRAM_BANKS > 0)
	volatile struct stm32f2_fsmc_regs	*fsmc_regs;
	volatile struct stm32f2_rcc_regs	*rcc_regs;
	int					i;

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
	rcc_regs = (struct stm32f2_rcc_regs *)STM32F2_RCC_BASE;
	rcc_regs->ahb3enr |= STM32F2_RCC_ENR_FSMC;

	/*
	 * Configure and enable Bank1 SRAM 2:
	 * - 16 bit data bus, SRAM, Mode 1, Enabled.
	 *
	 * The timings assume a IS61WV102416BLL high-speed asynchronous
	 * CMOS static RAM with 10ns access times, and maximum (120M) HCLK.
	 * Configure FSMC to acceptable minimals:
	 * - CLK period is HCLK (8.4s);
	 * - ADDSET  = 0 x CLK;
	 * - DATASET = 2 x CLK (16.8ns);
	 * - BUSTURN = 1 x CLK (8.4ns).
	 *
	 * These timings used both for read & write accesses (not-extended
	 * mode - WTR register isn't used).
	 */
	fsmc_regs = (struct stm32f2_fsmc_regs *)STM32F2_FSMC_BASE;

	/*
	 * Fake BCR read; if don't do this, then BCR remains configured
	 * with defaults.
	 */
	rv = fsmc_regs->cs[1].bcr;
	fsmc_regs->cs[1].bcr = STM32F2_FSMC_BCR_WREN |
			       (STM32F2_FSMC_BCR_MWID_16 <<
				STM32F2_FSMC_BCR_MWID_BIT) |
			       (STM32F2_FSMC_BCR_MTYP_SRAM_ROM <<
				STM32F2_FSMC_BCR_MTYP_BIT) |
			       STM32F2_FSMC_BCR_MBKEN;
	fsmc_regs->cs[1].btr = (1 << STM32F2_FSMC_BTR_BUSTURN_BIT) |
			       (2 << STM32F2_FSMC_BTR_DATAST_BIT);

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

#ifdef CONFIG_STM32F2_ETH
/*
 * Register ethernet driver
 */
int board_eth_init(bd_t *bis)
{
	return stm32f2_eth_init(bis);
}
#endif

