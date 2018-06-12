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
#ifndef _SP5K_ADAS_PRIVATE_H_
#define _SP5K_ADAS_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"


#define ADAS_PROF_LOG_ID   0

typedef struct {
	UINT32 (*fp_sp5kAdasCtrl)();
	UINT32 (*fp_sp5kAdasCfgSet)();
	UINT32 (*fp_sp5kAdasCfgGet)();
	UINT32 (*fp_sp5kAdasResultGet)();	
} sp5kAdasApiObj_t;

void sp5kAdasApiInstall(const sp5kAdasApiObj_t *pobj);



#ifdef __cplusplus
}
#endif

#endif /* _SP5K_ADAS_PRIVATE_H_ */
