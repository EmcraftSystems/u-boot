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

#if defined(CONFIG_ARMCORTEXM3_SOC_INIT)
extern void cortex_m3_soc_init(void);
#endif

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
#elif defined(CONFIG_SYS_LPC178X)
	gd->bd->bi_arch_number = MACH_TYPE_LPC178X;
#elif defined(CONFIG_SYS_LPC18XX)
	gd->bd->bi_arch_number = MACH_TYPE_LPC18XX;
#elif defined(CONFIG_SYS_KINETIS)
	gd->bd->bi_arch_number = MACH_TYPE_KINETIS;
#else
# error "Unsupported Cortex-M3 SOC."
#endif

	/*
	 * SoC configuration code that cannot be put into drivers
	 */
#if defined(CONFIG_ARMCORTEXM3_SOC_INIT)
	cortex_m3_soc_init();
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
void
#ifdef CONFIG_ARMCORTEXM3_RAMCODE
	__attribute__((section(".ramcode")))
	__attribute__((long_call))
#endif
	reset_cpu(ulong addr)
{
#ifdef CONFIG_SYS_LPC178X
	/*
	 * We use the function `lpc178x_pre_reset_cpu()` to reset
	 * the Ethernet PHY.
	 *
	 * LPC178x/7x requires a PHY reset immediately before resetting
	 * the SoC, otherwise the Ethernet block hangs after software reset
	 * of the SoC.
	 */
	lpc178x_pre_reset_cpu();
#endif

#ifdef CONFIG_SYS_LPC18XX
	/*
	 * Use watchdog reset on LPC18xx/43xx
	 */
	lpc18xx_reset_cpu();
#else
	/*
	 * Perform reset but keep priority group unchanged.
	 */
	CM3_SCB_REGS->aircr = (CM3_AIRCR_VECTKEY << CM3_AIRCR_VECTKEY_SHIFT) |
			  (CM3_SCB_REGS->aircr &
			  (CM3_AIRCR_PRIGROUP_MSK << CM3_AIRCR_PRIGROUP_SHIFT))
			  | CM3_AIRCR_SYSRESET;
#endif
}

/*
 * Dump the registers on an exception we don't know how to process.
 */
u8 cortex_m3_irq_vec_get(void)
{
	return CM3_SCB_REGS->icsr & CM3_ICSR_VECTACT_MSK;
}

/*
 * Add a custom MPU region
 *
 * region = Region Number
 * address = Region Base Address
 * attr = Region Attributes and Size
 */
void cortex_m3_mpu_add_region(u32 region, u32 address, u32 attr)
{
	CM3_MPU_REGS->rnr = region;
	CM3_MPU_REGS->rbar = address;
	CM3_MPU_REGS->rasr = attr;
}

/*
 * Enable or disable configurable MPU memory regions
 */
void cortex_m3_mpu_enable(int enable)
{
	CM3_MPU_REGS->ctrl =
		enable ? (CM3_MPU_CTRL_EN_MSK | CM3_MPU_CTRL_HFNMI_EN_MSK) : 0;
}
