/*
 * board/emcraft/a2f-lnx-evb/board.c
 *
 * Board specific code the the Emcraft A2F-LNX-EVB board.
 *
 * Copyright (C) 2010 Vladimir Khusainov, Emcraft Systems
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
#include <asm-arm/arch-a2f/a2f.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	return 0;
}

int dram_init (void)
{
#if ( CONFIG_NR_DRAM_BANKS > 0 )
	/*
	 * EMC timing parameters for chip select 0
	 */
        A2F_SYSREG->emc_cs_0_cr = CONFIG_SYS_EMC0CS0CR;


        gd->bd->bi_dram[0].start = EXT_RAM_BASE;
        gd->bd->bi_dram[0].size = EXT_RAM_SIZE;
#endif

        return 0;
}

int misc_init_r(void)
{

	return 0;
}

int board_eth_init(bd_t *bis)
{
	return 0;
}
