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

#include "clock.h"
#include "envm.h"

/*
 * STM32 Clock configuration is set by the number of CONFIG options.
 *
 * Source for system clock must be selected:
 * - CONFIG_STM32_SYS_CLK_HSI: HSI oscillator used as sys clock,
 *   CONFIG_STM32_SYS_CLK_HSE: HSE oscillator used as sys clock,
 *   CONFIG_STM32_SYS_CLK_PLL: PLL used as sys clock.
 *
 * In case of CONFIG_STM32_SYS_CLK_HSE or CONFIG_STM32_SYS_CLK_PLL with
 * CONFIG_STM32_PLL_SRC_HSE configurations, the following option must be set:
 * - CONFIG_STM32_HSE_HZ: HSE oscillator frequency value.
 *
 * In case of CONFIG_STM32_SYS_CLK_PLL configuration, the following options
 * must be set:
 * - CONFIG_STM32_PLL_SRC_HSI: HSI clock used as PLL clock entry,
 *   CONFIG_STM32_PLL_SRC_HSE: HSE clock used as PLL clock entry;
 * - CONFIG_STM32_PLL_M: division factor for PLL input clock;
 * - CONFIG_STM32_PLL_N: multiplication factor for VCO;
 * - CONFIG_STM32_PLL_P: division factor for main system clock;
 * - CONFIG_STM32_PLL_Q: division factor for USB OTG FS, SDIO and RNG clocks.
 * Then:
 *  Fsys = Fvco / P    : PLL general clock output;
 *  Fvco = Fin * N / M : VCO clock (Fin is the frequency of PLL source);
 *  Fout = Fvco / Q    : USB OTG FS, SDIO, RNG clock output.
 *
 * !!! Note, beside of the options specified above VCC voltage may limit the
 * acceptable range of system clock, below is assumed that VDD(V) is equal to
 * 3.3V (i.e. in range of [2.7; 3.6]).
 * See AN3362 "Clock configuration tool for STM32F2xx microcontrollers"
 * application note, and stm32f2xx_rcc.c code of STM32F2xx_StdPeriph_Driver
 * library from STM for details.
 */

/*
 * Check configuration params
 */
#if ((!!defined(CONFIG_STM32_SYS_CLK_HSI) +				       \
      !!defined(CONFIG_STM32_SYS_CLK_HSE) +				       \
      !!defined(CONFIG_STM32_SYS_CLK_PLL)) != 1)
# error "Incorrect SYS Clock source configuration."
#endif

#if defined(CONFIG_STM32_SYS_CLK_HSE) ||				       \
    (defined(CONFIG_STM32_SYS_CLK_PLL) &&				       \
     defined(CONFIG_STM32_PLL_SRC_HSE))
# if !defined(CONFIG_STM32_HSE_HZ)
#  error "External oscillator HSE value is not set."
# endif
#endif

#if defined(CONFIG_STM32_SYS_CLK_PLL)
# if ((!!defined(CONFIG_STM32_PLL_SRC_HSI) +				       \
       !!defined(CONFIG_STM32_PLL_SRC_HSE)) != 1)
#  error "Incorrect PLL clock source configuration."
# endif
# if ((!!defined(CONFIG_STM32_PLL_M) +					       \
       !!defined(CONFIG_STM32_PLL_N) +					       \
       !!defined(CONFIG_STM32_PLL_P) +					       \
       !!defined(CONFIG_STM32_PLL_Q)) != 4)
#  error "Incomplete PLL configuration."
# endif
# if (CONFIG_STM32_PLL_M < 2) || (CONFIG_STM32_PLL_M > 63)
#  error "Incorrect PLL_M value."
# endif
# if (CONFIG_STM32_PLL_N < 192) || (CONFIG_STM32_PLL_N > 432)
#  error "Incorrect PLL_N value."
# endif
# if (CONFIG_STM32_PLL_P != 2) && (CONFIG_STM32_PLL_P != 4) &&		       \
     (CONFIG_STM32_PLL_P != 6) && (CONFIG_STM32_PLL_P != 8)
#  error "Incorrect PLL_P value."
# endif
# if (CONFIG_STM32_PLL_Q < 4) || (CONFIG_STM32_PLL_Q > 15)
#  error "Incorrect PLL_Q value."
# endif
#endif

/*
 * Internal oscillator value
 */
#define STM32_HSI_HZ			16000000	/* 16 MHz	      */

/*
 * Get the SYS CLK value according to configuration
 */
