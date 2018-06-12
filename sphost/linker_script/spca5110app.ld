OUTPUT_FORMAT("elf32-littlemips", "elf32-bigmips","elf32-littlemips")
OUTPUT_ARCH(mips)
/*ENTRY(AppMain)*/
PROVIDE(__stack = 0x81fffffc);

SECTIONS
{
	.text 0x81F6A000 : {
		_text = . ;
		*(.boot)
 		*(.text)
		 PROVIDE (etext = .);
		 . = ALIGN(16);
		 _dataStart = . ;
	}

	.data . : AT ( LOADADDR(.text) + SIZEOF(.text) ) {
		_data = . ;
		*(.data)
		_edata = . ;
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

	.cri_sect . : AT ( LOADADDR(.ram_program) + SIZEOF(.ram_program) ) {
		. = ALIGN(1024);
		. = . + 512;
		*(.cri_sect)
		 . = ALIGN(16);
	}

	.eh_frame . : AT ( LOADADDR(.cri_sect) + SIZEOF(.cri_sect) ) {
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
	_etext = . ;
	etext = . ;

	.rodata . : AT ( LOADADDR(.dtors) + SIZEOF(.dtors) ) {
		*(.rodata)
		*(.rodata.*) /* For GCC 3.4.x */
		. = ALIGN(16);
	}

	_gp = . + 0x8000 ;
	__global = _gp ;

	.sdata . : AT ( LOADADDR(.rodata) + SIZEOF(.rodata) ) {
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
		 . = ALIGN(16);
		_bssEnd = . ;
	}
/*	.rel.dyn . : AT ( LOADADDR(.rodata) + SIZEOF(.rodata) ) { *(.rel.dyn) } = 0*/

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
