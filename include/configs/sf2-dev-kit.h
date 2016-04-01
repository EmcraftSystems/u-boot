/*
 * (C) Copyright 2012-2013
 * Emcraft Systems, <www.emcraft.com>
 * Alexander Potashev <aspotashev@emcraft.com>
 * Vladimir Khusainov, <vlad@emcraft.com>
 * Yuri Tikhonov, <yur@emcraft.com>
 * Vladimir Skvortsov, <vskvortsov@emcraft.com>
 *
 * Configuration settings for Microsemi SmartFusion2 Development
 * Kit Board (SF2-DEV-KIT).
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
 * This is the Microsemi SmartFusion2 (aka M2S) device
 */
#define CONFIG_SYS_M2S

/*
 * System frequencies are defined by Libero, with no
 * known way (as of yet) to read them in run time. Hence,
 * we define them as build-time constants
 */
#define CONFIG_SYS_M2S_SYSREF		166000000

/*
 * This is a specific revision of the board
 */
#define CONFIG_SYS_BOARD_REV		0x1

#if (CONFIG_SYS_BOARD_REV!=0x1)
#error CONFIG_SYS_BOARD_REV must be 0x1
#endif

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#define CONFIG_SYS_BOARD_REV_STR	"1.0"

/*
 * Monitor prompt
 */
#define CONFIG_SYS_PROMPT		"SF2-DEV-KIT> "

/*
 * We want to call the CPU specific initialization
 */
#define CONFIG_ARCH_CPU_INIT

/*
 * This ensures that the SoC-specific cortex_m3_soc_init() gets invoked.
 */
#define CONFIG_ARMCORTEXM3_SOC_INIT

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
 * Cache configuration
 */
#define CONFIG_M2S_CACHE_ON

/*
 * Memory layout configuration
 */
#define CONFIG_MEM_NVM_BASE		0x00000000
#define CONFIG_MEM_NVM_LEN		(256 * 1024)

#if 0
/*
 * Define the constant below to build U-boot for running
 * from offset 0x20000 (128KB) in eNVM. When built this way,
 * use the following commands to test the newly built U-boot:
 * - tftp u-boot.bin
 * - cptf 20000 ${loadaddr} ${filesize} 0
 * - go 20375 (or check address of _start in u-boot.map)
 */
#define CONFIG_MEM_NVM_UBOOT_OFF	(128 * 1024)
#endif

#define CONFIG_MEM_RAM_BASE		0x20000000
#define CONFIG_MEM_RAM_LEN		(16 * 1024)
#define CONFIG_MEM_RAM_BUF_LEN		(32 * 1024)
#define CONFIG_MEM_MALLOC_LEN		(12 * 1024)
#define CONFIG_MEM_STACK_LEN		(4 * 1024)

/*
 * malloc() pool size
 */
#define CONFIG_SYS_MALLOC_LEN		CONFIG_MEM_MALLOC_LEN

/*
 * Configuration of the external memory
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_RAM_BASE		0xA0000000
#define CONFIG_SYS_RAM_SIZE		(256 * 1024 * 1024)

/*
 * Ethernet driver configuration
 */
#define CONFIG_NET_MULTI
#define CONFIG_M2S_ETH
#define CONFIG_M2S_ETH_MODE_SGMII

#define CONFIG_SYS_RX_ETH_BUFFER	2

/*
 * Use standard MII PHY API
 */
#define CONFIG_MII
#define CONFIG_SYS_FAULT_ECHO_LINK_DOWN

/*
 * Configuration of the external Flash
 * No NOR Flash
 */
#define CONFIG_SYS_NO_FLASH

/*
 * Configure the SPI contoler device driver
 * FIFO Size is 64K, but leave 5 bytes for cmd[] + addr[]
 */
#define CONFIG_M2S_SPI			1
#define CONFIG_SPI_MAX_XF_LEN		65530

/*
 * Configure SPI Flash
 */

#define CONFIG_SPI_FLASH		1
#define CONFIG_SPI_FLASH_ATMEL		1
#define CONFIG_SPI_FLASH_BUS		0
#define CONFIG_SPI_FLASH_CS		0
#define CONFIG_SPI_FLASH_MODE		3
#define CONFIG_SPI_FLASH_SPEED		(CONFIG_SYS_M2S_SYSREF / 4)
#define CONFIG_SF_DEFAULT_SPEED		CONFIG_SPI_FLASH_SPEED
#define CONFIG_SF_DEFAULT_MODE		CONFIG_SPI_FLASH_MODE

