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

#ifndef _LPC18XX_IOMUX_H_
#define _LPC18XX_IOMUX_H_

/*
 * Bits and bit groups inside the SCU_SFS registers
 */
/* Selects pin function */
#define LPC18XX_IOMUX_CONFIG_FUNC_BITS	0
/* Enable pull-down resistor at pad */
#define LPC18XX_IOMUX_CONFIG_EPD_BIT	3
/* Disable pull-up resistor at pad */
#define LPC18XX_IOMUX_CONFIG_EPUN_BIT	4
/* Select Slew rate */
#define LPC18XX_IOMUX_CONFIG_EHS_BIT	5
/* Input buffer enable */
#define LPC18XX_IOMUX_CONFIG_EZI_BIT	6
/* Input glitch filter */
#define LPC18XX_IOMUX_CONFIG_ZIF_BIT	7

/*
 * These macros should be used to compute the value for the second argument of
 * `lpc18xx_iomux_config()` (`regval`). This value will be copied
 * into an SCU_SFS register.
 */
/*
 * Normal drive pins
 */
#define LPC18XX_IOMUX_CONFIG(func,epd,epun,ehs,ezi,zif) \
	((func << LPC18XX_IOMUX_CONFIG_FUNC_BITS) | \
	(epd   << LPC18XX_IOMUX_CONFIG_EPD_BIT) | \
	(epun  << LPC18XX_IOMUX_CONFIG_EPUN_BIT) | \
	(ehs   << LPC18XX_IOMUX_CONFIG_EHS_BIT) | \
	(ezi   << LPC18XX_IOMUX_CONFIG_EZI_BIT) | \
	(zif   << LPC18XX_IOMUX_CONFIG_ZIF_BIT))
/*
 * TBD: similar macros for other pin types
 */

/*
 * IOMUX pin descriptor
 */
struct lpc18xx_iomux_dsc {
	unsigned int group;	/* IOMUX pin group */
	unsigned int pin;	/* IOMUX pin number */
};

struct lpc18xx_pin_config {
	struct lpc18xx_iomux_dsc dsc;
	u32 regval;	/* Value for writing into the SCU_SFS register */
};

/*
 * Configure the specified MCU pin.
 * Returns 0 on success, -EINVAL otherwise.
 */
int lpc18xx_iomux_config(const struct lpc18xx_iomux_dsc *dsc, u32 regval);

/*
 * Configure a set of MCU pins using the given configuration table.
 * Returns 0 on success.
 */
extern int lpc18xx_pin_config_table(
	const struct lpc18xx_pin_config *table, unsigned int len);

#endif /* _LPC18XX_IOMUX_H_ */
