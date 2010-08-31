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
