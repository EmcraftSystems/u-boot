#ifndef __WDT_H__
#define __WDT_H__

/*
 * Strobe the WDT
 */
extern void wdt_strobe(void);

/*
 * Disable the WDT.
 */
extern void wdt_disable(void);

/*
 * Enable the WDT.
 */
extern void wdt_enable(void);

#endif /* __WDT_H__ */
