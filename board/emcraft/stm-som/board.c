/*
 * (C) Copyright 2011, 2012, 2013, 2015
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
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
 * Board specific code for the Emcraft Systems STM-SOM
 */

#include <common.h>
#include <netdev.h>
#include <ili932x.h>

#include <asm/arch/stm32.h>
#include <asm/arch/stm32f2_gpio.h>

#if CONFIG_SYS_BOARD_REV == 0x2A
# include <asm/arch/fmc.h>
# include <flash.h>
# include <asm/io.h>
# include <asm/system.h>
#endif

#include <asm/arch/fsmc.h>

#if CONFIG_SYS_BOARD_REV == 0x1A && (CONFIG_NR_DRAM_BANKS > 0)
/*
 * Check if RAM configured
 */
# if !defined(CONFIG_SYS_RAM_CS) || !defined(CONFIG_SYS_FSMC_PSRAM_BCR) ||     \
     !defined(CONFIG_SYS_FSMC_PSRAM_BTR)
#  error "Incorrect PSRAM FSMC configuration."
# endif
#endif /* CONFIG_NR_DRAM_BANKS */

DECLARE_GLOBAL_DATA_PTR;

static const struct stm32f2_gpio_dsc ext_ram_fsmc_fmc_gpio[] = {
	/* Chip is UFBGA176, see DM00077036.pdf for details */
	/* N15, FMC_D15 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_10},
	/* P14, FMC_D14 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_9},
	/* P15, FMC_D13 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_8},
	/* R11, FMC_D12 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_15},
	/* P11, FMC_D11 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_14},
	/* N11, FMC_D10 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_13},
	/* R10, FMC_D9 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_12},
	/* P10, FMC_D8 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_11},
	/* R9, FMC_D7 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_10},
	/* P9, FMC_D6 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_9},
	/* P8, FMC_D5 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_8},
	/* R8, FMC_D4 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_7},
	/* C12, FMC_D3 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_1},
	/* B12, FMC_D2 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_0},
	/* L14, FMC_D1 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_15},
	/* M14, FMC_D0 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_14},

	/* A3, FMC_NBL1 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_1},
	/* A4, FMC_NBL0 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_0},

	/* D10, FMC_NOE */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_4},
	/* C11, FMC_NWE */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_5},


	/* B3, FMC_A22 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_6},
	/* B2, FMC_A21 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_5},
	/* B1, FMC_A20 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_4},
	/* A1, FMC_A19 */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_3},
	/* M15, FMC_A18 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_13},
	/* N13, FMC_A17 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_12},
	/* N14, FMC_A16 */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_11},

	/* K13, FMC_A15, BA1 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_5},
	/* K14, FMC_A14, BA0 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_4},
	/* K15, FMC_A13 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_3},
	/* L15, FMC_A12 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_2},
	/* M7, FMC_A11 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_1},
	/* N7, FMC_A10 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_0},
	/* P7, FMC_A9 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_15},
	/* R7, FMC_A8 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_14},
	/* N6, FMC_A7 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_13},
	/* P6, FMC_A6 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_12},
	/* K3, FMC_A5 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_5},
	/* J3, FMC_A4 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_4},
	/* J2, FMC_A3 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_3},
	/* H2, FMC_A2 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_2},
	/* H3, FMC_A1 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_1},
	/* E2, FMC_A0 */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_0},

#if CONFIG_SYS_BOARD_REV == 0x2A
	/* SDRAM only, Revision 0x2A */
	/* M4, SDRAM_NE */
	{STM32F2_GPIO_PORT_C, STM32F2_GPIO_PIN_2},
	/* R6, SDRAM_NRAS */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_11},
	/* B7, SDRAM_NCAS */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_15},
	/* J4, SDRAM_NWE */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_5},
	/* M5, SDRAM_CKE */
	{STM32F2_GPIO_PORT_C, STM32F2_GPIO_PIN_3},

	/* H14, SDRAM_CLK */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_8},
#endif /* CONFIG_SYS_BOARD_REV == 0x2A */

#if CONFIG_SYS_BOARD_REV == 0x1A
	/* PSRAM only */
	/* B11, FMC_NWAIT */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_6},
	/* A2, FMC_CRE */
	{STM32F2_GPIO_PORT_E, STM32F2_GPIO_PIN_2},
	/* B5, FMC_NL */
	{STM32F2_GPIO_PORT_B, STM32F2_GPIO_PIN_7},
	/* D11, FMC_CLK */
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_3},
#endif /* CONFIG_SYS_BOARD_REV == 0x1A */

