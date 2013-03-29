/*
 * Copyright (C) 2009 Freescale Semiconductor, Inc.
 *
 * Author: Mingkai Hu (Mingkai.hu@freescale.com)
 * Based on stmicro.c by Wolfgang Denk (wd@denx.de),
 * TsiChung Liew (Tsi-Chung.Liew@freescale.com),
 * and  Jason McMullan (mcmullan@netapp.com)
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <malloc.h>
#include <spi_flash.h>

#include "spi_flash_internal.h"

/* S25FLxx-specific commands */
#define CMD_S25FLXX_READ	0x03	/* Read Data Bytes */
#define CMD_S25FLXX_FAST_READ	0x0b	/* Read Data Bytes at Higher Speed */
#define CMD_S25FLXX_READID	0x90	/* Read Manufacture ID and Device ID */
#define CMD_S25FLXX_WREN	0x06	/* Write Enable */
#define CMD_S25FLXX_WRDI	0x04	/* Write Disable */
#define CMD_S25FLXX_RDSR	0x05	/* Read Status Register */
#define CMD_S25FLXX_WRSR	0x01	/* Write Status Register */
#define CMD_S25FLXX_PP		0x02	/* Page Program */
#define CMD_S25FLXX_PE		0x20	/* Parameter 4-kB Sector Erase */
#define CMD_S25FLXX_SE		0xd8	/* Sector Erase */
#define CMD_S25FLXX_BE		0xc7	/* Bulk Erase */
#define CMD_S25FLXX_DP		0xb9	/* Deep Power-down */
#define CMD_S25FLXX_RES		0xab	/* Release from DP, and Read Signature */

#define SPSN_ID_S25FL008A	0x0213
#define SPSN_ID_S25FL016A	0x0214
#define SPSN_ID_S25FL032A	0x0215
#define SPSN_ID_S25FL064A	0x0216
#define SPSN_ID_S25FL064P	0x0216
#define SPSN_ID_S25FL128	0x2018
#define SPSN_EXT_ID_S25FL128P_256KB	0x0300
#define SPSN_EXT_ID_S25FL128P_64KB	0x0301
#define SPSN_EXT_ID_S25FL128S_64KB	0x4D01
#define SPSN_EXT_ID_S25FL064P	0x4D00

#define SPANSION_SR_WIP		(1 << 0)	/* Write-in-Progress */

/*
 * Allow hybrid geometries with up to the following number of sectors
 * with different sizes
 */
#define DIF_SEC_SIZE_NUM	2

struct spansion_spi_flash_params {
	/*
	 * User defined configuration
	 */
	u16 idcode1;
	u16 idcode2;
	u16 page_size;
	u16 pages_per_sector[DIF_SEC_SIZE_NUM];
	u16 nr_sectors[DIF_SEC_SIZE_NUM];
	const char *name;

	/*
	 * Calculated at initialization
	 */
	u32 end[DIF_SEC_SIZE_NUM];
	u8  cmd_pe;
};

struct spansion_spi_flash {
	struct spi_flash flash;
	struct spansion_spi_flash_params *params;
};

static inline struct spansion_spi_flash *to_spansion_spi_flash(struct spi_flash
							     *flash)
{
	return container_of(flash, struct spansion_spi_flash, flash);
}

static struct spansion_spi_flash_params spansion_spi_flash_table[] = {
	{
		.idcode1 = SPSN_ID_S25FL008A,
		.idcode2 = 0,
		.page_size = 256,
		.pages_per_sector[0] = 256,
		.nr_sectors[0] = 16,
		.name = "S25FL008A",
	},
	{
		.idcode1 = SPSN_ID_S25FL016A,
		.idcode2 = 0,
		.page_size = 256,
		.pages_per_sector[0] = 256,
		.nr_sectors[0] = 32,
		.name = "S25FL016A",
	},
	{
		.idcode1 = SPSN_ID_S25FL032A,
		.idcode2 = 0,
		.page_size = 256,
		.pages_per_sector[0] = 256,
		.nr_sectors[0] = 64,
		.name = "S25FL032A",
	},
	{
		.idcode1 = SPSN_ID_S25FL064A,
		.idcode2 = 0,
		.page_size = 256,
		.pages_per_sector[0] = 256,
		.nr_sectors[0] = 128,
		.name = "S25FL064A",
	},
	{
		.idcode1 = SPSN_ID_S25FL064P,
		.idcode2 = SPSN_EXT_ID_S25FL064P,
		.page_size = 256,
		.pages_per_sector[0] = 256,
		.nr_sectors[0] = 128,
		.name = "S25FL064P",
	},
	{
		.idcode1 = SPSN_ID_S25FL128,
		.idcode2 = SPSN_EXT_ID_S25FL128P_64KB,
		.page_size = 256,
		.pages_per_sector[0] = 256,
		.nr_sectors[0] = 256,
		.name = "S25FL128P_64K",
	},
	{
		.idcode1 = SPSN_ID_S25FL128,
		.idcode2 = SPSN_EXT_ID_S25FL128P_256KB,
		.page_size = 256,
		.pages_per_sector[0] = 1024,
		.nr_sectors[0] = 64,
		.name = "S25FL128P_256K",
	},
	{
		.idcode1 = SPSN_ID_S25FL128,
		.idcode2 = SPSN_EXT_ID_S25FL128S_64KB,
		.page_size = 256,
		.pages_per_sector[0] = 16,
		.nr_sectors[0] = 32,
		.pages_per_sector[1] = 256,
		.nr_sectors[1] = 254,
		.name = "S25FL128S_64K",
	},
};

