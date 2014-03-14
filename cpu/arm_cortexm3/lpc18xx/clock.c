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

#include <common.h>
#include <asm/errno.h>

#include <asm/arch/lpc18xx_creg.h>
#include <asm/arch/lpc18xx.h>
#include "clock.h"

/*
 * PLL0* register map
 * Used for PLL0USB at 0x4005001C and for PLL0AUDIO at 0x4005002C.
 *
 * This structure is 0x10 bytes long, it is important when it embedding into
 * `struct lpc18xx_cgu_regs`.
 */
struct lpc18xx_pll0_regs {
	u32 stat;	/* PLL status register */
	u32 ctrl;	/* PLL control register */
	u32 mdiv;	/* PLL M-divider register */
	u32 np_div;	/* PLL N/P-divider register */
};

/*
 * CGU (Clock Generation Unit) register map
 * Should be mapped at 0x40050000.
 */
struct lpc18xx_cgu_regs {
	u32 rsv0[5];
	u32 freq_mon;		/* Frequency monitor */
	u32 xtal_osc_ctrl;	/* XTAL oscillator control */
	struct lpc18xx_pll0_regs pll0usb;	/* PLL0USB registers */
	struct lpc18xx_pll0_regs pll0audio;	/* PLL0AUDIO registers */
	u32 pll0audio_frac;	/* PLL0AUDIO fractional divider */
	u32 pll1_stat;		/* PLL1 status register */
	u32 pll1_ctrl;		/* PLL1 control register */
	u32 idiv[5];		/* IDIVA_CTRL .. IDIVE_CTRL */

	/* BASE_* clock configuration registers */
	u32 safe_clk;
	u32 usb0_clk;
	u32 periph_clk;
	u32 usb1_clk;
	u32 m4_clk;
	u32 spifi_clk;
	u32 spi_clk;
	u32 phy_rx_clk;
	u32 phy_tx_clk;
	u32 apb1_clk;
	u32 apb3_clk;
	u32 lcd_clk;
	u32 vadc_clk;
	u32 sdio_clk;
	u32 ssp0_clk;
	u32 ssp1_clk;
	u32 uart0_clk;
	u32 uart1_clk;
	u32 uart2_clk;
	u32 uart3_clk;
	u32 out_clk;
	u32 rsv1[4];
	u32 apll_clk;
	u32 cgu_out0_clk;
	u32 cgu_out1_clk;
};

/*
 * CGU registers base
 */
#define LPC18XX_CGU_BASE		0x40050000
#define LPC18XX_CGU			((volatile struct lpc18xx_cgu_regs *) \
					LPC18XX_CGU_BASE)

/*
 * Bit offsets in Clock Generation Unit (CGU) registers
 */
/*
 * Crystal oscillator control register (XTAL_OSC_CTRL)
 */
/* Oscillator-pad enable */
#define LPC18XX_CGU_XTAL_ENABLE		(1 << 0)
/* Select frequency range */
#define LPC18XX_CGU_XTAL_HF		(1 << 2)

#if (CONFIG_LPC18XX_EXTOSC_RATE < 10000000) || \
    (CONFIG_LPC18XX_EXTOSC_RATE > 25000000)
#error CONFIG_LPC18XX_EXTOSC_RATE is out of range for PLL1
#endif
/*
 * For all CGU clock registers
 */
/* CLK_SEL: Clock source selection */
#define LPC18XX_CGU_CLKSEL_BITS		24
#define LPC18XX_CGU_CLKSEL_MSK		(0x1f << LPC18XX_CGU_CLKSEL_BITS)
/* ENET_RX_CLK */
#define LPC18XX_CGU_CLKSEL_ENET_RX	(0x02 << LPC18XX_CGU_CLKSEL_BITS)
/* ENET_TX_CLK */
#define LPC18XX_CGU_CLKSEL_ENET_TX	(0x03 << LPC18XX_CGU_CLKSEL_BITS)
/* Crystal oscillator */
#define LPC18XX_CGU_CLKSEL_XTAL		(0x06 << LPC18XX_CGU_CLKSEL_BITS)
/* PLL1 */
#define LPC18XX_CGU_CLKSEL_PLL1		(0x09 << LPC18XX_CGU_CLKSEL_BITS)
/* Block clock automatically during frequency change */
#define LPC18XX_CGU_AUTOBLOCK_MSK	(1 << 11)
/*
 * PLL1 control register
 */
