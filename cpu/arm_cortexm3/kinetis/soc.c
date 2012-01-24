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

#include "soc.h"

/*
 * Memory Protection Unit (MPU) register map
 *
 * See Chapter 18 of the K60 Reference Manual (page 409)
 */
struct kinetis_mpu_regs {
	u32 cesr;	/* Control/Error Status Register */
};

/*
 * MPU registers base
 */
#define KINETIS_MPU_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x0000D000)
#define KINETIS_MPU			((volatile struct kinetis_mpu_regs *) \
					KINETIS_MPU_BASE)

/*
 * SoC configuration code that cannot be put into drivers
 */
#ifdef CONFIG_ARMCORTEXM3_SOC_INIT
void cortex_m3_soc_init(void)
{
#ifdef CONFIG_MCFFEC
	/*
	 * Enable the clock on the Ethernet module of the MCU
	 */
	kinetis_periph_enable(KINETIS_CG_ENET, 1);
#endif /* CONFIG_MCFFEC */

#ifdef CONFIG_CMD_NAND
	/*
	 * Enable the clock on the NAND Flash Controller module of the MCU
	 */
	kinetis_periph_enable(KINETIS_CG_NFC, 1);
#endif /* CONFIG_CMD_NAND */

	/*
	 * Disable the MPU to let the Ethernet module access the SRAM
	 */
	KINETIS_MPU->cesr = 0;
}
#endif
