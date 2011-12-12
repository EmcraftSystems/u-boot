/*
 * (C) Copyright 2011
 *
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
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

#include "wdt.h"

/*
 * Watchdog Status and Control Register High (STCTRLH) register bits
 */
#define KINETIS_WDT_STCTRLH_WDOGEN_MSK	(1 << 0)

/*
 * Unlock sequence for writing into the UNLOCK register of the Watchdog Timer
 * in order to unlock its configuration registers.
 */
#define KINETIS_WDT_UNLOCK_KEY1		0xC520
#define KINETIS_WDT_UNLOCK_KEY2		0xD928

/*
 * Watchdog timer (WDOG) register map
 */
struct kinetis_wdt_regs {
	u16 stctrlh;	/* Status and Control Register High */
	u16 stctrll;	/* Status and Control Register Low */
	u16 tovalh;	/* Time-out Value Register High */
	u16 tovall;	/* Time-out Value Register Low */
	u16 winh;	/* Window Register High */
	u16 winl;	/* Window Register Low */
	u16 refresh;	/* Refresh Register */
	u16 unlock;	/* Unlock Register */
	u16 tmrouth;	/* Timer Output Register High */
	u16 tmroutl;	/* Timer Output Register Low */
	u16 rstcnt;	/* Reset Count Register */
	u16 presc;	/* Prescaler Register */
};

/*
 * Watchdog Timer registers base
 */
#define KINETIS_WDT_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x00052000)
#define KINETIS_WDT			((volatile struct kinetis_wdt_regs *) \
					KINETIS_WDT_BASE)

/*
 * Strobe the WDT.
 */
void wdt_strobe(void)
{
	/*
	 * TBD
	 */

	return;
}

/*
 * Unlock the write once registers in the watchdog so they are writable
 * within the WCT (watchdog configuration time) period.
 */
static void wdt_unlock(void)
{
	KINETIS_WDT->unlock = KINETIS_WDT_UNLOCK_KEY1;
	KINETIS_WDT->unlock = KINETIS_WDT_UNLOCK_KEY2;
}

/*
 * Disable the WDT.
 */
void wdt_disable(void)
{
	wdt_unlock();

	/*
	 * Disable watchdog operation
	 */
	KINETIS_WDT->stctrlh &= ~KINETIS_WDT_STCTRLH_WDOGEN_MSK;
}

/*
 * Enable the WDT.
 */
void wdt_enable(void)
{
	/*
	 * TBD
	 */

	return;
}
