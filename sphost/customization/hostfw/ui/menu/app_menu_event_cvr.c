/**************************************************************************
 *         Copyright(c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch Tech-  *
 *  nology Co., Ltd. All rights are reserved by iCatch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of iCatch Technology Co., Ltd.                 *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/

/**
 * @file      app_menu_event.c
 * @brief     Brief
 * @author    Phil Lin
 * @since     2012-10-15
 * @date      2012-10-15
*/

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include <stdio.h>

#include "app_com_def.h"

#include "app_menu.h"
#include "app_menu_core.h"
#include "app_menu_tab.h"
#include "app_menu_disp.h"
#include "app_menu_event.h"
#include "app_ui_para.h"
#include "app_setup_lib.h"
#include "app_user_setting.h"


#include "sp5k_cdsp_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_af_api.h"
#include "sp5k_rsvblk_api.h"
#include "customization/app_init.h"
#include "app_com_api.h"
#include "app_com_def.h"
#include "app_osd.h"
#include "app_user_setting.h"
#include "app_view_param.h"
#include "app_playback_def.h"
#include "app_zoom_api.h"
#include "app_util.h"

#include "app_still.h"
#include "app_video.h"

#include "app_strobe.h"
#include "app_led.h"
#include "app_aaa.h"
#include "app_exif.h"
#include "app_view_osd.h"
#include "app_face_detect_ex.h"
#include "app_osd_api.h"
#include "app_playback_def.h"
#include "app_playback_uiparam.h"
#include "app_calib_api.h"
#include "api/sp5k_awb_api.h"
#include "app_playback_osd.h"
#include "app_card_isp.h"
#include "customization/app_init.h"
#include "middleware/sysg_resource_def.h"

/*Isaac*/
#if GSENSOR
#include "app_gSensor.h"
#endif

#if SP5K_MOTION_DETECT
#include "app_md.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
UINT8 rtc_init[3]={0, 1, 2};
UINT8 rtc_left[3][6]={{5, 0, 1, 2, 3, 4}, {2, 5, 1, 0, 3, 4}, {1, 2, 5, 0, 3, 4}};
UINT8 rtc_right[3][6]={{1, 2, 3, 4, 5, 0}, {3, 2, 0, 4, 5, 1}, {3, 0, 1, 4, 5, 2}};
/*ez mark*/
/*static UINT8 sceneMenuMap[SENCEMENU_MAX][2]=
{
	{SENCEMENU_PROGRAM, 			PASM_PROGRAM},
	{SENCEMENU_MOVIE		,		VIDEO_MODE},
	{SENCEMENU_SMILEDET ,			SCENE_SMILE_DETECTION},
	{SENCEMENU_SHAKE_FREE	,		SCENE_SHAKE_FREE},
	{SENCEMENU_LANDSCAPE	,		SCENE_LANDSCAPE},
	{SENCEMENU_BACK_LIGHT	,		SCENE_BACK_LIGHT},
	{SENCEMENU_NIGHT		,		SCENE_NIGHT},
	{SENCEMENU_BUILDING ,			SCENE_BUILDING},
	{SENCEMENU_CHILDREN ,			SCENE_CHILDREN},
	{SENCEMENU_PORTRAIT ,			SCENE_PORTRAIT},
	{SENCEMENU_FOOD 	,			SCENE_FOOD},
	{SENCEMENU_TEXT 	,			SCENE_TEXT},
	{SENCEMENU_BEACH_SNOW	,		SCENE_BEACH_SNOW},
	{SENCEMENU_FIRE_WORKS	,		SCENE_FIRE_WORKS},
	{SENCEMENU_SUNSET		,		SCENE_SUNSET},
	{SENCEMENU_WDR			,		SCENE_WDR},
	{SENCEMENU_HDR		,			SCENE_HDR},
	{SENCEMENU_PANORAMA		,		SCENE_PANORAMA},
	{SENCEMENU_BLINKDETECT,			SCENE_BLINK},
	{SENCEMENU_VOICEREC ,			AUDIO_MODE},
};*/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 DPSImgIdxOld=1;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern BOOL _setupMenuFunctionFormat(void);
extern void appPeriodicalCaptureTimeSet(void);
extern void appMenu_PosInit(void);
extern void appStill_FeatureEffectSet(void);
extern void appIqSaturationFactorSet(UINT32 sat);
extern void appPlayOsd_FileNumDraw(UINT8 bShow,UINT32 CurrntIndex,UINT32	Total);
extern void _setupMenuFunctionResetAll(void);

extern UINT8 fileTypeSelect;
extern UINT8 IsSetImage;
extern UINT32 enPartial;
//extern UINT8 *pszSlideMusic[SLSHOW_MUSIC_TOTAL]; /*ez mark*/
extern appRTC_t rtcTime;
#if GPS_SUPPORT
extern UINT8 gps_update_RtcTime_flag;
#endif
extern UINT8 USBModeSet;
#if GSENSOR
/* Isaac add 20161209*/
extern void appGsensorLevelCtrl(UINT8 level);
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**
 * @fn        UINT32 appMenuEvt_ModeStill(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_ModeStill
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_ModeStill(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	item = item; /* just for clearing waring */
//	param = param; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_RIGHT:
			menuProcCurrPosSet(MENU_MARK_SETUP);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}

/**
 * @fn        UINT32 appMenuEvt_ModeVideo(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_ModeVideo
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_ModeVideo(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing warning */
//	item = item; /* just for clearing warning */
//	param = param; /* just for clearing warning */

	switch (msg) {
		case APP_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
#if	defined(PRJ_JETDVR) || defined(PRJ_PDVR)
		case APP_KEY_PRESS_SET:/*Isaac add for jetdvr */
#endif		
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_RIGHT:
			menuProcCurrPosSet(MENU_MARK_SETUP);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

/**
 * @fn        UINT32 appMenuEvt_VideoSizeSet(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_VideoSizeSet
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_VideoSizeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT32 mode ;
	sp5kModeGet(&mode);

//	item = item;	/* just for clear warning */
//	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			{
				menuProcParaSet(layer, item);

                if((puiPara->VideoSize== UI_VIDEO_SIZE_4K2K)||(puiPara->VideoSize == UI_VIDEO_SIZE_27K)){
                    puiPara->Seamless = UI_VIDEO_SEAMLESS_OFF;
                }

                appEZ_menuParaChange();
                appVideoSizeSet();
			}
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoQualitySet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
	UINT32 mode ;
	sp5kModeGet(&mode);

//	item = item;	/* just for clear warning */
//	param = param; /* just for clear warning */

	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appVideoSetQuality();
			break;
	}
	return ret;
}

