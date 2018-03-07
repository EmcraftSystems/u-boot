/*
 * (C) Copyright 2018
 * Alexander Dyachenko, Emcraft Systems, sasha_d@emcraft.com
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
#include <lcd.h>
#include <asm/io.h>
#include <div64.h>
#include <asm/arch/kinetis_gpio.h>
#include "videomodes.h"

#undef DEBUG

#ifdef DEBUG
#define DPRINT(x...) printf(x)
#else
#define DPRINT(x...) do{}while(0)
#endif

#define PICOS2KHZ(a) (1000000000UL/(a))
#define KHZ2PICOS(a) (1000000000UL/(a))

/*
 * System Clock Divider Register 3
 */
/* LCD Controller clock divider divisor */
#define KINETIS_SIM_CLKDIV3_LCDCDIV_BITS	16
#define KINETIS_SIM_CLKDIV3_LCDCDIV_BITWIDTH	12
#define KINETIS_SIM_CLKDIV3_LCDCDIV_MSK \
	(((1 << KINETIS_SIM_CLKDIV3_LCDCDIV_BITWIDTH) - 1) << \
	KINETIS_SIM_CLKDIV3_LCDCDIV_BITS)
/* LCD Controller clock divider fraction */
#define KINETIS_SIM_CLKDIV3_LCDCFRAC_BITS	8
#define KINETIS_SIM_CLKDIV3_LCDCFRAC_BITWIDTH	8
#define KINETIS_SIM_CLKDIV3_LCDCFRAC_MSK \
	(((1 << KINETIS_SIM_CLKDIV3_LCDCFRAC_BITWIDTH) - 1) << \
	KINETIS_SIM_CLKDIV3_LCDCFRAC_BITS)

/*
 * Misc Control Register
 */
/* Start LCDC display */
#define KINETIS_SIM_MCR_LCDSTART_MSK	(1 << 16)

/*
 * Maximum values for the LCDC clock divisor and fraction
 */
/* LCDCDIV: 12 bits, up to 4096 */
#define KINETIS_LCDC_MAX_NUMERATOR	((KINETIS_SIM_CLKDIV3_LCDCDIV_MSK >> \
					KINETIS_SIM_CLKDIV3_LCDCDIV_BITS) + 1)
/* LCDCFRAC: 8 bits, up to 256 */
#define KINETIS_LCDC_MAX_DENOMINATOR	((KINETIS_SIM_CLKDIV3_LCDCFRAC_MSK >> \
					KINETIS_SIM_CLKDIV3_LCDCFRAC_BITS) + 1)

/*
 * LCDC DMA control register
 */
#define KINETIS_LCDC_LDCR_HM_BITS	16
#define KINETIS_LCDC_LDCR_TM_BITS	0

/*
 * Freescale Kinetis LCD Controller register base
 */
#define KINETIS_LCDC_BASE		(KINETIS_AIPS1PERIPH_BASE + 0x00036000)

#define LCDC_SSA	0x00

#define LCDC_SIZE	0x04
#define SIZE_XMAX(x)	((((x) >> 4) & 0x3f) << 20)
#define SIZE_YMAX(y)	((y) & 0x3ff)

#define LCDC_VPW	0x08
#define VPW_VPW(x)	((x) & 0x3ff)

#define LCDC_CPOS	0x0C
#define CPOS_CC1	(1<<31)
#define CPOS_CC0	(1<<30)

#define LCDC_PCR	0x18
#define PCR_TFT		(1 << 31)
#define PCR_COLOR	(1 << 30)
#define PCR_PBSIZ_1	(0 << 28)
#define PCR_PBSIZ_2	(1 << 28)
#define PCR_PBSIZ_4	(2 << 28)
#define PCR_PBSIZ_8	(3 << 28)
#define PCR_BPIX_1	(0 << 25)
#define PCR_BPIX_2	(1 << 25)
#define PCR_BPIX_4	(2 << 25)
#define PCR_BPIX_8	(3 << 25)
#define PCR_BPIX_12	(4 << 25)
#define PCR_BPIX_16	(5 << 25)
#define PCR_BPIX_18	(6 << 25)
#define PCR_BPIX_24	(7 << 25)
#define PCR_PIXPOL	(1 << 24)
#define PCR_FLMPOL	(1 << 23)
#define PCR_LPPOL	(1 << 22)
#define PCR_CLKPOL	(1 << 21)
#define PCR_OEPOL	(1 << 20)
#define PCR_SCLKIDLE	(1 << 19)
#define PCR_END_SEL	(1 << 18)
#define PCR_END_BYTE_SWAP (1 << 17)
#define PCR_REV_VS	(1 << 16)
#define PCR_ACD_SEL	(1 << 15)
#define PCR_ACD(x)	(((x) & 0x7f) << 8)
#define PCR_SCLK_SEL	(1 << 7)
#define PCR_SHARP	(1 << 6)

