/**************************************************************************
 *
 *       Copyright (c) 2006-2016 by iCatch Technology, Inc.
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
 *  Author: Stephen Hung                                                  *
 *                                                                        *
 **************************************************************************/
#ifndef _OS_API__H_
#define _OS_API__H_

#ifdef   __cplusplus
extern   "C" {
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define TINY_STACK_SIZE   256
#define SMALL_STACK_SIZE  512
#define NORMAL_STACK_SIZE 1024
#define LARGE_STACK_SIZE  2048
#define HUGE_STACK_SIZE   4096

/*_________________________________________________________________________

  Memory allocation/free APIs
  _________________________________________________________________________
*/
UINT32 osMemFree(void *pointer);
void   osCustomMemBlkInit(const UINT32 blkprof[][2]);
void   osCustomMemByteInit(UINT32 size);
void*  osCustomMemAlloc(UINT32 size);
void*  osCustomMemAllocCache(UINT32 size);
void*  osCustomMemAllocExt(UINT32 size, UINT32 opt);
void*  osCustomMemAllocCacheExt(UINT32 size, UINT32 opt);

#ifdef   __cplusplus
}
#endif

#endif /* _OS_API__H_ */
