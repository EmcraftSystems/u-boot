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
 * STM32 F2 GPIO driver. Used just for setting Alternative functions
 * for GPIOs utilized for USART or Ethernet communications
 */

#include <common.h>

#include <asm/arch/stm32f2.h>

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

/*
 * AF11 selection
 */
#define STM32F2_GPIO_AF_MAC	0x0B

/*
 * Register map bases
 */
static const unsigned long io_base[] = {
	STM32F2_GPIOA_BASE, STM32F2_GPIOB_BASE, STM32F2_GPIOC_BASE,
	STM32F2_GPIOD_BASE, STM32F2_GPIOE_BASE, STM32F2_GPIOF_BASE,
	STM32F2_GPIOG_BASE, STM32F2_GPIOH_BASE, STM32F2_GPIOI_BASE
};

/*
 * AF values (note, indexed by enum stm32f2_gpio_role)
 */
static const u32 af_val[] = {
	STM32F2_GPIO_AF_USART1, STM32F2_GPIO_AF_USART2, STM32F2_GPIO_AF_USART3,
	STM32F2_GPIO_AF_USART4, STM32F2_GPIO_AF_USART5, STM32F2_GPIO_AF_USART6,
	STM32F2_GPIO_AF_MAC
};

/*
 * Configure the specified GPIO for the specified role
 */
int stm32f2_gpio_config(unsigned int port, unsigned int pin,
			enum stm32f2_gpio_role role)
{
	volatile struct stm32f2_gpio_regs	*gpio_regs;
	volatile struct stm32f2_rcc_regs	*rcc_regs;

	u32	otype, ospeed, pupd, i;
	int	rv;

	/*
	 * Check params
	 */
	if (port > 8 || pin > 15) {
		printf("%s: bad params %d.%d.\n", __func__, port, pin);
		rv = -1;
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
		break;
	case STM32F2_GPIO_ROLE_ETHERNET:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_100M;
		pupd   = STM32F2_GPIO_PUPD_NO;
		break;
	default:
		printf("%s: bad role %d.\n", __func__, role);
		rv = -1;
		goto out;
	}

	/*
	 * Get reg base
	 */
	rcc_regs  = (struct stm32f2_rcc_regs *)STM32F2_RCC_BASE;
	gpio_regs = (struct stm32f2_gpio_regs *)io_base[port];

	/*
	 * Enable GPIO clocks
	 */
	rcc_regs->ahb1enr |= 1 << port;

	/*
	 * Connect PXy to the specified controller (role)
	 */
	i = (pin & 0x07) * 4;
	gpio_regs->afr[pin >> 3] &= ~(0xF << i);
	gpio_regs->afr[pin >> 3] |= af_val[role] << i;

	i = pin * 2;

	/*
	 * Set Alternative function mode
	 */
	gpio_regs->moder &= ~(0x3 << i);
	gpio_regs->moder |= STM32F2_GPIO_MODE_AF << i;

	/*
	 * Output mode configuration
	 */
	gpio_regs->otyper &= ~(0x3 << i);
	gpio_regs->otyper |= otype << i;

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

