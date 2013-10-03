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
 * LPC18XX processor definitions
 */
#ifndef _MACH_LPC18XX_H_
#define _MACH_LPC18XX_H_

#include <asm/byteorder.h>

/*
 * This LPC18XX port assumes that the CPU works in little-endian mode.
 * Switching to big-endian will require different bit offsets in peripheral
 * devices' registers. Also, some bit groups may lay on byte edges, so issue
 * with big-endian cannot be fixed only by defining bit offsets differently
 * for the big-endian mode.
 */
#ifndef __LITTLE_ENDIAN
#error This LPC18XX port assumes that the CPU works in little-endian mode
#endif

/*
 * Peripheral memory map
 */
#define LPC18XX_PERIPH_BASE		0x40000000
#define LPC18XX_APB0PERIPH_BASE		0x40080000

/*
 * Clocks enumeration
 */
enum clock {
	CLOCK_SYSTICK,		/* Systimer clock rate expressed in Hz        */
	CLOCK_CCLK,		/* CPU core base clock rate expressed in Hz   */
	CLOCK_UART0,		/* UART0 base clock rate expressed in Hz      */
	CLOCK_UART1,		/* UART1 base clock rate expressed in Hz      */
	CLOCK_UART2,		/* UART2 base clock rate expressed in Hz      */
	CLOCK_UART3,		/* UART3 base clock rate expressed in Hz      */
	CLOCK_SPI,		/* SPI base clock rate expressed in Hz      */
	CLOCK_END		/* for internal usage			      */
};

/*
 * Return a clock value for the specified clock.
 *
 * @param clck          id of the clock
 * @returns             frequency of the clock
 */
unsigned long clock_get(enum clock clck);

/*
 * Trigger watchdog reset on LPC18xx/43xx
 */
void lpc18xx_reset_cpu(void);

/*
 * Set LPC18XX_PLL1_CLK_OUT to the output rate of PLL1
 */
#define LPC18XX_PLL1_CLK_OUT \
	(CONFIG_LPC18XX_EXTOSC_RATE * CONFIG_LPC18XX_PLL1_M)

#endif /* _MACH_LPC18XX_H_ */
