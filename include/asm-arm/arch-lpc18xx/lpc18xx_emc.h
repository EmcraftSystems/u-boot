/*
 * (C) Copyright 2012, 2013
 *
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Pavel Boldin, Emcraft Systems, paboldin@emcraft.com
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

/*
 * LPC18XX Extended Memory Controller
 */
#ifndef _MACH_LPC18XX_EMC_H_
#define _MACH_LPC18XX_EMC_H_

/*
 * EMC per-chip registers for DRAM.
 *
 * This structure must be 0x20 bytes in size
 * (for `struct lpc_emc_regs` to be correct.)
 */
struct lpc_emc_dy_regs {
	u32 cfg;	/* Dynamic Memory Configuration register */
	u32 rascas;	/* Dynamic Memory RAS & CAS Delay registers */
	u32 rsv0[6];
};

/*
 * EMC controls for Static Memory CS. Each block occupies 0x20 bytes.
 */
struct lpc_emc_st_regs {
	u32 cfg;	/* Static Memory Configuration register */
	u32 we;		/* CS to WE delay register */
	u32 oe;		/* CS to OE delay register */
	u32 rd;		/* CS to Read delay register */
	u32 page;	/* async page mode access delay */
	u32 wr;		/* CS to Write delay register */
	u32 ta;		/* number of turnaround cycles */
	u32 rsv0[1];
};

/*
 * EMC (External Memory Controller) register map
 */
struct lpc_emc_regs {
	/* 0x000 */
	u32 emcctrl;	/* EMC Control register */
	u32 emcsts;	/* EMC Status register */
	u32 emccfg;	/* EMC Configuration register */
	u32 rsv0[5];

	/* 0x020 */
	u32 dy_ctrl;	/* Dynamic Memory Control register */
	u32 dy_rfsh;	/* Dynamic Memory Refresh Timer register */
	u32 dy_rdcfg;	/* Dynamic Memory Read Configuration register */
	u32 rsv1;

	/* 0x030 */
	u32 dy_trp;	/* Dynamic Memory Precharge Command Period register */
	u32 dy_tras;	/* Dynamic Memory Active to Precharge Command
				Period register */
	u32 dy_srex;	/* Dynamic Memory Self-refresh Exit Time register */
	u32 dy_apr;	/* Dynamic Memory Last Data Out to Active
				Time register */
	u32 dy_dal;	/* Dynamic Memory Data-in to Active Command
				Time register */
	u32 dy_wr;	/* Dynamic Memory Write Recovery Time register */
	u32 dy_rc;	/* Dynamic Memory Active to Active Command
				Period register */
	u32 dy_rfc;	/* Dynamic Memory Auto-refresh Period register */
	u32 dy_xsr;	/* Dynamic Memory Exit Self-refresh register */
	u32 dy_rrd;	/* Dynamic Memory Active Bank A to
				Active Bank B Time register */
	u32 dy_mrd;	/* Dynamic Memory Load Mode register to
				Active Command Time */
	/* 0x05C */
	u32 rsv2[41];

	/* 0x100 */
	struct lpc_emc_dy_regs dy[4];	/* 4 DRAM chips are possible */
	u32 rsv3[32];
	/* 0x200 */
	struct lpc_emc_st_regs st[4];	/* 4 static memory devices */
};

#ifndef LPC18XX_EMC_BASE
# define LPC18XX_EMC_BASE		0x40005000
#endif

#define LPC_EMC				((volatile struct lpc_emc_regs *) \
					LPC18XX_EMC_BASE)

/*
 * Set the EMC clock freq.
 */
#ifdef CONFIG_LPC18XX_EMC_HALFCPU
# define LPC18XX_EMC_CLK_OUT		(LPC18XX_PLL1_CLK_OUT / 2)
#else
# define LPC18XX_EMC_CLK_OUT		LPC18XX_PLL1_CLK_OUT
#endif

static inline uint32_t NS2CLK(uint32_t time){
	uint32_t tmp = LPC18XX_EMC_CLK_OUT/1000000;
	return (tmp * time) / 1000;
}


#endif /* _MACH_LPC18XX_EMC_H_ */
