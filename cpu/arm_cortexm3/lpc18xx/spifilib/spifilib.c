/*
 * (C) Copyright 2013
 *
 * Dmitry Konyshev, Emcraft Systems, probables@emcraft.com
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

#include "spifilib.h"

int32_t cancel_mem_mode (SPIFIobj *obj);

const spifilib_header_t spilib_hdr __attribute__((section(".rodata.spifilib.header"))) =
{
	SPIFILIB_SIG,
	(spifilib_header_t *)&spilib_hdr,
	SPIFILIB_SIZE,
	spifi_init,
	spifi_program,
	spifi_erase,
	cancel_mem_mode
};

void *__aeabi_memcpy4(void *dest, const void *src, int n)
{
	int i;

	for (i = 0; i < n; i++) {
		*((char *)dest + i) = *((char *)src + i);
	}

	return dest;
}

void _start(void)
{
}
