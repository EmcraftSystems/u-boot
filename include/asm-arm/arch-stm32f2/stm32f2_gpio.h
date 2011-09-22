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
 * GPIO roles (alternative functions)
 */
enum stm32f2_gpio_role {
	STM32F2_GPIO_ROLE_USART1,	/* GPIO is used by USART1	      */
	STM32F2_GPIO_ROLE_USART2,	/* GPIO is used by USART2	      */
	STM32F2_GPIO_ROLE_USART3,	/* GPIO is used by USART3	      */
	STM32F2_GPIO_ROLE_USART4,	/* GPIO is used by USART4	      */
	STM32F2_GPIO_ROLE_USART5,	/* GPIO is used by USART5	      */
	STM32F2_GPIO_ROLE_USART6,	/* GPIO is used by USART6	      */
	STM32F2_GPIO_ROLE_ETHERNET	/* GPIO is used by MAC		      */
};

/*
 * Configure the specified GPIO for the specified role
 */
int stm32f2_gpio_config(unsigned int port, unsigned int pin,
			enum stm32f2_gpio_role role);

#endif /* _STM32F2_GPIO_H_ */
