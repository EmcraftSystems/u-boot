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
#define KINETIS_CG_PORTF	KINETIS_MKCG(4, 14)	/* SIM_SCGC5[14] */
/* ENET */
#define KINETIS_CG_ENET		KINETIS_MKCG(1, 0)	/* SIM_SCGC2[0] */
/* DDR */
#define KINETIS_CG_DDR		KINETIS_MKCG(2, 14)	/* SIM_SCGC3[14] */
/* NAND Flash Controller */
#define KINETIS_CG_NFC		KINETIS_MKCG(2, 8)	/* SIM_SCGC3[8] */
/* OSC1 */
#define KINETIS_CG_OSC1		KINETIS_MKCG(0, 5)	/* SIM_SCGC1[5] */
/* RTC */
#define KINETIS_CG_RTC		KINETIS_MKCG(5, 29)	/* SIM_SCGC6[29] */

/*
 * Limits for the `kinetis_periph_enable()` function:
 *     1. The number of SIM_SCGC[] registers
 *     2. The number of bits in those registers
 */
#define KINETIS_SIM_CG_NUMREGS	7
#define KINETIS_SIM_CG_NUMBITS	32

/*
 * System Integration Module (SIM) register map
 *
 * This map actually covers two hardware modules:
 *     1. SIM low-power logic, at 0x40047000
 *     2. System integration module (SIM), at 0x40048000
 */
struct kinetis_sim_regs {
	u32 sopt1;	/* System Options Register 1 */
	u32 rsv0[1024];
	u32 sopt2;	/* System Options Register 2 */
	u32 rsv1;
	u32 sopt4;	/* System Options Register 4 */
	u32 sopt5;	/* System Options Register 5 */
	u32 sopt6;	/* System Options Register 6 */
	u32 sopt7;	/* System Options Register 7 */
	u32 rsv2[2];
	u32 sdid;	/* System Device Identification Register */
	u32 scgc[KINETIS_SIM_CG_NUMREGS];	/* Clock Gating Regs 1...7 */
	u32 clkdiv1;	/* System Clock Divider Register 1 */
	u32 clkdiv2;	/* System Clock Divider Register 2 */
	u32 fcfg1;	/* Flash Configuration Register 1 */
	u32 fcfg2;	/* Flash Configuration Register 2 */
	u32 uidh;	/* Unique Identification Register High */
	u32 uidmh;	/* Unique Identification Register Mid-High */
	u32 uidml;	/* Unique Identification Register Mid Low */
	u32 uidl;	/* Unique Identification Register Low */
	u32 clkdiv3;	/* System Clock Divider Register 3 */
	u32 clkdiv4;	/* System Clock Divider Register 4 */
	u32 mcr;	/* Misc Control Register */
};

/*
 * SIM registers base
 */
#define KINETIS_SIM_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x00047000)
#define KINETIS_SIM			((volatile struct kinetis_sim_regs *) \
					KINETIS_SIM_BASE)

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
#ifdef CONFIG_KINETIS_DDR
	CLOCK_DDRCLK,		/* DDR clock frequency expressed in Hz        */
#endif /* CONFIG_KINETIS_DDR */
	CLOCK_END		/* for internal usage			      */
};

/*
 * Multipurpose Clock Generator (MCG) register map
 *
 * See Chapter 24 of the K60 Reference Manual (page 559)
 */
struct kinetis_mcg_regs {
	u8 c1;		/* MCG Control 1 Register */
	u8 c2;		/* MCG Control 2 Register */
	u8 c3;		/* MCG Control 3 Register */
	u8 c4;		/* MCG Control 4 Register */
	u8 c5;		/* MCG Control 5 Register */
	u8 c6;		/* MCG Control 6 Register */
	u8 status;	/* MCG Status Register */
	u8 rsv0;
	u8 atc;		/* MCG Auto Trim Control Register */
	u8 rsv1;
	u8 atcvh;	/* MCG Auto Trim Compare Value High Register */
	u8 atcvl;	/* MCG Auto Trim Compare Value Low Register */
	u8 c7;		/* MCG Control 7 Register */
	u8 c8;		/* MCG Control 8 Register */
	u8 rsv2;
	u8 c10;		/* MCG Control 10 Register */
	u8 c11;		/* MCG Control 11 Register */
	u8 c12;		/* MCG Control 12 Register */
	u8 status2;	/* MCG Status 2 Register */
};

/*
 * MCG registers base
 */
#define KINETIS_MCG_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x00064000)
#define KINETIS_MCG			((volatile struct kinetis_mcg_regs *) \
					KINETIS_MCG_BASE)

/*
 * Return a clock value for the specified clock.
 *
 * @param clck          id of the clock
 * @returns             frequency of the clock
 */
unsigned long clock_get(enum clock clck);

#endif /* _MACH_KINETIS_H_ */
