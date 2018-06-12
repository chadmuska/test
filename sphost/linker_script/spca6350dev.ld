#if RAM_4GBIT_SUPPORT
# define SECTION_PRECEDED_BY(sec)  AT ( LOADADDR(sec) + SIZEOF(sec) )
# define _KUSEG(x)           ((x) - 0x40000000)
# define ETEXT_KSEG0_ADDR    _large_ram_etext_kseg0_addr = . ;
# define ETEXT_KUSEG_JUMP    _KUSEG(_large_ram_etext_kseg0_addr)
# define ETEXT_KUSEG_ADDR    _large_ram_data_kuseg_addr = . ;
#else  /*#if RAM_4GBIT_SUPPORT*/
# define SECTION_PRECEDED_BY(sec)
# define _KUSEG(x)           (x)
# define ETEXT_KSEG0_ADDR
# define ETEXT_KUSEG_JUMP    .
# define ETEXT_KUSEG_ADDR
#endif /*#if RAM_4GBIT_SUPPORT*/

#if MULTI_BOOT
# define CAT(p,d)  p##d.o
# define BKDOOR_LIB_PREFIX(dev) CAT(LIB_PATH/bkdoor_, dev)
# define MULTI_BOOT_BKDOOR_DEV  BKDOOR_LIB_PREFIX(BOOT_DEV)
#endif /*#if MULTI_BOOT*/

#ifndef RAM_MAP_OFFSET
# define RAM_MAP_OFFSET 0
#endif /*#ifndef RAM_MAP_OFFSET*/

#ifndef I_CACHE_LINE_SIZE
#define I_CACHE_LINE_SIZE 16
#endif
#ifndef D_CACHE_LINE_SIZE
#define D_CACHE_LINE_SIZE 16
#endif

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
	.rom 0x80000000 + RAM_MAP_OFFSET : {
		_rom = . ;
		_except = . ;
		*(.reset)
		. = _except + 0x80;
		*(.bootParam)
		. = _except + 0x100;
		*(.exceptCacheErr)
		. = _except + 0x120;
		*(.exceptRom)
		. = _except + 0x180;
		*(.exceptGen)
		. = ALIGN(4);
		*(.exceptRam)
		. = ALIGN(4);
		*(.exceptRomLarge)
		_eexcept = . ;
#if MULTI_BOOT
		*bkdoor_api.o(.text .text.*)
		MULTI_BOOT_BKDOOR_DEV(.text .text.*)
		*(.text.mask)
#endif /*#if MULTI_BOOT*/
		_rom_end = . ;
		. = ALIGN(CACHE_LINE_SIZE_LCM);
		*(.signature)
		. = ALIGN(CACHE_LINE_SIZE_LCM);
#if MULTI_BOOT
		*bkdoor_api.o(.rodata .rodata.*)
		MULTI_BOOT_BKDOOR_DEV(.rodata .rodata.*)
		*bkdoor_api.o(.data .data.*)
		MULTI_BOOT_BKDOOR_DEV(.data .data.*)
		*bkdoor_api.o(.bss .bss.* COMMON comm)
		MULTI_BOOT_BKDOOR_DEV(.bss .bss.* COMMON comm)
		. = ALIGN(D_CACHE_LINE_SIZE);
#endif /*#if MULTI_BOOT*/
		_bkdoor_block_limit = . ;
#if MULTI_BOOT
# if ASSERT_BKDOOR_SIZE_16KB
		ASSERT(. - _rom <= 16 * 1024, "bkdoor size > 16KB");
# endif /*#if ASSERT_BKDOOR_SIZE_16KB*/
		. = ALIGN(D_CACHE_LINE_SIZE);
		*(.bss.mask)
		. = ALIGN(CACHE_LINE_SIZE_LCM);

		_multiboot_start = . ;
#endif /*#if MULTI_BOOT*/
#if ISR_ON_IMEM
# if MULTI_BOOT
		. = ALIGN(8192);
		_text_isr_on_imem = . ;
# endif /*#if MULTI_BOOT*/
		*os_wrap_irq.o(.text .text.*)
		*tx_tic.o(.text .text.*)
		*tx_except.o(.text .text.*)
#endif /*#if ISR_ON_IMEM*/
		*(.boot)
		. = ALIGN(I_CACHE_LINE_SIZE);
#if ISR_ON_IMEM
# if MULTI_BOOT
		. = . >= _text_isr_on_imem + 4096 ? . : ALIGN(4096);
# else  /*#if MULTI_BOOT*/
		. = . >= _except + 4096 ? . : ALIGN(4096);
# endif /*#if MULTI_BOOT*/
#endif /*#if ISR_ON_IMEM*/
		_text_hibernate_vcore = . ;
		*(.text.hibernate.vcore)
		. = ALIGN(I_CACHE_LINE_SIZE);
		_etext_hibernate_vcore = . ;
		_erom = . ;
	}
	. = ALIGN(I_CACHE_LINE_SIZE);

	/* CONTENTS, ALLOC, LOAD, RELOC, READONLY, CODE */
	.text . : {
#ifdef MIPS24K_ERRATA_E156_J_EXC_INV_EPC_HACK
		/*E156 j/jal IRQ EPC error hack*/
		. = . >= 0x80008000 ? . : ALIGN(32*1024);
#endif /*#ifdef MIPS24K_ERRATA_E156_J_EXC_INV_EPC_HACK*/
		_text = . ;
		*(.text .text.*)
		*(.gnu.linkonce.t.*)
		_etext = . ;
		PROVIDE (etext = .);
	}

