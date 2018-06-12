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


#ifndef APP_USER_SETTING_H
#define APP_USER_SETTING_H
#include "app_menu.h"
#include "app_cdfs_api.h"
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

typedef enum {
	APERTURE_AUTO,
	APERTURE_2_8,
	APERTURE_5_6,
	APERTURE_MAX,
} aperture_e;
#define APERTURE_DEFAULT	APERTURE_AUTO

typedef enum {
	SHUTTER_AUTO,
	SHUTTER_16S,
	SHUTTER_8S,
	SHUTTER_4S,
	SHUTTER_2S,
	SHUTTER_1S,
	SHUTTER_1_2S,
	SHUTTER_1_4S,
	SHUTTER_1_8S,
	SHUTTER_1_15S,
	SHUTTER_1_20S,
	SHUTTER_1_24S,
	SHUTTER_1_30S,
	SHUTTER_1_60S,
	SHUTTER_1_125S,
	SHUTTER_1_250S,
	SHUTTER_1_500S,
	SHUTTER_1_1000S,
	SHUTTER_1_2000S,
	SHUTTER_1_4000S,
	SHUTTER_1_8000S,
	SHUTTER_1_16000S,
	SHUTTER_MAX,
} shutter_e;
#define SHUTTER_DEFAULT		SHUTTER_AUTO

typedef enum {
	PASM_PROGRAM,
	VIDEO_MODE,
	PASM_MAX,
} pasm_e;
#define PASM_DEFAULT		PASM_PROGRAM


typedef enum {
	SETUP_FIRSTMENU,
	SETUP_SETUPMENU,
	SETUP_MENUCHANGE_MAX,
} setupMenuChange_e;

typedef enum {
	SOUNDS_SHTTUER,
	SOUNDS_STARTUP,
	SOUNDS_BEEP,
	SOUNDS_VOULUME,
	SOUNDS_MAX,
} soundsMode_e;

typedef enum {
	SHUTTER_SOUND_OFF,
	SHUTTER_SOUND_ON,
	SHUTTER_SOUND_MAX,
} shutterSound_e;
#define SHUTTER_SOUND_DEFAULT  SHUTTER_SOUND_ON

typedef enum {
	POWERON_SOUND_NONE,
	POWERON_SOUND_1,
	POWERON_SOUND_MAX,
	POWERON_SOUND_2,
	POWERON_SOUND_3,
} powerOnSound_e;
#define POWERON_SOUND_DEFAULT  POWERON_SOUND_1

typedef enum {
	BEEP_OFF,
	BEEP_ON,
	BEEP_MAX,
} beepSound_e;
#define BEEP_DEFAULT	BEEP_ON

typedef enum {
	SOUND_VOLUME_OFF,
	SOUND_VOLUME_LOW,
	SOUND_VOLUME_MIDDLE,
	SOUND_VOLUME_HIGH,
	SOUND_VOLUME_MAX,
} soundVolume_e;
#define SOUND_VOLUME_DEFAULT	 SOUND_VOLUME_MIDDLE

typedef enum {
	LIGHT_FREQ_50HZ,
	LIGHT_FREQ_60HZ,
	LIGHT_FREQ_MAX,
	LIGHT_FREQ_AUTO,
} lightFreq_e;
#define LIGHT_FREQ_DEFAULT   LIGHT_FREQ_50HZ

typedef enum {
	POWER_SAVE_OFF,
	POWER_SAVE_1MIN,
	POWER_SAVE_MAX,
	POWER_SAVE_3MIN,
	POWER_SAVE_5MIN,
} powerSave_e;
#define POWER_SAVE_DEFAULT	POWER_SAVE_OFF

typedef enum {
	POWER_ON_RECORD_OFF,
	POWER_ON_RECORD_ON,
	POWER_ON_RECORD_MAX,
} powerOnRecord_e;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define POWER_ON_RECORD_DEFAULT	POWER_ON_RECORD_ON
#else
#define POWER_ON_RECORD_DEFAULT	POWER_ON_RECORD_OFF
#endif
typedef enum {
	PERIODICAL_CAPTURE_OFF,
	PERIODICAL_CAPTURE_5SEC,
	PERIODICAL_CAPTURE_10SEC,
	PERIODICAL_CAPTURE_MAX,
	PERIODICAL_CAPTURE_30SEC,
	PERIODICAL_CAPTURE_60SEC,
	PERIODICAL_CAPTURE_TIME_LAPSE,
} periodicalCapture_e;
#define PERIODICAL_CAPTURE_DEFAULT  PERIODICAL_CAPTURE_OFF

typedef enum {
	DATE_TIME_YYMMDD,
	DATE_TIME_MMDDYY,
	DATE_TIME_DDMMYY,
	DATE_TIME_MAX,
} dateTimeType_e;
#define DATE_TIME_DEFAULT   DATE_TIME_YYMMDD

