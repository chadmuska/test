#if RAM_4GBIT_SUPPORT
# define ETEXT_KSEG0_ADDR    _large_ram_etext_kseg0_addr = . ;
# define ETEXT_KUSEG_JUMP    _large_ram_etext_kseg0_addr - 0x40000000
# define ETEXT_KUSEG_ADDR    _large_ram_data_kuseg_addr = . ;
#else  /*#if RAM_4GBIT_SUPPORT*/
# define ETEXT_KSEG0_ADDR
# define ETEXT_KUSEG_JUMP    .
# define ETEXT_KUSEG_ADDR
#endif /*#if RAM_4GBIT_SUPPORT*/

#define SECTION_PRECEDED_BY(sec)  AT ( LOADADDR(sec) + SIZEOF(sec) )

#if I_CACHE_LINE_SIZE % D_CACHE_LINE_SIZE == 0
# define CACHE_LINE_SIZE_LCM  I_CACHE_LINE_SIZE
#elif D_CACHE_LINE_SIZE % I_CACHE_LINE_SIZE == 0
# define CACHE_LINE_SIZE_LCM  D_CACHE_LINE_SIZE
#else
# error I_CACHE_LINE_SIZE and D_CACHE_LINE_SIZE not divisible
#endif


OUTPUT_FORMAT("elf32-littlemips", "elf32-bigmips","elf32-littlemips")
OUTPUT_ARCH(mips)
ENTRY(start)
PROVIDE(__stack = SYS_STACK_BOT);

