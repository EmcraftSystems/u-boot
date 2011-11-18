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

#include <common.h>
#include <asm/errno.h>

#include <asm/arch/lpc178x_gpio.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * List of available GPIO pins:
 *    P0[31:0]; P1[31:0]; P2[31:0]; P3[31:0]; P4[31:0]; P5[4:0]
 */
/* Number of GPIO ports */
#define LPC178X_GPIO_PORTS		6
/* Number of pins in all ports except the last one */
#define LPC178X_GPIO_NORMAL_PORT_PINS	32
/* Number of pins in the last port */
#define LPC178X_GPIO_LAST_PORT_PINS	5

/*
 * IOCON (pin connect) registers base
 */
#define LPC178X_IOCON_BASE		(LPC178X_APB0PERIPH_BASE + 0x0002C000)
/*
 * Address of the IOCON register for the given pin
 */
#define LPC178X_IOCON_PIN_ADDR(port,pin) \
	(LPC178X_IOCON_BASE + (port) * 0x80 + (pin) * 4)
/*
 * Reference to the IOCON register for the given pin
 */
#define LPC178X_IOCON(port,pin) \
	(*(volatile u32 *)LPC178X_IOCON_PIN_ADDR(port,pin))

/*
 * GPIO register map
 * Should be mapped at (0x20098000 + port * 0x20).
 */
struct lpc178x_gpio_regs {
	u32 fiodir;	/* Fast GPIO Port Direction control register */
	u32 rsv0[3];
	u32 fiomask;	/* Fast Mask register for port */
	u32 fiopin;	/* Fast Port Pin value register using FIOMASK */
	u32 fioset;	/* Fast Port Output Set register using FIOMASK */
	u32 fioclr;	/* Fast Port Output Clear register using FIOMASK */
};

/*
 * GPIO registers base
 */
#define LPC178X_GPIO_BASE		(LPC178X_AHB_PERIPH_BASE + 0x00018000)
#define LPC178X_GPIO_PORT_ADDR(port)	(LPC178X_GPIO_BASE + (port) * 0x20)
#define LPC178X_GPIO(port) \
	((volatile struct lpc178x_gpio_regs *)LPC178X_GPIO_PORT_ADDR(port))

/*
 * Check that the given (port, pin) pair is a valid LPC178x/7x GPIO pin.
 * Returns 0 on success, -EINVAL otherwise.
 */
static inline int lpc178x_validate_gpio(const struct lpc178x_gpio_dsc *dsc)
{
	int rv;

	rv = 0;

	/*
	 * P0[31:0]; P1[31:0]; P2[31:0]; P3[31:0]; P4[31:0]; P5[4:0]
	 */
	if (!dsc || dsc->port >= LPC178X_GPIO_PORTS ||
	    dsc->pin >= LPC178X_GPIO_NORMAL_PORT_PINS ||
	    (dsc->port == LPC178X_GPIO_PORTS - 1 &&
	     dsc->pin >= LPC178X_GPIO_LAST_PORT_PINS)) {
		if (gd->have_console) {
			printf("GPIO: incorrect params %d.%d.\n",
				dsc ? dsc->port : -1,
				dsc ? dsc->pin  : -1);
		}
		rv = -EINVAL;
	}

	return rv;
}

/*
 * Configure the specified GPIO pin.
 * Returns 0 on success, -EINVAL otherwise.
 */
int lpc178x_gpio_config(const struct lpc178x_gpio_dsc *dsc, u32 regval)
{
	int rv;

	rv = lpc178x_validate_gpio(dsc);
	if (rv == 0)
		LPC178X_IOCON(dsc->port, dsc->pin) = regval;

	return rv;
}

/*
 * Configure a set of GPIO pins using the given configuration table.
 * Returns 0 on success.
 */
int lpc178x_gpio_config_table(
	const struct lpc178x_gpio_pin_config *table, unsigned int len)
{
	unsigned int i;
	int rv;

	for (i = 0; i < len; i ++) {
		rv = lpc178x_gpio_config(&table[i].dsc, table[i].regval);
		if (rv != 0)
			goto out;
	}

	rv = 0;
out:
	return rv;
}

/*
 * output=0: Set a GPIO pin as an input.
 * output=1: Set a GPIO pin as an output.
 *
 * Returns 0 on success, -EINVAL otherwise.
 */
int lpc178x_gpio_config_direction(const struct lpc178x_gpio_dsc *dsc, int output)
{
	int rv;

	rv = lpc178x_validate_gpio(dsc);
	if (rv == 0) {
		if (output)
			LPC178X_GPIO(dsc->port)->fiodir |= (1 << dsc->pin);
		else
			LPC178X_GPIO(dsc->port)->fiodir &= ~(1 << dsc->pin);
	}

	return rv;
}

/*
 * Set an output GPIO pin to the state specified (1, 0).
 * Returns 0 on success, -EINVAL otherwise.
 */
int lpc178x_gpout_set(const struct lpc178x_gpio_dsc *dsc, int state)
{
	int rv;

	rv = lpc178x_validate_gpio(dsc);
	if (rv == 0) {
		if (state)
			 LPC178X_GPIO(dsc->port)->fioset = (1 << dsc->pin);
		else
			 LPC178X_GPIO(dsc->port)->fioclr = (1 << dsc->pin);
	}

	return rv;
}

/*
 * Return the state of an input GPIO.
 * Returns 0 or 1 on success, -EINVAL otherwise.
 */
int lpc178x_gpin_get(const struct lpc178x_gpio_dsc *dsc)
{
	int rv;

	rv = lpc178x_validate_gpio(dsc);
	if (rv == 0)
		rv = (LPC178X_GPIO(dsc->port)->fiopin & (1 << dsc->pin)) ? 1 : 0;

	return rv;
}

