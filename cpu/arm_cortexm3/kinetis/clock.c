/*
 * (C) Copyright 2011, 2012
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

#include "clock.h"

/*
 * Set the limits for the PLL divider and multiplier values for the MCU:
 *     KINETIS_PLL_PRDIV_MAX
 *     KINETIS_PLL_VDIV_MIN
 *     KINETIS_PLL_VDIV_MAX
 *
 * Also set the range for the PLL reference clock (see datasheets for MCUs):
 *     KINETIS_PLL_REF_MIN
 *     KINETIS_PLL_REF_MAX
 *
 * Divider value between the VCO output and the PLL output:
 *     KINETIS_PLL_VCO_DIV
 *
 * Clock frequency limits:
 */
#if defined(CONFIG_KINETIS_K60_100MHZ)
#define KINETIS_PLL_PRDIV_MAX	25
#define KINETIS_PLL_VDIV_MIN	24
#define KINETIS_PLL_VDIV_MAX	55

#define KINETIS_PLL_REF_MIN	(2 * 1000 * 1000)	/* 2 MHz */
#define KINETIS_PLL_REF_MAX	(4 * 1000 * 1000)	/* 4 MHz */

#define KINETIS_PLL_VCO_DIV	1	/* No /2 divider after VCO */

#define KINETIS_CPU_RATE_MAX		(100 * 1000 * 1000)	/* 100 MHz */
#define KINETIS_PCLK_RATE_MAX		(50 * 1000 * 1000)	/* 50 MHz */
#define KINETIS_FLEXBUS_RATE_MAX	(50 * 1000 * 1000)	/* 50 MHz */
#define KINETIS_FLASH_RATE_MAX		(25 * 1000 * 1000)	/* 25 MHz */
#define KINETIS_DDR_RATE_MAX		0	/* DDR not supported */

#undef KINETIS_MCG_OSC_MULTI		/* No support for multiple oscillators */

#ifdef KINETIS_MCGOUT_PLL1
#error There is no PLL1 on the Kinetis K60@100MHz MCUs
#endif

#elif defined(CONFIG_KINETIS_K70_120MHZ)
/* MCUs with maximum core rate 120MHz or 150MHz */
#define KINETIS_PLL_PRDIV_MAX	8
#define KINETIS_PLL_VDIV_MIN	16
#define KINETIS_PLL_VDIV_MAX	47

#define KINETIS_PLL_REF_MIN	(8 * 1000 * 1000)	/* 8 MHz */
#define KINETIS_PLL_REF_MAX	(16 * 1000 * 1000)	/* 16 MHz */

#define KINETIS_PLL_VCO_DIV	2	/* There is a /2 divider after VCO */

#if defined(CONFIG_KINETIS_120MHZ)
/* 120MHz */
#define KINETIS_CPU_RATE_MAX		(120 * 1000 * 1000)	/* 120 MHz */
#define KINETIS_PCLK_RATE_MAX		(60 * 1000 * 1000)	/* 60 MHz */
#define KINETIS_DDR_RATE_MAX		(125 * 1000 * 1000)	/* 125 MHz */
#elif defined(CONFIG_KINETIS_150MHZ)
/* 150MHz */
#define KINETIS_CPU_RATE_MAX		(150 * 1000 * 1000)	/* 150 MHz */
#define KINETIS_PCLK_RATE_MAX		(75 * 1000 * 1000)	/* 75 MHz */
#define KINETIS_DDR_RATE_MAX		(150 * 1000 * 1000)	/* 150 MHz */
#else
#error Unsupported Freescale Kinetis MCU core frequency
#endif

#define KINETIS_FLEXBUS_RATE_MAX	(50 * 1000 * 1000)	/* 50 MHz */
#define KINETIS_FLASH_RATE_MAX		(25 * 1000 * 1000)	/* 25 MHz */

/* PLL0 and PLL1 input: Oscillator on XTAL1/EXTAL1 controller by OSC1 module */
#define KINETIS_MCG_OSC_MULTI		/* MCU has two oscillators */

#else
#error Unsupported Freescale Kinetis MCU series
#endif

/*
 * Clock rate on the input of the MCG (Multipurpose Clock Generator)
 */
#if KINETIS_MCG_PLLREFSEL == 0
#define KINETIS_MCG_PLL_IN_RATE		KINETIS_OSC0_RATE
#else
#define KINETIS_MCG_PLL_IN_RATE		KINETIS_OSC1_RATE
#endif

/*
 * Set KINETIS_MCG_FREQ_RANGE according to the frequency range of the crystal
 * oscillator or external clock source (KINETIS_MCG_PLL_IN_RATE)
 */
#if KINETIS_MCG_PLL_IN_RATE >= 8000000
#define KINETIS_MCG_FREQ_RANGE	2	/* 8 MHz to 32 MHz */
#elif KINETIS_MCG_PLL_IN_RATE >= 1000000
#define KINETIS_MCG_FREQ_RANGE	1	/* 1 MHz to 8 MHz */
#elif (KINETIS_MCG_PLL_IN_RATE >= 32000 && KINETIS_MCG_PLL_IN_RATE <= 40000)
#define KINETIS_MCG_FREQ_RANGE	0	/* 32 kHz to 40 kHz */
#else
#error KINETIS_MCG_PLL_IN_RATE is out of possible ranges
#endif

/*
 * Set KINETIS_MCG_FRDIV to the code for writing into the FRDIV bit field
 * of the MCG_C1 register.
 */
