
#include <common.h>
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

        SYSREG->EMC_CS_1_CR = CONFIG_SYS_EMC0CS1CR;

	nvm_init();

#if 0
	printf("\nSystem Core Clock: %d\nPCLK0: %d\n"
		, SystemCoreClock/1000000, g_FrequencyPCLK0/1000000);
#endif

	timer_init();

        return 0;
}

int dram_init (void)
{
#if ( CONFIG_NR_DRAM_BANKS > 0 )
	/*
	 * EMC timing parameters for chip select 0
	 */
        SYSREG->EMC_CS_0_CR = CONFIG_SYS_EMC0CS0CR;

	/*
	 * External memory controller MUX configuration
	 * The EMC _SEL bit in the EMC_MUX_CR register is used
	 * to select either FPGA I/O or EMC I/O.
	 * 1 -> The multiplexed I/Os are allocated to the EMC.
	 */
        SYSREG->EMC_MUX_CR = CONFIG_SYS_EMCMUXCR;

        gd->bd->bi_dram[0].start = EXT_RAM_BASE;
        gd->bd->bi_dram[0].size = EXT_RAM_SIZE;
#endif

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