#define LCDC_HCR	0x1C
#define HCR_H_WIDTH(x)	(((x) & 0x3f) << 26)
#define HCR_H_WAIT_1(x)	(((x) & 0xff) << 8)
#define HCR_H_WAIT_2(x)	((x) & 0xff)

#define LCDC_VCR	0x20
#define VCR_V_WIDTH(x)	(((x) & 0x3f) << 26)
#define VCR_V_WAIT_1(x)	(((x) & 0xff) << 8)
#define VCR_V_WAIT_2(x)	((x) & 0xff)

#define LCDC_POS	0x24

#define LCDC_LSCR1	0x28

#define LCDC_PWMR	0x2C

#define LCDC_DMACR	0x30

#define LCDC_RMCR	0x34
#define RMCR_LCDC_EN	0
#define RMCR_SELF_REF	(1<<0)


#if defined(CONFIG_LCD_TWR_LCD_RGB) /* Freescale TWR-K70F120M with TWR-LCD-RGB module and Seiko 43WQW3T */

vidinfo_t panel_info = {
	.vl_col		= 480,
	.vl_row		= 272,
	/*
	 * The screen is 24bpp, but every pixel occupies 32 bits
	 * of memory.
	 */
	.vl_bpix	= 32,
};

static struct ctfb_res_modes panel_videomode = {
	.pixclock	= KHZ2PICOS(10000),	/* 10 MHz */
	.hsync_len	= 41,	/* Horiz. pulse width */
	.left_margin	= 3,	/* Horiz. front porch */
	.right_margin	= 2,	/* Horiz. back porch */
	.vsync_len	= 10,	/* Vert. pulse width */
	.upper_margin	= 2,	/* Vert. front porch */
	.lower_margin	= 2,	/* Vert. back porch */
};

static u32 panel_pcr =
	PCR_END_SEL |		/* Big Endian */
	PCR_TFT | PCR_COLOR |	/* Color TFT */
	PCR_SCLK_SEL |		/* Always enable LSCLK */
	PCR_SCLKIDLE |
	PCR_CLKPOL |		/* Polarities */
	PCR_FLMPOL |
	PCR_LPPOL;

#elif defined(CONFIG_LCD_FPC_TFC) /* Ametek FPC-1U TFC E9390RTCV32 */

vidinfo_t panel_info = {
	.vl_col		= 480,
	.vl_row		= 272,
	/*
	 * The screen is 24bpp, but every pixel occupies 32 bits
	 * of memory.
	 */
	.vl_bpix	= 32,
};

static struct ctfb_res_modes panel_videomode = {
	.pixclock	= KHZ2PICOS(13700),	/* 13.7 MHz */
	.hsync_len	= 2,		/* Horiz. pulse width */
	.left_margin	= 7,		/* Horiz. front porch =8-1*/
	.right_margin	= 38,		/* Horiz. back porch  =43-5*/
	.vsync_len	= 10,		/* Vert. pulse width */
	.upper_margin	= 4,		/* Vert. front porch */
	.lower_margin	= 2,		/* Vert. back porch 12-10*/
};

static u32 panel_pcr =
	PCR_END_SEL |		/* Big Endian */
	PCR_TFT | PCR_COLOR |	/* Color TFT */
	PCR_SCLK_SEL |		/* Always enable LSCLK */
	PCR_SCLKIDLE |
	PCR_ACD_SEL |
	PCR_FLMPOL |		/* Polarities */
	PCR_LPPOL;

