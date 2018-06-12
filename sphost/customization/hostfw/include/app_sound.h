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
#ifndef APP_SOUND_H
#define APP_SOUND_H

#include "app_user_setting.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	SOUND_ID_BEEP,
	SOUND_ID_FOCUSOK,
	SOUND_ID_WARNING,
	SOUND_ID_SHUTTER,
	SOUND_ID_BURST_SHUTTER,
	SOUNT_ID_TOTAL,
} preloadSoundIdDef_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appAudioDevOn(void);
void appPreloadSoundInit(void);
void appPreloadSoundPlay(UINT32 sound_id);
void appPreloadSoundAbort(void);
void appSundVolumeSet(soundVolume_e vol);

void appSoundUnload(void);
void appSoundStop(void);
void appSoundTerminate(	void);
UINT32 appSoundLoad(UINT8 *pszSoundFilename);
UINT32 appSoundPlay(void);
void appBeep(BOOL waitFinish,	BOOL forceBeep);
#endif
