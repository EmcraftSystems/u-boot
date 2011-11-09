/*
 * (C) Copyright 2011
 *
 * Alexander Potashev, Emcraft Systems, aspotashev@emcraft.com
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
 * Board specific code for the Embedded Artists LPC1788 board
 */

#include <common.h>

DECLARE_GLOBAL_DATA_PTR;

/*
 * Early hardware init.
 */
int board_init(void)
{
	/* TBD */
	return 0;
}

/*
 * Dump pertinent info to the console.
 */
int checkboard(void)
{
	printf("Board: EA-LPC1788 board %s\n",
		CONFIG_SYS_BOARD_REV_STR);

	return 0;
}

/*
 * Configure board specific parts.
 */
#ifdef CONFIG_MISC_INIT_R
int misc_init_r(void)
{
	/* TBD */
	return 0;
}
#endif /* CONFIG_MISC_INIT_R */

/*
 * Setup external RAM.
 */
int dram_init(void)
{
	/* TBD */
	return 0;
}

