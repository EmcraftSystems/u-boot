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

#define NVM_BASE	(CONFIG_MEM_NVM_BASE + CONFIG_MEM_NVM_UBOOT_OFF)
#define NVM_LEN		(CONFIG_MEM_NVM_LEN - CONFIG_MEM_NVM_UBOOT_OFF)

MEMORY
{
	Flash (rx): ORIGIN = (0x1C000000), LENGTH = ((16 * 1024 * 1024) - 0x10)
	Local_SRAM128 (rwx): ORIGIN = 0x10000000, LENGTH = (128*1024)
	RAM (rw):	ORIGIN = CONFIG_MEM_RAM_BASE, \
			LENGTH = CONFIG_MEM_RAM_LEN
	RAM_BUF (r):	ORIGIN = CONFIG_MEM_RAM_BASE + LENGTH(RAM), \
			LENGTH = CONFIG_MEM_RAM_BUF_LEN
	MALLOC (r):	ORIGIN = CONFIG_MEM_RAM_BASE + LENGTH(RAM) + LENGTH(RAM_BUF), \
			LENGTH = CONFIG_MEM_MALLOC_LEN
	STACK (r):	ORIGIN = CONFIG_MEM_RAM_BASE + LENGTH(RAM) + LENGTH(RAM_BUF) + LENGTH(MALLOC), \
			LENGTH = CONFIG_MEM_STACK_LEN
}

SECTIONS
{
	.vectors :
	{
		*(.vectors)
	} >Local_SRAM128 AT>Flash

	/*
	* Store contents of these two sections in the beginning of the U-Boot
	* image to ensure they are loaded by the Boot ROM of the LPC4350 MCU.
	*/
	.lpc18xx_image_top :
	{
	/* Global Section Table */
	    . = ALIGN(4) ;
	    __section_table_start = .;
	    LONG(LOADADDR(.vectors));
	    LONG(    ADDR(.vectors));
	    LONG(  SIZEOF(.vectors));
	    LONG(LOADADDR(.text));
	    LONG(    ADDR(.text));
	    LONG(  SIZEOF(.text));
	    LONG(LOADADDR(.u_boot_cmd));
	    LONG(    ADDR(.u_boot_cmd));
	    LONG(  SIZEOF(.u_boot_cmd));
	    LONG(LOADADDR(.data));
	    LONG(    ADDR(.data));
	    LONG(  SIZEOF(.data));
	    __section_table_end = . ;
	/* Global Section Table end */

	*(.lpc18xx_image_top_text)
	*(.lpc18xx_image_top_data)
	} >Flash
	.text :
	{
	_text_start = .;
	*(.text)
	*(.rodata*)
	_text_end = .;
	} >Local_SRAM128 AT>Flash
	.data :
	{
	_data_start = .;
	_data_lma_start = LOADADDR(.data);
	*(.data)
	. = ALIGN(4);
	*(.ramcode)
	_data_end = .;
	} >RAM AT>Flash
	.u_boot_cmd :
	{
	. = ALIGN(4);
	__u_boot_cmd_start = .;
	*(.u_boot_cmd)
	__u_boot_cmd_end = .;
	} >Local_SRAM128 AT>Flash
	.bss :
	{
	_bss_start = .;
	*(.bss)
	*(COMMON)
	_bss_end = .;
	} >RAM
	.stack :
	{
	*(.stack)
	} >STACK
	/* Has to be removed to keep .debug_* sections used for debugging
	* /DISCARD/ :
	{
	*(*)
	} */
	
}
_mem_nvm_base = ORIGIN(Flash);
_mem_nvm_size = LENGTH(Flash);
_mem_ram_base = ORIGIN(RAM);
_mem_ram_size = LENGTH(RAM);
_mem_ram_buf_base = ORIGIN(RAM_BUF);
_mem_ram_buf_size = LENGTH(RAM_BUF);
_mem_malloc_base = ORIGIN(MALLOC);
_mem_malloc_size = LENGTH(MALLOC);
_mem_stack_base = ORIGIN(STACK);
_mem_stack_size = LENGTH(STACK);
_mem_stack_end = ORIGIN(STACK) + LENGTH(STACK);