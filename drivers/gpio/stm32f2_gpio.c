/*
 * (C) Copyright 2011-2015
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
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
 * STM32 F2 GPIO driver. Used for instance for setting Alternative functions
 * for GPIOs utilized for USART or Ethernet communications
 */

#include <common.h>
#include <asm/errno.h>

#include <asm/arch/stm32.h>
#include <asm/arch/stm32f2_gpio.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * GPIO registers bases
 */
#define STM32F2_GPIOA_BASE	(STM32_AHB1PERIPH_BASE + 0x0000)
#define STM32F2_GPIOB_BASE	(STM32_AHB1PERIPH_BASE + 0x0400)
#define STM32F2_GPIOC_BASE	(STM32_AHB1PERIPH_BASE + 0x0800)
#define STM32F2_GPIOD_BASE	(STM32_AHB1PERIPH_BASE + 0x0C00)
#define STM32F2_GPIOE_BASE	(STM32_AHB1PERIPH_BASE + 0x1000)
#define STM32F2_GPIOF_BASE	(STM32_AHB1PERIPH_BASE + 0x1400)
#define STM32F2_GPIOG_BASE	(STM32_AHB1PERIPH_BASE + 0x1800)
#define STM32F2_GPIOH_BASE	(STM32_AHB1PERIPH_BASE + 0x1C00)
#define STM32F2_GPIOI_BASE	(STM32_AHB1PERIPH_BASE + 0x2000)
#define STM32F4_GPIOJ_BASE	(STM32_AHB1PERIPH_BASE + 0x2400)
#define STM32F4_GPIOK_BASE	(STM32_AHB1PERIPH_BASE + 0x2800)

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

/*
 * AF11 selection
 */
#define STM32F2_GPIO_AF_MAC	0x0B

/*
 * AF12 selection
 */
#define STM32F2_GPIO_AF_FSMC	0x0C

/*
 * LTDC AF
 */
#define STM32F2_GPIO_AF_LTDC   0x0E

#define STM32F2_GPIO_AF_QSPI9	0x09
#define STM32F2_GPIO_AF_QSPI10	0x0A

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
	u32	bsrr;		/* GPIO port bit set/reset		      */
	u32	lckr;		/* GPIO port configuration lock		      */
	u32	afr[2];		/* GPIO alternate function		      */
};

/*
 * Register map bases
 */
static const unsigned long io_base[] = {
	STM32F2_GPIOA_BASE, STM32F2_GPIOB_BASE, STM32F2_GPIOC_BASE,
	STM32F2_GPIOD_BASE, STM32F2_GPIOE_BASE, STM32F2_GPIOF_BASE,
	STM32F2_GPIOG_BASE, STM32F2_GPIOH_BASE, STM32F2_GPIOI_BASE,
	STM32F4_GPIOJ_BASE, STM32F4_GPIOK_BASE,
};

/*
 * AF values (note, indexed by enum stm32f2_gpio_role)
 */
static const u32 af_val[STM32F2_GPIO_ROLE_LAST] = {
	STM32F2_GPIO_AF_USART1, STM32F2_GPIO_AF_USART2, STM32F2_GPIO_AF_USART3,
	STM32F2_GPIO_AF_USART4, STM32F2_GPIO_AF_USART5, STM32F2_GPIO_AF_USART6,
	STM32F2_GPIO_AF_MAC,
	(u32)-1,
	STM32F2_GPIO_AF_LTDC,
	STM32F2_GPIO_AF_FSMC,
	(u32)-1,
	STM32F2_GPIO_AF_QSPI9,	/* STM32F2_GPIO_ROLE_QSPI_AF9 */
	STM32F2_GPIO_AF_QSPI10,	/* STM32F2_GPIO_ROLE_QSPI_AF10 */
	STM32F2_GPIO_MODE_IN,
	STM32F2_GPIO_MODE_IN,
};

/*
 * Configure the specified GPIO for the specified role
 * Returns 0 on success, -EINVAL otherwise.
 */
s32 stm32f2_gpio_config(const struct stm32f2_gpio_dsc *dsc,
			enum stm32f2_gpio_role role)
{
	volatile struct stm32f2_gpio_regs	*gpio_regs;

	u32	otype, ospeed, pupd, mode, i;
	s32	rv;

