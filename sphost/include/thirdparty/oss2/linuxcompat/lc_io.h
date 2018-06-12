/**************************************************************************
 *
 *       Copyright (c) 2013-2013 by iCatch Technology, Inc.
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
 *
 **************************************************************************/
/**
 * @file lc_io.h
 * @brief
 * @author LiJun
 *
 * The detail information about this file.
 */

#ifndef _LINUXCOMPAT_IO_H_
#define _LINUXCOMPAT_IO_H_

#include "common.h"
/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define readb(addr)		(*((volatile u8 *)(addr)))
#define writeb(b, addr)	(*((volatile u8 *)(addr)) = (b))
#define readw(addr)		(*((volatile u16 *)(addr)))
#define writew(w, addr)	(*((volatile u16 *)(addr)) = (w))
#define readl(addr)		(*((volatile u32 *)(addr)))
#define writel(l, addr)	(*((volatile u32 *)(addr)) = (l))
/*
#define writel(l, addr)	do{\
	printf("\nwritel:[0x%08x]=0x%08x\n", (unsigned int)addr, (unsigned int)l);\
	(*((volatile u32 *)(addr)) = (l));\
}while(0)
*/

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
extern unsigned long virt_to_phys(volatile void * address);
extern void * phys_to_virt(unsigned long address);
extern void dma_cache_wback_inv(void* start, unsigned int size);
extern void* _to_uncache(void* address);
extern void* _to_cache(void* address);

extern unsigned long ros_virt_to_phys(volatile void * address);
extern void * ros_phys_to_virt(unsigned long address);
extern void* _ros_to_uncache(void* address);
extern void* _ros_to_cache(void* address);

extern void   cpuDCacheWB(const void *addrstart, UINT32 size);
extern void   cpuDCacheInv(void *addrstart, UINT32 size);

#endif /* _LINUXCOMPAT_IO_H_ */