#if defined(CONFIG_STM32_SYS_CLK_HSI)
# define STM32_SYS_CLK			STM32_HSI_HZ
#elif defined(CONFIG_STM32_SYS_CLK_HSE)
# define STM32_SYS_CLK			CONFIG_STM32_HSE_HZ
#else
# if defined(CONFIG_STM32_PLL_SRC_HSE)
#  define STM32_PLL_IN_HZ		CONFIG_STM32_HSE_HZ
# else
#  define STM32_PLL_IN_HZ		STM32_HSI_HZ
# endif
# define STM32_SYS_CLK			((STM32_PLL_IN_HZ *		       \
					  CONFIG_STM32_PLL_N) /		       \
					 (CONFIG_STM32_PLL_M *		       \
					  CONFIG_STM32_PLL_P))
#endif

/*
 * Get the Flash latency value for this SYS CLK
 */
#  if (STM32_SYS_CLK >        0) && (STM32_SYS_CLK <=  30000000)
# define STM32_FLASH_WS			0
#elif (STM32_SYS_CLK > 30000000) && (STM32_SYS_CLK <=  60000000)
# define STM32_FLASH_WS			1
#elif (STM32_SYS_CLK > 60000000) && (STM32_SYS_CLK <=  90000000)
# define STM32_FLASH_WS			2
#elif (STM32_SYS_CLK > 90000000) && (STM32_SYS_CLK <= 120000000)
# define STM32_FLASH_WS			3
#elif (STM32_SYS_CLK > 120000000) && (STM32_SYS_CLK <= 150000000)
# define STM32_FLASH_WS			4
#elif (STM32_SYS_CLK > 150000000) && (STM32_SYS_CLK <= 168000000)
# define STM32_FLASH_WS			5
#else
# error "Incorrect System clock value configuration."
# define STM32_FLASH_WS			0	/* to avoid compile-time err  */
#endif

/*
 * Offsets and bitmasks of some RCC regs
 */
#define STM32_RCC_CR_HSEON		(1 << 16) /* HSE clock enable	      */
#define STM32_RCC_CR_HSERDY		(1 << 17) /* HSE clock ready	      */
#define STM32_RCC_CR_PLLON		(1 << 24) /* PLL clock enable	      */
#define STM32_RCC_CR_PLLRDY		(1 << 25) /* PLL clock ready	      */

#define STM32_RCC_CFGR_SW_BIT		0	/* System clock switch	      */
#define STM32_RCC_CFGR_SW_MSK		0x3
#define STM32_RCC_CFGR_SWS_BIT		2	/* System clock switch status */
#define STM32_RCC_CFGR_SWS_MSK		0x3

#define STM32_RCC_CFGR_SWS_HSI		0x0
#define STM32_RCC_CFGR_SWS_HSE		0x1
#define STM32_RCC_CFGR_SWS_PLL		0x2

#define STM32_RCC_CFGR_HPRE_BIT		4	/* AHB prescaler	      */
#define STM32_RCC_CFGR_HPRE_MSK		0xF
#define STM32_RCC_CFGR_HPRE_DIVNO	0x0
#define STM32_RCC_CFGR_HPRE_DIV2	0x8
#define STM32_RCC_CFGR_HPRE_DIV4	0x9
#define STM32_RCC_CFGR_HPRE_DIV8	0xA
#define STM32_RCC_CFGR_HPRE_DIV16	0xB
#define STM32_RCC_CFGR_HPRE_DIV64	0xC
#define STM32_RCC_CFGR_HPRE_DIV128	0xD
#define STM32_RCC_CFGR_HPRE_DIV256	0xE
#define STM32_RCC_CFGR_HPRE_DIV512	0xF

#define STM32_RCC_CFGR_PPRE1_BIT	10	/* APB Low speed presc (APB1) */
#define STM32_RCC_CFGR_PPRE1_MSK	0x7
#define STM32_RCC_CFGR_PPRE1_DIV0	0x0
#define STM32_RCC_CFGR_PPRE1_DIV2	0x4
#define STM32_RCC_CFGR_PPRE1_DIV4	0x5
#define STM32_RCC_CFGR_PPRE1_DIV8	0x6
#define STM32_RCC_CFGR_PPRE1_DIV16	0x7

#define STM32_RCC_CFGR_PPRE2_BIT	13	/* APB high-speed presc (APB2)*/
#define STM32_RCC_CFGR_PPRE2_MSK	0x7
#define STM32_RCC_CFGR_PPRE2_DIVNO	0x0
#define STM32_RCC_CFGR_PPRE2_DIV2	0x4
#define STM32_RCC_CFGR_PPRE2_DIV4	0x5
#define STM32_RCC_CFGR_PPRE2_DIV8	0x6
#define STM32_RCC_CFGR_PPRE2_DIV16	0x7

#define STM32_RCC_PLLCFGR_HSESRC	(1 << 22) /* Main PLL entry clock src */

