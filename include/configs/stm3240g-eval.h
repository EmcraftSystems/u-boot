/*
 * (C) Copyright 2011, 2012
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
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
 * Configuration settings for the STMicroelectronic STM3220G-EVAL board.
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
 * This is the STM32-F4 device
 */
#define CONFIG_SYS_STM32

/*
 * Enable GPIO driver
 */
#define CONFIG_STM32F2_GPIO

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#define CONFIG_SYS_BOARD_REV_STR	"Rev 2.A"

/*
 * Monitor prompt
 */
#define CONFIG_SYS_PROMPT		"STM3240G-EVAL> "

/*
 * We want to call the CPU specific initialization
 */
#define CONFIG_ARCH_CPU_INIT

/*
 * Clock configuration (see mach-stm32/clock.c for details):
 * - use PLL as the system clock;
 * - use HSE as the PLL source;
 * - configure PLL to get 168MHz system clock.
 */
#define CONFIG_STM32_SYS_CLK_PLL
#define CONFIG_STM32_PLL_SRC_HSE
#define CONFIG_STM32_HSE_HZ		25000000	/* 25 MHz */
#define CONFIG_STM32_PLL_M		25
#define CONFIG_STM32_PLL_N		336
#define CONFIG_STM32_PLL_P		2
#define CONFIG_STM32_PLL_Q		7

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
#define CONFIG_MEM_NVM_LEN		(1024 * 1024)

#define CONFIG_MEM_RAM_BASE		0x20000000
#define CONFIG_MEM_RAM_LEN		(20 * 1024)
#define CONFIG_MEM_RAM_BUF_LEN		(88 * 1024)
#define CONFIG_MEM_MALLOC_LEN		(16 * 1024)
#define CONFIG_MEM_STACK_LEN		(4 * 1024)

/*
 * malloc() pool size
 */
#define CONFIG_SYS_MALLOC_LEN		CONFIG_MEM_MALLOC_LEN

#define FSMC_NOR_PSRAM_CS_ADDR(n) \
	(0x60000000 + ((n) - 1) * 0x4000000)

/*
 * Configuration of the external PSRAM memory
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_RAM_SIZE		(32 * 1024 * 1024)
#define CONFIG_SYS_RAM_CS		2

#undef CONFIG_SYS_RAM_BURST
#define CONFIG_SYS_FSMC_PSRAM_BCR	0x00005059
#define CONFIG_SYS_FSMC_PSRAM_BTR	0x10000904
#define CONFIG_SYS_FSMC_PSRAM_BWTR	0x10000804
#define CONFIG_FSMC_NOR_PSRAM_CS2_ENABLE

#define CONFIG_SYS_RAM_BASE		FSMC_NOR_PSRAM_CS_ADDR(CONFIG_SYS_RAM_CS)

#undef CONFIG_LCD
#ifdef CONFIG_LCD
#define LCD_BPP				LCD_COLOR16
#define CONFIG_BMP_16BPP		1
#define CONFIG_SPLASH_SCREEN		1
#define CONFIG_CMD_BMP
#define CONFIG_FB_ADDR				\
	(CONFIG_SYS_RAM_BASE + CONFIG_SYS_RAM_SIZE - \
		roundup(calc_fbsize(), PAGE_SIZE))
#define CONFIG_LCD_CS			3
#define CONFIG_LCD_FSMC_BCR			\
	STM32_FSMC_BCR_MBKEN |			\
	STM32_FSMC_BCR_WREN |			\
	(STM32_FSMC_BCR_MWID_16 << STM32_FSMC_BCR_MWID_BIT)
#define CONFIG_LCD_FSMC_BTR			\
	(1 << STM32_FSMC_BTR_ADDRST_BIT) |	\
	(0 << STM32_FSMC_BTR_ADDHOLD_BIT) |	\
	(9 << STM32_FSMC_BTR_DATAST_BIT) |	\
	(0 << STM32_FSMC_BTR_BUSTURN_BIT) |	\
	(0 << STM32_FSMC_BTR_CLKDIV_BIT) |	\
	(0 << STM32_FSMC_BTR_DATLAT_BIT) |	\
	STM32_FSMC_BWTR_ACCMOD_A
#define CONFIG_LCD_FSMC_BWTR			\
	(1 << STM32_FSMC_BWTR_ADDRST_BIT) |	\
	(0 << STM32_FSMC_BWTR_ADDHOLD_BIT) |	\
	(9 << STM32_FSMC_BWTR_DATAST_BIT) |	\
	(0 << STM32_FSMC_BWTR_CLKDIV_BIT) |	\
	(0 << STM32_FSMC_BWTR_DATLAT_BIT) |	\
	STM32_FSMC_BWTR_ACCMOD_A
#define CONFIG_FSMC_NOR_PSRAM_CS3_ENABLE
#define CONFIG_LCD_ILI932x
#define CONFIG_LCD_ILI932x_BASE		FSMC_NOR_PSRAM_CS_ADDR(CONFIG_LCD_CS)
#define CONFIG_LCD_ILI932x_DOUBLE_BUFFER
#endif

/*
 * Configuration of the external Flash memory
 */
