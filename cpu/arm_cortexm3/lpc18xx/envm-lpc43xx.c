/*
 * (C) Copyright 2011-2013
 *
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
 * Pavel Boldin, Emcraft Systems, paboldin@emcraft.com
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
 * Convinience macro for selecting value for bank.
 */
#ifdef CONFIG_MEM_NVM2_BASE
# define CONFIG_NVM_VALUE(bank, VALUE) (bank ? CONFIG_MEM_NVM2_##VALUE : CONFIG_MEM_NVM_##VALUE)
#else
# define CONFIG_NVM_VALUE(bank, VALUE) (CONFIG_MEM_NVM_##VALUE)
#endif

/*
 * IAP library for editing eNVM
 */

/* IAP function pointer */
#define IAP_LOCATION_PTR	0x10400100
typedef void (*lpc43xn_iap) (u32 *, u32 *);

/* IAP commands */
#define IAP_CMD_INIT			49
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
#define IAP_BLOCK_SIZE			512
#define IAP_BLOCK_MASK			(IAP_BLOCK_SIZE - 1)

static u32 iap_clkrate;		/* CPU clock in KHz */

/*
 * List of block addresses and sizes
 *
 * IMPORTANT: We force this data into the `.data` section, because otherwise
 * it will be in eNVM and will be overwritten on self-upgrade.
 */
static lpc43xn_iap lpc43xn_iap_entry __attribute__((section(".data")));

struct lpc43xn_flash_layout {
	int sectors;
	int size;
};

static struct lpc43xn_flash_layout flash_layout[]
			__attribute__((section(".data"))) =
{
	{ 8,  8192},
	{ 7, 65536}
};


static u32 iap_commands[6];
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
lpc43xn_iap_prepare_sectors(u32 start, u32 end, u32 bank)
{
	iap_commands[0] = IAP_CMD_PREP_SECTORS;
	iap_commands[1] = start;
	iap_commands[2] = end;
	iap_commands[3] = bank;

	lpc43xn_iap_entry(iap_commands, iap_results);

	return iap_results[0];
}

/*
 * Copy RAM to FLASH
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
lpc43xn_iap_ram_to_flash(u32 dst, u32 src, u32 bytes)
{
	iap_commands[0] = IAP_CMD_RAM_TO_FLASH;
	iap_commands[1] = dst;
	iap_commands[2] = src;
	iap_commands[3] = bytes;
	iap_commands[4] = iap_clkrate;

	lpc43xn_iap_entry(iap_commands, iap_results);

	return iap_results[0];
}

/*
 * Erase sectors
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
lpc43xn_iap_erase_sectors(u32 start, u32 end, u32 bank)
{
	iap_commands[0] = IAP_CMD_ERASE_SECTORS;
	iap_commands[1] = start;
	iap_commands[2] = end;
	iap_commands[3] = iap_clkrate;
	iap_commands[4] = bank;

	lpc43xn_iap_entry(iap_commands, iap_results);

	return iap_results[0];
}

/*
 * Blank check sectors
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
lpc43xn_iap_blank_check_sectors(
	u32 start, u32 end, u32 bank, u32 *bad_addr, u32 *bad_data)
{
	iap_commands[0] = IAP_CMD_BLANK_CHECK_SECTORS;
	iap_commands[1] = start;
	iap_commands[2] = end;
	iap_commands[3] = bank;

	lpc43xn_iap_entry(iap_commands, iap_results);

	*bad_addr = iap_results[1];
	*bad_data = iap_results[2];

	return iap_results[0];
}

/*
 * Initialize IAP library - call this first
 *
 * This function should not be in .ramcode, because it will be called only once
 * before self-upgrade.
 */
