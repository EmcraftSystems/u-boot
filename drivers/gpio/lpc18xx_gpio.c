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

#include <common.h>
#include <asm/errno.h>

#include <asm/arch/lpc18xx_gpio.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * 16 pin groups. Number of pins in each group is limited to 32.
 */
/* Number of IOMUX pin groups */
#define LPC18XX_IOMUX_GROUPS		16
/* Maximum number of pins in each group */
#define LPC18XX_IOMUX_GROUP_PINS	32

/*
 * Pins CLK0..CLK3 with imaginary numbers 0x18.0-0x18.3
 */
/* Index of the the imaginary group of pins */
#define LPC18XX_IOMUX_CLK_GROUP		24
/* Number of CLK0..CLK3 pins */
#define LPC18XX_IOMUX_CLK_PINS		4

/*
 * System Control Unit (SCU) registers base
 */
#define LPC18XX_SCU_BASE	(LPC18XX_APB0PERIPH_BASE + 0x00006000)
/*
 * Address of the SCU_SFS register for the given pin
 */
#define LPC18XX_PIN_REG_ADDR(group,pin) \
	(LPC18XX_SCU_BASE + (group) * 0x80 + (pin) * 4)
/*
 * Reference to the SCU_SFS register for the given pin
 */
#define LPC18XX_PIN(group,pin) \
	(*(volatile u32 *)LPC18XX_PIN_REG_ADDR(group,pin))

/*
 * Check that the given (pin group, pin) pair is a valid LPC18xx pin.
 * Returns 0 on success, -EINVAL otherwise.
 */
static inline int lpc18xx_validate_pin(const struct lpc18xx_iomux_dsc *dsc)
{
	int rv;

	rv = 0;

	if (!dsc ||
		((dsc->group >= LPC18XX_IOMUX_GROUPS ||
			dsc->pin >= LPC18XX_IOMUX_GROUP_PINS) &&
		(dsc->group != LPC18XX_IOMUX_CLK_GROUP ||
			dsc->pin >= LPC18XX_IOMUX_CLK_PINS))) {
		if (gd->have_console) {
			printf("IOMUX: incorrect params %d.%d.\n",
				dsc ? dsc->group : -1,
				dsc ? dsc->pin : -1);
		}
		rv = -EINVAL;
	}

	return rv;
}

/*
 * Configure the specified MCU pin.
 * Returns 0 on success, -EINVAL otherwise.
 */
int
#ifdef CONFIG_LPC18XX_NORFLASH_BOOTSTRAP_WORKAROUND
	__attribute__((section(".lpc18xx_image_top_text")))
#endif
	lpc18xx_pin_config(const struct lpc18xx_iomux_dsc *dsc, u32 regval)
{
	int rv;

	rv = lpc18xx_validate_pin(dsc);
	if (rv == 0)
		LPC18XX_PIN(dsc->group, dsc->pin) = regval;

	return rv;
}

/*
 * Configure a set of MCU pins using the given configuration table.
 * Returns 0 on success.
 */
int
#ifdef CONFIG_LPC18XX_NORFLASH_BOOTSTRAP_WORKAROUND
	__attribute__((section(".lpc18xx_image_top_text")))
#endif
	lpc18xx_pin_config_table(
		const struct lpc18xx_pin_config *table, unsigned int len)
{
	unsigned int i;
	int rv;

	for (i = 0; i < len; i ++) {
		rv = lpc18xx_pin_config(&table[i].dsc, table[i].regval);
		if (rv != 0)
			goto out;
	}

	rv = 0;
out:
	return rv;
}

/*
 * Set direction for GPIO pin.
 */
void lpc_gpio_dir(struct lpc18xx_iomux_dsc pin, u8 dir)
{
	if (dir) {
		LPC18XX_GPIO->dir[pin.group] |= (1 << pin.pin);
	} else {
		LPC18XX_GPIO->dir[pin.group] &= ~(1 << pin.pin);
	}
}

void lpc_gpio_set(struct lpc18xx_iomux_dsc pin)
{
	LPC18XX_GPIO->set[pin.group] = (1 << pin.pin);
}

void lpc_gpio_clear(struct lpc18xx_iomux_dsc pin)
{
	LPC18XX_GPIO->clr[pin.group] = (1 << pin.pin);
}