#ifdef CONFIG_FSMC_NOR_PSRAM_CS1_ENABLE
	{STM32F2_GPIO_PORT_D, STM32F2_GPIO_PIN_7},
#endif
#ifdef CONFIG_FSMC_NOR_PSRAM_CS2_ENABLE
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_9},
#endif
#ifdef CONFIG_FSMC_NOR_PSRAM_CS3_ENABLE
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_10},
#endif
#ifdef CONFIG_FSMC_NOR_PSRAM_CS4_ENABLE
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_12},
#endif
};

#ifdef CONFIG_VIDEO_STM32F4_LTDC
static const struct stm32f2_gpio_dsc ltdc_iomux[] = {
	/* PG7  = LCD_CLK */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_7},
	/* PF10 = LCD_DE */
	{STM32F2_GPIO_PORT_F, STM32F2_GPIO_PIN_10},
	/* PI10 = LCD_HSYNC */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_10},
	/* PI9  = LCD_VSYNC */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_9},
	/* PG12 = LCD_B1 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_12},
	/* PG10 = LCD_B2 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_10},
	/* PI4  = LCD_B4 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_4},
	/* PI5  = LCD_B5 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_5},
	/* PI6  = LCD_B6 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_6},
	/* PI7  = LCD_B7 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_7},
	/* PH13 = LCD_G2 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_13},
	/* PH14 = LCD_G3 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_14},
	/* PH15 = LCD_G4 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_15},
	/* PI0  = LCD_G5 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_0},
	/* PC7  = LCD_G6 */
	{STM32F2_GPIO_PORT_C, STM32F2_GPIO_PIN_7},
	/* PI2  = LCD_G7 */
	{STM32F2_GPIO_PORT_I, STM32F2_GPIO_PIN_2},
	/* PH2  = LCD_R0 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_2},
	/* PH3  = LCD_R1 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_3},
	/* PH8  = LCD_R2 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_8},
	/* PH9  = LCD_R3 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_9},
	/* PH10 = LCD_R4 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_10},
	/* PH11 = LCD_R5 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_11},
	/* PH12 = LCD_R6 */
	{STM32F2_GPIO_PORT_H, STM32F2_GPIO_PIN_12},
	/* PG6  = LCD_R7 */
	{STM32F2_GPIO_PORT_G, STM32F2_GPIO_PIN_6},
};
#endif /* CONFIG_VIDEO_STM32F4_LTDC */

/*
 * Init FMC/FSMC GPIOs based
 */
static int fmc_fsmc_setup_gpio(void)
{
	int rv = 0;
	int i;

	/*
	 * Connect GPIOs to FMC controller
	 */
	for (i = 0; i < ARRAY_SIZE(ext_ram_fsmc_fmc_gpio); i++) {
		rv = stm32f2_gpio_config(&ext_ram_fsmc_fmc_gpio[i],
				STM32F2_GPIO_ROLE_FMC);
		if (rv)
			goto out;
	}

	fsmc_gpio_init_done = 1;
out:
	return rv;
}

#ifdef CONFIG_VIDEO_STM32F4_LTDC
/*
 * Initialize LCD pins
 */
static int ltdc_setup_iomux(void)
{
	int rv = 0;
	int i;

	/*
	 * Connect GPIOs to FMC controller
	 */
	for (i = 0; i < ARRAY_SIZE(ltdc_iomux); i++) {
		rv = stm32f2_gpio_config(&ltdc_iomux[i],
				STM32F2_GPIO_ROLE_LTDC);
		if (rv)
			break;
	}

	return rv;
}
#endif /* CONFIG_VIDEO_STM32F4_LTDC */

/*
 * Early hardware init.
 */
