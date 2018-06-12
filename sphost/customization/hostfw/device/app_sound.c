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
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_rsvblk_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_media_api.h"

#include "app_tvlcd.h"
#include "app_sound.h"
#include "app_buzzer.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32  preloadSoundPool[SOUNT_ID_TOTAL];
static UINT32 ghndSound = 0;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
_mediaSoundPlayAbsoluteStop(
	void
)
{
#if !AUDIO_DEC_SUPPORT
	return;
#endif

	sp5kMediaPlayControl(SP5K_MEDIA_PLAY_ABORT, 0, 0 );
	sp5kResDRAMControl(SP5K_RES_DRAM_ABORT,0,0,0,0,0);
}

void
appAudioDevOn(
	void
)
{
#if !AUDIO_DEC_SUPPORT
	return;
#endif

#if SPCA5210
	sp5kAudDevSrcSet(SP5K_AUD_DEV_PLAY, (IS_LCD_OUT) ?
		AUD_PLAY_SRC_FOR_LCD : AUD_PLAY_SRC_FOR_TV);
#else
	sp5kAudDevSpeakerCtrl(IS_LCD_OUT);
#endif
}

void
appPreloadSoundInit(void)
{
#if !AUDIO_DEC_SUPPORT
	return;
#endif

	preloadSoundPool[SOUND_ID_BEEP] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,(char*)RES_WAV_BEEP ,0,0,0,0);
	preloadSoundPool[SOUND_ID_SHUTTER] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,(char*)RES_WAV_SHUTTER,0,0,0,0);
	preloadSoundPool[SOUND_ID_BURST_SHUTTER] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,(char*)RES_WAV_BURST,0,0,0,0);
	preloadSoundPool[SOUND_ID_WARNING] = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV,(char*)RES_WAV_WARNING,0,0,0,0);
}

UINT32
appAudDevStatusPolling(
	void
)
{
#if !AUDIO_DEC_SUPPORT
	return SUCCESS;
#endif

	UINT32 status, i=0;

	sp5kAudDevStatusGet(&status);
	while (((status&SP5K_AUD_STATUS_PLAY_STOP)==0 && (status&SP5K_AUD_STATUS_PLAY_PAUSE) == 0)&& i<500){
		appTimeDelayMs(20);
		sp5kAudDevStatusGet(&status);
		i+=20;
	}
	return status;
}

void
appPreloadSoundPlay(
	UINT32 sound_id
)
{

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	return;
#else
#if !AUDIO_DEC_SUPPORT
		return;
#endif
#endif
	UINT32 status;

	status = appAudDevStatusPolling();
	if((status&SP5K_AUD_STATUS_PLAY_STOP)==0) {
		return;
	}

	status=sp5kResDRAMControl(SP5K_RES_DRAM_PLAY,preloadSoundPool[sound_id],0,0,0,0);

	do {
		appTimeDelayMs(20);
		sp5kAudDevStatusGet(&status);
	} while ((status&SP5K_AUD_STATUS_PLAY_STOP)==0);

}

void
appPreloadSoundAbort(
	void
)
{
#if !AUDIO_DEC_SUPPORT
			return;
#endif

	sp5kResDRAMControl(SP5K_RES_DRAM_ABORT,0,0,0,0,0);
}

void
appSundVolumeSet(
	soundVolume_e vol
)
{
#if !AUDIO_DEC_SUPPORT
	return;
#endif

	UINT32 volumeTbl[SOUND_VOLUME_MAX] = {0,9,16,30};
	UINT32 volume;

	if (vol >= SOUND_VOLUME_MAX)
	{
		__FUNC_TRACK__;
		return;
	}

    //Shen: When HDMI in should mute speaker, decide by project spec.(bit 17~16: SPEAKER EXTRA GAIN, bit 11~8: LINEOUT GAIN, bit 3~0: SPEAKER GAIN)
	if(IS_LCD_OUT)
	{
		volume = volumeTbl[vol];
        sp5kAudDevVolumeSet(SP5K_AUD_DEV_PLAY_VOL_GAIN, 0x000000);
	}
	else
	{
		volume = volumeTbl[vol] >> 1;
        sp5kAudDevVolumeSet(SP5K_AUD_DEV_PLAY_VOL_GAIN, 0x00000F);
	}
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_PLAY|SP5K_AUD_PLAY_CH_ALL,volume);
}

