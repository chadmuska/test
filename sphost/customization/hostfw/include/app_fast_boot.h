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
<<<<<<< .mine *  Hsin-Chu, Taiwan.
======= *  Hsin-Chu, Taiwan, R.O.C.
>>>>>>> .theirs *
 **************************************************************************/
#ifndef _APP_FAST_BOOT_H_
#define _APP_FAST_BOOT_H_

#define FAST_BOOT  1

typedef struct appPowerOnInfo_s {
	BOOL  isPbPowerOn;
	BOOL  isMutePowerOn;
	BOOL  isTestModePowerOn;
	BOOL  isAudioPowerOn;
	BOOL  isLensFastBoot;
	BOOL  isPvFreeze;
	BOOL  isPowerOnForCharging;
} appPowerOnInfo_t;

UINT32 appFastBootLensInfoGet(UINT8 *pMode, UINT8 *pBattType, UINT8 *pBattLevel);
void appFastBootLensModeSet(UINT32 mode);
void appFastBootLensBattSet(UINT32 type, UINT32 level);
void appPowerOnKeyGet(void);
void appLensFastBootInit(void);

#endif  /* _APP_FAST_BOOT_H_ */


