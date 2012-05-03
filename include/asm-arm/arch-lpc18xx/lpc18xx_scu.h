/*
 * (C) Copyright 2012
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

#ifndef _MACH_SCU_H_
#define _MACH_SCU_H_

/*
 * SCU (System Control Unit) register map
 */
struct lpc18xx_scu_regs {
	u32 sfs[802];		/* Pin configuration registers */
	u32 enaio0;		/* ADC0 function select register */
	u32 enaio1;		/* ADC1 function select register */
	u32 enaio2;		/* Analog function select register */
	u32 rsv0[27];
	u32 emcdelayclk;	/* EMC clock delay register */
	u32 rsv1[63];
	u32 pintsel0;		/* Pin interrupt select for interrupts 0 to 3 */
	u32 pintsel1;		/* Pin interrupt select for interrupts 4 to 7 */
};

/*
 * SCU registers base
 */
#define LPC18XX_SCU_BASE		0x40086000
#define LPC18XX_SCU			((volatile struct lpc18xx_scu_regs *) \
					LPC18XX_SCU_BASE)

#endif /* _MACH_SCU_H_ */