/*
 * Get size of the sector (in bytes), which covers the address specified
 */
static u32 spansion_sec_size(struct spansion_spi_flash *spsn, u32 addr)
{
	struct spansion_spi_flash_params	*p = spsn->params;
	u32					size = 0, i;

	for (i = 0; i < DIF_SEC_SIZE_NUM; i++) {
		if (!(addr < p->end[i]))
			continue;

		size = p->page_size * p->pages_per_sector[i];
		break;
	}

	return size;
}

static int spansion_wait_ready(struct spi_flash *flash, unsigned long timeout)
{
	struct spi_slave *spi = flash->spi;
	unsigned long timebase;
	int ret;
	u8 status;

	timebase = get_timer(0);
	do {
		ret = spi_flash_cmd(spi, CMD_S25FLXX_RDSR, &status, sizeof(status));
		if (ret)
			return -1;

		if ((status & SPANSION_SR_WIP) == 0)
			break;

	} while (get_timer(timebase) < timeout);


	if ((status & SPANSION_SR_WIP) == 0)
		return 0;

	/* Timed out */
	return -1;
}

static int spansion_read_fast(struct spi_flash *flash,
			     u32 offset, size_t len, void *buf)
{
	struct spansion_spi_flash *spsn = to_spansion_spi_flash(flash);
	unsigned long page_addr;
	unsigned long page_size;
	u8 cmd[5];

	page_size = spsn->params->page_size;
	page_addr = offset / page_size;

	cmd[0] = CMD_READ_ARRAY_FAST;
	cmd[1] = page_addr >> 8;
	cmd[2] = page_addr;
	cmd[3] = offset % page_size;
	cmd[4] = 0x00;

	debug
		("READ: 0x%x => cmd = { 0x%02x 0x%02x%02x%02x%02x } len = 0x%x\n",
		 offset, cmd[0], cmd[1], cmd[2], cmd[3], cmd[4], len);

	return spi_flash_read_common(flash, cmd, sizeof(cmd), buf, len);
}

static int spansion_write(struct spi_flash *flash,
			 u32 offset, size_t len, const void *buf)
{
	struct spansion_spi_flash *spsn = to_spansion_spi_flash(flash);
	unsigned long page_addr;
	unsigned long byte_addr;
	unsigned long page_size;
	size_t chunk_len;
	size_t actual;
	int ret;
	u8 cmd[4];

	page_size = spsn->params->page_size;
	page_addr = offset / page_size;
	byte_addr = offset % page_size;

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		debug("SF: Unable to claim SPI bus\n");
		return ret;
	}

	ret = 0;
	for (actual = 0; actual < len; actual += chunk_len) {
		chunk_len = min(len - actual, page_size - byte_addr);

		cmd[0] = CMD_S25FLXX_PP;
		cmd[1] = page_addr >> 8;
		cmd[2] = page_addr;
		cmd[3] = byte_addr;

		debug
		    ("PP: 0x%p => cmd = { 0x%02x 0x%02x%02x%02x } chunk_len = %d\n",
		     buf + actual, cmd[0], cmd[1], cmd[2], cmd[3], chunk_len);

		ret = spi_flash_cmd(flash->spi, CMD_S25FLXX_WREN, NULL, 0);
		if (ret < 0) {
			debug("SF: Enabling Write failed\n");
			break;
		}

		ret = spi_flash_cmd_write(flash->spi, cmd, 4,
					  buf + actual, chunk_len);
		if (ret < 0) {
			debug("SF: SPANSION Page Program failed\n");
			break;
		}

		ret = spansion_wait_ready(flash, SPI_FLASH_PROG_TIMEOUT);
		if (ret < 0) {
			debug("SF: SPANSION page programming timed out\n");
			break;
		}

		page_addr++;
		byte_addr = 0;
	}

	debug("SF: SPANSION: Successfully programmed %u bytes @ 0x%x\n",
	      len, offset);

	spi_release_bus(flash->spi);
	return ret;
}