int board_init(void)
{
	int rv;
#if CONFIG_SYS_BOARD_REV == 0x2A
	int i;
	char v;
#endif

	rv = fmc_fsmc_setup_gpio();
	if (rv)
		return rv;

#if !defined(CONFIG_SYS_NO_FLASH)

#if CONFIG_SYS_BOARD_REV == 0x2A
	/* Disable first bank */
	fsmc_nor_psram_init(1, 0, 0, 0);
	fsmc_nor_psram_init(3, 0, 0, 0);
	fsmc_nor_psram_init(4, 0, 0, 0);

	/*
	 * Put SDRAM in Self-refresh mode to workaround
	 * bug with Flash/SDRAM accessing,
	 * see errata 2.8.7
	 */
	STM32_RCC->ahb3enr |= 1;
	__asm__ __volatile__ ("dsb" : : : "memory");

	STM32_SDRAM_FMC->sdcr1 =
		CONFIG_SYS_RAM_FREQ_DIV << FMC_SDCR_SDCLK_SHIFT;

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_START_CLOCK;
	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_SELFREFRESH;
	FMC_BUSY_WAIT();
	udelay(60);
#endif

	if ((rv = fsmc_nor_psram_init(CONFIG_SYS_FLASH_CS, CONFIG_SYS_FSMC_FLASH_BCR,
			CONFIG_SYS_FSMC_FLASH_BTR,
			CONFIG_SYS_FSMC_FLASH_BWTR)))
		return rv;

#if CONFIG_SYS_BOARD_REV == 0x2A
	for (i = 1; i < 0x1000000; i <<= 1) {
		v = *(volatile char*)(0x64000000 + i);
		v = *(volatile char*)(0x64000000 + i - 1);
		nop(); nop();
		nop(); nop();
		nop(); nop();
	}
#endif
#endif

#ifdef CONFIG_VIDEO_STM32F4_LTDC
	rv = ltdc_setup_iomux();
	if (rv)
		return rv;
#endif /* CONFIG_VIDEO_STM32F4_LTDC */

	return 0;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
	printf("Board: STM-SOM Rev %s, www.emcraft.com\n",
		CONFIG_SYS_BOARD_REV_STR);

	return 0;
}

#if CONFIG_SYS_BOARD_REV == 0x1A
/*
 * Setup external RAM.
 */
int dram_init(void)
{
	static struct stm32f2_gpio_dsc	ctrl_gpio = {STM32F2_GPIO_PORT_I,
						     STM32F2_GPIO_PIN_9};
	int				rv = 0;

	rv = fsmc_nor_psram_init(CONFIG_SYS_RAM_CS,
			CONFIG_SYS_FSMC_PSRAM_BCR,
			CONFIG_SYS_FSMC_PSRAM_BTR,
#ifdef CONFIG_SYS_FSMC_PSRAM_BWTR
			CONFIG_SYS_FSMC_PSRAM_BWTR
#else
			(u32)-1
#endif
		);
	if (rv != 0)
		goto out;

	rv = stm32f2_gpio_config(&ctrl_gpio, STM32F2_GPIO_ROLE_GPOUT);
	if (rv != 0)
		goto out;

# if defined(CONFIG_SYS_RAM_BURST)
	/*
	 * FIXME: all this hardcoded stuff.
	 */

	/* Step.2 */
	stm32f2_gpout_set(&ctrl_gpio, 1);

	/* Step.3 */
	*(volatile u16 *)(CONFIG_SYS_RAM_BASE + 0x0010223E) = 0;

	/* Step.4-5 */
	stm32f2_gpout_set(&ctrl_gpio, 0);

	/* Step.6 */
	fsmc_nor_psram_init(CONFIG_SYS_RAM_CS, 0x00083115,
			0x0010FFFF, -1);

	/* Step.7 */
	rv = *(volatile u16 *)(CONFIG_SYS_RAM_BASE + 0x000000);

	/* Step.8 */
	fsmc_nor_psram_init(CONFIG_SYS_RAM_CS, 0x00005059,
			0x10000702, 0x10000602);

	/* Step.9 */
	stm32f2_gpout_set(&ctrl_gpio, 1);

	/* Step.10 */
	*(volatile u16 *)(CONFIG_SYS_RAM_BASE + 0x0110223E) = 0;

	/* Step.11 */
	stm32f2_gpout_set(&ctrl_gpio, 0);

	/* Step.12 */
	fsmc_nor_psram_init(CONFIG_SYS_RAM_CS, 0x00083115,
			0x0010FFFF, -1);

	/* Step.13 */
	rv = *(volatile u16 *)(CONFIG_SYS_RAM_BASE + 0x01000000);

# else
	/*
	 * Switch PSRAM in the Asyncronous Read/Write Mode
	 */
	stm32f2_gpout_set(&ctrl_gpio, 0);
# endif /* CONFIG_SYS_RAM_BURST */

	/*
	 * Fill in global info with description of SRAM configuration
	 */
	gd->bd->bi_dram[0].start = CONFIG_SYS_RAM_BASE;
	gd->bd->bi_dram[0].size  = CONFIG_SYS_RAM_SIZE;

	rv = 0;

out:
	return rv;
}
#endif /* CONFIG_SYS_BOARD_REV == 0x1A */