typedef enum {
	YEAR_SET,
	MONTH_SET,
	DAY_SET,
	HOUR_SET,
	MIN_SET,
	DATE_STYLE_SET,
	DAY_TIME_SET_MAX,
	PM_AM_SET,
} dateTimeSet_e;

typedef enum {
	LANG_ENGLISH,
	LANG_S_CHINESE,
	LANG_T_CHINESE,
	LANG_MAX,
	LANG_SPANISH,
	LANG_CZECH,
	LANG_DUTCH,
	LANG_FRENCH,
	LANG_GREEK,
	LANG_HUNGARY,
	LANG_ITALIAN,
	LANG_INDONESIAN,
	LANG_MALAY,
	LANG_POLISH,
	LANG_PORTUGESE,
	LANG_RUSSIAN,
	LANG_ROMANIAN,
	LANG_GERMAN,
	LANG_SWEDISH,
	LANG_TURKISH,
	LANG_CROATION,
	LANG_ARABIC,
	LANG_THAI,
	LANG_JAPANESE,
	LANG_KOREAN,
} language_e;
#define LANG_DEFAULT			LANG_ENGLISH

typedef enum {
#if defined(PRJ_PDVR)

	PB_FILETYPE_VIDEO,
	PB_FILETYPE_EVENT,	
	PB_FILETYPE_IMAGE,
#else
	PB_FILETYPE_IMAGE,
	PB_FILETYPE_VIDEO,
	PB_FILETYPE_EVENT,	

#endif
	PB_FILETYPE_MAX
} pbfileType_e;
#define PB_FILETYPE_DEFAULT	PB_FILETYPE_VIDEO

typedef enum {
	TV_NTSC,
	TV_PAL,
	TV_MAX,
} tv_e;
#define TV_DEFAULT	TV_NTSC

typedef enum {
	USB_MSDC,
	USB_UVCMJPG,
	USB_MAX,
	USB_UVCH264,
	USB_PICTBRIDGE,
} usb_e;
#define USB_DEFAULT  USB_MSDC

typedef enum {
	FORMAT_YES,
	FORMAT_NO,
	FORMAT_MAX,
} format_e;

typedef enum {
	RESET_YES,
	RESET_NO,
	RESET_MAX
} resetAll_e;

 typedef enum {
	BATT_AKALINE,
	BATT_NIMH,
	BATT_LITHIUM,
	BATT_OXYRIDE,
	BATT_TYPE_MAX
} battType_e;
#define BATTERY_TYPE_DEFAULT  BATT_LITHIUM

typedef enum {
	POWERON_LOGO_NONE,
	POWERON_LOGO_SYS,
	POWERON_LOGO_CUSTOM,
	POWERON_LOGO_MAX
} powerOnLogo_e;
#define POWERON_LOGO_DEFAULT  POWERON_LOGO_NONE

typedef enum {
	VIDEO_FORMAT_MJPG,
	VIDEO_FORMAT_H264,
	VIDEO_FORMAT_MAX,
} videoFormat_e;
#define VIDEO_FORMAT_DEFAULT  VIDEO_FORMAT_H264

typedef enum {
	MEDIA_FORMAT_AVI,
	MEDIA_FORMAT_MOV,
	MEDIA_FORMAT_MP4,
	MEDIA_FORMAT_MAX,

} mediaFormat_e;

/*Isaac change default Media REC type MOV to AVI */
#define MEDIA_FORMAT_DEFAULT  MEDIA_FORMAT_MP4

#if GPS_OPEN
typedef enum {
	GPS_TIMEZONE_WEST12,
	GPS_TIMEZONE_WEST11,
	GPS_TIMEZONE_WEST10,
	GPS_TIMEZONE_WEST9,
	GPS_TIMEZONE_WEST8,
	GPS_TIMEZONE_WEST7,
	GPS_TIMEZONE_WEST6,
	GPS_TIMEZONE_WEST5,
	GPS_TIMEZONE_WEST4,
	GPS_TIMEZONE_WEST3,
	GPS_TIMEZONE_WEST2,
	GPS_TIMEZONE_WEST1,
	GPS_TIMEZONE_0,
	GPS_TIMEZONE_EAST1,
	GPS_TIMEZONE_EAST2,
	GPS_TIMEZONE_EAST3,
	GPS_TIMEZONE_EAST4,
	GPS_TIMEZONE_EAST5,
	GPS_TIMEZONE_EAST6,
	GPS_TIMEZONE_EAST7,
	GPS_TIMEZONE_EAST8,
	GPS_TIMEZONE_EAST9,
	GPS_TIMEZONE_EAST10,
	GPS_TIMEZONE_EAST11,
	GPS_TIMEZONE_EAST12,
	GPS_TIMEZONE_MAX,
} gpsTimezone_e;
#define GPS_TIMEZONE_DEFAULT	GPS_TIMEZONE_0

