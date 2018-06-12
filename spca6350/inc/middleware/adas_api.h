/**************************************************************************
 *
 *       Copyright (c) 2008-2016 by iCatch Technology, Inc.
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
 *  Author: Anmin Deng
 *
 **************************************************************************/
#ifndef _ADAS_API_H_
#define _ADAS_API_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common.h"
/**

 * \param cfg in adasCfgSel_t
 */
UINT32 adasAlgoDoneCB(UINT32 algId,UINT32 param,UINT32 param2);
void adasCtrl(UINT32 enable);
UINT32 adasCfgSet(UINT32 selector,va_list args);
UINT32 adasCfgGet(UINT32 selector,va_list args);
UINT32 adasResultGet(UINT32 selector,va_list args);
void adasReset(void);
void adasCreate(void);
void adasInit(void);
UINT32 adasCfgCheck();











#ifdef __cplusplus
}
#endif

#endif /* _ADAS_API_H_ */
