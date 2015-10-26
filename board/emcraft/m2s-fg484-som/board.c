/*
 * board/emcraft/m2s-som/board.c
 *
 * Board specific code the the Emcraft SmartFusion2 system-on-module (SOM).
 *
 * (C) Copyright 2012, 2013
 * Emcraft Systems, <www.emcraft.com>
 * Alexander Potashev <aspotashev@emcraft.com>
 * Vladimir Khusainov <vlad@emcraft.com>
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

/*
 * Generate DDR timings depending on the following DDR clock
 */
#define M2S_DDR_MHZ		(CONFIG_SYS_M2S_SYSREF / (1000 * 1000))

/*
 * Common conversion macros used for DDR cfg
 */
#define M2S_CLK_VAL(ns, div)	((((ns) * M2S_DDR_MHZ) / div))
#define M2S_CLK_MOD(ns, div)	((((ns) * M2S_DDR_MHZ) % div))

#define M2S_CLK_MIN(ns)		(M2S_CLK_MOD(ns,1000) ?			\
				 M2S_CLK_VAL(ns,1000) + 1 :		\
				 M2S_CLK_VAL(ns,1000))
#define M2S_CLK32_MIN(ns)	(M2S_CLK_MOD(ns,32000) ?		\
				 M2S_CLK_VAL(ns,32000) + 1 :		\
				 M2S_CLK_VAL(ns,32000))
#define M2S_CLK1024_MIN(ns)	(M2S_CLK_MOD(ns,1024000) ?		\
				 M2S_CLK_VAL(ns,1024000) + 1 :		\
				 M2S_CLK_VAL(ns,1024000))
#define M2S_CLK_MAX(ns)		(M2S_CLK_VAL(ns,1000))
#define M2S_CLK32_MAX(ns)	(M2S_CLK_VAL(ns,32000))
#define M2S_CLK1024_MAX(ns)	(M2S_CLK_VAL(ns,1024000))

/*
 * MT46H32M16LFBF-6 params & timings
 */
#define DDR_BL			8	/* Burst length (value)		*/
#define DDR_MR_BL		3	/* Burst length (power of 2)	*/
#define DDR_BT			0	/* Burst type int(1)/seq(0)	*/

#define DDR_CL			3	/* CAS (read) latency		*/
#define DDR_WL			1	/* Write latency		*/
#define DDR_tMRD		2
#define DDR_tWTR		2
#define DDR_tXP			1
#define DDR_tCKE		1

#define DDR_tRFC		M2S_CLK_MIN(72)
#define DDR_tREFI		M2S_CLK32_MAX(7800)
#define DDR_tCKE_pre		M2S_CLK1024_MIN(200000)
#define DDR_tCKE_post		M2S_CLK1024_MIN(400)
#define DDR_tRCD		M2S_CLK_MIN(18)
#define DDR_tRRD		M2S_CLK_MIN(12)
#define DDR_tRP			M2S_CLK_MIN(18)
#define DDR_tRC			M2S_CLK_MIN(60)
#define DDR_tRAS_max		M2S_CLK1024_MAX(70000)
#define DDR_tRAS_min		M2S_CLK_MIN(42)
#define DDR_tWR			M2S_CLK_MIN(15)

/*
 * There are no these timings exactly in spec, so take smth
 */
#define DDR_tCCD		2	/* 2-reads/writes (bank A to B)	*/

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	return 0;
}

int checkboard(void)
{
	printf("Board: " CONFIG_SYS_BOARD_NAME " Rev %s\n",
		CONFIG_SYS_BOARD_REV_STR);
	return 0;
}

/*
 * Initialize DRAM
 */
