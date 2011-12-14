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
 * IAP library for editing eNVM
 */

/* IAP function pointer */
#define IAP_LOCATION	0x1FFF1FF1
typedef void (*lpc178x_iap) (u32 *, u32 *);

/* IAP commands */
#define IAP_CMD_PREP_SECTORS		50
#define IAP_CMD_RAM_TO_FLASH		51
#define IAP_CMD_ERASE_SECTORS		52
#define IAP_CMD_BLANK_CHECK_SECTORS	53
#define IAP_CMD_READ_PART_ID		54

/* IAP statuses */
#define IAP_STATUS_SUCCESS		0

/*
 * Block size used for the "Copy RAM to Flash" operation
 *
 * Can be 256, 512, 1024 or 4096
 */
#define IAP_BLOCK_SIZE			256
#define IAP_BLOCK_MASK			(IAP_BLOCK_SIZE - 1)

static u32 iap_num_sectors;	/* Number of eNVM sectors */
static u32 iap_flash_size;	/* Size of eNVM in bytes */
static u32 iap_clkrate;		/* CPU clock in KHz */

/*
 * List of block addresses and sizes
 *
 * IMPORTANT: We force this data into the `.data` section, because otherwise
 * it will be in eNVM and will be overwritten on self-upgrade.
 */
static lpc178x_iap lpc178x_iap_entry __attribute__((section(".data"))) =
	(lpc178x_iap) IAP_LOCATION;
static u32 flash_bsize[] __attribute__((section(".data"))) = {
	[0 ... 15]	= 4 * 1024,
	[16 ... 29]	= 32 * 1024
};

static u32 iap_commands[5];
static u32 iap_results[5];

/*
 * A temporary buffer in SRAM (section `.bss`) used for the "Copy RAM to Flash"
 * operation. We need this buffer, because direct transfer from the external RAM
 * does not work.
 *
 * This buffer should be aligned on a 4 byte boundary.
 */
static u8 iap_sram_buf[IAP_BLOCK_SIZE] __attribute__((aligned(4)));

/*
 * Prepare sectors for erase or write
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
lpc178x_iap_prepare_sectors(u32 start, u32 end)
{
	iap_commands[0] = IAP_CMD_PREP_SECTORS;
	iap_commands[1] = start;
	iap_commands[2] = end;

	lpc178x_iap_entry(iap_commands, iap_results);

	return iap_results[0];
}

/*
 * Copy RAM to FLASH
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
lpc178x_iap_ram_to_flash(u32 dst, u32 src, u32 bytes)
{
	iap_commands[0] = IAP_CMD_RAM_TO_FLASH;
	iap_commands[1] = dst;
	iap_commands[2] = src;
	iap_commands[3] = bytes;
	iap_commands[4] = iap_clkrate;

	lpc178x_iap_entry(iap_commands, iap_results);

	return iap_results[0];
}

/*
 * Erase sectors
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
lpc178x_iap_erase_sectors(u32 start, u32 end)
{
	iap_commands[0] = IAP_CMD_ERASE_SECTORS;
	iap_commands[1] = start;
	iap_commands[2] = end;
	iap_commands[3] = iap_clkrate;

	lpc178x_iap_entry(iap_commands, iap_results);

	return iap_results[0];
}

/*
 * Blank check sectors
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
lpc178x_iap_blank_check_sectors(
	u32 start, u32 end, u32 *bad_addr, u32 *bad_data)
{
	iap_commands[0] = IAP_CMD_BLANK_CHECK_SECTORS;
	iap_commands[1] = start;
	iap_commands[2] = end;

	lpc178x_iap_entry(iap_commands, iap_results);

	*bad_addr = iap_results[1];
	*bad_data = iap_results[2];

	return iap_results[0];
}

/*
 * Read part identification number
 *
 * This function should not be in .ramcode, because it will be called only once
 * before self-upgrade.
 */
u32 lpc178x_iap_read_part_id_num(void)
{
	iap_commands[0] = IAP_CMD_READ_PART_ID;

	lpc178x_iap_entry(iap_commands, iap_results);

	return iap_results[1];
}

/*
 * Initialize IAP library - call this first
 *
 * This function should not be in .ramcode, because it will be called only once
 * before self-upgrade.
 */
void lpc178x_iap_init(void)
{
	u32 val;

	/* Read part ID first to determine size of FLASH */
	val = lpc178x_iap_read_part_id_num();

	/* Decode part ID to get number of sectors and size */
	if ((val & 0x08000000) != 0) {
		/* LPC178x */
		if ((val & 0x00000004) != 0) {
			/* LPC1787, LPC1788 */
			iap_flash_size = 512 * 1024;
			iap_num_sectors = 30;
		} else {
			/* LPC1785, LPC1786 */
			iap_flash_size = 256 * 1024;
			iap_num_sectors = 22;
		}
	} else {
		/* LPC177x */
		if ((val & 0x00000004) != 0) {
			/* LPC1777, LPC1778 */
			iap_flash_size = 512 * 1024;
			iap_num_sectors = 30;
		} else if ((val & 0x00000002) != 0) {
			/* LPC1776 */
			iap_flash_size = 256 * 1024;
			iap_num_sectors = 22;
		} else {
			/* LPC1774 */
			iap_flash_size = 128 * 1024;
			iap_num_sectors = 18;
		}
	}
}

