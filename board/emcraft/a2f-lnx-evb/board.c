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

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	return 0;
}

int checkboard(void)
{
	printf("Board: A2F-LNX-EVB Rev %s, www.emcraft.com\n",
		CONFIG_SYS_BOARD_REV_STR);
	return 0;
}

int dram_init (void)
{
#if ( CONFIG_NR_DRAM_BANKS > 0 )
	/*
	 * External memory controller MUX configuration
	 * The EMC _SEL bit in the EMC_MUX_CR register is used
	 * to select either FPGA I/O or EMC I/O.
	 * 1 -> The multiplexed I/Os are allocated to the EMC.
	 */
        A2F_SYSREG->emc_mux_cr = CONFIG_SYS_EMCMUXCR;

	/*
	 * EMC timing parameters for chip select 0
	 * where the external SRAM memory resides on A2F-LNX-EVB.
	 */
        A2F_SYSREG->emc_cs_0_cr = CONFIG_SYS_EMC0CS0CR;

	/*
	 * Fill in global info with description of SRAM configuration.
	 */
        gd->bd->bi_dram[0].start = CONFIG_SYS_RAM_BASE;
        gd->bd->bi_dram[0].size = CONFIG_SYS_RAM_SIZE;

	/*
	 * EMC timing parameters for chip select 1
	 * where the external Flash memory resides on A2F-LNX-EVB.
	 */
        A2F_SYSREG->emc_cs_1_cr = CONFIG_SYS_EMC0CS1CR;
#endif

        return 0;
}

int misc_init_r(void)
{

	return 0;
}

#ifdef CONFIG_CORE10100
int board_eth_init(bd_t *bis)
{
	core_eth_init(bis);
	return 0;
}
#endif