#define CONFIG_SYS_FLASH_CS		1

#define CONFIG_SYS_FSMC_FLASH_BCR	0x00005015
#define CONFIG_SYS_FSMC_FLASH_BTR	0x00021206
#define CONFIG_SYS_FSMC_FLASH_BWTR	0x00021106
#define CONFIG_FSMC_NOR_PSRAM_CS1_ENABLE

#define CONFIG_SYS_FLASH_BANK1_BASE	FSMC_NOR_PSRAM_CS_ADDR(CONFIG_SYS_FLASH_CS)

#define CONFIG_SYS_FLASH_CFI		1
#define CONFIG_FLASH_CFI_DRIVER		1
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BANK1_BASE }
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	128
#define CONFIG_SYS_FLASH_CFI_AMD_RESET	1
#define CONFIG_SYS_FLASH_PROTECTION	1

/*
 * Store env in memory only
 */
#define CONFIG_ENV_IS_IN_FLASH
#define CONFIG_ENV_SIZE			(4 * 1024)
#define CONFIG_ENV_ADDR			CONFIG_SYS_FLASH_BANK1_BASE
#define CONFIG_INFERNO			1
#define CONFIG_ENV_OVERWRITE		1

/*
 * Serial console configuration
 */
#define CONFIG_STM32_USART_CONSOLE
#define CONFIG_STM32_USART_PORT		3	/* USART3 */
#define CONFIG_STM32_USART_TX_IO_PORT	2	/* PORTC */
#define CONFIG_STM32_USART_RX_IO_PORT	2	/* PORTC */
#define CONFIG_STM32_USART_TX_IO_PIN	10	/* GPIO10 */
#define CONFIG_STM32_USART_RX_IO_PIN	11	/* GPIO11 */

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Ethernet configuration
 */
#define CONFIG_NET_MULTI
#define CONFIG_STM32_ETH

/*
 * Ethernet RX buffers are malloced from the internal SRAM (more precisely,
 * from CONFIG_SYS_MALLOC_LEN part of it). Each RX buffer has size of 1536B.
 * So, keep this in mind when changing the value of the following config,
 * which determines the number of ethernet RX buffers (number of frames which
 * may be received without processing until overflow happens).
 */
#define CONFIG_SYS_RX_ETH_BUFFER	4

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
#define CONFIG_HOSTNAME			stm3240g-eval
#define CONFIG_BOOTARGS			"stm32_platform=stm3240g-eval "\
					"console=ttyS2,115200 panic=10"
#define CONFIG_BOOTCOMMAND		"run flashboot"

#define CONFIG_SYS_CONSOLE_IS_IN_ENV

/*
 * Short-cuts to some useful commands (macros)
 */
#define CONFIG_EXTRA_ENV_SETTINGS				\
	"loadaddr=0x64000000\0"					\
	"args=setenv bootargs " CONFIG_BOOTARGS "\0"		\
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:eth0:off\0"				\
	"flashaddr=60020000\0"					\
	"flashboot=run args addip;bootm ${flashaddr}\0"		\
	"ethaddr=C0:B1:3C:88:88:85\0"				\
	"ipaddr=172.17.4.206\0"					\
	"serverip=172.17.0.1\0"					\
	"image=stm32/uImage\0"					\
	"stdin=serial\0"					\
	"netboot=tftp ${image};run args addip;bootm\0"		\
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
