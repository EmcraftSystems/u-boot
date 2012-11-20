/*
 * (C) Copyright 2012
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
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
#include "clock.h"

/*
 * The current code is good only for ENVM0.
 * It needs to be updated to work with ENVM1.
 * ...
 * eNVM control & status registers
 */
struct mss_envm {
	unsigned int		reserved_0_to_80[32];
	unsigned int		wdbuff[32];
	unsigned int		reserved_100_to_120[8];
	unsigned int		status;
	unsigned int		reserved_124_to_128[1];
	unsigned int		nv_page_status;
	unsigned int		nv_freq_rng;
	unsigned int		nv_dpd_b;
	unsigned int		nv_ce;
	unsigned int		reserved_138_to_140[2];
	unsigned int		page_lock_set;
	unsigned int		dwsize;
	unsigned int		cmd;
	unsigned int		reserved_14c_to_154[2];
	unsigned int		inten;
	unsigned int		clrhint;
	unsigned int		reserved_15c_to_1fc[40];
	unsigned int		reqaccess;
};

/*
 * eNVM registers access handle
 */
#define MSS_ENVM_REGS_BASE		0x60080000
#define MSS_ENVM			((volatile struct mss_envm *) \
					(MSS_ENVM_REGS_BASE))

/*
 * Base address of the eNVM Flash
 */
#define MSS_ENVM_BASE			0x60000000

/*
 * eNVM Flash size.
 * TO-DO: this needs to be made a function of some build-time,
 * perhaps even run-time, parameter defining a SmartFusion chip model.
 */
#define MSS_ENVM_FLASH_SIZE		(1024 * 256)

/*
 * eNVM page parameters
 */
#define MSS_ENVM_PAGE_SIZE		128

/*
 * Various bit fields
 */
#define MSS_ENVM_REQACCESS_EXCL		0x1
#define MSS_ENVM_REQACCESS_BY_M3	0x5
#define MSS_ENVM_STATUS_READY		0x1
#define MSS_ENVM_CMD_PROGRAM_ADS	(0x08<<24)

/*
 * Initialize the eNVM interface
 */
void envm_init(void)
{
}

/*
 * Update an eNVM page.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in eNMV.
 * @param page		eNVM page
 * @param part_begin	updated area start offset in page
 * @param part_end	updated area end offset in page
 * @param buf		data to write
 * @returns		0 -> success
 */
unsigned int __attribute__((section(".ramcode")))
             __attribute__ ((long_call))
  envm_write_page_part(
	unsigned int page, unsigned int part_begin,
	unsigned int part_end, unsigned char *buf)
{
	/*
	 * If not writing a whole page, save the existing data in this page
	 */
	if (part_begin != 0 || part_end != MSS_ENVM_PAGE_SIZE) {
		memcpy((void *) MSS_ENVM->wdbuff,
			(const void *) ((unsigned char *) MSS_ENVM_BASE +
				page * MSS_ENVM_PAGE_SIZE),
			MSS_ENVM_PAGE_SIZE);
	}

	/*
	 * Copy new data to the Write Buffer
	 */
	memcpy((void *) ((unsigned char *) MSS_ENVM->wdbuff + part_begin),
		buf, part_end - part_begin);

	/*
	 * Wait for Ready in Status
	 */
	while (!(MSS_ENVM->status & MSS_ENVM_STATUS_READY));

	/*
	 * Issue the ProgramADS command (ProgramAd + ProgramDa + ProgramStart)
	 */
	MSS_ENVM->cmd = MSS_ENVM_CMD_PROGRAM_ADS | (page * MSS_ENVM_PAGE_SIZE);

	/*
	 * Wait for Ready in Status
	 */
	while (!(MSS_ENVM->status & MSS_ENVM_STATUS_READY));

	return 0;
}

/*
 * Write a data buffer to eNVM.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in eNMV.
 * @param offset	eNVM offset
 * @param buf		data to write
 * @param size		how many bytes to write
 * @returns		number of bytes writter
 */
unsigned int __attribute__((section(".ramcode")))
             __attribute__ ((long_call))
  envm_write(unsigned int offset, void *buf, unsigned int size)
{
	unsigned int page, first_page, last_page;

	/*
	 * Open exlusive access to eNVM by Cortex-M3
	 */
	MSS_ENVM->reqaccess = MSS_ENVM_REQACCESS_EXCL;
	while (MSS_ENVM->reqaccess != MSS_ENVM_REQACCESS_BY_M3);

	/*
	 * Update eNVM, page by page
	 */
	first_page = offset / MSS_ENVM_PAGE_SIZE;
	last_page = (offset + size - 1) / MSS_ENVM_PAGE_SIZE;

	for (page = first_page; page <= last_page; page++) {
		envm_write_page_part(page,
			max(0, offset - page * MSS_ENVM_PAGE_SIZE),
			min(MSS_ENVM_PAGE_SIZE,
				offset + size - page * MSS_ENVM_PAGE_SIZE),
			(unsigned char *)buf +
				max(page * MSS_ENVM_PAGE_SIZE - offset, 0));
	}

	/*
	 * Disable access to eNVM by Cortex-M3 core
	 */
	MSS_ENVM->reqaccess = ~MSS_ENVM_REQACCESS_EXCL;

	return size;
}