UINT32
appMenuEvt_SeamlessSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
	UINT32 mode ;

	sp5kModeGet(&mode);
//	item = item;	/* just for clear warning */
//	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoWBModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
#if SP5K_SENSOR_DUAL_MODE
#else
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
//	item = item;	/* just for clear warning */
//	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			appStill_SetWB(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			appStill_SetWB(menuProcCurrItemGet(layer));
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_MENU:
			appStill_SetWB(puiPara->WBMode);
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appStill_SetWB(pViewParam->wb);
            sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
			break;
	}

	return ret;
#endif
}

UINT32
appMenuEvt_RollingShutterSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clear warning */
//	item = item;	/* just for clear warning */
//	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			//if(appActiveStateGet() == APP_STATE_VIDEO_SUBMENU){
			menuProcParaSet(layer, item);
			//appVideo_SetRSC(puiPara->RollingShutter, puiPara->DisMode);
			//}
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoMeteringSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

//	layer = layer; /* just for clear warning */
//	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetMetering(puiPara->AEMetering);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoFaceTrackingSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

//	layer = layer; /* just for clear warning */
//	param =param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			if(item == 0){	//video face tracking on
				puiPara->VideoFaceTracking = 0;
			}
			else{			//video face tracking off
				puiPara->VideoFaceTracking = 1;
			}
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoStampModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
	UINT32 mode ;
	sp5kModeGet(&mode);

//	layer = layer; /* just for clear warning */
//	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer,item);
			appEZ_menuParaChange();
			break;
	}

	return ret;
}


UINT32
appMenuEvt_VideoEISModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clear warning */
//	param =param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			break;
	}

	return ret;
}

UINT32
appMenuEvt_VideoDistModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clear warning */
//	param =param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			//if(appActiveStateGet() == APP_STATE_VIDEO_SUBMENU){
			menuProcParaSet(layer, item);
			//appVideo_SetDist(puiPara->DisMode, puiPara->RollingShutter);
			//}
			break;
	}

	return ret;
}

UINT32
appMenuEvt_AdvanceDenoise(	//not ready
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

//	layer = layer; /* just for clear warning */
//	param =param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_SET:
			if(item == 0){	//advance denoise on
				puiPara->AdvanceDenoise = 0;
			}
			else{
				puiPara->AdvanceDenoise = 1;
			}
			break;
	}

	return ret;
}


/**
 * @fn        UINT32 appMenu_ModePlayback(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenu_ModePlayback
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_ModePlayback(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	item = item; /* just for clearing waring */
//	param = param; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_RIGHT:
			menuProcCurrPosSet(MENU_MARK_SETUP);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}

/**
 * @fn        UINT32 appMenuEvt_ModeSetup(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_ModeSetup
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_ModeSetup(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	item = item; /* just for clearing waring */
//	param = param; /* just for clearing waring */

	switch (msg) {
		case APP_KEY_PRESS_UP:
			if (menuProcChild (0) == SUCCESS)
				menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_DOWN:
			menuProcChild (0);
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
#if	defined(PRJ_JETDVR) || defined(PRJ_PDVR)
		case APP_KEY_PRESS_SET:/*Isaac add for jetdvr */
#endif
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_RIGHT:

#if	defined(PRJ_JETDVR) || defined(PRJ_PDVR)
	menuProcCurrPosSet(MENU_MARK_VIDEO);		
#else
	appMenu_PosInit();
#endif
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}

UINT32
appMenuEvt_SoundInit(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	item = item; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
		case APP_KEY_PRESS_RIGHT:
			menuProcChild(0);
			menuProcDisp2Layer();

			#if 0
			psoundcontrol->voicerec= appUiParaGet()->VoiceRec;
			#endif
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_UP:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}

UINT32
appMenuEvt_SoundSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//UINT8 preVoiceRecSts;
	//UINT32 mode ;
	//UINT32 OptionItem;
	printf("%s !! \n", __FUNCTION__);
	//sp5kModeGet(&mode);
//	param = param; /* just for clearing waring */

	//preVoiceRecSts = appUiParaGet()->VoiceRec;/*ez mark*/
    /*SHEN: beep sound call by appMenuState()*/
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			appSoundTerminate();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			appSoundTerminate();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			appSoundTerminate();
			appSetupMenu_Sound_Adjust(item, 0);
			//OptionItem = item;
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();
            appEZ_menuParaChange();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			appSoundTerminate();
			appSetupMenu_Sound_Adjust(item, 1);
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();
            appEZ_menuParaChange();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
#if	 defined(PRJ_JETDVR) || defined(PRJ_PDVR)
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			appSoundTerminate();
/*			appSetupMenu_Sounn_Adjust(item, psoundcontrol, 1); */
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
#else
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
#endif
			break;
		case APP_KEY_PRESS_MENU:
			menuProcParent();
			menuProcDisp();
			break;
	}
	return ret;
}

