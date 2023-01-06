OUTPUT_ARCH(m68k)
OUTPUT_FORMAT("binary")

MEMORY
{
    /* Operating system uses 128K of the 1M of static RAM */
    RAM (rwx) : ORIGIN = 0x00000400, LENGTH = 127K /* after the 1k byte of vectors */
    ROM (rx) : ORIGIN = 0x00F80000, LENGTH = 512K
}

/* stack location */
stack_size = 0x8000;

_stack_start = 0x00100000;
_stack_end = _stack_start - stack_size;

/* ram location */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM)+LENGTH(RAM);

SECTIONS
{
	.text :
	{
	    LONG (_stack_start) 	/* System Stack Pointer */
	    LONG (__start)		    /* Program start code */

        . = ALIGN(0x4);
		__s_text = . ;
        *(.text)                /* .text sections (code) */
        *(.text*)               /* .text* sections (code) */
		CONSTRUCTORS

        *(.rodata .rodata.*)

        . = ALIGN(0x4);
        *(.gcc_except_table)

        . = ALIGN(0x4);
        *(.eh_frame)

        KEEP (*(.init))
        KEEP (*(.fini))

		__e_text = . ;

	} > ROM

    .rodata :
    {
        . = ALIGN(4);
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } > ROM

    .bss :
	{
        . = ALIGN(0x4);
		__s_bss = . ; /* We should be able */
		*(.bss)
		*(COMMON)
        . = ALIGN(0x4);
		__e_bss = . ; /* to clear the bss. */
	} > RAM

    .data :
    {
        . = ALIGN(0x4);
		__s_data = . ; /* Symbols, to know where to */
        *(.got.plt) *(.got)
        *(.shdata)
        *(.data .data.*)
		__e_data = . ; /* copy the data.            */
        . = ALIGN(0x4);
    } > RAM AT > ROM
}
