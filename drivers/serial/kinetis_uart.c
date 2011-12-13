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

/*
 * Freescale Kinetis UART driver
 *
 * Configured with the following options:
 *    CONFIG_KINETIS_UART_CONSOLE
 *    CONFIG_KINETIS_UART_PORT       (0..5)
 *    CONFIG_KINETIS_UART_TX_IO_PORT (0..4 <-> A..E)
 *    CONFIG_KINETIS_UART_TX_IO_PIN  (0..31)
 *    CONFIG_KINETIS_UART_TX_IO_FUNC (0..7, see the Pinout table)
 *    CONFIG_KINETIS_UART_RX_IO_PORT (0..4 <-> A..E)
 *    CONFIG_KINETIS_UART_RX_IO_PIN  (0..31)
 *    CONFIG_KINETIS_UART_RX_IO_FUNC (0..7, see the Pinout table)
 */

#include <common.h>

#include <asm/arch/kinetis.h>
#include <asm/arch/kinetis_gpio.h>

/*
 * Verify CONFIG_KINETIS_UART_PORT
 */
#if CONFIG_KINETIS_UART_PORT < 0 || CONFIG_KINETIS_UART_PORT > 5
#error CONFIG_KINETIS_UART_PORT should be between 0 and 5
#endif

/*
 * Verify CONFIG_KINETIS_UART_TX_IO_PORT
 */
#if CONFIG_KINETIS_UART_TX_IO_PORT < 0 || CONFIG_KINETIS_UART_TX_IO_PORT > 4
#error CONFIG_KINETIS_UART_TX_IO_PORT should be between 0 and 4
#endif

/*
 * Verify CONFIG_KINETIS_UART_TX_IO_PIN
 */
#if CONFIG_KINETIS_UART_TX_IO_PIN < 0 || CONFIG_KINETIS_UART_TX_IO_PIN > 31
#error CONFIG_KINETIS_UART_TX_IO_PIN should be between 0 and 31
#endif

/*
 * Verify CONFIG_KINETIS_UART_TX_IO_FUNC
 */
#if CONFIG_KINETIS_UART_TX_IO_FUNC < 0 || CONFIG_KINETIS_UART_TX_IO_FUNC > 7
#error CONFIG_KINETIS_UART_TX_IO_FUNC should be between 0 and 7
#endif

/*
 * Verify CONFIG_KINETIS_UART_RX_IO_PORT
 */
#if CONFIG_KINETIS_UART_RX_IO_PORT < 0 || CONFIG_KINETIS_UART_RX_IO_PORT > 4
#error CONFIG_KINETIS_UART_RX_IO_PORT should be between 0 and 4
#endif

/*
 * Verify CONFIG_KINETIS_UART_RX_IO_PIN
 */
#if CONFIG_KINETIS_UART_RX_IO_PIN < 0 || CONFIG_KINETIS_UART_RX_IO_PIN > 31
#error CONFIG_KINETIS_UART_RX_IO_PIN should be between 0 and 31
#endif

/*
 * Verify CONFIG_KINETIS_UART_RX_IO_FUNC
 */
#if CONFIG_KINETIS_UART_RX_IO_FUNC < 0 || CONFIG_KINETIS_UART_RX_IO_FUNC > 7
#error CONFIG_KINETIS_UART_RX_IO_FUNC should be between 0 and 7
#endif

/*
 * Distrubution of the baudrate divisor value over the BDH/BDL registers and
 * the C4[BRFA] bit field.
 */
/* C4[BRFA] (Baud Rate Fine Adjust) */
#define KINETIS_UART_BRFA_BITWIDTH	5
#define KINETIS_UART_BRFA_BITWIDTH_MSK	((1 << KINETIS_UART_BRFA_BITWIDTH) - 1)
/* BDL (Baud Rate Registers: Low) */
#define KINETIS_UART_BDL_BITWIDTH	8
#define KINETIS_UART_BDL_BITWIDTH_MSK	((1 << KINETIS_UART_BDL_BITWIDTH) - 1)
/* BDH (Baud Rate Registers: High) */
#define KINETIS_UART_BDH_BITWIDTH	5
#define KINETIS_UART_BDH_BITWIDTH_MSK	((1 << KINETIS_UART_BDH_BITWIDTH) - 1)

/*
 * UART registers
 */
/*
 * UART Baud Rate Registers: High
 */
