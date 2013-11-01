/*
 * (C) Copyright 2013
 *
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

#ifndef _MACH_FMC_H_
#define _MACH_FMC_H_

struct stm32_fmc_regs {
	/* Control registers */
	u32 sdcr1;
	u32 sdcr2;

	/* Timing registers */
	u32 sdtr1;
	u32 sdtr2;

	/* Mode register */
	u32 sdcmr;

	/* Refresh timing register */
	u32 sdrtr;

	/* Status register */
	u32 sdsr;
};

/*
 * FMC registers base
 */
#define STM32_SDRAM_FMC_BASE			0xA0000140
#define STM32_SDRAM_FMC			((volatile struct stm32_fmc_regs *) \
						STM32_SDRAM_FMC_BASE)

/* Control register SDCR */
#define FMC_SDCR_RPIPE_SHIFT	13	/* RPIPE bit shift */
#define FMC_SDCR_RBURST_SHIFT	12	/* RBURST bit shift */
#define FMC_SDCR_SDCLK_SHIFT	10	/* SDRAM clock divisor shift */
#define FMC_SDCR_WP_SHIFT	9	/* Write protection shift */
#define FMC_SDCR_CAS_SHIFT	7	/* CAS latency shift */
#define FMC_SDCR_NB_SHIFT	6	/* Number of banks shift */
#define FMC_SDCR_MWID_SHIFT	4	/* Memory width shift */
#define FMC_SDCR_NR_SHIFT	2	/* Number of row address bits shift */
#define FMC_SDCR_NC_SHIFT	0	/* Number of col address bits shift */

/* Timings register SDTR */
#define FMC_SDTR_TMRD_SHIFT	0	/* Load mode register to active */
#define FMC_SDTR_TXSR_SHIFT	4	/* Exit self-refresh time */
#define FMC_SDTR_TRAS_SHIFT	8	/* Self-refresh time */
#define FMC_SDTR_TRC_SHIFT	12	/* Row cycle delay */
#define FMC_SDTR_TWR_SHIFT	16	/* Recovery delay */
#define FMC_SDTR_TRP_SHIFT	20	/* Row precharge delay */
#define FMC_SDTR_TRCD_SHIFT	24	/* Row-to-column delay */


#define FMC_SDCMR_NRFS_SHIFT	5

#define FMC_SDCMR_MODE_NORMAL		0
#define FMC_SDCMR_MODE_START_CLOCK	1
#define FMC_SDCMR_MODE_PRECHARGE	2
#define FMC_SDCMR_MODE_AUTOREFRESH	3
#define FMC_SDCMR_MODE_WRITE_MODE	4
#define FMC_SDCMR_MODE_SELFREFRESH	5
#define FMC_SDCMR_MODE_POWERDOWN	6

#define FMC_SDCMR_BANK_1		(1 << 4)
#define FMC_SDCMR_BANK_2		(1 << 3)

#define FMC_SDCMR_MODE_REGISTER_SHIFT	9

#define FMC_SDSR_BUSY			(1 << 5)

#define FMC_BUSY_WAIT()		do { \
		__asm__ __volatile__ ("dsb" : : : "memory"); \
		while(STM32_SDRAM_FMC->sdsr & FMC_SDSR_BUSY); \
	} while(0);


#endif /* _MACH_FMC_H_ */
