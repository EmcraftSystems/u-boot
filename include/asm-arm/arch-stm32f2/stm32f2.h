/*
 * (C) Copyright 2011
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
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
 * STM32F2 processor definitions
 */
#ifndef _MACH_STM32F2_H_
#define _MACH_STM32F2_H_

/******************************************************************************
 * Peripheral memory map
 ******************************************************************************/

#define STM32F2_PERIPH_BASE	0x40000000
#define STM32F2_APB1PERITH_BASE	(STM32F2_PERIPH_BASE + 0x00000000)
#define STM32F2_APB2PERITH_BASE	(STM32F2_PERIPH_BASE + 0x00010000)
#define STM32F2_AHB1PERITH_BASE	(STM32F2_PERIPH_BASE + 0x00020000)
#define STM32F2_AHB2PERITH_BASE	(STM32F2_PERIPH_BASE + 0x10000000)

/******************************************************************************
 * Universal Synchronous Asynchronous Receiver Transmitter
 ******************************************************************************/

/*
 * USART register map
 */
struct stm32f2_usart_regs {
	u16	sr;		/* Status				      */
	u16	rsv0;
	u16	dr;		/* Data					      */
	u16	rsv1;
	u16	brr;		/* Baud rate				      */
	u16	rsv2;
	u16	cr1;		/* Control 1				      */
	u16	rsv3;
	u16	cr2;		/* Control 2				      */
	u16	rsv4;
	u16	cr3;		/* Control 3				      */
	u16	rsv5;
	u16	gtpr;		/* Guard time and prescaler		      */
};

/*
 * USART registers bases
 */
#define STM32F2_USART1_BASE	(STM32F2_APB2PERITH_BASE + 0x1000)
#define STM32F2_USART2_BASE	(STM32F2_APB1PERITH_BASE + 0x4400)
#define STM32F2_USART3_BASE	(STM32F2_APB1PERITH_BASE + 0x4800)
#define STM32F2_USART4_BASE	(STM32F2_APB1PERITH_BASE + 0x4C00)
#define STM32F2_USART5_BASE	(STM32F2_APB1PERITH_BASE + 0x5000)
#define STM32F2_USART6_BASE	(STM32F2_APB2PERITH_BASE + 0x1400)

/*
 * SR bit masks
 */
#define STM32F2_USART_SR_TXE	(1 <<  7)	/* Transmit data reg empty   */
#define STM32F2_USART_SR_RXNE	(1 <<  5)	/* Read data reg not empty   */

/*
 * BRR reg fields
 */
#define STM32F2_USART_BRR_FRAC_BIT	0	/* fraction of USARTDIV	     */
#define STM32F2_USART_BRR_FRAC_MSK	0x0F

#define STM32F2_USART_BRR_MANT_BIT	4	/* mantissa of USARTDIV	     */
#define STM32F2_USART_BRR_MANT_MSK	0xFFF

/*
 * CR1 bit masks
 */
#define STM32F2_USART_CR1_UE	(1 << 13)	/* USART enable		     */
#define STM32F2_USART_CR1_TE	(1 <<  3)	/* Transmitter enable	     */
#define STM32F2_USART_CR1_RE	(1 <<  2)	/* Receiver enable	     */

/******************************************************************************
 * Reset and Clock Control
 ******************************************************************************/

/*
 * RCC register map
 */
struct stm32f2_rcc_regs {
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
};

/*
 * Clocks enumeration
 */
enum clock {
	CLOCK_SYSCLK,		/* SYSCLK clock frequency expressed in Hz     */
	CLOCK_HCLK,		/* HCLK clock frequency expressed in Hz       */
	CLOCK_PCLK1,		/* PCLK1 clock frequency expressed in Hz      */
	CLOCK_PCLK2,		/* PCLK2 clock frequency expressed in Hz      */

	CLOCK_END		/* for internal usage			      */
};

/*
 * RCC registers base
 */
#define STM32F2_RCC_BASE	(STM32F2_AHB1PERITH_BASE + 0x3800)

/*
 * Internal oscillator values
 */
#define STM32F2_HSI_HZ		16000000

/*
 * Offsets and bitmasks of some RCC regs
 */
#define STM32F2_RCC_ENR_USART1	offsetof(struct stm32f2_rcc_regs, apb2enr)
#define STM32F2_RCC_MSK_USART1	(1 <<  4)

#define STM32F2_RCC_ENR_USART2	offsetof(struct stm32f2_rcc_regs, apb1enr)
#define STM32F2_RCC_MSK_USART2	(1 << 17)

#define STM32F2_RCC_ENR_USART3	offsetof(struct stm32f2_rcc_regs, apb1enr)
#define STM32F2_RCC_MSK_USART3	(1 << 18)

#define STM32F2_RCC_ENR_USART4	offsetof(struct stm32f2_rcc_regs, apb1enr)
#define STM32F2_RCC_MSK_USART4	(1 << 19)

