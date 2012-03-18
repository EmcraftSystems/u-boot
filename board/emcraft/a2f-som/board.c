/*
 * board/emcraft/a2f-som/board.c
 *
 * Board specific code the the Emcraft SmartFusion system-on-module (SOM).
 *
 * Copyright (C) 2012
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
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

/*
 * FPGA Fabric may include the PSRAM_IP block, which is used to
 * control the signals on the EMC interface to get access to
 * the configuration registers of the external PSRAM, which
 * allows to put the PSRAM into the faster Page Mode.
 * The following data structure and the macros provide access
 * to the control registers of PSRAM_IP.
 * Note: PSRAM_IP may be accessed only when the EMC interface is
 * off. In case PSRAM_IP is instantiated in a FPGA design, EMC
 * is disabled (or Libero wouldn't have allowed to instantiate
 * PSRAM_IP, since it works on the same signals that are used by EMC).
 */
struct psram_ip {
	uint32_t val;
	uint32_t data_out;
	uint32_t addr;
	uint32_t trans;
	uint32_t magic;
};

#define PSRAM_IP_BASE		0x40050300
#define PSRAM_IP		((volatile struct psram_ip *)(PSRAM_IP_BASE))
#define PSRAM_IP_MAGIC		0x7777
#define PSRAM_IP_BUSY		(1<<31)

/*
 * Software flag: PSRAM in Page Mode
 */
static int psram_in_page_mode = 0;

/*
 * Check if PSRAM_IP is instantiated, and if so, put PSRAM into Page Mode.
 * Enable the EMC interface so we are able to access Flash (and then
 * external RAM after dram_init has run).
 */
void psram_page_mode(void)
{
	unsigned int v;

	/*
	 * Check if the PSRAM_IP IP block is there in the FPGA fabric.
	 */
	if (PSRAM_IP->magic == PSRAM_IP_MAGIC) {

		/*
		 * Switch the EMC signals into FPGA mode.
		 */
		A2F_SYSREG->emc_mux_cr &= ~CONFIG_SYS_EMCMUXCR;

		/*
		 * If so, perform the sequence to put PSRAM into Page Mode.
		 */
		PSRAM_IP->addr = 0xFFFFFFFF;
		PSRAM_IP->trans = 0x10000;
		while (PSRAM_IP->data_out & PSRAM_IP_BUSY);
		v = PSRAM_IP->data_out;

		PSRAM_IP->addr = 0xFFFFFFFF;
		PSRAM_IP->val = 0x90;
		PSRAM_IP->trans = 0x0;
		while (PSRAM_IP->data_out & PSRAM_IP_BUSY);

		PSRAM_IP->addr = 0xFFFFFFFF;
		PSRAM_IP->trans = 0x10000;
		while (PSRAM_IP->data_out & PSRAM_IP_BUSY);
		v = PSRAM_IP->data_out;

		/*
		 * If PSRAM has been successfully put into Page Mode,
		 * remember this in a software flag.
		 */
		if (v == 0x90) {
			psram_in_page_mode = 1;
		}
	}

	/*
	 * Release the EMC from reset. It may have been put
	 * into reset by a design that installs the IP core
	 * for setting the external PSRAM into Page Mode.
	 */
        A2F_SYSREG->soft_rst_cr &= ~(1<<3);

	/*
	 * External memory controller MUX configuration
	 * The EMC _SEL bit in the EMC_MUX_CR register is used
	 * to select either FPGA I/O or EMC I/O.
	 * 1 -> The multiplexed I/Os are allocated to the EMC.
	 */
        A2F_SYSREG->emc_mux_cr = CONFIG_SYS_EMCMUXCR;

	/*
	 * EMC timing parameters for chip select 1
	 * where the external Flash memory resides on A2F-LNX-EVB.
	 * We need to enable the Flash because env_init will
	 * run soon (which needs to access the Flash).
	 */
        A2F_SYSREG->emc_cs_1_cr = CONFIG_SYS_EMC0CS1CR;
}

int board_init(void)
{
	psram_page_mode();
	return 0;
}

int checkboard(void)
{
	printf("Board: A2F-SOM Rev %s, www.emcraft.com\n",
		CONFIG_SYS_BOARD_REV_STR);
	return 0;
}

int dram_init (void)
{
#if ( CONFIG_NR_DRAM_BANKS > 0 )
	/*
	 * EMC timing parameters for chip select 0
	 * where the external SRAM memory resides on A2F-LNX-EVB.
	 * The settings depend on whether we have put PSRAM
	 * into Page Mode or not.
	 */
        A2F_SYSREG->emc_cs_0_cr = psram_in_page_mode ?
					CONFIG_SYS_EMC0CS0CR_PM :
					CONFIG_SYS_EMC0CS0CR;

	/*
	 * Fill in global info with description of SRAM configuration.
	 */
        gd->bd->bi_dram[0].start = CONFIG_SYS_RAM_BASE;
        gd->bd->bi_dram[0].size = CONFIG_SYS_RAM_SIZE;
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
