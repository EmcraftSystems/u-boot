/*
 * (C) Copyright 2011
 * Sergei Poselenov, Emcraft systems, sposelenov@emcraft.com.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
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

/*
 * Cortex M3 common definitions - SCB, NVIC, System Timer, MPU.
 */

#ifndef _ARCH_HWCM3_H_
#define _ARCH_HWCM3_H_

/* SCB Base Address */
#define CM3_SCB_BASE			0xE000ED00
struct cm3_scb {
	uint32_t cpuid;			/* CPUID Base Register */
	uint32_t icsr;			/* Interrupt Control and State Register */
	uint32_t vtor;			/* Vector Table Offset Register */
	uint32_t aircr;			/* App Interrupt and Reset Control Register */
};

#define CM3_AIRCR_VECTKEY		0x5fa
#define CM3_AIRCR_VECTKEY_SHIFT		16
#define CM3_AIRCR_ENDIAN		(1<<15)
#define CM3_AIRCR_PRIGROUP_MSK		0x7
#define CM3_AIRCR_PRIGROUP_SHIFT	8
#define CM3_AIRCR_SYSRESET		(1<<2)

#define CM3_ICSR_VECTACT_MSK		0xFF


/* SysTick Base Address */
#define CM3_SYSTICK_BASE		0xE000E010
struct cm3_systick {
	uint32_t ctrl;			/* Control and Status Register */
	uint32_t load;			/* Reload Value Register       */
	uint32_t val;			/* Current Value Register      */
	uint32_t cal;			/* Calibration Register        */
};

#define CM3_SYSTICK_LOAD_RELOAD_MSK	(0x00FFFFFF)
#define CM3_SYSTICK_CTRL_EN		1

void __attribute__((section(".ramcode"))) __attribute__ ((long_call))
cortex_m3_reset_cpu(ulong addr);
unsigned char cortex_m3_irq_vec_get(void);

#endif
