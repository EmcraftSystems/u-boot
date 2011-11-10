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

#include "clock.h"

/*
 * Clock configuration options that are necessary to use the external
 * oscillator and for maximum CPU clock.
 *
 * Moved here to simplify the board configuration file.
 */
/*
 * Use the external oscillator instead of the internal oscillator (IRC).
 *
 * If defined CONFIG_LPC178X_SYS_CLK_IRC
 *  PLL0_CLK_OUT = CONFIG_LPC178X_PLL0_M * 12000000 (internal oscillator)
 * else
 *  PLL0_CLK_OUT = CONFIG_LPC178X_PLL0_M * CONFIG_LPC178X_EXTOSC_RATE
 */
#undef CONFIG_LPC178X_SYS_CLK_IRC

/*
 * Set CONFIG_LPC178X_PLL0_ENABLE to enable PLL0. If PLL0 is enabled,
 * the values of CONFIG_LPC178X_PLL0_M and CONFIG_LPC178X_PLL0_PSEL must also
 * be setup to configure the PLL0 rate based on the selected input clock.
 * See the LCP178x/7x User's Manual for information on setting these
 * values correctly. SYSCLK is used as the PLL0 input clock.
 */
#define CONFIG_LPC178X_PLL0_ENABLE

/*
 * Use the output of PLL0 for CPU clock.
 */
#define CONFIG_LPC178X_PLL0_FOR_CPU

/*
 * Clock values
 */
static u32 clock_val[CLOCK_END];

/*
 * Bit offsets in System and Clock Control (SCC) registers
 */
/*
 * System Controls and Status register bits
 */
#define LPC178X_SCC_SCS_OSCRANGE_BIT		4
#define LPC178X_SCC_SCS_OSCEN_BIT		5
#define LPC178X_SCC_SCS_OSCSTAT_BIT		6

/*
 * Clock Source Selection register bits
 */
#define LPC178X_SCC_CLKSRCSEL_CLKSRC_BIT	0

/*
 * PLL Configuration register bits
 */
#define LPC178X_SCC_PLLCFG_MSEL_BITS		0
#define LPC178X_SCC_PLLCFG_PSEL_BITS		5

/*
 * PLL Status register bits
 */
#define LPC178X_SCC_PLLSTAT_PLOCK_BIT		10

/*
 * CPU Clock Selection register bits
 */
#define LPC178X_SCC_CCLKSEL_CCLKDIV_BITS	0
#define LPC178X_SCC_CCLKSEL_CCLKSEL_BIT		8

/*
 * Calculate clock rates
 */
/*
 * Set LPC178X_PLL0_P to the PLL divider value.
 *
 * P (PPL divider value) = 2 in the power of PSEL
 * P can be 1, 2, 4 or 8.
 */
#define LPC178X_PLL0_P		(1 << CONFIG_LPC178X_PLL0_PSEL)

/*
 * Set LPC178X_OSC_RATE to the rate of the oscillator actually used.
 */
#ifdef CONFIG_LPC178X_SYS_CLK_IRC
#define LPC178X_OSC_RATE	12000000
#else
#define LPC178X_OSC_RATE	CONFIG_LPC178X_EXTOSC_RATE
#endif

/*
 * Set LPC178X_PLL0_CLK_OUT to the output rate of PLL0.
 */
#ifdef CONFIG_LPC178X_PLL0_ENABLE
#define LPC178X_PLL0_CLK_OUT	(LPC178X_OSC_RATE * CONFIG_LPC178X_PLL0_M)
#else
#define LPC178X_PLL0_CLK_OUT	0
#endif

/*
 * Set LPC178X_CPU_RATE to the CPU clock.
 */
#ifdef CONFIG_LPC178X_PLL0_FOR_CPU
#define LPC178X_CPU_RATE	(LPC178X_PLL0_CLK_OUT / CONFIG_LPC178X_CPU_DIV)
#else
#define LPC178X_CPU_RATE	(LPC178X_OSC_RATE / CONFIG_LPC178X_CPU_DIV)
#endif

/*
 * Compile time sanity checks for defined board clock setup
 */
#ifndef CONFIG_LPC178X_EXTOSC_RATE
#error CONFIG_LPC178X_EXTOSC_RATE is not setup, set to the external osc rate
#endif

/*
 * Verify CPU divider
 */
#ifndef CONFIG_LPC178X_CPU_DIV
#error CONFIG_LPC178X_CPU_DIV is not setup, must be between 1 and 31
#endif
#if CONFIG_LPC178X_CPU_DIV < 1
#error CONFIG_LPC178X_CPU_DIV is too low, 1 is the minimum
#endif
#if CONFIG_LPC178X_CPU_DIV > 31
#error CONFIG_LPC178X_CPU_DIV is too high, 31 is the maximum
#endif

/*
 * PLL0 checks
 */
#if defined(CONFIG_LPC178X_PLL0_FOR_CPU) && \
    !defined(CONFIG_LPC178X_PLL0_ENABLE)
