ENTRY(_mod_entry)
SECTIONS
{
	.text :
	{
		*(.text*)
		*(.rodata)
	}
	.data :
	{
		*(.data*)
	}
	.bss :
	{
		*(.bss*)
	}
}