SECTIONS
{
	.rom 0x9fc00000 : {
		_rom = . ;
		*(.reset)
		_rom_end = . ;
#if ISR_ON_IMEM
		*os_wrap_irq.o(.text .text.*)
		*tx_tic.o(.text .text.*)
		*tx_except.o(.text .text.*)
#endif /*#if ISR_ON_IMEM*/
		*(.boot)
		_erom = .;
	}

	/* CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE */
	.text . : {
		_text = . ;
		*(.text .text.*)
		*(.gnu.linkonce.t.*)
		_etext = . ;
		PROVIDE (etext = .);
	}

	/* CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE */
	.init . : {
		*(.init)
	}

	/* CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE */
	.fini . : {
		*(.fini)
		. = ALIGN(CACHE_LINE_SIZE_LCM);
	}
	. = ALIGN(CACHE_LINE_SIZE_LCM);

	.ram_program . : {
		*(.ram_program)
	}

	.cri_sect . : {
		/*
		. = ALIGN(1024);
		. = . + 512;
		*/
		*(.cri_sect)
	}

	/* CONTENTS, ALLOC, LOAD, READONLY, DATA */
	.gcc_except_table . : {
		*(.gcc_except_table .gcc_except_table.*)
		/* ALIGN(..) makes "SIZE(.gcc_except_table)" incorrect */
	}

	/* CONTENTS, ALLOC, LOAD, READONLY, DATA */
	.eh_frame_hdr . : {
		*(.eh_frame_hdr)
	}

	/* CONTENTS, ALLOC, LOAD, READONLY, DATA */
	.eh_frame . : {
		*(.eh_frame)
		. = ALIGN(CACHE_LINE_SIZE_LCM);
	}
	. = ALIGN(CACHE_LINE_SIZE_LCM);
	_dram_content_lma_start = . ;

	.except 0x80000000 : SECTION_PRECEDED_BY(.eh_frame) {
		_dram_content_vma_start = . ;
		_except = . ;
		*(.exceptRam)
		. = _except + 0x80;
		*(.bootParam)
		. = _except + 0x100;
		*(.exceptCacheErr)
		. = _except + 0x120;
		*(.exceptRom)
		. = _except + 0x180;
		*(.exceptGen)
		*(.exceptRomLarge)
		_eexcept = . ;
		. = ALIGN(CACHE_LINE_SIZE_LCM);
		*(.signature)
		. = ALIGN(D_CACHE_LINE_SIZE);
	}
	. = ALIGN(D_CACHE_LINE_SIZE);

	ETEXT_KSEG0_ADDR

	/* CONTENTS, ALLOC, LOAD, DATA */
	.ctors ETEXT_KUSEG_JUMP : SECTION_PRECEDED_BY(.except) {
		ETEXT_KUSEG_ADDR

		_data = . ;  /*code sentry failed: .data.sif1dma.var*/
		*(.ctors)
	}

	/* CONTENTS, ALLOC, LOAD, DATA */
	.dtors . : SECTION_PRECEDED_BY(.ctors) {
		*(.dtors)
	}

	/* CONTENTS, ALLOC, LOAD, DATA */
	.jcr . : SECTION_PRECEDED_BY(.dtors) {
		*(.jcr)
	}

	/* CONTENTS, ALLOC, LOAD, READONLY, DATA */
	/* zero addresses */
	.rel.dyn . : SECTION_PRECEDED_BY(.jcr) {
		*(.rel.dyn) *(.rel*)
		. = ALIGN(D_CACHE_LINE_SIZE);
	}
	. = ALIGN(D_CACHE_LINE_SIZE);

	/* CONTENTS, ALLOC, LOAD, READONLY, DATA */
	.rodata . : SECTION_PRECEDED_BY(.rel.dyn) {
		_rodata = . ;
		/* FIC to support sif1dma, must go before .rodata */
		_fic_start = . ; . += 32 ;
		. = ALIGN(D_CACHE_LINE_SIZE);
		*(.rodata.sif1dma) /* for sensor serial DMA */
		*(.data.sif1dma.var) /* for sensor serial DMA */
		_fic_load_end = . ;
		. = ALIGN(D_CACHE_LINE_SIZE);
		*(.rodata)
		*(.rodata.*) /* For GCC 3.4.x */
		*(.gnu.linkonce.r*)
		. = ALIGN(D_CACHE_LINE_SIZE);
		_erodata = . ;
	}
	. = ALIGN(D_CACHE_LINE_SIZE);

	/* CONTENTS, ALLOC, LOAD, DATA */
	.data . : SECTION_PRECEDED_BY(.rodata) {
		_dataStart = . ;
		/*_data = . ;  *code sentry failed: vfsOpen writes .rodata..*/
		*(.data .data.*)
		_edata = . ;
		. = ALIGN(D_CACHE_LINE_SIZE);
	}
	. = ALIGN(D_CACHE_LINE_SIZE);

	_gp = . + 0x8000 ;
	__global = _gp ;

	/* CONTENTS, ALLOC, LOAD, DATA */
	.sdata . : SECTION_PRECEDED_BY(.data) {
		*(.sdata .sdata.*)
		*(.gnu.linkonce.s*)
		. = ALIGN(D_CACHE_LINE_SIZE);
	}
	. = ALIGN(D_CACHE_LINE_SIZE);

	/* ALLOC */
	.sbss . : SECTION_PRECEDED_BY(.sdata) {
		_bssStart = . ;
		*(.scommon .sbss .sbss.*)
		. = ALIGN(D_CACHE_LINE_SIZE);
	}
	. = ALIGN(D_CACHE_LINE_SIZE);

	/* ALLOC */
	.bss . : SECTION_PRECEDED_BY(.sbss) {
		*(.bss .bss.* COMMON comm)
		. = ALIGN(D_CACHE_LINE_SIZE);
		_bssEnd = . ;
	}
	. = ALIGN(D_CACHE_LINE_SIZE);
	_bottomOfHeap = . ;

	/* CONTENTS, READONLY, LINK_ONCE_SAME_SIZE */
	.reginfo . : SECTION_PRECEDED_BY(.bss) {
		*(.reginfo)
	}

	/* CONTENTS, READONLY */
	.mdebug . : SECTION_PRECEDED_BY(.reginfo) {
		*(.mdebug .mdebug.*)
	}

	/* CONTENTS, READONLY */
	.pdr . : SECTION_PRECEDED_BY(.mdebug) {
		*(.pdr)
	}

	/* CONTENTS, READONLY, DEBUGGING */
	.stab . : SECTION_PRECEDED_BY(.pdr) {
		*(.stab*)
	}
}
