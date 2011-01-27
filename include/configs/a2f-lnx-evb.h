/*
 * (C) Copyright 2010-2011 Emcraft Systems
 *
 * Configuration settings for the Emcraft A2F-LNX-EVB board.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * Disable debug messages
 */
#undef DEBUG

/*
 * This is an ARM Cortex-M3 CPU core
 */
#define CONFIG_SYS_ARMCORTEXM3

/*
 * This is the Actel SmartFusion (aka A2F) device
 */
#define CONFIG_SYS_A2F

/*
 * This is a specific revision of the board
 */
#define CONFIG_SYS_BOARD_REV		0x2A

#if (CONFIG_SYS_BOARD_REV!=0x1A && CONFIG_SYS_BOARD_REV!=0x2A)
#error CONFIG_SYS_BOARD_REV must be 1A or 2A
#endif

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#if (CONFIG_SYS_BOARD_REV==0x1A)
# define CONFIG_SYS_BOARD_REV_STR	"1.A"
#else
# define CONFIG_SYS_BOARD_REV_STR	"2.A"
#endif

/*
 * Monitor prompt
 */
#define CONFIG_SYS_PROMPT		"A2F-LNX-EVB> "

/*
 * We want to call the CPU specific initialization
 */
#define CONFIG_ARCH_CPU_INIT

/*
 * System frequency (FCLK) and the other derivative clocks
 * coming out from reset. These are defined by the Libero
 * project programmed onto SmartFusion.
 * It is possible to read these frequencies from SmartFusion
 * at run-time, however for simplicity of configuration we define these
 * clocks at build-time.
 */
#define CONFIG_SYS_CLK_FREQ		80000000uL
#define CONFIG_SYS_CLK_PCLK0		(CONFIG_SYS_CLK_FREQ / 4)
#define CONFIG_SYS_CLK_PCLK1		(CONFIG_SYS_CLK_FREQ / 4)
#define CONFIG_SYS_ACE_PCLK1		(CONFIG_SYS_CLK_FREQ / 2)
#define CONFIG_SYS_FPGA_PCLK1		(CONFIG_SYS_CLK_FREQ / 2)

/* How many clock ticks in 1 sec */
#define CONFIG_SYS_HZ			1000

/*
 * Enable/disable h/w watchdog
 */
#undef CONFIG_HW_WATCHDOG

/*
 * No interrupts
 */
#undef CONFIG_USE_IRQ

/*
 * Configuration of the external memory
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_RAM_BASE		0x70000000
#if (CONFIG_SYS_BOARD_REV==0x1A)
# define CONFIG_SYS_RAM_SIZE		(8 * 1024 * 1024)
#else
# define CONFIG_SYS_RAM_SIZE		(16 * 1024 * 1024)
#endif

/*
 * External Memory Controller settings
 * Slow, safe timings for external SRAM
#define CONFIG_SYS_EMC0CS0CR		0x00002aad
 */

/*
 * Optimized timings for external SRAM
 */
#define CONFIG_SYS_EMC0CS0CR		0x00002225

/*
 * Settings for the EMC MUX register
 */
#define CONFIG_SYS_EMCMUXCR		0x00000001

/*
 * Configuration of the external Flash
 */
#define CONFIG_SYS_FLASH_BANK1_BASE	0x74000000

/*
 * Timings for the external Flash
 */
#define CONFIG_SYS_EMC0CS1CR		0x0000393F

/* 
 * Settings for the CFI Flash driver
 */
#define CONFIG_SYS_FLASH_CFI		1
#define CONFIG_FLASH_CFI_DRIVER		1
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BANK1_BASE }
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	128
#define CONFIG_SYS_FLASH_CFI_AMD_RESET	1

/* 
 * U-boot environment configruation
 */
#define CONFIG_ENV_IS_IN_FLASH		1
#define CONFIG_ENV_ADDR			CONFIG_SYS_FLASH_BANK1_BASE
#define CONFIG_ENV_SIZE			0x1000
#define CONFIG_INFERNO			1
#define CONFIG_ENV_OVERWRITE		1

/*
 * Serial console configuration
 */
#define CONFIG_SYS_NS16550		1
#undef CONFIG_NS16550_MIN_FUNCTIONS
#define CONFIG_SYS_NS16550_SERIAL	1
#define CONFIG_SYS_NS16550_REG_SIZE     (-4)
#define CONFIG_SYS_NS16550_CLK          CONFIG_SYS_CLK_PCLK0
#define CONFIG_CONS_INDEX               1
#define CONFIG_SYS_NS16550_COM1         0x40000000
#define CONFIG_BAUDRATE                 115200
#define CONFIG_SYS_BAUDRATE_TABLE       { 9600, 19200, 38400, 57600, 115200 }

/*
 * MALLOC_LEN can't be more than the specified size!
 * Refer to u-boot.lds for further details.
 */
#define CONFIG_SYS_MALLOC_LEN		(1024*8)

/*
 * Console I/O buffer size
 */
#define CONFIG_SYS_CBSIZE		256

/*
 * Print buffer size
 */
#define CONFIG_SYS_PBSIZE               (CONFIG_SYS_CBSIZE + \
                                        sizeof(CONFIG_SYS_PROMPT) + 16)

/* 
 * Ethernet driver configuration
 */
#define CONFIG_NET_MULTI
#define CONFIG_CORE10100		1

/*
 * Keep Rx & Tx buffers in internal RAM
 */
#define CONFIG_CORE10100_INTRAM_ADDRESS	0x20008000
#define CONFIG_BITBANGMII		1
#define CONFIG_BITBANGMII_MULTI		1

#define CONFIG_SYS_MEMTEST_START	CONFIG_SYS_RAM_BASE
#define CONFIG_SYS_MEMTEST_END		(CONFIG_SYS_RAM_BASE + CONFIG_SYS_RAM_SIZE)

/* Need ot be defined for "loadb" */
#define CONFIG_SYS_LOAD_ADDR		CONFIG_SYS_RAM_BASE

/*
 * Monitor is in NVM. For U-Boot, it is not flash, 
 * neither RAM, but CONFIG_SYS_MONITOR_BASE must be defined.
 */
#define CONFIG_SYS_MONITOR_BASE  	0x0
/*
 * Monitor is not in flash. Define the following to avoid
 * U-Boot to run flash_protect() on monitor code.
 */
#define CONFIG_MONITOR_IS_IN_RAM  	1

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
#define CONFIG_HOSTNAME			a2f-lnx-evb
#define CONFIG_BOOTARGS			"console=ttyS0,115200 panic=10"
#define CONFIG_BOOTCOMMAND		"run flashboot"

/* 
 * Short-cuts to some useful commands (macros)
 */
#define CONFIG_EXTRA_ENV_SETTINGS	\
	"loadaddr=70000000\0"		\
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:::${netmask}:${hostname}:eth0:off\0"				\
	"flashaddr=74020000\0"		\
	"flashboot=run addip;bootm ${flashaddr}\0"	\
	"netboot=tftp ${image};run addip;bootm\0"	\
	"image=a2f/uImage\0"

/*
 * Linux kernel boot parameters configuration
 */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG

#endif /* __CONFIG_H */
