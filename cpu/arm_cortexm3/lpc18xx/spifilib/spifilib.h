/*
 * (C) Copyright 2013
 *
 * Dmitry Konyshev, Emcraft Systems, probables@emcraft.com
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

#ifndef __SPIFILIB_H
#define __SPIFILIB_H

#include "spifi_rom_api.h"
#include <config.h>

#define SPIFILIB_SIG	0x591F121B
#define SPIFILIB_SIZE	(16*1024)

#ifdef CONFIG_SPIFILIB_IN_ENVM
# define SPIFI_BASE_ADDR CONFIG_MEM_NVM_BASE
#else
# define SPIFI_BASE_ADDR 0x14000000
#endif

typedef int32_t (*spifi_init_t)(SPIFIobj *obj, uint32_t csHigh, uint32_t options, uint32_t mhz);
typedef int32_t (*spifi_program_t)(SPIFIobj *obj, char *source, SPIFIopers *opers);
typedef int32_t (*spifi_erase_t)(SPIFIobj *obj, SPIFIopers *opers);
typedef int32_t (*spifi_cancel_mem_mode_t)(SPIFIobj *obj);

typedef struct spifilib_header_s
{
	unsigned long signature;
	void *link_addr;
	unsigned long lib_size;
	spifi_init_t init_func;
	spifi_program_t program_func;
	spifi_erase_t erase_func;
	spifi_cancel_mem_mode_t cancel_mem_mode_func;
} spifilib_header_t;

const spifilib_header_t *spifilib_flash_hdr = (spifilib_header_t *)(SPIFI_BASE_ADDR + 112 * 1024);

#endif
