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
#include <errno.h>

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
#define KINETIS_FLASH_CONF_FSEC_SEC_MSK \
	(3 << KINETIS_FLASH_CONF_FSEC_SEC_BITS)
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
 * Enable or disable EzPort (disabled by default).
 * Disable low-power boot.
 */
#if 0
#define KINETIS_FLASH_CONF_FOPT \
	(KINETIS_FLASH_CONF_FOPT_EZPORT_EN | \
	KINETIS_FLASH_CONF_FOPT_LPBOOT_NORMAL)
#else
#define KINETIS_FLASH_CONF_FOPT \
	KINETIS_FLASH_CONF_FOPT_LPBOOT_NORMAL
#endif

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
 * Address of the flash programming acceleration RAM
 */
#define FLASH_PROG_ACCEL_BASE	0x14000000
#define FLASH_PROG_ACCEL_PTR	((volatile u8 *)FLASH_PROG_ACCEL_BASE)
/*
 * Size in bytes of the flash programming acceleration RAM
 */
#if defined(CONFIG_ENVM_TYPE_K60)
#define FLASH_PROG_ACCEL_SIZE	(4 * 1024)
#elif defined(CONFIG_ENVM_TYPE_K70)
#define FLASH_PROG_ACCEL_SIZE	(16 * 1024)
#else
#error No CONFIG_ENVM_TYPE_XXX option defined
#endif /* CONFIG_ENVM_TYPE_XXX */
/*
 * Size of the bottom half of the flash programming acceleration RAM
 * that may be used for flash programming.
 */
#define FLASH_PROG_ACCEL_HALF	(FLASH_PROG_ACCEL_SIZE / 2)
/*
 * Size of a sector in the MCU internal flash is 4 Kbytes (K70) or
 * 2 Kbytes (K60) on the program flash only MCUs.
 */
#if defined(CONFIG_ENVM_TYPE_K60)
#define FLASH_SECTOR_SIZE	(2 * 1024)
#elif defined(CONFIG_ENVM_TYPE_K70)
#define FLASH_SECTOR_SIZE	(4 * 1024)
#else
#error No CONFIG_ENVM_TYPE_XXX option defined
#endif /* CONFIG_ENVM_TYPE_XXX */
/*
 * The measurement unit for the section size used by the Program Section
 * command.
 */
#if defined(CONFIG_ENVM_TYPE_K60)
#define KINETIS_ENVM_SIZE_UNIT_BITS	3	/* 8 bytes */
#elif defined(CONFIG_ENVM_TYPE_K70)
#define KINETIS_ENVM_SIZE_UNIT_BITS	4	/* 16 bytes */
#else
#error No CONFIG_ENVM_TYPE_XXX option defined
#endif /* CONFIG_ENVM_TYPE_XXX */
/*
 * Offset of the byte in flash that loads into the FSEC register on reset
 */
#define KINETIS_FLASH_FSEC_OFFSET	0x40C
/*
 * Pointer to the beginning of flash
 */
#define KINETIS_NVM_PTR	((u8 *)CONFIG_MEM_NVM_BASE)

/*
 * Check that the data for a sector fits the flash programming acceleration RAM
 *
 * If on some other MCU the flash sector size is larger than
 * `FLASH_PROG_ACCEL_HALF`, the code in `kinetis_flash_program()` must be
 * fixed accordingly.
 */
#if FLASH_SECTOR_SIZE > FLASH_PROG_ACCEL_HALF
#error Flash sector size exceeds the maximum flash programming block size
#endif

/*
 * FTFL commands
 */
#define FTFL_CMD_ERASE_SECTOR		0x09
#define FTFL_CMD_PROGRAM_SECTION	0x0B

/*
 * Flash Memory Controller (FMC) register map
 */
struct kinetis_fmc_regs {
	u32 pfapr;	/* Flash Access Protection Register */
	u32 pfb0cr;	/* Flash Bank 0 Control Register */
	u32 pfb1cr;	/* Flash Bank 1 Control Register */
};