#define STM32_RCC_PLLCFGR_PLLM_BIT	0	/* Div factor for input clock */
#define STM32_RCC_PLLCFGR_PLLM_MSK	0x3F

#define STM32_RCC_PLLCFGR_PLLN_BIT	6	/* Mult factor for VCO	      */
#define STM32_RCC_PLLCFGR_PLLN_MSK	0x1FF

#define STM32_RCC_PLLCFGR_PLLP_BIT	16	/* Div factor for main sysclk */
#define STM32_RCC_PLLCFGR_PLLP_MSK	0x3

#define STM32_RCC_PLLCFGR_PLLQ_BIT	24	/* Div factor for USB,SDIO,.. */
#define STM32_RCC_PLLCFGR_PLLQ_MSK	0xF

/*
 * Timeouts (in cycles)
 */
#define STM32_HSE_STARTUP_TIMEOUT	0x05000

/*
 * Clock values
 */
static u32 clock_val[CLOCK_END];

#if !defined(CONFIG_STM32_SYS_CLK_HSI)
/*
 * Set-up clock configuration.
 */
static void clock_setup(void)
{
	u32	val;
	int	i;

	/*
	 * Enable HSE, and wait it becomes ready
	 */
	STM32_RCC->cr |= STM32_RCC_CR_HSEON;
	for (i = 0; i < STM32_HSE_STARTUP_TIMEOUT; i++) {
		if (STM32_RCC->cr & STM32_RCC_CR_HSERDY)
			break;
	}

	if (!(STM32_RCC->cr & STM32_RCC_CR_HSERDY)) {
		/*
		 * Have no HSE clock
		 */
		goto out;
	}

	val = STM32_RCC->cfgr;

	/*
	 * HCLK = SYSCLK / 1
	 */
	val &= ~(STM32_RCC_CFGR_HPRE_MSK << STM32_RCC_CFGR_HPRE_BIT);
	val |= STM32_RCC_CFGR_HPRE_DIVNO << STM32_RCC_CFGR_HPRE_BIT;

	/*
	 * PCLK2 = HCLK / 2
	 */
	val &= ~(STM32_RCC_CFGR_PPRE2_MSK << STM32_RCC_CFGR_PPRE2_BIT);
	val |= STM32_RCC_CFGR_PPRE2_DIV2 << STM32_RCC_CFGR_PPRE2_BIT;

	/*
	 * PCLK1 = HCLK / 4
	 */
	val &= ~(STM32_RCC_CFGR_PPRE1_MSK << STM32_RCC_CFGR_PPRE1_BIT);
	val |= STM32_RCC_CFGR_PPRE1_DIV4 << STM32_RCC_CFGR_PPRE1_BIT;

	STM32_RCC->cfgr = val;

# if defined(CONFIG_STM32_SYS_CLK_PLL)
	/*
	 * Configure the main PLL
	 */
#  if defined(CONFIG_STM32_PLL_SRC_HSE)
	val = STM32_RCC_PLLCFGR_HSESRC;
#  else
	val = 0;
#  endif

	val |= CONFIG_STM32_PLL_M << STM32_RCC_PLLCFGR_PLLM_BIT;
	val |= CONFIG_STM32_PLL_N << STM32_RCC_PLLCFGR_PLLN_BIT;
	val |= ((CONFIG_STM32_PLL_P >> 1) - 1) << STM32_RCC_PLLCFGR_PLLP_BIT;
	val |= CONFIG_STM32_PLL_Q << STM32_RCC_PLLCFGR_PLLQ_BIT;

	STM32_RCC->pllcfgr = val;

	/*
	 * Enable the main PLL, and wait until main PLL becomes ready.
	 * Note: we wait infinitely here, since the max time necessary for
	 * PLL to lock is probably not a constant. There's no timeout here in
	 * STM lib code as well.
	 */
	STM32_RCC->cr |= STM32_RCC_CR_PLLON;
	while (STM32_RCC->cr & STM32_RCC_CR_PLLRDY);

	/*
	 * Select PLL as system source if it's setup OK, and HSE otherwise
	 */
	if (!(STM32_RCC->cr & STM32_RCC_CR_PLLRDY))
		val = STM32_RCC_CFGR_SWS_PLL;
	else
		val = STM32_RCC_CFGR_SWS_HSE;
# else
	/*
	 * Select HSE as system source
	 */
	val = STM32_RCC_CFGR_SWS_HSE;
# endif /* CONFIG_STM32_SYS_CLK_PLL */

	/*
	 * Configure Flash prefetch, Instruction cache, and wait
	 * latency.
	 */
	envm_config(STM32_FLASH_WS);

	/*
	 * Change system clock source, and wait (infinite!) till it done
	 */
	STM32_RCC->cfgr &= ~(STM32_RCC_CFGR_SW_MSK << STM32_RCC_CFGR_SW_BIT);
	STM32_RCC->cfgr |= val << STM32_RCC_CFGR_SW_BIT;
	while ((STM32_RCC->cfgr & (STM32_RCC_CFGR_SWS_MSK <<
				   STM32_RCC_CFGR_SWS_BIT)) !=
	       (val << STM32_RCC_CFGR_SWS_BIT));
out:
	return;
}
#endif /* CONFIG_STM32_SYS_CLK_HSI */

