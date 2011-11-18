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

#define LPC178X_AHB_PERIPH_BASE		0x20080000

/*
 * 1-bit masks for PCONP (Power Control for Peripherals register) for different
 * peripherals that enable power on them. One of these masks should be passed
 * as the first argument of `lpc178x_periph_enable`.
 */
#define LPC178X_SCC_PCONP_PCUART0_MSK	(1 << 3)
#define LPC178X_SCC_PCONP_PCUART1_MSK	(1 << 4)
#define LPC178X_SCC_PCONP_PCUART4_MSK	(1 << 8)
#define LPC178X_SCC_PCONP_PCEMC_MSK	(1 << 11)
#define LPC178X_SCC_PCONP_PCGPIO_MSK	(1 << 15)
#define LPC178X_SCC_PCONP_PCUART2_MSK	(1 << 24)
#define LPC178X_SCC_PCONP_PCUART3_MSK	(1 << 25)

/*
 * PLL register map
 * Used for PLL0 at 0x400FC080 and for PLL1 at 0x400FC0A0.
 */
struct lpc178x_pll_regs {
	u32 con;	/* PLL Control register */
	u32 cfg;	/* PLL Configuration register */
	u32 stat;	/* PLL Status register */
	u32 feed;	/* PLL Feed register */
};

/*
 * SCC (System and Clock Control) register map
 * Should be mapped at 0x400FC000.
 *
 * This structure is used by the code in `clock.c` and `power.c`.
 */
/*
 * TODO: convert to STM32/SmartFusion structure style,
 * i.e. use "rsvN[M]" hole fillers instead of "union"s for alignment.
 * (Not doing this until most lpc178x device drivers are implemented
 * and all necessary registers in this structure are known.)
 */
struct lpc178x_scc_regs {
	/* 0x400FC000: Flash Accelerator Configuration Register */
	union {
		u8 align0[0x80];
	};

	/* 0x400FC080: PLL0 registers */
	union {
		u8 align1[0x20];
		struct lpc178x_pll_regs pll0; /* PLL0 registers */
	};

	/* 0x400FC0A0: PLL1 registers */
	union {
		u8 align2[0x20];
		struct lpc178x_pll_regs pll1; /* PLL1 registers */
	};

	/* 0x400FC0C0: Power control registers */
	union {
		u8 align3[0x40];
		struct {
			u32 pcon;       /* Power Control register */
			u32 pconp;      /* Power Control for Peripherals */
		};
	};

	/* 0x400FC100: Clock control */
	union {
		u8 align4[0xA0];
		struct {
			u32 emcclksel;	/* External Memory Controller
						Clock Selection register */
			u32 cclksel;	/* CPU Clock Selection register */
			u32 usbclksel;	/* USB Clock Selection register */
			u32 clksrcsel;	/* Clock Source Selection register */
		};
	};

	/* 0x400FC1A0 */
	u32 scs;	/* System Controls and Status register */
	u32 rsv0;
	u32 pclksel;	/* Peripheral Clock Selection register */
	/* 0x400FC1AC */
	u32 rsv1[7];

	/* 0x400FC1C8 */
	u32 clkoutcfg;
	u32 rstcon0;
	u32 rstcon1;
	/* 0x400FC1D4 */
	u32 rsv2[2];

	/* 0x400FC1DC */
	u32 emcdlyctl;
	u32 emccal;
};

/*
 * SCC registers base
 */
#define LPC178X_SCC_BASE		(LPC178X_APB1PERIPH_BASE + 0x0007C000)
#define LPC178X_SCC			((volatile struct lpc178x_scc_regs *) \
					LPC178X_SCC_BASE)

/*
 * Enable or disable power on a peripheral device (timers, UARTs, USB, etc)
 */
extern void lpc178x_periph_enable(u32 pconp_mask, int enable);

/*
 * Clocks enumeration
 */
enum clock {
	CLOCK_SYSTICK,		/* Systimer clock frequency expressed in Hz   */
	CLOCK_EMCCLK,		/* EMC clock frequency expressed in Hz        */
	CLOCK_PCLK,		/* Peripheral clock frequency expressed in Hz */
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
