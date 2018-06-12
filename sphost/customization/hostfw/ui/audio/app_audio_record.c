/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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
 *  File name    : App_audio_record.c
 *  Function     : About audio config and control for record.
 *  First editor :   
 *  First date   :   
 *  History      :
 *               : 2018.06.01  Edward.Lu  "Add description and modify code"
 **************************************************************************/

#include "sp5k_global_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_media_api.h"
#include "sp5k_disk_api.h"

#include "app_com_def.h"
#include "app_com_api.h"
#include "app_icon_def.h"
#include "app_pal.h"
#include "app_osd.h"
#include "gpio_custom.h"
#include "app_audio.h"
#include "app_still.h"
#include "app_jpg_draw.h"
#include "app_error_msg.h"
#include "app_view_osd.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_res_def.h"
#include "app_sound.h"
#include "app_battery.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define HOST_DBG 0
/* 26 Verified on HW_SBC,  original is 16*/
#define APP_DEFAULT_REC_AUD_VOL		36

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
appAudioCB_t gAudioCB;
UINT32 AudioRemainTime = 0;
UINT8 gAudioAlcEnable = 0;

static BOOL gIsAudRecMute=FALSE;
static UINT32 gAudRecVol=APP_DEFAULT_REC_AUD_VOL;
static UINT32 gAudRecVolSave=APP_DEFAULT_REC_AUD_VOL;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appAudioRecVolumeRestore
* Function         : Audio Volume Restore
* Return value	   : void
* Parameter1	   : void
* History	       :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appAudioRecVolumeRestore(
	void
)
{
	DBG_PRINT("AUD Vol RESTORE\n");
	gAudRecVol = gAudRecVolSave;
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, gAudRecVol);
	gIsAudRecMute = FALSE;
}

/*--------------------------------------------------------------------------*
* Function name    : appAudioRecVolumeMute
* Function         : Audio Volume Mute
* Return value	   : void
* Parameter1	   : void
* History	       :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appAudioRecVolumeMute(
	void
)
{
    DBG_PRINT("AUD Vol MUTE\n");
	gAudRecVolSave = gAudRecVol;
	gAudRecVol = 0;
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, gAudRecVol);
	gIsAudRecMute = TRUE;
}

/*--------------------------------------------------------------------------*
* Function name    : appAudioRecVolumeConfig
* Function         : Audio Volume Config
* Return value	   : void
* Parameter1	   : void
* History	       :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define FIRSTNOTCH_EN 0
#define SECONDNOTCH_EN 0
#define THIRDNOTCH_EN 0
#define FOURTHNOTCH_EN 0
#define FIFTHNOTCH_EN 0
#define EQFILTER_EN 0
#define HIGHSHELF_FCENTRE 500
#define HIGHSHELF_GAIN -60
#else
#define FIRSTNOTCH_EN 1
#define SECONDNOTCH_EN 1
#define THIRDNOTCH_EN 1
#define FOURTHNOTCH_EN 1
#define FIFTHNOTCH_EN 1
#define EQFILTER_EN 1
#define HIGHSHELF_FCENTRE 10000
#define HIGHSHELF_GAIN -10
#endif
void
appAudioRecVolumeConfig(
	void
)
{
	if (gAudioAlcEnable == ENABLE) {
		sp5kMediaAlcParamFileLoad(RES_WAV_ALC_TXT);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_ON);
	}
	else {
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC, SP5K_MEDIA_REC_OFF);
	}

	gAudRecVol = gAudRecVolSave = APP_DEFAULT_REC_AUD_VOL;
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, gAudRecVol);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if (gAudioAlcEnable == ENABLE) {
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST | SP5K_AUD_REC_CH_LEFT, 0xc0000002);/* 0xc0000001 Verified on HW_SBC,  original is 0x4000001*/
	}
	else{
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST | SP5K_AUD_REC_CH_LEFT, 0xc000000F);/* 0xc0000001 Verified on HW_SBC,  original is 0x4000001*/
	}
		
#else
    sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC_VOL_BOOST | SP5K_AUD_REC_CH_ALL, 0xc0000001);/* 0xc0000001 Verified on HW_SBC,  original is 0x4000001*/