/*
 * Initialize internal Flash interface
 *
 * This function should not be in .ramcode, because it will be called only once
 * before self-upgrade.
 */
void envm_init(void)
{
	lpc178x_iap_init();
}

/*
 * `addr` is the offset from the beginning of eNVM
 */
static u32 __attribute__((section(".ramcode")))
	   __attribute__((long_call))
find_sector(u32 addr) {
	u32 i;
	u32 sect_base = 0;

	for (i = 0; i < iap_num_sectors; i ++) {
		if (addr >= sect_base && addr < sect_base + flash_bsize[i])
			goto out;

		sect_base += flash_bsize[i];
	}

	i = -1;
out:
	return i;
}

/*
 * Erase FLASH sectors
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
lpc178x_flash_erase(u32 offset, u32 size)
{
	int rv;
	u32 badaddr, baddata;
	u32 first, last;

	/*
	 * Convert (offset, size) to (first, last)
	 */
	first = find_sector(offset);
	last = find_sector(offset + size - 1);

	if (first < 0 || last < first || last >= iap_num_sectors) {
		rv = -EINVAL;
		goto out;
	}

	if (lpc178x_iap_prepare_sectors(first, last) != IAP_STATUS_SUCCESS) {
		rv = -EIO;
		goto out;
	}

	if (lpc178x_iap_erase_sectors(first, last) != IAP_STATUS_SUCCESS) {
		rv = -EIO;
		goto out;
	}

	if (lpc178x_iap_blank_check_sectors(first, last, &badaddr, &baddata) !=
	    IAP_STATUS_SUCCESS) {
		rv = -EIO;
		goto out;
	}

	rv = 0;
out:
	return rv;
}

/*
 * Copy memory buffer to FLASH
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
lpc178x_flash_program(u32 dest_addr, u8 *src, u32 size)
{
	int rv;
	u32 offset;	/* Offset of the current block being written */
	u32 i;
	u32 sect;

	u8 *dest = (u8 *)(CONFIG_MEM_NVM_BASE + dest_addr);

	/*
	 * Write size must be on a block boundary
	 */
	if (size & IAP_BLOCK_MASK) {
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Write address must be on a 256 byte boundary
	 * (even if IAP_BLOCK_SIZE is not 256)
	 */
	if (dest_addr & 0xFF) {
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Write range should not exceed the end of FLASH
	 */
	if (dest_addr + size > iap_flash_size) {
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Check that the destination area is erased
	 */
	for (i = 0; i < size; i ++) {
		if (dest[i] != 0xFF) {
			/* FLASH is not blank */
			rv = -EEXIST;
			goto out;
		}
	}

	/*
	 * Write data in blocks of size IAP_BLOCK_SIZE
	 */
	for (offset = 0; offset < size; offset += IAP_BLOCK_SIZE) {
		sect = find_sector(dest_addr + offset);

		if (lpc178x_iap_prepare_sectors(sect, sect) !=
		    IAP_STATUS_SUCCESS) {
			rv = -EACCES;
			goto out;
		}

		/*
		 * Copy block into the second SRAM region
		 * ("Peripheral RAM"). Cannot use `memcpy()` here, because it
		 * resides in eNVM, not in `.ramcode` section.
		 */
		for (i = 0; i < IAP_BLOCK_SIZE; i ++)
			iap_sram_buf[i] = src[offset + i];

		/*
		 * Copy block from RAM to FLASH
		 */
		if (lpc178x_iap_ram_to_flash(
		    dest_addr + offset, (u32)iap_sram_buf, IAP_BLOCK_SIZE) !=
		    IAP_STATUS_SUCCESS) {
			rv = -EACCES;
			goto out;
		}

	}

	/*
	 * Verify
	 */
	for (i = 0; i < size; i ++) {
		if (dest[i] != src[i]) {
			rv = -EAGAIN;
			goto out;
		}
	}

	rv = 0;
out:
	return rv;
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

	/*
	 * Clock in is CPU in KHz
	 *
	 * This initialization code cannot be put into `envm_init()`, because
	 * at the time when `envm_init()` is called `clock_get()` is not yet
	 * initialized.
	 */
	iap_clkrate = clock_get(CLOCK_SYSTICK) / 1000;

	if (offset < CONFIG_MEM_NVM_BASE ||
	    offset + size > CONFIG_MEM_NVM_BASE + iap_flash_size) {
		printf("%s: Address %#x is not in flash or "
			"size 0x%x is too big\n",
			__func__, offset, size);
		goto out;
	}

	if (lpc178x_flash_erase(offset, size) < 0 ||
	    lpc178x_flash_program(offset, buf,
				  (size + IAP_BLOCK_MASK) &
				  ~IAP_BLOCK_MASK) < 0)
		goto out;

	rv = size;
out:
	return rv;
}
