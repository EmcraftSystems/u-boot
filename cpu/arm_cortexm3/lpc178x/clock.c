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
 * be set to configure the PLL0 rate based on the selected input clock.
 * See the LCP178x/7x User's Manual for information on setting these
 * values correctly. SYSCLK is used as the PLL0 input clock.
 */
#define CONFIG_LPC178X_PLL0_ENABLE

/*
 * Set CONFIG_LPC178X_PLL1_ENABLE to enable PLL1. If PLL1 is enabled,
 * the values of CONFIG_LPC178X_PLL1_M and CONFIG_LPC178X_PLL1_PSEL must also
 * be set to configure the PLL1 rate based on the selected input clock.
 * See the LCP178x/7x User's Manual for information on setting these
 * values correctly. SYSCLK is used as the PLL1 input clock.
 *
 * We only enable the PLL1 if the USB clock configuration is enabled, because
 * only the USB clock uses the PLL1 output.
 */
#ifdef CONFIG_LPC178X_USB_DIV
#define CONFIG_LPC178X_PLL1_ENABLE
#endif /* CONFIG_LPC178X_USB_DIV */

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
#define LPC178X_SCC_SCS_OSCRANGE_MSK		(1 << 4)
#define LPC178X_SCC_SCS_OSCEN_MSK		(1 << 5)
#define LPC178X_SCC_SCS_OSCSTAT_MSK		(1 << 6)

/*
 * Clock Source Selection register bits
 */
#define LPC178X_SCC_CLKSRCSEL_CLKSRC_MSK	(1 << 0)

/*
 * PLL Configuration register bits
 */
#define LPC178X_SCC_PLLCFG_MSEL_BITS		0
#define LPC178X_SCC_PLLCFG_MSEL_MSK		((1 << 5) - 1)	/* bits 4:0 */
#define LPC178X_SCC_PLLCFG_PSEL_BITS		5

/*
 * PLL Status register bits
 */
#define LPC178X_SCC_PLLSTAT_PLOCK_MSK		(1 << 10)

/*
 * CPU Clock Selection register bits
 */
#define LPC178X_SCC_CCLKSEL_CCLKDIV_BITS	0
#define LPC178X_SCC_CCLKSEL_CCLKDIV_MSK		((1 << 5) - 1)	/* bits 4:0 */
#define LPC178X_SCC_CCLKSEL_CCLKSEL_MSK		(1 << 8)

/*
 * Peripheral Clock Selection register
 */
#define LPC178X_SCC_PCLKSEL_PCLKDIV_BITS	0
#define LPC178X_SCC_PCLKSEL_PCLKDIV_MSK		((1 << 5) - 1)	/* bits 4:0 */

/*
 * EMC Clock Selection register
 */
#define LPC178X_SCC_EMCCLKSEL_HALFCPU_MSK	(1 << 0)

/*
 * USB Clock Selection register
 */
/* Selects the divide value for creating the USB clock */
#define LPC178X_SCC_USBCLKSEL_USBDIV_BITS	0
/* The mask for all bits of USBCLKSEL[USBDIV] */
#define LPC178X_SCC_USBCLKSEL_USBDIV_MSK \
	(((1 << 5) - 1) << LPC178X_SCC_USBCLKSEL_USBDIV_BITS)
/* Selects the input clock for the USB clock divider */
#define LPC178X_SCC_USBCLKSEL_USBSEL_BITS	8
/* The mask for all bits of USBCLKSEL[USBSEL] */
#define LPC178X_SCC_USBCLKSEL_USBSEL_MSK \
	(3 << LPC178X_SCC_USBCLKSEL_USBSEL_BITS)
/* The output of the Alt PLL is used as the input to the USB clock divider */
#define LPC178X_SCC_USBCLKSEL_USBSEL_PLL1_MSK \
	(2 << LPC178X_SCC_USBCLKSEL_USBSEL_BITS)