/*
 * FMC registers base
 */
#define KINETIS_FMC_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x0001F000)
#define KINETIS_FMC			((volatile struct kinetis_fmc_regs *) \
					KINETIS_FMC_BASE)

/*
 * Flash Bank Control Registers (FMC_PFB0CR, FMC_PFB1CR)
 */
/* Bank Data Cache Enable */
#define KINETIS_FMC_PFBCR_BDCE_MSK	(1 << 4)
/* Bank Single Entry Buffer Enable */
#define KINETIS_FMC_PFBCR_BSEBE_MSK	(1 << 0)

/*
 * Flash Memory Module (FTFL) register map
 */
struct kinetis_ftfl_regs {
	u8 fstat;	/* Flash Status Register */
	u8 fcnfg;	/* Flash Configuration Register */
	u8 fsec;	/* Flash Security Register */
	u8 fopt;	/* Flash Option Register */

	/* Flash Common Command Object Registers (3:0) */
	u8 fccob3;
	u8 fccob2;
	u8 fccob1;
	u8 fccob0;

	/* Flash Common Command Object Registers (7:4) */
	u8 fccob7;
	u8 fccob6;
	u8 fccob5;
	u8 fccob4;

	/* Flash Common Command Object Registers (0xB:8) */
	u8 fccobB;
	u8 fccobA;
	u8 fccob9;
	u8 fccob8;
};

/*
 * FTFL registers base
 */
#define KINETIS_FTFL_BASE		(KINETIS_AIPS0PERIPH_BASE + 0x00020000)
#define KINETIS_FTFL			((volatile struct kinetis_ftfl_regs *) \
					KINETIS_FTFL_BASE)

/*
 * Flash Status Register (FTFL_FSTAT)
 */
/* Command Complete Interrupt Flag */
#define KINETIS_FTFL_FSTAT_CCIF_MSK	(1 << 7)
/* FTFL Read Collision Error Flag */
#define KINETIS_FTFL_FSTAT_RDCOLERR_MSK	(1 << 6)
/* Flash Access Error Flag */
#define KINETIS_FTFL_FSTAT_ACCERR_MSK	(1 << 5)
/* Flash Protection Violation Flag */
#define KINETIS_FTFL_FSTAT_FPVIOL_MSK	(1 << 4)