int dram_init (void)
{
#if ( CONFIG_NR_DRAM_BANKS > 0 )
	volatile struct ddr_regs	*ddr = (void *)0x40020000;
	u16				val;

	/*
	 * Enable access to MDDR regs
	 */
	M2S_SYSREG->mddr_cr = (1 << 0);

	/*
	 * No non-bufferable regions
	 */
	M2S_SYSREG->ddrb_nb_size_cr = 0;

#if (DDR_BL == 16)
	/*
	 * Disable all DDR Bridge buffers
	 * We suspect some bug in the buffering scheme, so disable
	 * this for now
	 */
	M2S_SYSREG->ddrb_cr = 0;
#endif

	/*
	 * Configure mode, and mapping:
	 * - LPDDR1 + PHY-16 + ECC_DISABLE
	 * - BANK:1-0,COL:9-0,ROW:12-0 <-> src[2]..
	 */
	ddr->ddrc.DYN_POWERDOWN_CR = (0 << REG_DDRC_POWERDOWN_EN);
	ddr->ddrc.PWR_SAVE_1_CR = (4 << REG_DDRC_POST_SELFREF_GAP_X32_SHIFT) |
		(0xc << REG_DDRC_POWERDOWN_TO_X32_SHIFT);

	ddr->ddrc.MODE_CR = (  1 << REG_DDRC_MOBILE) |
			    (  1 << REG_DDRC_SDRAM) |
			    (0x1 << REG_DDRC_DATA_BUS_WIDTH);
	ddr->ddrc.ADDR_MAP_BANK_CR  = 0x099F;
	ddr->ddrc.ADDR_MAP_COL_1_CR = 0x3333;
	ddr->ddrc.ADDR_MAP_COL_2_CR = 0xFFFF;
	ddr->ddrc.ADDR_MAP_COL_3_CR = 0x3300;
	ddr->ddrc.ADDR_MAP_ROW_1_CR = 0x7777;
	ddr->ddrc.ADDR_MAP_ROW_2_CR = 0x0FFF;

	/*
	 * Setup timings
	 */
	ddr->ddrc.DYN_REFRESH_1_CR = DDR_tRFC << REG_DDRC_T_RFC_MIN;
	ddr->ddrc.DYN_REFRESH_2_CR = (DDR_tREFI << REG_DDRC_T_RFC_NOM_X32);
	ddr->ddrc.CKE_RSTN_CYCLES_1_CR = DDR_tCKE_pre << REG_DDRC_PRE_CKE_X1024;
	ddr->ddrc.CKE_RSTN_CYCLES_2_CR = DDR_tCKE_post << REG_DDRC_POST_CKE_X1024;
	ddr->ddrc.DRAM_BANK_ACT_TIMING_CR = (DDR_tRCD << REG_DDRC_T_RCD) |
					    (DDR_tCCD << REG_DDRC_T_CCD) |
					    (DDR_tRRD << REG_DDRC_T_RRD) |
					    (DDR_tRP << REG_DDRC_T_RP);
	ddr->ddrc.DRAM_BANK_TIMING_PARAM_CR = DDR_tRC << REG_DDRC_T_RC;
	ddr->ddrc.DRAM_MR_TIMING_PARAM_CR = DDR_tMRD << REG_DDRC_T_MRD;
	ddr->ddrc.DRAM_RAS_TIMING_CR = (DDR_tRAS_max << REG_DDRC_T_RAS_MAX) |
				       (DDR_tRAS_min << REG_DDRC_T_RAS_MIN);
	ddr->ddrc.DFI_RDDATA_EN_CR = DDR_CL << REG_DDRC_DFI_T_RDDATA_EN;

	ddr->ddrc.DRAM_RD_WR_LATENCY_CR = (DDR_WL << REG_DDRC_WRITE_LATENCY) |
					  (DDR_CL << REG_DDRC_READ_LATENCY);

	/*
	 * DDR Mode register values
	 * - Burst Length, CL, and BT=Interleaved
	 * - Drive Strength 1/2
	 */
	val = (DDR_CL << 4) | (DDR_BT << 3) | (DDR_MR_BL << 0);
	ddr->ddrc.INIT_MR_CR = val;
	ddr->ddrc.INIT_EMR_CR = 0x0020;
	ddr->ddrc.MODE_REG_DATA_CR = val;
	ddr->ddrc.MODE_REG_RD_WR_CR = 1 << 3;

	/*
	 * Configure BL16, and related timings
	 */
	ddr->ddrc.PERF_PARAM_1_CR = ((DDR_BL >> 2) << REG_DDRC_BURST_RDWR);
	ddr->ddrc.PERF_PARAM_2_CR = (DDR_BT << REG_DDRC_BURST_MODE);

	ddr->ddrc.DRAM_RD_WR_TRNARND_TIME_CR = ((DDR_CL + (DDR_BL/2) + 2 -
						 DDR_WL) << REG_DDRC_RD2WR) |
					       ((DDR_WL + DDR_tWTR +
						(DDR_BL/2)) << REG_DDRC_WR2RD);
	ddr->ddrc.DRAM_RD_WR_PRE_CR = ((DDR_WL + (DDR_BL/2) +
					DDR_tWR) << REG_DDRC_WR2PRE) |
				      ((DDR_BL/2) << REG_DDRC_RD2PRE);

	ddr->ddrc.DRAM_T_PD_CR = (DDR_tXP << REG_DDRC_T_XP) |
				 (DDR_tCKE << REG_DDRC_T_CKE);

	/*
	 * Queue params
	 * FIXME: clean-up these somehow
	 */
	ddr->ddrc.HPR_QUEUE_PARAM_1_CR = 0x80F8;
	ddr->ddrc.HPR_QUEUE_PARAM_2_CR = 0x0007;
	ddr->ddrc.LPR_QUEUE_PARAM_1_CR = 0x80F8;
	ddr->ddrc.LPR_QUEUE_PARAM_2_CR = 0x0007;
	ddr->ddrc.WR_QUEUE_PARAM_CR    = 0x0200;

	/*
	 * PHY Registers
	 * FIXME: clean-up these somehow
	 */
	ddr->phy.DYN_LOOPBACK_TEST_CR		= 0x0000;
	ddr->phy.CTRL_SLAVE_RATIO_CR		= 0x0080;
	ddr->phy.DATA_SLICE_IN_USE_CR		= 0x000F;
	ddr->phy.DLL_LOCK_DIFF_CR		= 0x000B;

	ddr->phy.FIFO_WE_SLAVE_RATIO_1_CR	= 0x0000;
	ddr->phy.FIFO_WE_SLAVE_RATIO_2_CR	= 0x0000;
	ddr->phy.FIFO_WE_SLAVE_RATIO_3_CR	= 0x0000;
	ddr->phy.FIFO_WE_SLAVE_RATIO_4_CR	= 0x0000;

	ddr->phy.LOCAL_ODT_CR			= 0x0001;

	ddr->phy.RD_DQS_SLAVE_RATIO_1_CR	= 0x0040;
	ddr->phy.RD_DQS_SLAVE_RATIO_2_CR	= 0x0401;
	ddr->phy.RD_DQS_SLAVE_RATIO_3_CR	= 0x4010;

	ddr->phy.WR_DATA_SLAVE_RATIO_1_CR	= 0x0040;
	ddr->phy.WR_DATA_SLAVE_RATIO_2_CR	= 0x0401;
	ddr->phy.WR_DATA_SLAVE_RATIO_3_CR	= 0x4010;

	ddr->phy.WR_RD_RL_CR			= 0x0000;
	ddr->phy.RDC_WE_TO_RE_DELAY_CR		= 0x0003;
	ddr->phy.USE_FIXED_RE_CR		= 0x0001;
	ddr->phy.USE_RANK0_DELAYS_CR		= 0x0001;
	ddr->phy.DYN_CONFIG_CR			= 0x0009;
	ddr->phy.DQ_OFFSET_1_CR			= 0x0000;
	ddr->phy.DQ_OFFSET_2_CR			= 0x0000;
	ddr->phy.DYN_RESET_CR			= 0x0001;

	ddr->ddrc.DYN_SOFT_RESET_CR		= 0x0001;

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

#ifdef CONFIG_M2S_ETH
int board_eth_init(bd_t *bis)
{
	return m2s_eth_driver_init(bis);
}
#endif

