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

/*
 * Configuration settings for the Freescale TWR-K70F120M board.
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
 * Choose Kinetis MCU maximum core frequency
 */
#define CONFIG_KINETIS_120MHZ

/*
 * Enable GPIO driver
 */
#define CONFIG_KINETIS_GPIO
/* Number of GPIO ports (A..F on K70) */
#define KINETIS_GPIO_PORTS	6

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#define CONFIG_SYS_BOARD_REV_STR	"1"

/*
 * Monitor prompt
 */
#define CONFIG_SYS_PROMPT		"TWR-K70F120M> "

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
 * See also the description of the J19 jumper on the TWR-K70F120M board.
 * The PHY clock drives EXTAL0.
 */
#define KINETIS_OSC0_RATE	50000000
/* Use external reference clock from Ethernet PHY as main MCG input */
#define KINETIS_MCG_PLLREFSEL	0	/* OSC0 */
/*
 * The EXTAL rate divided by the divisor value (2**10 = 1024) specified by this
 * constant should be as close to the 32..40 kHz range as possible.
 */
#define KINETIS_MCG_FRDIV_POW	10
/* Core/system clock divider: 120/1 = 120 MHz */
#define KINETIS_CCLK_DIV	1
/* Peripheral clock divider: 120/2 = 60 MHz */
#define KINETIS_PCLK_DIV	2
/* FlexBus clock divider: 120/3 = 40 MHz */
#define KINETIS_FLEXBUS_CLK_DIV	3
/* Flash clock divider: 120/5 = 24 MHz */
#define KINETIS_FLASH_CLK_DIV	5
/* NFC clock divider: PLL0/5 = 120/5 = 24 MHz */
#define KINETIS_NFCCLK_DIV	5
/* NFC clock fraction: do no multiply */
#define KINETIS_NFCCLK_FRAC	1
/* LCDC clock divider: PLL/4 = 120/4 = 30 MHz */
#define KINETIS_LCDCCLK_DIV	4
/* LCDC clock fraction: do no multiply */
#define KINETIS_LCDCCLK_FRAC	1
/* PLL input divider: 50/5 = 10 MHz */
#define KINETIS_PLL_PRDIV	5
/* PLL multiplier: 10*24/2 = 120 MHz */
#define KINETIS_PLL_VDIV	24
/* PLL1 input divider: 50/5 = 10 MHz */
#define KINETIS_PLL1_PRDIV	5
/* PLL1 multiplier: 10*24/2 = 120 MHz */
#define KINETIS_PLL1_VDIV	24
/* Use PLL1 for MCGOUT (required for synchronous mode of the DDR controller) */
#define KINETIS_MCGOUT_PLL1
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
#define CONFIG_SYS_RAM_SIZE		(128 * 1024 * 1024)

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
 * UART2 is connected to the RS-232 port on the TWR-SER board
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
/* Only the RMII mode is possible on the TWR-K70F120M board */
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
 * We limit the MDC rate to 800 kHz, because the rate of 2.5 MHz leads to data
 * corruption when reading the PHY registers (we experienced data corruptions
 * on TWR-K60N512.)
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
 */
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_RAM_BASE

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
#define CONFIG_HOSTNAME			twr-k70f120m
#define CONFIG_BOOTARGS			"kinetis_platform=twr-k70f120m "\
					"console=ttyS2,115200 panic=10"
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
	"addip=setenv bootargs ${bootargs} "			\
		"ip=${ipaddr}:${serverip}:${gatewayip}:"	\
			"${netmask}:${hostname}:eth0:off\0"	\
	"ethaddr=C0:B1:3C:77:88:AA\0"				\
	"ipaddr=172.17.6.36\0"					\
	"serverip=172.17.0.1\0"					\
	"image=k70/uImage\0"					\
	"netboot=tftp ${image};run addip;bootm\0"		\
	"flashaddr=00100000\0"					\
	"flashboot=nboot ${loadaddr} 0 ${flashaddr};"		\
		"run addip;bootm\0"				\
	"update=tftp ${image};"					\
		"nand erase ${flashaddr} ${filesize};"		\
		"nand write ${loadaddr} ${flashaddr} ${filesize}\0"

/*
 * Linux kernel boot parameters configuration
 */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG

#endif /* __CONFIG_H */
