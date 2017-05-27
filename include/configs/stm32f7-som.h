/*
 * (C) Copyright 2011-2016
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
 * Configuration settings for the Emcraft STM32F7 SOM
 */

#ifndef __CONFIG_H
#define __CONFIG_H

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
#ifdef CONFIG_SYS_BOARD_UCL_BSB
# define CONFIG_SYS_PROMPT		"STM32F7-SOM-UCL> "
#else
# define CONFIG_SYS_PROMPT		"STM32F7-SOM> "
#endif

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
#define CONFIG_STM32_HSE_HZ		12000000	/* 12 MHz */
#define CONFIG_STM32_PLL_M		12
#define CONFIG_STM32_PLL_N		400
#define CONFIG_STM32_PLL_P		2
#define CONFIG_STM32_PLL_Q		8

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
 * without DCACHE. So, pass DMAMEM tag always.
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
 */
#define CONFIG_MEM_NVM_BASE		0x08000000
#define CONFIG_MEM_NVM_LEN		(1024 * 1024 * 1)
#define CONFIG_ENVM			1
#if defined(CONFIG_ENVM)
# define CONFIG_SYS_ENVM_BASE		0x08000000
# define CONFIG_SYS_ENVM_LEN		CONFIG_MEM_NVM_LEN
#endif


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
#ifndef CONFIG_SYS_RAM_SIZE
#define CONFIG_SYS_RAM_SIZE		(32 * 1024 * 1024)
#endif
#define CONFIG_SYS_RAM_FREQ_DIV	2
#define CONFIG_SYS_RAM_BASE		0xC0000000

/*
 * Configuration of the external Flash memory
 */
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

/*
 * Store env in Flash memory
 */
#if 0
# define CONFIG_ENV_IS_IN_ENVM
#else
# define CONFIG_ENV_IS_IN_FLASH
#endif
#define CONFIG_ENV_SIZE			(4 * 1024)
#if defined(CONFIG_ENV_IS_IN_ENVM)
# define CONFIG_ENV_ADDR 		(CONFIG_SYS_ENVM_BASE + (128 * 1024))
#else
# define CONFIG_ENV_ADDR		CONFIG_SYS_FLASH_BANK1_BASE
#endif
#define CONFIG_INFERNO			1
#define CONFIG_ENV_OVERWRITE		1

/*
 * Serial console configuration
 */
#define CONFIG_STM32_USART_CONSOLE

/*
 * USART1, TX PB.6, RX PA.10
 */
#define CONFIG_STM32_USART_PORT	1		/* USART1 */

#define CONFIG_STM32_USART_TX_IO_PORT	1	/* PORTB */
#define CONFIG_STM32_USART_TX_IO_PIN	6	/* GPIO6 */

#define CONFIG_STM32_USART_RX_IO_PORT	0	/* PORTA */
#define CONFIG_STM32_USART_RX_IO_PIN	10	/* GPIO10 */

#define CONFIG_BAUDRATE			115200
#define CONFIG_SYS_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }

/*
 * Ethernet configuration
 */
#define CONFIG_NET_MULTI
#define CONFIG_STM32_ETH
#define CONFIG_STM32_ETH_RMII
#define CONFIG_KSZ8081_RMII_FORCE

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
 * Framebuffer configuration
 */
#if defined(CONFIG_SYS_BOARD_UCL_BSB)
# define CONFIG_LCD
# define LCD_EMCRAFT_TN43_LCD
#elif defined(CONFIG_SYS_BOARD_IOT_BSB)
# define CONFIG_LCD
# define LCD_EMCRAFT_IOT_LCD
#else
# undef CONFIG_LCD
#endif

#ifdef CONFIG_LCD

#define CONFIG_FB_ADDR			CONFIG_DMAMEM_BASE

#define CONFIG_VIDEO_STM32F4_LTDC

#if defined(LCD_EMCRAFT_TN43_LCD)
# define CONFIG_STM32_LTDC_PIXCLK	(12 * 1000 * 1000)
#elif defined(LCD_EMCRAFT_IOT_LCD)
# define CONFIG_STM32_LTDC_PIXCLK	(9 * 1000 * 1000)
#else
# error "STM32F7 LTDC is enabled but no LCD configured"
#endif

