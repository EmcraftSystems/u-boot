/*
 * (C) Copyright 2010,2011
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

OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm",
	      "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
	NVM (r):	ORIGIN = 0x00000000, LENGTH = 256K
	RAM (rw):	ORIGIN = 0x20000000, LENGTH = 16K
	RAM_BUF (r):	ORIGIN = 0x20004000, LENGTH = 32K
	MALLOC (r):	ORIGIN = 0x2000D000, LENGTH = 8K
	STACK (r):	ORIGIN = 0x2000F000, LENGTH = 4K
}

SECTIONS
{
	.vectors 0 :
	{
		*(.vectors)
	} >NVM

	.text :
	{
		*(.text)
		*(.rodata*)

	} >NVM

	.data :
	{
		_data_start = .;
		_data_lma_start = LOADADDR(.data);
		*(.data)
		. = ALIGN(4);
		*(.ramcode)
		_data_end = .;
	} >RAM AT>NVM

	.u_boot_cmd :
	{
		. = ALIGN(4);
		__u_boot_cmd_start = .;
		*(.u_boot_cmd)
		__u_boot_cmd_end = .;
	} >NVM

	.bss :
	{
		_bss_start = .;
		*(.bss)
		*(COMMON)
		_bss_end = .;
	} >RAM

	.stack :
	{
		armboot_start = .;
		*(.stack)
	} >STACK

	/DISCARD/ :
	{
		*(*)
	}
}
