/*
 * (C) Copyright 2011, 2015
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 * Vladimir Skvortsov, Emcraft Systems, vskvortsov@emcraft.com
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
 * STM32 processor definitions
 */
#ifndef _MACH_STM32_H_
#define _MACH_STM32_H_

/******************************************************************************
 * Peripheral memory map
 ******************************************************************************/

#define STM32_PERIPH_BASE	((u32)0x40000000)
#define STM32_APB1PERIPH_BASE	(STM32_PERIPH_BASE + 0x00000000)
#define STM32_APB2PERIPH_BASE	(STM32_PERIPH_BASE + 0x00010000)
#define STM32_AHB1PERIPH_BASE	(STM32_PERIPH_BASE + 0x00020000)
#define STM32_AHB2PERIPH_BASE	(STM32_PERIPH_BASE + 0x10000000)
#define STM32_AHB3PERIPH_BASE	(STM32_PERIPH_BASE + 0x60000000)

/******************************************************************************
 * Reset and Clock Control
 ******************************************************************************/

/*
 * RCC register map
 */
struct stm32_rcc_regs {
	u32	cr;		/* RCC clock control			      */
	u32	pllcfgr;	/* RCC PLL configuration		      */
	u32	cfgr;		/* RCC clock configuration		      */
	u32	cir;		/* RCC clock interrupt			      */
	u32	ahb1rstr;	/* RCC AHB1 peripheral reset		      */
	u32	ahb2rstr;	/* RCC AHB2 peripheral reset		      */
	u32	ahb3rstr;	/* RCC AHB3 peripheral reset		      */
	u32	rsv0;
	u32	apb1rstr;	/* RCC APB1 peripheral reset		      */
	u32	apb2rstr;	/* RCC APB2 peripheral reset		      */
	u32	rsv1[2];
	u32	ahb1enr;	/* RCC AHB1 peripheral clock enable	      */
	u32	ahb2enr;	/* RCC AHB2 peripheral clock enable	      */
	u32	ahb3enr;	/* RCC AHB3 peripheral clock enable	      */
	u32	rsv2;
	u32	apb1enr;	/* RCC APB1 peripheral clock enable	      */
	u32	apb2enr;	/* RCC APB2 peripheral clock enable	      */
	u32	rsv3[2];
	u32	ahb1lpenr;	/* RCC AHB1 periph clk enable in low pwr mode */
	u32	ahb2lpenr;	/* RCC AHB2 periph clk enable in low pwr mode */
	u32	ahb3lpenr;	/* RCC AHB3 periph clk enable in low pwr mode */
	u32	rsv4;
	u32	apb1lpenr;	/* RCC APB1 periph clk enable in low pwr mode */
	u32	apb2lpenr;	/* RCC APB2 periph clk enable in low pwr mode */
	u32	rsv5[2];
	u32	bdcr;		/* RCC Backup domain control		      */
	u32	csr;		/* RCC clock control & status		      */
	u32	rsv6[2];
	u32	sscgr;		/* RCC spread spectrum clock generation	      */
	u32	plli2scfgr;	/* RCC PLLI2S configuration		      */

	/* Only for STM32F4{2,3}xxx and STM32F7 */
	u32	pllsaicfgr;	/* RCC PLLSAI configuration */
	u32	dckcfgr;	/* RCC Dedicated Clocks configuration */
};

/*
 * Clocks enumeration
 */
enum clock {
	CLOCK_SYSCLK,		/* SYSCLK clock frequency expressed in Hz     */
	CLOCK_HCLK,		/* HCLK clock frequency expressed in Hz       */
	CLOCK_PCLK1,		/* PCLK1 clock frequency expressed in Hz      */
	CLOCK_PCLK2,		/* PCLK2 clock frequency expressed in Hz      */
	CLOCK_SYSTICK,		/* Systimer clock frequency expressed in Hz   */
	CLOCK_DIVM,		/* Input clock for PLL, PLLI2S, PLLSAI in Hz */
	CLOCK_END		/* for internal usage			      */
};

/*
 * RCC registers base
 */
#define STM32_RCC_BASE			(STM32_AHB1PERIPH_BASE + 0x3800)
#define STM32_RCC			((volatile struct stm32_rcc_regs *) \
					STM32_RCC_BASE)
/*
 * PWR registers map
 */
struct stm32_pwr_regs {
	u32 cr1;   /* power control register 1 */
	u32 csr1;  /* power control/status register 2 */
	u32 cr2;   /* power control register 2 */
	u32 csr2;  /* power control/status register 2 */
};


#define STM32_PWR_BASE			(STM32_APB1PERIPH_BASE + 0x7000)
#define STM32_PWR			((volatile struct stm32_pwr_regs *) \
					STM32_PWR_BASE)

/*
 * LTDC registers map
 */
#define STM32F4_LTDC_BASE	(STM32_APB2PERIPH_BASE + 0x6800)


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

#define PAGE_SIZE	4096

#define  RCC_AHB1ENR_DMA1EN	((uint32_t)0x00200000)
#define  RCC_AHB1ENR_DMA2EN	((uint32_t)0x00400000)
#define  RCC_APB2ENR_SPI1EN	((uint32_t)0x00001000)
#define  RCC_AHB3ENR_QSPIEN	((uint32_t)(1 << 1))

#endif /* _MACH_STM32_H_ */
