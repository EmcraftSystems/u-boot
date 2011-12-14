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

#include <common.h>

#include <asm/arch/lpc178x.h>

/*
 * Enable or disable power on a peripheral device (timers, UARTs, USB, etc)
 *
 * This function will be called from `lpc178x_phy_final_reset()`, therefore this
 * should also be in `.ramcode`.
 */
void __attribute__((section(".ramcode")))
     __attribute__((long_call))
lpc178x_periph_enable(u32 pconp_mask, int enable)
{
	if (enable)
		LPC178X_SCC->pconp |= pconp_mask;
	else
		LPC178X_SCC->pconp &= ~pconp_mask;
}
