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
 * - CONFIG_STM32F2_USART_TX_IO_PORT (0..8 <-> A..I)
 * - CONFIG_STM32F2_USART_RX_IO_PORT (0..8 <-> A..I)
 * - CONFIG_STM32F2_USART_TX_IO_PIN  (0..15)
 * - CONFIG_STM32F2_USART_RX_IO_PIN  (0..15)
 */

#include <common.h>

#include <asm/arch/stm32f2.h>
#include <asm/arch/stm32f2_gpio.h>

/*
 * Set up configuration
 */
#if (CONFIG_STM32F2_USART_PORT >= 1) && \
    (CONFIG_STM32F2_USART_PORT <= 6)
# define USART_PORT		(CONFIG_STM32F2_USART_PORT - 1)
#else
# error "Bad CONFIG_STM32F2_USART_PORT value."
#endif

#if (CONFIG_STM32F2_USART_TX_IO_PORT >= 0) && \
    (CONFIG_STM32F2_USART_TX_IO_PORT <= 8)
# define USART_TX_IO_PORT	(CONFIG_STM32F2_USART_TX_IO_PORT)
#else
# error "Bad CONFIG_STM32F2_USART_TX_IO_PORT value."
#endif

#if (CONFIG_STM32F2_USART_RX_IO_PORT >= 0) && \
    (CONFIG_STM32F2_USART_RX_IO_PORT <= 8)
# define USART_RX_IO_PORT	(CONFIG_STM32F2_USART_RX_IO_PORT)
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

/*
 * STM32F USART definitions
 */
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
#define STM32F2_USART_SR_TXE	(1 << 7)	/* Transmit data reg empty   */
#define STM32F2_USART_SR_RXNE	(1 << 5)	/* Read data reg not empty   */

/*
 * BRR reg fields
 */
#define STM32F2_USART_BRR_F_BIT	0		/* fraction of USARTDIV	     */
#define STM32F2_USART_BRR_F_MSK	0x0F

#define STM32F2_USART_BRR_M_BIT	4		/* mantissa of USARTDIV	     */
#define STM32F2_USART_BRR_M_MSK	0xFFF

/*
 * CR1 bit masks
 */
#define STM32F2_USART_CR1_UE	(1 << 13)	/* USART enable		     */
#define STM32F2_USART_CR1_TE	(1 <<  3)	/* Transmitter enable	     */
#define STM32F2_USART_CR1_RE	(1 <<  2)	/* Receiver enable	     */

/*
 * STM32F2 RCC USART specific definitions
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

/*
 * U-Boot global data to get the baudrate from
 */
DECLARE_GLOBAL_DATA_PTR;

/*
 * Register map bases
 */
static const unsigned long usart_base[] = {
	STM32F2_USART1_BASE, STM32F2_USART2_BASE, STM32F2_USART3_BASE,
	STM32F2_USART4_BASE, STM32F2_USART5_BASE, STM32F2_USART6_BASE
};

/*
 * Register offsets
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

/*
 * GPIO roles
 */
static const enum stm32f2_gpio_role gpio_role[] = {
	STM32F2_GPIO_ROLE_USART1, STM32F2_GPIO_ROLE_USART2,
	STM32F2_GPIO_ROLE_USART3, STM32F2_GPIO_ROLE_USART4,
	STM32F2_GPIO_ROLE_USART5, STM32F2_GPIO_ROLE_USART6
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

	/*
	 * Setup registers
	 */
	usart_regs = (struct stm32f2_usart_regs *)usart_base[USART_PORT];
	rcc_regs   = (struct stm32f2_rcc_regs *)STM32F2_RCC_BASE;

	usart_enr  = (u32 *)(STM32F2_RCC_BASE + rcc_enr_offset[USART_PORT]);

	/*
	 * Enable USART clocks
	 */
	*usart_enr |= rcc_msk[USART_PORT];

	/*
	 * Configure GPIOs
	 */
	stm32f2_gpio_config(USART_TX_IO_PORT, USART_TX_IO_PIN,
			    gpio_role[USART_PORT]);
	stm32f2_gpio_config(USART_RX_IO_PORT, USART_RX_IO_PIN,
			    gpio_role[USART_PORT]);

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

	tmp = (int_div / 100) << STM32F2_USART_BRR_M_BIT;
	frac_div = int_div - (100 * (tmp >> 4));
	tmp |= (((frac_div * 16) + 50) / 100) & STM32F2_USART_BRR_F_MSK;

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