/*
 * Execute an FTFL command
 *
 * `flash_addr` goes to FCCOB[3:1], converted to big-endian.
 * `data0` goes FCCOB[7:4], converted to big-endian.
 * `data1` goes FCCOB[0xB:8], converted to big-endian.
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
kinetis_ftfl_command(u8 command, u32 flash_addr, u32 data0, u32 data1)
{
	int rv;

	/*
	 * This problem exists only in first released product version (mask 0M33Z)
	 *
	 * Single entry buffer disable; Data Cache disable.
	 */
	KINETIS_FMC->pfb0cr &=
		~(KINETIS_FMC_PFBCR_BDCE_MSK | KINETIS_FMC_PFBCR_BSEBE_MSK);
	KINETIS_FMC->pfb1cr &=
		~(KINETIS_FMC_PFBCR_BDCE_MSK | KINETIS_FMC_PFBCR_BSEBE_MSK);

	/*
	 * Wait until the previous command is finished
	 */
	while (!(KINETIS_FTFL->fstat & KINETIS_FTFL_FSTAT_CCIF_MSK));

	/*
	 * Clear the error bits before starting the next command
	 */
	KINETIS_FTFL->fstat = (KINETIS_FTFL_FSTAT_ACCERR_MSK | KINETIS_FTFL_FSTAT_FPVIOL_MSK);

	/* Write the command code to FCCOB[0] */
	KINETIS_FTFL->fccob0 = command;

	/* Write the flash address to FCCOB[3:1] */
	KINETIS_FTFL->fccob1 = (u8)(flash_addr >> 16);	/* flash_addr[23:16] */
	KINETIS_FTFL->fccob2 = (u8)(flash_addr >> 8);	/* flash_addr[15:8] */
	KINETIS_FTFL->fccob3 = (u8)flash_addr;		/* flash_addr[7:0] */

	/* Write the data word 0 to FCCOB[7:4] */
	KINETIS_FTFL->fccob4 = (u8)(data0 >> 24);	/* data0[31:24] */
	KINETIS_FTFL->fccob5 = (u8)(data0 >> 16);	/* data0[23:16] */
	KINETIS_FTFL->fccob6 = (u8)(data0 >> 8);	/* data0[15:8] */
	KINETIS_FTFL->fccob7 = (u8)data0;		/* data0[7:0] */

	/* Write the data word 1 to FCCOB[7:4] */
	KINETIS_FTFL->fccob8 = (u8)(data1 >> 24);	/* data1[31:24] */
	KINETIS_FTFL->fccob9 = (u8)(data1 >> 16);	/* data1[23:16] */
	KINETIS_FTFL->fccobA = (u8)(data1 >> 8);	/* data1[15:8] */
	KINETIS_FTFL->fccobB = (u8)data1;		/* data1[7:0] */

	/*
	 * Start command execution
	 */
	KINETIS_FTFL->fstat = KINETIS_FTFL_FSTAT_CCIF_MSK;
	while (!(KINETIS_FTFL->fstat & KINETIS_FTFL_FSTAT_CCIF_MSK));

	rv = 0;
	if (KINETIS_FTFL->fstat &
	    (KINETIS_FTFL_FSTAT_ACCERR_MSK | KINETIS_FTFL_FSTAT_FPVIOL_MSK |
	     KINETIS_FTFL_FSTAT_RDCOLERR_MSK))
		rv = -EIO;

	return rv;
}

/*
 * Run the "Erase Sector" command
 *
 * `sect` is the sector number.
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
kinetis_ftfl_erase_sector(u32 sect)
{
	return kinetis_ftfl_command(FTFL_CMD_ERASE_SECTOR,
		sect * FLASH_SECTOR_SIZE, 0, 0);
}

/*
 * Run the "Program Section" command
 *
 * `size` is the length of the data to program.
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
kinetis_ftfl_program_section(u32 dest_addr, u32 size)
{
	/*
	 * ">> 3" (K60): We divide `size` by 8 to convert the size in bytes
	 * into the size in phrases (64 bits of data).
	 *
	 * ">> 4" (K70): We divide `size` by 16 to convert the size in bytes
	 * into the size in double-phrases (128 bits of data).
	 *
	 * "<< 16": Then we move the two bytes inside the `data0` word,
	 * so that they go into the FCCOB[5:4] registers.
	 */
	return kinetis_ftfl_command(FTFL_CMD_PROGRAM_SECTION,
		dest_addr, size << (16 - KINETIS_ENVM_SIZE_UNIT_BITS), 0);
}

/*
 * A copy of the `memcpy()` function in the `.ramcode` section
 */
void __attribute__((section(".ramcode")))
     __attribute__((long_call))
memcpy_ramcode(volatile u8 *dest, const volatile u8 *src, u32 count)
{
	while (count--)
		*dest++ = *src++;
}

