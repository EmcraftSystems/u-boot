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
 * STM32 F2 USART driver; configured with the following options:
 * - CONFIG_STM32F2_USART_CONSOLE
 * - CONFIG_STM32F2_USART_PORT       (1..6)
 * - CONFIG_STM32F2_USART_TX_IO_PORT (1..9 <-> A..I)
 * - CONFIG_STM32F2_USART_RX_IO_PORT (1..9 <-> A..I)
 * - CONFIG_STM32F2_USART_TX_IO_PIN  (1..16)
 * - CONFIG_STM32F2_USART_RX_IO_PIN  (1..16)
 */

#include <common.h>

#include <asm/arch/stm32f2.h>

/*
 * Set up configuration
 */
#if (CONFIG_STM32F2_USART_PORT >= 1) && \
    (CONFIG_STM32F2_USART_PORT <= 6)
# define USART_PORT		(CONFIG_STM32F2_USART_PORT - 1)
#else
# error "Bad CONFIG_STM32F2_USART_PORT value."
#endif

#if (CONFIG_STM32F2_USART_TX_IO_PORT >= 1) && \
    (CONFIG_STM32F2_USART_TX_IO_PORT <= 9)
# define USART_TX_IO_PORT	(CONFIG_STM32F2_USART_TX_IO_PORT - 1)
#else
# error "Bad CONFIG_STM32F2_USART_TX_IO_PORT value."
#endif

#if (CONFIG_STM32F2_USART_RX_IO_PORT >= 1) && \
    (CONFIG_STM32F2_USART_RX_IO_PORT <= 9)
# define USART_RX_IO_PORT	(CONFIG_STM32F2_USART_RX_IO_PORT - 1)
#else
# error "Bad CONFIG_STM32F2_USART_RX_IO_PORT value."
#endif

#if (CONFIG_STM32F2_USART_TX_IO_PIN >= 0) && \
    (CONFIG_STM32F2_USART_TX_IO_PIN <= 15)
# define USART_TX_IO_PIN	(CONFIG_STM32F2_USART_TX_IO_PIN)
#else
# error "Bad CONFIG_STM32F2_USART_TX_IO_PIN value."
#endif

#if (CONFIG_STM32F2_USART_RX_IO_PIN >= 0) && \
    (CONFIG_STM32F2_USART_RX_IO_PIN <= 15)
# define USART_RX_IO_PIN	(CONFIG_STM32F2_USART_RX_IO_PIN)
#else
# error "Bad CONFIG_STM32F2_USART_RX_IO_PIN value."
#endif

DECLARE_GLOBAL_DATA_PTR;

/*
 * Register map bases
 */
static const unsigned long usart_base[] = {
	STM32F2_USART1_BASE, STM32F2_USART2_BASE, STM32F2_USART3_BASE,
	STM32F2_USART4_BASE, STM32F2_USART5_BASE, STM32F2_USART6_BASE
};

static const unsigned long io_base[] = {
	STM32F2_GPIOA_BASE, STM32F2_GPIOB_BASE, STM32F2_GPIOC_BASE,
	STM32F2_GPIOD_BASE, STM32F2_GPIOE_BASE, STM32F2_GPIOF_BASE,
	STM32F2_GPIOG_BASE, STM32F2_GPIOH_BASE, STM32F2_GPIOI_BASE
};

/*
 * Regsiter offsets
 */
static const unsigned long rcc_enr_offset[] = {
	STM32F2_RCC_ENR_USART1, STM32F2_RCC_ENR_USART2, STM32F2_RCC_ENR_USART3,
	STM32F2_RCC_ENR_USART4, STM32F2_RCC_ENR_USART5, STM32F2_RCC_ENR_USART6
};

/*
 * Different masks
 */
static const unsigned long rcc_msk[] = {
	STM32F2_RCC_MSK_USART1, STM32F2_RCC_MSK_USART2, STM32F2_RCC_MSK_USART3,
	STM32F2_RCC_MSK_USART4, STM32F2_RCC_MSK_USART5, STM32F2_RCC_MSK_USART6
};

static const unsigned long af_val[] = {
	STM32F2_GPIO_AF_USART1, STM32F2_GPIO_AF_USART2, STM32F2_GPIO_AF_USART3,
	STM32F2_GPIO_AF_USART4, STM32F2_GPIO_AF_USART5, STM32F2_GPIO_AF_USART6
};

/*
 * Hardware resources
 */
static volatile struct stm32f2_usart_regs	*usart_regs;

/*
 * Initialize the serial port.
 */
