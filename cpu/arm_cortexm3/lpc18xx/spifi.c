/*
 * (C) Copyright 2013
 *
 * Dmitry Konyshev, Emcraft Systems, probables@emcraft.com
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
#include "spifilib/spifilib.h"

static SPIFIobj spifi_obj;
const spifilib_header_t *spifilib_ram_hdr = NULL;

int spifi_initialize(void)
{
	int ret;

	if (spifilib_flash_hdr->signature == SPIFILIB_SIG) {
		/*
		 * Already initalized, skip
		 */
		if (spifilib_ram_hdr != NULL)
			return 0;

#ifndef CONFIG_SPIFILIB_IN_ENVM
		spifilib_ram_hdr = spifilib_flash_hdr->link_addr;
		memcpy(spifilib_flash_hdr->link_addr, spifilib_flash_hdr,
				spifilib_flash_hdr->lib_size);
#else
		spifilib_ram_hdr = spifilib_flash_hdr;
#endif
		if ((ret = spifilib_ram_hdr->init_func(&spifi_obj, 10, S_MODE3, 25))) {
			spifilib_ram_hdr = NULL;
			printf("SPIFI lib init failed with code %i\n", ret);
			return 1;
		}
	} else {
		puts("No SPIFI lib found\n");
		return 1;
	}

	return 0;
}

int spifi_write(ulong offset, const void *buf, ulong len)
{
	SPIFIopers ops = {
		.dest = offset < CONFIG_SPIFI_BASE ? (char *)offset :
				(char *)offset - CONFIG_SPIFI_BASE,
		.length = len,
		.scratch = NULL,
		.protect = 0,
		.options = 0,
	};

	if (!spifilib_ram_hdr) {
		puts("SPIFI lib is not initialized!\n");
		return 1;
	}

	return spifilib_ram_hdr->program_func(&spifi_obj, (char *)buf, &ops);
}

void spifi_cancel_mem_mode(void)
{
	if (!spifilib_ram_hdr) {
		puts("SPIFI lib is not initialized!\n");
		return;
	}
	spifilib_ram_hdr->cancel_mem_mode_func(&spifi_obj);
}

