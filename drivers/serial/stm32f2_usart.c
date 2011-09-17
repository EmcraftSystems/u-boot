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
 * STM32 F2 USART driver
 */

#include <common.h>

/*
 * Initialize the serial port.
 */
int serial_init(void)
{
	/*
	 * TBD
	 */

	return 0;
}

/*
 * Set new baudrate.
 */
void serial_setbrg(void)
{
	/*
	 * TBD
	 */

	return;
}

/*
 * Read a single character from the serial port.
 */
int serial_getc(void)
{
	/*
	 * TBD
	 */

	return 0;
}

/*
 * Put a single character to the serial port.
 */
void serial_putc(const char c)
{
	/*
	 * TBD
	 */

	return;
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
	/*
	 * TBD
	 */

	return 0;
}


