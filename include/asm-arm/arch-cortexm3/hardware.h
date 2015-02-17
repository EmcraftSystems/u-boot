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
#define CM3_SCB_REGS		((volatile struct cm3_scb *)CM3_SCB_BASE)

#define CM3_AIRCR_VECTKEY		0x5fa
#define CM3_AIRCR_VECTKEY_SHIFT		16
#define CM3_AIRCR_ENDIAN		(1<<15)
#define CM3_AIRCR_PRIGROUP_MSK		0x7
#define CM3_AIRCR_PRIGROUP_SHIFT	8
#define CM3_AIRCR_SYSRESET		(1<<2)

#define CM3_ICSR_VECTACT_MSK		0xFF


/* MPU Base Address */
#define CM3_MPU_BASE			0xE000ED90
struct cm3_mpu {
	uint32_t type;			/* Type Register */
	uint32_t ctrl;			/* Control Register */
	uint32_t rnr;			/* Region Number Register */
	uint32_t rbar;			/* Region Base Address Register */
	uint32_t rasr;			/* Region Attribute and Size Register */
};
#define CM3_MPU_REGS		((volatile struct cm3_mpu *)CM3_MPU_BASE)

/*
 * MPU Region Base Address Register
 */
/*
 * MPU Region Attribute and Size Register
 */
/* Region enable bit */
#define CM3_MPU_RASR_EN			(1 << 0)
/*
 * Region size field
 */
#define CM3_MPU_RASR_SIZE_BITS		1
#define CM3_MPU_RASR_SIZE_4GB		(31 << CM3_MPU_RASR_SIZE_BITS)
/*
 * Access permission field
 */
/* Full access for both privileged and unprivileged software */
#define CM3_MPU_RASR_AP_RW_RW		(3 << 24)

/*
 * MPU Control register
 */
/*
 * Enables the operation of MPU during hard fault, NMI, and
 * FAULTMASK handlers
 */
#define CM3_MPU_CTRL_EN_MSK		(1 << 0)
#define CM3_MPU_CTRL_HFNMI_EN_MSK	(1 << 1)

/* SysTick Base Address */
#define CM3_SYSTICK_BASE		0xE000E010
struct cm3_systick {
	uint32_t ctrl;			/* Control and Status Register */
	uint32_t load;			/* Reload Value Register       */
	uint32_t val;			/* Current Value Register      */
	uint32_t cal;			/* Calibration Register        */
};

#define CM3_SYSTICK_LOAD_RELOAD_MSK	(0x00FFFFFF)
/* System Tick counter enable */
#define CM3_SYSTICK_CTRL_EN		(1 << 0)
/* System Tick clock source selection: 1=CPU, 0=STCLK (external clock pin) */
#define CM3_SYSTICK_CTRL_SYSTICK_CPU	(1 << 2)

u8 cortex_m3_irq_vec_get(void);

void cortex_m3_mpu_set_region(u32 region, u32 address, u32 attr);
void cortex_m3_mpu_enable(int enable);
void cortex_m3_mpu_full_access(void);

#endif
