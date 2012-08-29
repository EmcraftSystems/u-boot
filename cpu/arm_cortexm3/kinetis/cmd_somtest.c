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
#include <asm/arch/kinetis.h>
#include <asm/arch/kinetis_gpio.h>

#define K70_RTC_CR		0x4003D010
#define K70_RTC_SC4P		(1 << 12)
#define K70_RTC_SC16P		(1 << 10)
#define K70_RTC_CLKO		(1 << 9)
#define K70_RTC_OSCE		(1 << 8)

#define KINETIS_SIM_CLKOUTSEL_MASK		(7 << 5)
#define KINETIS_SIM_CLKOUTSEL_RTC		(5 << 5)
#define KINETIS_SIM_CLKOUTSEL_PHY		(6 << 5)
#define KINETIS_SIM_CLKOUTSEL_XTAL		(7 << 5)

#define KINETIS_OSC1_CR		0x400E5000
/* External Reference Enable */
#define KINETIS_OSC_CR_EREFSTEN_MSK	(1 << 5)
/* External Reference Stop Enable */
#define KINETIS_OSC_CR_ERCLKEN_MSK	(1 << 7)
#define KINETIS_OSC_CR_SC2P		(1 << 3)
#define KINETIS_OSC_CR_SC4P		(1 << 2)
#define KINETIS_OSC_CR_SC8P		(1 << 1)

#define KINETIS_MCG_FREQ_RANGE01	(1 << 4)
#define KINETIS_MCG_EREFS1		(1 << 2)
#define KINETIS_MCG_LOCRE2		(1 << 7)

struct kinetis_gpio_dsc portc_pin3 = {KINETIS_GPIO_PORT_C, 3};

static int common_rtc_init(void)
{
	int ret = -1;

	if (kinetis_gpio_config(&portc_pin3,
					KINETIS_GPIO_CONFIG_MUX(5)) != 0) {
		printf("%s: error in port config\n", __func__);
		goto xit;
	}
	/*
	 * Enable the RTC in the clock gate, SIM_SCGC6[29]
	 */
	if (kinetis_periph_enable(KINETIS_MKCG(5, 29), 1) != 0) {
		printf("%s: error enabling the clock gate\n", __func__);
		goto xit;
	}

	ret = 0;
xit:
	return ret;
}

static int somtest_rtc(void)
{
	int ret = -1;

	if (common_rtc_init() < 0)
		goto xit;

	/* Enable RTC */
	*(volatile u32 *)K70_RTC_CR |=
		K70_RTC_SC16P | K70_RTC_SC4P | K70_RTC_OSCE;

	KINETIS_SIM->sopt2 &= ~KINETIS_SIM_CLKOUTSEL_MASK;
	KINETIS_SIM->sopt2 |= KINETIS_SIM_CLKOUTSEL_RTC;

	ret = 0;
xit:
	return ret;
}

static int somtest_phy(void)
{
	int ret = -1;

	if (common_rtc_init() < 0)
		goto xit;

	KINETIS_SIM->sopt2 &= ~KINETIS_SIM_CLKOUTSEL_MASK;
	KINETIS_SIM->sopt2 |= KINETIS_SIM_CLKOUTSEL_PHY;

	ret = 0;
xit:
	return ret;
}

static int somtest_xtal(void)
{
	int ret = -1;

	if (common_rtc_init() < 0)
		goto xit;

	/*
	 * Enable the OSC1 in the clock gate, SIM_SCGC1[5]
	 */
	if (kinetis_periph_enable(KINETIS_MKCG(0, 5), 1) != 0) {
		printf("%s: error enabling the clock gate\n", __func__);
		goto xit;
	}
	/* Enable OSC1ERCLK */
	*(volatile u8 *)KINETIS_OSC1_CR =
		KINETIS_OSC_CR_EREFSTEN_MSK | KINETIS_OSC_CR_ERCLKEN_MSK |
			KINETIS_OSC_CR_SC4P;

	/* Additional setup for OSC1 */
	KINETIS_MCG->c10 = KINETIS_MCG_FREQ_RANGE01 | KINETIS_MCG_EREFS1 |
		KINETIS_MCG_LOCRE2;

	KINETIS_SIM->sopt2 &= ~KINETIS_SIM_CLKOUTSEL_MASK;
	KINETIS_SIM->sopt2 |= KINETIS_SIM_CLKOUTSEL_XTAL;
	ret = 0;
xit:
	return ret;
}