/*-------------------------------------------------------------------------
 *  File Name : appSoundUnload
 *------------------------------------------------------------------------*/
void
appSoundUnload(
	void
)
{
#if !AUDIO_DEC_SUPPORT
			return;
#endif
	if (ghndSound)
	{
		sp5kResDRAMUnload(ghndSound);
		ghndSound = 0;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appSoundStop
 *------------------------------------------------------------------------*/
void
appSoundStop(
	void
)
{
#if !AUDIO_DEC_SUPPORT
			return;
#endif

	DBG_PRINT("sound: %s !! \n", __FUNCTION__);

	if (ghndSound) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "sound: %s: Unload ResDRAM", __FUNCTION__);
		sp5kResDRAMControl(SP5K_RES_DRAM_ABORT, 0, 0, 0, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appSoundTerminate
 *------------------------------------------------------------------------*/
void
appSoundTerminate(
	void
)
{
#if !AUDIO_DEC_SUPPORT
			return;
#endif
	DBG_PRINT("sound: %s !! \n", __FUNCTION__);

	appSoundStop();
	appSoundUnload();
}

/*-------------------------------------------------------------------------
 *  File Name : appSoundLoad
 *------------------------------------------------------------------------*/
UINT32
appSoundLoad(
	UINT8 *pszSoundFilename
)
{
#if !AUDIO_DEC_SUPPORT
		return SUCCESS;
#endif

	DBG_PRINT("sound: %s: %s !! \n", __FUNCTION__, pszSoundFilename);

	if (pszSoundFilename==NULL) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "sound: %s: Null PTR", __FUNCTION__);
		return FAIL;
	}

	/*-- Release previoud sound resource, if any
	 */
	appSoundTerminate();

	/*-- Music preload
	 */
	ghndSound = sp5kResDRAMPreLoad(SP5K_RES_TYPE_WAV, (char*)pszSoundFilename, 0, 0, 0, 0);

	if (ghndSound==0) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "sound: %s: Music preload failed", __FUNCTION__);
		return FAIL;
	}

	return SUCCESS;

}

/*-------------------------------------------------------------------------
 *  File Name : appSoundPlay
 *------------------------------------------------------------------------*/
UINT32
appSoundPlay(
	void
)
{
#if !AUDIO_DEC_SUPPORT
return SUCCESS;
#endif
	if (!ghndSound) {
		DBG_PRINT();
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "sound: %s: Null sound handle", __FUNCTION__);
		return FAIL;
	}

	if (FAIL==sp5kResDRAMControl(SP5K_RES_DRAM_PLAY, ghndSound, 0, 0, 0, 0)) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "sound: %s: Sound play failed", __FUNCTION__);
		return FAIL;
	}

	DBG_PRINT("sound: %s: Sound play OK !! \n", __FUNCTION__);
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appBeep
 *------------------------------------------------------------------------*/
void
appBeep(
	BOOL waitFinish,
	BOOL forceBeep
)
{
#if !AUDIO_DEC_SUPPORT
	return ;
#endif
	UINT8 sysBeep = pUiSetting->sound.beep;
	_mediaSoundPlayAbsoluteStop();

	if (sysBeep || forceBeep)
	{
		appPreloadSoundPlay(SOUND_ID_BEEP);

		if (waitFinish)
		{
			UINT32 param;
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "sound: %s wait beep", __FUNCTION__);
			appHostMsgWait(SP5K_MSG_MEDIA_SOUND_FINISH, &param, 2000);
		}
	}
}