#if KINETIS_MCG_FREQ_RANGE == 0 || defined(KINETIS_FLLREF_RTC)
#if KINETIS_MCG_FRDIV_POW >= 0 && KINETIS_MCG_FRDIV_POW <= 7
#define KINETIS_MCG_FRDIV	KINETIS_MCG_FRDIV_POW
#else
#error KINETIS_MCG_FRDIV_POW should be between 0 and 7
#endif /* KINETIS_MCG_FRDIV_POW in 0 ... 7 */
#else
#if KINETIS_MCG_FRDIV_POW >= 5 && KINETIS_MCG_FRDIV_POW <= 10
#define KINETIS_MCG_FRDIV	(KINETIS_MCG_FRDIV_POW - 5)
#else
#error KINETIS_MCG_FRDIV_POW should be between 5 and 10
#endif /* KINETIS_MCG_FRDIV_POW in 5 ... 10 */
#endif

/*
 * Verify the PRDIV divider value
 */
#if !defined(KINETIS_PLL_PRDIV)
#error KINETIS_PLL_PRDIV is not defined
#elif KINETIS_PLL_PRDIV < 1 || KINETIS_PLL_PRDIV > KINETIS_PLL_PRDIV_MAX
#error KINETIS_PLL_PRDIV should be between 1 and KINETIS_PLL_PRDIV_MAX
#endif

/*
 * Verify the VDIV multiplier value
 */
#if !defined(KINETIS_PLL_VDIV)
#error KINETIS_PLL_VDIV is not defined
#elif KINETIS_PLL_VDIV < KINETIS_PLL_VDIV_MIN || \
      KINETIS_PLL_VDIV > KINETIS_PLL_VDIV_MAX
#error KINETIS_PLL_VDIV should be set to an integer in the allowed range
#endif

/*
 * Verify the PLL reference clock rate
 */
#if ((KINETIS_MCG_PLL_IN_RATE / KINETIS_PLL_PRDIV) < KINETIS_PLL_REF_MIN) || \
    ((KINETIS_MCG_PLL_IN_RATE / KINETIS_PLL_PRDIV) > KINETIS_PLL_REF_MAX)
#error The PLL reference clock should be in the defined range
#endif

/*
 * MCGOUTCLK rate
 */
#ifdef KINETIS_MCGOUT_PLL1
#define KINETIS_MCGOUT_RATE \
	(KINETIS_MCG_PLL_IN_RATE / KINETIS_PLL1_PRDIV * KINETIS_PLL1_VDIV / \
	KINETIS_PLL_VCO_DIV)
#else
#define KINETIS_MCGOUT_RATE \
	(KINETIS_MCG_PLL_IN_RATE / KINETIS_PLL_PRDIV * KINETIS_PLL_VDIV / \
	KINETIS_PLL_VCO_DIV)
#endif /* KINETIS_MCGOUT_PLL1 */

#if !defined(CONFIG_KINETIS_K60_100MHZ)
/*
 * Verify the PRDIV divider value for PLL1
 */
#if !defined(KINETIS_PLL1_PRDIV)
#error KINETIS_PLL1_PRDIV is not defined
#elif KINETIS_PLL1_PRDIV < 1 || KINETIS_PLL1_PRDIV > KINETIS_PLL_PRDIV_MAX
#error KINETIS_PLL1_PRDIV should be between 1 and KINETIS_PLL_PRDIV_MAX
#endif

/*
 * Verify the VDIV multiplier value for PLL1
 */
#if !defined(KINETIS_PLL1_VDIV)
#error KINETIS_PLL1_VDIV is not defined
#elif KINETIS_PLL1_VDIV < KINETIS_PLL_VDIV_MIN || \
      KINETIS_PLL1_VDIV > KINETIS_PLL_VDIV_MAX
#error KINETIS_PLL1_VDIV should be set to an integer in the allowed range
#endif

/*
 * Verify the PLL reference clock rate for PLL1
 */
#if ((KINETIS_MCG_PLL_IN_RATE / KINETIS_PLL1_PRDIV) < KINETIS_PLL_REF_MIN) || \
    ((KINETIS_MCG_PLL_IN_RATE / KINETIS_PLL1_PRDIV) > KINETIS_PLL_REF_MAX)
#error The PLL1 reference clock should be in the defined range
#endif

/*
 * PLL1 output rate
 */
#define KINETIS_PLL1OUT_RATE \
	(KINETIS_MCG_PLL_IN_RATE / KINETIS_PLL1_PRDIV * KINETIS_PLL1_VDIV / \
	KINETIS_PLL_VCO_DIV)
#endif /* !defined(CONFIG_KINETIS_K60_100MHZ) */

/*
 * Core/system clock rate
 */
#define KINETIS_CPU_RATE	(KINETIS_MCGOUT_RATE / KINETIS_CCLK_DIV)
#if KINETIS_CPU_RATE > KINETIS_CPU_RATE_MAX
#error KINETIS_CPU_RATE exceeds the limit for this MCU
#endif

/*
 * Peripheral clock rate
 */
#define KINETIS_PCLK_RATE	(KINETIS_MCGOUT_RATE / KINETIS_PCLK_DIV)
#if KINETIS_PCLK_RATE > KINETIS_PCLK_RATE_MAX
#error KINETIS_PCLK_RATE exceeds the limit for this MCU
#endif

/*
 * FlexBus clock rate
 */