#error Enable PLL0 before using it for CPU clock
#endif

/*
 * Verify that PLL0 FCCO fits the range of 156 MHz to 320 MHz
 */
#ifdef CONFIG_LPC178X_PLL0_ENABLE
#if (LPC178X_PLL0_CLK_OUT * 2 * LPC178X_PLL0_P) < 156000000
#error PLL0 FCCO rate is too low
#endif
#if (LPC178X_PLL0_CLK_OUT * 2 * LPC178X_PLL0_P) > 320000000
#error PLL0 FCCO rate is too high
#endif
#endif /* CONFIG_LPC178X_PLL0_ENABLE */

/*
 * Feed sequence values for pll_latch()
 */
#define LPC178X_PLL_FEED_KEY1	0xAA
#define LPC178X_PLL_FEED_KEY2	0x55

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

	/* 0x400FC1A0: System Controls and Status register */
	u32 scs;
};

/*
 * SCC registers base
 */
#define LPC178X_SCC_BASE		(LPC178X_APB1PERIPH_BASE + 0x0007C000)
#define LPC178X_SCC			((volatile struct lpc178x_scc_regs *) \
					LPC178X_SCC_BASE)

/*
 * Apply changes made in PLLCON and PLLCFG
 *
 * `pll_regs` can be either `&LPC178X_SCC->pll0` or `&LPC178X_SCC->pll1`.
 */
static inline void pll_latch(volatile struct lpc178x_pll_regs *pll_regs)
{
	pll_regs->con = 1;	/* Set "PLL Enable" bit to 1 */
	pll_regs->feed = LPC178X_PLL_FEED_KEY1;
	pll_regs->feed = LPC178X_PLL_FEED_KEY2;
}

/*
 * Set-up the main oscillator, PLL0 and CPU clock (if necessary)
 */
static void clock_setup(void)
{
#ifndef CONFIG_LPC178X_SYS_CLK_IRC
	/*
	 * Enable external oscillator and wait for it to become ready
	 */
	LPC178X_SCC->scs |=
#if CONFIG_LPC178X_EXTOSC_RATE > 15000000
		(1 << LPC178X_SCC_SCS_OSCRANGE_BIT) |
#endif
		(1 << LPC178X_SCC_SCS_OSCEN_BIT);
	while (!(LPC178X_SCC->scs & (1 << LPC178X_SCC_SCS_OSCSTAT_BIT)));

	/*
	 * Switch PLL0 and SYSCLK to the external oscillator.
	 * Only one bit used in CLKSRCSEL register, therefore not using |=
	 */
	LPC178X_SCC->clksrcsel = (1 << LPC178X_SCC_CLKSRCSEL_CLKSRC_BIT);
#endif /* !CONFIG_LPC178X_SYS_CLK_IRC */

#ifdef CONFIG_LPC178X_PLL0_ENABLE
	/*
	 * PLL0 setup.
	 *
	 * Only MSEL and PSEL bit groups used in PLL0CFG,
	 * therefore not using |=
	 */
	LPC178X_SCC->pll0.cfg =
		((CONFIG_LPC178X_PLL0_M - 1) << LPC178X_SCC_PLLCFG_MSEL_BITS) |
		(CONFIG_LPC178X_PLL0_PSEL << LPC178X_SCC_PLLCFG_PSEL_BITS);
	pll_latch(&LPC178X_SCC->pll0);

	/* Wait for lock */
	while (!(LPC178X_SCC->pll0.stat & (1 << LPC178X_SCC_PLLSTAT_PLOCK_BIT)));
#endif /* CONFIG_LPC178X_PLL0_ENABLE */

	/* TBD: enable PLL1 (necessary only for USB?) */

	/*
	 * CPU clock
	 */
	LPC178X_SCC->cclksel =
#ifdef CONFIG_LPC178X_PLL0_FOR_CPU
		(1 << LPC178X_SCC_CCLKSEL_CCLKSEL_BIT) |
#endif
		(CONFIG_LPC178X_CPU_DIV << LPC178X_SCC_CCLKSEL_CCLKDIV_BITS);
}

/*
 * Initialize the reference clocks.
 */
void clock_init(void)
{
	clock_setup();

	/*
	 * Set SYSTICK.
	 * We use CPU clock (not the external STCLK pin) as the clock source
	 * for the system tick timer, therefore SYSTICK clock is equal to the
	 * CPU clock.
	 */
	clock_val[CLOCK_SYSTICK] = LPC178X_CPU_RATE;
}

/*
 * Return a clock value for the specified clock.
 * Note that we need this function in RAM because it will be used
 * during self-upgrade of U-boot into eNMV.
 * @param clck          id of the clock
 * @returns             frequency of the clock
 */
unsigned long  __attribute__((section(".ramcode")))
	       __attribute__ ((long_call))
	       clock_get(enum clock clck)
{
	return clock_val[clck];
}
