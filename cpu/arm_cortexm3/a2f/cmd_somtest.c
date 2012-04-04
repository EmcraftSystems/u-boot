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

#define COREGPIO_BASE0		0x40050200
#define COREGPIO_BASE1		0x40050400
#define COREGPIO_BASE2		0x40050500

struct coregpio {
	int config[32];
	int intclr;
	int res0[3];
	int in;
	int res1[3];
	int out;
};

#define CGPIO0			((volatile struct coregpio *)COREGPIO_BASE0)
#define CGPIO1			((volatile struct coregpio *)COREGPIO_BASE1)
#define CGPIO2			((volatile struct coregpio *)COREGPIO_BASE2)
/* Number of pins to test */
#define TESTPINS		20

static int test_gpio (volatile struct coregpio *gin,
		volatile struct coregpio *gout, int testval)
{
	int i;
	int ret = -1;

	/* Reset all bits in config registers. This will disable all
	   pin functionality.
	*/
	for (i = 0; i < 32; i++) {
		gin->config[i] = 0;
		gout->config[i] = 0;
	}

	/* Configure gpio0 as output, gpio1 as input */
	for (i = 0; i < TESTPINS; i++) {
		gin->config[i] |= 2; /* Input first */
		gout->config[i] |= 5;

		if (!(gin->config[i] & 2) || !(gout->config[i] & 5)) {
			printf("FAIL: Config bits don't set!\n");
			goto xit;
		}
	}
	printf("  Writing test pattern %#x\n", testval);
	gout->out = testval;
	if (gout->out != testval) {
		printf("FAIL, bad value written: %#x (should be %#x)\n",
			gout->out, testval);
		goto xit;
	}
	printf("  Reading test pattern\n");
	if (gin->in != testval) {
		printf("FAIL, bad value read: %#x (should be %#x)\n",
			gin->in, testval);
		goto xit;
	}

	ret = 0;
xit:
	return ret;

}

static int somtest_gpio(void)
{
	int ret = -1;
	int testval1 = 0x55555;
	int testval2 = 0xAAAAA;

	printf("Loopback test SOM CoreGPIO_0 and 1, both directions...\n");

	if (!test_gpio(CGPIO1, CGPIO0, testval1) &&
		!test_gpio(CGPIO1, CGPIO0, testval2) &&
		!test_gpio(CGPIO0, CGPIO1, testval1) &&
		!test_gpio(CGPIO0, CGPIO1, testval2)) {
		printf("PASS\n");
		ret = 0;
	}
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
