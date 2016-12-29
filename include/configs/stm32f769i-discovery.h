/*
 * (C) Copyright 2011-2016
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 * Sergei Miroshnichenko, Emcraft Systems, sergeimir@emcraft.com
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
 * Configuration settings for the STmicro STM32F769I Discovery board
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#undef DEBUG

/*
 * This is an ARM Cortex-M7 CPU core.
 * Also use the common Cortex-M3 and Cortex-M4 code.
 */
#define CONFIG_SYS_ARMCORTEXM3
#define CONFIG_SYS_ARMCORTEXM4
#define CONFIG_SYS_ARMCORTEXM7

/*
 * This is an STM32 and STM32F7 device.
 */
#define CONFIG_SYS_STM32
#define CONFIG_SYS_STM32F7
#define CONFIG_SYS_STM32F76xxx
#undef CONFIG_SYS_STM32F7_DISCO
#define CONFIG_SYS_STM32F769I_DISCO

/*
 * Enable GPIO driver
 */
#define CONFIG_STM32F2_GPIO

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#define CONFIG_SYS_BOARD_REV_STR	"Revision B-01"

/*
 * Monitor prompt
 */
#define CONFIG_SYS_PROMPT		"STM32F769I-DISCO> "

/*
 * We want to call the CPU specific initialization
 */
#define CONFIG_ARCH_CPU_INIT

/*
 * Clock configuration (see mach-stm32/clock.c for details):
 * - use PLL as the system clock;
 * - use HSE as the PLL source;
 * - configure PLL to get 216MHz system clock.
 */
#define CONFIG_STM32_SYS_CLK_PLL
#define CONFIG_STM32_PLL_SRC_HSE
#define CONFIG_STM32_HSE_HZ		25000000
#define CONFIG_STM32_PLL_M		25
#define CONFIG_STM32_PLL_N		432
#define CONFIG_STM32_PLL_P		2
#define CONFIG_STM32_PLL_Q		9
#define CONFIG_STM32_PLL_R		2

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
#define CONFIG_SYS_CACHELINE_SIZE	32
#define CONFIG_STM32F7_ICACHE_ON
#define CONFIG_STM32F7_DCACHE_ON

/*
 * Actually we don't need DMAMEM if DCACHE is off. But we
 * want to be able to run the same kernel image with or
 * without DCACHE. So, pass a DMAMEM tag to the kernel unconditionally.
 * Note, SZ_ALL must be power of 2 (to program MPU correctly)!
 */
#define CONFIG_DMAMEM
#if defined(CONFIG_DMAMEM)
# define CONFIG_DMAMEM_SZ_ALL		(1 << 20)	/* 1MB */
# define CONFIG_DMAMEM_SZ_FB		(640 * 1024)
# define CONFIG_DMAMEM_BASE		(CONFIG_SYS_RAM_BASE + \
					 (CONFIG_SYS_RAM_SIZE / 2) - \
					 CONFIG_DMAMEM_SZ_ALL)
#endif

#define CONFIG_ARMCORTEXM3_SOC_INIT

/*
 * Memory layout configuration
 *
 * On-chip Flash:
 */
#define CONFIG_MEM_NVM_BASE		0x08000000
#define CONFIG_MEM_NVM_LEN		(1024 * 1024 * 2)
#define CONFIG_ENVM			1
#if defined(CONFIG_ENVM)
# define CONFIG_SYS_ENVM_BASE		0x08000000
# define CONFIG_SYS_ENVM_LEN		CONFIG_MEM_NVM_LEN
#endif


/*
 * On-chip SRAM:
 */
#define CONFIG_MEM_RAM_BASE		0x20000000
#define CONFIG_MEM_RAM_LEN		(20 * 1024)
#define CONFIG_MEM_RAM_BUF_LEN		(88 * 1024)
#define CONFIG_MEM_MALLOC_LEN		(16 * 1024)
#define CONFIG_MEM_STACK_LEN		(4 * 1024)

/*
 * malloc() pool size
 */
#define CONFIG_SYS_MALLOC_LEN		CONFIG_MEM_MALLOC_LEN