int serial_init(void)
{
	static volatile u32			 *usart_enr;
	static volatile struct stm32f2_rcc_regs	 *rcc_regs;
	static volatile struct stm32f2_gpio_regs *io_regs[2];
	static u32				 io_ports[2];
	static u32				 io_pins[2];

	u32	i, ofs;

	/*
	 * GPIO params
	 */
	io_ports[0] = USART_TX_IO_PORT;
	io_ports[1] = USART_RX_IO_PORT;
	io_pins[0]  = USART_TX_IO_PIN;
	io_pins[1]  = USART_RX_IO_PIN;

	/*
	 * Setup registers
	 */
	usart_regs  = (struct stm32f2_usart_regs *)usart_base[USART_PORT];
	rcc_regs    = (struct stm32f2_rcc_regs *)STM32F2_RCC_BASE;
	for (i = 0; i < 2; i++)
		io_regs[i] = (struct stm32f2_gpio_regs *)io_base[io_ports[i]];

	usart_enr  = (u32 *)(STM32F2_RCC_BASE + rcc_enr_offset[USART_PORT]);

	/*
	 * Enable GPIO and USART clocks
	 */
	for (i = 0; i < 2; i++)
		rcc_regs->ahb1enr |= 1 << io_ports[i];
	*usart_enr |= rcc_msk[USART_PORT];

	/*
	 * Configure GPIOs
	 */
	for (i = 0; i < 2; i++) {
		/*
		 * Connect PXy to USART Tx/Rx
		 */
		ofs = (io_pins[i] & 0x07) * 4;
		io_regs[i]->afr[io_pins[i] >> 3] &= ~(0xF << ofs);
		io_regs[i]->afr[io_pins[i] >> 3] |= af_val[USART_PORT] << ofs;

		ofs = io_pins[i] * 2;
		/*
		 * Set Alternative function mode
		 */
		io_regs[i]->moder &= ~(0x3 << ofs);
		io_regs[i]->moder |= STM32F2_GPIO_MODE_AF << ofs;

		/*
		 * Output mode configuration
		 */
		io_regs[i]->otyper &= ~(0x3 << ofs);
		io_regs[i]->otyper |= STM32F2_GPIO_OTYPE_PP << ofs;

		/*
		 * Speed mode configuration
		 */
		io_regs[i]->ospeedr &= ~(0x3 << ofs);
		io_regs[i]->ospeedr |= STM32F2_GPIO_SPEED_50M << ofs;

		/*
		 * Pull-up, pull down resistor configuration
		 */
		io_regs[i]->pupdr &= ~(0x3 << ofs);
		io_regs[i]->pupdr |= STM32F2_GPIO_PUPD_UP;
	}

	/*
	 * CR1:
	 * - 1 Start bit, 8 Data bits, n Stop bit
	 * - parity control disabled
	 */
	usart_regs->cr1 = STM32F2_USART_CR1_TE | STM32F2_USART_CR1_RE;

	/*
	 * CR2:
	 * - 1 Stop bit
	 */
	usart_regs->cr2 = 0;

	/*
	 * CR3:
	 * - flow control disabled
	 * - full duplex
	 */
	usart_regs->cr3 = 0;

	/*
	 * Set baudrate
	 */
	serial_setbrg();

	/*
	 * Enable USART
	 */
	usart_regs->cr1 |= STM32F2_USART_CR1_UE;

	return 0;
}

/*
 * Set new baudrate.
 */
void serial_setbrg(void)
{
	u32	apb_clock, int_div, frac_div, tmp;

	if (USART_PORT == 0 || USART_PORT == 5)
		apb_clock = clock_get(CLOCK_PCLK2);
	else
		apb_clock = clock_get(CLOCK_PCLK1);

	/*
	 * Assume oversampling mode of 16 Samples
	 */
	int_div = (25 * apb_clock) / (4 * gd->baudrate);

	tmp = (int_div / 100) << STM32F2_USART_BRR_MANT_BIT;
	frac_div = int_div - (100 * (tmp >> 4));
	tmp |= (((frac_div * 16) + 50) / 100) & STM32F2_USART_BRR_FRAC_MSK;

	usart_regs->brr = tmp;

	return;
}

/*
 * Read a single character from the serial port.
 */
int serial_getc(void)
{
	while (!(usart_regs->sr & STM32F2_USART_SR_RXNE));

	return usart_regs->dr & 0xFF;
}

/*
 * Put a single character to the serial port.
 */
void serial_putc(const char c)
{
	if (c == '\n')
		serial_putc('\r');

	while (!(usart_regs->sr & STM32F2_USART_SR_TXE));

	usart_regs->dr = c;
}

/*
 * Put a string ('\0'-terminated) to the serial port.
 */
void serial_puts(const char *s)
{
	while (*s)
		serial_putc(*s++);
}

/*
 * Test whether a character in in the RX buffer.
 */
int serial_tstc(void)
{
	return (usart_regs->sr & STM32F2_USART_SR_RXNE) ? 1 : 0;
}