/* Power-down */
#define LPC18XX_CGU_PLL1CTRL_PD_MSK		(1 << 0)
/* Input clock bypass control */
#define LPC18XX_CGU_PLL1CTRL_BYPASS_MSK		(1 << 1)
/* PLL feedback select */
#define LPC18XX_CGU_PLL1CTRL_FBSEL_MSK		(1 << 6)
/* PLL direct CCO output */
#define LPC18XX_CGU_PLL1CTRL_DIRECT_MSK		(1 << 7)
/* Post-divider division ratio P. The value applied is 2**P. */
#define LPC18XX_CGU_PLL1CTRL_PSEL_BITS		8
#define LPC18XX_CGU_PLL1CTRL_PSEL_MSK \
	(3 << LPC18XX_CGU_PLL1CTRL_PSEL_BITS)
/* Pre-divider division ratio */
#define LPC18XX_CGU_PLL1CTRL_NSEL_BITS		12
#define LPC18XX_CGU_PLL1CTRL_NSEL_MSK \
	(3 << LPC18XX_CGU_PLL1CTRL_NSEL_BITS)
/* Feedback-divider division ratio (M) */
#define LPC18XX_CGU_PLL1CTRL_MSEL_BITS		16
#define LPC18XX_CGU_PLL1CTRL_MSEL_MSK \
	(0xff << LPC18XX_CGU_PLL1CTRL_MSEL_BITS)
/*
 * PLL1 status register
 */
/* PLL1 lock indicator */
#define LPC18XX_CGU_PLL1STAT_LOCK	(1 << 0)

/*
 * RGU (Reset Generation Unit) register map
 */
struct lpc18xx_rgu_regs {
	u32 rsv0[64];

	u32 ctrl0;		/* Reset control register 0 */
	u32 ctrl1;		/* Reset control register 1 */
	u32 rsv1[2];

	u32 status0;		/* Reset status register 0 */
	u32 status1;		/* Reset status register 1 */
	u32 status2;		/* Reset status register 2 */
	u32 status3;		/* Reset status register 3 */
	u32 rsv2[12];

	u32 active_status0;	/* Reset active status register 0 */
	u32 active_status1;	/* Reset active status register 1 */
	u32 rsv3[170];

	u32 ext_stat[64];	/* Reset external status registers */
};

/*
 * RGU registers base
 */
#define LPC18XX_RGU_BASE		0x40053000
#define LPC18XX_RGU			((volatile struct lpc18xx_rgu_regs *) \
					LPC18XX_RGU_BASE)

/*
 * RESET_CTRL0 register
 */
/* ETHERNET_RST */
#define LPC18XX_RGU_CTRL0_ETHERNET	(1 << 22)


#define MIN_LOCK_PERIOD_MS		250

/*
 * Clock values
 */
static u32 clock_val[CLOCK_END];

/*
 * Compile time sanity checks for defined board clock setup
 */
#ifndef CONFIG_LPC18XX_EXTOSC_RATE
#error CONFIG_LPC18XX_EXTOSC_RATE is not set, set to the external osc rate
#endif

/*
 * Verify that the request PLL1 output frequency fits the range
 * of 156 MHz to 320 MHz, so that the PLL1 Direct Mode is applicable.
 * Our clock configuration code (`clock_setup()`) support only this mode.
 */
#if LPC18XX_PLL1_CLK_OUT < 144000000
#error Requested PLL1 output frequency is too low
#endif
#if LPC18XX_PLL1_CLK_OUT > 320000000
#error Requested PLL1 output frequency is too high
#endif

/*
 * Use this function to implement delays until the clock system is initialized
 */
static void cycle_delay(int n)
{
	volatile int i;
	for (i = 0; i < n; i++);
}

/*
 * Set-up the external crystal oscillator, PLL1, CPU core clock and
 * all necessary clocks for peripherals.
 */