UINT32
appMenuEvt_AutoReviewSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
#if 0 /*ez mark*/
	switch(msg){
		case APP_KEY_PRESS_SET:
			if(item == 0){		//autoreview off
				puiPara->AutoReviewTime = QV_TIME_OFF;
			}
			else if(item == 1){	//autoreview 1s
				puiPara->AutoReviewTime = QV_TIME_1SEC;
			}
			else if(item == 2){	//autoreview 3s
				puiPara->AutoReviewTime = QV_TIME_3SEC;
			}
			else if(item == 3){	//autoreview 5s
				puiPara->AutoReviewTime = QV_TIME_5SEC;
			}
			if((puiPara->AutoReviewTime != QV_TIME_OFF && puiPara->FaceTracking == UI_FACETRACK_SMILE_DETECT)\
				||(puiPara->AutoReviewTime != QV_TIME_OFF && puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)) /*add for mantis bug 0047602*/
			{
				puiPara->FaceTracking = UI_FACETRACK_OFF;
			}
			break;
	}

#endif
	return ret;
}

UINT32
appMenuEvt_LightFreqSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			applightFrequencySetting(pUiSetting->lightFrequency);
			break;
		}

	return ret;
}
UINT32
appMenuEvt_VideoFormatSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			puiPara->VideoFormat = item;
			break;
		}
	return ret;
}

UINT32
appMenuEvt_MediaFormatSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_powerOnRecordSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
	}
	return ret;
}


UINT32
appMenuEvt_SleepTimeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
	}

	return ret;
}

UINT32
appMenuEvt_PeriodicalCapSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
#if SP5K_SENSOR_DUAL_MODE
#else
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appPeriodicalCaptureTimeSet();
			break;
	}
	return ret;
#endif
}

void
appMenuEvt_DayItemCheck(
	UINT8 item,
	UINT32 msg
)
{
	UINT8 dayMax;
	uiPara_t* puiPara = appUiParaGet();

	switch(puiPara->rtcMonth+1)
	{
		case 2:
		//modify for leep year problem @2008-04-11 wei.sun begin
		//if ((puiPara->rtcYear) && (!(puiPara->rtcYear & 0x03)))
		if (!(puiPara->rtcYear & 0x03))
		//modify for leep year problem @2008-04-11 wei.sun end
			dayMax = 28;
		else
			dayMax = 27;
		if (puiPara->rtcDay > dayMax)
		{
			if ((item == 2) && (msg == APP_KEY_PRESS_UP))
				puiPara->rtcDay = 0;
			else
				puiPara->rtcDay = dayMax;
		}
		break;

		case 4:
		case 6:
		case 9:
		case 11:
		dayMax = 29;
		if (puiPara->rtcDay > dayMax)
		{
			if ((item == 2) && (msg == APP_KEY_PRESS_UP))
				puiPara->rtcDay = 0;
			else
				puiPara->rtcDay = dayMax;
		}
		break;
	}
/*modify for mantis bug 0050361*/
	if(puiPara->rtcYear == 50 && msg == APP_KEY_PRESS_DOWN)
	{
		puiPara->rtcYear = 50;
	}
	if(puiPara->rtcYear == 0 && msg == APP_KEY_PRESS_UP)
	{
		puiPara->rtcYear = 0;
	}

}
UINT32
appMenuEvt_RTCInit(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			appRtcGet(&rtcTime);
			puiPara->rtcYear = rtcTime.year - 2000;
			puiPara->rtcMonth = rtcTime.mon - 1;
			puiPara->rtcDay = rtcTime.day - 1;
			puiPara->rtcHour = rtcTime.hour;
			puiPara->rtcMinute = rtcTime.min;
			menuProcChild (rtc_init[puiPara->DateStyle]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		}

	return ret;
}

UINT32
appMenuEvt_RTCSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	UINT32 mode ;
	sp5kModeGet(&mode);

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_REPEAT_UP:
			if (item == 5)
			{
				ret = MENU_IGNORE_DEFAULT;
				break;
			}
		case APP_KEY_PRESS_UP:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			appMenuEvt_DayItemCheck(item, APP_KEY_PRESS_UP);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_REPEAT_DOWN:
			if(item == 5)
			{
				ret = MENU_IGNORE_DEFAULT;
				break;
			}
		case APP_KEY_PRESS_DOWN:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			appMenuEvt_DayItemCheck(item, APP_KEY_PRESS_DOWN);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
			menuProcParent();
			menuProcChild (rtc_left[puiPara->DateStyle][item]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcParent();
			menuProcChild (rtc_right[puiPara->DateStyle][item]);
			menuRTCDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			if(mode == SP5K_MODE_VIDEO_RECORD)
			{
				ret = menuProcVidRecAlwaysRtcExcute();
			}
			else
			{
				rtcTime.year = puiPara->rtcYear + 2000;
				rtcTime.mon = puiPara->rtcMonth + 1;
				rtcTime.day = puiPara->rtcDay + 1;
				rtcTime.hour = puiPara->rtcHour;
				rtcTime.min = puiPara->rtcMinute;
				rtcTime.sec = 0;
				appRtcSet(&rtcTime);
				menuProcParent();
#if	defined(PRJ_JETDVR) || defined(PRJ_PDVR)
				menuProcChild (rtc_right[puiPara->DateStyle][item]);
				menuRTCDisp();
#else
				menuProcDisp();
#endif
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
		}
	return ret;
}

/**
 * @fn        UINT32 appMenuEvt_LanguageSet(UINT8 layer, UINT8 item, UINT32 msg, UINT32 param)
 * @brief     appMenuEvt_LanguageSet
 * @param     [in] layer
 * @param     [in] item
 * @param     [in] msg
 * @param     [in] param
 * @retval    return = SUCCESS / FAIL
 * @see       NULL
 * @author    Phil Lin
 * @since     2012-10-15
 * @todo      N/A
 * @bug       N/A
*/
UINT32
appMenuEvt_LanguageSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appSetupMenu_SetCurLanguage(pUiSetting->language);
			break;
	}
	return ret;
}

