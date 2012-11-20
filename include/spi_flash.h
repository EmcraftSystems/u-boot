/*
 * Interface to SPI flash
 *
 * Copyright (C) 2008 Atmel Corporation
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
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
#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_

#include <spi.h>
#include <linux/types.h>

struct spi_flash_region {
	unsigned int	count;
	unsigned int	size;
};

struct spi_flash {
	struct spi_slave *spi;

	const char	*name;

	u32		size;

	int		(*read)(struct spi_flash *flash, u32 offset,
				size_t len, void *buf);
	int		(*write)(struct spi_flash *flash, u32 offset,
				size_t len, const void *buf);
	int		(*erase)(struct spi_flash *flash, u32 offset,
				size_t len);
};

struct spi_flash *spi_flash_probe(unsigned int bus, unsigned int cs,
		unsigned int max_hz, unsigned int spi_mode);
void spi_flash_free(struct spi_flash *flash);

static inline int spi_flash_read(struct spi_flash *flash, u32 offset,
		size_t len, void *buf)
{
#if defined(CONFIG_SPI_MAX_XF_LEN)
	u32	adr = offset, pos = 0, sz;
	int	rv;

	do {
		sz = len < CONFIG_SPI_MAX_XF_LEN ? len : CONFIG_SPI_MAX_XF_LEN;
		rv = flash->read(flash, adr, sz, &((u8 *)buf)[pos]);
		if (rv != 0)
			break;
		len -= sz;
		pos += sz;
		adr += sz;
	} while (len);

	return rv;
#else
	return flash->read(flash, offset, len, buf);
#endif
}

static inline int spi_flash_write(struct spi_flash *flash, u32 offset,
		size_t len, const void *buf)
{
#if defined(CONFIG_SPI_MAX_XF_LEN)
	u32	adr = offset, pos = 0, sz;
	int	rv;

	do {
		sz = len < CONFIG_SPI_MAX_XF_LEN ? len : CONFIG_SPI_MAX_XF_LEN;
		rv = flash->write(flash, adr, sz, &((const u8 *)buf)[pos]);
		if (rv != 0)
			break;
		len -= sz;
		pos += sz;
		adr += sz;
	} while (len);

	return rv;
#else
	return flash->write(flash, offset, len, buf);
#endif
}

static inline int spi_flash_erase(struct spi_flash *flash, u32 offset,
		size_t len)
{
	return flash->erase(flash, offset, len);
}

#endif /* _SPI_FLASH_H_ */