void lpc43xn_iap_init(void)
{
	lpc43xn_iap_entry = (lpc43xn_iap) *(u32*)IAP_LOCATION_PTR;


	iap_commands[0] = IAP_CMD_INIT;
	lpc43xn_iap_entry(iap_commands, iap_results);

	if (iap_results[0] != IAP_STATUS_SUCCESS) {
		printf("%s: failed %d\n", __func__, iap_results[0]);
		return;
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
	lpc43xn_iap_init();
}

/*
 * `addr` is the offset from the beginning of eNVM
 */
static u32 __attribute__((section(".ramcode")))
	   __attribute__((long_call))
find_sector(u32 addr, u32 bank) {
	u32 i, j, current = CONFIG_NVM_VALUE(bank, BASE), sector = 0;

	for (i = 0; flash_layout[i].sectors; i++)
	{
		for (j = 0; j < flash_layout[i].sectors; j++)
		{
			if (addr < current + flash_layout[i].size)
			{
				return sector;
			}
			sector++;
			current += flash_layout[i].size;
		}
	}
	return -1;
}

/*
 * Erase FLASH sectors
 */
int __attribute__((section(".ramcode")))
    __attribute__((long_call))
lpc43xn_flash_erase(u32 offset, u32 size, u32 bank)
{
	int rv;
	u32 badaddr, baddata;
	u32 first, last;

	/*
	 * Convert (offset, size) to (first, last)
	 */
	first = find_sector(offset, bank);
	last = find_sector(offset + size - 1, bank);

	if (first < 0 || last < first ||
			last >= CONFIG_NVM_VALUE(bank, SECTORS)) {
		rv = -EINVAL;
		goto out;
	}

	if (lpc43xn_iap_prepare_sectors(first, last, bank)
		!= IAP_STATUS_SUCCESS) {
		rv = -EIO;
		goto out;
	}

	if (lpc43xn_iap_erase_sectors(first, last, bank)
		!= IAP_STATUS_SUCCESS) {
		rv = -EIO;
		goto out;
	}

	if (lpc43xn_iap_blank_check_sectors(first, last,
		bank, &badaddr, &baddata) != IAP_STATUS_SUCCESS) {
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
lpc43xn_flash_program(u32 dest_addr, u8 *src, u32 size)
{
	int rv;
	u32 offset;	/* Offset of the current block being written */
	u32 i;
	u32 sect, bank = 0;

#ifdef CONFIG_MEM_NVM2_BASE
	bank = CONFIG_MEM_NVM2_BASE <= dest_addr;
#endif

	u8 *dest = (u8 *)dest_addr;

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
	if (size > CONFIG_NVM_VALUE(bank, LEN)) {
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
		sect = find_sector(dest_addr + offset, bank);

		if (lpc43xn_iap_prepare_sectors(sect, sect, bank) !=
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
		if (lpc43xn_iap_ram_to_flash(
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
	int rv = 0, bank = 0;

	/*
	 * Clock in is CPU in KHz
	 *
	 * This initialization code cannot be put into `envm_init()`, because
	 * at the time when `envm_init()` is called `clock_get()` is not yet
	 * initialized.
	 */
	iap_clkrate = clock_get(CLOCK_SYSTICK) / 1000;

	if ((offset < CONFIG_MEM_NVM_BASE ||
	    offset + size > CONFIG_MEM_NVM_BASE + CONFIG_MEM_NVM_LEN)
#ifdef CONFIG_MEM_NVM2_BASE
	 && (offset < CONFIG_MEM_NVM2_BASE ||
	    offset + size > CONFIG_MEM_NVM2_BASE + CONFIG_MEM_NVM2_LEN)
#endif
	    ) {
		printf("%s: Address %#x is not in flash or "
			"size 0x%x is too big\n",
			__func__, offset, size);
		goto out;
	}

#ifdef CONFIG_MEM_NVM2_BASE
	bank = CONFIG_MEM_NVM2_BASE <= offset;
#endif

	rv = lpc43xn_flash_erase(offset, size, bank);
	if (rv < 0)
		goto out;

	rv = lpc43xn_flash_program(offset, buf,
		(size + IAP_BLOCK_MASK) & ~IAP_BLOCK_MASK);
	if (rv < 0)
		goto out;

	rv = size;
out:
	return rv;
}
