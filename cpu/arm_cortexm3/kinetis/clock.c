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
 * Clock rate on the input of the MCG (Multipurpose Clock Generator)
 */
#define KINETIS_MCG_PLL_IN_RATE		KINETIS_EXTAL_RATE

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
#if KINETIS_MCG_FREQ_RANGE == 0
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

#undef KINETIS_MCG_PLLREFSEL	/* No support for multiple oscillators */

#else
#error Unsupported Freescale Kinetis MCU series
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
#define KINETIS_MCGOUT_RATE \
	(KINETIS_MCG_PLL_IN_RATE / KINETIS_PLL_PRDIV * KINETIS_PLL_VDIV / \
	KINETIS_PLL_VCO_DIV)

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
#define KINETIS_MCG_C2_RANGE_BITS	4
/*
 * External Reference Select
 *    0 = External reference clock requested.
 *    1 = Oscillator requested.
 */
#define KINETIS_MCG_C2_EREFS_MSK	(1 << 2)
/* Configure crystal oscillator for high-gain operation */
#define KINETIS_MCG_C2_HGO_MSK		(1 << 3)
/*
 * MCG Control 5 Register
 */
/* PLL External Reference Divider */
#define KINETIS_MCG_C5_PRDIV_BITS	0
/* PLL Stop Enable */
#define KINETIS_MCG_C5_PLLSTEN_MSK	(1 << 5)
/*
 * MCG Control 6 Register
 */
/* VCO Divider */
#define KINETIS_MCG_C6_VDIV_BITS	0
/* PLL Select */
#define KINETIS_MCG_C6_PLLS_MSK		(1 << 6)
/*
 * MCG Status Register
 */
/* OSC Initialization */
#define KINETIS_MCG_S_OSCINIT_MSK	(1 << 1)
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
 * SIM registers
 */
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
	u32 scgc1;	/* System Clock Gating Control Register 1 */
	u32 scgc2;	/* System Clock Gating Control Register 2 */
	u32 scgc3;	/* System Clock Gating Control Register 3 */
	u32 scgc4;	/* System Clock Gating Control Register 4 */
	u32 scgc5;	/* System Clock Gating Control Register 5 */
	u32 scgc6;	/* System Clock Gating Control Register 6 */
	u32 scgc7;	/* System Clock Gating Control Register 7 */
	u32 clkdiv1;	/* System Clock Divider Register 1 */
	u32 clkdiv2;	/* System Clock Divider Register 2 */
	u32 fcfg1;	/* Flash Configuration Register 1 */
	u32 fcfg2;	/* Flash Configuration Register 2 */
	u32 uidh;	/* Unique Identification Register High */
	u32 uidmh;	/* Unique Identification Register Mid-High */
	u32 uidml;	/* Unique Identification Register Mid Low */
	u32 uidl;	/* Unique Identification Register Low */
};

/*
 * SIM registers base
 */
#define KINETIS_SIM_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x00047000)
#define KINETIS_SIM			((volatile struct kinetis_sim_regs *) \
					KINETIS_SIM_BASE)

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
};

/*
 * MCG registers base
 */
#define KINETIS_MCG_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x00064000)
#define KINETIS_MCG			((volatile struct kinetis_mcg_regs *) \
					KINETIS_MCG_BASE)

/*
 * Oscillator (OSC) register map
 */
struct kinetis_osc_regs {
	u8 cr;		/* OSC Control Register */
};

/*
 * OSC registers base
 */
#define KINETIS_OSC_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x00065000)
#define KINETIS_OSC			((volatile struct kinetis_osc_regs *) \
					KINETIS_OSC_BASE)

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
	KINETIS_MCG->c2 = (KINETIS_MCG_FREQ_RANGE << KINETIS_MCG_C2_RANGE_BITS);

	/*
	 * Set the FLL External Reference Divider.
	 * Select the clock source for MCGOUTCLK.
	 */
	KINETIS_MCG->c1 =
		(KINETIS_MCG_FRDIV << KINETIS_MCG_C1_FRDIV_BITS) |
		KINETIS_MCG_C1_CLKS_EXT_REF_MSK;

	/*
	 * Wait for the input from the external crystal to initialize
	 */
	while (!(KINETIS_MCG->status & KINETIS_MCG_S_OSCINIT_MSK));

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

/*
 * Transition from the FBE (FLL Bypassed External) to
 * the PBE (PLL Bypassed External) mode.
 */
static void clock_fbe_to_pbe(void)
{
	/*
	 * Configure the PLL input divider.
	 * Also, enable the PLL clock during Normal Stop.
	 */
	KINETIS_MCG->c5 =
		((KINETIS_PLL_PRDIV - 1) << KINETIS_MCG_C5_PRDIV_BITS) |
		KINETIS_MCG_C5_PLLSTEN_MSK;

	/*
	 * Set the PLL multiplication factor
	 */
	KINETIS_MCG->c6 = (KINETIS_PLL_VDIV - KINETIS_PLL_VDIV_MIN) <<
		KINETIS_MCG_C6_VDIV_BITS;

	/*
	 * Switch to the PBE mode
	 */
	KINETIS_MCG->c6 |= KINETIS_MCG_C6_PLLS_MSK;

	/*
	 * Wait for the source for the PLLS clock to switch to PLL
	 */
	while (!(KINETIS_MCG->status & KINETIS_MCG_S_PLLST_MSK));

	/*
	 * Wait for the PLL to acquire lock
	 */
	while (!(KINETIS_MCG->status & KINETIS_MCG_S_LOCK_MSK));
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

/*
 * Set-up clocks
 */
static void clock_setup(void)
{
	/*
	 * Set the OSC module to buffer a clock from EXTAL onto the OSC_CLK_OUT
	 *
	 * The OSCERCLK clock is used as the Ethernet RMII clock,
	 * it must be 50 MHz.
	 */
	KINETIS_OSC->cr =
		KINETIS_OSC_CR_EREFSTEN_MSK | KINETIS_OSC_CR_ERCLKEN_MSK;

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
