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
#ifndef APP_VIDEO_API_H
#define APP_VIDEO_API_H
#include "app_aaa.h"
#include "customization/app_init.h"

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	OSD_VIDEO_SIMPLE, OSD_VIDEO_VERBOSE, OSD_VIDEO_NONE
};

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
enum {
	APP_VIDEO_MODE_DUAL,	/*Daul Sensor + PIP display*/
	APP_VIDEO_MODE_SINGLE	/*Daul Sensor + single display*/
};
enum {
	APP_VIDEO_DUAL_FILE,	/*Dual + no MultiTrack*/
	APP_VIDEO_SINGLE_FILE	/*Dual + MultiTrack*/
};
#endif
enum{
	EVENT_TRIG_DONE,
	EVENT_TRIG_OPEN,
	EVENT_TRIG_CLOSE,
	EVENT_TRIG_DUAL
};

#define OSD_VIDEO_DEFAULT	OSD_VIDEO_SIMPLE

#define CAN_I_START_RECORD	0
#define CAN_I_LEAVE_VIDEO	1
#define VIDEO_STATE_YES     0
#define VIDEO_STATE_NO 	    1

#define VIDEO_REC_STAMP

#define VIDEO_AUDIO_BIT_RATE        128000
#define VIDEO_AUDIO_SAMPLE_RATE	    AUDIO_SAMPLE_RATE
#define VIDEO_AUDIO_SAMPLE_RATE_RTP	AUDIO_SAMPLE_RATE/2 /* notes: VIDEO_AUDIO_SAMPLE_RATE % VIDEO_AUDIO_SAMPLE_RATE_RTP == 0, and CANNOT odd value, some value such 14700Hz would assert */
#define VIDEO_AUDIO_SAMPLE_BIT	    16
#if SPCA6330 && !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
#define VIDEO_AUDIO_CHANNEL		    2
#else
#define VIDEO_AUDIO_CHANNEL		    1 /*Mono*/
#endif
#define VIDEO_AUDIO_CODEC        SP5K_MEDIA_AUDIO_PCM

#define VIDEO_5_FRAME_RATE		    5
#define VIDEO_10_FRAME_RATE		    10
#define VIDEO_15_FRAME_RATE		    15
#if defined(PRJ_PDVR)
#define VIDEO_275_FRAME_RATE        27.5
#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define VIDEO_28_FRAME_RATE		    28
#define VIDEO_14_FRAME_RATE		    14
#endif
#define VIDEO_30_FRAME_RATE		    30
#define VIDEO_60_FRAME_RATE		    60
#define VIDEO_120_FRAME_RATE		120
#define VIDEO_240_FRAME_RATE		240
#define VIDEO_1080P_HEIGHT			1080

#define AVI_HDR_KB	                8
#define APP_CAPTURE_IN_MOVIE	    16384	/* 8 * 2 * 1024 */
#define APP_THRESHOLD_SPACE	        10240
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
#define DUAL_VIDEO_1080_WIDTH       1920
#define DUAL_VIDEO_1080_HEIGHT      1080
#define DUAL_VIDEO_720_WIDTH        1280
#define DUAL_VIDEO_720_HEIGHT       720
#define DUAL_VIDEO_VGA_WIDTH        640
#define DUAL_VIDEO_VGA_HEIGHT       480
#define DUAL_VIDEO_QHD_WIDTH        960
#define DUAL_VIDEO_QHD_HEIGHT       540
#endif

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appVideoCB_s {
	UINT32 CapTimer;
	UINT32 CapSec;
	UINT8 osd;
	int fCapture;
	int fPause;
	int fStop;
	UINT32 isDiskReady;
	UINT32 err;
	UINT32 streamBytesPerSec;
	UINT32 aviKbPerSec;
	UINT32 isAutoPowerOffOn;
} appVideoCB_t;




/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define VIDEO_SET_CAPTURE_MODE		(1<<0)
#define VIDEO_SET_CAPTURE_SIZE		(1<<1)
#define VIDEO_SET_ALL				0xffffffff

