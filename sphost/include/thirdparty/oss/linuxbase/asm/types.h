/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994, 1995, 1996, 1999 by Ralf Baechle
 * Copyright (C) 1999 Silicon Graphics, Inc.
 */
#ifndef _ASM_TYPES_H
#define _ASM_TYPES_H

#ifndef NULL
#define NULL  ((void *)0)
#endif

#define TRUE   1
#define FALSE  0


typedef signed char   		s8_t;
typedef unsigned char 		u8_t;
typedef signed short 		s16_t;
typedef unsigned short 		u16_t;
typedef signed int 			s32_t;
typedef unsigned int 		u32_t;
typedef long long 			s64_t;
typedef unsigned long long u64_t;

typedef u8_t		uint8_t;
typedef u16_t		uint16_t;
typedef u32_t		uint32_t;
typedef u64_t		uint64_t;
typedef s8_t 		int8_t;
typedef s16_t		int16_t;
typedef s32_t		int32_t;
typedef s64_t		int64_t;

typedef signed char 			__s8;
typedef unsigned char 		__u8;
typedef signed short 		__s16;
typedef unsigned short 		__u16;
typedef signed int 			__s32;
typedef unsigned int 		__u32;
typedef signed long long 	__s64;
typedef unsigned long long __u64;

typedef signed char 			s8;
typedef unsigned char 		u8;
typedef signed short 		s16;
typedef unsigned short 		u16;
typedef signed int 			s32;
typedef unsigned int 		u32;
typedef signed long long 	s64;
typedef unsigned long long u64;

/* we are little endian */
typedef unsigned short 		__le16;
typedef unsigned int 		__le32;
typedef unsigned long long __le64;

typedef unsigned short 		__be16;
typedef unsigned int 		__be32;
typedef unsigned long long __be64;

typedef __u16		__sum16;
typedef __u32		__wsum;

typedef unsigned int size_t;
typedef int 			ssize_t;
typedef unsigned int	uint_t;
typedef long long		loff_t;

#ifndef __cplusplus
typedef char			bool;
#endif

#define BITS_PER_LONG	32
#define BITS_PER_BYTE	8

#define TX_ADDRWORD u32
#define TX_ADDRESS u32

/* for dma */
typedef u32 dma_addr_t;

typedef TX_ADDRWORD handle_t;
typedef TX_ADDRWORD addrword_t;
typedef TX_ADDRESS address_t;

#define U8_MAX			((u8)~0U)
#define S8_MAX			((s8)(U8_MAX>>1))
#define S8_MIN			((s8)(-S8_MAX - 1))
#define U16_MAX			((u16)~0U)
#define S16_MAX			((s16)(U16_MAX>>1))
#define S16_MIN			((s16)(-S16_MAX - 1))
#define U32_MAX			((u32)~0U)
#define S32_MAX			((s32)(U32_MAX>>1))
#define S32_MIN			((s32)(-S32_MAX - 1))
#define U64_MAX			((u64)~0ULL)
#define S64_MAX			((s64)(U64_MAX>>1))
#define S64_MIN			((s64)(-S64_MAX - 1))

#endif /* _ASM_TYPES_H */
