/*
 * board/emcraft/sf2-dev-kit/board.c
 *
 * Board specific code for the Microsemi SmartFusion2 Development Kit.
 *
 * (C) Copyright 2012-2013
 * Emcraft Systems, <www.emcraft.com>
 * Alexander Potashev <aspotashev@emcraft.com>
 * Vladimir Khusainov <vlad@emcraft.com>
 * Vladimir Skvortsov <vskvortsov@emcraft.com>
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
#include <netdev.h>
#include <asm/arch/ddr.h>

DECLARE_GLOBAL_DATA_PTR;

void dummy_func(void)
{
	return;
}

int board_init(void)
{
	return 0;
}

int checkboard(void)
{
	printf("Board: SF2-DEV-KIT Rev %s, www.emcraft.com\n",
		CONFIG_SYS_BOARD_REV_STR);
	return 0;
}

/*
 * Initialize DRAM
 */
int dram_init (void)
{
	return 0;
}

int misc_init_r(void)
{
	return 0;
}

#ifdef CONFIG_M2S_ETH
int board_eth_init(bd_t *bis)
{
	return m2s_eth_driver_init(bis);
}
#endif
