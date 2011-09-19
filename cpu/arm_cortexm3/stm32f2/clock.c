/*
 * (C) Copyright 2011
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
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
#include <asm/arch/stm32f2.h>

#include "clock.h"

#if !defined(CONFIG_STM32F2_HSE_HZ)
# error "Value of the External oscillator, CONFIG_STM32F2_HSE_HZ, is not set."
#endif

/*
 * Internal oscillator value
 */
#define STM32F2_HSI_HZ			16000000

/*
 * Offsets and bitmasks of some RCC regs
 */
#define STM32F2_RCC_CFGR_SWS_BIT	0
#define STM32F2_RCC_CFGR_SWS_MSK	0x000F

#define STM32F2_RCC_CFGR_HPRE_BIT	4
#define STM32F2_RCC_CFGR_HPRE_MSK	0x000F

#define STM32F2_RCC_CFGR_PPRE1_BIT	10
#define STM32F2_RCC_CFGR_PPRE1_MSK	0x0007

#define STM32F2_RCC_CFGR_PPRE2_BIT	13
#define STM32F2_RCC_CFGR_PPRE2_MSK	0x0007

#define STM32F2_RCC_PLLCFGR_PLLSRC_BIT	22
#define STM32F2_RCC_PLLCFGR_PLLSRC_MSK	0x0001

#define STM32F2_RCC_PLLCFGR_PLLM_BIT	0
#define STM32F2_RCC_PLLCFGR_PLLM_MSK	0x003F

#define STM32F2_RCC_PLLCFGR_PLLN_BIT	6
#define STM32F2_RCC_PLLCFGR_PLLN_MSK	0x01FF

#define STM32F2_RCC_PLLCFGR_PLLP_BIT	16
#define STM32F2_RCC_PLLCFGR_PLLP_MSK	0x0003

/*
 * Clock values
 */
static u32 clock_val[CLOCK_END];

/*
 * Initialize the reference clocks.
 */
void clock_init(void)
{
	static u32 apbahb_presc_tbl[] = {0, 0, 0, 0, 1, 2, 3, 4,
					 1, 2, 3, 4, 6, 7, 8, 9};

	volatile struct stm32f2_rcc_regs *rcc_regs =
		(struct stm32f2_rcc_regs *)STM32F2_RCC_BASE;
	u32 tmp, presc, pllvco, pllp, pllm;

	/*
	 * Get SYSCLK
	 */
	tmp  = rcc_regs->cfgr >> STM32F2_RCC_CFGR_SWS_BIT;
	tmp &= STM32F2_RCC_CFGR_SWS_MSK;
	switch (tmp) {
	case 0x00: /* HSI used as system clock source */
		clock_val[CLOCK_SYSCLK] = STM32F2_HSI_HZ;
		break;
	case 0x04: /* HSE used as system clock  source */
		clock_val[CLOCK_SYSCLK] = CONFIG_STM32F2_HSE_HZ;
		break;
	case 0x08: /* PLL used as system clock  source */
		/*
		 * PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN
		 * SYSCLK = PLL_VCO / PLLP
		 */
		pllm  = rcc_regs->pllcfgr >> STM32F2_RCC_PLLCFGR_PLLM_BIT;
		pllm &= STM32F2_RCC_PLLCFGR_PLLM_MSK;

		if (rcc_regs->pllcfgr & (1 << STM32F2_RCC_PLLCFGR_PLLSRC_BIT)) {
			/* HSE used as PLL clock source */
			tmp = CONFIG_STM32F2_HSE_HZ;
		} else {
			/* HSI used as PLL clock source */
			tmp = STM32F2_HSI_HZ;
		}
		pllvco  = rcc_regs->pllcfgr >> STM32F2_RCC_PLLCFGR_PLLN_BIT;
		pllvco &= STM32F2_RCC_PLLCFGR_PLLN_MSK;
		pllvco *= tmp / pllm;

		pllp  = rcc_regs->pllcfgr >> STM32F2_RCC_PLLCFGR_PLLP_BIT;
		pllp &= STM32F2_RCC_PLLCFGR_PLLP_MSK;
		pllp  = (pllp + 1) * 2;

		clock_val[CLOCK_SYSCLK] = pllvco / pllp;
		break;
	default:
		clock_val[CLOCK_SYSCLK] = STM32F2_HSI_HZ;
		break;
	}

	/*
	 * Get HCLK
	 */
	tmp  = rcc_regs->cfgr >> STM32F2_RCC_CFGR_HPRE_BIT;
	tmp &= STM32F2_RCC_CFGR_HPRE_MSK;
	presc = apbahb_presc_tbl[tmp];
	clock_val[CLOCK_HCLK] = clock_val[CLOCK_SYSCLK] >> presc;

	/*
	 * Get PCLK1
	 */
	tmp  = rcc_regs->cfgr >> STM32F2_RCC_CFGR_PPRE1_BIT;
	tmp &= STM32F2_RCC_CFGR_PPRE1_MSK;
	presc = apbahb_presc_tbl[tmp];
	clock_val[CLOCK_PCLK1] = clock_val[CLOCK_HCLK] >> presc;

	/*
	 * Get PCLK2
	 */
	tmp  = rcc_regs->cfgr >> STM32F2_RCC_CFGR_PPRE2_BIT;
	tmp &= STM32F2_RCC_CFGR_PPRE2_MSK;
	presc = apbahb_presc_tbl[tmp];
	clock_val[CLOCK_PCLK2] = clock_val[CLOCK_HCLK] >> presc;

	return;
}

/*
 * Return a clock value for the specified clock.
 * Note that we need this function in RAM because it will be used
 * during self-upgrade of U-boot into eNMV.
 * @param clck          id of the clock
 * @returns             frequency of the clock
 */
unsigned long  __attribute__((section(".ramcode")))
	       __attribute__ ((long_call))
	       clock_get(enum clock clck)
{
	return clock_val[clck];
}
