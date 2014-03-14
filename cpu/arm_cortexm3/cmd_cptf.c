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
#include <command.h>
#include <string.h>
#include "envm.h"

/*
 * Base address and the length of buffer in internal RAM, which
 * we use as a source for buffer to be written to eNVM.
 * Size of this buffer is set in the configuration file, address
 * (position within internal RAM) is calculated automatically, at
 * linking stage. Thus, the appropriate symbols are exported from
 * the linker script *.lds.
 */
extern char	_mem_ram_buf_base, _mem_ram_buf_size;

#define SOC_RAM_BUFFER_BASE	(ulong)(&_mem_ram_buf_base)
#define SOC_RAM_BUFFER_SIZE	(ulong)(&_mem_ram_buf_size)

/*
 * Write the eNVM and, optionally, reset the CPU.
 * We need it in RAM so as to be able to update U-boot,
 * which itself runs from the eNVM.
 * NOTE: This function is only used locally, nevertheless, the "static"
 * attrubute was removed. This was done because otherwise GCC optimizes
 * this function to be inlined into do_cptf(), which is located in flash.
 * This makes the U-Boot crash while performing self-upgrade.
 */
int
#ifdef CONFIG_ARMCORTEXM3_RAMCODE
	__attribute__((section(".ramcode")))
	__attribute__((long_call))
#endif
	envm_write_and_reset(ulong dst, ulong src, ulong size, int do_reset)
{
	int ret = 0;

	/*
	 * Copy the buffer to the destination.
	 */
	ret = envm_write((uint) dst, (void *) src, (uint) size);
	if (ret != size) {
		goto Done;
	}

	/*
	 * If the user needs a reset, do the reset
	 */
	if (do_reset) {
		/*
		 * Cortex-M3 core reset.
		 */
		reset_cpu(0);

		/*
		 * Should never be here.
		 */
	}

	Done:
	return ret;
}

 /*
  * cptf: copy content of a buffer (including one in Flash) to Flash.
  */
int do_cptf(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong dst;
	ulong src = SOC_RAM_BUFFER_BASE;
	ulong size = SOC_RAM_BUFFER_SIZE;
	int do_reset = 0;
	int ret = 0;

	/*
	 * Check that at least the destination is specified
	 */
	if (argc == 1) {
		printf("%s: Flash offset must be specified\n",
                       (char *) argv[0]);
		goto Done;
	}

	/*
	 * Parse the command arguments
	 */
	dst = simple_strtoul(argv[1], NULL, 16);
	if (argc >= 3) {
		src = simple_strtoul(argv[2], NULL, 16);
	}
	if (argc >= 4) {
		size = simple_strtoul(argv[3], NULL, 16);
	}
	if (argc >= 5) {
		do_reset = simple_strtol(argv[4], NULL, 16);
	}

	printf("%s: Updating eNVM. Please wait ...\n", (char *) argv[0]);

	/*
	 * Copy the buffer to the destination.
	 */
	if ((ret = envm_write_and_reset(dst, src, size, do_reset)) <= 0) {
		printf("%s: envm_write_and_reset failed: %d\n",
						(char *) argv[0], ret);
		goto Done;
	}

	Done:
	return ret;
}

U_BOOT_CMD(
	cptf,	5,		0,	do_cptf,
	"copy memory buffer to internal Flash of Cortex-M3",
	"dst [[src] [[size] [do_reset]]]"
);
