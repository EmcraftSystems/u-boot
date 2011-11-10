/*
 * (C) Copyright 2011
 * Dmitry Konyshev, Emcraft Systems, probables@emcraft.com
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <common.h>
#include <lcd.h>
#include <ili932x.h>

#ifdef CONFIG_LCD_ILI932x

#define XRES		320
#define YRES		240
#define BIT_PER_PIXEL	16

vidinfo_t panel_info = {
	vl_col:		XRES,
	vl_row:		YRES,
	vl_bpix:	4,
};

void *lcd_base;					/* Start of framebuffer memory */
void *lcd_console_address;			/* Start of console buffer     */

int lcd_line_length;
int lcd_color_fg;
int lcd_color_bg;

short console_col;
short console_row;

static volatile u16 *ili932x_reg = NULL;
static volatile u16 *ili932x_ram = NULL;

static u16 ili932x_read_reg(u8 reg)
{
	*ili932x_reg = reg;
	return *ili932x_ram;
}

static void ili932x_write_reg(u8 reg, u16 val)
{
	*ili932x_reg = reg;
	*ili932x_ram = val;
}

void lcd_enable(void)
{
	/* Display On */
	ili932x_write_reg(7, 0x0173); /* 262K color and display ON */
}

void lcd_disable(void)
{
	/* Display Off */
	ili932x_write_reg(7, 0); /* 262K color and display ON */
}

ulong calc_fbsize(void)
{
#ifdef CONFIG_LCD_ILI932x_DOUBLE_BUFFER
	int buffers_cnt = 2;
#else
	int buffers_cnt = 1;
#endif
	return ((panel_info.vl_col * panel_info.vl_row *
		NBITS(panel_info.vl_bpix)) / 8) * buffers_cnt;
}

int overwrite_console(void)
{
	/* Keep stdout / stderr on serial, our LCD is for splashscreen only */
	return 1;
}

void ili932x_update(void)
{
	u16 *b = lcd_base;
	int i;

	ili932x_write_reg(32, 0);
	ili932x_write_reg(33, 319);
	*ili932x_reg = 34;
	for (i = 0; i < XRES * YRES; i++) {
		*ili932x_ram = *b++;
	}
}

void lcd_ctrl_init(void *lcdbase)
{
	u32 mem_len = XRES * YRES * BIT_PER_PIXEL / 8;
	ili932x_reg = (volatile u16 *)CONFIG_LCD_ILI932x_BASE;
	ili932x_ram = (volatile u16 *)CONFIG_LCD_ILI932x_BASE + 1;

	if (!lcdbase)
		return;

	if (ili932x_read_reg(0) != 0x9325)
		return;

	/*
	 * LCD controller initialization taken from the stm322xg_eval_lcd.c sample
	 */

	/* Start Initial Sequence ------------------------------------------------*/
	ili932x_write_reg(0, 0x0001); /* Start internal OSC. */
	ili932x_write_reg(1, 0x0100); /* Set SS and SM bit */
	ili932x_write_reg(2, 0x0700); /* Set 1 line inversion */
	ili932x_write_reg(3, 0x1018); /* Set GRAM write direction and BGR=1. */
	ili932x_write_reg(4, 0x0000); /* Resize register */
	ili932x_write_reg(8, 0x0202); /* Set the back porch and front porch */
	ili932x_write_reg(9, 0x0000); /* Set non-display area refresh cycle ISC[3:0] */
	ili932x_write_reg(10, 0x0000); /* FMARK function */
	ili932x_write_reg(12, 0x0001); /* RGB interface setting */
	ili932x_write_reg(13, 0x0000); /* Frame marker Position */
	ili932x_write_reg(15, 0x0000); /* RGB interface polarity */

	/* Power On sequence -----------------------------------------------------*/
	ili932x_write_reg(16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
	ili932x_write_reg(17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
	ili932x_write_reg(18, 0x0000); /* VREG1OUT voltage */
	ili932x_write_reg(19, 0x0000); /* VDV[4:0] for VCOM amplitude */
	udelay(200000);            /* Dis-charge capacitor power voltage (200ms) */
	ili932x_write_reg(16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
	ili932x_write_reg(17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
	udelay(50000);                       /* Delay 50 ms */
	ili932x_write_reg(18, 0x0139); /* VREG1OUT voltage */
	udelay(50000);                       /* Delay 50 ms */
	ili932x_write_reg(19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
	ili932x_write_reg(41, 0x0013); /* VCM[4:0] for VCOMH */
	udelay(50000);                       /* Delay 50 ms */
	ili932x_write_reg(32, 0x0000); /* GRAM horizontal Address */
	ili932x_write_reg(33, 0x0000); /* GRAM Vertical Address */

	/* Adjust the Gamma Curve (ILI9325)---------------------------------------*/
	ili932x_write_reg(48, 0x0007);
	ili932x_write_reg(49, 0x0302);
	ili932x_write_reg(50, 0x0105);
	ili932x_write_reg(53, 0x0206);
	ili932x_write_reg(54, 0x0808);
	ili932x_write_reg(55, 0x0206);
	ili932x_write_reg(56, 0x0504);
	ili932x_write_reg(57, 0x0007);
	ili932x_write_reg(60, 0x0105);
	ili932x_write_reg(61, 0x0808);

	/* Set GRAM area ---------------------------------------------------------*/
	ili932x_write_reg(80, 0x0000); /* Horizontal GRAM Start Address */
	ili932x_write_reg(81, 0x00EF); /* Horizontal GRAM End Address */
	ili932x_write_reg(82, 0x0000); /* Vertical GRAM Start Address */
	ili932x_write_reg(83, 0x013F); /* Vertical GRAM End Address */

	ili932x_write_reg(96,  0xA700); /* Gate Scan Line(GS=1, scan direction is G320~G1) */
	ili932x_write_reg(97,  0x0001); /* NDL,VLE, REV */
	ili932x_write_reg(106, 0x0000); /* set scrolling line */

	/* Partial Display Control -----------------------------------------------*/
	ili932x_write_reg(128, 0x0000);
	ili932x_write_reg(129, 0x0000);
	ili932x_write_reg(130, 0x0000);
	ili932x_write_reg(131, 0x0000);
	ili932x_write_reg(132, 0x0000);
	ili932x_write_reg(133, 0x0000);

	/* Panel Control ---------------------------------------------------------*/
	ili932x_write_reg(144, 0x0010);
	ili932x_write_reg(146, 0x0000);
	ili932x_write_reg(147, 0x0003);
	ili932x_write_reg(149, 0x0110);
	ili932x_write_reg(151, 0x0000);
	ili932x_write_reg(152, 0x0000);

	/* set GRAM write direction and BGR = 1 */
	/* I/D=00 (Horizontal : increment, Vertical : decrement) */
	/* AM=1 (address is updated in vertical writing direction) */
	ili932x_write_reg(3, 0x1018);

	ili932x_write_reg(7, 0x0133); /* 262K color and display ON */

	memset(lcdbase, 0, mem_len);
	ili932x_update();
}

#endif /* CONFIG_LCD_ILI932x */
