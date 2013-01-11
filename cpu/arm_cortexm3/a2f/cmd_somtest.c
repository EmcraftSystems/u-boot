/*
 * (C) Copyright 2012
 * Vladimir Khusainov, Emcraft Systems, vlad@emcraft.com
 * Sergei Poselenov, Emcraft Systems, sposelenov@emcraft.com
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

/* Active Bipolar Prescalers */
#define ABPS01_BASE		0x40020224
#define ABPS23_BASE		0x40020254
#define ABPS45_BASE		0x40020284
#define ABPS67_BASE		0x400202B4

/* Current Monitor registers */
#define CM0_BASE		0x40020228
#define CM1_BASE		0x40020258
#define CM2_BASE		0x40020288

/* Temperature monitor registers */
#define TM0_BASE		0x4002022C
#define TM1_BASE		0x4002025C
#define TM2_BASE		0x4002028C

/* Temp and Current Monitor definitions */
#define BPMUX			(1 << 0)
#define ISOLATE			(1 << 1)
#define ENA			(1 << 2)
#define STROBE			(1 << 3)
#define COMP_ENABLE		(1 << 4)

/* Structure of the CORE GPIO */
struct coregpio {
	int config[32];
	int intclr;
	int res0[3];
	int in;
	int res1[3];
	int out;
};
/* And MSS GPIO */
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

/* GPIO NN in COREGPIO0 block */
#define DS3LED_GPIO		0
#define DS4LED_GPIO		1
#define S2BUTTON_GPIO		2


/*
 * Connection description for the test 4. P9[i] corresponds to the P10[i].
 */

struct gpiopin {
	volatile void *base;
	int pin;
};
/* Definition of the pin assignment of the P9 header */
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

/* Definition of the pin assignment of the P10 header */
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

/* Structure of the ADC */
struct pa_adc {
	int conv_ctrl;
	int stc;
	int tvc;
	int misc_ctrl;
};

#define ADC_BASE0		0x40020050
#define ADC_BASE1		0x40020090
#define ADC_BASE2		0x400200D0
#define ADC0			((volatile struct pa_adc *)ADC_BASE0)
#define ADC1			((volatile struct pa_adc *)ADC_BASE1)
#define ADC2			((volatile struct pa_adc *)ADC_BASE2)

#define mdelay(n) ({unsigned long msec=(n); while (msec--) udelay(1000);})

#define ADC_STATUS(n)		(*((volatile int *)0x40021000+(n)))

#define SSE_TS_CTRL		0x40020004
#define PPE_CTRL		0x40021404
#define DEVICE_SR		0xE0042034
#define ANA_COMM_CTRL		0x4002000C

/* Number of ADC to use for the P7 header test */
static int adc_P7[] =
{
	99,
	99,
	99,
	1, /*pin3*/
	1, /*pin4*/
	1, /*pin5*/
	0,
	1, /*pin7*/
	99,
	0, /*pin9*/
	99,
	0, /*pin11*/
	99,
	1, /*pin13*/
	99,
	1, /*pin15*/
	99,
	0, /*pin17*/
	99,
	0, /*pin19*/
};
/* Number of ADC to use for the P8 header test */
static int adc_P8[] =
{
	99,
	99,
	99,
	0, /*pin3*/
	99,
	0, /*pin5*/
	99,
	1, /*pin7*/
	99,
	1, /*pin9*/
	99,
	0, /*pin11*/
	99,
	0, /*pin13*/
	99,
	1, /*pin15*/
	99,
	1 /*pin17*/
};

/* Number of ADC to use for the testplan ch.4 testpins */
static int adc_testpins[] =
{
	0,
	0,
};
/* MUXSEL value for the P7 header test */
static int muxsel_P7[] =
{
	99,
	99,
	99,
	2, /*ABPS5*/
	1, /*ABPS4*/
	6, /*ABPS7*/
	2, /*ABPS1*/
	5, /*ABPS6*/
	99,
	5, /*ABPS2*/
	99,
	6, /*ABPS3*/
	99,
	4, /*TM2*/
	99,
	3, /*CM2*/
	99,
	7, /*CM1*/
	99,
	8, /*TM1*/
};

