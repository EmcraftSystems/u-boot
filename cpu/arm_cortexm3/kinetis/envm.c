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

#include "envm.h"

/*
 * The Flash Configuration Field must be present in the image and
 * occupy addresses 0x00000400 to 0x0000040f.
 */
struct kinetis_flash_conf {
	u8 backdoor_key[8];	/* Backdoor Comparison Key */
	u8 fprot[4];		/* Program flash protection bytes */
	u8 fsec;		/* Flash security byte */
	u8 fopt;		/* Flash nonvolatile option byte */
	u8 feprot;		/* FlexNVM: EEPROM protection byte */
	u8 fdprot;		/* FlexNVM: Data flash protection byte */
};

/*
 * Program Flash Protection Registers
 */
#define KINETIS_FLASH_CONF_FPROT_UNPROTECTED	0xFF
/*
 * Data Flash Protection Register
 */
#define KINETIS_FLASH_CONF_FDPROT_UNPROTECTED	0xFF
/*
 * EEPROM Protection Register
 */
#define KINETIS_FLASH_CONF_FEPROT_UNPROTECTED	0xFF

/*
 * Flash Option Register
 */
#define KINETIS_FLASH_CONF_FOPT_EZPORT_BIT	1
#define KINETIS_FLASH_CONF_FOPT_EZPORT_EN \
	(1 << KINETIS_FLASH_CONF_FOPT_EZPORT_BIT)
#define KINETIS_FLASH_CONF_FOPT_LPBOOT_BIT	0
#define KINETIS_FLASH_CONF_FOPT_LPBOOT_NORMAL \
	(1 << KINETIS_FLASH_CONF_FOPT_LPBOOT_BIT)

/*
 * EEPROM Protection Register
 */
#define KINETIS_FLASH_CONF_FEPROT_UNPROTECTED	0xFF

/*
 * Flash Security Register
 */
/* Backdoor Key Security Enable */
#define KINETIS_FLASH_CONF_FSEC_KEYEN_BITS	6
#define KINETIS_FLASH_CONF_FSEC_KEYEN_DISABLE \
	(3 << KINETIS_FLASH_CONF_FSEC_KEYEN_BITS)
/* Mass Erase Enable */
#define KINETIS_FLASH_CONF_FSEC_MEEN_BITS	4
#define KINETIS_FLASH_CONF_FSEC_MEEN_ENABLE \
	(3 << KINETIS_FLASH_CONF_FSEC_MEEN_BITS)
/* Freescale Failure Analysis Access */
#define KINETIS_FLASH_CONF_FSEC_FSLACC_BITS	2
#define KINETIS_FLASH_CONF_FSEC_FSLACC_GRANTED \
	(3 << KINETIS_FLASH_CONF_FSEC_FSLACC_BITS)
/* Flash Security */
#define KINETIS_FLASH_CONF_FSEC_SEC_BITS	0
#define KINETIS_FLASH_CONF_FSEC_SEC_UNSECURE \
	(2 << KINETIS_FLASH_CONF_FSEC_SEC_BITS)

/*
 * This is an invalid backdoor key. But we will not secure the MCU and
 * therefore will not have to use the backdoor key to unsecure it.
 */
#define KINETIS_FLASH_CONF_BACKDOOR_KEY		{0xFF, 0xFF, 0xFF, 0xFF, \
						 0xFF, 0xFF, 0xFF, 0xFF}

/*
 * All 32 flash regions are unprotected
 */
#define KINETIS_FLASH_CONF_FPROT \
	{KINETIS_FLASH_CONF_FPROT_UNPROTECTED,	\
	KINETIS_FLASH_CONF_FPROT_UNPROTECTED,	\
	KINETIS_FLASH_CONF_FPROT_UNPROTECTED,	\
	KINETIS_FLASH_CONF_FPROT_UNPROTECTED}

/*
 * Disable chip lockout
 */
#define KINETIS_FLASH_CONF_FSEC \
	(KINETIS_FLASH_CONF_FSEC_KEYEN_DISABLE |	\
	KINETIS_FLASH_CONF_FSEC_MEEN_ENABLE |		\
	KINETIS_FLASH_CONF_FSEC_FSLACC_GRANTED |	\
	KINETIS_FLASH_CONF_FSEC_SEC_UNSECURE)		\

/*
 * Enable EzPort.
 * Disable low-power boot.
 */
#define KINETIS_FLASH_CONF_FOPT \
	(KINETIS_FLASH_CONF_FOPT_EZPORT_EN | \
	KINETIS_FLASH_CONF_FOPT_LPBOOT_NORMAL)

/*
 * FlexRAM is unprotected
 */
#define KINETIS_FLASH_CONF_FEPROT	KINETIS_FLASH_CONF_FEPROT_UNPROTECTED
/*
 * Data Flash is unprotected
 */
#define KINETIS_FLASH_CONF_FDPROT	KINETIS_FLASH_CONF_FDPROT_UNPROTECTED

const struct kinetis_flash_conf kinetis_flash_conf
__attribute__((section(".kinetis_flash_conf"), used)) = {
	.backdoor_key = KINETIS_FLASH_CONF_BACKDOOR_KEY,
	.fprot = KINETIS_FLASH_CONF_FPROT,
	.fsec = KINETIS_FLASH_CONF_FSEC,
	.fopt = KINETIS_FLASH_CONF_FOPT,
	.feprot = KINETIS_FLASH_CONF_FEPROT,
	.fdprot = KINETIS_FLASH_CONF_FDPROT
};

/*
 * Initialize internal Flash interface
 */
void envm_init(void)
{
	/*
	 * TBD
	 */

	return;
}

/*
 * Write a data buffer to internal Flash.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in internal Flash.
 */
unsigned int __attribute__((section(".ramcode")))
	     __attribute__ ((long_call))
  envm_write(unsigned int offset, void * buf, unsigned int size)
{
	/*
	 * TBD
	 */

	return 0;
}
