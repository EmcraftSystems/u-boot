/*
    Copyright (c) 2009-2010 Pigeon Point Systems.
    All rights reserved.
    Pigeon Point Systems proprietary and confidential.

    Description:
	This module implements the WDT API using
	the SmartFusion MSS Watchdog timer.
    
    $Revision: 9405 $
*/

#include "wdt.h"

#define MSS_WDT_CLOCK_RATE		100000000
#define MSS_WDT_DISABLE			0x4C6E55FA
#define MSS_WDT_REFRESH			0xAC15DE42

#define MSS_WDT_CTRL_TIMEOUTINT_EN	(1<<0)
#define MSS_WDT_CTRL_WAKEUPINT_EN	(1<<1)
#define MSS_WDT_CTRL_MODE_INTR		(1<<2)

/*
    Disable the watchdog by switching to INTR mode
    and leaving the interrupts disabled.  This allows
    re-enabling the watchdog later (so that the
    firmware continues to run after unit tests).
*/
void mss_wdt_disable(void)
{
    WATCHDOG->WDOGCONTROL = MSS_WDT_CTRL_MODE_INTR;
}

/*
    Watchdog driver API
 */

/*
    The following function initializes the
    internal watchdog timer.
*/
void wdt_init(void)
{
    WATCHDOG->WDOGMVRP = 0xFFFFFFFF;
    WATCHDOG->WDOGLOAD = CFG_WDT_TIMEOUT * (MSS_WDT_CLOCK_RATE / 1000);
    WATCHDOG->WDOGCONTROL = 0;
    wdt_reset();
}

/*
    The following function resets the internal
    watchdog timer.
 */
void wdt_reset(void)
{
    /* Refresh watchdog */
    WATCHDOG->WDOGREFRESH = MSS_WDT_REFRESH;
}

/*
    The following function disables the internal
    watchdog timer.
 */
void wdt_disable(void)
{
    mss_wdt_disable();
}