#define KINETIS_FLEXBUS_RATE	(KINETIS_MCGOUT_RATE / KINETIS_FLEXBUS_CLK_DIV)
#if KINETIS_FLEXBUS_RATE > KINETIS_FLEXBUS_RATE_MAX
#error KINETIS_FLEXBUS_RATE exceeds the limit for this MCU
#endif

/*
 * Flash clock rate
 */
#define KINETIS_FLASH_RATE	(KINETIS_MCGOUT_RATE / KINETIS_FLASH_CLK_DIV)
#if KINETIS_FLASH_RATE > KINETIS_FLASH_RATE_MAX
#error KINETIS_FLASH_RATE exceeds the limit for this MCU
#endif

#ifdef CONFIG_KINETIS_DDR
/*
 * DDR clock rate
 */
#define KINETIS_DDR_RATE	KINETIS_PLL1OUT_RATE
#if KINETIS_DDR_RATE > KINETIS_DDR_RATE_MAX
#error KINETIS_DDR_RATE exceeds the limit for this MCU
#endif
#endif /* CONFIG_KINETIS_DDR */

/*
 * Oscillator (OSC) registers
 */
/*
 * OSC_CR register
 */
/* External Reference Enable */
#define KINETIS_OSC_CR_EREFSTEN_MSK	(1 << 5)
/* External Reference Stop Enable */
#define KINETIS_OSC_CR_ERCLKEN_MSK	(1 << 7)

/*
 * MCG registers
 */
/*
 * MCG Control 1 Register
 */
#define KINETIS_MCG_C1_FRDIV_BITS	3
/*
 * Clock Source Select bits (CLKS)
 */
#define KINETIS_MCG_C1_CLKS_BITS	6
#define KINETIS_MCG_C1_CLKS_MSK		(3 << KINETIS_MCG_C1_CLKS_BITS)
#define KINETIS_MCG_C1_CLKS_FLLPLL_MSK	(0 << KINETIS_MCG_C1_CLKS_BITS)
#define KINETIS_MCG_C1_CLKS_INT_REF_MSK	(1 << KINETIS_MCG_C1_CLKS_BITS)
#define KINETIS_MCG_C1_CLKS_EXT_REF_MSK	(2 << KINETIS_MCG_C1_CLKS_BITS)
/*
 * MCG Control 2 Register
 */
/* Frequency Range Select bits */
#define KINETIS_MCG_C2_RANGE0_BITS	4
/*
 * External Reference Select for OSC0
 *    0 = External reference clock requested.
 *    1 = Oscillator requested.
 */
#define KINETIS_MCG_C2_EREFS0_MSK	(1 << 2)
/* Configure crystal oscillator for high-gain operation */
#define KINETIS_MCG_C2_HGO_MSK		(1 << 3)
/*
 * MCG Control 5 Register
 */
/* PLL External Reference Divider */
#define KINETIS_MCG_C5_PRDIV_BITS	0
/* PLL Stop Enable */
#define KINETIS_MCG_C5_PLLSTEN_MSK	(1 << 5)
/* PLL0 Clock Enable */
#define KINETIS_MCG_C5_PLLCLKEN_MSK	(1 << 6)
/* PLL External Reference Select (for K70@120MHz) */
#define KINETIS_MCG_C5_PLLREFSEL_BIT	7
#define KINETIS_MCG_C5_PLLREFSEL_MSK	(1 << KINETIS_MCG_C5_PLLREFSEL_BIT)
/*
 * MCG Control 6 Register
 */
/* VCO Divider */
#define KINETIS_MCG_C6_VDIV_BITS	0
/* PLL Select */
#define KINETIS_MCG_C6_PLLS_MSK		(1 << 6)
/*
 * MCG Control 7 Register
 */
/* MCG OSC Clock Select */
#define KINETIS_MCG_C7_OSCSEL_MSK	(1 << 0)
/*
 * MCG Control 10 Register
 */
/* Frequency Range1 Select */
#define KINETIS_MCG_C10_RANGE1_BITS	4
/*
 * External Reference Select for OSC1
 *    0 = External reference clock requested.
 *    1 = Oscillator requested.
 */
#define KINETIS_MCG_C10_EREFS1_MSK	(1 << 2)
/*
 * MCG Control 11 Register
 */
/* PLL1 External Reference Divider */
#define KINETIS_MCG_C11_PRDIV_BITS	0
/* PLL Clock Select: PLL0 or PLL1 */
#define KINETIS_MCG_C11_PLLCS_MSK	(1 << 4)
/* PLL1 Stop Enable */
#define KINETIS_MCG_C11_PLLSTEN1_MSK	(1 << 5)
/* PLL1 Clock Enable */
#define KINETIS_MCG_C11_PLLCLKEN1_MSK	(1 << 6)
/* PLL1 External Reference Select (for K70@120MHz) */
#define KINETIS_MCG_C11_PLLREFSEL1_BIT	7
#define KINETIS_MCG_C11_PLLREFSEL1_MSK	(1 << KINETIS_MCG_C11_PLLREFSEL1_BIT)
/*
 * MCG Control 12 Register
 */
/* VCO1 Divider */
#define KINETIS_MCG_C12_VDIV1_BITS	0
/*
 * MCG Status Register
 */
