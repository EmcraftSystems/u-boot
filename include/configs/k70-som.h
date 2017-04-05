/*
 * (C) Copyright 2011, 2012
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

/*
 * Configuration settings for the Emcraft K70-SOM board
 * on the Emcraft SOM-BSB Baseboard.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * Disable debug messages
 */
#undef DEBUG

/*
 * This is an ARM Cortex-M4 CPU core which is backward-compatible with Cortex-M3
 */
#define CONFIG_SYS_ARMCORTEXM3

/*
 * This is a Kinetis-based device
 */
#define CONFIG_SYS_KINETIS

/*
 * Choose Kinetis MCU family and maximum core frequency
 * Default is K70 @120MHz and can be redefined by config.h
 */
#ifndef CONFIG_KINETIS_K61
#define CONFIG_KINETIS_K70
#endif

#ifndef CONFIG_KINETIS_120MHZ
#define CONFIG_KINETIS_150MHZ
#endif

/*
 * Enable GPIO driver
 */
#define CONFIG_KINETIS_GPIO
/* Number of GPIO ports (A..F on K70) */
#define KINETIS_GPIO_PORTS	6

/*
 * This is a specific revision of the board
 */
#define CONFIG_SYS_BOARD_REV		0x1A

#if (CONFIG_SYS_BOARD_REV != 0x1A)
#error CONFIG_SYS_BOARD_REV must be 1A
#endif

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#if (CONFIG_SYS_BOARD_REV == 0x1A)
#define CONFIG_SYS_BOARD_REV_STR	"1.A"
#endif

/*
 * Monitor prompt and hostname
 */
#if defined(CONFIG_KINETIS_K70)
/* Kinetis K70 */
#define CONFIG_SYS_PROMPT		"K70-SOM> "
#define CONFIG_HOSTNAME			k70-som
#define CONFIG_KINETIS_PLATFORM		"k70-som"
#define KINETIS_HAS_LCD
#elif defined(CONFIG_KINETIS_K61)
/* Kinetis K61 */
#define CONFIG_SYS_PROMPT		"K61-SOM> "
#define CONFIG_HOSTNAME			k61-som
#define CONFIG_KINETIS_PLATFORM		"k61-som"
#undef KINETIS_HAS_LCD
#else
#error No Kinetis MCU family specified
#endif

/*
 * We want to call the CPU specific initialization
 */
#define CONFIG_ARCH_CPU_INIT

/*
 * This ensures that the SoC-specific cortex_m3_soc_init() gets invoked.
 */
#define CONFIG_ARMCORTEXM3_SOC_INIT

/*
 * Clock configuration (see cpu/arm_cortexm3/kinetis/clock.c for details)
 */
/* Select MCG configuration type */
#define CONFIG_KINETIS_K70_120MHZ
/*
 * Clock rate at the EXTAL0 input
 *
 * The MCU uses the external 50 MHz clock from the Ethernet PHY. This is
 * possible because the PHY's REF_CLK output clock signal is connected
 * to the EXTAL0 pin of the MCU.
 */
#define KINETIS_OSC0_RATE	50000000
/* Frequency of the oscillator at XTAL1/EXTAL1 */
#define KINETIS_OSC1_RATE	12000000
/* Use crystal oscillator connected to OSC1 as main MCG input */
#define KINETIS_MCG_PLLREFSEL	1	/* OSC1 */
#define KINETIS_MCG_EXT_CRYSTAL
/*
 * Use RTC oscillator for FLL reference clock, because on K70-SOM/DNI-ETH
 * we do not have reference clock from Ethernet PHY on EXTAL0.
 */
#define KINETIS_FLLREF_RTC
/*
 * The EXTAL32 rate (32 kHz) divided by the divisor value (2**0 = 1)
 * specified by this constant should be as close to the 32..40 kHz
 * range as possible.
 */
#define KINETIS_MCG_FRDIV_POW	0
/* Core/system clock divider: 120/1 = 120MHz or 150/1 = 150MHz */
#define KINETIS_CCLK_DIV	1
/* Peripheral clock divider: 120/2 = 60MHz or 150/2 = 75MHz */
#define KINETIS_PCLK_DIV	2
/* FlexBus clock divider: 120/3 = 40MHz or 150/3 = 50MHz */
#define KINETIS_FLEXBUS_CLK_DIV	3

#if defined(CONFIG_KINETIS_120MHZ)
/*
 * 120MHz MCU
 */
