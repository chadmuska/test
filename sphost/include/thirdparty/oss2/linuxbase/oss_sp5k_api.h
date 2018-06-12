/**************************************************************************
 *
 *       Copyright (c) 2016 by iCatch Technology, Inc.
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
#ifndef __OSS_SP5K_API_H__
#define __OSS_SP5K_API_H__

#include <sp5k_os_api.h>
#include <sp5k_dbg_api.h>

#ifdef  __cplusplus
extern  "C" {
#endif

#define sp5kProfLogAddN(id, msg, n)		profLogAddN(id, msg, n)
#define sp5kProfLogPrintf(slot, format...)	profLogPrintf(slot, format)
#define sp5kProfLogCallStack(id)		profLogCallStack(id)
#define sp5kProfLogInit(id, size)		profLogInit(id, size)
#define sp5kProfLogDump(id)			profLogDump(id)

#ifdef  __cplusplus
}
#endif

#endif /* __OSS_SP5K_API_H__ */

