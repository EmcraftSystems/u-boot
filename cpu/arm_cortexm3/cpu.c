
#include <common.h>
#include <command.h>
#include <asm-arm/arch-a2f/a2f.h>
#include "envm.h"
#include "wdt.h"
#include "CMSIS/a2fxxxm3.h"

DECLARE_GLOBAL_DATA_PTR;

int arch_cpu_init(void)
{
	/*
	 * CMSIS clock initialization
	 * TO-DO: move this somewhere else
	 */
#if 0
	SystemCoreClockUpdate();
#endif

	/*
	 * Initialize timer
	 * TO-DO: move this somewhere else
	 */
	SYSREG->SOFT_RST_CR &= ~(1 << 6);
	TIMER->TIM64_MODE = 0;
	TIMER->TIM1_CTRL = 0x03;

	envm_init();

	timer_init();

	/*
	 * Architecture number; used by the Linux kernel.
	 */
	gd->bd->bi_arch_number = MACH_TYPE_A2F;

	/*
	 * Address of the kernel boot parameters.
	 * Use start of the external RAM for that;
	 * kernel resides at offset 0x8000 in the external RAM.
	 */
	gd->bd->bi_boot_params = CONFIG_SYS_RAM_BASE;

        return 0;
}

int print_cpuinfo(void)
{
	printf("CPU: %s\n", "SmartFusion FPGA (Cortex-M3 Hard IP)");
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
 * Perform the low-level reset.
 */
void reset_cpu(ulong addr)
{
	/*
	 * Perform reset but keep priority group unchanged.
	 */
	A2F_SCB->aircr  = (0x5FA << 16) |
                          (A2F_SCB->aircr & (7<<8)) |
                          (1<<2); 
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