#define STM32F2_RCC_ENR_USART5	offsetof(struct stm32f2_rcc_regs, apb1enr)
#define STM32F2_RCC_MSK_USART5	(1 << 20)

#define STM32F2_RCC_ENR_USART6	offsetof(struct stm32f2_rcc_regs, apb2enr)
#define STM32F2_RCC_MSK_USART6	(1 <<  5)

#define STM32F2_RCC_CFGR_SWS_BIT	0
#define STM32F2_RCC_CFGR_SWS_MSK	0x000F

#define STM32F2_RCC_CFGR_HPRE_BIT	4
#define STM32F2_RCC_CFGR_HPRE_MSK	0x000F

#define STM32F2_RCC_CFGR_PPRE1_BIT	10
#define STM32F2_RCC_CFGR_PPRE1_MSK	0x0007

#define STM32F2_RCC_CFGR_PPRE2_BIT	13
#define STM32F2_RCC_CFGR_PPRE2_MSK	0x0007

#define STM32F2_RCC_PLLCFGR_PLLSRC_BIT	22
#define STM32F2_RCC_PLLCFGR_PLLSRC_MSK	0x0001

#define STM32F2_RCC_PLLCFGR_PLLM_BIT	0
#define STM32F2_RCC_PLLCFGR_PLLM_MSK	0x003F

#define STM32F2_RCC_PLLCFGR_PLLN_BIT	6
#define STM32F2_RCC_PLLCFGR_PLLN_MSK	0x01FF

#define STM32F2_RCC_PLLCFGR_PLLP_BIT	16
#define STM32F2_RCC_PLLCFGR_PLLP_MSK	0x0003

/******************************************************************************
 * General Purpose I/O
 ******************************************************************************/

/*
 * GPIO register map
 */
struct stm32f2_gpio_regs {
	u32	moder;		/* GPIO port mode			      */
	u32	otyper;		/* GPIO port output type		      */
	u32	ospeedr;	/* GPIO port output speed		      */
	u32	pupdr;		/* GPIO port pull-up/pull-down		      */
	u32	idr;		/* GPIO port input data			      */
	u32	odr;		/* GPIO port output data		      */
	u16	bsrrl;		/* GPIO port bit set/reset low		      */
	u16	bsrrh;		/* GPIO port bit set/reset high		      */
	u32	lckr;		/* GPIO port configuration lock		      */
	u32	afr[2];		/* GPIO alternate function		      */
};

/*
 * GPIO registers bases
 */
#define STM32F2_GPIOA_BASE	(STM32F2_AHB1PERITH_BASE + 0x0000)
#define STM32F2_GPIOB_BASE	(STM32F2_AHB1PERITH_BASE + 0x0400)
#define STM32F2_GPIOC_BASE	(STM32F2_AHB1PERITH_BASE + 0x0800)
#define STM32F2_GPIOD_BASE	(STM32F2_AHB1PERITH_BASE + 0x0C00)
#define STM32F2_GPIOE_BASE	(STM32F2_AHB1PERITH_BASE + 0x1000)
#define STM32F2_GPIOF_BASE	(STM32F2_AHB1PERITH_BASE + 0x1400)
#define STM32F2_GPIOG_BASE	(STM32F2_AHB1PERITH_BASE + 0x1800)
#define STM32F2_GPIOH_BASE	(STM32F2_AHB1PERITH_BASE + 0x1C00)
#define STM32F2_GPIOI_BASE	(STM32F2_AHB1PERITH_BASE + 0x2000)

/*
 * GPIO configuration mode
 */
#define STM32F2_GPIO_MODE_IN	0x00
#define STM32F2_GPIO_MODE_OUT	0x01
#define STM32F2_GPIO_MODE_AF	0x02
#define STM32F2_GPIO_MODE_AN	0x03

/*
 * GPIO output type
 */
#define STM32F2_GPIO_OTYPE_PP	0x00
#define STM32F2_GPIO_OTYPE_OD	0x01

/*
 * GPIO output maximum frequency
 */
#define STM32F2_GPIO_SPEED_2M	0x00
#define STM32F2_GPIO_SPEED_25M	0x01
#define STM32F2_GPIO_SPEED_50M	0x02
#define STM32F2_GPIO_SPEED_100M	0x03

/*
 * GPIO pullup, pulldown configuration
 */
#define STM32F2_GPIO_PUPD_NO	0x00
#define STM32F2_GPIO_PUPD_UP	0x01
#define STM32F2_GPIO_PUPD_DOWN	0x02

/*
 * AF7 selection
 */
#define STM32F2_GPIO_AF_USART1	0x07
#define STM32F2_GPIO_AF_USART2	0x07
#define STM32F2_GPIO_AF_USART3	0x07

/*
 * AF8 selection
 */
#define STM32F2_GPIO_AF_USART4	0x08
#define STM32F2_GPIO_AF_USART5	0x08
#define STM32F2_GPIO_AF_USART6	0x08

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

#endif /* _MACH_STM32F2_H_ */
