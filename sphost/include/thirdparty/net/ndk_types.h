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
 *  Author: Andy.Li
 *
 **************************************************************************/

#ifndef __NDK_API_TYPES_H__
#define __NDK_API_TYPES_H__

#include <../inc/common.h>

#ifdef __cplusplus
	#define NDK_EXTERN_C extern "C"
#else
	#define NDK_EXTERN_C
#endif

#ifndef __LITTLE_ENDIAN
# define __LITTLE_ENDIAN        1234
#endif

#ifndef __BIG_ENDIAN
# define __BIG_ENDIAN           4321
#endif

#ifndef __BYTE_ORDER
# define __BYTE_ORDER           __LITTLE_ENDIAN
#endif

typedef SINT8 		__s8;
typedef UINT8 		__u8;
typedef SINT16		__s16;
typedef UINT16		__u16;
typedef SINT32		__s32;
typedef UINT32		__u32;
typedef SINT64		__s64;
typedef UINT64		__u64;

typedef SINT8 		s8;
typedef UINT8 		u8;
typedef SINT16		s16;
typedef UINT16		u16;
typedef SINT32		s32;
typedef UINT32		u32;
typedef SINT64		s64;
typedef UINT64		u64;

typedef UINT8		u8_t;
typedef SINT8		s8_t;
typedef UINT16		u16_t;
typedef SINT16		s16_t;
typedef UINT32		u32_t;
typedef SINT32		s32_t;

typedef __u16		__le16;
typedef __u16		__be16;
typedef __u32		__le32;
typedef __u32		__be32;
typedef __u64		__le64;
typedef __u64		__be64;
typedef __u16		__sum16;
typedef __u32		__wsum;

typedef unsigned int	size_t;
typedef unsigned int	uint_t;
typedef long long	loff_t;

typedef UINT8		uint8_t;
typedef UINT16		uint16_t;
typedef UINT32		uint32_t;
typedef UINT64		uint64_t;
typedef SINT8 		int8_t;
typedef SINT16		int16_t;
typedef SINT32		int32_t;
typedef	SINT64		int64_t;

#ifndef __cplusplus
typedef char            bool;
#endif

#define NDK_ARRAY_SIZE(a)       (sizeof(a)/sizeof((a)[0]))

#ifndef INT64_MIN
# define INT64_MIN              (-0x7fffffffffffffffLL - 1)
#endif

#ifndef INT_MAX
# define INT_MAX                ((int)(~0U>>1))
#endif

#ifndef INT_MIN
# define INT_MIN                (-INT_MAX - 1)
#endif

#ifndef USHRT_MAX
# define USHRT_MAX              (~((unsigned short)0))
#endif

#ifndef UCHAR_MAX
# define UCHAR_MAX              (~((unsigned char)0))
#endif

#define	IFNAMSIZ                16
#define	IFALIASZ                256
#define IF_NAMESIZE             IFNAMSIZ
#define IF_HWADDRSIZE           14

/*
 * Driver Types.
 */
#define NET_DRIVER_NONE         1 << 0
#define NET_DRIVER_CSR          1 << 1
#define NET_DRIVER_MTK          1 << 2
#define NET_DRIVER_RTK          1 << 3
#define NET_DRIVER_BCM          1 << 4
#define NET_DRIVER_ALL          1 << 5

NDK_EXTERN_C unsigned netdev_get_drv_type();

#ifndef SPCA6350
# define SP5K_EVENT_FLAGS_GROUP TX_EVENT_FLAGS_GROUP
# define SP5K_QUEUE TX_QUEUE
# define SP5K_MUTEX TX_MUTEX
# define SP5K_SEMAPHORE TX_SEMAPHORE
# define SP5K_THREAD TX_THREAD
#endif

#endif
