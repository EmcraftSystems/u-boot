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

#ifndef _STM32F2_GPIO_H_
#define _STM32F2_GPIO_H_

/*
 * GPIO ports
 */
enum stm32f2_gpio_port {
	STM32F2_GPIO_PORT_A	= 0,
	STM32F2_GPIO_PORT_B,
	STM32F2_GPIO_PORT_C,
	STM32F2_GPIO_PORT_D,
	STM32F2_GPIO_PORT_E,
	STM32F2_GPIO_PORT_F,
	STM32F2_GPIO_PORT_G,
	STM32F2_GPIO_PORT_H,
	STM32F2_GPIO_PORT_I,
	STM32F2_GPIO_PORT_J,
	STM32F2_GPIO_PORT_K
};

/*
 * GPIO port pins
 */
enum stm32f2_gpio_pin {
	STM32F2_GPIO_PIN_0	= 0,
	STM32F2_GPIO_PIN_1,
	STM32F2_GPIO_PIN_2,
	STM32F2_GPIO_PIN_3,
	STM32F2_GPIO_PIN_4,
	STM32F2_GPIO_PIN_5,
	STM32F2_GPIO_PIN_6,
	STM32F2_GPIO_PIN_7,
	STM32F2_GPIO_PIN_8,
	STM32F2_GPIO_PIN_9,
	STM32F2_GPIO_PIN_10,
	STM32F2_GPIO_PIN_11,
	STM32F2_GPIO_PIN_12,
	STM32F2_GPIO_PIN_13,
	STM32F2_GPIO_PIN_14,
	STM32F2_GPIO_PIN_15
};

/*
 * GPIO roles (alternative functions); role determines by whom GPIO is used
 */
enum stm32f2_gpio_role {
	STM32F2_GPIO_ROLE_USART1 = 0,	/* USART1			      */
	STM32F2_GPIO_ROLE_USART2,	/* USART2			      */
	STM32F2_GPIO_ROLE_USART3,	/* USART3			      */
	STM32F2_GPIO_ROLE_USART4,	/* USART4			      */
	STM32F2_GPIO_ROLE_USART5,	/* USART5			      */
	STM32F2_GPIO_ROLE_USART6,	/* USART6			      */
	STM32F2_GPIO_ROLE_ETHERNET,	/* MAC				      */
	STM32F2_GPIO_ROLE_MCO,		/* MC external output clock	      */
	STM32F2_GPIO_ROLE_LTDC,		/* LCD controller		      */
	STM32F2_GPIO_ROLE_FSMC,		/* FSMC static memory controller      */
	STM32F2_GPIO_ROLE_FMC = STM32F2_GPIO_ROLE_FSMC,
	STM32F2_GPIO_ROLE_GPOUT,	/* GPOUT			      */
	STM32F2_GPIO_ROLE_QSPI_AF9,
	STM32F2_GPIO_ROLE_QSPI_AF10,
	STM32F2_GPIO_ROLE_GPIN,
	STM32F2_GPIO_ROLE_GPIN_PULLUP,

	STM32F2_GPIO_ROLE_LAST		/* for internal usage, must be last   */
};

/*
 * GPIO descriptor
 */
struct stm32f2_gpio_dsc {
	enum stm32f2_gpio_port	port;	/* GPIO port			      */
	enum stm32f2_gpio_pin	pin;	/* GPIO pin			      */
};

/*
 * Configure the specified GPIO for the specified role
 */
int stm32f2_gpio_config(const struct stm32f2_gpio_dsc *gpio_dsc,
			enum stm32f2_gpio_role role);

/*
 * Set GPOUT to the state specified (1, 0)
 */
int stm32f2_gpout_set(const struct stm32f2_gpio_dsc *gpio_dsc, int state);
int stm32f2_gpout_get(const struct stm32f2_gpio_dsc *dsc);

#endif /* _STM32F2_GPIO_H_ */