UINT32
appMenuEvt_TVModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			break;
	}

	return ret;
}

UINT32
appMenuEvt_G_SensorSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
	//UINT32 mode;

//	param=param;
#if 0 /*ez mark*/
	sp5kModeGet(&mode);
	switch(msg){
		#if FUNC_HOST_TOUCH_PANEL
		case APP_KEY_PRESS_TPSET:
		#endif
		case APP_KEY_PRESS_SET:
			appGsensorInit(GSENSOR_NORMAL_INIT);
			appGsensorBasePointGet();
			if(mode == SP5K_MODE_VIDEO_RECORD)
			{
				ret = menuProcVidRecAlwaysParaExcute(layer, item, msg,puiPara->G_Sensor);
			}
			else
			{
				menuProcParaSet(layer, item);
			}
			break;
		}
#endif
/* Isaac add Gsensor Function */

#if defined(HW_PDVR)
//	item = item;	/* just for clear warning */
//	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appGsensorLevelCtrl(item);
			break;
		}
#endif

	return ret;
}

#if defined(PRJ_PDVR)
UINT32
	appMenuEvt_HDRSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

//	item = item;	/* just for clear warning */
//	param = param; /* just for clear warning */
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();

			if(item == HDR_OFF)
			{
				sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, 0x32);

			}
			else
			{
				sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, 0x30);
			}
			sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
			sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
			break;
		}

	return ret;
}
#endif


UINT32
appMenuEvt_GPSSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
//	layer=layer;
//	item=item;
//	msg=msg;
//	param=param;

	return MENU_CALL_DEFAULT;
}

UINT32
appMenuEvt_WaterMarkSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	//uiPara_t* puiPara = appUiParaGet();
	//UINT32 mode ;
	//sp5kModeGet(&mode);
//	param=param;

	UINT8 ret = MENU_CALL_DEFAULT;
#if 0 /*ez mark*/

	switch (msg) {
		case APP_KEY_PRESS_SET:
			if((mode == SP5K_MODE_VIDEO_RECORD)&&(puiPara->StampMode != UI_STAMP_OFF))
			{
				ret = menuProcVidRecAlwaysParaExcute(layer, item, msg,puiPara->WaterMark);
			}
			else
			{
				menuProcParaSet(layer, item);
			}
			break;
	}
#endif

	return ret;
}

UINT32
appMenuEvt_LOGOMarkSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	//uiPara_t* puiPara = appUiParaGet();
	//UINT32 mode ;
	//sp5kModeGet(&mode);
//	param=param;

	UINT8 ret = MENU_CALL_DEFAULT;
#if 0 /*ez mark*/

	switch (msg) {
		case APP_KEY_PRESS_SET:
			if((mode == SP5K_MODE_VIDEO_RECORD)&&(puiPara->StampMode != UI_STAMP_OFF))
			{
				ret = menuProcVidRecAlwaysParaExcute(layer, item, msg,puiPara->LOGOMark);
			}
			else
			{
				menuProcParaSet(layer, item);
			}
			break;
	}
#endif

	return ret;
}

UINT32
appMenuEvt_InvertModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
//	param=param;
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			#if GPS_SUPPORT
			gps_update_RtcTime_flag=0;
			#endif
			break;
	}

	return ret;
}

UINT32
appMenuEvt_CollisionDetectionSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
//	layer=layer;
//	item=item;
//	msg=msg;
//	param=param;

	return MENU_CALL_DEFAULT;
}

UINT32
appMenuEvt_TimeZonesSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	//uiPara_t* puiPara = appUiParaGet();
	UINT8 ret = MENU_CALL_DEFAULT;
	//UINT32 mode ;

	//sp5kModeGet(&mode);
//	param=param;
#if 0 /*ez mark*/

	switch (msg) {
		case APP_KEY_PRESS_SET:
			if(mode == SP5K_MODE_VIDEO_RECORD)
			{
				ret = menuProcVidRecAlwaysTimeZonesExcute(layer,item);
			}
			else
			{
				appTimezoneChange(puiPara->TimeZones,item);
				menuProcParaSet(layer, item);
			}
			break;
	}

#endif
	return ret;
}



UINT32
appMenuEvt_SystemModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
//	layer=layer;
//	item=item;
//	param=param;

	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			CLEAR_OSD_SRC;
			appStateChange(APP_STATE_MENU_SYSTEM, STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}

UINT32
appMenuEvt_WIFIPasswordInit(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
//	item=item;
//	msg=msg;

#if 0 /*ez mark*/
	switch(msg){
		case APP_KEY_PRESS_RIGHT:
		case APP_KEY_PRESS_SET:
			menuProcChild(0);
			appWIFIPasswordLoad();
			menuWIFIPasswordDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		}
#endif
	return ret;
}


UINT32
appMenuEvt_WIFIPasswordSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
//	item=item;
//	msg=msg;
#if 0 /*ez mark*/

	switch(msg){
		case APP_KEY_REPEAT_UP:
		case APP_KEY_PRESS_UP:
			appWIFIPasswordSet(DIR_UP);
			menuWIFIPasswordDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_REPEAT_DOWN:
		case APP_KEY_PRESS_DOWN:
			appWIFIPasswordSet(DIR_DOWN);
			menuWIFIPasswordDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
			appWIFIPasswordSet(DIR_LEFT);
			menuWIFIPasswordDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			appWIFIPasswordSet(DIR_RIGHT);
			menuWIFIPasswordDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			appWIFIPasswordSave();
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		}

#endif
	return ret;
}

