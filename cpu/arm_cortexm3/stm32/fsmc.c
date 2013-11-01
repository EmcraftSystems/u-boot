/*
 * (C) Copyright 2010,2011
 * Dmitry Konyshev, Emcraft Systems, probables@emcraft.com
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */
#include <common.h>
#include <asm/arch/stm32.h>
#include <asm/arch/stm32f2_gpio.h>
#include <asm/errno.h>

/*
 * STM32 RCC FSMC specific definitions
 */
#define STM32_RCC_ENR_FSMC		(1 << 0)	/* FSMC module clock  */

/*
 * FSMC registers base
 */
#define STM32_FSMC_BASE			0xA0000000
#define STM32_FSMC			((volatile struct stm32_fsmc_regs *) \
					STM32_FSMC_BASE)
/*
 * FSMC NOR/PSRAM controller register map
 */
struct stm32_fsmc_regs {
	struct {
		u32	bcr;		/* Chip-select control		      */
		u32	btr;		/* Chip-select timing		      */
	} cs[4];
	u32	rsv0[57];
	struct {
		u32	bwtr;		/* Write timing			      */
		u32	rsv1;
	} wt[4];
};

/*
 * External SRAM GPIOs for FSMC:
 *
 * D0..D15, A0..A23, NOE, NWE, NBL1/0, CLK, NL, NWAIT
 */
static const struct stm32f2_gpio_dsc ext_ram_fsmc_gpio[] = {
	{STM32F2_GPIO_PORT_B, STM32F2_GPIO_PIN_7},

	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_0},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_1},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_3},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_4},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_5},
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_6},
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

#ifdef CONFIG_FSMC_NOR_PSRAM_CS1_ENABLE
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

int fsmc_gpio_init_done = 0;

int fsmc_nor_psram_init(u32 cs, u32 bcr, u32 btr, u32 bwtr)
{
	int rv = 0;

	cs--;

	if (cs > 3)
		return -EINVAL;

	if (!fsmc_gpio_init_done) {
		int	i;

		/*
		 * Connect GPIOs to FSMC controller
		 */
		for (i = 0; i < ARRAY_SIZE(ext_ram_fsmc_gpio); i++) {
			rv = stm32f2_gpio_config(&ext_ram_fsmc_gpio[i],
					STM32F2_GPIO_ROLE_FSMC);
			if (rv != 0)
				goto out;
		}

		fsmc_gpio_init_done = 1;
	}

	/*
	 * Enable FSMC interface clock
	 */
	STM32_RCC->ahb3enr |= STM32_RCC_ENR_FSMC;

	/* Errata 2.1.6 */
	__asm__ __volatile__ ("dsb" : : : "memory");

	/*
	 * Fake BCR read; if don't do this, then BCR remains configured
	 * with defaults.
	 */
	rv = STM32_FSMC->cs[cs].bcr;

	STM32_FSMC->cs[cs].btr = btr;
	if (bwtr != (u32)-1)
		STM32_FSMC->wt[cs].bwtr = bwtr;
	STM32_FSMC->cs[cs].bcr = bcr;

	rv = 0;
out:
	return rv;
}

