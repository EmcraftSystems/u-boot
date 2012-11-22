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
 * UART will not work without running its power-on code in
 * `cortex_m3_soc_init()`.
 */
#if defined(CONFIG_LPC178X_UART_PORT) && !defined(CONFIG_ARMCORTEXM3_SOC_INIT)
#error UART enabled, but it requires CONFIG_ARMCORTEXM3_SOC_INIT
#endif

/*
 * 1-bit masks for PCONP (Power Control for Peripherals register) for every
 * UART that enable power on these UARTs
 */
#ifdef CONFIG_LPC178X_UART_PORT
static const u32 uart_pconp_mask[] = {
	LPC178X_SCC_PCONP_PCUART0_MSK, LPC178X_SCC_PCONP_PCUART1_MSK,
	LPC178X_SCC_PCONP_PCUART2_MSK, LPC178X_SCC_PCONP_PCUART3_MSK,
	LPC178X_SCC_PCONP_PCUART4_MSK
};
#endif

/*
 * SoC configuration code that cannot be put into drivers
 */
#ifdef CONFIG_ARMCORTEXM3_SOC_INIT
void cortex_m3_soc_init(void)
{
#ifdef CONFIG_LPC178X_UART_PORT
	/*
	 * Enable power on the chosen UART
	 */
	lpc178x_periph_enable(uart_pconp_mask[CONFIG_LPC178X_UART_PORT], 1);
#endif
	/*
	 * Configure the memory protection unit (MPU) to allow full access to
	 * the whole 4GB address space.
	 *
	 * This is required, because in the default configuration code
	 * execution is not permitted at the addresses above 0xA0000000
	 * (including SDRAM.)
	 */
	cortex_m3_mpu_full_access();
}
#endif