/* MUXSEL value for the P8 header test */
static int muxsel_P8[] =
{
	99,
	99,
	99,
	9, /*ADC0*/
	99,
	10, /*ADC1*/
	99,
	9, /*ADC4*/
	99,
	10, /*ADC5*/
	99,
	12, /*ADC3*/
	99,
	11, /*ADC2*/
	99,
	11, /*ADC6*/
	99,
	12 /*ADC7*/
};

/* MUXSEL value for testpins for testplan ch.4 */
static int muxsel_testpins[] =
{
	1, /*ABPS0*/
	3, /*CM0*/
};
/* The names of the pins on the P7 header */
static char *pinnames_P7[] = {
	"n/a",
	"n/a",
	"n/a",
	"ABPS5",
	"ABPS4",
	"ABPS7",
	"ABPS1",
	"ABPS6",
	"n/a",
	"ABPS2",
	"n/a",
	"ABPS3",
	"n/a",
	"TM2",
	"n/a",
	"CM2",
	"n/a",
	"CM1",
	"n/a",
	"TM1"
};
/* The names of the pins on the P8 header */
static char *pinnames_P8[] = {
	"n/a",
	"n/a",
	"n/a",
	"ADC0",
	"n/a",
	"ADC1",
	"n/a",
	"ADC4",
	"n/a",
	"ADC5",
	"n/a",
	"ADC3",
	"n/a",
	"ADC2",
	"n/a",
	"ADC6",
	"n/a",
	"ADC7"
};

static char *pinnames_testpins[] = {
	"ABPS0",
	"CM0"
};


/* RTC definitions */
#define RTC_CTRL_STAT_REG		0x40014160
#define RTC_RST				(1 << 7)
#define CNTR_EN				(1 << 6)
#define RSTB_CNT			(1 << 1)
#define XTAL_EN				(1 << 0)

#define RTC_COUNTER0_REG		0x40014100
#define RTC_COUNTER1_REG		0x40014104
#define RTC_COUNTER2_REG		0x40014108
#define RTC_COUNTER3_REG		0x4001410C
#define RTC_COUNTER4_REG		0x40014110

/* Clear GPIO configuration bits, thus disable the GPIO */
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

/* Perform testing of the GPIO. "gin" is loop-back-connected with "gout" */

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

	udelay(1);

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
		udelay(1);
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