UINT32
appMenuEvt_USBModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
	//UINT32 mode ;
	//UINT32 hdmiHWflag;
	//sp5kModeGet(&mode);

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_MODE:
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			{
				#if 0 /*ez mark*/
				if(appFwUpdateFileGet()&& USBModeSet)
				{
					if(mode == SP5K_MODE_VIDEO_RECORD)
					{
						menuProcVidRecAlwayStateChangeExcute();
					}
					appStateChange(APP_STATE_MENU_FW_UPDATE, STATE_PARAM_NORMAL_INIT);
				}
				else
				{
					hdmiHWflag = sp5kDispHdmiHotPlugDetectGet(0);
					if(hdmiHWflag)
					{
						sp5kHostMsgSend(APP_KEY_HDMI_IN, 0xff);
					}
					appMenu_MenuExit(0);
				}
				#endif
				ret = MENU_CALL_DEFAULT;
			}
			break;
	}
	return ret;
}

UINT32
appMenuEvt_Format(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//UINT32 mode ;
	//sp5kModeGet(&mode);

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */

	switch(msg)
	{
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer,item);
			if (item == FORMAT_YES)
			{
				appEZ_menuParaChange();
				_setupMenuFunctionFormat();

			}
			break;
	}

	return ret;
}

UINT32
appMenuEvt_StartUpPicSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			break;
		}

	return ret;
}

UINT32
appMenuEvt_FWUpdate(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara;
	//UINT32 mode ;
	//sp5kModeGet(&mode);

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer,item);
			if (item == RESET_YES)
			{
				#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
				appFirmwareUpdate();
				#endif
				//puiPara = appUiParaGet();
			}
			break;

	}

	return ret;
}



UINT32
appMenuEvt_ResetAll(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara;
	//UINT32 mode ;
	//sp5kModeGet(&mode);

//	layer = layer; /* just for clearing waring */
//	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer,item);
			if (item == RESET_YES)
			{
				_setupMenuFunctionResetAll();
				//puiPara = appUiParaGet();
			}
			break;

	}

	return ret;
}

void
appMenuEvt_SceneMenuBackToStill(
	void
)
{
#if 0 /*ez mark*/
	UINT8 count;
	uiPara_t* puiPara = appUiParaGet();

	for(count = 0; count < SENCEMENU_MAX; count++)
	{
		if(puiPara->SceneMode == sceneMenuMap[count][0])
		{
			gStillCB.lastcap.CurViewMode = gStillCB.cap.CurViewMode;
			gStillCB.lastcap.CurScene = gStillCB.cap.CurScene;
			if((puiPara->SceneMode == SENCEMENU_PROGRAM)
				||(puiPara->SceneMode == SENCEMENU_MOVIE)
				||(puiPara->SceneMode == SENCEMENU_VOICEREC))
			{
				gStillCB.cap.CurViewMode = sceneMenuMap[count][1];
				gStillCB.cap.CurScene  = SCENE_NONE;
			}
			else
			{
				gStillCB.cap.CurViewMode = SCENE_MODE;
				gStillCB.cap.CurScene  = sceneMenuMap[count][1];
 			}
		}
	}
#endif
}

UINT32
appMenuEvt_SceneModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

#if 0 /*ez mark*/
	appViewInit();
	if(gStillCB.cap.CurViewMode == AUDIO_MODE)
	{
		appJpgDraw_ImgLayer("A:\\RO_RES\\UI\\JPG\\AUDIOBK.JPG", 0,0,320,240);
	}

	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appMenuEvt_SceneMenuBackToStill();
			/*if (gStillCB.cap.CurViewMode == AUDIO_MODE)
			{
				appStateChange(APP_STATE_AUDIO_REC, STATE_PARAM_NORMAL_INIT);
			}*/
			else if (gStillCB.cap.CurViewMode == VIDEO_MODE)
			{
				appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
			}
			else
			{
				appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
			}
			break;
		}
#endif

	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_FisrstItemProcess (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_LastItemProcess (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32
appMenuEvt_ImageSizeSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
	//UINT32 mode ;
	//sp5kModeGet(&mode);


	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			/*appStill_SetResolution(menuProcCurrItemGet(layer));*/   /*junqiong.hu@20121121*/
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp();
			/*appStill_SetResolution(menuProcCurrItemGet(layer));*/  /*junqiong.hu@20121121*/
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_MENU:
			/*appStill_SetResolution(puiPara->ImageSize);*/  /*junqiong.hu@20121121*/
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appStill_SetResolution(pViewParam->stillSize);
			appStill_SetDateStamp(pViewParam->stillStamp, pUiSetting->dateTime);
			appStill_SetQuality(pViewParam->stillQuality);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}


	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32
appMenuEvt_ImageQualitySet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appStill_SetQuality(pViewParam->stillQuality);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_StillStampModeSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appStill_SetDateStamp(pViewParam->stillStamp, pUiSetting->dateTime);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;

}

UINT32
appMenuEvt_PhotoBurstSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
#if SP5K_SENSOR_DUAL_MODE
#else
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
//	layer=layer;
//	item=item;
//	param=param;

	switch(msg){
		case APP_KEY_PRESS_SET:

			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appStill_SetDriveMode(pViewParam->stillDriverMode);
			break;
	}

	return ret;
