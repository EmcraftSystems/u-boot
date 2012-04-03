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

static int somtest_gpio(void)
{
	int i, j;
	int ret = 0;

	printf("SOM test: gpio: starting ...\n");

	/*
 	 * This is some sample code that shows how to access
	 * a hardware register from the U-boot context.
 	 * ...
 	 * TBD - this code needs to be updated to implement
 	 * a desired GPIO test for the SmartFision SOM.
 	 */
	for (i = 1; i <= 10; i++) {
		/*
		 * This toggles the LED on A2F-LNX-EVB
		 * The LED is turned on when i is an odd integer,
		 * off - when i is an even integer.
		 */
		* (volatile unsigned int *) 0x400502a0 = i % 2;

		/*
 		 * This sleeps a second
 		 */
		for (j = 0; j < 1000; j++) udelay(1000);
	}

	printf("SOM test: gpio: %s\n", !ret ? "PASS" : "FAIL");

	return ret;
}

 /*
  * do_somtest: TBD
  */
int do_somtest(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 1;

	/*
	 * Check that at least the destination is specified
	 */
	if (argc == 1) {
		printf("%s: command must be specified\n", (char *) argv[0]);
		goto Done;
	}

	/*
	 * Parse the command arguments
	 */
	if (! strcmp(argv[1], "gpio")) {
		ret = somtest_gpio();
		goto Done;
	}
	printf("%s: unrecognized command %s\n",
		(char *) argv[0], (char *) argv[1]);

	Done:
	return ret;
}

U_BOOT_CMD(
	somtest,	3,		0,	do_somtest,
	"SmartFusion SOM test suite",
	"[gpio]"
);