/* Oscillator 0 initialization */
#define KINETIS_MCG_S_OSCINIT0_MSK	(1 << 1)
/* Clock Mode Status */
#define KINETIS_MCG_S_CLKST_BITS	2
#define KINETIS_MCG_S_CLKST_MSK		(3 << KINETIS_MCG_S_CLKST_BITS)
#define KINETIS_MCG_S_CLKST_FLL		(0 << KINETIS_MCG_S_CLKST_BITS)
#define KINETIS_MCG_S_CLKST_INT_REF	(1 << KINETIS_MCG_S_CLKST_BITS)
#define KINETIS_MCG_S_CLKST_EXT_REF	(2 << KINETIS_MCG_S_CLKST_BITS)
#define KINETIS_MCG_S_CLKST_PLL		(3 << KINETIS_MCG_S_CLKST_BITS)
/* Internal Reference Status */
#define KINETIS_MCG_S_IREFST_MSK	(1 << 4)
/* PLL Select Status */
#define KINETIS_MCG_S_PLLST_MSK		(1 << 5)
/* Indicates whether the PLL has acquired lock */
#define KINETIS_MCG_S_LOCK_MSK		(1 << 6)
/*
 * MCG Status 2 Register
 */
/* This bit indicates whether PLL1 has acquired lock */
#define KINETIS_MCG_S2_LOCK1_MSK	(1 << 6)
/* Oscillator 1 initialization */
#define KINETIS_MCG_S2_OSCINIT1_MSK	(1 << 1)

/*
 * SIM registers
 */
/*
 * System Options Register 2
 */
/*
 * Selects the clock divider source for NFC flash clock
 */
#define KINETIS_SIM_SOPT2_NFCSRC_BITS	30
#define KINETIS_SIM_SOPT2_NFCSRC_MSK	(3 << KINETIS_SIM_SOPT2_NFCSRC_BITS)
/* MCGPLL0CLK */
#define KINETIS_SIM_SOPT2_NFCSRC_PLL0	(1 << KINETIS_SIM_SOPT2_NFCSRC_BITS)
/* MCGPLL1CLK */
#define KINETIS_SIM_SOPT2_NFCSRC_PLL1	(2 << KINETIS_SIM_SOPT2_NFCSRC_BITS)
/*
 * NFC Flash clock select
 */
#define KINETIS_SIM_SOPT2_NFC_CLKSEL_MSK	(1 << 15)
/* Clock divider NFC clock */
#define KINETIS_SIM_SOPT2_NFC_CLKSEL_NFCDIV	0
/* EXTAL1 clock */
#define KINETIS_SIM_SOPT2_NFC_CLKSEL_EXTAL1	KINETIS_SIM_SOPT2_NFC_CLKSEL_MSK
/*
 * LCD Controller source clock
 */
#define KINETIS_SIM_SOPT2_LCDCSRC_BITS	26
#define KINETIS_SIM_SOPT2_LCDCSRC_MSK	(3 << KINETIS_SIM_SOPT2_LCDCSRC_BITS)
/* MCGPLL0CLK */
#define KINETIS_SIM_SOPT2_LCDCSRC_PLL0	(1 << KINETIS_SIM_SOPT2_LCDCSRC_BITS)
/* MCGPLL1CLK */
#define KINETIS_SIM_SOPT2_LCDCSRC_PLL1	(2 << KINETIS_SIM_SOPT2_LCDCSRC_BITS)
/*
 * LCD Controller clock select
 */
#define KINETIS_SIM_SOPT2_LCDC_CLKSEL_MSK	(1 << 14)
/* Clock divider LCDC pixel clock */
#define KINETIS_SIM_SOPT2_LCDC_CLKSEL_LCDCDIV	0
/* EXTAL1 clock */
#define KINETIS_SIM_SOPT2_LCDC_CLKSEL_EXTAL1 \
	KINETIS_SIM_SOPT2_LCDC_CLKSEL_MSK

/*
 * System Clock Divider Register 1
 */
/* Clock 1 output divider value (for the core/system clock) */
#define KINETIS_SIM_CLKDIV1_OUTDIV1_BITS	28
/* Clock 2 output divider value (for the peripheral clock) */
#define KINETIS_SIM_CLKDIV1_OUTDIV2_BITS	24
/* Clock 3 output divider value (for the FlexBus clock) */
#define KINETIS_SIM_CLKDIV1_OUTDIV3_BITS	20
/* Clock 4 output divider value (for the flash clock) */
#define KINETIS_SIM_CLKDIV1_OUTDIV4_BITS	16

/*
 * System Clock Divider Register 3
 */
/* LCD Controller clock divider divisor */
#define KINETIS_SIM_CLKDIV3_LCDCDIV_BITS	16
#define KINETIS_SIM_CLKDIV3_LCDCDIV_BITWIDTH	12
#define KINETIS_SIM_CLKDIV3_LCDCDIV_MSK \
	(((1 << KINETIS_SIM_CLKDIV3_LCDCDIV_BITWIDTH) - 1) << \
	KINETIS_SIM_CLKDIV3_LCDCDIV_BITS)
/* LCD Controller clock divider fraction */
#define KINETIS_SIM_CLKDIV3_LCDCFRAC_BITS	8
#define KINETIS_SIM_CLKDIV3_LCDCFRAC_BITWIDTH	8
#define KINETIS_SIM_CLKDIV3_LCDCFRAC_MSK \
	(((1 << KINETIS_SIM_CLKDIV3_LCDCFRAC_BITWIDTH) - 1) << \
	KINETIS_SIM_CLKDIV3_LCDCFRAC_BITS)

