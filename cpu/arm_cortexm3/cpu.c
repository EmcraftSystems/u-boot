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

#include <common.h>
#include <command.h>

#include "envm.h"
#include "wdt.h"
#include "clock.h"

DECLARE_GLOBAL_DATA_PTR;

/*
 * CPU specific initilization
 */
int arch_cpu_init(void)
{
	/*
	 * Initialize the eNVM driver
	 */
	envm_init();

	/*
	 * Initialize the timers.
	 */
	timer_init();

	/*
	 * Initialize the clock frequencies.
	 */
	clock_init();

	/*
	 * Architecture number; used by the Linux kernel.
	 */
#if defined(CONFIG_SYS_A2F)
	gd->bd->bi_arch_number = MACH_TYPE_A2F;
#elif defined(CONFIG_SYS_STM32)
	gd->bd->bi_arch_number = MACH_TYPE_STM32;
#else
# error "Unsupported Cortex-M3 SOC."
#endif

	/*
	 * Address of the kernel boot parameters.
	 * Use start of the external RAM for that;
	 * kernel resides at offset 0x8000 in the external RAM.
	 */
	gd->bd->bi_boot_params = CONFIG_SYS_RAM_BASE;

        return 0;
}

/*
 * This is called right before passing control to
 * the Linux kernel point.
 */
int cleanup_before_linux(void)
{
	return 0;
}

/*
 * H/w WDT strobe routine
 */
#if defined (CONFIG_HW_WATCHDOG)
void hw_watchdog_reset(void)
{
	/*
	 * Call the h/w-specific WDT strobe.
	 */
	wdt_strobe();
}
#endif

/*
 * Perform the low-level reset.
 * Note that we need for this function to reside in RAM since it
 * can be used to self-upgrade U-boot in eNMV.
 */
void __attribute__((section(".ramcode")))
		__attribute__ ((long_call))
		cortex_m3_reset_cpu(ulong addr)
{
	volatile struct cm3_scb *scb = (volatile struct cm3_scb *)CM3_SCB_BASE;
	/*
	 * Perform reset but keep priority group unchanged.
	 */
	scb->aircr  = (CM3_AIRCR_VECTKEY << CM3_AIRCR_VECTKEY_SHIFT) |
			  (scb->aircr &
			  (CM3_AIRCR_PRIGROUP_MSK << CM3_AIRCR_PRIGROUP_SHIFT)) |
			  CM3_AIRCR_SYSRESET;

}

/*
 * Dump the registers on an exception we don't know how to process.
 */
unsigned char cortex_m3_irq_vec_get(void)
{
	volatile struct cm3_scb *scb = (volatile struct cm3_scb *)CM3_SCB_BASE;
	return scb->icsr & CM3_ICSR_VECTACT_MSK;
}
