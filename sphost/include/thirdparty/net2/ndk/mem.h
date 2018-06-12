/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2012 by iCatch Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/

#ifndef __NDK_MEM_H__
#define __NDK_MEM_H__

#include <stdlib.h>
#include <ndk_types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Allocate cached memory. */
void* ndk_mem_malloc(size_t size);
void* ndk_mem_zalloc(size_t size);
void* ndk_mem_realloc(void * ptr, size_t size);
void  ndk_mem_free(void *ptr);
void  ndk_mem_dumpinfo();
void  ndk_mem_global_init(int mempool_size);

#define ndk_mem_talloc(T)	((T*)ndk_mem_malloc(sizeof(T)))
#define ndk_mem_tzalloc(T)	((T*)ndk_mem_zalloc(sizeof(T)))

#define ndk_mem_tnew(T, n)	((T*)ndk_mem_malloc(sizeof(T) * (n)))
#define ndk_mem_tznew(T, n)	((T*)ndk_mem_zalloc(sizeof(T) * (n)))

#define ndk_mem_calloc(n, size)	(ndk_mem_zalloc((n)*(size)))
///////////////////////////////////////////////////////////////////////////////
// Redefine libc memory allocate/fre functions.
#ifdef malloc
#error malloc is already defined.
#endif

#ifdef realloc
#error realloc is already defined.
#endif

#ifdef free
#error free is already defined.
#endif

///#define malloc(n)       ((void*)ndk_mem_malloc(n))
///#define realloc(p, n)   ((void*)ndk_mem_realloc(p, n))
///#define free(p)         ((void)ndk_mem_free(p))

#ifdef __cplusplus
}
#endif

#endif