/*
 * System Clock Divider Register 4
 */
/* NFC clock divider divisor */
#define KINETIS_SIM_CLKDIV4_NFCDIV_BITS		27
#define KINETIS_SIM_CLKDIV4_NFCDIV_BITWIDTH	5
#define KINETIS_SIM_CLKDIV4_NFCDIV_MSK \
	(((1 << KINETIS_SIM_CLKDIV4_NFCDIV_BITWIDTH) - 1) << \
	KINETIS_SIM_CLKDIV4_NFCDIV_BITS)
/* NFC clock divider fraction */
#define KINETIS_SIM_CLKDIV4_NFCFRAC_BITS	24
#define KINETIS_SIM_CLKDIV4_NFCFRAC_BITWIDTH	3
#define KINETIS_SIM_CLKDIV4_NFCFRAC_MSK \
	(((1 << KINETIS_SIM_CLKDIV4_NFCFRAC_BITWIDTH) - 1) << \
	KINETIS_SIM_CLKDIV4_NFCFRAC_BITS)

/*
 * Set configuration options based on usage of PLL0 or PLL1 for the CPU clock
 */
#ifdef KINETIS_MCGOUT_PLL1
#define KINETIS_SIM_SOPT2_NFCSRC	KINETIS_SIM_SOPT2_NFCSRC_PLL1
#define KINETIS_SIM_SOPT2_LCDCSRC	KINETIS_SIM_SOPT2_LCDCSRC_PLL1
#else
#define KINETIS_SIM_SOPT2_NFCSRC	KINETIS_SIM_SOPT2_NFCSRC_PLL0
#define KINETIS_SIM_SOPT2_LCDCSRC	KINETIS_SIM_SOPT2_LCDCSRC_PLL0
#endif /* KINETIS_MCGOUT_PLL1 */

/*
 * Check the requirements for the synchronous mode of the DDR controller
 */
#ifdef CONFIG_KINETIS_DDR_SYNC
#ifndef KINETIS_MCGOUT_PLL1
#error DDR synchronous mode requires that the system clock is sourced from PLL1
#endif /* !KINETIS_MCGOUT_PLL1 */
#ifndef CONFIG_KINETIS_DDR
#error CONFIG_KINETIS_DDR_SYNC requires CONFIG_KINETIS_DDR
#endif /* !CONFIG_KINETIS_DDR */
#endif /* CONFIG_KINETIS_DDR_SYNC */

/*
 * RTC Control Register
 */
/* Oscillator 4pF load configure */
#define KINETIS_RTC_CR_SC4P_MSK		(1 << 12)
/* Oscillator 16pF load configure */
#define KINETIS_RTC_CR_SC16P_MSK	(1 << 10)
/* Oscillator Enable */
#define KINETIS_RTC_CR_OSCE_MSK		(1 << 8)

/*
 * Real Time Clock (RTC) register map
 */
struct kinetis_rtc_regs {
	u32 tsr;	/* RTC Time Seconds Register */
	u32 tpr;	/* RTC Time Prescaler Register */
	u32 tar;	/* RTC Time Alarm Register */
	u32 tcr;	/* RTC Time Compensation Register */
	u32 cr;		/* RTC Control Register */
	u32 sr;		/* RTC Status Register */
	u32 lr;		/* RTC Lock Register */
	u32 ier;	/* RTC Interrupt Enable Register */
	u32 ttsr;	/* RTC Tamper Time Seconds Register */
	u32 mer;	/* RTC Monotonic Enable Register */
	u32 mclr;	/* RTC Monotonic Counter Low Register */
	u32 mchr;	/* RTC Monotonic Counter High Register */
	u32 ter;	/* RTC Tamper Enable Register */
	u32 tdr;	/* RTC Tamper Detect Register */
	u32 ttr;	/* RTC Tamper Trim Register */
	u32 tir;	/* RTC Tamper Interrupt Register */
	u32 rsv0[496];
	u32 war;	/* RTC Write Access Register */
	u32 rar;	/* RTC Read Access Register */
};

/*
 * RTC registers base
 */
#define KINETIS_RTC_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x0003D000)
#define KINETIS_RTC			((volatile struct kinetis_rtc_regs *) \
					KINETIS_RTC_BASE)

/*
 * Clock values
 */
static u32 clock_val[CLOCK_END];

/*
 * Transition from the FEI (FLL Engaged Internal) to
 * the FBE (FLL Bypassed External) mode.
 */