/*
 * Initialize the reference clocks.
 */
void clock_init(void)
{
	static u32 apbahb_presc_tbl[] = {0, 0, 0, 0, 1, 2, 3, 4,
					 1, 2, 3, 4, 6, 7, 8, 9};

	u32 tmp, presc, pllvco, pllp, pllm;

#if !defined(CONFIG_STM32_SYS_CLK_HSI)
	/*
	 * Set clocks to cfg, which is differs from the poweron default
	 */
	clock_setup();
#else
	/*
	 * For consistency with !HSI configs, enable prefetch and cache
	 */
	envm_config(STM32_FLASH_WS);
#endif

	/*
	 * Get SYSCLK
	 */
	tmp  = STM32_RCC->cfgr >> STM32_RCC_CFGR_SWS_BIT;
	tmp &= STM32_RCC_CFGR_SWS_MSK;
	switch (tmp) {
	case STM32_RCC_CFGR_SWS_HSI:
		/* HSI used as system clock source */
		clock_val[CLOCK_SYSCLK] = STM32_HSI_HZ;
		break;
	case STM32_RCC_CFGR_SWS_HSE:
		/* HSE used as system clock source */
		clock_val[CLOCK_SYSCLK] = CONFIG_STM32_HSE_HZ;
		break;
	case STM32_RCC_CFGR_SWS_PLL:
		/* PLL used as system clock source */
		/*
		 * PLL_VCO = (HSE_VALUE or HSI_VALUE / PLLM) * PLLN
		 * SYSCLK = PLL_VCO / PLLP
		 */
		pllm  = STM32_RCC->pllcfgr >> STM32_RCC_PLLCFGR_PLLM_BIT;
		pllm &= STM32_RCC_PLLCFGR_PLLM_MSK;

		if (STM32_RCC->pllcfgr & STM32_RCC_PLLCFGR_HSESRC) {
			/* HSE used as PLL clock source */
			tmp = CONFIG_STM32_HSE_HZ;
		} else {
			/* HSI used as PLL clock source */
			tmp = STM32_HSI_HZ;
		}
		pllvco  = STM32_RCC->pllcfgr >> STM32_RCC_PLLCFGR_PLLN_BIT;
		pllvco &= STM32_RCC_PLLCFGR_PLLN_MSK;
		pllvco *= tmp / pllm;

		pllp  = STM32_RCC->pllcfgr >> STM32_RCC_PLLCFGR_PLLP_BIT;
		pllp &= STM32_RCC_PLLCFGR_PLLP_MSK;
		pllp  = (pllp + 1) * 2;

		clock_val[CLOCK_SYSCLK] = pllvco / pllp;
		break;
	default:
		clock_val[CLOCK_SYSCLK] = STM32_HSI_HZ;
		break;
	}

	/*
	 * Get HCLK
	 */
	tmp  = STM32_RCC->cfgr >> STM32_RCC_CFGR_HPRE_BIT;
	tmp &= STM32_RCC_CFGR_HPRE_MSK;
	presc = apbahb_presc_tbl[tmp];
	clock_val[CLOCK_HCLK] = clock_val[CLOCK_SYSCLK] >> presc;

	/*
	 * Get PCLK1
	 */
	tmp  = STM32_RCC->cfgr >> STM32_RCC_CFGR_PPRE1_BIT;
	tmp &= STM32_RCC_CFGR_PPRE1_MSK;
	presc = apbahb_presc_tbl[tmp];
	clock_val[CLOCK_PCLK1] = clock_val[CLOCK_HCLK] >> presc;

	/*
	 * Get PCLK2
	 */
	tmp  = STM32_RCC->cfgr >> STM32_RCC_CFGR_PPRE2_BIT;
	tmp &= STM32_RCC_CFGR_PPRE2_MSK;
	presc = apbahb_presc_tbl[tmp];
	clock_val[CLOCK_PCLK2] = clock_val[CLOCK_HCLK] >> presc;

	/*
	 * Set SYSTICK. Divider "8" is the SOC hardcoded.
	 */
	 clock_val[CLOCK_SYSTICK] = clock_val[CLOCK_HCLK] / 8;

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
