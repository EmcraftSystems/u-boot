/*
 * (C) Copyright 2011
 *
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
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
 * Board specific code for the Freescale TWR-K60N512 board
 */

#include <common.h>
#include <netdev.h>

#include <asm/arch/kinetis_gpio.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * GPIO pin configuration table for TWR-K60N512
 *
 * This table does not list all GPIO pins that will be configured. See also
 * the code in `gpio_init()` and in the drivers (for example, the UART driver).
 */
static const struct kinetis_gpio_pin_config twr_k60n512_gpio[] = {
#ifdef CONFIG_MCFFEC
	/* A.5 = RMII0_RXER */
	{{KINETIS_GPIO_PORT_A,  5}, KINETIS_GPIO_CONFIG_PULLDOWN(4)},
	/* A.12 = RMII0_RXD1 */
	{{KINETIS_GPIO_PORT_A, 12}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* A.13 = RMII0_RXD0 */
	{{KINETIS_GPIO_PORT_A, 13}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* A.14 = RMII0_CRS_DV */
	{{KINETIS_GPIO_PORT_A, 14}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* A.15 = RMII0_TXEN */
	{{KINETIS_GPIO_PORT_A, 15}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* A.16 = RMII0_TXD0 */
	{{KINETIS_GPIO_PORT_A, 16}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* A.17 = RMII0_TXD1 */
	{{KINETIS_GPIO_PORT_A, 17}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* B.0 = RMII0_MDIO */
	{{KINETIS_GPIO_PORT_B,  0}, KINETIS_GPIO_CONFIG_MUX(4)},
	/* B.1 = RMII0_MDC */
	{{KINETIS_GPIO_PORT_B,  1}, KINETIS_GPIO_CONFIG_MUX(4)},
#endif /* CONFIG_MCFFEC */
};

/*
 * Configure all necessary GPIO pins
 */
static void gpio_init(void)
{
	/*
	 * Configure GPIO pins using the `twr_k60n512_gpio[]` table
	 */
	kinetis_gpio_config_table(twr_k60n512_gpio, ARRAY_SIZE(twr_k60n512_gpio));
}

/*
 * Early hardware init.
 */
int board_init(void)
{
	/*
	 * Enable GPIO pins
	 */
	gpio_init();

	return 0;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
	printf("Board: Freescale TWR-K60N512 rev %s\n",
		CONFIG_SYS_BOARD_REV_STR);

	return 0;
}

/*
 * Configure board specific parts.
 */
int misc_init_r(void)
{
	/* TBD */
	return 0;
}

/*
 * Setup external RAM.
 */
int dram_init(void)
{
	/* TBD */
	return 0;
}

#ifdef CONFIG_MCFFEC
/*
 * Register the Ethernet driver
 */
int board_eth_init(bd_t *bis)
{
	return mcffec_initialize(bis);
}
#endif