static void clock_fei_to_fbe(void)
{
	/*
	 * Select frequency range on the input of MCG
	 */
#if KINETIS_MCG_PLLREFSEL == 0
	KINETIS_MCG->c2 = KINETIS_MCG_FREQ_RANGE << KINETIS_MCG_C2_RANGE0_BITS;
#else
	KINETIS_MCG->c10 = KINETIS_MCG_FREQ_RANGE << KINETIS_MCG_C10_RANGE1_BITS;
#endif /* KINETIS_MCG_PLLREFSEL */

	/*
	 * Select if an oscillator is connected to the main input of MCG
	 */
#ifdef KINETIS_MCG_EXT_CRYSTAL
#if KINETIS_MCG_PLLREFSEL == 0
	KINETIS_MCG->c2 |= KINETIS_MCG_C2_EREFS0_MSK;
#else
	KINETIS_MCG->c10 |= KINETIS_MCG_C10_EREFS1_MSK;
#endif /* KINETIS_MCG_PLLREFSEL */
#endif /* KINETIS_MCG_EXT_CRYSTAL */

	/*
	 * Use RTC clock for FLL reference clock, if needed.
	 */
#ifdef KINETIS_FLLREF_RTC
	/*
	 * Enable the RTC module of the MCU
	 */
	kinetis_periph_enable(KINETIS_CG_RTC, 1);

	/*
	 * Enable RTC
	 *
	 * RTC oscillator load capacity should be 20pF (=16+4) for K70-SOM.
	 */
	KINETIS_RTC->cr |=
		KINETIS_RTC_CR_SC16P_MSK | KINETIS_RTC_CR_SC4P_MSK |
		KINETIS_RTC_CR_OSCE_MSK;

	/*
	 * Select 32 kHz RTC as the FLL external reference clock
	 */
	KINETIS_MCG->c7 |= KINETIS_MCG_C7_OSCSEL_MSK;
#endif /* KINETIS_FLLREF_RTC */

	/*
	 * Set the FLL External Reference Divider.
	 * Select the clock source for MCGOUTCLK.
	 */
	KINETIS_MCG->c1 =
		(KINETIS_MCG_FRDIV << KINETIS_MCG_C1_FRDIV_BITS) |
		KINETIS_MCG_C1_CLKS_EXT_REF_MSK;

#ifdef KINETIS_MCG_EXT_CRYSTAL
	/*
	 * Wait for the input from the external crystal to initialize
	 */
#if KINETIS_MCG_PLLREFSEL == 0
	while (!(KINETIS_MCG->status & KINETIS_MCG_S_OSCINIT0_MSK));
#else
	while (!(KINETIS_MCG->status2 & KINETIS_MCG_S2_OSCINIT1_MSK));
#endif /* KINETIS_MCG_PLLREFSEL */
#endif /* KINETIS_MCG_EXT_CRYSTAL */

	/*
	 * Wait for reference clock to switch to external reference
	 */
	while (KINETIS_MCG->status & KINETIS_MCG_S_IREFST_MSK);

	/*
	 * Wait for the external reference clock to start feeding MCGOUT
	 */
	while ((KINETIS_MCG->status & KINETIS_MCG_S_CLKST_MSK) !=
		KINETIS_MCG_S_CLKST_EXT_REF);
}

#if defined(KINETIS_MCGOUT_PLL0) || !defined(KINETIS_MCGOUT_PLL1)
static void clock_setup_pll0(void)
{
	/*
	 * Configure the PLL0 input divider.
	 * Also, enable the PLL0 clock during Normal Stop.
	 */
	KINETIS_MCG->c5 =
		((KINETIS_PLL_PRDIV - 1) << KINETIS_MCG_C5_PRDIV_BITS) |
		KINETIS_MCG_C5_PLLSTEN_MSK;

	/*
	 * If the Multipurpose Clock Generator (MCG) supports multiple
	 * oscillators (e.g. on K70 @ 120 MHz), select the necessary
	 * oscillator as the external reference clock for the PLL0.
	 */
#ifdef KINETIS_MCG_OSC_MULTI
	KINETIS_MCG->c5 =
		(KINETIS_MCG->c5 & ~KINETIS_MCG_C5_PLLREFSEL_MSK) |
		(KINETIS_MCG_PLLREFSEL << KINETIS_MCG_C5_PLLREFSEL_BIT);
#endif

	/*
	 * Set the PLL multiplication factor
	 */
	KINETIS_MCG->c6 = (KINETIS_PLL_VDIV - KINETIS_PLL_VDIV_MIN) <<
		KINETIS_MCG_C6_VDIV_BITS;

	/*
	 * Enable the PLL0
	 */
	KINETIS_MCG->c5 |= KINETIS_MCG_C5_PLLCLKEN_MSK;

	/*
	 * Wait for the PLL0 to acquire lock
	 */
	while (!(KINETIS_MCG->status & KINETIS_MCG_S_LOCK_MSK));
}
#endif /* KINETIS_MCGOUT_PLL0 */

#if defined(KINETIS_MCGOUT_PLL1) || \
    (defined(CONFIG_KINETIS_DDR) && !defined(CONFIG_KINETIS_DDR_SYNC))
static void clock_setup_pll1(void)
{
	/*
	 * Configure the PLL1 input divider.
	 * Also, enable the PLL1 clock during Normal Stop.
	 */
	KINETIS_MCG->c11 =
		((KINETIS_PLL1_PRDIV - 1) << KINETIS_MCG_C11_PRDIV_BITS) |
		KINETIS_MCG_C11_PLLSTEN1_MSK;

	/*
	 * If the Multipurpose Clock Generator (MCG) supports multiple
	 * oscillators (e.g. on K70 @ 120 MHz), select the necessary
	 * oscillator as the external reference clock for the PLL1.
	 */
#ifdef KINETIS_MCG_OSC_MULTI
	KINETIS_MCG->c11 =
		(KINETIS_MCG->c11 & ~KINETIS_MCG_C11_PLLREFSEL1_MSK) |
		(KINETIS_MCG_PLLREFSEL << KINETIS_MCG_C11_PLLREFSEL1_BIT);
#endif

	/*
	 * Set the PLL1 multiplication factor
	 */
	KINETIS_MCG->c12 = (KINETIS_PLL1_VDIV - KINETIS_PLL_VDIV_MIN) <<
		KINETIS_MCG_C12_VDIV1_BITS;

	/*
	 * Enable the PLL1
	 */
	KINETIS_MCG->c11 |= KINETIS_MCG_C11_PLLCLKEN1_MSK;

	/*
	 * Wait for the PLL1 to acquire lock
	 */
	while (!(KINETIS_MCG->status2 & KINETIS_MCG_S2_LOCK1_MSK));
}
#endif /* KINETIS_MCGOUT_PLL1 || (KINETIS_DDR && !KINETIS_DDR_SYNC) */