/* Flash clock divider: 120/5 = 24 MHz */
#define KINETIS_FLASH_CLK_DIV	5
/* NFC clock divider: PLL0/7 = 120/7 = 17.14 MHz */
#define KINETIS_NFCCLK_DIV	7
/* NFC clock fraction: do no multiply */
#define KINETIS_NFCCLK_FRAC	1

#ifdef KINETIS_HAS_LCD
/* LCDC clock divider: PLL/4 = 120/4 = 30 MHz */
#define KINETIS_LCDCCLK_DIV	4
/* LCDC clock fraction: do no multiply */
#define KINETIS_LCDCCLK_FRAC	1
#endif /* KINETIS_HAS_LCD */

/* PLL input divider: 12/1 = 12 MHz */
#define KINETIS_PLL_PRDIV	1
/* PLL multiplier: 12*20/2 = 120 MHz */
#define KINETIS_PLL_VDIV	20
/* PLL1 input divider: 12/1 = 12 MHz */
#define KINETIS_PLL1_PRDIV	1
/* PLL1 multiplier: 12*20/2 = 120 MHz */
#define KINETIS_PLL1_VDIV	20

#elif defined(CONFIG_KINETIS_150MHZ)
/*
 * 150MHz MCU
 */
/* Flash clock divider: 150/6 = 25 MHz */
#define KINETIS_FLASH_CLK_DIV	6
/* NFC clock divider: PLL0/8 = 150/8 = 18.75 MHz */
#define KINETIS_NFCCLK_DIV	8
/* NFC clock fraction: do no multiply */
#define KINETIS_NFCCLK_FRAC	1

#ifdef KINETIS_HAS_LCD
/* LCDC clock divider: PLL/5 = 150/5 = 30 MHz */
#define KINETIS_LCDCCLK_DIV	5
/* LCDC clock fraction: do no multiply */
#define KINETIS_LCDCCLK_FRAC	1
#endif /* KINETIS_HAS_LCD */

/* PLL input divider: 12/1 = 12 MHz */
#define KINETIS_PLL_PRDIV	1
/* PLL multiplier: 12*16/2 = 96 MHz */
#define KINETIS_PLL_VDIV	16
/* PLL1 input divider: 12/1 = 12 MHz */
#define KINETIS_PLL1_PRDIV	1
/* PLL1 multiplier: 12*25/2 = 150 MHz */
#define KINETIS_PLL1_VDIV	25

#endif

/* Use PLL1 for MCGOUT (required for synchronous mode of the DDR controller) */
#define KINETIS_MCGOUT_PLL1

/* Use PLL0 (required for USB-FS) */
#define KINETIS_MCGOUT_PLL0

/*
 * Number of clock ticks in 1 sec
 */
#define CONFIG_SYS_HZ			1000

/*
 * The SYSTICK Timer's clock source is always the core clock, the MCU hardware
 * does not allow to change this.
 */
#define CONFIG_ARMCORTEXM3_SYSTICK_CPU

/*
 * Enable/disable h/w watchdog
 */
#undef CONFIG_HW_WATCHDOG

/*
 * No interrupts
 */
#undef CONFIG_USE_IRQ

/*
 * Cache configuration
 */
#define CONFIG_KINETIS_PC_CACHE_ON
/* #undef CONFIG_KINETIS_PC_CACHE_ON */
#define CONFIG_KINETIS_PS_CACHE_ON
/* #undef CONFIG_KINETIS_PS_CACHE_ON */

/*
 * Configuration of the external DDR2 SDRAM memory
 */
#define CONFIG_KINETIS_DDR
#define CONFIG_KINETIS_DDR_SYNC		/* DDR synchronous mode */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_RAM_CS		0
#define CONFIG_SYS_RAM_BASE		0x70000000
#define CONFIG_SYS_RAM_ALIAS		0x80000000
#define CONFIG_SYS_RAM_SIZE		(64 * 1024 * 1024)

/*
 * Memory layout configuration
 */
#define CONFIG_MEM_NVM_BASE		0x00000000
#define CONFIG_MEM_NVM_LEN		(1024 * 1024)
/*
 * 128 kB of SRAM centered at 0x20000000
 * SRAM_L: 0x1FFF0000 - 0x1FFFFFFF (64 kB)
 * SRAM_U: 0x20000000 - 0x2000FFFF (64 kB)
 */