#if CONFIG_SYS_BOARD_REV == 0x2A
/*
 * STM32 RCC FMC specific definitions
 */
#define STM32_RCC_ENR_FMC		(1 << 0)	/* FMC module clock  */

static int dram_initialized = 0;


static inline u32 _ns2clk(u32 ns, u32 freq)
{
	uint32_t tmp = freq/1000000;
	return (tmp * ns) / 1000;
}

#define NS2CLK(ns) (_ns2clk(ns, freq))

/*
 * Following are timings for M12L2561616A-6BI, from corresponding datasheet
 */
#define SDRAM_CAS	3
#define SDRAM_NB	1	/* Number of banks */
#define SDRAM_MWID	1	/* 16 bit memory */

#define SDRAM_NR	0x2	/* 13-bit row */
#define SDRAM_NC	0x1	/* 9-bit col */

#define SDRAM_TRRD	NS2CLK(12)
#define SDRAM_TRCD	NS2CLK(18)
#define SDRAM_TRP	NS2CLK(18)
#define SDRAM_TRAS	NS2CLK(42)
#define SDRAM_TRC	NS2CLK(60)
#define SDRAM_TRFC	NS2CLK(60)
#define SDRAM_TCDL	(1 - 1)
#define SDRAM_TRDL	NS2CLK(12)
#define SDRAM_TBDL	(1 - 1)
#define SDRAM_TREF	(NS2CLK(64000000 / 8192) - 20)
#define SDRAM_TCCD	(1 - 1)

#define SDRAM_TXSR	SDRAM_TRFC	/* Row cycle time after precharge */
#define SDRAM_TMRD	(3 - 1)		/* Page 10, Mode Register Set */

/* Last data in to row precharge, need also comply ineq on page 1648 */
#define SDRAM_TWR	max(\
	(int)max((int)SDRAM_TRDL, (int)(SDRAM_TRAS - SDRAM_TRCD)), \
	(int)(SDRAM_TRC - SDRAM_TRCD - SDRAM_TRP)\
)

int dram_init(void)
{
	u32 freq;
	int rv;

	/*
	 * Enable FMC interface clock
	 */
	STM32_RCC->ahb3enr |= STM32_RCC_ENR_FMC;

	/*
	 * Get frequency for NS2CLK calculation.
	 */
	freq = clock_get(CLOCK_HCLK) / CONFIG_SYS_RAM_FREQ_DIV;

	STM32_SDRAM_FMC->sdcr1 = (
		CONFIG_SYS_RAM_FREQ_DIV << FMC_SDCR_SDCLK_SHIFT |
		SDRAM_CAS << FMC_SDCR_CAS_SHIFT |
		SDRAM_NB << FMC_SDCR_NB_SHIFT |
		SDRAM_MWID << FMC_SDCR_MWID_SHIFT |
		SDRAM_NR << FMC_SDCR_NR_SHIFT |
		SDRAM_NC << FMC_SDCR_NC_SHIFT |
		0 << FMC_SDCR_RPIPE_SHIFT |
		1 << FMC_SDCR_RBURST_SHIFT
	);

	STM32_SDRAM_FMC->sdtr1 = (
		SDRAM_TRCD << FMC_SDTR_TRCD_SHIFT |
		SDRAM_TRP << FMC_SDTR_TRP_SHIFT |
		SDRAM_TWR << FMC_SDTR_TWR_SHIFT |
		SDRAM_TRC << FMC_SDTR_TRC_SHIFT |
		SDRAM_TRAS << FMC_SDTR_TRAS_SHIFT |
		SDRAM_TXSR << FMC_SDTR_TXSR_SHIFT |
		SDRAM_TMRD << FMC_SDTR_TMRD_SHIFT
	);

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_START_CLOCK;

	udelay(200);	/* 200 us delay, page 10, "Power-Up" */
	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_PRECHARGE;

	udelay(100);
	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = (
		FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_AUTOREFRESH |
		7 << FMC_SDCMR_NRFS_SHIFT
	);

	udelay(100);
	FMC_BUSY_WAIT();


#define SDRAM_MODE_BL_SHIFT		0
#define SDRAM_MODE_CAS_SHIFT		4

#define SDRAM_MODE_BL			0
#define SDRAM_MODE_CAS			SDRAM_CAS

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 |
	(
		SDRAM_MODE_BL << SDRAM_MODE_BL_SHIFT |
		SDRAM_MODE_CAS << SDRAM_MODE_CAS_SHIFT
	) << FMC_SDCMR_MODE_REGISTER_SHIFT | FMC_SDCMR_MODE_WRITE_MODE;

	udelay(100);

	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_NORMAL;

	FMC_BUSY_WAIT();

	/* Refresh timer */
	STM32_SDRAM_FMC->sdrtr = SDRAM_TREF;

	/*
	 * Fill in global info with description of SRAM configuration
	 */
	gd->bd->bi_dram[0].start = CONFIG_SYS_RAM_BASE;
	gd->bd->bi_dram[0].size  = CONFIG_SYS_RAM_SIZE;

	rv = 0;

	cortex_m3_mpu_full_access();

	dram_initialized = 1;

	return rv;
}