/*
 * Transition from the FBE (FLL Bypassed External) to
 * the PBE (PLL Bypassed External) mode.
 */
static void clock_fbe_to_pbe(void)
{
	/*
	 * Configure the PLL that we will use for the MCGOUTCLK
	 */
#if defined(KINETIS_MCGOUT_PLL1)
	clock_setup_pll1();
#endif

#if defined(KINETIS_MCGOUT_PLL0)
	clock_setup_pll0();
#endif

#if defined(KINETIS_MCGOUT_PLL1)
	/* Select PLL1 output as the MCG source clock */
	KINETIS_MCG->c11 |= KINETIS_MCG_C11_PLLCS_MSK;
#else
	/* Select PLL0 output as the MCG source clock */
	KINETIS_MCG->c11 &= ~KINETIS_MCG_C11_PLLCS_MSK;
#endif

	/*
	 * Switch to the PBE mode
	 */
	KINETIS_MCG->c6 |= KINETIS_MCG_C6_PLLS_MSK;

	/*
	 * Wait for the source for the PLLS clock to switch to PLL
	 */
	while (!(KINETIS_MCG->status & KINETIS_MCG_S_PLLST_MSK));
}

/*
 * Transition from the PBE (PLL Bypassed External) to
 * the PEE (PLL Engaged External) mode.
 */
static void clock_pbe_to_pee(void)
{
	/*
	 * Select the output of the PLL as the system clock source
	 */
	KINETIS_MCG->c1 =
		/* Keep all bits but CLKS */
		(KINETIS_MCG->c1 & ~KINETIS_MCG_C1_CLKS_MSK) |
		KINETIS_MCG_C1_CLKS_FLLPLL_MSK;

	/*
	 * Wait for the PLL to start feeding MCGOUT
	 */
	while ((KINETIS_MCG->status & KINETIS_MCG_S_CLKST_MSK) !=
		KINETIS_MCG_S_CLKST_PLL);
}

#if defined(CONFIG_KINETIS_DDR) && !defined(CONFIG_KINETIS_DDR_SYNC)
/*
 * Configure the DDR clock for the DDR asynchronous mode
 */
static void clock_setup_ddr_async(void)
{
	clock_setup_pll1();
}
#endif /* CONFIG_KINETIS_DDR && !CONFIG_KINETIS_DDR_SYNC */

/*
 * Oscillator (OSC) register map
 */
struct kinetis_osc_regs {
	u8 cr;		/* OSC Control Register */
};

/*
 * OSC registers base
 */
#define KINETIS_OSC0_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x00065000)
#define KINETIS_OSC1_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x000E5000)

static u32 kinetis_osc_base[] = {
	KINETIS_OSC0_BASE, KINETIS_OSC1_BASE,
};

/*
 * Enable OSCx module to control oscillator or external reference clock
 */
static void osc_enable(int osc)
{
	volatile struct kinetis_osc_regs *regs =
		(volatile struct kinetis_osc_regs *)kinetis_osc_base[osc];

	/*
	 * Enable clock on OSC1 module. OSC0 is always enabled.
	 */
	if (osc == 1)
		kinetis_periph_enable(KINETIS_CG_OSC1, 1);

	regs->cr = KINETIS_OSC_CR_EREFSTEN_MSK | KINETIS_OSC_CR_ERCLKEN_MSK;
}

/*
 * Set-up clocks
 */
