/*
 * (C) Copyright 2006-2010 Emcraft Systems
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
 * This is an ARM Cortex-M3 CPU core
 */
#define CONFIG_ARMCORTEXM3
/*
 * This is the Actel SmartFusion (aka A2F) device
 */
#define CONFIG_A2F

/* System frequency (FCLK) coming out of reset */
#define CONFIG_SYS_RESET_SYSCLCK_FREQ	80000000uL

/*
 * Enable/disable debug messages
 */
#define DEBUG
#undef DEBUG

/*
 * Display CPU and Board information
 */
#define CONFIG_DISPLAY_CPUINFO		1
#define CONFIG_DISPLAY_BOARDINFO	1

/* 
 * Enbale all those monitor commands that are really needed
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
 * TO-DO: review this list
 */

/*
 * Configuration of the external RAM.
 */
#define CONFIG_NR_DRAM_BANKS        1
#define EXT_RAM_BASE                0x70000000
#define EXT_RAM_SIZE                (8 * 1024 * 1024)
#define CONFIG_SYS_FLASH_BANK1_BASE 0x74000000
//#define CONFIG_SYS_FLASH_BANK2_BASE 0x74800000

/* External Memory Controller settings
 *
 */

/* #define CONFIG_SYS_EMC0CS0CR	0x00002aad */ /* Slow timing */
#define CONFIG_SYS_EMC0CS0CR	0x00002225
#define CONFIG_SYS_EMC0CS1CR	0x000000af

#define CONFIG_SYS_EMCMUXCR		0x00000001

/*
 * MALLOC_LEN can't be more than the specified size!
 * Refer to u-boot.lds for further details.
 */
#define CONFIG_SYS_MALLOC_LEN	(1024*8)

/*
 * We want to call the CPU specific initialization
 */
#define CONFIG_ARCH_CPU_INIT

/*
 * No interrupts
 */
#undef CONFIG_USE_IRQ

/*
 * TO-DO: what is this? ... something to do with clocks...
 */
#undef CONFIG_FSL_ESDHC 

/*
 * To save memory
 */
#undef CONFIG_SYS_LONGHELP

/*
 * Monitor prompt
 */
#define CONFIG_SYS_PROMPT		"A2F-LNX-EVB> "

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
 * Max number of command args
 */
#define CONFIG_SYS_MAXARGS 16

/*
 *
 */

#define CONFIG_SYS_LOAD_ADDR 0
#define CONFIG_SYS_MEMTEST_START 0
#define CONFIG_SYS_MEMTEST_END 0
#define CONFIG_SYS_HZ 1000
/* system core clock /32 */
#define CONFIG_SYSTICK_FREQ 3125000

/*
 * Serial console configuration
 */

#define CONFIG_BAUDRATE                 115200
#define CONFIG_CONS_INDEX               1
#define CONFIG_SYS_NS16550
#undef	CONFIG_NS16550_MIN_FUNCTIONS
#define CONFIG_SYS_NS16550_SERIAL
#define CONFIG_SYS_NS16550_COM1         0x40000000
#define CONFIG_SYS_NS16550_COM2		0x40010000
#define CONFIG_SYS_NS16550_REG_SIZE     (-4)
#define CONFIG_SYS_NS16550_CLK          (100)

/* valid baudrates */
#define CONFIG_SYS_BAUDRATE_TABLE       { 9600, 19200, 38400, 57600, 115200 }

 /*
  * Enable/disable h/w watchdog
  */
#undef CONFIG_HW_WATCHDOG

/* 
 * Short-cut to a command sequence to perform self-upgrade 
 */

#if 0
#define CONFIG_EXTRA_ENV_SETTINGS \
	"s=cpff 0; cptf 20000; " \
	  "cpff 8000; cptf 28000; " \
	  "cptf 0 20000 10000 1\0"
#endif

/*-----------------------------------------------------------------------
 * FLASH organization
 */
/* use CFI flash driver */
#define CONFIG_SYS_FLASH_CFI		1	/* Flash is CFI conformant */
#define CONFIG_FLASH_CFI_DRIVER		1	/* Use the common driver */
#define CONFIG_SYS_FLASH_CFI_WIDTH	FLASH_CFI_16BIT
#define CONFIG_SYS_FLASH_BANKS_LIST	{ CONFIG_SYS_FLASH_BANK1_BASE }
#define CONFIG_SYS_MAX_FLASH_BANKS	1	/* max number of memory banks */
#define CONFIG_SYS_MAX_FLASH_SECT	128	/* max number of sectors on one chip */
#define CONFIG_SYS_FLASH_CFI_AMD_RESET

#define CONFIG_SYS_MONITOR_BASE  0x0 
#define CONFIG_MONITOR_IS_IN_RAM 1

/* ENV settings */
#define CONFIG_ENV_IS_IN_FLASH 1
#define CONFIG_ENV_ADDR        CONFIG_SYS_FLASH_BANK1_BASE
#define CONFIG_ENV_SIZE        0x1000
#define CONFIG_INFERNO         1

#define CONFIG_BOOTDELAY    10
#define CONFIG_ZERO_BOOTDELAY_CHECK

/* Kernel parameters */
#if 0
#define CONFIG_SERIAL_TAG
#endif
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_CMDLINE_TAG

#define CONFIG_CMD_NET
#define CONFIG_NET_MULTI
#define CONFIG_CORE10100	1
/* allocated Rx and Tx buffers in internal RAM */
#define CONFIG_CORE10100_INTRAM_ADDRESS	   0x20008000
#define CONFIG_BITBANGMII	1
#undef CONFIG_CMD_MII

#if defined(CONFIG_BITBANGMII)
#include <asm/io.h>
/* Register offsets */
#define RCSR9			0x48
#define RCSR9_MDC		(1 << 16)
#define RCSR9_MDO		(1 << 17)
#define RCSR9_MDEN		(1 << 18)
#define RCSR9_MDI		(1 << 19)

/* CORE 10/100 base address */
#define ETH_CORE_BASE		0x40003000
#define READ_REG(reg)		readl(ETH_CORE_BASE + reg)
#define WRITE_REG(reg, val)	writel(val, ETH_CORE_BASE + reg)

/* Direction - output */
#define MDIO_ACTIVE		(WRITE_REG(RCSR9, READ_REG(RCSR9) | RCSR9_MDEN))
/* Direction - input */
#define MDIO_TRISTATE	(WRITE_REG(RCSR9, READ_REG(RCSR9) & ~RCSR9_MDEN))
#define MDIO_READ		((READ_REG(RCSR9) & RCSR9_MDI) != 0)
#define MDIO(val)							\
	if (val) {							\
		WRITE_REG(RCSR9, READ_REG (RCSR9) | RCSR9_MDO);		\
	} else {							\
		WRITE_REG(RCSR9, READ_REG (RCSR9) & ~RCSR9_MDO);	\
	}
#define MDC(val)						 \
	if (val) {						 \
		WRITE_REG(RCSR9, READ_REG (RCSR9) | RCSR9_MDC);	 \
	} else {						 \
		WRITE_REG(RCSR9, READ_REG (RCSR9) & ~RCSR9_MDC); \
	}
#define MIIDELAY		udelay(1)
#endif

#endif /* __CONFIG_H */
