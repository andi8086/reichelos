SECTIONS
{
	. = 0x100000;
	.boot : { *(.multiboot_header) }
	.text : { 
		*(.text)
	}

	.rodata : {
		*(.rodata*)
	}
	
	.data : {
		*(.data*)
	}

	PROVIDE( _LOAD_END_ = . );

	.bss :
	{
		_BSS_START_ = . ;
		*(.bss .bss.* .gnu.linkonce.b.*)
		*(COMMON)
		_BSS_END_ = . ;
	}
	
	. = ALIGN(0x10);
	_end = . ;

}