#endif

	if (gAudioAlcEnable == ENABLE) {
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_ALC_MODE, SP5K_MEDIA_REC_ALC_DRC_MODE); /* ALC mode set */
	}

	gIsAudRecMute = FALSE;
    /* Verified on HW_SBC*/
    {
    	sp5kAudFilterNotchCfg_t NotchCfg;

    	NotchCfg.chnlId= SP5K_AUD_FILTER_ALL;
    	NotchCfg.Fsample =44100;
    	NotchCfg.firstNotch.en=FIRSTNOTCH_EN;
    	NotchCfg.firstNotch.Fcentre=7900;
    	NotchCfg.firstNotch.Qfactor=5;
    	NotchCfg.secondNotch.en=SECONDNOTCH_EN;
    	NotchCfg.secondNotch.Fcentre=4000;
    	NotchCfg.secondNotch.Qfactor=2;
    	NotchCfg.thirdNotch.en=THIRDNOTCH_EN;
    	NotchCfg.thirdNotch.Fcentre=6000;
    	NotchCfg.thirdNotch.Qfactor=2;
    	NotchCfg.fourthNotch.en=FOURTHNOTCH_EN;
    	NotchCfg.fourthNotch.Fcentre=7800;
    	NotchCfg.fourthNotch.Qfactor=1;
    	NotchCfg.fifthNotch.en=FIFTHNOTCH_EN;
    	NotchCfg.fifthNotch.Fcentre=10000;
    	NotchCfg.fifthNotch.Qfactor=1;

    	sp5kAudFilterNotchCfg(&NotchCfg);

    	/* Equalizer filter */
    	sp5kAudFilterEqCfg_t eqFilter;

    	eqFilter.chnlId = SP5K_AUD_FILTER_ALL;
    	eqFilter.en = EQFILTER_EN;
    	eqFilter.Fsample = 44100;

    	eqFilter.lowShelf.Fcentre = 300;
    	eqFilter.lowShelf.gain = -10;

    	eqFilter.firstPeak.Fcentre = 4000;
    	eqFilter.firstPeak.gain = 0;
    	eqFilter.firstPeak.Qfactor = 1;
    	eqFilter.secondPeak.Fcentre = 10;
    	eqFilter.secondPeak.gain = 0;
    	eqFilter.secondPeak.Qfactor = 1;

    	eqFilter.thirdPeak.Fcentre = 10;
    	eqFilter.thirdPeak.gain = 0;
    	eqFilter.thirdPeak.Qfactor = 1;

    	eqFilter.highShelf.Fcentre = HIGHSHELF_FCENTRE;
    	eqFilter.highShelf.gain = HIGHSHELF_GAIN;
    	sp5kAudFilterEqCfg(&eqFilter);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	/* Highpass filter */                 
	sp5kAudFilterHpCfg_t filter;                 
	filter.chnlId = SP5K_AUD_FILTER_ALL;                
	filter.Fsample = 44100;                     
	filter.filter.type = SP5K_HP_FILTER_2ND_ORDER; 
	/* SP5K_HP_FILTER_DISABLE: disable, 
	SP5K_HP_FILTER_1ST_ORDER: enable 1st order high-pass filter, 
	SP5K_HP_FILTER_2ND_ORDER: enable 2nd order high-pass filter */                
	filter.filter.Fcentre = 1200;                 
	filter.filter.Dfactor = 1.414; // suggest >= 1.414                 
	sp5kAudFilterHpCfg(&filter);
#endif
    }
}


/*--------------------------------------------------------------------------*
* Function name    : appAudioRecFlagSet
* Function         : Set Audio On/Off. Isaac Add to enable/disable gAudioAlcEnable voice rec 
* Return value	   : void
* Parameter1	   : pUiSetting->voiceSet (BOOL)
* History	       :
*                  : 2018.06.01  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appAudioRecFlagSet(BOOL onoff){	
	gAudioAlcEnable = onoff;	
	printf("%s  %d\n",__FUNCTION__,gAudioAlcEnable);
}