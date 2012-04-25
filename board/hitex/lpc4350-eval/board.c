/*
 * (C) Copyright 2012
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
 * Board specific code for the Hitex LPC4350 Eval board
 */

#include <common.h>
#include <netdev.h>

#include <asm/arch/lpc18xx_gpio.h>

/*
 * Pin configuration table for Hitex LPC4350 Eval.
 *
 * This table does not list all MCU pins that will be configured. See also
 * the code in `iomux_init()`.
 */
static const struct lpc18xx_pin_config hitex_lpc4350_iomux[] = {
	/*
	 * Pin configuration for UART
	 */
	{{CONFIG_LPC18XX_UART_TX_IO_GROUP, CONFIG_LPC18XX_UART_TX_IO_PIN},
		LPC18XX_IOMUX_CONFIG(1, 0, 1, 0, 0, 0)},
	{{CONFIG_LPC18XX_UART_RX_IO_GROUP, CONFIG_LPC18XX_UART_RX_IO_PIN},
		LPC18XX_IOMUX_CONFIG(1, 0, 1, 0, 1, 0)},
};

/*
 * Configure all necessary MCU pins
 */
static void iomux_init(void)
{
	/*
	 * Configure GPIO pins using the `hitex_lpc4350_iomux[]` table
	 */
	lpc18xx_pin_config_table(
		hitex_lpc4350_iomux, ARRAY_SIZE(hitex_lpc4350_iomux));
}

/*
 * Early hardware init.
 */
int board_init(void)
{
	/*
	 * Configure MCU pins
	 */
	iomux_init();

	return 0;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
	printf("Board: Hitex LPC4350 Eval rev %s\n",
		CONFIG_SYS_BOARD_REV_STR);

	return 0;
}

/*
 * Configure board specific parts.
 */
#ifdef CONFIG_MISC_INIT_R
int misc_init_r(void)
{
	/* TBD */
	return 0;
}
#endif /* CONFIG_MISC_INIT_R */

/*
 * Setup external RAM.
 */
int dram_init(void)
{
	return 0;
}