/*
 * STM32 Flash bug workaround.
 */
extern char	_mem_ram_buf_base, _mem_ram_buf_size;

#define SOC_RAM_BUFFER_BASE	(ulong)(&_mem_ram_buf_base)
#define SOC_RAM_BUFFER_SIZE	(ulong)((&_mem_ram_buf_size) - 0x100)

void stop_ram(void)
{
	if (!dram_initialized)
		return;

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_SELFREFRESH;

	FMC_BUSY_WAIT();
}

void start_ram(void)
{
	if (!dram_initialized)
		return;

	/*
	 * Precharge according to chip requirement, page 12.
	 */

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_PRECHARGE;
	FMC_BUSY_WAIT();


	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_NORMAL;
	FMC_BUSY_WAIT();

	udelay(60);
}

#define NOP10()		do {	nop(); nop(); nop(); nop(); nop(); \
				nop(); nop(); nop(); nop(); nop(); \
			} while(0);

u16 flash_read16(void *addr)
{
	u16 value;
	stop_ram();
	value = __raw_readw(addr);
	NOP10();
	start_ram();
	return value;
}

void flash_write16(u16 value, void *addr)
{
	stop_ram();
	__raw_writew(value, addr);
	NOP10();
	NOP10();
	start_ram();
}

__attribute__((noinline)) void copy_one(volatile u16* src, volatile u16* dst)
{
	*dst = *src;
}

u32 flash_write_buffer(void *src, void *dst, int cnt, int portwidth)
{
	u32 retval = 0;

	if (portwidth != FLASH_CFI_16BIT) {
		retval = ERR_INVAL;
		goto out;
	}

	memcpy((void*)SOC_RAM_BUFFER_BASE, (void*)src, cnt * portwidth);

	stop_ram();
	__asm__ __volatile__("": : :"memory");

	src = (void*) SOC_RAM_BUFFER_BASE;

	while(cnt-- > 0) {
		copy_one(src, dst);
		src += 2, dst += 2;
		NOP10();
		NOP10();
	}

	__asm__ __volatile__("": : :"memory");
	start_ram();
out:
	return retval;
}

u32 flash_check_flag(void *src, void *dst, int cnt, int portwidth)
{
	u32 flag = 1;

	if (portwidth != FLASH_CFI_16BIT) {
		flag = 0;
		goto out;
	}

	stop_ram();

	while((cnt-- > 0) && (flag == 1)) {
		flag = *(u16*)dst == 0xFFFF;
		dst += 2;
	}

	start_ram();

out:
	return flag;
}

#endif /* CONFIG_SYS_BOARD_REV == 0x2A */


#ifdef CONFIG_STM32_ETH
/*
 * Register ethernet driver
 */
int board_eth_init(bd_t *bis)
{
	return stm32_eth_init(bis);
}
#endif

