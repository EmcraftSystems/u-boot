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
#define MSSGPIO_BASE		0x40013000

struct coregpio {
	int config[32];
	int intclr;
	int res0[3];
	int in;
	int res1[3];
	int out;
};

struct mssgpio {
	int config[32];
	int intstat;
	int in;
	int out;
};

#define CGPIO0			((volatile struct coregpio *)COREGPIO_BASE0)
#define CGPIO1			((volatile struct coregpio *)COREGPIO_BASE1)
#define CGPIO2			((volatile struct coregpio *)COREGPIO_BASE2)
#define MSSGPIO			((volatile struct mssgpio *)MSSGPIO_BASE)

#define IN			0
#define OUT			1

static void clear_gpio(volatile int *cfg)
{
	int i;
	/* Turn off all pins */
	for (i = 0; i < 32; i ++, cfg++) {
		*cfg = 0;
	}
}

static void clear_all_gpio(void)
{
	clear_gpio(&MSSGPIO->config[0]);
	clear_gpio(&CGPIO0->config[0]);
	clear_gpio(&CGPIO1->config[0]);
	clear_gpio(&CGPIO2->config[0]);
}

static int test_gpio (volatile int *gin, volatile int *gout,
		int testval, int startpin, int npins)
{
	int i, val;
	int fail = 0;
	int ret = -1;

	*gout = (testval << startpin);
	if (*gout != (testval << startpin)) {
		printf("FAIL, bad value written: %#x (should be %#x)\n",
			*gout, (testval << startpin));
		goto xit;
	}

	udelay(1);/* tbd */

	val = *gin;
	if (val != (testval << startpin)) {
		printf("FAIL, bad value read: %#x (should be %#x)\n",
			val, (testval << startpin));
		goto xit;
	}

	printf("  Walking 1 test on pins %d - %d...\n", startpin,
		startpin+npins-1);
	for (i = startpin; i < startpin+npins; i++) {
		*gout = (1 << i);
		if (*gout != (1 << i)) {
			printf("FAIL pin %d, bad value written: %#x (should be %#x)\n",
				i, *gout, (1 << i));
			goto xit;
		}
		udelay(1);/* tbd */
		val = *gin;
		if (val != (1 << i)) {
			printf("FAIL pin %d, bad value read: %#x (should be %#x)\n",
				i, val, (1 << i));
			fail = 1;
		}
	}
	if (!fail)
		ret = 0;
xit:
	return ret;

}

static void config_gpio(volatile int *cfg, int dir, int startpin, int npins)
{
	volatile int *config;
	int i;

	/* Configure gpio0 as output, gpio1 as input */
	for (i = startpin, config = cfg+startpin; i < startpin+npins; i++, config++) {
		if (dir == IN) {
			*config = 2;
			if ((*config != 2)) {
				printf("FAIL: Config bits don't set!\n");
				return;
			}
		} else {
			*config = 5;
			if ((*config != 5)) {
				printf("FAIL: Config bits don't set!\n");
				return;
			}

		}
	}

}

static int somtest_gpio1112(void)
{
	int ret = -1;
	int testval1 = 0x55555;
	int testval2 = 0xAAAAA;

	printf("Loopback test SOM CoreGPIO_0 and 1, pins 0-19...\n");
	clear_gpio(&CGPIO1->config[0]);
	config_gpio(&CGPIO1->config[0], IN, 0, 20);
	clear_gpio(&CGPIO0->config[0]);
	config_gpio(&CGPIO0->config[0], OUT, 0, 20);

	if (test_gpio(&CGPIO1->in, &CGPIO0->out, testval1, 0, 20))
		goto ret;

	clear_gpio(&CGPIO0->config[0]);
	config_gpio(&CGPIO0->config[0], IN, 0, 20);
	clear_gpio(&CGPIO1->config[0]);
	config_gpio(&CGPIO1->config[0], OUT, 0, 20);
	if (test_gpio(&CGPIO0->in, &CGPIO1->out, testval1, 0, 20))
		goto ret;

	clear_gpio(&CGPIO1->config[0]);
	config_gpio(&CGPIO1->config[0], IN, 0, 20);
	clear_gpio(&CGPIO0->config[0]);
	config_gpio(&CGPIO0->config[0], OUT, 0, 20);
	if (test_gpio(&CGPIO1->in, &CGPIO0->out, testval2, 0, 20))
		goto ret;

	clear_gpio(&CGPIO0->config[0]);
	config_gpio(&CGPIO0->config[0], IN, 0, 20);
	clear_gpio(&CGPIO1->config[0]);
	config_gpio(&CGPIO1->config[0], OUT, 0, 20);
	if (test_gpio(&CGPIO0->in, &CGPIO1->out, testval2, 0, 20))
		goto ret;

	ret = 0;
	printf("PASS\n");
ret:
	clear_all_gpio();
	return ret;
}

