/*
 * (C) Copyright 2010,2011
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
 * ENVM control & status registers
 */
struct mss_envm {
	u32 status;
	u32 control;
	u32 enable;
	u32 reserved0;
	u32 config_0;
	u32 config_1;
	u32 page_status_0;
	u32 page_status_1;
	u32 segment;
	u32 envm_select;
};

#define MSS_ENVM_REGS_BASE		0x60100000
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
 * Delay parameters.
 */
#define MSS_ENVM_MAX_WAIT_CNT		1000
#define MSS_ENVM_WAIT_INTERVAL		100

/*
 * eNVM page parameters
 */
#define MSS_ENVM_PAGE_SIZE		128
#define MSS_ENVM_PAGE_OFFSET_MASK	(MSS_ENVM_PAGE_SIZE - 1)
#define MSS_ENVM_PAGE_ADDR_MASK		(~MSS_ENVM_PAGE_OFFSET_MASK & 0xFFFFF)

/*
 * Various bit fields
 */
#define MSS_ENVM_CONTROL_CC_UNPROTECT	0x02000000
#define MSS_ENVM_CONTROL_CC_DISCARD	0x04000000
#define MSS_ENVM_CONTROL_CC_PROGRAM	0x10000000
#define MSS_ENVM_STATUS_BUSY		((1<<0)|(1<<16))
#define MSS_ENVM_STATUS_ERROR_MASK	(0x300|(0x300<<16))

/*
 * Initialize the eNVM interface
 */
void envm_init(void)
{
	MSS_ENVM->enable = 0;
	MSS_ENVM->config_0 = 0;
	MSS_ENVM->config_1 = 0;
	MSS_ENVM->envm_select = 0;
	MSS_ENVM->status = 0;
}

/*
 * Execute an eNVM command.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in eNMV.
 */
static s32 __attribute__((section(".ramcode")))
  mss_envm_exec_cmd(unsigned int addr, unsigned int cmd)
{
	/*
	 * Get the page address.
	 */
	addr &= MSS_ENVM_PAGE_ADDR_MASK;

	/*
	 * Clear any pending status
	 */
	MSS_ENVM->status = 0xFFFFFFFF;

	/*
	 * Start the command
	 */
	MSS_ENVM->control = addr | cmd;

	/*
	 * Wait for the command to finish
	 */
	while (MSS_ENVM->status & MSS_ENVM_STATUS_BUSY)
		;

	if (MSS_ENVM->status & MSS_ENVM_STATUS_ERROR_MASK) {
		/*
		 * This code below is a workaround for an occurance
		 * of the write count has exceeded the 10-year retention
		 * threshold error for some page.
		 * Apparently, that error is persistently set for some
		 * pages on the boards we have here (supposedly due to
		 * a power-off while programming the board using FlashPro.)
		 * ... Supposedly, FlashPro uses the same workaround.
		 * The bit value in the code below doesn't correspond to
		 * the value in the Actel SmartFusion DataSheet.
		 * It has been figured out experimentally (the same page
		 * on the board I have has that problem, and the code below
		 * allows to actually program that page with new content.
		 */
		if ((MSS_ENVM->status & 0x180) == 0x180)  {
			/*
			 * Assume the page has been programmed successfully
			 */
			return 0;
		}
		return -1;
	}

	/*
	 * All is good
	 */
	return 0;
}

/*
 * Write a data buffer to eNVM.
 * Note that we need for this function to reside in RAM since it
 * will be used to self-upgrade U-boot in eNMV.
 */
u32 __attribute__((section(".ramcode")))
             __attribute__ ((long_call))
  envm_write(u32 offset, void * buf, u32 size)
{
	u32 addr = MSS_ENVM_BASE + offset;
	u8 *src = (u8 *) buf;
	u32 i, written = 0;
	s32 ret = 0;

	/*
	 * Check the sanity of the request.
	 */
	if (offset > MSS_ENVM_FLASH_SIZE) {
		return 0;
	}

	for (i = 0; i < size; i++) {
		if (i == 0 || (addr & MSS_ENVM_PAGE_OFFSET_MASK) == 0) {
			/*
			 * We need to unprotect the Flash for the first byte
			 */
			ret = mss_envm_exec_cmd(addr,
						MSS_ENVM_CONTROL_CC_UNPROTECT);
			if (ret < 0) {
				break;
			}
		}

		/*
		 * Copy a byte of the data
		 */
		*(unsigned char *)addr++ = *src++;

		/*
		 * Commit the page to NVM on the last byte write
		 */
		if (i == size - 1 || (addr & MSS_ENVM_PAGE_OFFSET_MASK) == 0) {
			ret = mss_envm_exec_cmd(addr - 1,
						MSS_ENVM_CONTROL_CC_PROGRAM);
			if (ret < 0) {
				break;
			}
			written = i + 1;
		}
    }

    return written;
}

