/*
 * Copyright (C) 2010,2011
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

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
	 * anymore, until a next power cycle.
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
	 * This (the period) can be made configurable of course
	 * however we do not really need that at this time.
	 */
	MSS_WDT->mvrp = 0xFFFFFFFF;
	MSS_WDT->load = 0xFFFFFF00;
	MSS_WDT->control = 0;

	/* Do a first strobe
	 */
	wdt_strobe();
}