static void clock_setup(void)
{
	int lock_count = 0;
	/*
	 * Configure and enable the external crystal oscillator
	 * (make sure it's turned off while setting its mode)
	 */
	LPC18XX_CGU->xtal_osc_ctrl |= LPC18XX_CGU_XTAL_ENABLE;
#if CONFIG_LPC18XX_EXTOSC_RATE > 15000000
	LPC18XX_CGU->xtal_osc_ctrl |= LPC18XX_CGU_XTAL_HF;
#else
	LPC18XX_CGU->xtal_osc_ctrl &= ~LPC18XX_CGU_XTAL_HF;
#endif
	LPC18XX_CGU->xtal_osc_ctrl &= ~LPC18XX_CGU_XTAL_ENABLE;

	/*
	 * Wait for the external oscillator to stabilize
	 */
	cycle_delay(1000000);

	/*
	 * PLL1 disabled while altering values
	 */
	LPC18XX_CGU->pll1_ctrl &= ~LPC18XX_CGU_PLL1CTRL_PD_MSK;

	/*
	 * PLL1 clksrc = xtal
	 */
	LPC18XX_CGU->pll1_ctrl =
		(LPC18XX_CGU->pll1_ctrl & ~LPC18XX_CGU_CLKSEL_MSK) |
		LPC18XX_CGU_CLKSEL_XTAL | LPC18XX_CGU_AUTOBLOCK_MSK;

	/*
	 * Configure PLL1 for desired output
	 */
	LPC18XX_CGU->pll1_ctrl =
		(LPC18XX_CGU->pll1_ctrl &
		~(LPC18XX_CGU_PLL1CTRL_FBSEL_MSK |
		LPC18XX_CGU_PLL1CTRL_BYPASS_MSK |
		LPC18XX_CGU_PLL1CTRL_DIRECT_MSK |
		LPC18XX_CGU_PLL1CTRL_PSEL_MSK |
		LPC18XX_CGU_PLL1CTRL_NSEL_MSK |
		LPC18XX_CGU_PLL1CTRL_MSEL_MSK)) |
		((CONFIG_LPC18XX_PLL1_M - 1) <<
			LPC18XX_CGU_PLL1CTRL_MSEL_BITS) |
		(0 << LPC18XX_CGU_PLL1CTRL_NSEL_BITS) |
		(1 << LPC18XX_CGU_PLL1CTRL_PSEL_BITS) |
		LPC18XX_CGU_PLL1CTRL_DIRECT_MSK |
		LPC18XX_CGU_PLL1CTRL_FBSEL_MSK;

	/*
	 * Now enable PLL1
	 */
	LPC18XX_CGU->pll1_ctrl &= ~LPC18XX_CGU_PLL1CTRL_PD_MSK;

	/*
	 * Wait for PLL1 to acquire lock
	 */
	while (lock_count < MIN_LOCK_PERIOD_MS) {
		if (!(LPC18XX_CGU->pll1_stat & LPC18XX_CGU_PLL1STAT_LOCK)) {
			lock_count = 0;
		} else {
			lock_count++;
		}
		cycle_delay(5000);
	}

	/*
	 * Now safe to switch to PLL1 for M4 core clock
	 */
	LPC18XX_CGU->m4_clk =
		(LPC18XX_CGU->m4_clk & ~LPC18XX_CGU_CLKSEL_MSK) |
		LPC18XX_CGU_CLKSEL_PLL1 | LPC18XX_CGU_AUTOBLOCK_MSK;

	/* Wait 1ms */
	cycle_delay(60000);

	/*
	 * Set-up clocks for UARTs
	 */
#ifdef CONFIG_UART0_CLOCK_XTAL
	LPC18XX_CGU->uart0_clk = LPC18XX_CGU_CLKSEL_XTAL |
		LPC18XX_CGU_AUTOBLOCK_MSK;
#else
	LPC18XX_CGU->uart0_clk = LPC18XX_CGU_CLKSEL_PLL1 |
		LPC18XX_CGU_AUTOBLOCK_MSK;
#endif
	LPC18XX_CGU->uart1_clk = LPC18XX_CGU_CLKSEL_PLL1 |
		LPC18XX_CGU_AUTOBLOCK_MSK;
	LPC18XX_CGU->uart2_clk = LPC18XX_CGU_CLKSEL_PLL1 |
		LPC18XX_CGU_AUTOBLOCK_MSK;
	LPC18XX_CGU->uart3_clk = LPC18XX_CGU_CLKSEL_PLL1 |
		LPC18XX_CGU_AUTOBLOCK_MSK;

#if defined(CONFIG_LPC_SPI)
	/*
	 * Set-up clocks for SPI
	 */
	LPC18XX_CGU->spifi_clk = LPC18XX_CGU_CLKSEL_PLL1 |
		LPC18XX_CGU_AUTOBLOCK_MSK;
#endif
}

