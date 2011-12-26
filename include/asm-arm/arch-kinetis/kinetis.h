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
 * Freescale Kinetis processor definitions
 */
#ifndef _MACH_KINETIS_H_
#define _MACH_KINETIS_H_

#include <asm/byteorder.h>

/*
 * This Kinetis port assumes that the CPU works in little-endian mode.
 * Switching to big-endian will require different bit offsets in peripheral
 * devices' registers. Also, some bit groups may lay on byte edges, so issue
 * with big-endian cannot be fixed only by defining bit offsets differently
 * for the big-endian mode.
 */
#ifndef __LITTLE_ENDIAN
#error This Kinetis port assumes that the CPU works in little-endian mode
#endif

/*
 * Peripheral memory map
 */
#define KINETIS_AIPS0PERIPH_BASE	0x40000000
#define KINETIS_AIPS1PERIPH_BASE	0x40080000

/*
 * Pack the SIM_SCGC[] register index and the bit index in that register into
 * a single word. This is similar to the implementation of `dev_t` in
 * the Linux kernel with its `MAJOR(dev)`, `MINOR(dev)` and
 * `MKDEV(major,minor)` macros.
 *
 * This is useful when you want to have an array of `kinetis_clock_gate_t`s:
 * you do not have to use a 2-dimensional array or a real structure.
 */
typedef u32 kinetis_clock_gate_t;
#define KINETIS_CG_IDX_BITS	16
#define KINETIS_CG_IDX_MASK	((1U << KINETIS_CG_IDX_BITS) - 1)
/*
 * Extract the register number and the bit index from a `kinetis_clock_gate_t`.
 * The register number counts from 0,
 * i.e. the register number for SIM_SCGC7 is 6.
 */
#define KINETIS_CG_REG(gate)	((unsigned int) ((gate) >> KINETIS_CG_IDX_BITS))
#define KINETIS_CG_IDX(gate)	((unsigned int) ((gate) & KINETIS_CG_IDX_MASK))
/*
 * Build a `kinetis_clock_gate_t` from a register number and a bit index
 */
#define KINETIS_MKCG(reg,idx) \
	(((kinetis_clock_gate_t)(reg) << KINETIS_CG_IDX_BITS) | \
	(kinetis_clock_gate_t)(idx))

/*
 * Clock gates for the modules inside the MCU
 */
/* UARTs */
#define KINETIS_CG_UART0	KINETIS_MKCG(3, 10)	/* SIM_SCGC4[10] */
#define KINETIS_CG_UART1	KINETIS_MKCG(3, 11)	/* SIM_SCGC4[11] */
#define KINETIS_CG_UART2	KINETIS_MKCG(3, 12)	/* SIM_SCGC4[12] */
#define KINETIS_CG_UART3	KINETIS_MKCG(3, 13)	/* SIM_SCGC4[13] */
#define KINETIS_CG_UART4	KINETIS_MKCG(0, 10)	/* SIM_SCGC1[10] */
#define KINETIS_CG_UART5	KINETIS_MKCG(0, 11)	/* SIM_SCGC1[11] */
/* Ports */
#define KINETIS_CG_PORTA	KINETIS_MKCG(4, 9)	/* SIM_SCGC5[9] */
#define KINETIS_CG_PORTB	KINETIS_MKCG(4, 10)	/* SIM_SCGC5[10] */
#define KINETIS_CG_PORTC	KINETIS_MKCG(4, 11)	/* SIM_SCGC5[11] */
#define KINETIS_CG_PORTD	KINETIS_MKCG(4, 12)	/* SIM_SCGC5[12] */
#define KINETIS_CG_PORTE	KINETIS_MKCG(4, 13)	/* SIM_SCGC5[13] */
/* ENET */
#define KINETIS_CG_ENET		KINETIS_MKCG(1, 0)	/* SIM_SCGC2[0] */

/*
 * Enable or disable the clock on a peripheral device (timers, UARTs, USB, etc)
 */
int kinetis_periph_enable(kinetis_clock_gate_t gate, int enable);

/*
 * Clocks enumeration
 */
enum clock {
	CLOCK_SYSTICK,		/* Systimer clock frequency expressed in Hz   */
	CLOCK_CCLK,		/* Core clock frequency expressed in Hz       */
	CLOCK_PCLK,		/* Bus clock frequency expressed in Hz        */
	CLOCK_MACCLK,		/* MAC module clock frequency expressed in Hz */
	CLOCK_END		/* for internal usage			      */
};

/*
 * Return a clock value for the specified clock.
 *
 * @param clck          id of the clock
 * @returns             frequency of the clock
 */
unsigned long clock_get(enum clock clck);

#endif /* _MACH_KINETIS_H_ */
