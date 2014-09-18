/*
 * (C) Copyright 2013
 * Pavel Boldin, Emcraft Systems, paboldin@emcraft.com
 *
 * This is command to do a safe copy from NOR flash into SDRAM through
 * SRAM buffer to workaround errata 2.8.7
 * (SDRAM and NOR flash can't be accessed simultaneously).
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
#include <asm/arch/fmc.h>
#include <asm/errno.h>

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
#define SOC_RAM_BUFFER_SIZE	(ulong)((&_mem_ram_buf_size) - 0x100)


int nor_sdram_bufcopy_onebuf(ulong dst, ulong src, ulong size)
{
	int ret = 0;

	if (size > SOC_RAM_BUFFER_SIZE) {
		ret = -ENOMEM;
		goto out;
	}

	/*
	 * Switch memory to self-refresh mode.
	 * Controller issues PALL command automatically before that.
	 */
	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_SELFREFRESH;

	/* Wait until Self-Refresh mode is enabled */
	FMC_BUSY_WAIT();

	memcpy((void*)SOC_RAM_BUFFER_BASE, (void*)src, size);

	/*
	 * Precharge according to chip requirement, page 12.
	 */
	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_PRECHARGE;

	FMC_BUSY_WAIT();

	STM32_SDRAM_FMC->sdcmr = FMC_SDCMR_BANK_1 | FMC_SDCMR_MODE_NORMAL;

	/* tRFC delay */
	udelay(60);

	/* Are you still busy? */
	FMC_BUSY_WAIT();

	memcpy((void*)dst, (void*)SOC_RAM_BUFFER_BASE, size);

	ret = size;

out:
	return ret;
}

/*
 * Read from NOR and write into SDRAM
 * See errata 2.8.7.
 */
int nor_sdram_bufcopy(ulong dst, ulong src, ulong size)
{
	int ret = 0;
	ulong copysize = 0;

	while(size) {
		copysize = min(size, SOC_RAM_BUFFER_SIZE);

		if ((ret = nor_sdram_bufcopy_onebuf(dst, src, copysize)) <= 0)
			goto done;

		dst += min(size, SOC_RAM_BUFFER_SIZE);
		src += min(size, SOC_RAM_BUFFER_SIZE);
		size -= min(size, SOC_RAM_BUFFER_SIZE);
	}

done:
	return ret;
}

/*
 * bufcopy: copy content of a NOR flash into SDRAM through SRAM buffer
 */
int do_bufcopy(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong dst;
	ulong src;
	ulong size;
	int ret = 0;

	/*
	 * Check that at least the destination is specified
	 */
	if (argc != 4) {
		printf("%s: dst src size\n", (char *) argv[0]);
		goto Done;
	}

	/*
	 * Parse the command arguments
	 */
	dst = simple_strtoul(argv[1], NULL, 16);
	src = simple_strtoul(argv[2], NULL, 16);
	size = simple_strtoul(argv[3], NULL, 16);

	if (dst < CONFIG_SYS_RAM_BASE) {
		printf("%s: dst is outside SDRAM\n", (char *) argv[0]);
		goto Done;
	}

	/*
	 * Copy the buffer to the destination.
	 */
	if ((ret = nor_sdram_bufcopy(dst, src, size)) < 0) {
		printf("%s: nor_sdram_bufcopy failed: %d\n",
						(char *) argv[0], ret);
		goto Done;
	}

Done:
	return ret;
}

U_BOOT_CMD(
	stmbufcopy,	5,		0,	do_bufcopy,
	"copy from NOR flash into SDRAM through SRAM buf",
	"dst src size"
);
