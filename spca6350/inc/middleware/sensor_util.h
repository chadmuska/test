/**************************************************************************
 *
 *       Copyright (c) 2009-2016 by iCatch Technology, Inc.
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
 *  Author: Paul May                                                      *
 *                                                                        *
 **************************************************************************/
#ifndef _SENSOR_UTIL_H_
#define _SENSOR_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"


UINT32 afeCmdBufExecute(const UINT8 *cmdbuf, UINT32 bufsize,
		UINT32 cmdportid, UINT32 nbitaddr, UINT32 nbitdata,UINT32 sync);
UINT32 afeCmdFileExecute(const char *fname,
		UINT32 cmdportid, UINT32 nbitaddr, UINT32 nbitdata);
UINT8* afeCmdFileRead(const char *fname,
		UINT8 *pbuf, UINT32 buflen, UINT32 *filelen);

/**
 * \brief to free the buf returned by afeCmdFileRead(..); where the
 * argument "pbuf" was NULL
 * \param pbuf, the return value of afeCmdFileRead(..,NULL,..,..);
 * \limit afe bin file must be in afe fif file if afe cmd fif is available
 */
void   afeCmdBufFree(UINT8 *pbuf);


#ifdef __cplusplus
}
#endif

#endif