#define FSMC_NOR_PSRAM_CS_ADDR(n)	(0x60000000 + ((n) - 1) * 0x4000000)

/*
 * Configuration of the external SDRAM memory
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_RAM_SIZE		(16 * 1024 * 1024)
#define CONFIG_SYS_RAM_FREQ_DIV		2
#define CONFIG_SYS_RAM_BASE		0xC0000000

#define CONFIG_SYS_NO_FLASH

/*
 * Store env in Flash memory
 */
#define CONFIG_ENV_IS_IN_ENVM
#define CONFIG_ENV_SIZE			(4 * 1024)
#define CONFIG_ENV_ADDR			(CONFIG_SYS_ENVM_BASE + (128 * 1024))
#define CONFIG_INFERNO			1
#define CONFIG_ENV_OVERWRITE		1

/*
 * Serial console configuration
 */
#define CONFIG_STM32_USART_CONSOLE

/*
 * USART6, TX PC.6, RX PC.7
 */
#define CONFIG_STM32_USART_PORT	6		/* USART6 */

#define CONFIG_STM32_USART_TX_IO_PORT	2	/* PORTC */
#define CONFIG_STM32_USART_TX_IO_PIN	6	/* GPIO6 */

#define CONFIG_STM32_USART_RX_IO_PORT	2	/* PORTC */
#define CONFIG_STM32_USART_RX_IO_PIN	7	/* GPIO7 */

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Ethernet configuration
 */
#define CONFIG_NET_MULTI
#define CONFIG_STM32_ETH
#define CONFIG_STM32_ETH_RMII

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
 * Monitor is not in flash. Need to define this to prevent
 * U-Boot from running flash_protect() on the monitor code.
 */
#define CONFIG_MONITOR_IS_IN_RAM	1

#undef CONFIG_LCD

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
#undef CONFIG_CMD_BUFCOPY

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
#define CONFIG_BOOTDELAY		1
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_BOOTCOMMAND		"run netboot"

/* boot args and env */
#define CONFIG_HOSTNAME			stm32f769i-disco
#define CONFIG_BOOTARGS			"stm32_platform=stm32f769i-disco "	\
					"console=ttyS5,115200 panic=10"

/*
 * These are the good addresses to get Image data right at the 'Load Address'
 * (0xC0008000), and thus avoid additional uImage relocation:
 * - linux-2.6: 0xC0007FC0 (reserve place for uImage header)
 * - linux-4.2: 0xC0007FB4 (reserve place for 2-files multi-image header)
 */
#define LOADADDR			"0xC0007FB4"

#define REV_EXTRA_ENV							\
	"envmboot=run args addip;bootm ${envmaddr}\0"			\
	"envmupdate=tftp ${image};"					\
		"cptf ${envmaddr} ${loadaddr} ${filesize}\0"

#define CONFIG_SYS_CONSOLE_IS_IN_ENV

/*
 * Short-cuts to some useful commands (macros)
 */
#define CONFIG_EXTRA_ENV_SETTINGS				\
	"loadaddr=" LOADADDR "\0"				\
	"args=setenv bootargs " CONFIG_BOOTARGS "\0"		\
	"addip=setenv bootargs ${bootargs}"			\
	" ip=${ipaddr}:${serverip}:${gatewayip}:"		\
	 "${netmask}:${hostname}:eth0:off\0"			\
	"envmaddr=08040000\0"					\
	"ethaddr=C0:B1:3C:88:88:85\0"				\
	"ipaddr=172.17.4.206\0"					\
	"serverip=172.17.0.19\0"				\
	"image=stm32f7/f769i-disco.uImage\0"			\
	"stdin=serial\0"					\
	"netboot=tftp ${image}; run args addip; bootm\0"	\
	REV_EXTRA_ENV

/*
 * Linux kernel boot parameters configuration
 */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG

/*
 * Enable support for booting with FDT
 */
#define CONFIG_OF_LIBFDT
#define CONFIG_OF_FORCE_RELOCATE
#define CONFIG_SYS_BOOTMAPSZ		CONFIG_SYS_RAM_SIZE

#endif /* __CONFIG_H */
