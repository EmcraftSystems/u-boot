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

#ifndef _LPC178X_GPIO_H_
#define _LPC178X_GPIO_H_

/*
 * Bits and bit groups inside the IOCON registers
 */
/* Selects pin function */
#define LPC178X_GPIO_CONFIG_FUNC_BITS	0
/*
 * Selects function mode (LPC178X_NO_PULLUP, LPC178X_PULLDOWN, LPC178X_PULLUP
 * or LPC178X_REPEATER)
 */
#define LPC178X_GPIO_CONFIG_MODE_BITS	3
/* Hysteresis (1=enable, 0=disable) */
#define LPC178X_GPIO_CONFIG_HYS_BIT	5
/* Input polarity (1=input is inverted) */
#define LPC178X_GPIO_CONFIG_INV_BIT	6
/* Select Analog/Digital mode (0=analog, 1=digital) */
#define LPC178X_GPIO_CONFIG_ADMODE_BIT	7
#define LPC178X_GPIO_CONFIG_FILTER_BIT	8	/* Types A and W */
#define LPC178X_GPIO_CONFIG_HS_BIT	8	/* Type I */
#define LPC178X_GPIO_CONFIG_SLEW_BIT	9	/* Types D and W */
#define LPC178X_GPIO_CONFIG_HIDRIVE_BIT	9	/* Type I */
#define LPC178X_GPIO_CONFIG_OD_BITS	10
#define LPC178X_GPIO_CONFIG_DACEN_BIT	16

/*
 * These macros should be used to compute the value for the second argument of
 * `lpc178x_gpio_config()` (`regval`). This value will be copied into an IOCON
 * register.
 */
/*
 * Type D pins (digital pins)
 */
#define LPC178X_GPIO_CONFIG_D(func,mode,hys,inv,slew,od) \
	((func << LPC178X_GPIO_CONFIG_FUNC_BITS) | \
	(mode  << LPC178X_GPIO_CONFIG_MODE_BITS) | \
	(hys   << LPC178X_GPIO_CONFIG_HYS_BIT  ) | \
	(inv   << LPC178X_GPIO_CONFIG_INV_BIT  ) | \
	(slew  << LPC178X_GPIO_CONFIG_SLEW_BIT ) | \
	(od    << LPC178X_GPIO_CONFIG_OD_BITS  ))
/*
 * TBD: similar macros for other pin types (A, U, I, W)
 */

/*
 * Function mode for pins.
 *
 * One of these should be passed into LPC178X_GPIO_CONFIG_[DAUIW]() macros as
 * the "mode" argument.
 */
#define LPC178X_NO_PULLUP	0
#define LPC178X_PULLDOWN	1
#define LPC178X_PULLUP		2
#define LPC178X_REPEATER	3

/*
 * GPIO descriptor
 */
struct lpc178x_gpio_dsc {
	unsigned int port;	/* GPIO port */
	unsigned int pin;	/* GPIO pin */
};

struct lpc178x_gpio_pin_config {
	struct lpc178x_gpio_dsc dsc;
	u32 regval;	/* Value for writing into the IOCON register */
};

/*
 * Configure the specified GPIO pin.
 * Returns 0 on success, -EINVAL otherwise.
 */
int lpc178x_gpio_config(const struct lpc178x_gpio_dsc *dsc, u32 regval);

/*
 * Configure a set of GPIO pins using the given configuration table.
 * Returns 0 on success.
 */
extern int lpc178x_gpio_config_table(
	const struct lpc178x_gpio_pin_config *table, unsigned int len);

/*
 * Set a GPIO pin as an input.
 * Returns 0 on success, -EINVAL otherwise.
 */
int lpc178x_gpio_set_input(const struct lpc178x_gpio_dsc *dsc);

/*
 * Set a GPIO pin as an output.
 * Returns 0 on success, -EINVAL otherwise.
 */
int lpc178x_gpio_set_output(const struct lpc178x_gpio_dsc *dsc);

/*
 * Set an output GPIO pin to the state specified (1, 0).
 * Returns 0 on success, -EINVAL otherwise.
 */
int lpc178x_gpout_set(const struct lpc178x_gpio_dsc *gpio_dsc, int state);

/*
 * Return the state of an input GPIO.
 * Returns 0 or 1 on success, -EINVAL otherwise.
 */
int lpc178x_gpin_get(const struct lpc178x_gpio_dsc *gpio_dsc);

#endif /* _LPC178X_GPIO_H_ */