static void clock_setup(void)
{
	/*
	 * Set the OSC module to buffer the clock from EXTAL0 onto the OSC0ERCLK
	 *
	 * The OSC0ERCLK clock is used as the Ethernet RMII clock,
	 * it must be 50 MHz.
	 */
	osc_enable(0);

#if KINETIS_MCG_PLLREFSEL == 1
	/*
	 * Enable OSC1, if needed
	 */
	osc_enable(1);
#endif

	/*
	 * Switch to the FBE (FLL Bypassed External) mode
	 */
	clock_fei_to_fbe();

	/*
	 * Configure dividers for:
	 *     1. Core/system clock
	 *     2. Bus clock
	 *     3. FlexBus clock
	 *     4. Flash clock
	 */
	KINETIS_SIM->clkdiv1 =
		((KINETIS_CCLK_DIV - 1) << KINETIS_SIM_CLKDIV1_OUTDIV1_BITS) |
		((KINETIS_PCLK_DIV - 1) << KINETIS_SIM_CLKDIV1_OUTDIV2_BITS) |
		((KINETIS_FLEXBUS_CLK_DIV - 1) <<
			KINETIS_SIM_CLKDIV1_OUTDIV3_BITS) |
		((KINETIS_FLASH_CLK_DIV - 1) <<
			KINETIS_SIM_CLKDIV1_OUTDIV4_BITS);

	/*
	 * Configure clock divider for the NAND Flash Controller
	 */
	KINETIS_SIM->clkdiv4 =
		(KINETIS_SIM->clkdiv4 & ~(KINETIS_SIM_CLKDIV4_NFCDIV_MSK |
			KINETIS_SIM_CLKDIV4_NFCFRAC_MSK)) |
		((KINETIS_NFCCLK_DIV - 1) << KINETIS_SIM_CLKDIV4_NFCDIV_BITS) |
		((KINETIS_NFCCLK_FRAC - 1) << KINETIS_SIM_CLKDIV4_NFCFRAC_BITS);
	/*
	 * Select the NAND Flash Controller clock source
	 */
	KINETIS_SIM->sopt2 =
		(KINETIS_SIM->sopt2 & ~(KINETIS_SIM_SOPT2_NFCSRC_MSK |
			KINETIS_SIM_SOPT2_NFC_CLKSEL_MSK)) |
		KINETIS_SIM_SOPT2_NFCSRC |
		KINETIS_SIM_SOPT2_NFC_CLKSEL_NFCDIV;

#ifdef KINETIS_LCDCCLK_DIV
	/*
	 * Configure the LCD Controller clock source
	 */
	KINETIS_SIM->sopt2 =
		(KINETIS_SIM->sopt2 & ~(KINETIS_SIM_SOPT2_LCDCSRC_MSK |
			KINETIS_SIM_SOPT2_LCDC_CLKSEL_MSK)) |
		KINETIS_SIM_SOPT2_LCDCSRC |
		KINETIS_SIM_SOPT2_LCDC_CLKSEL_LCDCDIV;
	/*
	 * Configure the LCD Controller clock divider
	 */
	KINETIS_SIM->clkdiv3 =
		(KINETIS_SIM->clkdiv3 & ~(KINETIS_SIM_CLKDIV3_LCDCDIV_MSK |
			KINETIS_SIM_CLKDIV3_LCDCFRAC_MSK)) |
		((KINETIS_LCDCCLK_DIV - 1) <<
			KINETIS_SIM_CLKDIV3_LCDCDIV_BITS) |
		((KINETIS_LCDCCLK_FRAC - 1) <<
			KINETIS_SIM_CLKDIV3_LCDCFRAC_BITS);
#endif /* KINETIS_LCDCCLK_DIV */

	/*
	 * TBD: Configure clock dividers for USB and I2S here
	 * via KINETIS_SIM->clkdiv2
	 */

	/*
	 * Configure PLL and enter the PBE (PLL Bypassed External) mode
	 */
	clock_fbe_to_pbe();

	/*
	 * Switch to the PEE (PLL Engaged External) mode
	 */
	clock_pbe_to_pee();

#if defined(CONFIG_KINETIS_DDR) && !defined(CONFIG_KINETIS_DDR_SYNC)
	/*
	 * Configure the DDR clock for the DDR asynchronous mode
	 *
	 * If we use the DDR synchronous mode, then the PLL1 is already
	 * configured.
	 */
	clock_setup_ddr_async();
#endif /* CONFIG_KINETIS_DDR && !CONFIG_KINETIS_DDR_SYNC */

	/*
	 * TBD: Configure the USB clock via KINETIS_SIM->sopt2::PLLFLLSEL
	 */
}

/*
 * Initialize the reference clocks.
 */
void clock_init(void)
{
	clock_setup();

	/*
	 * Set the SYSTICK rate
	 *
	 * On Kinetis, the SYSTICK rate is always equal to the CPU clock rate.
	 */
	clock_val[CLOCK_SYSTICK] = KINETIS_CPU_RATE;

	/*
	 * Set the core/system clock rate
	 */
	clock_val[CLOCK_CCLK] = KINETIS_CPU_RATE;

	/*
	 * Set the bus clock rate (used for many peripherals)
	 */
	clock_val[CLOCK_PCLK] = KINETIS_PCLK_RATE;

	/*
	 * The MAC internal module clock is OSC0ERCLK
	 */
	clock_val[CLOCK_MACCLK] = KINETIS_OSC0_RATE;

#ifdef CONFIG_KINETIS_DDR
	/*
	 * Set the DDR clock rate
	 */
	clock_val[CLOCK_DDRCLK] = KINETIS_DDR_RATE;
#endif /* CONFIG_KINETIS_DDR */
}

/*
 * Return a clock value for the specified clock.
 *
 * @param clck          id of the clock
 * @returns             frequency of the clock
 */
unsigned long clock_get(enum clock clck)
{
	return clock_val[clck];
}

/*
 * Enable or disable the clock on a peripheral device (timers, UARTs, USB, etc)
 */
int kinetis_periph_enable(kinetis_clock_gate_t gate, int enable)
{
	volatile u32 *scgc;
	u32 mask;
	int rv;

	/*
	 * Verify the function arguments
	 */
	if (KINETIS_CG_REG(gate) >= KINETIS_SIM_CG_NUMREGS ||
	    KINETIS_CG_IDX(gate) >= KINETIS_SIM_CG_NUMBITS) {
		rv = -EINVAL;
		goto out;
	}

	scgc = &KINETIS_SIM->scgc[KINETIS_CG_REG(gate)];
	mask = 1 << KINETIS_CG_IDX(gate);

        if (gate == KINETIS_CG_PORTF && enable) { /* K70 Errata #5234 */
                mask |= (1 << KINETIS_CG_IDX(KINETIS_CG_PORTE));
        }

	if (enable)
		*scgc |= mask;
	else
		*scgc &= ~mask;

	rv = 0;
out:
	return rv;
}
