OUTPUT_FORMAT("elf32-littlemips", "elf32-bigmips","elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(start)
PROVIDE(__stack = 0xd0001ffc);

SECTIONS
{
	.rom 0x9fc00000 : {
		*(.reset)
		. = ALIGN(0x80);
		*(.exceptRam)
		. = ALIGN(0x100);
		. += 0x80;
		*(.exceptRom)
		*(.boot)
		. = ALIGN(16);
		_erom = .;
	}

	.text . : {
		_text = . ;
 		*(.text)
		 . = ALIGN(16);
		*(.rodata)
		*(.rodata.*) /* For GCC 3.4.x */
		 . = ALIGN(16);
		 _dram_content_lma_start = . ;
	}

	.data 0xd0000000 : AT ( LOADADDR(.text) + SIZEOF(.text) ) {
		_dram_content_vma_start = . ;
		*(.data)
		 . = ALIGN(16);
	}

	.jcr . : AT ( LOADADDR(.data) + SIZEOF(.data) ) {
		*(.jcr)
		. = ALIGN(16);
	}

	.ram_program . : AT ( LOADADDR(.jcr) + SIZEOF(.jcr) ) {
		*(.ram_program)
		. = ALIGN(16);
	}

	.eh_frame . : AT ( LOADADDR(.ram_program) + SIZEOF(.ram_program) ) {
		*(.eh_frame)
		 . = ALIGN(16);
	}

	.init . : AT ( LOADADDR(.eh_frame) + SIZEOF(.eh_frame) ) {
		*(.init)
		 . = ALIGN(16);
	}

	.fini . : AT ( LOADADDR(.init) + SIZEOF(.init) ) {
		*(.fini)
		 . = ALIGN(16);
	}

	.ctors   . : AT ( LOADADDR(.fini) + SIZEOF(.fini) ) {
		*(.ctors)
		 . = ALIGN(16);
	}
	.dtors   . : AT ( LOADADDR(.ctors) + SIZEOF(.ctors) ) {
		*(.dtors)
		 . = ALIGN(16);
	}

	_gp = . + 0x8000 ;

	.sdata . : AT ( LOADADDR(.dtors) + SIZEOF(.dtors) ) {
		*(.sdata)
		. = ALIGN(16);
	}
	.sbss . : AT ( LOADADDR(.sdata) + SIZEOF(.sdata) ) {
		_bssStart = . ;
		*(.scommon)
		*(.sbss)
		. = ALIGN(16);
	}
	.bss . : AT ( LOADADDR(.sbss) + SIZEOF(.sbss) ) {
		*(.bss)
		*(COMMON)
		*(comm)
		 . = ALIGN(16);
		_bssEnd = . ;
	}

	.reginfo . : AT ( LOADADDR(.bss) + SIZEOF(.bss) ) {
		*(.reginfo)
	}
	.mdebug . : AT ( LOADADDR(.reginfo) + SIZEOF(.reginfo) ) {
		*(.mdebug)
	}
	.mdebug.abi32 . : AT ( LOADADDR(.mdebug) + SIZEOF(.mdebug) ) {
		*(.mdebug.abi32)
	}
	.pdr . : AT ( LOADADDR(.mdebug.abi32) + SIZEOF(.mdebug.abi32) ) {
		*(.pdr)
	}
	.stab . : AT ( LOADADDR(.pdr) + SIZEOF(.pdr) ) {
		*(.stab*)
	}

}