#define CONFIG_MEM_RAM_BASE		0x1FFF0000
#define CONFIG_MEM_RAM_LEN		(32 * 1024)
#define CONFIG_MEM_RAM_BUF_LEN		(74 * 1024)
#define CONFIG_MEM_MALLOC_LEN		(18 * 1024)
#define CONFIG_MEM_STACK_LEN		(4 * 1024)
/*
 * Use 1 MB at the end of the external memory for the malloc() pool
 */
#define CONFIG_SYS_MALLOC_EXT_LEN	(1024 * 1024)
#define CONFIG_SYS_MALLOC_EXT_BASE \
	(CONFIG_SYS_RAM_ALIAS + CONFIG_SYS_RAM_SIZE - CONFIG_SYS_MALLOC_EXT_LEN)
/*
 * The generic code still needs CONFIG_SYS_MALLOC_LEN to calculate the base
 * address of the global data (`gd`) structure.
 */
#define CONFIG_SYS_MALLOC_LEN		CONFIG_MEM_MALLOC_LEN

/*
 * Program flash configuration
 */
#define CONFIG_ENVM_TYPE_K70

/*
 * Configuration of the external Flash memory
 */
/* No NOR flash present */
#define CONFIG_SYS_NO_FLASH
/* NAND Flash configuration */
#define CONFIG_NAND_FSL_NFC
#define CONFIG_NAND_FSL_NFC_BUSWIDTH_8
#define CONFIG_SYS_NAND_BASE		0x400A8000
#define CONFIG_SYS_MAX_NAND_DEVICE	1
#define NAND_MAX_CHIPS			CONFIG_SYS_MAX_NAND_DEVICE
#define CONFIG_SYS_NAND_SELECT_DEVICE
#define CONFIG_SYS_64BIT_VSPRINTF	/* needed for nand_util.c */

/*
 * Store environment in the NAND Flash
 */
#define CONFIG_ENV_IS_IN_NAND
#define CONFIG_ENV_OFFSET	0
#define CONFIG_ENV_SIZE		0x20000
#define CONFIG_ENV_SECT_SIZE	0x20000
/* Allocate 4 blocks for environment in case three of them are bad blocks */
#define CONFIG_ENV_RANGE	(CONFIG_ENV_SECT_SIZE * 4)
/* Allocate another 4 blocks for the redundant copy of environment */
#define CONFIG_ENV_SIZE_REDUND		CONFIG_ENV_SIZE
#define CONFIG_ENV_OFFSET_REDUND	(CONFIG_ENV_OFFSET + CONFIG_ENV_RANGE)
#define CONFIG_ENV_OVERWRITE		1

/*
 * Serial console configuration
 */
#define CONFIG_KINETIS_UART_CONSOLE
/*
 * UART2 is connected to the USB-to-serial adapter on the SOM-BSB Baseboard.
 * UART2 pin configuration: Rx: PORT_E.17, Tx = PORT_E.16
 */
#define CONFIG_KINETIS_UART_PORT	2	/* UART2 */
#define CONFIG_KINETIS_UART_RX_IO_PORT	4	/* PORT E */
#define CONFIG_KINETIS_UART_RX_IO_PIN	17	/* pin 17 */
#define CONFIG_KINETIS_UART_RX_IO_FUNC	3	/* UART2_RX */
#define CONFIG_KINETIS_UART_TX_IO_PORT	4	/* PORT E */
#define CONFIG_KINETIS_UART_TX_IO_PIN	16	/* pin 16 */
#define CONFIG_KINETIS_UART_TX_IO_FUNC	3	/* UART2_TX */

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Ethernet configuration
 */
#define CONFIG_MCFFEC
/* Enable the RMII mode */
#undef CONFIG_MCFFEC_MII
#define CONFIG_NET_MULTI
#define CONFIG_MII
#define CONFIG_MII_INIT
/*
 * The value of CONFIG_SYS_FEC0_PINMUX does not matter.
 * This configuration option is required by the `mcffec.c` driver.
 */
#define CONFIG_SYS_FEC0_PINMUX		0
#define CONFIG_SYS_FEC0_IOBASE		0x400C0000
#define CONFIG_SYS_FEC0_MIIBASE		CONFIG_SYS_FEC0_IOBASE
/*
 * Ethernet buffer descriptor tables should be aligned on 512-byte boundaries
 */