/*
 * Copy memory buffer to a sector in flash
 *
 * This function also erases the sector before programming it.
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
kinetis_flash_program_sector(u32 sect, u32 sect_offset, u8 *src, u32 size)
{
	int rv;

	if (sect >= CONFIG_MEM_NVM_LEN / FLASH_SECTOR_SIZE) {
		/* The sector number is too big */
		rv = -EINVAL;
		goto out;
	}
	if (sect_offset + size > FLASH_SECTOR_SIZE) {
		/* Attempt to write into more than one sector */
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Save the existing data before the requested region
	 */
	if (sect_offset > 0) {
		memcpy_ramcode(FLASH_PROG_ACCEL_PTR,
			KINETIS_NVM_PTR + sect * FLASH_SECTOR_SIZE,
			sect_offset);
	}
	/*
	 * Save the existing data after the requested region
	 */
	if (sect_offset + size < FLASH_SECTOR_SIZE) {
		memcpy_ramcode(FLASH_PROG_ACCEL_PTR + sect_offset + size,
			KINETIS_NVM_PTR + sect * FLASH_SECTOR_SIZE +
				sect_offset + size,
			FLASH_SECTOR_SIZE - (sect_offset + size));
	}
	/*
	 * Copy the input data into the flash programming acceleration RAM
	 */
	memcpy_ramcode(FLASH_PROG_ACCEL_PTR + sect_offset, src, size);

	/*
	 * Erase the sector
	 */
	rv = kinetis_ftfl_erase_sector(sect);
	if (rv != 0)
		goto out;

	/*
	 * Program the sector
	 */
	rv = kinetis_ftfl_program_section(sect * FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);
	if (rv != 0)
		goto out;

	rv = 0;
out:
	return rv;
}

/*
 * Copy memory buffer to flash
 *
 * This function also erases the necessary flash sectors before programming
 * them.
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
kinetis_flash_program(u32 dest_addr, u8 *src, u32 size)
{
	int rv;
	u32 sect, first_sect, last_sect;
	u32 sect_offset, sect_len;

	first_sect = dest_addr / FLASH_SECTOR_SIZE;
	last_sect = (dest_addr + size - 1) / FLASH_SECTOR_SIZE;
	for (sect = first_sect; sect <= last_sect; sect ++) {
		if (first_sect == last_sect) {
			sect_offset = dest_addr - first_sect * FLASH_SECTOR_SIZE;
			sect_len = size;
		} else if (sect == first_sect) {
			sect_offset = dest_addr - first_sect * FLASH_SECTOR_SIZE;
			sect_len = FLASH_SECTOR_SIZE - sect_offset;
		} else if (sect == last_sect) {
			sect_offset = 0;
			sect_len = dest_addr + size - last_sect * FLASH_SECTOR_SIZE;
		} else { /* a sector in the middle */
			sect_offset = 0;
			sect_len = FLASH_SECTOR_SIZE;
		}

		rv = kinetis_flash_program_sector(sect, sect_offset,
			src + (sect * FLASH_SECTOR_SIZE + sect_offset - dest_addr),
			sect_len);
		if (rv != 0)
			goto out;
	}

	rv = 0;
out:
	return rv;
}

/*
 * Initialize internal Flash interface
 *
 * This function should not be in .ramcode, because it will be called only once
 * before self-upgrade.
 */
void envm_init(void)
{
}

/*
 * Write a data buffer to internal Flash.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in internal Flash.
 */
u32 __attribute__((section(".ramcode")))
    __attribute__((long_call))
envm_write(u32 offset, void *buf, u32 size)
{
	u32 rv = 0;

	if (offset < CONFIG_MEM_NVM_BASE ||
	    offset + size > CONFIG_MEM_NVM_BASE + CONFIG_MEM_NVM_LEN) {
		printf("%s: Address %#x is not in flash or "
			"size 0x%x is too big\n",
			__func__, offset, size);
		goto out;
	}

	if (KINETIS_FLASH_FSEC_OFFSET >= offset &&
	    KINETIS_FLASH_FSEC_OFFSET < offset + size &&
	    (((u8 *)buf)[KINETIS_FLASH_FSEC_OFFSET - offset] &
	    KINETIS_FLASH_CONF_FSEC_SEC_MSK) !=
	    KINETIS_FLASH_CONF_FSEC_SEC_UNSECURE) {
		printf("%s: You have attempted to secure\n"
			"    the MCU internal flash by writing the relevant\n"
			"    value at the address 0x40C of the flash. This\n"
			"    operation will not continue because it may lead\n"
			"    to lock-up of the MCU.\n",
			__func__);
		goto out;
	}

	if (kinetis_flash_program(offset, buf, size) < 0)
		goto out;

	rv = size;
out:
	return rv;
}
