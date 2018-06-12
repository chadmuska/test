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
 * @file lc_slab.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_SLAB_H_
#define _LINUXCOMPAT_SLAB_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include	<string.h>


/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
#define KMEM_POOL_USB (0x0)
#define KMEM_POOL_NET (0x1)
#define KMEM_POOL_MAX (2)

void Os_DataCacheInvalidate(void *pAddr, size_t dSize);
void Os_DataCacheFlush(void *pAddr, size_t dSize);
void Os_DataCacheInvalidateAll(void);
int kmem_create(size_t size, int flag);
int kmem_destroy(int flag);
void *kmalloc(size_t uiSize, int flag);
void* kzalloc(size_t uiSize, int flag);
void* kcalloc(size_t n, size_t size, int flag);
void kfree(void* ptr);
void* vmalloc(size_t uiSize);
void vfree(void* ptr);
#define vmalloc_32 vmalloc

#endif	/* _LINUXCOMPAT_SLAB_H_ */