#endif
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32
appMenuEvt_AEMeteringSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetMetering(puiPara->AEMetering);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32
appMenuEvt_WBModeSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			appStill_SetWB(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp();
			appStill_SetWB(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_MENU:
			appStill_SetWB(puiPara->WBMode);
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appStill_SetWB(pViewParam->wb);
			break;
	}
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32
appMenuEvt_IsoSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetISO(puiPara->Iso);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;

}
UINT32 appMenuEvt_AEModeSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			appStill_SetExposure(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp();
			appStill_SetExposure(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
		case APP_KEY_PRESS_MENU:
			appStill_SetExposure(puiPara->AEMode);
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appEZ_menuParaChange();
			appStill_SetExposure(pViewParam->ev);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_ParkingModeSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
	//UINT32 mode ;
#if 0 /*ez mark*/

	sp5kModeGet(&mode);

	switch (msg) {
		case APP_KEY_PRESS_SET:
			if(mode == SP5K_MODE_VIDEO_RECORD)
			{
				ret = menuProcVidRecAlwaysParaExcute(layer, item, msg,puiPara->ParkingMode);
			}
			else
			{
				menuProcParaSet(layer, item);
				appVideo_SetParkingMode(puiPara->ParkingMode);
			}
			break;
	}
#endif
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_AEBSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
#if 0 /*ez mark*/

	switch (msg) {
		case APP_KEY_PRESS_SET:
			if(puiPara->DriverMode == UI_DRIVE_OFF)
			{
				menuProcParaSet(layer, item);
				appStill_SetAEB(puiPara->AEB);
			}
			if(puiPara->AEB != UI_AEB_OFF) /*add for mantis bug 0047495*/
			{
				puiPara->FaceTracking = UI_FACETRACK_OFF;
			}
			menuProcParent();
			menuProcDisp();

			ret = MENU_IGNORE_DEFAULT;
			break;
	}
#endif
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/

UINT32
appMenuEvt_SharpnessSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetSharpness(puiPara->Sharpness);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;

}


/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_VideoDistSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			/*If stabilizer enabled,face tracking  will be disable*/
			break;
	}
	return ret;

}
/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_FaceTrackingSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();

//	layer=layer;//just for clearing waring
//	item=item;//just for clearing waring
#if 0 /*ez mark*/

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			/*If FaceTracking enabled,stabilizer  will be disable*/
			if(puiPara->FaceTracking != UI_FACETRACK_OFF){
				puiPara->Stabilizer = UI_STABILIZER_OFF;
			}
			if((puiPara->FaceTracking == UI_FACETRACK_BLINKING_DETECT)||(puiPara->FaceTracking == UI_FACETRACK_SMILE_DETECT))
			/*junqiong.hu@20121127*/
			{
				puiPara->AutoReviewTime = QV_TIME_OFF;
				puiPara->DriverMode = UI_DRIVE_OFF;
				appStill_SetDriveMode(puiPara->DriverMode);
				puiPara->AEB = UI_AEB_OFF;
				appStill_SetAEB(puiPara->AEB);
			}
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
#endif
	return ret;
}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_PhotoFrameSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown();
			menuProcDisp();
			/*ez mark*/
			//appStill_SetPhotoFrame(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_UP:
			menuProcMoveUp();
			menuProcDisp();
			/*ez mark*/
			//appStill_SetPhotoFrame(menuProcCurrItemGet(layer));
			ret=MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
		case APP_KEY_PRESS_MENU:
		     /*ez mark*/
			//appStill_SetPhotoFrame(puiPara->PhotoFrame);
			break;
	}
	return ret;

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_DigitalZoomSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetDZoom(puiPara->DigitalZoom);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;

}

/**************************************************************************
 *                                                                        *
 *  Function Name:                                                        *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments:                                                            *
 *                                                                        *
 *  Returns: None                                                         *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
UINT32
appMenuEvt_WDRModeSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp();

			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;

}

UINT32
appMenuEvt_MDModeSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
	//UINT32 mode ;
	//sp5kModeGet(&mode);
	switch (msg) {
		#if SP5K_MOTION_DETECT
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appMotionDetectSet(APP_MD_EN, item);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
		#endif
	}
	return ret;
}

UINT32
appMenuEvt_ADASSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
#if TEST_ADAS
	//uiPara_t* puiPara = appUiParaGet();
	//UINT32 mode ;
	//sp5kModeGet(&mode);
	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
#endif
	return ret;
}

#if TIMELAPSE_EN
UINT32
appMenuEvt_TimeLapseSet(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 para
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStill_SetTimeLapse(puiPara->TimeLapse);
			if(puiPara->TimeLapse != UI_TIMELAPSE_OFF)
			{
				printf("\033[35m appMenuEvt_TimeLapseSet cvr \033[0m\n");
				puiPara->ImageSize = UI_PIC_SIZE_2M_169;
				appStill_SetResolution(puiPara->ImageSize);
				puiPara->VideoFormat = VIDEO_FORMAT_MJPG;
				puiPara->PhotoBurst=UI_PHOTOBURST_OFF;

			}
			else
			{
				puiPara->ImageSize = UI_PIC_SIZE_FULL;
				appStill_SetResolution(puiPara->ImageSize);
				puiPara->VideoFormat= VIDEO_FORMAT_H264;
			}
			menuProcParent();
			menuProcDisp();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;
}
#endif

UINT32
appMenuEvt_PbFileProtect (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selImgProtect= puiPara->selImgProtect;
			gpbMainCb->mainStat = PB_MAIN_STAT_IMG_PROT_SUSPEND;
			APP_OSD_REFRESH_OFF;
			CLEAR_OSD_SRC;
			//appPlayOsd_ImagePlay(); /*ez mark*/
			APP_OSD_REFRESH_ON;
			appStateChange(APP_STATE_PB_IMG_PROT, STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}
UINT32 appMenuEvt_PbFileDelete (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbMainCb->isDelHotKey= FALSE;
			gpbUiParam->selImgDelete= puiPara->selImgDelete;
			gpbMainCb->mainStat = PB_MAIN_STAT_IMG_DEL_SUSPEND;
			APP_OSD_REFRESH_OFF;
			CLEAR_OSD_SRC;
			//appPlayOsd_ImagePlay(); /*ez mark*/
			APP_OSD_REFRESH_ON;
			appStateChange(APP_STATE_PB_IMG_DEL, STATE_PARAM_NORMAL_INIT);
			break;
	}

	return ret;

}