/*
 * Set-up the Ethernet clock and reset the Ethernet MAC
 */
void eth_clock_setup(void)
{
	int timeout;
	int rv;

#ifndef CONFIG_LPC18XX_ENET_USE_PHY_RMII
	/*
	 * This clock configuration is valid only for MII
	 */
	LPC18XX_CGU->phy_rx_clk =
		LPC18XX_CGU_CLKSEL_ENET_RX | LPC18XX_CGU_AUTOBLOCK_MSK;
	LPC18XX_CGU->phy_tx_clk =
		LPC18XX_CGU_CLKSEL_ENET_TX | LPC18XX_CGU_AUTOBLOCK_MSK;
#endif /* CONFIG_LPC18XX_ENET_USE_PHY_RMII */

	/*
	 * Choose the MII Ethernet mode
	 */
	LPC18XX_CREG->creg6 =
		(LPC18XX_CREG->creg6 & ~LPC18XX_CREG_CREG6_ETHMODE_MSK) |
#ifndef CONFIG_LPC18XX_ENET_USE_PHY_RMII
		LPC18XX_CREG_CREG6_ETHMODE_MII;
#else
		LPC18XX_CREG_CREG6_ETHMODE_RMII;
#endif

	/*
	 * Reset the Ethernet module of the MCU
	 */
	LPC18XX_RGU->ctrl0 = LPC18XX_RGU_CTRL0_ETHERNET;

	/*
	 * Wait for the Ethernet module to exit the reset state
	 */
	timeout = 10;
	rv = -ETIMEDOUT;
	while (timeout-- > 0) {
		if (!(LPC18XX_RGU->active_status0 &
		    LPC18XX_RGU_CTRL0_ETHERNET)) {
			udelay(1000);
		} else {
			timeout = 0;
			rv = 0;
		}
	}

	if (rv < 0) {
		printf("%s: Reset of the Ethernet module timed out.\n",
			__func__);
	}
}

/*
 * Initialize the reference clocks.
 */
void clock_init(void)
{
	/*
	 * Set-up clocks
	 */
	clock_setup();

	/*
	 * Set-up Ethernet clocks
	 */
	eth_clock_setup();

	/*
	 * Set SysTick timer rate to the CPU core clock
	 */
	clock_val[CLOCK_SYSTICK] = LPC18XX_PLL1_CLK_OUT;

	/*
	 * Set the CPU core clock
	 */
	clock_val[CLOCK_CCLK] = LPC18XX_PLL1_CLK_OUT;

	/*
	 * Set UARTx base clock rate
	 */
#ifdef CONFIG_UART0_CLOCK_XTAL
	clock_val[CLOCK_UART0] = CONFIG_LPC18XX_EXTOSC_RATE;
#else
	clock_val[CLOCK_UART0] = LPC18XX_PLL1_CLK_OUT;
#endif
	clock_val[CLOCK_UART1] = LPC18XX_PLL1_CLK_OUT;
	clock_val[CLOCK_UART2] = LPC18XX_PLL1_CLK_OUT;
	clock_val[CLOCK_UART3] = LPC18XX_PLL1_CLK_OUT;

	/*
	 * Set SPI base clock rate
	 */
#if defined(CONFIG_LPC_SPI)
	clock_val[CLOCK_SPI] = LPC18XX_PLL1_CLK_OUT;
#endif
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
