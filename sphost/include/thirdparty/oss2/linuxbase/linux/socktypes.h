/**************************************************************************
 *
 *       Copyright (c) 2006-2015 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
#ifndef __LINUX_SOCKTYPES__
#define __LINUX_SOCKTYPES__

#include <asm/types.h>

#ifndef TCPIP_INET_SOCKTYPES
#define TCPIP_INET_SOCKTYPES

#include <uapi/linux/ip.h>

/*
 * Macros
 */
#define SOCK_STREAM		1
#define SOCK_DGRAM		2
#define SOCK_RAW		3
#define SOCK_SEQPACKET		5

/*
 * Types
 */
typedef u16			sa_family_t;
typedef u32			socklen_t;

struct in_addr {
	u32			s_addr;
};

struct sockaddr_in {
	sa_family_t		sin_family;
	u16			sin_port;
	struct in_addr		sin_addr;
	char			sin_zero[8];
};

struct sockaddr {
	sa_family_t		sa_family;
	char			sa_data[14];
};


#define __ss_aligntype		u32
#define _SS_SIZE		128
#define _SS_PADSIZE		(_SS_SIZE - (2 * sizeof (__ss_aligntype)))

struct sockaddr_storage {
	sa_family_t		ss_family;	/* Address family */
	__ss_aligntype		__ss_align;	/* Force desired alignment.  */
	char			__ss_padding[_SS_PADSIZE];
};

#endif /* TCPIP_INET_SOCKTYPES */


/*
 * Bluetooth
 */
#ifndef TCPIP_BT_SOCKTYPES
#define TCPIP_BT_SOCKTYPES

typedef struct bdaddr {
	unsigned char		b[6];
} bdaddr_t;

#endif /* TCPIP_BT_SOCKTYPES */


struct sockaddr_ll {
	unsigned short	sll_family;
	u16		sll_protocol;
	int		sll_ifindex;
	unsigned short	sll_hatype;
	unsigned char	sll_pkttype;
	unsigned char	sll_halen;
	unsigned char	sll_addr[8];
};

struct sockaddr_un
{
	short		sun_family;
	const char*	sun_path;
};

struct sockaddr_alg {
	u16		salg_family;
	const char*	salg_type;
	u32		salg_feat;
	u32		salg_mask;
	const char*	salg_name;
};
#endif

