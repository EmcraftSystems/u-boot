/*
 * (C) Copyright 2012
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

#include <common.h>
#include <errno.h>
#include "envm.h"

/*
 * Initialize internal Flash interface
 *
 * This function should not be in .ramcode, because it will be called only once
 * before self-upgrade.
 */
void envm_init(void)
{
}

/*
 * Write a data buffer to internal Flash.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in internal Flash.
 */
u32
#ifdef CONFIG_ARMCORTEXM3_RAMCODE
	__attribute__((section(".ramcode")))
	__attribute__((long_call))
#endif
	envm_write(u32 offset, void *buf, u32 size)
{
	printf("Error: Internal flash is not supported "
	       "on LPC18xx and LPC43xx.\n");
	return -ENODEV;
}