#define CONFIG_SPLASH_SCREEN
#define CONFIG_SPLASH_SCREEN_ALIGN

#define CONFIG_BMP
#undef CONFIG_CMD_BMP
#define CONFIG_BMP_24BPP
#define LCD_BPP				LCD_COLOR32

#if defined(LCD_EMCRAFT_TN43_LCD) || \
    defined(LCD_EMCRAFT_IOT_LCD)
# define CONFIG_STM32F4_LTDC_XRES	480
# define CONFIG_STM32F4_LTDC_YRES	272
# define CONFIG_STM32F4_LTDC_BPP	LCD_BPP

# define CONFIG_STM32F4_LTDC_LEFT_MARGIN	2
# define CONFIG_STM32F4_LTDC_HSYNC_LEN		41
# define CONFIG_STM32F4_LTDC_RIGHT_MARGIN	2

# define CONFIG_STM32F4_LTDC_UPPER_MARGIN	2
# define CONFIG_STM32F4_LTDC_VSYNC_LEN		10
# define CONFIG_STM32F4_LTDC_LOWER_MARGIN	2
#endif

#endif /* CONFIG_LCD */

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
#define CONFIG_BOOTDELAY		3
#define CONFIG_ZERO_BOOTDELAY_CHECK
#define CONFIG_BOOTCOMMAND		"run flashboot"

/* boot args and env */
#define CONFIG_HOSTNAME			stm32f7-som
#define CONFIG_BOOTARGS			"stm32_platform=stm32f7-som "	\
					"console=ttyS0,115200 panic=10"

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
		"cptf ${envmaddr} ${loadaddr} ${filesize}\0"		\
	"flashboot=run args addip;bootm ${flashaddr}\0"			\
	"update=tftp ${image};"						\
		"prot off ${flashaddr} +${filesize};"			\
		"era ${flashaddr} +${filesize};"			\
		"cp.b ${loadaddr} ${flashaddr} ${filesize};"		\
		"setenv kernelsize ${filesize};"			\
		"setenv filesize; setenv fileaddr;"			\
		"saveenv\0"

#ifdef CONFIG_SPLASH_SCREEN
# define SPLASH_ENV							\
	"splashfile=\"logo.bmp\"\0"					\
	"splashaddr=60020000\0"						\
	"flashaddr=600A0000\0"						\
	"splashupdate=tftp ${splashfile};"				\
		"prot off ${splashaddr} +${filesize};"			\
		"era ${splashaddr} +${filesize};"			\
		"cp.b ${loadaddr} ${splashaddr} ${filesize};"		\
		"setenv splashimage ${splashaddr};"			\
		"saveenv\0"
#else
# define SPLASH_ENV							\
	"flashaddr=60020000\0"
#endif

#ifdef CONFIG_LCD
# define LCD_ENV	"gui=yes\0"
#else
# define LCD_ENV	""
#endif

#define CONFIG_SYS_CONSOLE_IS_IN_ENV

/*
 * Short-cuts to some useful commands (macros)
 */
#define CONFIG_EXTRA_ENV_SETTINGS				\
	"loadaddr=" LOADADDR "\0"				\
	"args=setenv bootargs " CONFIG_BOOTARGS "\0"		\
	"addip=setenv bootargs ${bootargs} ip=${ipaddr}:${serverip}:${gatewayip}:${netmask}:${hostname}:eth0:off\0"				\
	"envmaddr=08040000\0"					\
	"ethaddr=C0:B1:3C:88:88:85\0"				\
	"ipaddr=172.17.4.206\0"					\
	"serverip=172.17.0.1\0"					\
	"image=stm32f7/uImage\0"		\
	"stdin=serial\0"					\
	"netboot=tftp ${image};run args addip;bootm\0"		\
	REV_EXTRA_ENV						\
	SPLASH_ENV						\
	LCD_ENV

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