#if 0 /*ez mark*///!(CAM_TYPE_CVR||CAM_TYPE_SBC)
UINT32
appMenuEvt_PbSlideCfg(
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;

	layer = layer; /* just for clearing waring */
	item = item; /* just for clearing waring */
	param = param; /* just for clearing waring */
	switch(msg){
		case APP_KEY_PRESS_SET:
			menuProcChild(0);
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}


UINT32
appMenuEvt_PbSlideStart (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
#if 0 /*ez mark*/
	UINT8 ret = MENU_CALL_DEFAULT;
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			if (item==PB_UI_SLIDE_START)
			{
				gpbMainCb->mainStat = PB_MAIN_STAT_SLIDE_SUSPEND;
				CLEAR_OSD_SRC;
				/*appBeep(TRUE, FALSE);*/  /*junqiong.hu@20121226  for mantis bug 0046650*/
				appStateChange(APP_STATE_PB_SLIDE_SHOW, STATE_PARAM_NORMAL_INIT);
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
#endif
}

UINT32
appMenuEvt_PbSlideInterval (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			gpbUiParam->selSlideInteval=puiPara->selSlideInteval;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			gpbUiParam->selSlideInteval=puiPara->selSlideInteval;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;

}

UINT32
appMenuEvt_PbSlideMusic (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
#if 0 /*ez mark*/
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();

	switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();

			#if SOUND_PLAY_NEW_PROC
			appReleaseSound(slideSoundID[puiPara->selSlideMusic]);
			#else
			appSoundTerminate();
			#endif

			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();

			#if SOUND_PLAY_NEW_PROC
			appReleaseSound(slideSoundID[puiPara->selSlideMusic]);
			#else
			appSoundTerminate();
			#endif

			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			#if SOUND_PLAY_NEW_PROC
			appReleaseSound(slideSoundID[puiPara->selSlideMusic]);
			#else
			appSoundTerminate();
			#endif

			if (puiPara->selSlideMusic>SLSHOW_NONE)
			{
			#if SOUND_PLAY_NEW_PROC
				appAddSoundToPlay(slideSoundID[puiPara->selSlideMusic]);
			#else
				if (FAIL==appSoundLoad(pszSlideMusic[puiPara->selSlideMusic]))
				{
					printf("%s: !!FATAL!!, sound load failed \n", __FUNCTION__);
					return ret;
				}
				if (FAIL==appSoundPlay())
				{
					printf("%s: !!FATAL!!, sound play failed \n", __FUNCTION__);
					return ret;
				}
			#endif
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			#if SOUND_PLAY_NEW_PROC
			appReleaseSound(slideSoundID[puiPara->selSlideMusic]);
			#else
			appSoundTerminate();
			#endif

			if (puiPara->selSlideMusic>SLSHOW_NONE)
			{
			#if SOUND_PLAY_NEW_PROC
				appAddSoundToPlay(slideSoundID[puiPara->selSlideMusic]);
			#else
				if (FAIL==appSoundLoad(pszSlideMusic[puiPara->selSlideMusic]))
				{
					printf("%s: !!FATAL!!, sound load failed \n", __FUNCTION__);
					return ret;
				}
				if (FAIL==appSoundPlay())
				{
					printf("%s: !!FATAL!!, sound play failed \n", __FUNCTION__);
					return ret;
				}
			#endif
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
#endif
}

UINT32
appMenuEvt_PbSlideTransition (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			gpbUiParam->selSlideTrans=puiPara->selSlideTrans;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			gpbUiParam->selSlideTrans=puiPara->selSlideTrans;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;

}


UINT32
appMenuEvt_PbSlideRepeat (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_UP:
			menuProcMoveUp ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;

		case APP_KEY_PRESS_DOWN:
			menuProcMoveDown ();
			menuProcDisp2Layer();
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_LEFT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveUp();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			gpbUiParam->selSlideRepeat=puiPara->selSlideRepeat;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_RIGHT:
			menuProcChild (menuProcParaGet(layer, item));
			menuProcMoveDown();
			layer = menuProcLayerGet();
			item = menuProcCurrItemGet(layer);
			menuProcParaSet(layer, item);
			menuProcParent();
			menuProcDisp2Layer();

			gpbUiParam->selSlideRepeat=puiPara->selSlideRepeat;
			ret = MENU_IGNORE_DEFAULT;
			break;
		case APP_KEY_PRESS_SET:
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;

}
#endif

UINT32
appMenuEvt_PbCopyToCard (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();
	//pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	//pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();
#if 0 /*ez mark*/

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selCopy2Card= puiPara->selCopy2Card;
			gpbMainCb->mainStat = PB_MAIN_STAT_C2C_SUSPEND;
			CLEAR_OSD_SRC;
			appStateChange(APP_STATE_PB_C2C, STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
#endif

	return ret;
}

#if 0//!(CAM_TYPE_CVR||CAM_TYPE_SBC)
UINT32
appMenuEvt_PbPhotoEffect (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();
	pbImgCb_t *ppbImgCb = appPb_ImageCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selImgEffect= puiPara->selImgEffect;
			appPbEffectPreview(gpbUiParam->selImgEffect);
			if (FAIL==appPbEffectExecute(ppbImgCb)) {
				DBG_PRINT("Saving Effect(%d) failed \n", item);
				appPbErrorProcess(gpbMainCb->err);
				/*
				appStateChange(APP_STATE_PLAY_SUBMENU, STATE_PARAM_NORMAL_INIT);
				*/
				menuProcDisp();
				ret = MENU_IGNORE_DEFAULT;
				return ret;
			} else {
				appPbEffectDestroy(ppbImgCb);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			}
			if(gpbUiParam->selImgEffect == PB_UI_EFFECT_RED_EYE || gpbUiParam->selImgEffect ==PB_UI_EFFECT_SKIN_BEAUTY)
			{
				sp5kImageEditCallbackSet(NULL);
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;

}

UINT32
appMenuEvt_PbPhotoRotInit (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();
	param = param;
	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbMainCb->isDelHotKey= FALSE;
			gpbMainCb->mainStat = PB_MAIN_STAT_JPG_ROTATE;
			CLEAR_OSD_SRC;
			/*appPbImagePlay(gpbMainCb->curImgIdx);
			menuProcChild(0);
			menuProcDispYesNo(ID_STR_ROTATE);
			ret = MENU_IGNORE_DEFAULT;*/
			appStateChange(APP_STATE_PB_IMG_ROTATE, STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;
}

UINT32
appMenuEvt_PbPhotoRotate (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();
	pbImgCb_t *ppbImgCb = appPb_ImageCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selImgRotate= puiPara->selImgRotate;
			if (item==PB_UI_ROTATE_SELECT_SET)
			{
				if (FAIL==appPbRotatePreview(PB_ROT_CW))
				{
					appPbErrorProcess(gpbMainCb->err);
					appPbRotateDestroy(ppbImgCb);
					appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
					return MENU_IGNORE_DEFAULT;
				}
			}
			else if (item==PB_UI_ROTATE_SELECT_SAVE)
			{
				if (FAIL==appPbRotateExecute(ppbImgCb, PB_ROT_SAVE))
				{
					appPbErrorProcess(gpbMainCb->err);
				}
				appPbRotateDestroy(ppbImgCb);
				/*
				appStateChange(APP_STATE_PLAY_MAINMENU, STATE_PARAM_NORMAL_INIT);
				*/
				menuProcDisp();
				return MENU_IGNORE_DEFAULT;;
			}
			else if (item==PB_UI_ROTATE_SELECT_BACK)
			{
				appPbRotateExecute(ppbImgCb, PB_ROT_CANCEL);
				gpbMainCb->err = PB_ERR_NONE;
				appPbRotateDestroy(ppbImgCb);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				return MENU_IGNORE_DEFAULT;;
			}
			else
			{
				DBG_PRINT("\nFATAL: It should not happen !!\n");
				gpbMainCb->err = PB_ERR_ROTATE_ERROR;
				appPbRotateDestroy(ppbImgCb);
				/*
				appStateChange(APP_STATE_PLAY_MAINMENU, STATE_PARAM_NORMAL_INIT);
				*/
				menuProcDisp();
				return MENU_IGNORE_DEFAULT;
			}
			break;
	}

	return ret;

}


UINT32
appMenuEvt_PbPhotoResize (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();
	pbImgCb_t *ppbImgCb = appPb_ImageCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selImgResize= puiPara->selImgResize;
			if (FAIL==appPbResizeExecute(ppbImgCb, item))
			{
				DBG_PRINT("Saving RESIZE(%d) failed \n", item);
				appPbErrorProcess(gpbMainCb->err);
				menuProcDisp();
				return MENU_IGNORE_DEFAULT;
			}
			else
			{
				appPbResizeDestroy(ppbImgCb);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
				ret = MENU_IGNORE_DEFAULT;
			}
			break;
	}

	return ret;

}


UINT32
appMenuEvt_PbPhotoVoiceMemo (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			if(gpbImgCb.isProtected)  /*add for mantis bug 0047522*/
			{
				gpbMainCb->err = PB_ERR_IMG_PROTECTED;
				appPbErrorProcess(gpbMainCb->err);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			}
			else
			{
				gpbUiParam->selVoiceMemo= puiPara->selVoiceMemo;
				gpbMainCb->mainStat = PB_MAIN_STAT_MEMOREC_SUSPEND;
				CLEAR_OSD_SRC;
				appStateChange(APP_STATE_PB_MEMO_REC, APP_UI_MSG_PB_MEMO_REC_CREATE);
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;

}


UINT32
appMenuEvt_PbStartImageSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selStartImage= puiPara->selStartImage;
			gpbMainCb->mainStat = PB_MAIN_STAT_START_IMG_SUSPEND;
			CLEAR_OSD_SRC;
			appStateChange(APP_STATE_PB_START_IMG, STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;

}


UINT32
appMenuEvt_PbPhotoFrame (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	uiPara_t* puiPara = appUiParaGet();
	pbUIParam_t* gpbUiParam=appPb_UiParamPtrGet();
	pbMainCB_t* gpbMainCb=appPb_MainCbPtrGet();
	pbImgCb_t *ppbImgCb = appPb_ImageCbPtrGet();

	switch (msg) {
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			gpbUiParam->selPhotoframe= puiPara->selPhotoframe;
			if (FAIL==appPbPhotoframeExecute(ppbImgCb, item)) {
				DBG_PRINT("Saving Photoframe(%d) failed \n", item);
				appPbErrorProcess(gpbMainCb->err);
				//appPbPhotoframeDestroy(ppbImgCb);
				menuProcDisp();
				return MENU_IGNORE_DEFAULT;
			} else {
				appPbPhotoframeDestroy(ppbImgCb);
				appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			}
			ret = MENU_IGNORE_DEFAULT;
			break;
	}

	return ret;

}
#endif

UINT32
appMenuEvt_FileTypeSet (
	UINT8 layer,
	UINT8 item,
	UINT32 msg,
	UINT32 param
)
{
	UINT8 ret = MENU_CALL_DEFAULT;
	//uiPara_t* puiPara = appUiParaGet();

	switch (msg)
	{
	#if defined(PRJ_PDVR)
		case APP_KEY_PRESS_MENU:
			printf("appMenuEvt_FileTypeSet\n");
			fileTypeSelect = 0;/*ez_menu for pb filetype */
			appChangeState_Video();
			appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);

			ret = MENU_IGNORE_DEFAULT;
			
			break;
	#endif
		case APP_KEY_PRESS_SET:
			menuProcParaSet(layer, item);
			appStateChange(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
			ret = MENU_IGNORE_DEFAULT;
			break;
	}
	return ret;

}

