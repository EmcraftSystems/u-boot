/*
 * (C) Copyright 2010,2011
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
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
#include <config.h>
#include <string.h>
#include "wdt.h"

#ifdef CONFIG_LPC18XX_NORFLASH_BOOTSTRAP_WORKAROUND
#include <asm/arch/lpc18xx_gpio.h>
#endif

 /*
 * FIXME: move to the appropriate header
 */
unsigned char cortex_m3_irq_vec_get(void);

/*
 * Declare symbols used in the start-up code
 */
extern void printf(const char *fmt, ...)
		__attribute__ ((format (__printf__, 1, 2)));

extern char _data_lma_start;
extern char _data_start;
extern char _data_end;

extern char _mem_stack_base, _mem_stack_end;
unsigned long _armboot_start;

extern char _bss_start;
extern char _bss_end;

void _start(void);
void default_isr(void);
#ifdef CONFIG_LPC18XX_USB
void USB0_IRQHandler(void);
#endif

extern void start_armboot(void);

#ifdef CONFIG_LPC18XX_NORFLASH_BOOTSTRAP_WORKAROUND
extern void lpc18xx_norflash_bootstrap_iomux_init(void);
#endif

/*
 * Control IRQs
 */
static inline void __attribute__((used)) __enable_irq(void)
{
	asm volatile ("cpsie i");
}

static inline void __attribute__((used)) __disable_irq(void)
{
	asm volatile ("cpsid i");
}

/*
 * Exception-processing vectors:
 */
unsigned int vectors[] __attribute__((section(".vectors"))) = {

	/*
	 * The first word is the stack base address (stack grows downwards)
	 * Stack is defined by reserving an area at high RAM in u-boot.lds.
	 */
	[0]		= (unsigned long)&_mem_stack_end,

	/*
	 * Reset entry point
	 */
	[1]		= (unsigned int)&_start,

	/*
	 * Other exceptions
	 */
#ifdef CONFIG_LPC18XX_USB
	[2 ... 23]	= (unsigned int)&default_isr,
	[24] = (unsigned int)&USB0_IRQHandler,
	[25 ... 165]	= (unsigned int)&default_isr
#else
	[2 ... 165]	= (unsigned int)&default_isr
#endif
};

#ifdef CONFIG_LPC18XX_NORFLASH_BOOTSTRAP_WORKAROUND
/*
 * This function will be called very early on U-Boot initialization to reload
 * the whole U-Boot image from NOR flash if we use bootloading from NOR flash.
 */
void __attribute__((section(".lpc18xx_image_top_text")))
	lpc18xx_bootstrap_from_norflash(void);
#endif /* CONFIG_LPC18XX_NORFLASH_BOOTSTRAP_WORKAROUND */

 /*
  * Reset entry point
  */
void
#ifdef CONFIG_LPC18XX_NORFLASH_BOOTSTRAP_WORKAROUND
	__attribute__((section(".lpc18xx_image_top_text")))
#endif
	_start(void)
{

	/*
	 * Make sure interrupts are disabled.
	 */
	__disable_irq();


	/*
	 * Depending on the config parameter, enable or disable the WDT.
	 */
#if !defined(CONFIG_HW_WATCHDOG)
#if !defined(CONFIG_SYS_M2S)
	wdt_disable();
#endif
#else
	wdt_enable();
#endif



#ifdef CONFIG_LPC18XX_NORFLASH_BOOTSTRAP_WORKAROUND
	/*
	 * Reload the whole U-Boot image from NOR flash.
	 * The Boot ROM on LPC4350 parts cannot load more than 32KBytes
	 * from NOR flash when booting.
	 */
	lpc18xx_bootstrap_from_norflash();
#endif /* CONFIG_LPC18XX_NORFLASH_BOOTSTRAP_WORKAROUND */

	/*
	 * Copy data and initialize BSS
	 * This is in lieu of the U-boot "conventional" relocation
	 * of code & data from Flash to RAM.
	 * With Cortex-M3, we execute from NVRAM (internal Flash),
	 * having relocated data to internal RAM (and having cleared the BSS
	 * area in internal RAM as well)
	 * Stack grows downwards; the stack base is set-up by the first
	 * value in the first word in the vectors.
	 */

	 memcpy(&_data_start, &_data_lma_start, &_data_end - &_data_start);
	 memset(&_bss_start, 0, &_bss_end - &_bss_start);

	/*
	 * In U-boot (armboot) lingvo, "go to the C code" -
	 * in fact, with M3, we are at the C code from the very beginning.
	 * In actuality, this is the jump to the ARM generic start code.
	 * ...
	 * Note initialization of _armboot_start below. The ARM generic
	 * code expects that this variable is set to the upper boundary of
	 * the malloc pool area.
	 * For Cortex-M3, where we do not relocate the code to RAM, I set
	 * the malloc pool right behind the stack. See how armboot_start
	 * is defined in the CPU specific .lds file.
	 */
	 
#ifdef CONFIG_LPC18XX_USB
	// Reenable interrupts
	__enable_irq();
#endif

	// ******************************
	// Check to see if we are running the code from a non-zero
    // address (eg RAM, external flash), in which case we need
    // to modify the VTOR register to tell the CPU that the
    // vector table is located at a non-0x0 address.
	unsigned int * pSCB_VTOR = (unsigned int *) 0xE000ED08;
	if ((unsigned int *)vectors!=(unsigned int *) 0x00000000) {
		// CMSIS : SCB->VTOR = <address of vector table>
		*pSCB_VTOR = (unsigned int)vectors;
	}
	 
	_armboot_start = (unsigned long)&_mem_stack_base;
	start_armboot();
}


/*
 * Default exception handler
 */
void __attribute__((naked, noreturn))
#ifdef CONFIG_LPC18XX_NORFLASH_BOOTSTRAP_WORKAROUND
	__attribute__((section(".lpc18xx_image_top_text")))
#endif
	default_isr(void);

void default_isr(void)
{
	/*
	 * Dump the registers
	 */
	asm("mov r0, sp; bl dump_ctx");

	/* And hang
	 */
	for (;;) ;
}

/*
 * Dump the registers on an exception we don't know how to process.
 */
static void __attribute__((used)) dump_ctx(unsigned int *ctx)
{
	static char *regs[] = {
		"R0", "R1", "R2", "R3", "R12", "LR", "PC", "PSR"
	};
	static char *exc[] = {
		0,
		0,
		"NMI",
		"HARD FAULT",
		"MEMORY MANAGEMENT",
		"BUS FAULT",
		"USAGE FAULT",
		"RESERVED",
		"RESERVED",
		"RESERVED",
		"RESERVED",
		"SVCALL",
		"DEBUG MONITOR",
		"RESERVED",
		"PENDSV",
		"SYSTICK",
};
	unsigned char vec = cortex_m3_irq_vec_get();
	int i;

	printf("UNHANDLED EXCEPTION: ");
	if (vec < 16) {
		printf("%s\n", exc[vec]);
	} else {
		printf("INTISR[%d]\n", vec - 16);
	}
	for (i = 0; i < 8; i++) {
		printf("  %s\t= %08x", regs[i], ctx[i]);
		if (((i + 1) % 2) == 0) {
			printf("\n");
		}
	}
}
