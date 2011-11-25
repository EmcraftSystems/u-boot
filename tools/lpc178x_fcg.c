/*
 * Copyright (C) 2011 by NXP Semiconductors
 * All rights reserved.
 *
 * @Author: Kevin Wells
 * @Descr: Inserts checksum into FLASH image for LPC17xx devices
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * This LPC1788 requires a checksum of the first 7 words in FLASH to boot
 * a user program. All this program does is compute the 2's complement
 * checksum on the first 7 32-bit words and inserts it into the 8th 32-bit
 * word - overwriting what is there.
 *
 * Note that some LPC1788 FLASH burners may already generate the checksum
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Works for LPC1787and LPC1788, but adjust for other parts
 */
#define MAX_FLASH_SIZE (512 * 1024)

int main(int argc, char *argv[])
{
	FILE *fp = NULL;
	char *filename, *nfilename;
	unsigned int cksum, *buff;
	size_t fsize;
	int i, ret = 0;

	if (argc != 3)
	{
		printf("Usage: %s infile outfile\n", argv[0]);
		return 1;
	}

	/* Enough for a 512K image to burn */
	buff = (unsigned int *) malloc(MAX_FLASH_SIZE);
	if (!buff) {
		printf("Error allocating memory for buffer\n");
		return 1;
	}

	filename = (char *) argv[1];
	fp = fopen((const char *) filename, "r");
	if (!fp) {
		printf("Error opening file %s\n", filename);
		ret = 1;
		goto free_buff;
	}

	/* Load file and save file size */
	fsize = fread((void *) buff, 1, MAX_FLASH_SIZE, fp);

	if (ferror(fp)) {
		printf("Error loading file %s\n", filename);
		ret = 1;
		goto close_file;
	}


	if (!feof(fp)) {
		printf("File %s is too large. The maximum size is "
			"limited by FLASH (%d bytes)\n", filename,
			MAX_FLASH_SIZE);
		ret = 1;
		goto close_file;
	}

	printf("File %s loaded with size %d bytes\n", filename, (int) fsize);

	fclose(fp);

	nfilename = (char *) argv[2];

	/* Open new file for writing */
	fp = fopen(nfilename, "w");
	if (!fp) {
		printf("Error creating new file %s\n", nfilename);
		ret = 1;
		goto free_buff;
	}

	/* Compute 2's complement checksum */
	cksum = 0;
	for (i = 0; i < 7; i++) {
		cksum += buff[i];
		printf("Word %d: 0x%08x\n", i, buff[i]);
	}

	buff[7] = 0xFFFFFFFF - cksum + 1;
	printf("Word 7: 0x%08x (cksum total: 0x%08x)\n", buff[7],
		(cksum + buff[7]));

	/* Write data back to file */
	if (fwrite(buff, 1, fsize, fp) != fsize) {
		printf("Error writing new file %s\n", nfilename);
		ret = 1;
		goto close_file;
	}
	printf("File %s created with size %d bytes\n", nfilename,
		(int) fsize);

close_file:
	fclose(fp);

free_buff:
	free(buff);

	return ret;
}