/*
 * U-boot environment configuration
 */
#define CONFIG_ENV_IS_IN_SPI_FLASH	1
#define CONFIG_ENV_SECT_SIZE		0x1000
#define CONFIG_ENV_SIZE			CONFIG_ENV_SECT_SIZE
#define CONFIG_ENV_OFFSET		0x0
#define CONFIG_ENV_SPI_BUS		CONFIG_SPI_FLASH_BUS
#define CONFIG_ENV_SPI_CS		CONFIG_SPI_FLASH_CS
#define CONFIG_ENV_SPI_MAX_HZ		CONFIG_SPI_FLASH_SPEED
#define CONFIG_ENV_SPI_MODE		CONFIG_SPI_FLASH_MODE

#define CONFIG_INFERNO			1
#define CONFIG_ENV_OVERWRITE		1

/*
 * Serial console configuration: MSS UART1
 */
#define CONFIG_SYS_NS16550		1
#undef CONFIG_NS16550_MIN_FUNCTIONS
#define CONFIG_SYS_NS16550_SERIAL	1
#define CONFIG_SYS_NS16550_REG_SIZE	(-4)
#define CONFIG_SYS_NS16550_CLK		clock_get(CLOCK_PCLK1)
#define CONFIG_CONS_INDEX		2
#define CONFIG_SYS_NS16550_COM2		0x40010000
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
#define CONFIG_CMD_LOADS
#undef CONFIG_CMD_MISC
#define CONFIG_CMD_NET
#define CONFIG_CMD_PING
#undef CONFIG_CMD_NETBOOT
#undef CONFIG_CMD_NFS
#undef CONFIG_CMD_SOURCE
#undef CONFIG_CMD_XIMG
#undef CONFIG_CMD_SOMTEST
#if defined(CONFIG_SPI_FLASH)
#define CONFIG_CMD_SF
#endif

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
#define CONFIG_HOSTNAME			sf2-dev-kit
#define CONFIG_BOOTARGS			"m2s_platform=sf2-dev-kit "\
					"console=ttyS1,115200 panic=10"
#define CONFIG_BOOTCOMMAND		"run flashboot"
#define CONFIG_ENV_IMG_OFFSET		0x10000

/*
 * Macro for the "loadaddr". The most optimal load address
 * for the non-compressed uImage is the kernel link address
 * (CONFIG_SYS_RAM_BASE + 0x8000) minus sizeof uImage header (0x40),
 * so the kernel start address would be loaded just to the right
 * place.
 */
#define UIMAGE_LOADADDR			0xA0007FC0

/*
 * Short-cuts to some useful commands (macros)
 */
#define CONFIG_EXTRA_ENV_SETTINGS				\
	"loadaddr=" MK_STR(UIMAGE_LOADADDR) "\0"		\
	"args=setenv bootargs " CONFIG_BOOTARGS "\0"		\
	"ethaddr=C0:B1:3C:88:88:88\0"				\
	"ipaddr=172.17.4.221\0"					\
	"serverip=172.17.0.1\0"					\
	"image=networking.uImage\0"				\
	"spiaddr=" MK_STR(CONFIG_ENV_IMG_OFFSET) "\0"		\
	"spisize=3F0000\0"					\
	"spiprobe=sf probe " MK_STR(CONFIG_SPI_FLASH_BUS) "\0"	\
	"addip=setenv bootargs ${bootargs}"			\
	" ip=${ipaddr}:${serverip}:${gatewayip}:"		\
	"${netmask}:${hostname}:eth0:off\0"			\
	"flashboot=run args addip;run spiprobe;"		\
	" sf read ${loadaddr} ${spiaddr} ${spisize};"		\
	" bootm ${loadaddr}\0"					\
	"netboot=tftp ${loadaddr} ${image};run args addip;bootm\0"	\
	"update=tftp ${loadaddr} ${image};run spiprobe;"	\
	" sf erase ${spiaddr} ${filesize};"			\
	" sf write ${loadaddr} ${spiaddr} ${filesize};"		\
	" setenv spisize 0x${filesize}; saveenv\0"

/*
 * Linux kernel boot parameters configuration
 */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG

#endif /* __CONFIG_H */