typedef enum {
	GPS_OFF,
	GPS_ON,
	GPS_MAX,
} gpsStatus_e;
#endif
#define GPS_STATUS_DEFAULT	GPS_ON


#if defined(HW_PDVR)||defined(HW_17DVR)||defined(HW_19DVR)

typedef enum 
{
	GSENSOR_OFF,
	GSENSOR_LOW,
	GSENSOR_MEDIUE,
	GSENSOR_HIGH,
}GSensorLevelCtrl;


#define GSENSOR_STATUS_DEFAULT GSENSOR_OFF
#endif

#if defined(PRJ_PDVR)
typedef enum {
	HDR_OFF,
	HDR_ON,
	HDR_MAX,
} HDR_e;

#define HDR_STATUS_DEFAULT	HDR_ON
#endif
#define RTC_YEAR_MIN        2000
#define RTC_YEAR_MAX        2000 + 50

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct {
	UINT8 CurViewMode; /* PASM ,Scene ,Video, Audio */
	UINT8 CurScene; /* Current scene */
	UINT8 qvSec;
	UINT8 reserve;
} appCaptureSetting_t;

typedef struct uiSoundSetting_s {
	UINT8 shutter;
	UINT8 powerOn;
	UINT8 beep;
	UINT8 volume;
	UINT8 VoiceRec;
} uiSoundSetting_t;

typedef struct uiPbSlideSettings_s {
	UINT8 intvl;	/* slide interval time */
	UINT8 trans;	/* effect transition */
	UINT8 music;	/* background music */
	UINT8 isRepeat;
} uiPbSlideSettings_t;

typedef struct uiperiodicalCapSettings_s {
	UINT16 periodicalSet;
	UINT16 videoFileKey;
	UINT16 imgProcCnt;	/* if this parameter is not zero, means this action is not finish but poweroff first */
	UINT16 interval;
	UINT16 duration;
} uiperiodicalCapSettings_t;

/*if you add variable to struct,you must modify  relative file in \sphost\resource\B\USER_RES*/

typedef struct uiParamSetting_s
{
	appCaptureSetting_t capSetting;
	uiSoundSetting_t sound;
	uiPbSlideSettings_t pbSlideSetting;
	uiperiodicalCapSettings_t periodicalCap;
    UINT8 pbfileType;    
	UINT8 lightFrequency;
	UINT8 powerSave;
	UINT8 powerOnRecord;
	UINT8 dateTime;
	UINT8 language;
	UINT8 TV;
	UINT8 usbMode;
	UINT8 batteryType;
	UINT8 powerOnLogo;
	UINT8 videoFormat;
	UINT8 mediaFormat;
	UINT8 videoSize;
	UINT8 buzzerSet;
	UINT8 voiceSet;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT8 ParkingModesetting;
	UINT8 ParkingModereCordingTimeH;
	UINT8 ParkingModereCordingTimeL;
	UINT8 ParkingModebatteryVoltageS;
	UINT8 ParkingModegSensorLevel;
	UINT8 ParkingModeparkingEvntToAVN;
	UINT8 ParkingModebyPassTimeH;
	UINT8 ParkingModebyPassTimeL;
	UINT8 ParkingTrigger;
	UINT8 Gsensor;
	UINT8 GSensorComX;
	UINT8 GSensorComY;
	UINT8 GSensorComZ;
#endif
    #if GPS_OPEN
	UINT8 TimezoneSTATUS;
	UINT8 GPSSTATUS;
	#endif
	/*Isaac add 20161209*/
	#if defined(HW_PDVR)
	UINT8 GSENSOR;
	#endif

	#if defined(PRJ_PDVR)
	UINT8 HDR;
	#endif
} uiParamSetting_t;

typedef struct uiDPS_SET
{
	UINT8	nPrint;
	UINT8	nImage;
	UINT8	nQuality;
	UINT8	nLayout;
	UINT8	nPaperSize;
	UINT8	nPaperType;
	UINT8	nDate;
	UINT8	nFileNo;
	UINT8	nReset;
}uiDPS_SET,*PuiDPS_SET;

/*Isaac add for version number 20170104*/
#if defined(PRJ_PDVR) 

#define CUSTOMERVER "Ver 0.12"
#define VERSION	"Ver 0.12.1"

#endif
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
extern uiParamSetting_t *pUiSetting;
/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appUserSettingLoad(uiParamSetting_t *pbuf);
UINT32 appUserSettingSave(uiParamSetting_t *pbuf);
/*void appDcfUserSettingGet(fileNum_e *fnumRule, UINT16 * dirKey, UINT16 *fileKey);*/
void appFactorySettingApply(void);
#endif  /* APP_USER_SETTING_H */

