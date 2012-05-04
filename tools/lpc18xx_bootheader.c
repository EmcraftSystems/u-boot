/*
 * (C) Copyright 2012
 * Emcraft Systems, <www.emcraft.com>
 * Alexander Potashev <aspotashev@emcraft.com>
 *
 * Header generator for the NXP LPC18xx/43xx Boot ROM bootloader.
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

/*
 * The header format is described in section "5.3.3 Boot image format"
 * in the LPC43xx User Manual.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	long f_size;	/* Input file size */
	long blocks;	/* Number of 512-byte blocks in image */
	long out_size;	/* Image data size round up to 512-byte blocks */
	FILE *in, *out;
	unsigned char header[16];
	unsigned char *buf;

	if (argc != 3) {
		printf("Usage:   lpc18xx_bootheader [in-file] [out-file]\n");
		return 1;
	}

	/* Get file size */
	in = fopen(argv[1], "rb");
	if (!in) {
		printf("Cannot open input file: %s\n", argv[1]);
		return 2;
	}
	fseek(in, 0, SEEK_END);
	f_size = ftell(in);
	rewind(in);

	/* Round up to an integer number of 512-byte blocks */
	blocks = (f_size + 511) / 512;
	out_size = blocks * 512;

	/* Number of blocks in limited to 2**16 = 65536 = 32 MBytes */
	if (blocks >> 16) {
		printf("Number of blocks is too large: %ld\n", blocks);
		return 3;
	}

	/* Read image data */
	buf = malloc(out_size);
	if (!buf) {
		printf("Could not allocate %ld bytes of memory\n", f_size);
		return 4;
	}
	if (fread(buf, f_size, 1, in) != 1) {
		printf("Could not read file contents: %s\n", argv[1]);
		return 5;
	}
	fclose(in);

	/* Zero the padding bytes after actual image data ends */
	memset(buf + f_size, 0, out_size - f_size);

	/* Generate header */
	memset(header, 0, sizeof(header));
	header[0] = 0x1A;	/* AES encryption not active */
	header[0] |= (3 << 6);	/* no hash is used */
	header[1] = 0x3F;	/* Reserved, must be 6 binary 1s */
	header[2] = blocks & 0xFF;		/* Little endian: LSB */
	header[3] = (blocks >> 8) & 0xFF;	/* Little endian: MSB */
	header[12] = 0xFF;	/* Reserved */
	header[13] = 0xFF;	/* Reserved */
	header[14] = 0xFF;	/* Reserved */
	header[15] = 0xFF;	/* Reserved */

	/* Write output file */
	out = fopen(argv[2], "wb");
	if (!out) {
		printf("Cannot open output file: %s\n", argv[2]);
		return 6;
	}
	if (fwrite(header, sizeof(header), 1, out) != 1) {
		printf("Could not write header to file: %s\n", argv[2]);
		return 7;
	}
	if (fwrite(buf, out_size, 1, out) != 1) {
		printf("Could not write image data to file: %s\n", argv[2]);
		return 8;
	}
	fclose(out);

	return 0;
}
