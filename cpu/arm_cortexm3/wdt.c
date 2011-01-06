#include "wdt.h"

/*
 * WDT control & status registers
 */
struct mss_wdt {
	unsigned int value;
	unsigned int load;
	unsigned int mvrp;
	unsigned int refresh;
	unsigned int enable;
	unsigned int control;
	unsigned int status;
	unsigned int ris;
};

#define MSS_WDT_REGS_BASE		0x40006000
#define MSS_WDT		((volatile struct mss_wdt *)(MSS_WDT_REGS_BASE))

/*
 * Disable & refresh constants
 */
#define MSS_WDT_DISABLE			0x4C6E55FA
#define MSS_WDT_REFRESH			0xAC15DE42

/*
 * Strobe the WDT
 */
void wdt_strobe(void)
{
	/*
	 * Do the strobe
	 */
	MSS_WDT->refresh = MSS_WDT_REFRESH;
}

/*
 * Disable the WDT.
 */
void wdt_disable(void)
{
	/* 
 	 * Disable WDT - unless this is done, we would have to strobe
 	 * the WDT every once in a while to avoid a reset.
 	 * Note that after the WDT is disabled, it can't be enabled
 	 * anymore, until a next re-boot.
	 */
	MSS_WDT->enable = MSS_WDT_DISABLE;
}

/*
 * Enable the WDT.
 */
void wdt_enable(void)
{
	/*
	 * Enable WDT -> period is about 40 seconds.
	 */
	MSS_WDT->mvrp = 0xFFFFFFFF;
	MSS_WDT->load = 0xFFFFFF00;
	MSS_WDT->control = 0;

	/* Do a first strobe
	 */
	wdt_strobe();
}
