/*
 * (C) Copyright 2011
 *
 * Dmitry Konyshev, Emcraft Systems, probables@emcraft.com
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

#ifndef _MACH_FSMC_H_
#define _MACH_FSMC_H_

/*
 * BCR reg fields
 */
#define STM32_FSMC_BCR_MBKEN		(1 << 0)	/* Memory bank enble  */
#define STM32_FSMC_BCR_MWID_BIT		4		/* Databus width      */
#define STM32_FSMC_BCR_MWID_16		0x1		/* 16 bits	      */
#define STM32_FSMC_BCR_WREN		(1 << 12)	/* Write enable	      */

/*
 * BTR reg fields
 */
#define STM32_FSMC_BTR_ADDRST_BIT	0		/* Addr-phase time    */
#define STM32_FSMC_BTR_ADDHOLD_BIT	4		/* Addr-hold time     */
#define STM32_FSMC_BTR_DATAST_BIT	8		/* Data-phase time    */
#define STM32_FSMC_BTR_BUSTURN_BIT	16		/* BusTurnaround time */
#define STM32_FSMC_BTR_CLKDIV_BIT	20		/* ClkDiv */
#define STM32_FSMC_BTR_DATLAT_BIT	24		/* Data latency */
#define STM32_FSMC_BTR_ACCMOD_A		(0 << 28)	/* Access mode A */
#define STM32_FSMC_BTR_ACCMOD_B		(1 << 28)	/* Access mode B */
#define STM32_FSMC_BTR_ACCMOD_C		(2 << 28)	/* Access mode C */
#define STM32_FSMC_BTR_ACCMOD_D		(3 << 28)	/* Access mode D */

/*
 * BTW reg fields
 */
#define STM32_FSMC_BWTR_ADDRST_BIT	0		/* Addr-phase time    */
#define STM32_FSMC_BWTR_ADDHOLD_BIT	4		/* Addr-hold time     */
#define STM32_FSMC_BWTR_DATAST_BIT	8		/* Data-phase time    */
#define STM32_FSMC_BWTR_CLKDIV_BIT	20		/* ClkDiv */
#define STM32_FSMC_BWTR_DATLAT_BIT	24		/* Data latency */
#define STM32_FSMC_BWTR_ACCMOD_A	(0 << 28)	/* Access mode A */
#define STM32_FSMC_BWTR_ACCMOD_B	(1 << 28)	/* Access mode B */
#define STM32_FSMC_BWTR_ACCMOD_C	(2 << 28)	/* Access mode C */
#define STM32_FSMC_BWTR_ACCMOD_D	(3 << 28)	/* Access mode D */

/**
 * Initialize the specified NOR/PSRAM controller
 * Note that controller numbering starts at 1
 * @param num           controller number (1-4)
 * @param bcr           content of the BCR register
 * @param btr           content of the BTR register
 * @param bwtr          content of the BWTR register
 * @returns             0 on success, < 0 on failure
 */
int fsmc_nor_psram_init(u32 num, u32 bcr, u32 btr, u32 bwtr);

extern int fsmc_gpio_init_done;

#endif /* _MACH_FSMC_H_ */