/*
 * Calculate clock rates
 */
/*
 * Set LPC178X_PLL0_P to the PLL divider value.
 * The same for PLL1.
 *
 * P (PPL divider value) = 2 in the power of PSEL
 * P can be 1, 2, 4 or 8.
 */
#define LPC178X_PLL0_P		(1 << CONFIG_LPC178X_PLL0_PSEL)
#define LPC178X_PLL1_P		(1 << CONFIG_LPC178X_PLL1_PSEL)

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
 * Set LPC178X_PLL1_CLK_OUT to the output rate of PLL1.
 */
#ifdef CONFIG_LPC178X_PLL1_ENABLE
#define LPC178X_PLL1_CLK_OUT	(LPC178X_OSC_RATE * CONFIG_LPC178X_PLL1_M)
#else
#define LPC178X_PLL1_CLK_OUT	0
#endif

/*
 * Clock at the output of CPU clock selector
 *
 * This is also the input clock for:
 *   1. CPU clock divider
 *   2. Peripheral clock divider
 *   3. EMC clock divider
 */
#ifdef CONFIG_LPC178X_PLL0_FOR_CPU
#define LPC178X_CPU_CLK_SEL_OUT		LPC178X_PLL0_CLK_OUT
#else
#define LPC178X_CPU_CLK_SEL_OUT		LPC178X_OSC_RATE
#endif

/*
 * Set LPC178X_CPU_RATE to the CPU clock.
 */
#define LPC178X_CPU_RATE	(LPC178X_CPU_CLK_SEL_OUT / CONFIG_LPC178X_CPU_DIV)

/*
 * Set LPC178X_PCLK_RATE to the peripheral clock.
 */
#define LPC178X_PCLK_RATE	(LPC178X_CPU_CLK_SEL_OUT / CONFIG_LPC178X_PCLK_DIV)

/*
 * Set LPC178X_EMC_RATE to the EMC clock
 */
#ifdef CONFIG_LPC178X_EMC_HALFCPU
#define LPC178X_EMC_RATE	(LPC178X_CPU_CLK_SEL_OUT / 2)
#else
#define LPC178X_EMC_RATE	(LPC178X_CPU_CLK_SEL_OUT)
#endif

/*
 * Compile time sanity checks for defined board clock setup
 */
#ifndef CONFIG_LPC178X_EXTOSC_RATE
#error CONFIG_LPC178X_EXTOSC_RATE is not set, set to the external osc rate
#endif

/*
 * Verify CPU divider
 */
#ifndef CONFIG_LPC178X_CPU_DIV
#error CONFIG_LPC178X_CPU_DIV is not set, must be between 1 and 31
#endif
#if CONFIG_LPC178X_CPU_DIV < 1
#error CONFIG_LPC178X_CPU_DIV is too low, 1 is the minimum
#endif
#if CONFIG_LPC178X_CPU_DIV > 31
#error CONFIG_LPC178X_CPU_DIV is too high, 31 is the maximum
#endif

/*
 * Verify peripheral divider
 */
#ifndef CONFIG_LPC178X_PCLK_DIV
#error CONFIG_LPC178X_PCLK_DIV is not set, must be between 1 and 31
#endif
#if CONFIG_LPC178X_PCLK_DIV < 1
#error CONFIG_LPC178X_PCLK_DIV is too low, 1 is the minimum
#endif
#if CONFIG_LPC178X_PCLK_DIV > 31
#error CONFIG_LPC178X_PCLK_DIV is too high, 31 is the maximum
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
 * Verify that PLL1 FCCO fits the range of 156 MHz to 320 MHz
 */
#ifdef CONFIG_LPC178X_PLL1_ENABLE
#if (LPC178X_PLL1_CLK_OUT * 2 * LPC178X_PLL1_P) < 156000000
#error PLL1 FCCO rate is too low
#endif
#if (LPC178X_PLL1_CLK_OUT * 2 * LPC178X_PLL1_P) > 320000000
#error PLL1 FCCO rate is too high
#endif
#endif /* CONFIG_LPC178X_PLL1_ENABLE */