#elif defined(CONFIG_LCD_FPC_TFC2U) /* Ametek FPC-2U TFC 9500RTWQ30TR */

vidinfo_t panel_info = {
	.vl_col		= 480,
	.vl_row		= 272,
	/*
	 * The screen is 24bpp, but every pixel occupies 32 bits
	 * of memory.
	 */
	.vl_bpix	= 32,
};

static struct ctfb_res_modes panel_videomode = {
	.pixclock	= KHZ2PICOS(9000),	/* 9 MHz */
	.hsync_len	= 2,		/* Horiz. pulse width */
	.left_margin	= 4,		/* Horiz. front porch =5-1*/
	.right_margin	= 35,		/* Horiz. back porch  =40-5*/
	.vsync_len	= 10,		/* Vert. pulse width */
	.upper_margin	= 8,		/* Vert. front porch */
	.lower_margin	= 8,		/* Vert. back porch */
};

static u32 panel_pcr =
	PCR_END_SEL |		/* Big Endian */
	PCR_TFT | PCR_COLOR |	/* Color TFT */
	PCR_SCLK_SEL |		/* Always enable LSCLK */
	PCR_SCLKIDLE |
	PCR_ACD_SEL |
	PCR_FLMPOL |		/* Polarities */
	PCR_LPPOL;

#else
#error "No LCD panel defined!"
#endif


/*
 * Write the LCDC register
 */
static inline u32 imxfb_writel(u32 val, u32 offset)
{
	return writel(val, KINETIS_LCDC_BASE + offset);
}

/*
 * Read the LCDC register
 */
static inline u32 imxfb_readl(u32 offset)
{
	return readl(KINETIS_LCDC_BASE + offset);
}

/*
 * Adjust the LCDC clock divider to produce a frequency as close as possible
 * to the desired value.
 */
static void imxfb_lcdc_adjust_clock_divider(void)
{
	unsigned long clock = PICOS2KHZ(panel_videomode.pixclock) * 1000;
	unsigned long base = clock_get(CLOCK_CCLK);

	/*
	 * Length limit for the continued fraction. In the worst case
	 * of the integer parts in the continued fraction being all ones
	 * (golden ratio), numerator and denominator of the evaluated continued
	 * fraction grow like Fibonacci numbers, or like powers of golden
	 * ratio. This is why for the length limit (N) we choose the logarithm
	 * of 4096 (LCDCDIV maximum value) to base of golden ratio (1.618...),
	 * plus a few extra array elements for safety.
	 */
	const int N = 20;

	/* Integer parts in the continued fraction */
	u32 seq[N];
	/* Indices in the "seq" array */
	int i, j;
	/* Temporary variables for evaluation of continued fraction */
	u32 a, b, c;
	/* Final numerator and denominator */
	u32 last_a = 1, last_b = 1;
	/* Target value, multiplied by 2**16 */
	u32 x;

	/* x = (base << 16) / clock */
	u64 tmp = (u64)base << 16;
	do_div(tmp, clock);
	x = (u32)tmp;

	/*
	 * Build the continued fraction (Diophantine approximation of "x")
	 */
	for (i = 0; i < N; i++) {
		/* floor(x) */
		seq[i] = x >> 16;

		/* Result is a/b, "c" is a helper variable */
		a = 1;
		b = 0;
		for (j = i; j >= 0; j--) {
			/* Partial fraction p_{next} = seq + p^{-1} */
			/* New numerator */
			c = seq[j] * a + b;
			/* New denominator */
			b = a;
			/* Write nominator into the correct variable */
			a = c;

			if (a > KINETIS_LCDC_MAX_NUMERATOR ||
			    b > KINETIS_LCDC_MAX_DENOMINATOR)
				break;
		}

		if (b > KINETIS_LCDC_MAX_DENOMINATOR ||
		    a > KINETIS_LCDC_MAX_NUMERATOR) {
			/* Stop approximation on exceeding of the limits */
			break;
		} else {
			/* Approximation is good, save it */
			last_a = a;
			last_b = b;
		}

		/* Keep fractional part */
		x &= 0xffff;
		/* Exit if we are already very close to the target value */
		if (x < 2)
			break;

		/* x = 1.0/x */
		x = ((u32)-1) / x;
	}

	/*
	 * Write LCDC clock divider values to the SIM_CLKDIV3 register
	 */
	KINETIS_SIM->clkdiv3 =
		((last_a - 1) << KINETIS_SIM_CLKDIV3_LCDCDIV_BITS) |
		((last_b - 1) << KINETIS_SIM_CLKDIV3_LCDCFRAC_BITS);
}

