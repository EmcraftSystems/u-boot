/*
 * (C) Copyright 2011-2017
 *
 * Yuri Tikhonov, Emcraft Systems, yur@emcraft.com
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 * Pavel Boldin, Emcraft Systems, paboldin@emcraft.com
 * Vladimir Skvortsov, Emcraft Systems, vskvortsov@emcraft.com
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
 * Configuration settings for the STmicro STM32F746 Discovery board
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/mtd/stm32_qspi.h>

#define stringify(s)	tostring(s)
#define tostring(s)	#s

/*
 * Disable debug messages
 */
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
#define CONFIG_SYS_STM32F7_DISCO

/*
 * Enable GPIO driver
 */
#define CONFIG_STM32F2_GPIO

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

#define CONFIG_SYS_BOARD_REV_STR	"1.A"

/*
 * Monitor prompt
 */
#define CONFIG_SYS_PROMPT		"STM32F746-DISCO> "

/*
 * For "&&" support in commands in env variables
 */
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_SYS_PROMPT_HUSH_PS2	"> "

/*
 * We want to call the CPU specific initialization
 */
#define CONFIG_ARCH_CPU_INIT

#define BOARD_LATE_INIT

/*
 * Clock configuration (see mach-stm32/clock.c for details):
 * - use PLL as the system clock;
 * - use HSE as the PLL source;
 * - configure PLL to get 200MHz system clock.
 */
#define CONFIG_STM32_SYS_CLK_PLL
#define CONFIG_STM32_PLL_SRC_HSE
#define CONFIG_STM32_HSE_HZ		25000000	/* 25 MHz */
#define CONFIG_STM32_PLL_M		25
#define CONFIG_STM32_PLL_N		400
#define CONFIG_STM32_PLL_P		2
#define CONFIG_STM32_PLL_Q		8

#define CONFIG_SPI
#define CONFIG_SPI_FLASH
#define CONFIG_SPI_FLASH_SIZE_OFF	24		/* 2^24 = 16MiB */
#define CONFIG_STM32_QSPI
#define CONFIG_STM32_QSPI_FREQ		100000000	 /* max we can get from HCLK=200MHz */

/*
 * N25Q Flash specific configs
 */
#define CONFIG_STM32_QSPI_FAST_READ_DUMMY_CYCLES	10
#define CONFIG_STM32_QSPI_FAST_PROGRAM_CMD		0x12
#define CONFIG_STM32_QSPI_64KB_ERASE_TYP_TIME_MS	700
#define CONFIG_STM32_QSPI_256B_PROGRAM_TYP_TIME_US	500

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
/* #undef CONFIG_STM32F7_ICACHE_ON */
#define CONFIG_STM32F7_DCACHE_ON
/* #undef CONFIG_STM32F7_DCACHE_ON */

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
					 CONFIG_SYS_RAM_SIZE - \
					 CONFIG_DMAMEM_SZ_ALL)
#endif

#define CONFIG_ARMCORTEXM3_SOC_INIT

/*
 * Memory layout configuration
 *
 * On-chip Flash:
 */
#define CONFIG_MEM_NVM_BASE		0x08000000
#define CONFIG_MEM_NVM_LEN		(1024 * 1024 * 1)
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
 * Even though the board uses a 16 MByte SDRAM device
 * only half of that is used by the hardware design
 */
#define CONFIG_NR_DRAM_BANKS		1
#define CONFIG_SYS_RAM_SIZE		(8 * 1024 * 1024)
#define CONFIG_SYS_RAM_FREQ_DIV	2
#define CONFIG_SYS_RAM_BASE		0xC0000000

/*
 * Configuration of the external Flash memory
 * TBD: Check if external Flash is available on the Discovery board.
 * If not, remove the below defines related to external Flash.
 */
#if 1

/*
 * No external Flash
 */
#define CONFIG_SYS_NO_FLASH

#else

#define CONFIG_SYS_FLASH_CS		1

/*
 * Flash is in ModeC, that means 'OE toggle on write'
 *
 * MBKEN(0) = 1, enable memory bank
 * MTYP(3-2) = 0b10, NOR flash
 * MWID(5-4) = 0b01, 16 bit
 * FACCEN(6) = 1,
 * reserved(7) = 0,
 * WREN(12) = 1,
 * EXTMOD(14) = 1
 */
#define CONFIG_SYS_FSMC_FLASH_BCR	0x00005059

/*
 * See Spansion memory reference manual for S29GL128S10DHI010
 * Read:
 * ADDSET(3-0) = 25 ns = 5 HCLK (on 200 MHz)
 * DATAST(15-8) = 110 ns = 22 HCLK (on 200 MHz)
 * BUSTURN(19-16) = 10 ns = 2 HCLK
 * ACCMODE(29-28) = 0x2 (mode C)
 * Write:
 * ADDSET(3-0) = 35 ns = 7 HCLK (on 200 MHz)
 * DATAST(15-8) = 25 ns + 1HCLC = 6 HCLK (on 200 MHz)
 * BUSTURN(19-16) = 10 ns = 2 HCLK
 * ACCMODE(29-28) = 0x2 (mode C)
 */
#define CONFIG_SYS_FSMC_FLASH_BTR	0x20021605
#define CONFIG_SYS_FSMC_FLASH_BWTR	0x20020607
#define CONFIG_FSMC_NOR_PSRAM_CS1_ENABLE