#define KINETIS_UART_BDH_SBR_BITS	0
#define KINETIS_UART_BDH_SBR_MSK	(KINETIS_UART_BDH_BITWIDTH_MSK << \
					KINETIS_UART_BDH_SBR_BITS)

/*
 * UART Baud Rate Registers: Low
 */
#define KINETIS_UART_BDL_SBR_BITS	0

/*
 * UART Status Register 1
 */
/* Receive Data Register Full Flag */
#define KINETIS_UART_S1_RDRF_MSK	(1 << 5)
/* Transmit Data Register Empty Flag */
#define KINETIS_UART_S1_TDRE_MSK	(1 << 7)

/*
 * UART Control Register 2
 */
/* Receiver Enable */
#define KINETIS_UART_C2_RE_MSK	(1 << 2)
/* Transmitter Enable */
#define KINETIS_UART_C2_TE_MSK	(1 << 3)

/*
 * UART Control Register 4
 */
/* Baud Rate Fine Adjust */
#define KINETIS_UART_C4_BRFA_BITS	0
#define KINETIS_UART_C4_BRFA_MSK	(KINETIS_UART_BRFA_BITWIDTH_MSK << \
					KINETIS_UART_C4_BRFA_BITS)

/*
 * UART registers bases
 */
#define KINETIS_UART0_BASE	(KINETIS_AIPS0PERIPH_BASE + 0x0006A000)
#define KINETIS_UART1_BASE	(KINETIS_AIPS0PERIPH_BASE + 0x0006B000)
#define KINETIS_UART2_BASE	(KINETIS_AIPS0PERIPH_BASE + 0x0006C000)
#define KINETIS_UART3_BASE	(KINETIS_AIPS0PERIPH_BASE + 0x0006D000)
#define KINETIS_UART4_BASE	(KINETIS_AIPS1PERIPH_BASE + 0x0006A000)
#define KINETIS_UART5_BASE	(KINETIS_AIPS1PERIPH_BASE + 0x0006B000)

/*
 * UART register map
 */
struct kinetis_uart_regs {
	u8 bdh;		/* Baud Rate Registers: High */
	u8 bdl;		/* Baud Rate Registers: Low */
	u8 c1;		/* Control Register 1 */
	u8 c2;		/* Control Register 2 */
	u8 s1;		/* Status Register 1 */
	u8 s2;		/* Status Register 2 */
	u8 c3;		/* Control Register 3 */
	u8 d;		/* Data Register */
	u8 m1;		/* Match Address Registers 1 */
	u8 m2;		/* Match Address Registers 2 */
	u8 c4;		/* Control Register 4 */
	u8 c5;		/* Control Register 5 */
	u8 ed;		/* Extended Data Register */
	u8 modem;	/* Modem Register */
	u8 ir;		/* Infrared Register */
	u8 rsv0;
	u8 pfifo;	/* FIFO Parameters */
	u8 cfifo;	/* FIFO Control Register */
	u8 sfifo;	/* FIFO Status Register */
	u8 twfifo;	/* FIFO Transmit Watermark */
	u8 tcfifo;	/* FIFO Transmit Count */
	u8 rwfifo;	/* FIFO Receive Watermark */
	u8 rcfifo;	/* FIFO Receive Count */
	u8 rsv1;
	u8 c7816;	/* 7816 Control Register */
	u8 ie7816;	/* 7816 Interrupt Enable Register */
	u8 is7816;	/* 7816 Interrupt Status Register */
	u8 wp7816t;	/* 7816 Wait Parameter Register */
	u8 wn7816;	/* 7816 Wait N Register */
	u8 wf7816;	/* 7816 Wait FD Register */
	u8 et7816;	/* 7816 Error Threshold Register */
	u8 tl7816;	/* 7816 Transmit Length Register */
};

/*
 * U-Boot global data to get the baudrate from
 */
DECLARE_GLOBAL_DATA_PTR;

/*
 * UART registers bases
 */
static const u32 uart_base[] = {
	KINETIS_UART0_BASE, KINETIS_UART1_BASE, KINETIS_UART2_BASE,
	KINETIS_UART3_BASE, KINETIS_UART4_BASE, KINETIS_UART5_BASE
};

/*
 * Clock gates for UARTs. These values can be passed
 * into the `kinetis_periph_enable()` function.
 */
static const kinetis_clock_gate_t uart_clock_gate[] = {
	KINETIS_CG_UART0, KINETIS_CG_UART1, KINETIS_CG_UART2,
	KINETIS_CG_UART3, KINETIS_CG_UART4, KINETIS_CG_UART5
};

