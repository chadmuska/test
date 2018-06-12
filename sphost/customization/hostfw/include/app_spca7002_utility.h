/**************************************************************************
 *
 *       Copyright (c) 2006-2018 by iCatch Technology, Inc.
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
#ifndef _APP_SPCA7002_UTILITY_H_
#define _APP_SPCA7002_UTILITY_H_

#ifdef __cplusplus
extern "C" {
#endif

UINT32 appSPCA7002_I2CDataWrite(UINT32 a_udwInputAddr, UINT32 a_udwInputData);
UINT32 appSPCA7002_I2CDataRead(UINT32 a_udwInputAddr);



#ifdef __cplusplus
}
#endif

#endif /*_APP_SPCA7002_UTILITY_H_*/
