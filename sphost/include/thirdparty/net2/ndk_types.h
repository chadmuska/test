/**************************************************************************
 *
 *       Copyright (c) 2011 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/

#ifndef __NDK_API_TYPES_H__
#define __NDK_API_TYPES_H__

#include <limits.h>
#include <../inc/common.h>

#ifndef __LITTLE_ENDIAN
# define __LITTLE_ENDIAN        1234
#endif

#ifndef __BIG_ENDIAN
# define __BIG_ENDIAN           4321
#endif

#ifndef __BYTE_ORDER
# define __BYTE_ORDER           __LITTLE_ENDIAN
#endif

typedef SINT8                   __s8;
typedef UINT8                   __u8;
typedef SINT16                  __s16;
typedef UINT16                  __u16;
typedef SINT32                  __s32;
typedef UINT32                  __u32;
typedef SINT64                  __s64;
typedef UINT64                  __u64;

typedef SINT8                   s8;
typedef UINT8                   u8;
typedef SINT16                  s16;
typedef UINT16                  u16;
typedef SINT32                  s32;
typedef UINT32                  u32;
typedef SINT64                  s64;
typedef UINT64                  u64;

typedef UINT8                   u8_t;
typedef SINT8                   s8_t;
typedef UINT16                  u16_t;
typedef SINT16                  s16_t;
typedef UINT32                  u32_t;
typedef SINT32                  s32_t;

typedef __u16                   __le16;
typedef __u16                   __be16;
typedef __u32                   __le32;
typedef __u32                   __be32;
typedef __u64                   __le64;
typedef __u64                   __be64;
typedef __u16                   __sum16;
typedef __u32                   __wsum;

typedef unsigned int            size_t;
typedef unsigned int            uint_t;
typedef long long               loff_t;

#ifndef IFNAMSIZ
# define IFNAMSIZ		16
#endif

#ifndef IFALIASZ
# define IFALIASZ		256
#endif

#ifndef IF_NAMESIZE
# define IF_NAMESIZE		IFNAMSIZ
#endif

#ifndef IF_HWADDRSIZE
# define IF_HWADDRSIZE		14
#endif

#endif
