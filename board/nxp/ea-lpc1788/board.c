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
 * Board specific code for the Embedded Artists LPC1788 board
 */

#include <common.h>

#include <asm/arch/lpc178x_gpio.h>

/*
 * GPIO pin configuration table for EA-LPC1788
 */
static const struct lpc178x_gpio_pin_config ea_lpc1788_gpio[] = {
#if CONFIG_LPC178X_UART_PORT == 0
	/* P0.2 (D) = UART0 TXD */
	{{0,  2}, LPC178X_GPIO_CONFIG_D(1, LPC178X_NO_PULLUP, 0, 0, 0, 0)},
	/* P0.3 (D) = UART0 RXD */
	{{0,  3}, LPC178X_GPIO_CONFIG_D(1, LPC178X_NO_PULLUP, 0, 0, 0, 0)},
#elif CONFIG_LPC178X_UART_PORT == 2
	/* P0.10 (D) = U2_TXD */
	{{0, 10}, LPC178X_GPIO_CONFIG_D(1, LPC178X_NO_PULLUP, 0, 0, 0, 0)},
	/* P0.11 (D) = U2_RXD */
	{{0, 11}, LPC178X_GPIO_CONFIG_D(1, LPC178X_NO_PULLUP, 0, 0, 0, 0)},
#else /* Neither UART0 nor UART2 */
#error This configuration of GPIO pins supports only UART0 or UART2
#endif
};

/*
 * Configure all necessary GPIO pins
 */
static void gpio_init(void)
{
	/*
	 * Enable power on GPIO. This is not really necessary, because power
	 * on GPIO is enabled on SoC reset.
	 */
	lpc178x_periph_enable(LPC178X_SCC_PCONP_PCGPIO_MSK, 1);

	/*
	 * Configure GPIO pins
	 */
	lpc178x_gpio_config_table(ea_lpc1788_gpio, ARRAY_SIZE(ea_lpc1788_gpio));
}

/*
 * Early hardware init.
 */
int board_init(void)
{
	gpio_init();

	return 0;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
	printf("Board: EA-LPC1788 board %s\n",
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
	/* TBD */
	return 0;
}