	/*
	 * Check params
	 */
	if (!dsc || dsc->port >= ARRAY_SIZE(io_base) || dsc->pin > 15) {
		if (gd->have_console) {
			printf("%s: incorrect params %d.%d.\n", __func__,
				dsc ? dsc->port : -1,
				dsc ? dsc->pin  : -1);
		}
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Depending on the role, select the appropriate io params
	 */
	switch (role) {
	case STM32F2_GPIO_ROLE_USART1:
	case STM32F2_GPIO_ROLE_USART2:
	case STM32F2_GPIO_ROLE_USART3:
	case STM32F2_GPIO_ROLE_USART4:
	case STM32F2_GPIO_ROLE_USART5:
	case STM32F2_GPIO_ROLE_USART6:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_50M;
		pupd   = STM32F2_GPIO_PUPD_UP;
		mode   = STM32F2_GPIO_MODE_AF;
		break;
	case STM32F2_GPIO_ROLE_ETHERNET:
	case STM32F2_GPIO_ROLE_MCO:
	case STM32F2_GPIO_ROLE_FSMC:
	case STM32F2_GPIO_ROLE_QSPI_AF9:
	case STM32F2_GPIO_ROLE_QSPI_AF10:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_100M;
		pupd   = STM32F2_GPIO_PUPD_NO;
		mode   = STM32F2_GPIO_MODE_AF;
		break;
	case STM32F2_GPIO_ROLE_GPOUT:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_50M;
		pupd   = STM32F2_GPIO_PUPD_NO;
		mode   = STM32F2_GPIO_MODE_OUT;
		break;
	case STM32F2_GPIO_ROLE_LTDC:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_50M;
		pupd   = STM32F2_GPIO_PUPD_NO;
		mode   = STM32F2_GPIO_MODE_AF;
		break;
	case STM32F2_GPIO_ROLE_GPIN:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_2M;
		pupd   = STM32F2_GPIO_PUPD_NO;
		mode   = STM32F2_GPIO_MODE_IN;
		break;
	case STM32F2_GPIO_ROLE_GPIN_PULLUP:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_2M;
		pupd   = STM32F2_GPIO_PUPD_UP;
		mode   = STM32F2_GPIO_MODE_IN;
		break;
	default:
		if (gd->have_console)
			printf("%s: incorrect role %d.\n", __func__, role);
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Get reg base
	 */
	gpio_regs = (struct stm32f2_gpio_regs *)io_base[dsc->port];

	/*
	 * Enable GPIO clocks
	 */
	STM32_RCC->ahb1enr |= 1 << dsc->port;

	if (af_val[role] != (u32)-1) {
		/*
		 * Connect PXy to the specified controller (role)
		 */
		i = (dsc->pin & 0x07) * 4;
		gpio_regs->afr[dsc->pin >> 3] &= ~(0xF << i);
		gpio_regs->afr[dsc->pin >> 3] |= af_val[role] << i;
	}

	i = dsc->pin;

	/*
	 * Output mode configuration
	 */
	gpio_regs->otyper &= ~(0x1 << i);
	gpio_regs->otyper |= otype << i;

	i = dsc->pin * 2;

	/*
	 * Set Alternative function mode
	 */
	gpio_regs->moder &= ~(0x3 << i);
	gpio_regs->moder |= mode << i;

	/*
	 * Speed mode configuration
	 */
	gpio_regs->ospeedr &= ~(0x3 << i);
	gpio_regs->ospeedr |= ospeed << i;

	/*
	 * Pull-up, pull-down resistor configuration
	 */
	gpio_regs->pupdr &= ~(0x3 << i);
	gpio_regs->pupdr |= pupd << i;

	rv = 0;
out:
	return rv;
}

/*
 * Set GPOUT to the state specified (1, 0).
 * Returns 0 on success, -EINVAL otherwise.
 */
s32 stm32f2_gpout_set(const struct stm32f2_gpio_dsc *dsc, int state)
{
	volatile struct stm32f2_gpio_regs	*gpio_regs;
	s32					rv;

	if (!dsc || dsc->port >= ARRAY_SIZE(io_base) || dsc->pin > 15) {
		if (gd->have_console) {
			printf("%s: incorrect params %d.%d.\n", __func__,
				dsc ? dsc->port : -1,
				dsc ? dsc->pin  : -1);
		}
		rv = -EINVAL;
		goto out;
	}

	gpio_regs = (struct stm32f2_gpio_regs *)io_base[dsc->port];

	if (state) {
		/* Set */
		gpio_regs->bsrr = 1 << dsc->pin;
	} else {
		/* Reset */
		gpio_regs->bsrr = 1 << (dsc->pin + 16);
	}
	rv = 0;
out:
	return rv;
}

int stm32f2_gpout_get(const struct stm32f2_gpio_dsc *dsc)
{
	volatile struct stm32f2_gpio_regs *gpio_regs;
	int state;

	if (!dsc || dsc->port >= ARRAY_SIZE(io_base) || dsc->pin > 15) {
		if (gd->have_console) {
			printf("%s: incorrect params %d.%d.\n", __func__,
				dsc ? dsc->port : -1,
				dsc ? dsc->pin  : -1);
		}
		return -EINVAL;
	}

	gpio_regs = (struct stm32f2_gpio_regs *)io_base[dsc->port];

	state = !!(gpio_regs->idr & (1 << dsc->pin));

	return state;
}