/*seamless cdfs control*/
#if defined(PRJ_17DVR) ||defined(PRJ_19DVR)
#define EVENT_DEL_VIDEO_FILE			0x01
#define EVENT_DEL_GEVENT_FILE			0x02
#define EVENT_DEL_FILE_BY_AVN			0x04
#define EVENT_DEL_FILE_ALL			    (EVENT_DEL_VIDEO_FILE | EVENT_DEL_GEVENT_FILE | EVENT_DEL_FILE_BY_AVN)
#else
#define EVENT_VIDEO_FILE_ADD			0x01
#define EVENT_VIDEO_FILE_DEL			0x02
#define EVENT_VIDEO_CHECK_SPCAE			0x04
#define EVENT_VIDEO_ALL				    (EVENT_VIDEO_FILE_ADD | EVENT_VIDEO_FILE_DEL | EVENT_VIDEO_CHECK_SPCAE)
#endif

#define VIDEO_SIZE_REACH_4G				0x4 
#define ALIGN_16                        16
#define INTERVAL_FRAMES_COMES           1
#define NUMBER_ONE_STREAM               1
#define NUMBER_TWO_STREAMS              2
#define BLEND_ALPHA                     0x80000000


/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
extern appVideoCB_t gVideoCB;
extern UINT8 HDR_En;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appVideoFifoNumCalculate(UINT32 expectTime, UINT32 streamNum);
UINT32 appVideoSeamLessSecCriticalGet(UINT32 state);
void appVideoSettingInit(UINT32 flg);
void appVideoFormatSet(void);
void appVideoSizeSet(void);
void appVideoFifoSet(void);
void appVideoSetQuality(void);
UINT32 appVideoSizeGet(UINT8 size_idx, UINT32 *pW, UINT32 *pH, UINT32 *pfrmrate);
UINT32 appVideoRemainSecGet();

void appVideoOsdShow(UINT8 clean, UINT8 outInOsdFreeze, UINT8 osd);
void appVideoOsdDiskShow(BOOL bShow);

void AppVideoSeamlessCallback(void);
UINT32 appVideoSeamLessSecGet(UINT32 state);
void appVideoSeamLessTimeSet(UINT32 idx);
void appSeamlessDelFileEvent(void);
void AppVideoSeamlessCB(void);
void appVideoLogFileInit(void);
void appVideoLogFileSave(void);
void appVideoLogFile_Print(const char *format, ...);
void appVideoStop(void);
#if SP5K_USB_UVC_SUPPORT
void appVideoSizeParamSet(UINT32 vidSizeParam);
#endif
void appVideoDateStampResSet(void);

#ifdef VIDEO_REC_STAMP
void appVideoRecStampInit(UINT32 mode, UINT32 roix, UINT32 roiy);
UINT32 appVideoRecStampCbReg(UINT32 mode);
UINT32 appVideoRecStampCbUnReg(void);
void appVideoRecStampUpdate(void);
void appVideoRecStampExit(void);

#if (SP5K_VREC_YUV_SAVE)
void appVideoYuvSaveStart(void);
void appVideoYuvSaveStop(void);
void appVideoYuvFlagSet(UINT8 en);
#endif
#endif

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
void appVideoDualStreamCfgSet(UINT8 cfg);
void appVideoDualStreamStart(void);
void appVideoDualStreamStop(void);
#if SINGLE_DUAL_SWITCH_SAMPLE
void appSwitchSingleInDual(BOOL bSingle);
UINT8 appSingleModeCheck(void);
void appAutoSwitchUI(void);
#endif
#endif
#if SP5K_DIQ_HDR
void appVideoHDRModeSetting(UINT8 videoHDRenable);
#endif

/* For Multi-track video file use */
UINT32 appVideoMultiTrackSet(UINT8 bEnable);
UINT8 appVideoMultiTrackGet(void);
void appReset4k2kSetting(void);
void appResetCmdSetting();
UINT32 appVideoFpsGet(void);

#if SP5K_ICAT_EIS
void appVideoRSCExpTimeGet(void);
void appVideo_SetRscEis(UINT8 RSC_en,UINT8 EIS_en,UINT32 EIS_mode);
#endif

/*void App_videoGPSDataSave(void); //for gps demo*/
#endif  /* APP_VIDEO_API_H */