static int somtest_gpio1314(void)
{
	int ret = -1;
	int testval1 = 0x55;
	int testval2 = 0xAA;

	printf("Loopback test SOM CoreGPIO_0 and 1, pins 20-27...\n");

	clear_gpio(&CGPIO1->config[0]);
	config_gpio(&CGPIO1->config[0], IN, 20, 8);
	clear_gpio(&CGPIO0->config[0]);
	config_gpio(&CGPIO0->config[0], OUT, 20, 8);

	if (test_gpio(&CGPIO1->in, &CGPIO0->out, testval1, 20, 8))
		goto ret;

	clear_gpio(&CGPIO0->config[0]);
	config_gpio(&CGPIO0->config[0], IN, 20, 8);
	clear_gpio(&CGPIO1->config[0]);
	config_gpio(&CGPIO1->config[0], OUT, 20, 8);

	if (test_gpio(&CGPIO0->in, &CGPIO1->out, testval1, 20, 8))
		goto ret;

	clear_gpio(&CGPIO1->config[0]);
	config_gpio(&CGPIO1->config[0], IN, 20, 8);
	clear_gpio(&CGPIO0->config[0]);
	config_gpio(&CGPIO0->config[0], OUT, 20, 8);

	if (test_gpio(&CGPIO1->in, &CGPIO0->out, testval2, 20, 8))
		goto ret;

	clear_gpio(&CGPIO0->config[0]);
	config_gpio(&CGPIO0->config[0], IN, 20, 8);
	clear_gpio(&CGPIO1->config[0]);
	config_gpio(&CGPIO1->config[0], OUT, 20, 8);

	if (test_gpio(&CGPIO0->in, &CGPIO1->out, testval2, 20, 8))
		goto ret;

	ret = 0;
	printf("PASS\n");
ret:
	clear_all_gpio();
	return ret;
}


static int somtest_gpio1314_mss(void)
{
	int ret = -1;
	int testval1 = 0x555;
	int testval2 = 0xAAA;

	printf("Loopback test SOM CoreGPIO_2 and MSSGPIO, pins 4 - 15...\n");

	clear_gpio(&MSSGPIO->config[0]);
	config_gpio(&MSSGPIO->config[0], IN, 4, 12);
	clear_gpio(&CGPIO2->config[0]);
	config_gpio(&CGPIO2->config[0], OUT, 4, 12);

	if (test_gpio(&MSSGPIO->in, &CGPIO2->out, testval1, 4, 12))
		goto ret;

	clear_gpio(&CGPIO2->config[0]);
	config_gpio(&CGPIO2->config[0], IN, 4, 12);
	clear_gpio(&MSSGPIO->config[0]);
	config_gpio(&MSSGPIO->config[0], OUT, 4, 12);

	if (test_gpio(&CGPIO2->in, &MSSGPIO->out, testval1, 4, 12))
		goto ret;

	clear_gpio(&MSSGPIO->config[0]);
	config_gpio(&MSSGPIO->config[0], IN, 4, 12);
	clear_gpio(&CGPIO2->config[0]);
	config_gpio(&CGPIO2->config[0], OUT, 4, 12);

	if (test_gpio(&MSSGPIO->in, &CGPIO2->out, testval2, 4, 12))
		goto ret;

	clear_gpio(&CGPIO2->config[0]);
	config_gpio(&CGPIO2->config[0], IN, 4, 12);
	clear_gpio(&MSSGPIO->config[0]);
	config_gpio(&MSSGPIO->config[0], OUT, 4, 12);

	if (test_gpio(&CGPIO2->in, &MSSGPIO->out, testval2, 4, 12))
		goto ret;

	printf("PASS\n");
	ret = 0;

ret:
	clear_all_gpio();
	return ret;
}

static int test_gpio_910 (volatile int *gin, volatile int *gout,
			unsigned int testval, unsigned int expval)
{
	int ret = -1;
	int val;

	*gout = testval;
	if (*gout != testval) {
		printf("FAIL, bad value written: %#x (should be %#x)\n",
			*gout, testval);
		goto ret;
	}

	val = *gin;
	if (val != expval) {
		printf("FAIL, bad value read: %#x (should be %#x)\n",
			val, expval);
		goto ret;
	}
	ret = 0;
ret:
	return ret;
}


/*
 * Connection description for the test 4. P9[i] corresponds to the P10[i].
 */

struct gpiopin {
	volatile void *base;
	int pin;
};