#if RAM_PARAM_FIND_IMEM
	.dramParamImem . : {
# if MULTI_BOOT || ISR_ON_IMEM
		ASSERT(0, "isrimem/multiboot not supporting ram param find on imem");
# endif /*# if MULTI_BOOT || ISR_ON_IMEM*/
		. = ALIGN(8192);
		_dramParamFindStart = . ;
		*(.dramParamImem.align)
		*(.dramParamImem)
		. = ALIGN(I_CACHE_LINE_SIZE);
		_dramParamFindEnd = . ;
		ASSERT(_dramParamFindEnd - _dramParamFindStart <= 4096, "imem section too large");
	}
	. = ALIGN(I_CACHE_LINE_SIZE);
#endif /*#if RAM_PARAM_FIND_IMEM*/

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
		. = ALIGN(D_CACHE_LINE_SIZE);
	}
	. = ALIGN(D_CACHE_LINE_SIZE);

	ETEXT_KSEG0_ADDR

	/* CONTENTS, ALLOC, LOAD, DATA */
	.ctors ETEXT_KUSEG_JUMP : SECTION_PRECEDED_BY(.eh_frame) {
		ETEXT_KUSEG_ADDR

		_data = . ;  /*code sentry failed: .data.sif1dma.var*/
		__init_array_start = .;
		KEEP(*(.ctors))
		__init_array_end = .;
	}

	/* CONTENTS, ALLOC, LOAD, DATA */
	.dtors . : SECTION_PRECEDED_BY(.ctors) {
		*(.dtors)
	}

#if REL_DYN_SECTION_SUPPORT
# define RODATA_PRECEDED_SECTION  .rel.dyn
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
#else  /*#if REL_DYN_SECTION_SUPPORT*/
# define RODATA_PRECEDED_SECTION  .jcr
/* MIDE not supporting .rel.dyn section in linker script */
	/* CONTENTS, ALLOC, LOAD, DATA */
	.jcr . : SECTION_PRECEDED_BY(.dtors) {
		*(.jcr)
		. = ALIGN(D_CACHE_LINE_SIZE);
	}
#endif /*#if REL_DYN_SECTION_SUPPORT*/

	. = ALIGN(D_CACHE_LINE_SIZE);

	/* CONTENTS, ALLOC, LOAD, READONLY, DATA */
	.rodata . : SECTION_PRECEDED_BY(RODATA_PRECEDED_SECTION) {
		_rodata = . ;
		/* FIC to support sif1dma, must go before .rodata */
		_fic_start = . ; . += D_CACHE_LINE_SIZE ;
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
#if ASSERT_IMAGE_SIZE_8MB
		ASSERT(_bssStart - _KUSEG(_rom) <= 0x800000, "> 8MB on rev A");
#endif /*#if ASSERT_IMAGE_SIZE_8MB*/
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
		*(.mdebug)
	}

	/* CONTENTS, READONLY */
	.mdebug.abi32 . : SECTION_PRECEDED_BY(.mdebug) {
		*(.mdebug.abi32)
	}

	/* CONTENTS, READONLY */
	.pdr . : SECTION_PRECEDED_BY(.mdebug.abi32) {
		*(.pdr)
	}

	/* CONTENTS, READONLY, DEBUGGING */
	.stab . : SECTION_PRECEDED_BY(.pdr) {
		*(.stab*)
	}
}
