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

#ifndef __SPIFI_H
#define __SPIFI_H

static inline int spifi_addr(unsigned long addr)
{
	return addr >= CONFIG_SPIFI_BASE &&
		addr < CONFIG_SPIFI_BASE + CONFIG_SPIFI_SIZE;
}

extern int spifi_initialize(void);
extern int spifi_write(ulong off, const void *buf, ulong size);
extern void spifi_cancel_mem_mode(void);

#endif
