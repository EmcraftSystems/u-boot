
#include <common.h>
#include <command.h>
#include <asm-arm/arch-a2f/a2f.h>
#include "my_uart.h"
#include "my_lib.h"
#include "nvm.h"
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

	/*
 	 * Initialize UART
	 * TO-DO: move this somewhere else
 	 */
	my_uart_init(115200);

	/*
	 * External memory controller MUX configuration
	 * The EMC _SEL bit in the EMC_MUX_CR register is used
	 * to select either FPGA I/O or EMC I/O.
	 * 1 -> The multiplexed I/Os are allocated to the EMC.
	 */
        A2F_SYSREG->emc_mux_cr = CONFIG_SYS_EMCMUXCR;
        A2F_SYSREG->emc_cs_1_cr = CONFIG_SYS_EMC0CS1CR;

	nvm_init();

	timer_init();

	/*
	 * Architecture number; used by the Linux kernel.
	 */
	gd->bd->bi_arch_number = MACH_TYPE_A2F;

	/*
	 * Address of the kernel boot parameters.
	 * Use start of the external RAM for that;
	 * kernel resides at offset 0x8000.
	 */
	gd->bd->bi_boot_params = EXT_RAM_BASE;

        return 0;
}

int print_cpuinfo(void)
{
        return 0;
}

int checkboard(void)
{
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