/*
 * Feed sequence values for pll_latch()
 */
#define LPC178X_PLL_FEED_KEY1	0xAA
#define LPC178X_PLL_FEED_KEY2	0x55

/*
 * Verify that the Ethernet clock divider value results in a
 * PHY REF_CLK that does not exceed 2.5 MHz.
 */
#ifdef CONFIG_LPC178X_ETH
#if !defined(CONFIG_LPC178X_ETH_DIV_SEL)
#error CONFIG_LPC178X_ETH_DIV_SEL is not set
#elif CONFIG_LPC178X_ETH_DIV_SEL == 0	/* HCLK/4 */
	#if LPC178X_EMC_RATE > 10 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
/* CONFIG_LPC178X_ETH_DIV_SEL=1 leads to the same HCLK/4 */
#elif CONFIG_LPC178X_ETH_DIV_SEL == 2	/* HCLK/6 */
	#if LPC178X_EMC_RATE > 15 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
	#if LPC178X_EMC_RATE <= 10 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too large
	#endif
#elif CONFIG_LPC178X_ETH_DIV_SEL == 3	/* HCLK/8 */
	#if LPC178X_EMC_RATE > 20 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
	#if LPC178X_EMC_RATE <= 15 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too large
	#endif
#elif CONFIG_LPC178X_ETH_DIV_SEL == 4	/* HCLK/10 */
	#if LPC178X_EMC_RATE > 25 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
	#if LPC178X_EMC_RATE <= 20 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too large
	#endif
#elif CONFIG_LPC178X_ETH_DIV_SEL == 5	/* HCLK/14 */
	#if LPC178X_EMC_RATE > 35 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
	#if LPC178X_EMC_RATE <= 25 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too large
	#endif
#elif CONFIG_LPC178X_ETH_DIV_SEL == 6	/* HCLK/20 */
	#if LPC178X_EMC_RATE > 50 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
	#if LPC178X_EMC_RATE <= 35 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too large
	#endif
#elif CONFIG_LPC178X_ETH_DIV_SEL == 7	/* HCLK/28 */
	#if LPC178X_EMC_RATE > 70 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
	#if LPC178X_EMC_RATE <= 50 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too large
	#endif
#elif CONFIG_LPC178X_ETH_DIV_SEL == 8	/* HCLK/36 */
	#if LPC178X_EMC_RATE > 80 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
	#if LPC178X_EMC_RATE <= 70 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too large
	#endif
#elif CONFIG_LPC178X_ETH_DIV_SEL == 9	/* HCLK/40 */
	#if LPC178X_EMC_RATE > 90 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
	#if LPC178X_EMC_RATE <= 80 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too large
	#endif
#elif CONFIG_LPC178X_ETH_DIV_SEL == 10	/* HCLK/44 */
	#if LPC178X_EMC_RATE > 100 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
	#if LPC178X_EMC_RATE <= 90 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too large
	#endif
#elif CONFIG_LPC178X_ETH_DIV_SEL == 11	/* HCLK/48 */
	#if LPC178X_EMC_RATE > 120 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too small
	#endif
	#if LPC178X_EMC_RATE <= 100 * 1000 * 1000
	#error CONFIG_LPC178X_ETH_DIV_SEL is too large
	#endif
#else
#error Unknown value of CONFIG_LPC178X_ETH_DIV_SEL
#endif /* CONFIG_LPC178X_ETH_DIV_SEL */
#endif /* CONFIG_LPC178X_ETH */

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
 * PLLn setup
 */