struct gpiopin P9[] = {
	{MSSGPIO, 31},
	{MSSGPIO, 30},
	{MSSGPIO, 22},
	{MSSGPIO, 29},
	{MSSGPIO, 16},
	{MSSGPIO, 19},
	{MSSGPIO, 27},
	{MSSGPIO, 25},
	{MSSGPIO, 17},
	{MSSGPIO, 18},
	{MSSGPIO, 26},
	{MSSGPIO, 24},
	{MSSGPIO, 28},
	{MSSGPIO, 23}
};

struct gpiopin P10[] = {
	{MSSGPIO, 0},
	{MSSGPIO, 1},
	{MSSGPIO, 2},
	{MSSGPIO, 3},
	{CGPIO0, 28},
	{CGPIO0, 29},
	{CGPIO0, 30},
	{CGPIO0, 31},
	{CGPIO1, 28},
	{CGPIO1, 29},
	{CGPIO1, 30},
	{CGPIO1, 31},
	{CGPIO2, 2},
	{CGPIO2, 3}
};

static int test_gpio910_walk(struct gpiopin *in, struct gpiopin *out, int num)
{
	volatile int *gin, *gout;
	int i;
	int failed = 0;

	clear_all_gpio();
	for (i = 0; i < num; i++) {
		config_gpio((volatile int *)(in[i].base), IN, in[i].pin, 1);
		config_gpio((volatile int *)(out[i].base), OUT, out[i].pin, 1);
	}

	for (i = 0; i < num; i++) {
		if (in[i].base == MSSGPIO)
			gin = (volatile int *)&((volatile struct mssgpio *)(in[i].base))->in;
		else
			gin = (volatile int *)&((volatile struct coregpio *)(in[i].base))->in;

		if (out[i].base == MSSGPIO)
			gout = (volatile int *)&((volatile struct mssgpio *)(out[i].base))->out;
		else
			gout = (volatile int *)&((volatile struct coregpio *)(out[i].base))->out;

		if (test_gpio_910(gin, gout, 1 << out[i].pin, 1 << in[i].pin)) {
			failed = 1;
		}
		test_gpio_910(gin, gout, 0, 0); /* clean */

	}

	return failed;
}

