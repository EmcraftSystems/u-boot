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

#include <asm/arch/kinetis.h>

#if defined(CONFIG_CMD_NET)
/*
 * This function is called by the `mcffec.c` driver with setclear=1
 * on FEC initialization, and with setclear=0 on FEC halt.
 *
 * Someone might think that we could put the clock gate enable/disable code
 * for Ethernet here. But this thought is wrong, because the Ethernet module
 * clocking must be enabled even when resetting the Ethernet module (otherwise
 * we get an exception.) In the `fec_halt()` function, a MAC reset may be
 * attempted before calling `fecpin_setclear(dev, 1)`, therefore the clock for
 * the Ethernet module must be enabled somewhere else.
 * We do this in `cortex_m3_soc_init()`.
 */
int fecpin_setclear(struct eth_device *dev, int setclear)
{
	return 0;
}
#endif /* CONFIG_CMD_NET */