/*
 * Calculate value of the LCDC panel configuration register (LCDC_LPCR)
 */
static u32 imxfb_calc_pcr(void)
{
	u32 pcr;
	unsigned long lcd_clk;
	unsigned long long tmp;

	/*
	 * Calculate the the actual clock generated by System Clock Divider 3
	 */
	lcd_clk = clock_get(CLOCK_CCLK) /
		(((KINETIS_SIM->clkdiv3 &
			KINETIS_SIM_CLKDIV3_LCDCDIV_MSK) >>
		KINETIS_SIM_CLKDIV3_LCDCDIV_BITS) + 1) *
		(((KINETIS_SIM->clkdiv3 &
			KINETIS_SIM_CLKDIV3_LCDCFRAC_MSK) >>
		KINETIS_SIM_CLKDIV3_LCDCFRAC_BITS) + 1);

	DPRINT ("imxfb: pixel clock: desired = %luMHz, actual = %luMHz\n",
		PICOS2KHZ(panel_videomode.pixclock) * 1000, lcd_clk);

	/*
	 * Calculate the pixel clock divider (PCD) field as
	 * a ratio of the actual clock generated by System Clock Divider 3
	 * to the desired clock for the used LCD panel
	 */
	tmp = panel_videomode.pixclock * (unsigned long long)lcd_clk;

	do_div(tmp, 1000000);

	if (do_div(tmp, 1000000) > 500000)
		tmp++;

	pcr = (unsigned int)tmp;

	DPRINT ("imxfb: pixel clock divider: actual / desired = %lu\n", pcr);

	/*
	 * The bus clock is divided by N (PCD plus one) to yield the pixel clock rate.
	 * Values of 1 to 63 yield N=2 to 64.
	 */
	if (--pcr > 0x3F) {
		pcr = 0x3F;
		printf("imxfb: WARNING: Must limit pixel clock to %luHz\n",
				lcd_clk / pcr);
	}

	/*
	 * Calculate the BPIX field (Bits per pixel)
	 */
	switch (panel_info.vl_bpix) {
	case 32:
		/*
		 * red:   offset = 16, length = 8
		 * green: offset = 8,  length = 8
		 * blue:  offset = 0,  length = 8
		 *
		 * Actually this really is 18bit support,
		 * the lowest 2 bits of each colour are unused in hardware.
		 */
		pcr |= PCR_BPIX_24;
		break;
	case 16:
	default:
		/*
		 * if (panel_pcr & PCR_TFT) != 0
		 *     red:   offset = 11, length = 5
		 *     green: offset = 5,  length = 6
		 *     blue:  offset = 0,  length = 5
		 *
		 * if (panel_pcr & PCR_TFT) == 0
		 *     red:   offset = 8,  length = 4
		 *     green: offset = 4,  length = 4
		 *     blue:  offset = 0,  length = 4
		 */
		pcr |= PCR_BPIX_16;
		break;
	case 8:
		/*
		 * red:   offset = 0, length = 8
		 * green: offset = 0, length = 8
		 * blue:  offset = 0, length = 8
		 */
		pcr |= PCR_BPIX_8;
		break;
	}

	/*
	 * Add sync polarities
	 */
	pcr |= panel_pcr & ~(0x3f | (7 << 25));

	return pcr;
}

/*
 * Configure the LCDC controller
 */
