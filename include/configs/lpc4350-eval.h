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

/*
 * Configuration settings for the Hitex LPC4350 Eval board.
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * Disable debug messages
 */
#undef DEBUG

/*
 * This is an ARM Cortex-M4 CPU core. Also use the common Cortex-M3 code.
 */
#define CONFIG_SYS_ARMCORTEXM3
#define CONFIG_SYS_ARMCORTEXM4

/*
 * This is the NXP LPC4350 device which is backward-compatible with LPC1850
 */
#define CONFIG_SYS_LPC18XX

/*
 * Enable GPIO driver
 */
#define CONFIG_LPC18XX_GPIO

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#define CONFIG_SYS_BOARD_REV_STR	"1"

/*
 * Monitor prompt
 */
#define CONFIG_SYS_PROMPT		"LPC4350-EVAL> "

/*
 * We want to call the CPU specific initialization
 */
#define CONFIG_ARCH_CPU_INIT

/*
 * We do not use cortex_m3_soc_init() yet.
 */
#undef CONFIG_ARMCORTEXM3_SOC_INIT

/*
 * Clock configuration (see cpu/arm_cortexm3/lpc18xx/clock.c for details)
 */
/*
 * This should be setup to the board specific rate for the external oscillator
 */
#define CONFIG_LPC18XX_EXTOSC_RATE		12000000

/*
 * PLL1 multiplier value (1..256)
 */
#define CONFIG_LPC18XX_PLL1_M		17	/* 12 MHz * 17 = 204 MHz */

/*
 * Number of clock ticks in 1 sec
 */
#define CONFIG_SYS_HZ			1000

/*
 * Use internal clock (CPU clock) for the Cortex-M4 SysTick timer
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
 * Memory layout configuration
 */
/*
 * No internal flash on the NXP LPC1850 MCU. Setting CONFIG_MEM_NVM_LEN to the
 * size of the contiguous region of internal SRAM at address 0x10000000.
 */
#define CONFIG_MEM_NVM_BASE		0x00000000
#define CONFIG_MEM_NVM_LEN		(128 * 1024)

#define CONFIG_MEM_RAM_BASE		0x20000000
#define CONFIG_MEM_RAM_LEN		(32 * 1024)
#define CONFIG_MEM_RAM_BUF_LEN		(1 * 1024)
#define CONFIG_MEM_MALLOC_LEN		(27 * 1024)
#define CONFIG_MEM_STACK_LEN		(4 * 1024)

/*
 * malloc() pool size
 */
#define CONFIG_SYS_MALLOC_LEN		CONFIG_MEM_MALLOC_LEN

/*
 * Configuration of the external DRAM memory
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_RAM_CS		0	/* 0 .. 3 */
#define CONFIG_SYS_RAM_BASE		0x28000000
#define CONFIG_SYS_RAM_SIZE		(8 * 1024 * 1024)

/*
 * Store env in memory only, if no flash.
 */
#define CONFIG_ENV_IS_NOWHERE
#define CONFIG_SYS_NO_FLASH

#define CONFIG_ENV_SIZE			(4 * 1024)
#define CONFIG_ENV_ADDR			CONFIG_SYS_FLASH_BANK1_BASE
#define CONFIG_INFERNO			1
#define CONFIG_ENV_OVERWRITE		1

/*
 * Serial console configuration
 */
#define CONFIG_SYS_NS16550		1
#undef CONFIG_NS16550_MIN_FUNCTIONS
#define CONFIG_SYS_NS16550_SERIAL	1
/*
 * Registers are 32-bit. The negative value tells the ns16550 driver that
 * registers should be post-padded with zeroes (because the CPU is in
 * little-endian mode.)
 */
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
/*
 * USART0 uses the BASE_UART0_CLK clock
 */
#define CONFIG_SYS_NS16550_CLK		clock_get(CLOCK_UART0)
#define CONFIG_CONS_INDEX		1
/*
 * USART0 registers base: 0x40081000
 * UART1 registers base:  0x40082000
 * USART2 registers base: 0x400C1000
 * USART3 registers base: 0x400C2000
 */
#define CONFIG_SYS_NS16550_COM1		0x40081000
/*
 * Pin configuration for UART
 */
#define CONFIG_LPC18XX_UART_TX_IO_GROUP		15	/* PF */
#define CONFIG_LPC18XX_UART_TX_IO_PIN		10	/* PF.10 = USART0 TXD */
#define CONFIG_LPC18XX_UART_TX_IO_FUNC		1
#define CONFIG_LPC18XX_UART_RX_IO_GROUP		15	/* PF */
#define CONFIG_LPC18XX_UART_RX_IO_PIN		11	/* PF.11 = USART0 RXD */
#define CONFIG_LPC18XX_UART_RX_IO_FUNC		1

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Console I/O buffer size
 */
#define CONFIG_SYS_CBSIZE		256

/*
 * Print buffer size
 */
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
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
#define CONFIG_HOSTNAME			lpc4350-eval
#define CONFIG_BOOTARGS			"lpc18xx_platform=hitex-lpc4350 "\
					"console=ttyS0,115200 panic=10"
#define CONFIG_BOOTCOMMAND		"run flashboot"

/*
 * This ensures that the board-specific misc_init_r() gets invoked.
 */
#define CONFIG_MISC_INIT_R

/*
 * Short-cuts to some useful commands (macros)
 */
#define CONFIG_EXTRA_ENV_SETTINGS				\
	"loadaddr=0x28000000\0"					\
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:eth0:off\0"				\
	"flashaddr=1C020000\0"					\
	"flashboot=run addip;bootm ${flashaddr}\0"		\
	"ethaddr=C0:B1:3C:88:88:90\0"				\
	"ipaddr=172.17.4.215\0"					\
	"serverip=172.17.0.1\0"					\
	"image=lpc18xx/uImage\0"				\
	"netboot=tftp ${image};run addip;bootm\0"		\
	"update=tftp ${image};"					\
	"prot off ${flashaddr} +${filesize};"			\
	"era ${flashaddr} +${filesize};"			\
	"cp.b ${loadaddr} ${flashaddr} ${filesize}\0"

/*
 * Linux kernel boot parameters configuration
 */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG

#endif /* __CONFIG_H */