#define CONFIG_SYS_FLASH_BANK1_BASE	\
	FSMC_NOR_PSRAM_CS_ADDR(CONFIG_SYS_FLASH_CS)

#define CONFIG_SYS_FLASH_CFI		1
#define CONFIG_FLASH_CFI_DRIVER		1
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BANK1_BASE }
#define CONFIG_SYS_MAX_FLASH_BANKS	1
#define CONFIG_SYS_MAX_FLASH_SECT	128
#define CONFIG_SYS_FLASH_CFI_AMD_RESET	1
#define CONFIG_SYS_FLASH_PROTECTION	1
#define CONFIG_SYS_FLASH_USE_BUFFER_WRITE
#define CONFIG_CFI_FLASH_USE_WEAK_ACCESSORS

#endif

/*
 * Store env in Flash memory
 * CONFIG_ENV_SIZE = 4 * 1024
 * CONFIG_ENV_ADDR = CONFIG_SYS_ENVM_BASE + (128 * 1024)
 */
#define CONFIG_ENV_IS_IN_ENVM
#define CONFIG_ENV_SIZE			0x1000
#define CONFIG_ENV_ADDR			0x8020000
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

/*
 * Framebuffer configuration
 */
#undef CONFIG_LCD

#ifdef CONFIG_LCD

#define CONFIG_FB_ADDR			CONFIG_DMAMEM_BASE

#define CONFIG_VIDEO_STM32F4_LTDC
#define CONFIG_STM32_LTDC_PIXCLK	(9 * 1000 * 1000)
#define LCD_EMCRAFT_IOT_LCD

#define CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN

#define CONFIG_BMP
#undef CONFIG_CMD_BMP
#define CONFIG_BMP_24BPP
#define LCD_BPP				LCD_COLOR32

#ifdef LCD_EMCRAFT_IOT_LCD
# define CONFIG_STM32F4_LTDC_XRES	480
# define CONFIG_STM32F4_LTDC_YRES	272
# define CONFIG_STM32F4_LTDC_BPP	LCD_BPP

# define CONFIG_STM32F4_LTDC_LEFT_MARGIN	2
# define CONFIG_STM32F4_LTDC_HSYNC_LEN		41
# define CONFIG_STM32F4_LTDC_RIGHT_MARGIN	2

# define CONFIG_STM32F4_LTDC_UPPER_MARGIN	2
# define CONFIG_STM32F4_LTDC_VSYNC_LEN		10
# define CONFIG_STM32F4_LTDC_LOWER_MARGIN	2

#elif defined(CONFIG_VIDEO_STM32F4_LTDC)
# error "STM32F7 LTDC is enabled but no LCD configured"
#endif

#endif /* CONFIG_LCD */

/*
 * Enable all those monitor commands that are needed
 */
#include <config_cmd_default.h>
#undef CONFIG_CMD_BOOTD
#undef CONFIG_CMD_CONSOLE
#define CONFIG_CMD_ECHO
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
#define CONFIG_BOOTCOMMAND		"run qspiboot || echo 'Boot from QSPI failed, run the update_kernel command'"

/* boot args and env */
#define CONFIG_HOSTNAME			stm32f7-disco
#define CONFIG_BOOTARGS			"stm32_platform=stm32f7-disco console=ttyS5,115200 panic=10"

#define LOADADDR			"0xC0007FC0"

#define REV_EXTRA_ENV						\
	"netboot=tftp ${image} && run args addip && bootm\0"    \
	"qspiboot=echo 'Booting from QSPI'" \
	" && cp.b " stringify(STM32_QSPI_BANK) " ${loadaddr} ${kernel_size}" \
	" && run args addip && bootm\0"                         \
	"update_uboot=tftp ${uboot_image}"                      \
	" && cptf " stringify(CONFIG_MEM_NVM_BASE) " ${loadaddr} ${filesize} do_reset\0" \
	"update_kernel=tftp ${image}"                           \
	" && qspi erase 0 ${filesize}"                          \
	" && qspi write ${loadaddr} 0 ${filesize}"              \
	" && setenv kernel_size ${filesize}"                    \
	" && saveenv"                                           \
	" && echo 'Successfully updated'\0"                     \
	"env_default=mw.b ${loadaddr} 0xFF " stringify(CONFIG_ENV_SIZE) "" \
	" && cptf ${envmaddr} ${loadaddr} " stringify(CONFIG_ENV_SIZE) "\0"

#define CONFIG_SYS_CONSOLE_IS_IN_ENV

/*
 * Short-cuts to some useful commands (macros)
 */
#define CONFIG_EXTRA_ENV_SETTINGS				\
	"loadaddr=" LOADADDR "\0"				\
	"args=setenv bootargs " CONFIG_BOOTARGS "\0"		\
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:eth0:off\0"				\
	"envmaddr=" stringify(CONFIG_ENV_ADDR) "\0"		\
	"ethaddr=C0:B1:3C:88:88:85\0"				\
	"ipaddr=172.17.4.206\0"					\
	"serverip=172.17.0.1\0"					\
	"image=stm32f7/uImage\0"		\
	"uboot_image=stm32f7/f746-disco.u-boot.bin\0"		\
	"stdin=serial\0"					\
	REV_EXTRA_ENV

/*
 * Linux kernel boot parameters configuration
 */
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG

#endif /* __CONFIG_H */