static const struct kinetis_gpio_pin_config p13_odd[] = {
	/* F.16 */
	{{KINETIS_GPIO_PORT_F, 16}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.17 */
	{{KINETIS_GPIO_PORT_F, 17}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.18 */
	{{KINETIS_GPIO_PORT_F, 18}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.19 */
	{{KINETIS_GPIO_PORT_F, 19}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.20 */
	{{KINETIS_GPIO_PORT_F, 20}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* D.2 */
	{{KINETIS_GPIO_PORT_D, 2}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* D.3 */
	{{KINETIS_GPIO_PORT_D, 3}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* E.9 */
	{{KINETIS_GPIO_PORT_E, 9}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* E.12 */
	{{KINETIS_GPIO_PORT_E, 12}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* E.11 */
	{{KINETIS_GPIO_PORT_E, 11}, KINETIS_GPIO_CONFIG_MUX(1)}
};


static const struct kinetis_gpio_pin_config p13_even[] = {
	/* F.15 */
	{{KINETIS_GPIO_PORT_F, 15}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.14 */
	{{KINETIS_GPIO_PORT_F, 14}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.13 */
	{{KINETIS_GPIO_PORT_F, 13}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.27 */
	{{KINETIS_GPIO_PORT_F, 27}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.26 */
	{{KINETIS_GPIO_PORT_F, 26}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.25 */
	{{KINETIS_GPIO_PORT_F, 25}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.23 */
	{{KINETIS_GPIO_PORT_F, 23}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.24 */
	{{KINETIS_GPIO_PORT_F, 24}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.22 */
	{{KINETIS_GPIO_PORT_F, 22}, KINETIS_GPIO_CONFIG_MUX(1)},
	/* F.21 */
	{{KINETIS_GPIO_PORT_F, 21}, KINETIS_GPIO_CONFIG_MUX(1)},
};

#define IN			0
#define OUT			1

struct kinetis_gpio {
	u32 pdor; /* data output */
	u32 psor; /* set output */
	u32 pcor; /* clear output */
	u32 ptor; /* toggle output */
	u32 pdir; /* data input */
	u32 pddr; /* data direction */
};

#define K70_GPIO_BASE		0x400FF000
#define KINETIS_GPIO(a)		((volatile struct kinetis_gpio *) \
				(K70_GPIO_BASE + (a)*0x40))
/* Configure GPIO as IN or OUT */
static void config_gpio(int port, int dir, int startpin, int npins)
{
	int i;
	for (i = startpin; i < startpin+npins; i++) {
		if (dir == OUT)
			KINETIS_GPIO(port)->pddr |= (1 << i);
		else
			KINETIS_GPIO(port)->pddr &= ~(1 << i);
	}
}


static char * portname(int port)
{
	switch (port) {
	case KINETIS_GPIO_PORT_A:
		return "A";
	case KINETIS_GPIO_PORT_B:
		return "B";
	case KINETIS_GPIO_PORT_C:
		return "C";
	case KINETIS_GPIO_PORT_D:
		return "D";
	case KINETIS_GPIO_PORT_E:
		return "E";
	case KINETIS_GPIO_PORT_F:
		return "F";
	default:
		return "unknown";
	}
}

static int test_gpio(const struct kinetis_gpio_pin_config *in, u32 size_in,
		const struct kinetis_gpio_pin_config *out, u32 size_out)
{
	int ret = 0;
	int i, j;

	/* odd ports - out, even ports - in */
	for (i = 0; i < size_out; i++) {
		config_gpio(out[i].dsc.port, OUT, out[i].dsc.pin, 1);
	}

	for (i = 0; i < size_in; i++) {
		config_gpio(in[i].dsc.port, IN, in[i].dsc.pin, 1);
	}

	/* set "1010..." to output, check the input */
	for (i = 0; i < size_out; i++) {
		if (i % 2 == 0)
			KINETIS_GPIO(out[i].dsc.port)->psor =
				(1 << out[i].dsc.pin);
		else
			KINETIS_GPIO(out[i].dsc.port)->pcor =
				(1 << out[i].dsc.pin);
	}
	for (i = 0; i < size_in; i++) {
		if (i % 2 == 0) {
			if (!(KINETIS_GPIO(in[i].dsc.port)->pdir &
					(1 << in[i].dsc.pin))) {
				printf("FAIL GPIO%s-%d, should be 1\n",
					portname(in[i].dsc.port),
					in[i].dsc.pin);
				ret = -1;
			}
		} else {
			if (KINETIS_GPIO(in[i].dsc.port)->pdir &
				(1 << in[i].dsc.pin)) {
				printf("FAIL GPIO%s-%d, should be 0\n",
					portname(in[i].dsc.port),
					in[i].dsc.pin);
				ret = -1;
			}
		}
	}

	if (!ret)
		printf("PASSED\n");

	printf("Test another pattern...\n");
	/* Another pattern: set "0101..." to output, check the input */
	for (i = 0; i < size_out; i++) {
		if (i % 2)
			KINETIS_GPIO(out[i].dsc.port)->psor =
				(1 << out[i].dsc.pin);
		else
			KINETIS_GPIO(out[i].dsc.port)->pcor =
				(1 << out[i].dsc.pin);
	}
	for (i = 0; i < size_in; i++) {
		if (i % 2) {
			if (!(KINETIS_GPIO(in[i].dsc.port)->pdir &
					(1 << in[i].dsc.pin))) {
				printf("FAIL GPIO%s-%d, should be 1\n",
					portname(in[i].dsc.port),
					in[i].dsc.pin);
				ret = -1;
			}
		} else {
			if (KINETIS_GPIO(in[i].dsc.port)->pdir &
				(1 << in[i].dsc.pin)) {
				printf("FAIL GPIO%s-%d, should be 0\n",
					portname(in[i].dsc.port),
					in[i].dsc.pin);
				ret = -1;
			}
		}
	}
	if (!ret)
		printf("PASSED\n");
	/* Walking "1" test */
	ret = 0;
	printf("Walking ones test SOM P13, even - in, odd - out...\n");
	/* Clear output first */
	for (i = 0; i < size_out; i++) {
		KINETIS_GPIO(out[i].dsc.port)->pcor =
			(1 << out[i].dsc.pin);
	}

	for (i = 0; i < size_out; i++) {
		KINETIS_GPIO(out[i].dsc.port)->psor =
			(1 << out[i].dsc.pin);
		udelay(10);
		if (!(KINETIS_GPIO(in[i].dsc.port)->pdir &
				(1 << in[i].dsc.pin))) {
			printf("FAIL GPIO%s-%d, should be 1\n",
				portname(in[i].dsc.port),
				in[i].dsc.pin);
			ret = -1;
		}

		/* Now check other inputs are zero */
		for (j = 0; j < size_out; j++) {
			if (i == j) /* already tested above */
				continue;
			if (KINETIS_GPIO(in[j].dsc.port)->pdir &
				(1 << in[j].dsc.pin)) {
				printf("FAIL GPIO%s-%d, should be 0\n",
					portname(in[j].dsc.port),
					in[j].dsc.pin);
				ret = -1;
			}
		}
		/* Clear tested pin */
		KINETIS_GPIO(out[i].dsc.port)->pcor =
			(1 << out[i].dsc.pin);
	}
	if (!ret)
		printf("PASSED\n");


	return ret;
}

static int somtest_gpio_p13(void)
{
	int ret;
	/* Turn on GPIO functionality on the selected pins */
	kinetis_gpio_config_table(p13_odd, ARRAY_SIZE(p13_odd));
	kinetis_gpio_config_table(p13_even, ARRAY_SIZE(p13_even));

	printf("Loopback test SOM P13, even - in, odd - out...\n");
	ret = test_gpio(p13_even, ARRAY_SIZE(p13_even),
			p13_odd, ARRAY_SIZE(p13_odd));

	printf("Loopback test SOM P13, even - out, odd - in...\n");
	ret = test_gpio(p13_odd, ARRAY_SIZE(p13_odd),
			p13_even, ARRAY_SIZE(p13_even));

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
		printf("%s: command must be specified ",
			(char *) argv[0]);
		goto Done;
	}

	/*
	 * Parse the command arguments
	 */
	if (! strcmp(argv[1], "rtc")) {
		ret = somtest_rtc();
		goto Done;
	}

	if (! strcmp(argv[1], "phy")) {
		ret = somtest_phy();
		goto Done;
	}

	if (! strcmp(argv[1], "xtal")) {
		ret = somtest_xtal();
		goto Done;
	}

	if (! strcmp(argv[1], "gpio-p13")) {
		ret = somtest_gpio_p13();
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
