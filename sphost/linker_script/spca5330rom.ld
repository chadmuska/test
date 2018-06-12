OUTPUT_FORMAT("elf32-littlemips", "elf32-bigmips","elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(start)
PROVIDE(__stack = 0xd0002000);

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
 		_etext = . ;
		 PROVIDE (etext = .);
		 . = ALIGN(16);
	}

	.jcr . : {
		*(.jcr)
		. = ALIGN(16);
	}

	.ram_program . : {
		*(.ram_program)
		. = ALIGN(16);
	}

	.cri_sect . : {
		. = ALIGN(1024);
		. = . + 512;
		*(.cri_sect)
		. = ALIGN(16);
	}

	.eh_frame . : {
		*(.eh_frame)
		 . = ALIGN(16);
	}

	.init . : {
		*(.init)
		 . = ALIGN(16);
	}

	.fini . : {
		*(.fini)
		 . = ALIGN(16);
	}

	.ctors   . : {
		*(.ctors)
		 . = ALIGN(16);
	}
	.dtors   . : {
		*(.dtors)
		 . = ALIGN(16);
		 _dram_content_lma_start = . ;
	}
	_etext = . ;
	etext = . ;

	.rodata 0x800000a0 : AT ( LOADADDR(.dtors) + SIZEOF(.dtors) ) {
		_dataStart = . ;
		_data = . ;  /*code sentry failed: vfsOpen writes .rodata..*/
		_dram_content_vma_start = . ;
		/* FIC to support sif1dma, must go before .rodata */
		_fic_start = . ; . += 16 ;
		. = ALIGN(16);
		*(.rodata.sif1dma) /* for sensor serial DMA */
		*(.data.sif1dma.var) /* for sensor serial DMA */
		. = ALIGN(16);
		*(.rodata)
		*(.rodata.*) /* For GCC 3.4.x */
		. = ALIGN(16);
	}

	.data . : AT ( LOADADDR(.rodata) + SIZEOF(.rodata) ) {
		/*_data = . ;  *code sentry failed: vfsOpen writes .rodata..*/
		*(.data)
		_edata = . ;
		 . = ALIGN(16);
	}

	_gp = . + 0x8000 ;
	__global = _gp ;

	.sdata . : AT ( LOADADDR(.data) + SIZEOF(.data) ) {
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
	_bottomOfHeap = . ;
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