#define PLL_CONFIG(idx) do { \
	LPC178X_SCC->pll##idx.cfg =			\
		((CONFIG_LPC178X_PLL## idx ##_M - 1) <<	\
		LPC178X_SCC_PLLCFG_MSEL_BITS) |		\
		(CONFIG_LPC178X_PLL## idx ##_PSEL <<	\
		LPC178X_SCC_PLLCFG_PSEL_BITS);		\
	pll_latch(&LPC178X_SCC->pll##idx);		\
	/* Wait for lock */				\
	while (!(LPC178X_SCC->pll##idx.stat &		\
		LPC178X_SCC_PLLSTAT_PLOCK_MSK));	\
} while (0)

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
		LPC178X_SCC_SCS_OSCRANGE_MSK |
#endif
		LPC178X_SCC_SCS_OSCEN_MSK;
	while (!(LPC178X_SCC->scs & LPC178X_SCC_SCS_OSCSTAT_MSK));

	/*
	 * Switch PLL0 and SYSCLK to the external oscillator.
	 * Only one bit used in CLKSRCSEL register, therefore not using |=
	 */
	LPC178X_SCC->clksrcsel = LPC178X_SCC_CLKSRCSEL_CLKSRC_MSK;
#endif /* !CONFIG_LPC178X_SYS_CLK_IRC */

#ifdef CONFIG_LPC178X_PLL0_ENABLE
	PLL_CONFIG(0);
#endif /* CONFIG_LPC178X_PLL0_ENABLE */

#ifdef CONFIG_LPC178X_PLL1_ENABLE
	/*
	 * PLL1 is necessary for USB
	 */
	PLL_CONFIG(1);
#endif /* CONFIG_LPC178X_PLL1_ENABLE */

	/*
	 * CPU clock
	 */
	LPC178X_SCC->cclksel =
#ifdef CONFIG_LPC178X_PLL0_FOR_CPU
		LPC178X_SCC_CCLKSEL_CCLKSEL_MSK |
#endif
		(CONFIG_LPC178X_CPU_DIV << LPC178X_SCC_CCLKSEL_CCLKDIV_BITS);

	/*
	 * Peripheral clock (used for UARTs, etc)
	 *
	 * Only PCLKDIV bit group used in PCLKSEL, therefore not using |=
	 */
	LPC178X_SCC->pclksel =
		(CONFIG_LPC178X_PCLK_DIV << LPC178X_SCC_PCLKSEL_PCLKDIV_BITS);

#ifdef CONFIG_NR_DRAM_BANKS
	/*
	 * EMC clock
	 */
#ifdef CONFIG_LPC178X_EMC_HALFCPU
	LPC178X_SCC->emcclksel = LPC178X_SCC_EMCCLKSEL_HALFCPU_MSK;
#else /* CONFIG_LPC178X_EMC_HALFCPU */
	LPC178X_SCC->emcclksel = 0;
#endif /* CONFIG_LPC178X_EMC_HALFCPU */
#endif /* CONFIG_NR_DRAM_BANKS */

#ifdef CONFIG_LPC178X_USB_DIV
	/*
	 * USB clock
	 */
	LPC178X_SCC->usbclksel =
		(CONFIG_LPC178X_USB_DIV << LPC178X_SCC_USBCLKSEL_USBDIV_BITS) |
		LPC178X_SCC_USBCLKSEL_USBSEL_PLL1_MSK;
#endif /* CONFIG_LPC178X_USB_DIV */
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

	/*
	 * Set peripheral clock rate
	 */
	clock_val[CLOCK_PCLK] = LPC178X_PCLK_RATE;

	/*
	 * Set EMC clock rate
	 */
	clock_val[CLOCK_EMCCLK] = LPC178X_EMC_RATE;
}

/*
 * Return a clock value for the specified clock.
 *
 * Note that we do not need this function in RAM (.ramcode) because it will
 * be used during self-upgrade of U-boot into eNMV only once, before the data
 * in the eNMV will be actually changed.
 *
 * @param clck          id of the clock
 * @returns             frequency of the clock
 */
unsigned long clock_get(enum clock clck)
{
	return clock_val[clck];
}
