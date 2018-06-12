/**************************************************************************
 *
 *       Copyright (c) 2004,2005,2006-2016 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch
 *  Technology, Inc.. All rights are reserved by iCatch Technology, Inc..
 *  This software may only be used in accordance with the corresponding
 *  license agreement. Any unauthorized use, duplication, distribution,
 *  or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology, Inc..
 *
 *  iCatch Technology, Inc. reserves the right to modify this software
 *  without notice.
 *
 *  iCatch Technology, Inc.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *                                                                        *
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */
#ifndef _OS_BMAP_H_
#define _OS_BMAP_H_

#ifdef __cplusplus
extern "C" {
#endif


#if __mips
#  define _OS_IDX_SECTION    __attribute__ ((section(".sdata")))
# if MEM_TOO_SMALL
#  define _OS_BMAP_SECTION   __attribute__ ((section(".rodata")))
# else
#  define _OS_BMAP_SECTION   __attribute__ ((section(".sdata")))
# endif
#else  /*#if __mips*/
#  define _OS_IDX_SECTION
#  define _OS_BMAP_SECTION
#endif /*#if __mips*/


#define _os_bitmap_msb_to_idx  __clz_tab

extern const unsigned char _os_idx_to_bitmap[] _OS_IDX_SECTION;
extern const unsigned char __clz_tab[] _OS_BMAP_SECTION;
extern const unsigned char _os_bitmap_lsb_to_idx[] _OS_BMAP_SECTION;


/**
 * \return bit width of non-zero bits of value t, 0 if t==0
 */
#if __mips >= 32 || __arm__

/* MIPS32, ARMv5E or later has "count leading zero" instruction */
__inline__ static unsigned int u32bitWidthGet(unsigned int t)
		__attribute__((always_inline));
unsigned int
u32bitWidthGet(
		unsigned int t
		)
{
	unsigned int r;
	__asm__ ("clz	%0, %1" : "=r" (r) : "r" (t));
	return 32 - r;
}

#define u16bitWidthGet  u32bitWidthGet
#define u8bitWidthGet  u32bitWidthGet

#else  /*#if __mips >= 32 || __arm__*/

extern unsigned int u16bitWidthGet(unsigned int t);
extern unsigned int u32bitWidthGet(unsigned int t);

__inline__ static unsigned int u8bitWidthGet(unsigned int t)
		__attribute__((always_inline));
unsigned int
u8bitWidthGet(
		unsigned int t
		)
{
	return _os_bitmap_msb_to_idx[t];
}

#endif /*#if __mips >= 32 || __arm__*/


/**
 * \return the least significant non-zero bit position of value t, 0 if t==0
 */
#if __mips >= 32 && CPU_CORE_USE_DSP

/* MIPS32, DSP ASE "clz" and "bitrev" instructions */
__inline__ static unsigned int u16Lsb1PosGet(unsigned int t)
		__attribute__((always_inline));
unsigned int
u16Lsb1PosGet(
		unsigned int t
		)
{
	unsigned int r = 16;
	if (t != 0) {
		__asm__ (".set	dsp\n"
		"	bitrev	%0, %1\n"
		"	clz	%0, %0" : "=r" (r) : "r" (t));
	}
	return r - 16;
}

#define u8Lsb1PosGet  u16Lsb1PosGet

#else  /*#if __mips >= 32 && CPU_CORE_USE_DSP*/

__inline__ static unsigned int u8Lsb1PosGet(unsigned int t)
		__attribute__((always_inline));
unsigned int
u8Lsb1PosGet(
		unsigned int t
		)
{
	return _os_bitmap_lsb_to_idx[t];
}

# if __mips >= 32 || __arm__
#  define u16Lsb1PosGet  u32Lsb1PosGet
# else  /*#if __mips >= 32 || __arm__*/
extern unsigned int u16Lsb1PosGet(unsigned int t);
# endif /*#if __mips >= 32 || __arm__*/

#endif /*#if __mips >= 32 && CPU_CORE_USE_DSP*/

#if __mips >= 32 || __arm__

/* MIPS32, ARMv5E or later has "count leading zero" instruction */
__inline__ static unsigned int u32Lsb1PosGet(unsigned int t)
		__attribute__((always_inline));
unsigned int
u32Lsb1PosGet(unsigned int t)
{
	unsigned int r = 31;
	if (t != 0) {
		/* extracting the least significant set bit */
		unsigned int x = t & -t; /* t ^ (t - 1) */
		__asm__ ("clz   %0, %1" : "=r" (r) : "r" (x));
	}
	return 31 - r;
}

#else  /*#if __mips >= 32 || __arm__*/

extern unsigned int u32Lsb1PosGet(unsigned int t);

#endif /*#if __mips >= 32 || __arm__*/


#ifdef __cplusplus
}
#endif

#endif
