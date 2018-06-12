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
#ifndef _APP_FACE_DETECT_EX_H_
#define _APP_FACE_DETECT_EX_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/ 
#include "app_mmface_detect.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
UINT32 appFdCreateEx(appFdIniOption_e iniFlag);
UINT32 appFdFaceDataGetEx(appFdResult_t *fdResult);
void appFdDestroyEx(void);
void appFdProcStartEx(void);
void appFdProcPauseEx(void);
void appFdProcStopEx(void);

UINT32 appOdObjectDataGetEx(appOdResult_t *odResult);

UINT32 appFdBlinkingDetectCheck(UINT32 yuvAddr,UINT32 qvgaAddr,void * psSizeInfo);
void appFdBlinkDetectCheckEnd(void);
void appFdBlinkDetectSemInit(void);
void appFdBlinkDetectSemPut(void);
void appFdBlinkDetectSemGet(void);
void appFdBlinkDetectSet(UINT32 set);

UINT32 appFdBeautyFun(UINT32 yuvAddr,UINT32 qvgaAddr,void * psSizeInfo);
#endif  /* _APP_FACE_DETECT_EX_H_ */