static void imxfb_configure_controller(u32 pcr, void *lcdbase)
{
	/* physical screen start address	    */
	imxfb_writel(VPW_VPW(panel_info.vl_col * panel_info.vl_bpix / 8 / 4),
		LCDC_VPW);

	imxfb_writel(HCR_H_WIDTH(panel_videomode.hsync_len - 1) |
		HCR_H_WAIT_1(panel_videomode.right_margin - 1) |
		HCR_H_WAIT_2(panel_videomode.left_margin - 3),
		LCDC_HCR);

	imxfb_writel(VCR_V_WIDTH(panel_videomode.vsync_len) |
		VCR_V_WAIT_1(panel_videomode.lower_margin) |
		VCR_V_WAIT_2(panel_videomode.upper_margin),
		LCDC_VCR);

	imxfb_writel(SIZE_XMAX(panel_info.vl_col) | SIZE_YMAX(panel_info.vl_row),
		LCDC_SIZE);

	imxfb_writel(pcr, LCDC_PCR);

	/* Disable PWM contrast control */
	imxfb_writel(0x00000000, LCDC_PWMR);

	/* LSCR1 is not supported on Kinetis */
	imxfb_writel(0x00000000, LCDC_LSCR1);

	/*
	 * DMA control register value. We use default values for the
	 * `DMA high mark` and the `DMA trigger mark`. The burst length is
	 * dynamic.
	 */
	imxfb_writel((0x04 << KINETIS_LCDC_LDCR_HM_BITS) |
		(0x60 << KINETIS_LCDC_LDCR_TM_BITS),
		LCDC_DMACR);

	imxfb_writel((u32)lcdbase, LCDC_SSA);

	/* panning offset 0 (0 pixel offset)        */
	imxfb_writel(0x00000000, LCDC_POS);

	/* disable hardware cursor */
	imxfb_writel(imxfb_readl(LCDC_CPOS) & ~(CPOS_CC0 | CPOS_CC1),
		LCDC_CPOS);

	/*
	 * TBD: It's possible that the RMCR_SELF_REF is needed as well
	 */
	imxfb_writel(RMCR_LCDC_EN, LCDC_RMCR);

	/* Enable the LCDC clock */
	KINETIS_SIM->mcr |= KINETIS_SIM_MCR_LCDSTART_MSK;
}

/*
 * Turn on the LCD backlight
 */
static void imxfb_backlight_power_on(void)
{
#if defined(CONFIG_LCD_BACKLIGHT_PORT) && defined(CONFIG_LCD_BACKLIGHT_PIN)
	struct kinetis_gpio_dsc dsc = {
		.port = CONFIG_LCD_BACKLIGHT_PORT,
		.pin  = CONFIG_LCD_BACKLIGHT_PIN,
	};
	kinetis_gpio_set_out(&dsc, CONFIG_LCD_BACKLIGHT_VAL);
#endif
}

/*
 * Enable LCD
 */
static void imxfb_enable_lcd(void)
{
#if defined(CONFIG_LCD_ENABLE_PORT) && defined(CONFIG_LCD_ENABLE_PIN)
	struct kinetis_gpio_dsc dsc = {
		.port = CONFIG_LCD_ENABLE_PORT,
		.pin  = CONFIG_LCD_ENABLE_PIN,
	};
	kinetis_gpio_set_out(&dsc, CONFIG_LCD_ENABLE_VAL);
#endif
}


/*********************************
 * Standard U-Boot LCD framework *
 *********************************/

/* Start of framebuffer memory */
void *lcd_base;
/* Start of console buffer */
void *lcd_console_address;

int lcd_line_length;
int lcd_color_fg;
int lcd_color_bg;

short console_col;
short console_row;


void lcd_ctrl_init(void *lcdbase)
{
	u32 pcr;

	if (!lcdbase)
		return;

	/*
	 * Enable power on the LCD Controller module to make its
	 * register map accessible.
	 */
	kinetis_periph_enable(KINETIS_CG_LCDC, 1);

	/*
	 * Adjust the LCDC clock divider to produce a frequency as close as possible
	 * to the desired value.
	 */
	imxfb_lcdc_adjust_clock_divider();

	/*
	 * Calculate value of the LCDC panel configuration register (LCDC_LPCR)
	 */
	pcr = imxfb_calc_pcr();

	/*
	 * Configure the LCDC controller
	 */
	imxfb_configure_controller(pcr, lcdbase);

}

void lcd_setcolreg (ushort regno, ushort red, ushort green, ushort blue)
{
}

void lcd_enable(void)
{
	/*
	 * Turn on the LCD backlight
	 */
	imxfb_backlight_power_on();

	/*
	 * Enable LCD
	 */
	imxfb_enable_lcd();
}
