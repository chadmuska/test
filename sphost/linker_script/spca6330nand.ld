OUTPUT_FORMAT("elf32-littlemips", "elf32-bigmips","elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(start)
PROVIDE(__stack = 0xd0002000);

SECTIONS
{
	.rom 0x80000000 : {
		*(.reset)
		. = ALIGN(0x80);
		*(.exceptRam)
		. = ALIGN(0x100);
/*
		. += 0x80;
*/

		*(.bootParam)
		. = ALIGN(0x80);

		*(.exceptRom)
		. = ALIGN(0x80);

		*(.signature)
		_bkdoor_block_limit = . ;
		. = ALIGN(0x80);

		*(.boot)
		. = ALIGN(16);
		_dram_content_lma_start = . ;
	}

	.text . : AT ( LOADADDR(.rom) + SIZEOF(.rom) ) {
		_text = . ;
		_dram_content_vma_start = _text;
 		*(.text)
 		_etext = . ;
		 PROVIDE (etext = .);
		 . = ALIGN(16);
	}

	.jcr . : AT ( LOADADDR(.text) + SIZEOF(.text) ) {
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
		_dataStart = . ;
		_data = . ;  /*code sentry failed: vfsOpen writes .rodata..*/
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
		*(.stab)
	}
}