#define CONFIG_SYS_CACHELINE_SIZE	512
#define MCFFEC_TOUT_LOOP		50000
#define CONFIG_SYS_DISCOVER_PHY
#define CONFIG_SYS_RX_ETH_BUFFER	8
/*
 * Options for the MDC clock
 */
/* Internal MAC clock rate */
#define CONFIG_MCFFEC_MAC_CLK		clock_get(CLOCK_MACCLK)
/*
 * We limit the MDC rate to 800 kHz, because the rate of 2.5 MHz may lead
 * to data corruptions on the K70-SOM + TWR-SOM-BSB platform when reading
 * from the PHY registers (we experienced data corruptions on TWR-K60N512.)
 */
#define CONFIG_MCFFEC_MII_SPEED_LIMIT	800000

/*
 * Console I/O buffer size
 */
#define CONFIG_SYS_CBSIZE		256

/*
 * Print buffer size
 */
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE + \
                                        sizeof(CONFIG_SYS_PROMPT) + 16)

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_RAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_RAM_BASE + \
					CONFIG_SYS_RAM_SIZE - \
					CONFIG_SYS_MALLOC_EXT_LEN)

/*
 * Needed by "loadb"
 * Set to be aligned with Linux entry point inside multi uImage
 * (64-byte header and 12-byte data for 2 embedded images)
 */
#define CONFIG_SYS_LOAD_ADDR		(0x8008000 - 64 - 12)

/*
 * Monitor is actually in eNVM. In terms of U-Boot, it is neither "flash",
 * not RAM, but CONFIG_SYS_MONITOR_BASE must be defined.
 */
#define CONFIG_SYS_MONITOR_BASE		0x0

/*
 * Monitor is not in flash. Needs to define this to prevent
 * U-Boot from running flash_protect() on the monitor code.
 */
#define CONFIG_MONITOR_IS_IN_RAM	1

/*
 * Enable all those monitor commands that are needed
 */
#include <config_cmd_default.h>
#undef CONFIG_CMD_BOOTD
#undef CONFIG_CMD_CONSOLE
#undef CONFIG_CMD_ECHO
#undef CONFIG_CMD_EDITENV
#undef CONFIG_CMD_FPGA
#undef CONFIG_CMD_IMI
#undef CONFIG_CMD_ITEST
#undef CONFIG_CMD_IMLS
#undef CONFIG_CMD_LOADS
#undef CONFIG_CMD_MISC
#define CONFIG_CMD_NET
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_SOURCE
#undef CONFIG_CMD_XIMG
#define CONFIG_CMD_NAND

/*
 * To save memory disable long help
 */
#undef CONFIG_SYS_LONGHELP

/*
 * Max number of command args
 */
#define CONFIG_SYS_MAXARGS		16

/*
 * Auto-boot sequence configuration
 */
#define CONFIG_BOOTDELAY		3
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_BOOTARGS			"console=ttymxc2,115200 panic=10"
#define CONFIG_BOOTCOMMAND		"run flashboot"

/*
 * This ensures that the board-specific misc_init_r() gets invoked.
 */
#define CONFIG_MISC_INIT_R

/*
 * Short-cuts to some useful commands (macros)
 */
#define CONFIG_EXTRA_ENV_SETTINGS				\
	"loadaddr=0x08007fc0\0"					\
	"args=setenv bootargs " CONFIG_BOOTARGS "\0"		\
	"addip=setenv bootargs ${bootargs} "			\
		"ip=${ipaddr}:${serverip}:${gatewayip}:"	\
			"${netmask}:${hostname}:eth0:off\0"	\
	"ethaddr=C0:B1:3C:77:88:AB\0"				\
	"ipaddr=172.17.6.46\0"					\
	"serverip=172.17.0.1\0"					\
	"image=k70/uImage\0"					\
	"netboot=tftp ${image} && run args addip && bootm\0"		\
	"flashaddr=00100000\0"					\
	"flashboot=nboot ${loadaddr} 0 ${flashaddr} && "		\
		"run args addip && bootm\0"				\
	"update=tftp ${image} && "					\
		"nand erase ${flashaddr} ${filesize} && "		\
		"nand write ${loadaddr} ${flashaddr} ${filesize}\0"

/*
 * Linux kernel boot parameters configuration
 */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG

#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "

/*
 * Enable support for booting with FDT
 */
#define CONFIG_OF_LIBFDT
#define CONFIG_SYS_BOOTMAPSZ		CONFIG_SYS_RAM_SIZE

#endif /* __CONFIG_H */
