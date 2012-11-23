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
#include <command.h>
#include <string.h>
#include "envm.h"

extern void m2s_spi_test(unsigned int bus, unsigned int cmd);

 /*
  * Run SPI test
  */
int do_spitest(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	unsigned int bus;
	unsigned int cmd;
	int ret = 0;

	/*
	 * Check that the call has the right # of parameters
	 */
	if (argc < 3) {
		printf("%s: bus number and command must be specified\n",
			(char *) argv[0]);
		goto done;
	}

	/*
	 * Parse and validate command arguments
	 */
	bus = simple_strtoul(argv[1], NULL, 16);
	if (bus != 0 && bus != 1 && bus != 2) {
		printf("%s: bus number must be 0, 1 or 2\n", (char *) argv[0]);
		goto done;
	}
	cmd = simple_strtoul(argv[2], NULL, 16);

	/*
	 * Call the SPI driver in order to run actual test
	 */
	m2s_spi_test(bus, cmd);

done:
	return ret;
}

U_BOOT_CMD(
	spitest, 5, 0, do_spitest,
	"Run SPI test",
	"bus"
);