int spansion_erase(struct spi_flash *flash, u32 offset, size_t len)
{
	struct spansion_spi_flash *spsn = to_spansion_spi_flash(flash);
	unsigned long sector_size, start, end, pos;
	int ret;
	u8 cmd[4];

	/*
	 * This function currently uses sector erase only.
	 * probably speed things up by using bulk erase
	 * when possible.
	 */

	/*
	 * Fit [offset; offset + len] into the sector aligned
	 * boundaries [start; end]
	 */
	sector_size = spansion_sec_size(spsn, offset);
	start = offset & ~(sector_size - 1);

	sector_size = spansion_sec_size(spsn, offset + len);
	end = (offset + len + sector_size - 1) & ~(sector_size - 1);

	/*
	 * Don't check alignments, just warns instead
	 */
	if (start != offset || end != (offset + len)) {
		debug("SF: Warn, auto-align erase area [%x;%x] -> [%lx;%lx]\n",
			offset, offset + len, start, end);
	}

	ret = spi_claim_bus(flash->spi);
	if (ret) {
		debug("SF: Unable to claim SPI bus\n");
		return ret;
	}

	ret = 0;
	pos = start;
	do {
		sector_size = spansion_sec_size(spsn, pos);

		cmd[0] = (sector_size == 4096) ? spsn->params->cmd_pe :
						 CMD_S25FLXX_SE;
		cmd[1] = pos >> 16;
		cmd[2] = pos >>  8;
		cmd[3] = pos >>  0;

		ret = spi_flash_cmd(flash->spi, CMD_S25FLXX_WREN, NULL, 0);
		if (ret < 0) {
			debug("SF: Enabling Write failed\n");
			break;
		}

		ret = spi_flash_cmd_write(flash->spi, cmd, 4, NULL, 0);
		if (ret < 0) {
			debug("SF: SPANSION page erase failed\n");
			break;
		}

		/* Up to 2 seconds */
		ret = spansion_wait_ready(flash, SPI_FLASH_PAGE_ERASE_TIMEOUT);
		if (ret < 0) {
			debug("SF: SPANSION page erase timed out\n");
			break;
		}

		pos += sector_size;
	} while (pos < end);

	if (ret == 0) {
		debug("SF: SPANSION: Successfully erased %lu bytes @ 0x%lx\n",
		      pos, start);
	}

	spi_release_bus(flash->spi);
	return ret;
}

struct spi_flash *spi_flash_probe_spansion(struct spi_slave *spi, u8 *idcode)
{
	struct spansion_spi_flash_params *params;
	struct spansion_spi_flash *spsn;
	unsigned int i, size;
	unsigned short jedec, ext_jedec;

	jedec = idcode[1] << 8 | idcode[2];
	ext_jedec = idcode[3] << 8 | idcode[4];

	for (i = 0; i < ARRAY_SIZE(spansion_spi_flash_table); i++) {
		params = &spansion_spi_flash_table[i];
		if (params->idcode1 == jedec) {
			if (params->idcode2 == ext_jedec)
				break;
		}
	}

	if (i == ARRAY_SIZE(spansion_spi_flash_table)) {
		debug("SF: Unsupported SPANSION ID %04x %04x\n", jedec, ext_jedec);
		return NULL;
	}

	spsn = malloc(sizeof(struct spansion_spi_flash));
	if (!spsn) {
		debug("SF: Failed to allocate memory\n");
		return NULL;
	}

	for (i = 0, size = 0; i < DIF_SEC_SIZE_NUM; i++) {
		size += params->page_size * params->pages_per_sector[i] *
			params->nr_sectors[i];
		params->end[i] = size;
	}

	if (params->idcode1 == SPSN_ID_S25FL128 &&
	    params->idcode2 == SPSN_EXT_ID_S25FL128S_64KB) {
		/*
		 * Use special cmd for 4kB sector erase
		 */
		params->cmd_pe = CMD_S25FLXX_PE;
	} else {
		/*
		 * Use common cmd for 4kB sector erase
		 */
		params->cmd_pe = CMD_S25FLXX_SE;
	}

	spsn->params = params;
	spsn->flash.spi = spi;
	spsn->flash.name = params->name;

	spsn->flash.write = spansion_write;
	spsn->flash.erase = spansion_erase;
	spsn->flash.read = spansion_read_fast;
	spsn->flash.size = size;

	debug("SF: Detected %s with page size %u, total %u bytes\n",
	      params->name, params->page_size, spsn->flash.size);

	return &spsn->flash;
}
