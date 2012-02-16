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

#include <asm/arch/kinetis_gpio.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * PORTx register map
 */
struct kinetis_port_regs {
	u32 pcr[32];	/* Pin Control Registers */
	u32 gpclr;	/* Global Pin Control Low Register */
	u32 gpchr;	/* Global Pin Control High Register */
	u32 rsv0[6];
	u32 isfr;	/* Interrupt Status Flag Register */
	u32 rsv1[7];
	u32 dfer;	/* Digital Filter Enable Register */
	u32 dfcr;	/* Digital Filter Clock Register */
	u32 dfwr;	/* Digital Filter Width Register */
};

/*
 * PORTx registers base
 */
#define KINETIS_PORT_BASE(port)		(KINETIS_AIPS0PERIPH_BASE + \
					0x00049000 + (port) * 0x1000)
#define KINETIS_PORT(port)		((volatile struct kinetis_port_regs *) \
					KINETIS_PORT_BASE(port))

/*
 * Clock gates for the I/O ports: 0..5 <-> A..F
 *
 * These values can be passed into the `kinetis_periph_enable()` function.
 */
static const kinetis_clock_gate_t port_clock_gate[] = {
	KINETIS_CG_PORTA, KINETIS_CG_PORTB, KINETIS_CG_PORTC,
	KINETIS_CG_PORTD, KINETIS_CG_PORTE, KINETIS_CG_PORTF,
};

/*
 * Check that the given (port, pin) pair is a valid Kinetis GPIO pin.
 * Returns 0 on success, -EINVAL otherwise.
 */
static inline int kinetis_validate_gpio(const struct kinetis_gpio_dsc *dsc)
{
	int rv;

	rv = 0;

	/*
	 * A[31:0]; B[31:0]; C[31:0]; D[31:0]; E[31:0]; optionally F[31:0]
	 */
	if (!dsc || dsc->port >= KINETIS_GPIO_PORTS ||
	    dsc->pin >= KINETIS_GPIO_PORT_PINS) {
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
 * The clocks on the necessary ports will be enabled automatically.
 *
 * Returns 0 on success, -EINVAL otherwise.
 */
int kinetis_gpio_config(const struct kinetis_gpio_dsc *dsc, u32 regval)
{
	int rv;

	/*
	 * Verify the function arguments
	 */
	rv = kinetis_validate_gpio(dsc);
	if (rv != 0)
		goto out;

	/*
	 * Enable the clock on the port we are going to use
	 */
	rv = kinetis_periph_enable(port_clock_gate[dsc->port], 1);
	if (rv != 0)
		goto out;

	/*
	 * Configure the pin
	 */
	KINETIS_PORT(dsc->port)->pcr[dsc->pin] = regval;

	rv = 0;
out:
	return rv;
}

/*
 * Configure a set of GPIO pins using the given configuration table.
 * Returns 0 on success.
 */
int kinetis_gpio_config_table(
	const struct kinetis_gpio_pin_config *table, unsigned int len)
{
	unsigned int i;
	int rv;

	for (i = 0; i < len; i ++) {
		rv = kinetis_gpio_config(&table[i].dsc, table[i].regval);
		if (rv != 0)
			goto out;
	}

	rv = 0;
out:
	return rv;
}
