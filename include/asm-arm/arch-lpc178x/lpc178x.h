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
 * LPC178X processor definitions
 */
#ifndef _MACH_LPC178X_H_
#define _MACH_LPC178X_H_

#include <asm/byteorder.h>

/*
 * This LPC178X port assumes that the CPU works in little-endian mode.
 * Switching to big-endian will require different bit offsets in peripheral
 * devices' registers. Also, some bit groups may lay on byte edges, so issue
 * with big-endian cannot be fixed only by defining bit offsets differently
 * for the big-endian mode.
 */
#ifndef __LITTLE_ENDIAN
#error This LPC178X port assumes that the CPU works in little-endian mode
#endif

/******************************************************************************
 * Peripheral memory map
 ******************************************************************************/

#define LPC178X_APB_PERIPH_BASE		0x40000000
#define LPC178X_APB0PERIPH_BASE		(LPC178X_APB_PERIPH_BASE + 0x00000000)
#define LPC178X_APB1PERIPH_BASE		(LPC178X_APB_PERIPH_BASE + 0x00080000)

/*
 * Clocks enumeration
 */
enum clock {
	CLOCK_SYSTICK,		/* Systimer clock frequency expressed in Hz   */
	CLOCK_END		/* for internal usage			      */
};

/******************************************************************************
 * FIXME: get rid of this
 ******************************************************************************/

/*
 * Return a clock value for the specified clock.
 * Note that we need this function in RAM because it will be used
 * during self-upgrade of U-boot into eNMV.
 * @param clck          id of the clock
 * @returns             frequency of the clock
 */
unsigned long  __attribute__((section(".ramcode")))
	       __attribute__ ((long_call))
	       clock_get(enum clock clck);

#endif /* _MACH_LPC178X_H_ */
