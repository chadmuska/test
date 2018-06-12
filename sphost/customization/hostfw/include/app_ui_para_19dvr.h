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
 * @file		app_ui_para.h
 * @brief		App for UI setting parameter definition
 * @author	Phil Lin
 * @since		2008-02-20
 * @date		2008-02-20
 */
#ifndef __APP_UI_PARA_CVR_H__
#define __APP_UI_PARA_CVR_H__
/*
 //=============================================================================
//Header file
//=============================================================================

//=============================================================================
//Symbol
//=============================================================================
//-----------------------------------------------------------------------------
//Constant
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Type definition
//-----------------------------------------------------------------------------
*/
typedef struct uiPara_s {	/* Can't be larger than 512Bytes */
/* Note: Don't add any struct member at here !!! */
/* ------ Note: Auto create code zone start flag, don't modify or delete this line!!! ------ */
	UINT8 EmptyPara;		/* Auto create code zone, don't modify or delete! */
	UINT8 ImageSize;		/* Auto create code zone, don't modify or delete! */
	UINT8 ImageQuality;		/* Auto create code zone, don't modify or delete! */
	UINT8 PhotoBurst;		/* Auto create code zone, don't modify or delete! */
	UINT8 StillStampMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 VideoSize;		/* Auto create code zone, don't modify or delete! */
	UINT8 VideoQuality;		/* Auto create code zone, don't modify or delete! */
	UINT8 Seamless;		/* Auto create code zone, don't modify or delete! */
	UINT8 VideoStampMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 VideoEISMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 selImgProtect;		/* Auto create code zone, don't modify or delete! */
	UINT8 selImgDelete;		/* Auto create code zone, don't modify or delete! */
	UINT8 ShutterSound;		/* Auto create code zone, don't modify or delete! */
	UINT8 StartupSound;		/* Auto create code zone, don't modify or delete! */
	UINT8 BeepSound;		/* Auto create code zone, don't modify or delete! */
	UINT8 Volume;		/* Auto create code zone, don't modify or delete! */
	UINT8 VoiceRec;		/* Auto create code zone, don't modify or delete! */
	UINT8 WBMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 LightFreq;		/* Auto create code zone, don't modify or delete! */
	UINT8 SleepTime;		/* Auto create code zone, don't modify or delete! */
	UINT8 powerOnRecord;		/* Auto create code zone, don't modify or delete! */
	UINT8 PeriodicalCap;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcYear;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcMonth;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcDay;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcHour;		/* Auto create code zone, don't modify or delete! */
	UINT8 rtcMinute;		/* Auto create code zone, don't modify or delete! */
	UINT8 DateStyle;		/* Auto create code zone, don't modify or delete! */
	UINT8 Language;		/* Auto create code zone, don't modify or delete! */
	UINT8 TVMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 USBMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 VideoFormat;		/* Auto create code zone, don't modify or delete! */
	UINT8 SceneMode;		/* Auto create code zone, don't modify or delete! */
	UINT8 fileType;		/* Auto create code zone, don't modify or delete! */
/* ------ Note: Auto create code zone end flag, don't modify or delete this line!!! ------ */
/* Note: You can add struct member after this line !!! */
	UINT8 DriverMode;
	UINT8 VideoDist;
	UINT8 InvertMode;
	UINT8 WDRMode;
	UINT8 GPS;
	UINT8 LOGOMark;
	UINT8 CollisionDetection;
	UINT8 TimeZones;
	UINT8 AEMode;
	UINT8 ImageEdge;
	UINT8 flash;
	UINT8 resCreate;
	UINT8 BlinkMode;	
	UINT8 redEyePb;
	UINT8 tpCorrect;
	UINT8 shutter;
	UINT8 soundcontrol;
	UINT8 af;
	UINT8 batteryType;
	UINT8 fileNumber;
	UINT16 dcfDirKey;
	UINT16 dcfFileKey;
	UINT8 OpenPic;
	UINT8 AEMetering;
	UINT8 Sharpness;
	UINT8 ImageEffect;
	UINT8 FeatureEffect;
	UINT8 FaceTracking;
	UINT8 Digit;
	UINT8 RollingShutter;
	UINT8 VideoFaceTracking;
	UINT8 DiaMode;
	UINT8 AdvanceDenoise;
	UINT8 AutoReviewTime;
	UINT8 Iso;
	UINT8 AEB;
	UINT8 PhotoFrame;
	UINT8 DigitalZoom;
	UINT8 DisMode;
	UINT8 MediaFormat;
	UINT8 selSlideMusic;
	UINT8 selSlideTrans;
	UINT8 selSlideRepeat;
	UINT8 selCopy2Card;
	UINT8 selImgEffect;
	UINT8 selImgRotate;
	UINT8 selImgResize;
	UINT8 selVoiceMemo;
	UINT8 selStartImage;
	UINT8 selPhotoframe;
	UINT8 selSlideInteval;
	UINT8 CurViewMode; /* PASM ,Scene ,Video, Audio */
	UINT8 CurScene; /* Current scene */
	UINT8 ParkingMode;
	UINT8 vidrsc;
	UINT8 Stabilizer;
	UINT8 DateStamp;
	#if ICAT_WIFI
	UINT8 FirstWifiOn;  /* first open wifi after update FW */
	#endif
	UINT8 tzSigned;
	UINT8 tzHour;
	UINT8 tzMin;
/* Note: Next is checksum , don't add anything below */
	UINT8 iCatchFlag[6];
	UINT32 Checksum;
} uiPara_t;


typedef enum uiParaInitMode_e {
	PARA_INIT_WITH_DEF = 0x00,
	PARA_INIT_FROM_UDF = 0x01,
} uiParaInitMode_t;

typedef enum uiItemOrder_e {
	ITEM_EMPTY = 0x80,
	ITEM_FIRST = 0x81,
	ITEM_MAX   = 0xfe,
	ITEM_NULL  = 0xff
} uiItemOrder_t;
/*
//-----------------------------------------------------------------------------
//Variable
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//Function declare
//-----------------------------------------------------------------------------*/
void appRTCUiParaInit(void);
void appUiParaInit(uiParaInitMode_t mode);
UINT32 appUIParaSave(void);

#endif /*__APP_UI_PARA_CVR_H__*/
