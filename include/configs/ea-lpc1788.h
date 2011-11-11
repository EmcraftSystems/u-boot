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
 * Configuration settings for the Embedded Artists LPC1788 board.
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
 * This is the EA LPC1788 device
 */
#define CONFIG_SYS_LPC178X

/*
 * Enable GPIO driver
 */
/* TBD */

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#define CONFIG_SYS_BOARD_REV_STR	"1"

/*
 * Monitor prompt
 */
#define CONFIG_SYS_PROMPT		"EA-LPC1788> "

/*
 * We want to call the CPU specific initialization
 */
#define CONFIG_ARCH_CPU_INIT

/*
 * Clock configuration (see cpu/arm_cortexm3/lpc178x/clock.c for details)
 */
/*
 * This should be setup to the board specific rate for the external oscillator
 */
#define CONFIG_LPC178X_EXTOSC_RATE		12000000

/*
 * PLL0 is enabled, therefore
 * the values of CONFIG_LPC178X_PLL0_M and CONFIG_LPC178X_PLL0_PSEL must
 * be setup to configure the PLL0 rate based on the selected input clock.
 * See the LCP178x/7x User's Manual for information on setting these
 * values correctly. SYSCLK is used as the PLL0 input clock.
 */
/*
 * PLL0_CLK_OUT = CONFIG_LPC178X_PLL0_M * CONFIG_LPC178X_EXTOSC_RATE
 *
 * PLL0_FCCO must be between 156MHz and 320MHz
 *  where PLL0_FFC0 = PLL0_CLK_OUT * 2 * P
 * PLL0_CLK_OUT must be between 9.75MHz and 160MHz
 */
/*
 * PLL Multiplier value (1..32)
 */
/* TODO: change M to 10 in order to reach 120 MHz CPU clock */
#define CONFIG_LPC178X_PLL0_M		9	/* 12 MHz * 9 = 108 MHz */
/*
 * PSEL (a 0..3 code for PLL Divider value)
 * P (PPL divider value) = 2 in the power of PSEL
 * PSEL = 0..3 make P = 1, 2, 4 or 8.
 */
#define CONFIG_LPC178X_PLL0_PSEL	0	/* P = 1 */

/*
 * CCLKDIV value. Selects the divide value for creating the CPU clock (CCLK)
 * from the selected clock source.
 * CONFIG_LPC178X_CPU_DIV=1 means that the input clock is divided by 1
 * to produce the CPU clock.
 */
#define CONFIG_LPC178X_CPU_DIV		1

/*
 * Peripheral clock selector. This divider, between 1 and 31, is used to
 * generate the clock to the peripherals. The rate is divided from the
 * selected clock source (PLL0, internal or main oscillator).
 */
#define CONFIG_LPC178X_PCLK_DIV		2

/*
 * Number of clock ticks in 1 sec
 */
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
 * Memory layout configuration
 */
#define CONFIG_MEM_NVM_BASE		0x00000000
#define CONFIG_MEM_NVM_LEN		(512 * 1024)   /* 64, 128, 256 or 512 kB */

#define CONFIG_MEM_RAM_BASE		0x10000000
#define CONFIG_MEM_RAM_LEN		(16 * 1024)
#define CONFIG_MEM_RAM_BUF_LEN		(32 * 1024)
#define CONFIG_MEM_MALLOC_LEN		(12 * 1024)
#define CONFIG_MEM_STACK_LEN		(4 * 1024)

/*
 * malloc() pool size
 */
#define CONFIG_SYS_MALLOC_LEN		CONFIG_MEM_MALLOC_LEN

/*
 * Configuration of the external DRAM memory
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_RAM_CS		0       /* 0 .. 3 */
#define CONFIG_SYS_RAM_BASE		(0xA0000000 +			\
					 (CONFIG_SYS_RAM_CS * 0x10000000))
#define CONFIG_SYS_RAM_SIZE		(32 * 1024 * 1024)

/*
 * Configuration of the external Flash memory
 */
#define CONFIG_SYS_NO_FLASH

/*
 * Store env in memory only
 */
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_ENV_SIZE			(4 * 1024)
#define CONFIG_ENV_ADDR			CONFIG_SYS_FLASH_BANK1_BASE
#define CONFIG_INFERNO			1
#define CONFIG_ENV_OVERWRITE		1

/*
 * Serial console configuration
 */
#define CONFIG_LPC178X_UART_CONSOLE

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Ethernet configuration
 */
/* TBD */

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
					CONFIG_SYS_RAM_SIZE)

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
#undef CONFIG_CMD_NET
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
#define CONFIG_HOSTNAME			ea-lpc1788
#define CONFIG_BOOTARGS			"lpc178x_platform=ea-lpc1788 "\
					"console=ttyS2,115200 panic=10"
#define CONFIG_BOOTCOMMAND		"run netboot"

/*
 * This ensures that the board-specific misc_init_r() gets invoked.
 */
#define CONFIG_MISC_INIT_R

/*
 * Short-cuts to some useful commands (macros)
 */
#define CONFIG_EXTRA_ENV_SETTINGS				\
	"loadaddr=0xA0000000\0"					\
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:eth0:off\0"				\
	"ethaddr=C0:B1:3C:88:88:88\0"				\
	"ipaddr=172.17.4.206\0"					\
	"serverip=172.17.0.1\0"					\
	"image=lpc17xx/uImage\0"					\
	"netboot=tftp ${image};run addip;bootm\0"

/*
 * Linux kernel boot parameters configuration
 */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG

#endif /* __CONFIG_H */