/*
 * Hardware resources
 */
static volatile struct kinetis_uart_regs *uart_regs;

/*
 * Initialize the serial port.
 */
int serial_init(void)
{
	static struct kinetis_gpio_dsc tx_gpio = {
		CONFIG_KINETIS_UART_TX_IO_PORT,
		CONFIG_KINETIS_UART_TX_IO_PIN};
	static struct kinetis_gpio_dsc rx_gpio = {
		CONFIG_KINETIS_UART_RX_IO_PORT,
		CONFIG_KINETIS_UART_RX_IO_PIN};

	int rv;

	/*
	 * Setup register map
	 */
	uart_regs = (volatile struct kinetis_uart_regs *)
		uart_base[CONFIG_KINETIS_UART_PORT];

	/*
	 * Enable clocks on the I/O ports and the UART used
	 */
	rv = kinetis_periph_enable(uart_clock_gate[CONFIG_KINETIS_UART_PORT], 1);
	if (rv != 0)
		goto out;

	/*
	 * Configure GPIO
	 *
	 * The clocks on the necessary ports will be enabled automatically.
	 */
	rv = kinetis_gpio_config(&tx_gpio,
		KINETIS_GPIO_CONFIG_MUX(CONFIG_KINETIS_UART_TX_IO_FUNC));
	if (rv != 0)
		goto out;
	rv = kinetis_gpio_config(&rx_gpio,
		KINETIS_GPIO_CONFIG_MUX(CONFIG_KINETIS_UART_RX_IO_FUNC));
	if (rv != 0)
		goto out;

	/*
	 * CR1:
	 * - Start, 8 Data bits, Stop
	 * - parity control disabled
	 */
	uart_regs->c1 = 0;

	/*
	 * Set baudrate
	 */
	serial_setbrg();

	/*
	 * Enable receiver and transmitter
	 */
	uart_regs->c2 = KINETIS_UART_C2_RE_MSK | KINETIS_UART_C2_TE_MSK;

	rv = 0;
out:
	return rv;
}

/*
 * Set new baudrate.
 */
void serial_setbrg(void)
{
	u32 br_div;

	/*
	 * Configure baudrate
	 *
	 * UART0 and UART1 are clocked from the core clock, the remaining UARTs
	 * are clocked from the bus clock. The maximum baud rate is 1/16
	 * of the related source clock frequency.
	 *
	 *   32*SBR + BRFD = 2*clk / baudrate
	 */
	br_div = 2 * (CONFIG_KINETIS_UART_PORT <= 1 ?
		clock_get(CLOCK_CCLK) : clock_get(CLOCK_PCLK)) / gd->baudrate;

	/*
	 * Baudrate fine adjust
	 */
	uart_regs->c4 =
		(uart_regs->c4 & ~KINETIS_UART_C4_BRFA_MSK) |
		((br_div & KINETIS_UART_BRFA_BITWIDTH_MSK) <<
		KINETIS_UART_C4_BRFA_BITS);
	/*
	 * Changes in BDH will not take effect until we write into BDL.
	 * Therefore we have to write into BDH first, and after that into BDL.
	 */
	uart_regs->bdh =
		(uart_regs->bdh & ~KINETIS_UART_BDH_SBR_MSK) |
		(((br_div >>
			(KINETIS_UART_BRFA_BITWIDTH +
			KINETIS_UART_BDL_BITWIDTH)) &
		KINETIS_UART_BDH_BITWIDTH_MSK) << KINETIS_UART_BDH_SBR_BITS);
	uart_regs->bdl =
		((br_div >> KINETIS_UART_BRFA_BITWIDTH) & KINETIS_UART_BDL_BITWIDTH_MSK) << KINETIS_UART_BDL_SBR_BITS;
}

/*
 * Read a single character from the serial port.
 */
int serial_getc(void)
{
	while (!(uart_regs->s1 & KINETIS_UART_S1_RDRF_MSK));

	return uart_regs->d;
}

/*
 * Put a single character to the serial port.
 */
void serial_putc(const char c)
{
	if (c == '\n')
		serial_putc('\r');

	while (!(uart_regs->s1 & KINETIS_UART_S1_TDRE_MSK));

	uart_regs->d = c;
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
	return (uart_regs->s1 & KINETIS_UART_S1_RDRF_MSK) ? 1 : 0;
}