static int somtest_gpio910(void)
{
	int ret = -1;

	clear_all_gpio();

	/* P9 IN */
	config_gpio(&MSSGPIO->config[0], IN, 16, 4);
	config_gpio(&MSSGPIO->config[0], IN, 22, 10);
	/* P10 OUT */
	config_gpio(&CGPIO0->config[0], OUT, 28, 4);
	config_gpio(&CGPIO1->config[0], OUT, 28, 4);
	config_gpio(&CGPIO2->config[0], OUT, 2, 2);
	config_gpio(&MSSGPIO->config[0], OUT, 0, 4);

	printf("Testing MSSGPIO 0-3, out...\n");
	if (test_gpio_910(&MSSGPIO->in, &MSSGPIO->out, 0x5, (0<<29|1<<22|0<<30|1<<31)))
		goto ret;
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	if (test_gpio_910(&MSSGPIO->in, &MSSGPIO->out, 0xa, (1<<29|0<<22|1<<30|0<<31)))
		goto ret;
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	test_gpio_910(&MSSGPIO->in, &MSSGPIO->out, 0, 0);

	printf("Testing CGPIO0 28-31, out...\n");
	if (test_gpio_910(&MSSGPIO->in, &CGPIO0->out, 0x5<<28, (0<<25|1<<27|0<<19|1<<16)))
		goto ret;
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	if (test_gpio_910(&MSSGPIO->in, &CGPIO0->out, 0xa<<28, (1<<25|0<<27|1<<19|0<<16)))
		goto ret;
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	test_gpio_910(&MSSGPIO->in, &CGPIO0->out, 0, 0);


	printf("Testing CGPIO1 28-31, out...\n");
	if (test_gpio_910(&MSSGPIO->in, &CGPIO1->out, 0x5<<28, (0<<24|1<<26|0<<18|1<<17)))
		goto ret;
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	if (test_gpio_910(&MSSGPIO->in, &CGPIO1->out, 0xa<<28, (1<<24|0<<26|1<<18|0<<17)))
		goto ret;
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	test_gpio_910(&MSSGPIO->in, &CGPIO1->out, 0, 0);

	printf("Testing CGPIO2 2-3, out...\n");
	if (test_gpio_910(&MSSGPIO->in, &CGPIO2->out, 1<<2, (0<<23|1<<28)))
		goto ret;
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	if (test_gpio_910(&MSSGPIO->in, &CGPIO2->out, 2<<2, (1<<23|0<<28)))
		goto ret;
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	test_gpio_910(&MSSGPIO->in, &CGPIO2->out, 0, 0);

	clear_all_gpio();

	/* P9 OUT */
	config_gpio(&MSSGPIO->config[0], OUT, 16, 4);
	config_gpio(&MSSGPIO->config[0], OUT, 22, 10);
	/* P9 IN */
	config_gpio(&CGPIO0->config[0], IN, 28, 4);
	config_gpio(&CGPIO1->config[0], IN, 28, 4);
	config_gpio(&CGPIO2->config[0], IN, 2, 2);
	config_gpio(&MSSGPIO->config[0], IN, 0, 4);

	printf("Testing MSSGPIO 0-3, in...\n");
	if (test_gpio_910(&MSSGPIO->in, &MSSGPIO->out, (0<<29|1<<22|0<<30|1<<31), 0x5))
		goto ret;
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	if (test_gpio_910(&MSSGPIO->in, &MSSGPIO->out, (1<<29|0<<22|1<<30|0<<31), 0xa))
		goto ret;
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	test_gpio_910(&MSSGPIO->in, &MSSGPIO->out, 0, 0);

	printf("Testing CGPIO0 28-31, in...\n");
	if (test_gpio_910(&CGPIO0->in, &MSSGPIO->out, (0<<25|1<<27|0<<19|1<<16), 0x5<<28))
		goto ret;
	if (MSSGPIO->in)
		printf("FAIL, unexpected value %#x in MSSGPIO\n", MSSGPIO->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	if (test_gpio_910(&CGPIO0->in, &MSSGPIO->out, (1<<25|0<<27|1<<19|0<<16), 0xa<<28))
		goto ret;
	if (MSSGPIO->in)
		printf("FAIL, unexpected value %#x in MSSGPIO\n", MSSGPIO->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	test_gpio_910(&CGPIO0->in, &MSSGPIO->out, 0, 0);


	printf("Testing CGPIO1 28-31, in...\n");
	if (test_gpio_910(&CGPIO1->in, &MSSGPIO->out, (0<<24|1<<26|0<<18|1<<17), 0x5<<28))
		goto ret;
	if (MSSGPIO->in)
		printf("FAIL, unexpected value %#x in MSSGPIO\n", MSSGPIO->in);
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	if (test_gpio_910(&CGPIO1->in, &MSSGPIO->out, (1<<24|0<<26|1<<18|0<<17), 0xa<<28))
		goto ret;
	if (MSSGPIO->in)
		printf("FAIL, unexpected value %#x in MSSGPIO\n", MSSGPIO->in);
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO2->in)
		printf("FAIL, unexpected value %#x in CGPIO2\n", CGPIO2->in);

	test_gpio_910(&CGPIO1->in, &MSSGPIO->out, 0, 0);

	printf("Testing CGPIO2 2-3, in...\n");
	if (test_gpio_910(&CGPIO2->in, &MSSGPIO->out, (0<<23|1<<28), 1<<2))
		goto ret;
	if (MSSGPIO->in)
		printf("FAIL, unexpected value %#x in MSSGPIO\n", MSSGPIO->in);
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);

	if (test_gpio_910(&CGPIO2->in, &MSSGPIO->out, (1<<23|0<<28), 2<<2))
		goto ret;
	if (MSSGPIO->in)
		printf("FAIL, unexpected value %#x in MSSGPIO\n", MSSGPIO->in);
	if (CGPIO0->in)
		printf("FAIL, unexpected value %#x in CGPIO0\n", CGPIO0->in);
	if (CGPIO1->in)
		printf("FAIL, unexpected value %#x in CGPIO1\n", CGPIO1->in);

	test_gpio_910(&CGPIO2->in, &MSSGPIO->out, 0, 0);

	printf("Walking 1 test on P9 in, P10 out...\n");
	if (test_gpio910_walk(P9, P10, ARRAY_SIZE(P9)))
		goto ret;
	printf("Walking 1 test on P10 in, P9 out...\n");
	if (test_gpio910_walk(P10, P9, ARRAY_SIZE(P9)))
		goto ret;

	printf("PASS\n");

ret:
	clear_all_gpio();
	return ret;
}
 /*
  * do_somtest: command handler.
  */
int do_somtest(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	int ret = 1;

	/*
	 * Check that at least the destination is specified
	 */
	if (argc == 1) {
		printf("%s: command must be specified \
(\"gpio11-12\", \"gpio13-14\", \"gpio13-14-mss\" or \"gpio9-10\"\n", (char *) argv[0]);
		goto Done;
	}

	/*
	 * Parse the command arguments
	 */
	if (! strcmp(argv[1], "gpio11-12")) {
		ret = somtest_gpio1112();
		goto Done;
	}
	if (! strcmp(argv[1], "gpio13-14")) {
		ret = somtest_gpio1314();
		goto Done;
	}
	if (! strcmp(argv[1], "gpio13-14-mss")) {
		ret = somtest_gpio1314_mss();
		goto Done;
	}
	if (! strcmp(argv[1], "gpio9-10")) {
		ret = somtest_gpio910();
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