/* Configure GPIO as IN or OUT */
static void config_gpio(volatile int *cfg, int dir, int startpin, int npins)
{
	volatile int *config;
	int i;

	for (i = startpin, config = cfg+startpin; i < startpin+npins;
	     i++, config++) {
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

/* Init ADC. We assume ACE clock is 40 MHz, as reported in the U-Boot banner */
static int adc_init(void)
{
	int ret = -1;
	/* Init pins */
	/* Set range for ABPS1-7 for +-2.56V, enable */
	*(volatile int *)ABPS01_BASE |= (3 << 5) | (1 << 4);
	*(volatile int *)ABPS23_BASE |= (3 << 1 | 3 << 5) | (1 | 1 << 4);
	*(volatile int *)ABPS45_BASE |= (3 << 1 | 3 << 5) | (1 | 1 << 4);
	*(volatile int *)ABPS67_BASE |= (3 << 1 | 3 << 5) | (1 | 1 << 4);

	/* Set range +-5.12 V for ABPS0 and enable it */
	*(volatile int *)ABPS01_BASE |= (2 << 1) | (1 << 0);

	/* Configure CM0,1-2 and TM1-2, see Table 9-1:
	 * comparator disable, isolation switch closed,
	 * mux direct input
	 */
	*(volatile int *)CM0_BASE &= ~(ENA | COMP_ENABLE);
	*(volatile int *)CM0_BASE |= BPMUX | ISOLATE;
	*(volatile int *)CM1_BASE &= ~(ENA | COMP_ENABLE);
	*(volatile int *)CM1_BASE |= BPMUX | ISOLATE;
	*(volatile int *)CM2_BASE &= ~(ENA|COMP_ENABLE);
	*(volatile int *)CM2_BASE |= BPMUX | ISOLATE;

	*(volatile int *)TM1_BASE &= ~ENA;
	*(volatile int *)TM1_BASE |= BPMUX | ISOLATE;
	*(volatile int *)TM2_BASE &= ~ENA;
	*(volatile int *)TM2_BASE |= BPMUX | ISOLATE;

	/* Enable Temp Monitor 0 for Ch.4 test:
	 * Isolation switch open, mux from TM, TM0 enable
	 */
	*(volatile int *)TM0_BASE &= ~(ISOLATE | BPMUX);
	*(volatile int *)TM0_BASE |= ENA;
	/* ADC Clock must be between 0.5 and 10 MHz.
	 * Set the divider for 10 MHz.
	 */
	ADC0->tvc = 0;
	ADC1->tvc = 0;

	/* Set the resolution, 12 bit */
	ADC0->misc_ctrl &= ~0x3;
	ADC1->misc_ctrl &= ~0x3;
	ADC0->misc_ctrl |= 1;
	ADC1->misc_ctrl |= 1;

	/* Turn off PPE and SSE */
	*(volatile int *)SSE_TS_CTRL &= ~0x3;
	*(volatile int *)PPE_CTRL &= ~0x1;

	/* Set up sampling time for the given ADC clock/sampling time
	 * ADCx_STC = (Tsample/(1/ADCCLK)) - 2. See Table 3-2 for Tsample.
	 */
	ADC0->stc = 5;
	ADC1->stc = 5;
	/* Check for the 3.3V OK */
	if (!(*(volatile int *)DEVICE_SR & 2)) {
		printf(" ADC init fail: brownout 3.3V is set?\n");
		goto ret;
	}
	/* Select internal VAREF, release reset */
	*(volatile int *)ANA_COMM_CTRL &= ~3;
	ADC0->misc_ctrl &= ~(1<<4); /* clear  reset */
	ADC1->misc_ctrl &= ~(1<<4); /* clear  reset */

	/* Enable Analog Block */
	*(volatile int *)ANA_COMM_CTRL |= (1 << 3); /* ABPOWERON */
	/* wait for stabilization for the given mode and VAREF capacitor */
	mdelay(400);

	/* wait for calibration end */
	while (ADC_STATUS(0) & (1 << 15) || ADC_STATUS(1) & (1 << 15))
		udelay(1);
	printf("ADCs ready\n");

	ret = 0;
ret:
	return ret;
}

/* Convert raw ADC reading to mV.
 */
static float adc_raw2v(char *name, int val)
{
	float ret = -1;
	float gain;
	if (strstr(name, "ABPS")) {
		if (!strcmp(name, "ABPS0")) {
			/* ABPS0 is configured for +- 5.12 V */
			gain = 0.25;
		} else
			gain = 0.5;

		ret = ((float)(2560/2) -
			((float)val * 2560.0)/4095.0)/gain;

	} else
		ret = ((float)val*2560.)/4095.;

	return ret;
}

static void adc_measure(void)
{
	volatile struct pa_adc *ptr;
	int i, val;

	for (i = 0; i < ARRAY_SIZE(muxsel_P7); i++) {
		printf("P7-%d:(%s)\t", i, pinnames_P7[i]);
		if (adc_P7[i] == 99) {
			printf("n/a\n");
			continue;
		}
		if (adc_P7[i] == 0)
			ptr = ADC0;
		else
			ptr = ADC1;

		/* setup muxsel for the input pin */
		ptr->conv_ctrl &= ~0xf;
		ptr->conv_ctrl |= muxsel_P7[i];
		/* Start conversion */
		ptr->conv_ctrl |= (1 << 7);
		/* Wait until datavalid bit appears */
		while (!(ADC_STATUS(adc_P7[i]) & (1 << 12)))
			udelay(10);

		printf("%d mV (raw %d)\n",
			(int)adc_raw2v(pinnames_P7[i], ADC_STATUS(adc_P7[i]) & 0xfff),
			ADC_STATUS(adc_P7[i]) & 0xfff);
	}

	printf("\n\n");
	for (i = 0; i < ARRAY_SIZE(muxsel_P8); i++) {
		printf("P8-%d:(%s)\t", i, pinnames_P8[i]);
		if (adc_P8[i] == 99) {
			printf("n/a\n");
			continue;
		}
		if (adc_P8[i] == 0)
			ptr = ADC0;
		else
			ptr = ADC1;

		/* setup muxsel for the input pin */
		ptr->conv_ctrl &= ~0xf;
		ptr->conv_ctrl |= muxsel_P8[i];
		/* Start conversion */
		ptr->conv_ctrl |= (1 << 7);
		/* Wait until datavalid bit appears */
		while (!(ADC_STATUS(adc_P8[i]) & (1 << 12)))
			udelay(10);

		printf("%d mV (raw %d)\n",
			(int)adc_raw2v(pinnames_P8[i], ADC_STATUS(adc_P8[i]) & 0xfff),
			ADC_STATUS(adc_P8[i]) & 0xfff);
	}

	printf("\n\n");
	for (i = 0; i < ARRAY_SIZE(muxsel_testpins); i++) {
		printf("%s:\t", pinnames_testpins[i]);
		if (adc_testpins[i] == 99) {
			printf("n/a\n");
			continue;
		}
		if (adc_testpins[i] == 0)
			ptr = ADC0;
		else
			ptr = ADC1;

		/* setup muxsel for the input pin */
		ptr->conv_ctrl &= ~0xf;
		ptr->conv_ctrl |= muxsel_testpins[i];
		/* Start conversion */
		ptr->conv_ctrl |= (1 << 7);
		/* Wait until datavalid bit appears */
		while (!(ADC_STATUS(adc_testpins[i]) & (1 << 12)))
			udelay(10);

		printf("%d mV (raw %d)\n",
			(int)adc_raw2v(pinnames_testpins[i],
				ADC_STATUS(adc_testpins[i]) & 0xfff),
			ADC_STATUS(adc_testpins[i]) & 0xfff);
	}


	/* Measure the temperature from TM0 */
	*(volatile int *)TM0_BASE |= STROBE;
	udelay(7); /* > 5us */
	/* setup muxsel for the input pin */
	ADC0->conv_ctrl &= ~0xf;
	ADC0->conv_ctrl |= 4;
	/* Start conversion */
	ADC0->conv_ctrl |= (1 << 7);
	/* Wait until datavalid bit appears */
	while (!(ADC_STATUS(0) & (1 << 12)))
		udelay(10);
	/* Remove STROBE */
	*(volatile int *)TM0_BASE &= ~(STROBE);
	val = ADC_STATUS(0) & 0xfff;
	printf("TM0:\t%d C (%d mv, raw %d)\n",
		((int)(adc_raw2v("TM0", val)*2)/5)-273,
		(int)adc_raw2v("TM0", val), val);
	printf("PASS\n");
}

static void adc_disable(void)
{
	/* Disable the Analog Block */
	*(volatile int *)ANA_COMM_CTRL &= ~(1 << 3); /* ABPOWERON */
}

static int somtest_adc(void)
{
	if (!adc_init())
		adc_measure();

	adc_disable();
	return 0;
}

/* After enable, the counter appear to work after ~40 seconds (!?).
 * Submitted a SR to Microsemi.
 */
static int somtest_rtc(char *arg)
{
	unsigned long long cnt;

	cnt = *(volatile int *)RTC_COUNTER0_REG;
	cnt |= (*(volatile int *)RTC_COUNTER1_REG) << 8;
	cnt |= (*(volatile int *)RTC_COUNTER2_REG) << 16;
	cnt |= (*(volatile int *)RTC_COUNTER3_REG) << 24;
	cnt |= (unsigned long long)(*(volatile int *)RTC_COUNTER4_REG) << 32;

	printf("current RTC val is %Ld (%Ld sec), ctrl reg %#x\n", cnt,
		cnt/256, *(volatile int *)RTC_CTRL_STAT_REG);

	if (!strcmp(arg, "start")) {
		/* Stop counter */
		*(volatile int *)RTC_CTRL_STAT_REG &= ~CNTR_EN;
		A2F_SYSREG->clr_mss_sr |= 0x1; /* Clear pending RTC int */
		/* init counter */
		*(volatile int *)RTC_CTRL_STAT_REG |= XTAL_EN;
		*(volatile int *)RTC_CTRL_STAT_REG |= RSTB_CNT;

		printf("Reset RTC to zero\n");
		/* clear counter */
		*(volatile int *)RTC_COUNTER0_REG = 0;
		*(volatile int *)RTC_COUNTER1_REG = 0;
		*(volatile int *)RTC_COUNTER2_REG = 0;
		*(volatile int *)RTC_COUNTER3_REG = 0;
		*(volatile int *)RTC_COUNTER4_REG = 0;

		/* start counter */
		*(volatile int *)RTC_CTRL_STAT_REG |= CNTR_EN;
		/* delay for 1 sec */
	} else {
		if (!strcmp(arg, "stop")) {
			/* Stop counter */
			*(volatile int *)RTC_CTRL_STAT_REG &= ~CNTR_EN;
		}
	}
	return 0;
}

/* Test plan chapter 5. Test LEDs */
static int somtest_led(void)
{
	int i;

	config_gpio(&CGPIO0->config[0], OUT, DS3LED_GPIO, 1);
	config_gpio(&CGPIO0->config[0], OUT, DS4LED_GPIO, 1);
	config_gpio(&CGPIO0->config[0], IN, S2BUTTON_GPIO, 1);

	printf("Check the DS4 LED, should blink three times...\n");
	mdelay(1000);
	for (i = 0; i < 6; i++) {
		CGPIO0->out ^= (1 << DS3LED_GPIO);
		mdelay(1000);
	}

	printf("Check the DS3 LED, should blink three times...\n");
	mdelay(1000);
	for (i = 0; i < 6; i++) {
		CGPIO0->out ^= (1 << DS4LED_GPIO);
		mdelay(1000);
	}
	for (i = 0; i < 6; i++) {
		printf("Current state of the S2 button is %d, %s the button\n",
			!!(CGPIO0->in & (1 << S2BUTTON_GPIO)), i%2 ? "release" : "hold");
		mdelay(5000);
	}
	printf("PASSED\n");

	return 0;
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
		printf("%s: command must be specified "
			"(\"gpio11-12\", \"gpio13-14\", \"gpio13-14-mss\", "
			"\"gpio9-10\", \"adc\",\"rtc\" or \"led\"\n",
			(char *) argv[0]);
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

	if (! strcmp(argv[1], "adc")) {
		ret = somtest_adc();
		goto Done;
	}

	if (! strcmp(argv[1], "rtc")) {
		ret = somtest_rtc(argv[2]);
		goto Done;
	}

	if (! strcmp(argv[1], "led")) {
		ret = somtest_led();
		goto Done;
	}

	printf("%s: unrecognized command %s\n",
		(char *) argv[0], (char *) argv[1]);

	Done:
	return ret;
}

U_BOOT_CMD(
	somtest,	3,		1,	do_somtest,
	"SmartFusion SOM test suite",
	"[gpio]"
);
