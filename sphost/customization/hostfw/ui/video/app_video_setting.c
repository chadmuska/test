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
 *  File name    : App_video_setting.c
 *  Function     : about video setting 		   
 *  First editor :   
 *  First date   :   
 *  History      :
 *               : 2018.05.23  Edward.Lu  "Add description and modify code"
 **************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_aud_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_dq_api.h"
#include "sp5k_media_api.h"
#include "sp5k_dzoom_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_media_api.h"
#include "sp5k_sensor_api.h"
#include "sp5k_dbg_api.h"
#include "middleware/common_types.h"
#include "sp5k_rsvblk_api.h"
#if SP5K_ICAT_EIS
#include "sp5k_eis_rsc_algo_api.h"
#include "../../drivers/gyro/app_gyro.h"
#include "api/sp5k_eis_rsc_algo_api.h"
#include "app_sys_cfg.h"
#include "sp5k_eis_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_rsc_api.h"
#endif

#include "app_pal.h"
#include "app_icon_def.h"
#include "app_video.h"
#include "app_sensor.h"
#include "app_user_setting.h"
#include "app_view_param.h"
#include "app_disk.h"
#include "app_still.h"
#include "app_calib_api.h"
#include "app_cdfs_api.h"
#include "app_awbalg_api.h"
#include "app_aealg_api.h"
#include "app_exif.h"
#if SP5K_SENSOR_DUAL_MODE
#include "app_spca7002_utility.h"
#include "app_dcf.h"
#endif

#if SP5K_USB_UVC_SUPPORT
#include "app_dcf.h"
#include "app_video_uvc.h"
#include "app_view_osd.h"
#endif
#include "api/sp5k_utility_api.h"
#include "api/sp5k_os_api.h"
#include "api/sp5k_msg_def.h"

#if SP5K_MOTION_DETECT ||  SP5K_MOTION_DETECT_FRONT || SP5K_MOTION_DETECT_REAR
#include "app_md.h"
#endif

#if defined(PRJ_PDVR)
#include "app_ui_para.h"
#elif defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_ui_para.h"
#endif

#if GPS_OPEN
#include "app_gps.h"
#endif

#if GSENSOR
extern void appGsensorOpen(void);
extern void appGsensorOff(void);
#endif

#if defined(PRJ_JETDVR) ||defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_uart2.h"
#include "app_infohandlerComm.h"
#include "sp5k_dbg_api.h"
#include "jet_utility.h"
#include "app_infohandler_feedback.h"
#endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern PowerDetect appInfoPowerDETStateGet(void);
extern BOOL CDFS_SET_JPG_FILENAME;
SP5K_EVENT_FLAGS_GROUP seamLessDelFileEvt;
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define SET_VIDEO_TYPE(x) \
	do { \
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, x);\
		videoFormat = x;\
	} while(0);
#define AUDIO_FPS                        40
#define ABS(a)                           (((a) >= 0) ? (a) : -(a))
#define READ8(a)                         (*((volatile UINT8*)(a)))
#define WRITE8(a,b)                      *(volatile UINT8*)(a) = (b)
#define READ16(a)                        (*((volatile UINT16*)(a)))
#define WRITE16(a,b)                     *(volatile UINT16*)(a) = (b)
#define ROUND_DOWN_TO_DIVISIBLE(num,div) ( (UINT32)(num) & -(UINT32)(div) )
#define ROUND_UP_TO_DIVISIBLE(num,div)   ROUND_DOWN_TO_DIVISIBLE( (UINT32)(num) + (div) - 1, div )
// Non-integer frame rate: b[31]:1, b[30-16]:denominator, b[15-0]:numerator
#define NON_INTEGER_FRAME_RATE           ((1<<31) | (10<<16) | 275) //0x800a0113 -> 27.5 fps
#define AUTO_WIDTH                       0
#define AUTO_HEIGHT                      0
#define DEFAULT_EXPECT_TIME              60

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 videoFormat;
void appAWBALG_WBParamSet(UINT32 selector,UINT32 value);

static unsigned char	TestFilePathF[32];
UINT32 TestEvtFlagF=EVENT_TRIG_DONE;

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
static unsigned char	TestFilePathR[32];
UINT32 TestEvtFlagR=EVENT_TRIG_DONE;
#endif
static struct {
	UINT16 w,h;
	UINT16 icon;
	UINT16 sensormode;
	UINT8  audio;
	UINT32 frameRate ;
    char*  datestamp;
} VideoSize[UI_VIDEO_SIZE_MAX]={
    #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
	#if defined(PRJ_19DVR)
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_14_FRAME_RATE ,RES_FONT_DATSTAMP},
    #else
	{ 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},
	{ 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},
	{ 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},
	#if SINGLE_DUAL_SWITCH_SAMPLE
        { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_60FPS, VIDEO_AUDIO_CODEC, VIDEO_60_FRAME_RATE ,RES_FONT_DATSTAMP},
        { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP}, /*HDR mode*/
#endif
	#endif
    #else
    #if defined(PRJ_PDVR)
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 1280,  720, ID_ICON_MOVIE_SIZE_720P , SENSOR_MODE_1080P_30FPS , VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP_24},
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_60FPS, VIDEO_AUDIO_CODEC, VIDEO_60_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 3840, 2160, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_4K_15FPS,    VIDEO_AUDIO_CODEC, VIDEO_15_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 2704, 1524, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_27K_30FPS,   VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},
    #elif defined(PRJ_17DVR)
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_28_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 1280,  720, ID_ICON_MOVIE_SIZE_720P , SENSOR_MODE_1080P_30FPS , VIDEO_AUDIO_CODEC, VIDEO_14_FRAME_RATE ,RES_FONT_DATSTAMP_24},
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_14_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 3840, 2160, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_4K_15FPS,    VIDEO_AUDIO_CODEC, VIDEO_15_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 2704, 1524, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_27K_30FPS,   VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},    
    #elif defined(PRJ_19DVR)/*weikai added*/
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_28_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 1280,  720, ID_ICON_MOVIE_SIZE_720P , SENSOR_MODE_1080P_30FPS , VIDEO_AUDIO_CODEC, VIDEO_14_FRAME_RATE ,RES_FONT_DATSTAMP_24},
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_14_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 3840, 2160, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_4K_15FPS,    VIDEO_AUDIO_CODEC, VIDEO_15_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 2704, 1524, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_27K_30FPS,   VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},    
    #else
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_60FPS, VIDEO_AUDIO_CODEC, VIDEO_60_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 1920, 1080, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_1080P_30FPS, VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 1280,  720, ID_ICON_MOVIE_SIZE_720P , SENSOR_MODE_720P_30FPS , VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP_24},
    { 3840, 2160, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_4K_15FPS,    VIDEO_AUDIO_CODEC, VIDEO_15_FRAME_RATE ,RES_FONT_DATSTAMP},
    { 2704, 1524, ID_ICON_MOVIE_SIZE_1080P, SENSOR_MODE_27K_30FPS,   VIDEO_AUDIO_CODEC, VIDEO_30_FRAME_RATE ,RES_FONT_DATSTAMP},
    #endif
    #endif
  };

UINT8 qualityRatio_h264[UI_VIDEO_SIZE_MAX][3]={

    #if SP5K_USB_UVC_SUPPORT
    {11,10,9},             //1920x1080 30fps + 1920x1080 30fps
    {11,10,9},             //1920x1080 30fps + 1280x720 30fps
    {11,10,9},
    #elif SP5K_SENSOR_DUAL_MODE
    {12,10,10},             //1920x1080 30fps
    {12,10,10},             //1920x1080 30fps
    {12,10,10},             //1920x1080 30fps
	#if SINGLE_DUAL_SWITCH_SAMPLE
        {22,16,16},             //1920x1080 60fps
        {17,8,12},             //1920x1080 30fps
	#endif
    #else
 #if defined(PRJ_PDVR)

    {10,8,6},             //1920x1080 30fps
    {12,10,8},	    		//1280x720  30fps
    {22,18,16},             //1920x1080 60fps
    {60,55,50},             //4k2k 15fps
    {60,55,50},             //27k 30fps
  #elif defined(PRJ_17DVR)||defined(PRJ_19DVR)

    {10,8,6},             //1920x1080 30fps
    {3,6,4},	    		//1280x720  14fps
     {7,8,6},             //1920x1080 14fps
    {60,55,50},             //4k2k 15fps
    {60,55,50},             //27k 30fps
 #else
    {22,18,16},             //1920x1080 60fps
    {17,14,12},             //1920x1080 30fps
    {12,10,8},	    		//1280x720  30fps
    {60,55,50},             //4k2k 15fps
    {60,55,50},             //27k 30fps
#endif
    #endif
}; //superfine ,fine ,normal

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
UINT8 qualityRatio_h264_2nd[UI_VIDEO_SIZE_MAX][3]={
    #if SP5K_USB_UVC_SUPPORT
    { 6, 6, 6},             //1920x1080 30fps + 1920x1080 30fps
    { 6, 6, 6},             //1920x1080 30fps + 1280x720 30fps
    { 6, 6, 6},
    #else
    {12,10, 8},    //1920x1080 30fps
    { 9, 8, 7},    //1280x 720 30fps
    { 6, 5, 4},    // 640x 480 30fps
    #endif
}; //superfine ,fine ,normal
#endif

/*superfine ,fine ,normal*/
UINT8 qualityRatio_mjp[UI_VIDEO_SIZE_MAX][3]={

    #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    {48,42,38},             //1920x1080 30fps + 1920x1080 30fps
    {48,42,38},             //1920x1080 30fps + 1280x720 30fps
    {48,42,38},
    #else
 #if defined(PRJ_PDVR)
    {48,42,38},             //1920x1080 30fps
    {48/2,42/2,38/2},	    //1280x720  30fps
    {49,43,39},             //1920x1080 60fps
    {60,60,60},             //3840x2160  15fps
    {60,60,60},             //27k  30fps
  #elif defined(PRJ_17DVR)||defined(PRJ_19DVR)
    {48,42,38},             //1920x1080 30fps
    {48/2,42/2,38/2},	    //1280x720  30fps
    {48,42,38},             //1920x1080 Parking
    {60,60,60},             //3840x2160  15fps
    {60,60,60},             //27k  30fps
 #else
    {49,43,39},             //1920x1080 60fps
    {48,42,38},             //1920x1080 30fps
    {48/2,42/2,38/2},	    //1280x720  30fps
    {60,60,60},             //3840x2160  15fps
    {60,60,60},             //27k  30fps
#endif
    #endif
};

/*ae/awb cb mapping rule*/
/*ae period ,awb period ,awb interval*/
/*
    {1, 2, 2},             //1080P
    {1, 2, 2},             //720P-30fps
    {2, 2, 2},             //720P-60FPS
    {4, 4, 4},             //640
    {4, 4, 4},             //320
*/
UINT8 ae_awb_config[UI_VIDEO_SIZE_MAX][3]={
   #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    {1, 2, 2},              //1920x1080 30fps
    {1, 2, 2},              //1920x1080 30fps
    {1, 2, 2},              //1920x1080 30fps
	#if SINGLE_DUAL_SWITCH_SAMPLE
        {1, 2, 2},              //1920x1080 30fps
        {1, 2, 2},              //1920x1080 30fps
	#endif
  #elif defined(PRJ_PDVR)
    {1, 2, 2},              //1920x1080 30fps
    {1, 2, 2},	            //1280x720  30fps
    {2, 4, 2},              //1920x1080 60fps
    {1, 2, 2},              //3840x2160  15fps
    {1, 2, 2},              //27k  30fps
 #elif defined(PRJ_17DVR)||defined(PRJ_19DVR)
    {1, 1, 2},              //1920x1080 30fps
    {1, 1, 2}, 	            //1280x720  30fps
    {1, 1, 2},              //1920x1080 Parking
    {1, 1, 2},               //3840x2160  15fps
    {1, 1, 2},               //27k  30fps   
 #else
    {2, 4, 2},              //1920x1080 60fps
    {1, 2, 2},              //1920x1080 30fps
    {1, 2, 2},	            //1280x720  30fps
    {1, 2, 2},              //3840x2160  15fps
    {1, 2, 2},              //27k  30fps
    #endif
};
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
BOOL gVideoRscEnTag=FALSE;
BOOL gVideoDistEnTag=FALSE;
#endif
UINT8 gVideoSizeTag =0;

//for ae debug
#define VIDEO_LOG 50    ///Make default size smaller for memory save
char videoDebugBuff[VIDEO_LOG];
char fname[32];
UINT16 videoLogCount =0;
static UINT16 wcount =0;

/*seamless cdfs control*/
#if SP5K_CDFS_OPT
#define measDiskTime          1000
#define seamLessDelayMs       13000
#define seamLessMeasDelayMs   2000
#define seamLessMeasDiskRatio 45 /*45%*/
#define seamLessMeasVfifoCnt  10 /*video fifo cnt*/
static UINT8 seamLessThrCreated = FALSE;
static SP5K_THREAD *seamLessDelFileThr=NULL;
SP5K_QUEUE seamlessFileAddQue;
#endif

static UINT8* pdstbuf=NULL;
static UINT32 ulYUVw=0, ulYUVh=0;
static BOOL bYUVcap=FALSE;

SP5K_THREAD *pPIVThread = NULL;
SP5K_QUEUE *pPIVMsgQueue = NULL;

 typedef struct {
	 UINT32 cmd;
	 UINT32 param;
 } sysMsg_t;

BOOL bPIVEnd = TRUE;

 /**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern BOOL doFirstTimer;
extern BOOL getTimer;
extern UINT32 timer;
#if GPS_OPEN
extern UINT8 LastFileNotRellyClose;
#endif
#if SP5K_ICAT_EIS
extern BOOL AdvEIS_ModeExt;
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if SP5K_ICAT_EIS
UINT32	pvRaw2YuvCb(frameBufInfo_t *praws, UINT32 rawNum, UINT32 rawbit, frameBufInfo_t *pyuvs, UINT32 yuvNum);
#endif

/*--------------------------------------------------------------------------*
* Function name    : App_videoGPSDataSave
* Function         : video GPS data save
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

#if GPSDEMO //for gps demo
void App_videoGPSDataSave(void)
{
	return ;

	UINT32 i ,len, writesize=1000;
	char fname[32], tmpBuff[32] ;
	UINT8 testchar=sp5kMalloc(1000);
	UINT32 totalFile;
	UINT32 currMode;

	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);
	sp5kModeGet(&currMode);

	memset(&testchar ,0x0, writesize);
	memset(&tmpBuff ,0x0, 32);
	for(i=0 ;i<=writesize ; i++)
	{
		 testchar[i]='T';
	}

	/* Get current file index  and set the TXT file */
	AppVideoSeamlessNextFileNameGet((UINT8*)fname);

	strtok(fname ,".");
	len = strlen(fname );
	strncpy(tmpBuff,fname,len);
	strcat(tmpBuff, ".TXT");
	WriteFile((UINT8*)tmpBuff , (UINT8*)testchar,writesize);
	sp5kFree(testchar);
}
#endif

/*--------------------------------------------------------------------------*
* Function name    : App_videoGPSDataSave
* Function         : picture in video task
* Return value	   : void
* Parameter1	   : entry_input (UINT32)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void
appPIVTask(
	UINT32 entry_input
)
{
	sysMsg_t msg;
	sp5kPrevYuvRoi_t roi;
	UINT32 w, h, fr;/*weikai modify 180504*/
	printf("%s, %d\n", __FUNCTION__, __LINE__);

	while(1) {
	    /*ret = sp5kOsQueueReceive(pPIVMsgQueue , &msg, TX_WAIT_FOREVER);*/
		sp5kOsQueueReceive(pPIVMsgQueue , &msg, TX_WAIT_FOREVER);/*weikai modify 180504*/

		printf("xxxxxxxxxxx %s xxxxxxxxxx\n",__FUNCTION__);
		/*Need check PIV function Isaac 21080316*/

		appVideoSizeGet(pViewParam->videoSize, &w, &h, &fr);

		memset(&roi, 0, sizeof(sp5kPrevYuvRoi_t));
		roi.roiw=w;
		roi.roih=h;
		roi.width=ulYUVw;
		roi.height=ulYUVh;

		sp5kPreviewJpegSave(pdstbuf, &roi, w, h);

		if(pdstbuf){
			sp5kYuvBufferFree(pdstbuf);
			pdstbuf = NULL;
		}

	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		CDFS_SET_JPG_FILENAME = FALSE;
	#endif
		bPIVEnd = TRUE;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appPIVTaskInit
* Function         : picture in video task init for create thread
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

UINT32
appPIVTaskInit(void)
{
	if(pPIVMsgQueue!=NULL || pPIVThread !=NULL)
		return FAIL;
 
	pPIVMsgQueue = sp5kMalloc(sizeof(SP5K_THREAD));
    if (pPIVMsgQueue) {
		UINT32 ret, queuesize=20;
        ret = sp5kOsQueueCreate(pPIVMsgQueue ,
        						"DiskMountQueue",
        						sizeof(sysMsg_t)/sizeof(UINT32),
        						NULL,
        						queuesize*sizeof(sysMsg_t));

        if ( ret != 0 /*ROS_SUCCESS*/) {
            printf("Line %d, ret=0x%u\n", __LINE__, ret);
            /*HOST_ASSERT(0);*/
        }
    }
	else
		return FAIL;

	pPIVThread = sp5kOsThreadCreateEx("PIVThread",
								  appPIVTask,
								  0   /*entry_input*/,
								  29  /*priority*/,              /*10,*/	/*16,*/
								  29  /*preempt_threshold*/,     /*10,*/	/*16,*/
								  0   /*time_slice_milli_sec*/,
								  8192/*stack_size*/,
								  1   /*auto_start*/);
	if ( pPIVThread != 0 /*ROS_SUCCESS*/) {
            printf("Line %d, ret=0x%u\n", __LINE__, (UINT32)pPIVThread);
            /*HOST_ASSERT(0);*/
    }
	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appSendMsg
* Function         : picture in video task send message
* Return value	   : result (UINT32 ret=0 is Send SUCCESS, other is FAIL)
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

UINT32 appSendMsg(void)
{
	sysMsg_t msg;
	ULONG wait_option = TX_NO_WAIT;
	UINT32 ret;

	/*send first msg*/
    msg.cmd = 0x13;
    msg.param= 0;
    ret = sp5kOsQueueSend(pPIVMsgQueue , &msg, wait_option);
	return ret;
}

/*--------------------------------------------------------------------------*
* Function name    : appPIVTrig
* Function         : picture in video task trig
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appPIVTrig(void)
{
	bYUVcap = TRUE;
}

/*--------------------------------------------------------------------------*
* Function name    : appPIVCb
* Function         : picture in video task callback function
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : nth stream, from 0 (UINT32)
* Parameter2	   : frame id (UINT32)
* Parameter3	   : buffer information (frameBufInfo_t)
* Parameter4	   : duplicated buffer address (UINT8*)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appPIVCb(
	UINT32 streamId,        /* nth stream, from 0 */
	UINT32 fid,             /* frame id */
	frameBufInfo_t *pframe, /* buffer information */
	UINT8 *pbufdup          /* duplicated buffer address */
)
{
	if(pframe->pbuf == NULL){
		printf("-- %s pframe->pbuf is NULL --\n",__FUNCTION__);
		return FAIL;
	}
		
	if(bYUVcap)
	{
		UINT32 videow,videoh,fr;
//		UINT32 framefmt = SP5K_GFX_FMT_YUV422;
		UINT32 framefmt;
		UINT32 rw, rh;

        //weikai modify for cppcheck 180607		
		#if SP5K_ICAT_EIS
		framefmt = (AdvEIS_ModeExt==TRUE)?SP5K_GFX_FMT_YUV420:SP5K_GFX_FMT_YUV422;
		#else 
		framefmt = SP5K_GFX_FMT_YUV422;
		#endif

		appVideoSizeGet(pViewParam->videoSize, &videow, &videoh, &fr);
		rw = ROUND_UP_TO_DIVISIBLE(videow,ALIGN_16); // width alignment 16
		rh = ROUND_UP_TO_DIVISIBLE(videoh,ALIGN_16); // height alignment 16

		if(!pdstbuf)
			pdstbuf=sp5kYuvBufferAlloc(rw,rh);
		
		sp5kGfxObj_t srcObj =
		{
			.pbuf = pframe->pbuf,
			.bufW = pframe->width,
			.bufH = pframe->height,
			.roiX = pframe->roiX,
			.roiY = pframe->roiY,
			.fmt  = framefmt,
			.roiW = pframe->roiW,
			.roiH = pframe->roiH
		};
			
		sp5kGfxObj_t dstObj =
		{
			.pbuf = pdstbuf,
			.bufW = rw,
			.bufH = rh,
			.roiX = 0,
			.roiY = 0,
			.fmt  = SP5K_GFX_FMT_YUV422,
			.roiW = videow,
			.roiH = videoh
		};

		sp5kGfxObjectBilinearScale(&srcObj, &dstObj);

		#if PIVDEBUG //DEBUG
		printf("--- %s BUF:%dx%d, ROI:(%d,%d, %dx%d) ---\n",__FUNCTION__,pframe->width,pframe->height,pframe->roiX,pframe->roiY,pframe->roiW,pframe->roiH);
		fsSimpleWrite( "D:\\PIVCB_Src.yuv", pframe->pbuf, srcObj.bufW*srcObj.bufH * 2); // yuv buffer size= w*h*2
		printf("--- rw:%d rh:%d, videow%d videoh:%d---\n",rw,rh,videow,videoh);
		fsSimpleWrite( "D:\\PIVCB_Scale.yuv", pdstbuf, dstObj.bufW*dstObj.bufH * 2);
		#endif

		ulYUVw = dstObj.bufW;
		ulYUVh = dstObj.bufH;	

		bYUVcap = FALSE;
		appSendMsg();
	}
	return SUCCESS;
}

#if SP5K_USB_UVC_SUPPORT

/*--------------------------------------------------------------------------*
* Function name    : appVideoSizeParamSet
* Function         : ui vide size parameter set
* Return value	   : void
* Parameter1	   : vidSizeParam (app_view_param_def.h)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoSizeParamSet(UINT32 vidSizeParam)
{
	pViewParam->videoSize = vidSizeParam;
}
#endif

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
UINT32 DualFile1, DualFile2, MtrackFile;
static UINT32 rec_cfg = DUALSEN_SINGLE_MAIN;

#if SERDES_4WIRE_UNIDIR_YUV_CASE
extern void sensorSingleWrite(UINT16 addr,UINT16 data);
#endif

/*--------------------------------------------------------------------------*
* Function name    : appVideoDualAttrSet
* Function         : dual sensor video attribute set
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoDualAttrSet(void)
{
    UINT8 size_idx = pViewParam->videoSize;
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "vset: dualsteeam size %d",pViewParam->videoSize);
#if SP5K_USB_UVC_SUPPORT
    #if SP5K_HW_DDR3_PARM_2G
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE, SP5K_MEDIA_H264_GOP_IBBP);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_GOP_STRUCTURE, SP5K_MEDIA_H264_GOP_IBBP);
    #else
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE, SP5K_MEDIA_H264_GOP_IPPP);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_GOP_STRUCTURE, SP5K_MEDIA_H264_GOP_IPPP);
    #endif
	UINT32 framerate=30;
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_FRAME_RATE, framerate);
    sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_INPUT_FRAME_RATE, framerate);
#else
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_FRAME_RATE, VideoSize[size_idx].frameRate);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_INPUT_FRAME_RATE, VideoSize[size_idx].frameRate);
#endif
#if SERDES_4WIRE_UNIDIR_YUV_CASE
    /* Allen Added for fixing frame rate */
	UINT16 addr=0x7124;
	sensorSingleWrite(addr, VideoSize[size_idx].frameRate); /* Fix to 30fps */
#endif
    /*Setup codec type for 2nd video stream */
    /*Setup width, height, type for 2nd video stream */
    if(size_idx == UI_VIDEO_DUAL_SIZE_FHD)
    {
        sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH,  DUAL_VIDEO_1080_WIDTH);
        sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, DUAL_VIDEO_1080_HEIGHT);
        sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);
    }
    else if(size_idx == UI_VIDEO_DUAL_SIZE_FHD_720P)
    {
        /*panel clk issue*/
        sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH,  DUAL_VIDEO_720_WIDTH);
        sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, DUAL_VIDEO_720_HEIGHT);
        sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);
    }
    else if(size_idx == UI_VIDEO_DUAL_SIZE_FHD_VGA)
    {
        #if SP5K_USB_UVC_SUPPORT	///for demo's request, scale to 1280x960
			UINT32 width_2nd=1280, height_2nd=960;
	        sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH,  width_2nd);
	        sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, height_2nd);
        #else
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH,  DUAL_VIDEO_VGA_WIDTH);
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, DUAL_VIDEO_VGA_HEIGHT);
		#endif
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoDualStreamCfgSet
* Function         : dual sensor video stream config set
* Return value	   : void
* Parameter1	   : sensor config (UINT8 0~3)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoDualStreamCfgSet(UINT8 cfg)
{
    UINT32 Stream_cfg = SP5K_MEDIA_REC_STREAM_1ST;
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "vset: dualsteeam cfg %d",cfg);
    rec_cfg = cfg;
    if (rec_cfg == DUALSEN_SINGLE_MAIN)
    {
    	#if SP5K_USB_UVC_SUPPORT
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, ENABLE);
		Stream_cfg = SP5K_MEDIA_REC_STREAM_1ST | SP5K_MEDIA_REC_STREAM_2ND;
		#else
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, DISABLE); /* disable the multi stream mode */
		Stream_cfg = SP5K_MEDIA_REC_STREAM_1ST;
		#endif
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAMS, Stream_cfg);
    }
    else if(rec_cfg == DUALSEN_SINGLE_SEC)
    {
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, ENABLE);
        if (appVideoMultiTrackGet() == APP_VIDEO_SINGLE_FILE)
        {
            Stream_cfg = SP5K_MEDIA_REC_STREAM_1ST | SP5K_MEDIA_REC_STREAM_2ND;
        }
        else
        {
        	#if SP5K_USB_UVC_SUPPORT
			Stream_cfg = SP5K_MEDIA_REC_STREAM_1ST | SP5K_MEDIA_REC_STREAM_2ND;
			#else
            Stream_cfg = SP5K_MEDIA_REC_STREAM_2ND;
			#endif
        }
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAMS, Stream_cfg);
        appVideoDualAttrSet();
    }
    else
    {
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAM_EN, ENABLE);
        Stream_cfg = SP5K_MEDIA_REC_STREAM_1ST|SP5K_MEDIA_REC_STREAM_2ND;
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MULTI_STREAMS, Stream_cfg);
        appVideoDualAttrSet();
    }
	#if SP5K_USB_UVC_SUPPORT
	appVideoDualAttrSet();
	appUvcModeSet();
	appViewOsd_DualModeDraw();
	#endif
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoDualStreamStart
* Function         : dual sensor video stream start
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoDualStreamStart(void)
{
    UINT8 sound_voicerec = pUiSetting->sound.VoiceRec;
    HOST_PROF_LOG_ADD(LEVEL_INFO, "vset: dualsteeam start");
	#if !SP5K_USB_UVC_SUPPORT
	if (rec_cfg == DUALSEN_SINGLE_MAIN)
    {
        return;
    }
	#endif
    if(!sound_voicerec)
    	VideoSize[ pViewParam->videoSize].audio = SP5K_MEDIA_AUDIO_UNKNOWN;	
    if (appVideoMultiTrackGet() == APP_VIDEO_SINGLE_FILE)
    {
        MtrackFile = sp5kMediaRecControl(SP5K_MEDIA_REC_SC_CREATE, MEDIA_REC_SC_TYPE_FILE,
            SP5K_MEDIA_REC_STREAM_1ST | SP5K_MEDIA_REC_STREAM_2ND, VideoSize[ pViewParam->videoSize].audio);
        sp5kMediaRecControl(SP5K_MEDIA_REC_SC_ATTACH, MtrackFile);
    }
    else
    {
        DualFile1 = sp5kMediaRecControl(SP5K_MEDIA_REC_SC_CREATE, SP5K_MEDIA_REC_SC_TYPE_FILE,
            SP5K_MEDIA_REC_STREAM_1ST, VideoSize[ pViewParam->videoSize].audio);
        sp5kMediaRecControl(SP5K_MEDIA_REC_SC_ATTACH, DualFile1);
        DualFile2 = sp5kMediaRecControl(SP5K_MEDIA_REC_SC_CREATE, SP5K_MEDIA_REC_SC_TYPE_FILE,
            SP5K_MEDIA_REC_STREAM_2ND, VideoSize[ pViewParam->videoSize].audio);
        sp5kMediaRecControl(SP5K_MEDIA_REC_SC_ATTACH, DualFile2);
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoDualStreamStop
* Function         : dual sensor video stream stop
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoDualStreamStop(void)
{
    HOST_PROF_LOG_ADD(LEVEL_INFO, "vset: dualsteeam stop");
	#if !SP5K_USB_UVC_SUPPORT
	if (rec_cfg == DUALSEN_SINGLE_MAIN)
	{
        return;
	}
	#endif
    if (appVideoMultiTrackGet() == APP_VIDEO_SINGLE_FILE)
    {
        sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DETACH, MtrackFile);
        sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DESTROY, MtrackFile);
    }
    else
    {
        sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DETACH, DualFile1);
        sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DETACH, DualFile2);
        sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DESTROY, DualFile1);
        sp5kMediaRecControl(SP5K_MEDIA_REC_SC_DESTROY, DualFile2);
    }
}
#endif

#if SP5K_DIQ_HDR
extern void * sp5kYuvBufferAlloc(UINT32 bufW,UINT32 bufH);
extern void sp5kYuvBufferFree(void * ptr);
#define ASSERT_MSG(expr, str, args...) \
	do { \
		if ( !(expr) ) { \
			dbgAssert(__FILE__, __LINE__, str, ##args); \
		} \
	} while (0)
#define IS_ROUND_TO_DIVISIBLE(num, div)    (((num) & ((div) - 1)) == 0)
#define LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR(addr)  ((void *)((UINT32)(addr) | 0x20000000))
#define HDR_W 1344
#define HDR_H 2160
static UINT8 *pHDRBuf=NULL;
static UINT8 *pHDRBuf_done=NULL;
static BOOL HDRBufFlag=FALSE;
static BOOL HDRBufFlag_done=FALSE;
static UINT8 bCBDone = FALSE;
UINT8 bSaveDone = TRUE;
UINT8 jpgSave = FALSE;

/*--------------------------------------------------------------------------*
* Function name    : appHDRYuvUrgentCallback
* Function         : get HDR yuv urgent frame Callback
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appHDRYuvUrgentCallback(
	UINT32 streamId,        /* nth stream, from 0 */
	UINT32 fid,             /* frame id */
	frameBufInfo_t *pframe, /* buffer information */
	UINT8 *pbufdup          /* duplicated buffer address */
)
{
	UINT8 *src1Addr, *src2Addr, *dstAddr;
	frameBufInfo_t src[ 2 ];
	frameBufInfo_t dst[ 1 ];
	UINT32 alignYuvXSize, alignYuvYSize;
	UINT32 roundDownYuvXSize, roundDownYuvYSize, done=0;

	if(!bSaveDone)
		return done;

	if(jpgSave)
	{
		sp5kGfxObj_t srcObj =
		{
			.pbuf = pframe->pbuf,
			.bufW = pframe->width,
			.bufH = pframe->height,
			.roiX = pframe->roiX,
			.roiY = pframe->roiY,
			.fmt = SP5K_GFX_FMT_YUV422,
			.roiW = pframe->roiW,
			.roiH = HDR_H
		};
		sp5kGfxObj_t dstObj =

		{
			.pbuf = pHDRBuf,
			.bufW = pframe->width,
			.bufH = pframe->height,
			.roiX = pframe->roiX,
			.roiY = pframe->roiY,
			.fmt = SP5K_GFX_FMT_YUV422,
			.roiW = pframe->roiW,
			.roiH = HDR_H
		};

		sp5kGfxObjectCopy(&srcObj, &dstObj);
	}

	alignYuvXSize = ROUND_UP_TO_DIVISIBLE( pframe->width, ALIGN_16 );
	alignYuvYSize = ROUND_UP_TO_DIVISIBLE( pframe->height / 2, ALIGN_16 );

	roundDownYuvXSize = ROUND_DOWN_TO_DIVISIBLE( pframe->width, ALIGN_16 );
	roundDownYuvYSize = ROUND_DOWN_TO_DIVISIBLE( pframe->height / 2, ALIGN_16 );

	src1Addr = (UINT8 *)LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR( pframe->pbuf );
	src2Addr = pframe->pbuf + roundDownYuvXSize * roundDownYuvYSize * 2;
	src2Addr = (UINT8 *)LOGI_CACH_BADDR_TO_LOGI_UNCACH_BADDR( src2Addr );
	dstAddr = src1Addr;

	src[ 0 ].pbuf = src1Addr;
	src[ 1 ].pbuf = src2Addr;
	dst[ 0 ].pbuf = dstAddr;

	src[ 0 ].width = src[ 1 ].width = dst[ 0 ].width = alignYuvXSize;
	src[ 0 ].height = src[ 1 ].height = dst[ 0 ].height = alignYuvYSize;

	src[ 0 ].roiW = dst[ 0 ].roiW = pframe->roiW;
	src[ 0 ].roiH = dst[ 0 ].roiH = pframe->roiH;
	src[ 0 ].roiX = dst[ 0 ].roiX = pframe->roiX;
	src[ 0 ].roiY = dst[ 0 ].roiY = pframe->roiY;

	src[ 1 ].roiW = pframe->roiW;
	src[ 1 ].roiH = pframe->roiH;
	src[ 1 ].roiX = pframe->roiX + ( pframe->width - roundDownYuvXSize );
	src[ 1 ].roiY = pframe->roiY + ( pframe->height / 2 - roundDownYuvYSize );

	sp5kHdrYuvDo( src, dst, 2 );

	if(jpgSave){
		sp5kGfxObj_t srcObj =
		{
			.pbuf = dstAddr,
			.bufW = pframe->width,
			.bufH = pframe->height,
			.roiX = pframe->roiX,
			.roiY = pframe->roiY,
			.fmt = SP5K_GFX_FMT_YUV422,
			.roiW = pframe->roiW,
			.roiH = pframe->roiH
		};
		sp5kGfxObj_t dstObj =

		{
			.pbuf = pHDRBuf_done,
			.bufW = pframe->width,
			.bufH = pframe->height,
			.roiX = pframe->roiX,
			.roiY = pframe->roiY,
			.fmt = SP5K_GFX_FMT_YUV422,
			.roiW = pframe->roiW,
			.roiH = pframe->roiH
		};

		sp5kGfxObjectCopy(&srcObj, &dstObj);
	}

	jpgSave = FALSE;
	bCBDone = TRUE;
	return done;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoHDRModeSetting
* Function         : video HDR mode setting
* Return value	   : void
* Parameter1	   : video HDR enable (UINT8 ENABLE/DISABLE)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoHDRModeSetting(
	UINT8 videoHDRenable
)
{
extern UINT32 sensorModeGet(void);
	if(videoHDRenable){
    	if(jpgSave)/*For debug*/
        {
            if(pHDRBuf == NULL){
    			pHDRBuf = sp5kYuvBufferAlloc(ROUND_UP_TO_DIVISIBLE(HDR_W,ALIGN_16), ROUND_UP_TO_DIVISIBLE(HDR_H,ALIGN_16));
    			HDRBufFlag = TRUE;
    		}
    		if(pHDRBuf_done == NULL){
    			pHDRBuf_done= sp5kYuvBufferAlloc(ROUND_UP_TO_DIVISIBLE(HDR_W,ALIGN_16), ROUND_UP_TO_DIVISIBLE(HDR_H/2,ALIGN_16));
    			HDRBufFlag_done=TRUE;
    		}
        }

		sp5kModeCfgSet(SP5K_MODE_CFG_PV_SENSOR_OPT, SP5K_MODE_PV_SENSOR_HDR_SEQ_MODE);
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, SENSOR_MODE_VIDEO_HDR);
        appAeFrameRateSet(60);/*For HDR is 60fps input, AE should be less than 1/60*/
		//weikai modify for cppcheck 180607 
		UINT32/* pvw=1920, pvh=1080, */framenumber=8; /*frame number. Can be 4, 8, 12, 16. Only 8 is valid before SPCA6350.*/
        sp5kModeCfgSet(SP5K_MODE_CFG_VIDEO_PV_SIZE, 1920, 1080, framenumber);/*For memory reduce, need supress to 8x PV buffer*/
	}
    else
    {
		if(HDRBufFlag){
			sp5kYuvBufferFree(pHDRBuf);
			HDRBufFlag = FALSE;
		}
		if(HDRBufFlag_done){
			sp5kYuvBufferFree(pHDRBuf_done);
			HDRBufFlag_done = FALSE;
		}
		sp5kModeCfgSet(SP5K_MODE_CFG_PV_SENSOR_OPT, SP5K_MODE_PV_SENSOR_DEFAULT);
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, VideoSize[pViewParam->videoSize].sensormode);
        appAeFrameRateSet(VideoSize[pViewParam->videoSize].frameRate);
        sp5kModeCfgSet(SP5K_MODE_CFG_VIDEO_PV_SIZE, VideoSize[pViewParam->videoSize].w, VideoSize[pViewParam->videoSize].h, 12);
	}

	appUrgentSet( URGENT_HDR, videoHDRenable);		/* for video  preview and record  */
    appAutoHDRLibEnable(videoHDRenable);
	#if STILLPREVIEW
    	sp5kPrevUrgentCallback_t cb[2];
    	cb[0].fp = appHDRYuvUrgentCallback;
    	cb[1].fp = NULL;
    	sp5kPreviewUrgentCallbackSet( INTERVAL_FRAMES_COMES, videoHDRenable, cb, NUMBER_ONE_STREAM );	/* for still preview  */
	#endif

	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, VideoSize[pViewParam->videoSize].w);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, VideoSize[pViewParam->videoSize].h);
	sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_AUDIO_CODEC, VideoSize[pViewParam->videoSize].audio);

    appAeFrameRateSet(VideoSize[pViewParam->videoSize].frameRate);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, VideoSize[pViewParam->videoSize].frameRate);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_INPUT_FRAME_RATE, VideoSize[pViewParam->videoSize].frameRate);

	appModeSet(SP5K_MODE_VIDEO_PREVIEW);
    appAeHDRALGEnable(videoHDRenable);
    printf("%s(%d)_HDR:%d cur senMode=0x%.2X\n",__FUNCTION__, __LINE__, videoHDRenable, sensorModeGet());
}

/*--------------------------------------------------------------------------*
* Function name    : appSaveHDRCBTOJPEG
* Function         : save HDR to JPEG
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appSaveHDRCBTOJPEG(void)
{
    sp5kPrevYuvRoi_t roi_hdr;

    jpgSave = TRUE;
    bCBDone = FALSE;

	while (!bCBDone)
	{
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 3/*ms*/);
	}
    bSaveDone = FALSE;

    memset(&roi_hdr, 0, sizeof(sp5kPrevYuvRoi_t));
    roi_hdr.width = HDR_W;
    roi_hdr.height= HDR_H;
    roi_hdr.roiw = HDR_W;
    roi_hdr.roih = HDR_H;

    sp5kPreviewJpegSave(pHDRBuf, &roi_hdr, ROUND_UP_TO_DIVISIBLE(HDR_W,ALIGN_16), ROUND_UP_TO_DIVISIBLE(HDR_H,ALIGN_16));


    memset(&roi_hdr, 0, sizeof(sp5kPrevYuvRoi_t));
    roi_hdr.width = HDR_W;
    roi_hdr.height= ROUND_UP_TO_DIVISIBLE(HDR_H/2,ALIGN_16);
    roi_hdr.roiw = HDR_W;
    roi_hdr.roih = ROUND_UP_TO_DIVISIBLE(HDR_H/2,ALIGN_16);

    sp5kPreviewJpegSave(pHDRBuf_done, &roi_hdr, ROUND_UP_TO_DIVISIBLE(HDR_W,ALIGN_16), ROUND_UP_TO_DIVISIBLE(HDR_H/2,ALIGN_16));

    bSaveDone = TRUE;
}
#endif

/*--------------------------------------------------------------------------*
* Function name    : appVideoFormatSet
* Function         : video format set
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoFormatSet(void)
{
  	UINT8 vidformat = pUiSetting->videoFormat;
	UINT8 medformat = pUiSetting->mediaFormat;
	UINT8 size_idx = pViewParam->videoSize;
	UINT8 sound_voicerec = pUiSetting->sound.VoiceRec;

    if(pViewParam->videoSize == UI_VIDEO_SIZE_4K2K||pViewParam->videoSize == UI_VIDEO_SIZE_27K)
    {
        vidformat = pUiSetting->videoFormat = VIDEO_FORMAT_MJPG;
    }
    else
        vidformat = pUiSetting->videoFormat = VIDEO_FORMAT_H264;


	if(vidformat == VIDEO_FORMAT_H264){
	    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_H264);
	}
	else if(vidformat == VIDEO_FORMAT_MJPG){
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_CODEC, SP5K_MEDIA_VIDEO_MJPG);
	}
    else
    {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vset: vidformat fail =%d",vidformat);
        DBG_ASSERT(0);
    }
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE, SP5K_MEDIA_H264_GOP_IPPP);
	if(VideoSize[size_idx].frameRate == VIDEO_14_FRAME_RATE){
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_NO,7);
	}else if(VideoSize[size_idx].frameRate == VIDEO_28_FRAME_RATE){
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_NO,14);
	}
#endif
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_BRC_TYPE, SP5K_MEDIA_CBR);
    #if SET_FRAME_RATE   /*redundant setting og INPUT_FRAME_RATE*/
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_INPUT_FRAME_RATE, VideoSize[size_idx].frameRate);
    #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_INPUT_FRAME_RATE, VideoSize[size_idx].frameRate);
	#endif
    #endif
	
    if(vidformat == VIDEO_FORMAT_H264)
    {

        /* Allen added, for different H264 maximum QP and Initial QP setting */

        /*
            b[7:0] - Initial qp value, maximum 51.
            b[8:15] - Maximum qp value, maximum 51. (the worst qp value)
            b[16:23] - Minimum qp value, maximum 51. (the best qp value)
            b[31] - 0: Rate control mode. 1: Fixed QP mode.
            */

        /* for primary sensor */
        {
            UINT32 h264Qp;
            /* ini:36 min:default max:45 */
            sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_H264_QP, &h264Qp);
            h264Qp = h264Qp & 0xFF000000;
            h264Qp = h264Qp | (10 << 16) | (45 << 8)| (36 << 0);
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_QP, h264Qp);
        }

        /* for secondary sensor */
        {
            UINT32 h264Qp;

            sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_H264_QP, &h264Qp);
            h264Qp = h264Qp & 0xFF000000;
            h264Qp = h264Qp | (20 << 16) | (45 << 8) | (36 << 0);
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_H264_QP, h264Qp);
        }
    }
    else
    {
        /**2016/11/11 baseFW default is 1/90/70  (min,Max,Init) **/
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MAX_QVALUE,    (85*1024)/100);	/* 85%*/
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_MIN_QVALUE,    ( 1*1024)/100);
        sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_INIT_QVALUE, (30*1024)/100);	/* for 4k */
    }


	/* Allen added, for enable/disable VUI tag, disable is limited range : 16~235*/
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_VUI_TAG, 0);

	if(medformat == MEDIA_FORMAT_AVI){
		/*sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_AVI);*/
		SET_VIDEO_TYPE(SP5K_MEDIA_AVI);
	}
	else if(medformat == MEDIA_FORMAT_MOV)
	{
		/*sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_MOV);*/
		SET_VIDEO_TYPE(SP5K_MEDIA_MOV);

	}
	else if(medformat == MEDIA_FORMAT_MP4)
	{
		/*sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_MOV);*/
		SET_VIDEO_TYPE(SP5K_MEDIA_MP4);

	}
    else
    {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vset: medformat fail =%d",vidformat);
        DBG_ASSERT(0);
    }

	if(sound_voicerec)
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, VideoSize[size_idx].audio);
	else
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_UNKNOWN);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_AVG_BITRATE, VIDEO_AUDIO_BIT_RATE);
    sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_AVG_BITRATE, VIDEO_AUDIO_BIT_RATE);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_RATE, VIDEO_AUDIO_SAMPLE_RATE);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_SAMPLE_BITS, VIDEO_AUDIO_SAMPLE_BIT);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CHANNELS, VIDEO_AUDIO_CHANNEL);
    /*duplicate L->R*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(VIDEO_AUDIO_CHANNEL==1)/*MONO*/
	    sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DUP_L_TO_R_ENABLE, DISABLE);
	else/*Stereo*/
#endif
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DUP_L_TO_R_ENABLE, ENABLE);

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, 16);
#else
	sp5kAudDevVolumeSet(SP5K_AUD_DEV_REC, 31);
#endif
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoSizeSet
* Function         : video size set
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoSizeSet(
	void
)
{
	UINT32 mode;
    UINT8 size_idx = pViewParam->videoSize;

	sp5kModeGet(&mode);
	if (mode != SP5K_MODE_STANDBY) {
		/* return or go standby*/
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
	}

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
   if(size_idx == UI_VIDEO_SIZE_FHD_30FPS || size_idx == UI_VIDEO_SIZE_FHD_PARK ){
   	if(appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP)
   		 size_idx = UI_VIDEO_SIZE_FHD_PARK;
	else
		 size_idx = UI_VIDEO_SIZE_FHD_30FPS;
	    gVideoSizeTag = size_idx;
	    pViewParam->videoSize = size_idx;
   }
#endif
    appAeHighPriority(TRUE);/*Set AA task priority high to avoid AE/AWB CB delay & preview begin with green frame/late AE.*/

    sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, 1);
	if (size_idx >= UI_VIDEO_SIZE_MAX) {
        printf("\n\n%s(#%d)THAT IS ERROR INDEX, idx:%d over max_idx:%d\n", __FUNCTION__, __LINE__, size_idx, UI_VIDEO_SIZE_MAX);
		size_idx = 0;
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vset: size idx fail=%d",size_idx);
	}

	#if (SP5K_USB_UVC_SUPPORT == 0 || SP5K_SENSOR_DUAL_MODE ==0)
	if (IS_HDMI_IN && (pViewParam->videoSize==UI_VIDEO_SIZE_FHD_60FPS || pViewParam->videoSize==UI_VIDEO_SIZE_4K2K || pViewParam->videoSize==UI_VIDEO_SIZE_27K)) {
        size_idx = UI_VIDEO_SIZE_FHD_30FPS;   /*disable 1080p 60fps*/
	}
    #endif

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "vset: video size: %d %dx%d %x"\
                        ,size_idx, VideoSize[size_idx].w, VideoSize[size_idx].h, VideoSize[size_idx].sensormode);

	//make sure flow sync new video_size, so assign to global variable here, DO NOT change video size after here,
    gVideoSizeTag = size_idx;
    pViewParam->videoSize = size_idx;

#if SP5K_ICAT_EIS
    isLdcIsDisable();
#endif

#if !SP5K_SENSOR_DUAL_MODE
	UINT32 sizefactorcfg;
    if(pViewParam->videoSize == UI_VIDEO_SIZE_4K2K||pViewParam->videoSize == UI_VIDEO_SIZE_27K) {   /* 4k video test */
        appModeCfgPvRawSet( SP5K_MODE_PV_RAW_DO_UHD_MODE );
		sizefactorcfg=((2<<4) | 2) /*0x22*/;
        sp5kSystemCfgSet(SP5K_CDSP_2NDPATH_FACTOR_CFG, sizefactorcfg);
        sp5kModeCfgSet(SP5K_MODE_CFG_PV_RAW_DO_PARAM, SP5K_MODE_PV_RAW_DO_UHD_MODE_PARAM_RAW_NUM, 3);
    }
    else if (appModeCfgPvRawGet() == SP5K_MODE_PV_RAW_DO_UHD_MODE) {
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
       sp5kModeSet(SP5K_MODE_STANDBY);
        sp5kModeWait(SP5K_MODE_STANDBY);
#endif
        appModeCfgPvRawSet(SP5K_MODE_PV_RAW_DO_DISABLE);
		sizefactorcfg=((1<<4) | 1) /*0x11*/;
        sp5kSystemCfgSet(SP5K_CDSP_2NDPATH_FACTOR_CFG, sizefactorcfg);   /* default */
        sp5kModeCfgSet(SP5K_MODE_CFG_PV_RAW_DO_PARAM, SP5K_MODE_PV_RAW_DO_UHD_MODE_PARAM_RAW_NUM, 2);
    }
#endif

    /*set sensor mode*/

#if UI_HDR_OFF/* defined(PRJ_PDVR) */
	uiPara_t* puiPara = appUiParaGet();
	if(puiPara->HDR == HDR_OFF)
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, 0x32);
	else
		sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, VideoSize[size_idx].sensormode);
#else
	sp5kSensorModeCfgSet(SP5K_MODE_VIDEO_PREVIEW, VideoSize[size_idx].sensormode);

    /*Setup width, height, type for 1nd video stream */
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_WIDTH, VideoSize[size_idx].w);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_HEIGHT, VideoSize[size_idx].h);
#endif
#if defined(PRJ_PDVR) /* 27.5fps */
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, NON_INTEGER_FRAME_RATE); /* 27.5fps */
 	sp5kSensorCfgSet(SP5K_SENSOR_CFG_FRAMERATE_MAX,VideoSize[size_idx].sensormode,27500/*kfps*/); /* 27.5fps*1000 */
#else

	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, VideoSize[size_idx].frameRate);
	sp5kSensorCfgSet(SP5K_SENSOR_CFG_FRAMERATE_MAX,VideoSize[size_idx].sensormode,VideoSize[size_idx].frameRate*1000/*kfps*/);
#endif

	#if SP5K_SENSOR_DUAL_MODE && SINGLE_DUAL_SWITCH_SAMPLE
		if (appSingleModeCheck()==APP_VIDEO_MODE_SINGLE)
		{
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_FRAME_RATE, 0);
			sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_INPUT_FRAME_RATE, 0);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_WIDTH,	320);
			sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_HEIGHT, 240);
		}
	#endif
    #if SP5K_USB_UVC_SUPPORT
    appVideoDualStreamCfgSet(appDualPipModeGet());
    #elif SP5K_SENSOR_DUAL_MODE
	    sp5kSensorCfgSet(SP5K_SENSOR_CFG_FRAMERATE_MAX,SENSOR_MODE_1080P_30FPS, VideoSize[size_idx].frameRate*1000/*kfps*/);
	    sp5kSensorCfgSet(SP5K_SENSOR_CFG_FRAMERATE_MIN,SENSOR_MODE_1080P_30FPS, VideoSize[size_idx].frameRate*1000/*kfps*/);
	    #if SERDES_4WIRE_UNIDIR_YUV_CASE
			if (size_idx == UI_VIDEO_DUAL_SIZE_FHD) {
				sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_PREVIEW, 0x30);
				sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_RECORD, 0x30);
			}
			else if (size_idx == UI_VIDEO_DUAL_SIZE_FHD_720P) {
				sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_PREVIEW, 0x30);
				sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_RECORD, 0x30);
			}
			else if (size_idx == UI_VIDEO_DUAL_SIZE_FHD_VGA) {
				sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_PREVIEW, 0x30);
				sp5kSensorModeCfgSet(SP5K_MODE_SEC_SENSOR_SEL|SP5K_MODE_VIDEO_RECORD, 0x30);
			}
	    #endif
	    appVideoDualStreamCfgSet(appDualPipModeGet());
    #endif

    /* frame rate */
    appAeFrameRateSet(VideoSize[size_idx].frameRate);
	#if defined(PRJ_PDVR)
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_INPUT_FRAME_RATE, NON_INTEGER_FRAME_RATE/*27.5 fps*/);
	#else
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_INPUT_FRAME_RATE, VideoSize[size_idx].frameRate);
	#endif
    /*1080p 60fps need use 12 yuv buffer for no drop frame in cdsp*/
    /*MJPG maybe can use 4, but real test is drop frame, so default as 8*/
    if (size_idx == UI_VIDEO_SIZE_FHD_60FPS) {
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE, SP5K_MEDIA_H264_GOP_IBBP);
	    sp5kModeCfgSet(SP5K_MODE_CFG_VIDEO_PV_SIZE, AUTO_WIDTH, AUTO_HEIGHT, 12/*yuv buffer*/);
	}
    else {
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE, SP5K_MEDIA_H264_GOP_IPPP);
        sp5kModeCfgSet(SP5K_MODE_CFG_VIDEO_PV_SIZE, AUTO_WIDTH, AUTO_HEIGHT, 8/*yuv buffer*/);
    }
    #if SP5K_ICAT_EIS    //////SHEN: Implment from ISAW S2
        sp5kStillCapCfgSet(SP5K_CAPTURE_RAW_DATA_TYPE, SP5K_CAPTURE_RAW_FMT_10_BIT);
    #endif
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoH264VlcSizeCalculate
* Function         : Video H264 vlc size calculate
* Return value	   : vlcSize (UINT32)
* Parameter1	   : stream index (UINT32 SUCCESS/FAIL)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoH264VlcSizeCalculate(
	UINT32 streamId
)
{
	UINT32 h264Qp, mjpgMaxQ;
	UINT32 w, h;
	UINT32 vlcSize;
	UINT32 codec;

	if (streamId == 0) {
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_VIDEO_CODEC, &codec);
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_H264_QP, &h264Qp);
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_WIDTH, &w);
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &h);
	}
	else {
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_VIDEO_CODEC, &codec);
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_H264_QP, &h264Qp);
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_WIDTH, &w);
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_HEIGHT, &h);
	}

	if (codec == SP5K_MEDIA_VIDEO_MJPG) {
		sp5kMediaRecCfgGet(SP5K_MEDIA_REC_MAX_QVALUE, &mjpgMaxQ);	/* no cfg for 2nd stream */
		vlcSize = w * h * 2 * 15 / 100;
		vlcSize = vlcSize * (100 * mjpgMaxQ / 1024) / 100;
		profLogPrintf(0, "MJPG,%dx%d,Q=%d", w, h, mjpgMaxQ);
	}
	else {
		h264Qp = (h264Qp >> 16) & 0xff;	/* use minimal QP to guess VLC size. Assume QP 10 is for 50% compression. */

		//From pure environment to complex environment, would meet small QP and complex environment.
		//Thus minQP is more important for VLC output size instead of frame rate or bit rate.
		vlcSize = w * h * 2 * 40 / 100;
		vlcSize = vlcSize * (100 - h264Qp * 2 + 10) / 100;	/* this formula might need to fine tune. */
		profLogPrintf(0, "H264,%dx%d,QPmin=%d", w, h, h264Qp);
	}
	profLogPrintf(0, "Video vlc size=%d", vlcSize);

	return vlcSize;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoFifoNumCalculate
* Function         : calculate video fifo number before record
* Return value	   : return time is time of bitrate in 100 mini-second (frametate * retTime/10 = fifonum)
* Parameter1	   : initial value in 100 mini-second (UINT32 expectTime>2)
* Parameter2	   : stream number (UINT32 1~2)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

/* Use expectTime to check memory usage and get exactly available time. */
UINT32
appVideoFifoNumCalculate(
	UINT32 expectTime,		/* in 100 mini-second */
	UINT32 streamNum	/* 1 for single file stream, 2 for dual file stream*/
)
{
	/***********************************************************/
	/* FIFO number determination                               */
	/* 1. memory estimate:                                     */
	/*     it will come out with max available fsFifoDepth.    */
	UINT32 memUsage = 0, vlcUsage;
	UINT32 retTime = expectTime;
	void *ptr = NULL;
	UINT32 vlcSize = 0, byteRate = 0;
	UINT32 temp;

	sp5kMediaRecCfgGet(SP5K_MEDIA_REC_VLC_BUF_CNT, &temp);
	sp5kMediaRecCfgGet(SP5K_MEDIA_REC_VLC_BUF_SIZE, &vlcSize);
	vlcUsage = vlcSize * temp;
	/* ref buffer */
	sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_VIDEO_CODEC, &temp);
	if (temp == SP5K_MEDIA_VIDEO_H264) {
		UINT32 w, h;
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_WIDTH, &w);
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_HEIGHT, &h);
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_H264_GOP_STRUCTURE, &temp);
		vlcUsage += w*h*2<<(temp==SP5K_MEDIA_H264_GOP_IBBP);
	}
	sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, &temp);
	byteRate = temp >> 3;
	vlcUsage += 1000000; /* mux buffer */
	sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_FILE_TYPE, &temp);
	if (temp == SP5K_MEDIA_MOV) {
		sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_CLUSTER_SIZE_BY_BYTES, &temp);
		vlcUsage += (temp << 3) + 512*512*5; /* mux buffer */
	}
	else {
		/* what if using other muxer? */
		vlcUsage += 3000000;	/* just guess a number */
	}
	if (streamNum == 2) {
		sp5kMediaRecCfgGet(SP5K_MEDIA_REC_2ND_VLC_BUF_CNT, &temp);
		sp5kMediaRecCfgGet(SP5K_MEDIA_REC_2ND_VLC_BUF_SIZE, &vlcSize);
		vlcUsage += vlcSize * temp;
		/* ref buffer */
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_VIDEO_CODEC, &temp);
		if (temp == SP5K_MEDIA_VIDEO_H264) {
			UINT32 w, h;
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_WIDTH, &w);
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_HEIGHT, &h);
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_H264_GOP_STRUCTURE, &temp);
			vlcUsage += w*h*2<<(temp==SP5K_MEDIA_H264_GOP_IBBP);
		}
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_VIDEO_AVG_BITRATE, &temp);
		byteRate += temp >> 3;
		vlcUsage += 1000000; /* mux buffer */
		sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_FILE_TYPE, &temp);
		if (temp == SP5K_MEDIA_MOV) {
			sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_CLUSTER_SIZE_BY_BYTES, &temp);
			vlcUsage += (temp << 3) + 512*512*5; /* mux buffer */
		}
		else {
			/* what if using other muxer? */
			vlcUsage += 3000000;	/* just guess a number */
		}
	}
	/* for safety, some more bytes for over-average vlc */
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, &temp);
	vlcUsage += byteRate / temp * 3;
	for ( ; retTime > 1 ;) {
		memUsage = (byteRate * retTime)/10 + vlcUsage;
		memUsage = (memUsage / 1000000 + 1) * 1000000;	/* round up */
		ptr = sp5kMalloc(memUsage);
		printf("%u.%u sec needs %uMB, %s\n", retTime/10, retTime%10, memUsage/1000000, ptr?"OK":"NG");
		if (ptr)
			break;
		if (retTime > 10)	/* > 1sec */
			retTime -= 10;
		else
			retTime --;
	}
	if (ptr)
		sp5kFree(ptr);
	else
		HOST_ASSERT(0);

	profLogPrintf(0, "VideoFifo=%d.%dsec(%d)", retTime/10, retTime%10, memUsage);

	return retTime;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoFifoSet
* Function         : set video fifo number
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoFifoSet(
	void
)
{
	UINT32 temp;
	UINT32 fsFifoDepth;
	UINT32 framerate;

	framerate = VideoSize[pViewParam->videoSize].frameRate;
	if(framerate < 15)
		framerate = 15;

	#if SP5K_SENSOR_DUAL_MODE
	if (appSingleModeCheck() == APP_VIDEO_MODE_DUAL) {
		fsFifoDepth = appVideoFifoNumCalculate(60, 2);
	}
	else {
	fsFifoDepth = appVideoFifoNumCalculate(60, 1);
	}
	#elif SP5K_USB_UVC_SUPPORT
	#if SINGLE_DUAL_SWITCH_SAMPLE
	#error todo
	#endif
	fsFifoDepth = appVideoFifoNumCalculate(60, 2);
	#else
	fsFifoDepth = appVideoFifoNumCalculate(60, 1);
	#endif
	HOST_ASSERT(fsFifoDepth);
	/* 1. video fifo number determination.                     */
	/*     time * frame rate                                   */
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VIDEO_FIFO_BUF_NO, fsFifoDepth * framerate / 10 - 3);
	/* 2. audio fifo number determination.                     */
	/*     time * (1000/duration)                              */
	sp5kMediaRecCfgGet(SP5K_MEDIA_REC_AUD_FRAME_DURATION, &temp);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_AUDIO_FIFO_BUF_NO, fsFifoDepth * 100 / temp);
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoSetQuality
* Function         : set video quality
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoSetQuality(
	void
)
{
    UINT32 size_idx, quality_idx, VLC_size, framerate;
    UINT32 ae_period, awb_period, awb_interval;


    size_idx    = pViewParam->videoSize;
	quality_idx = pViewParam->videoQuality;
    ae_period   = ae_awb_config[size_idx][0];
    awb_period  = ae_awb_config[size_idx][1];
    awb_interval= ae_awb_config[size_idx][2];

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "vset: size idx=%d,quality idx=%d,card =%s"\
                        ,size_idx, quality_idx, (IS_CARD_EXIST)? "SD" : "NAND");

	if(size_idx >= UI_VIDEO_SIZE_MAX) {
        printf("\n\n%s(#%d)THAT IS ERROR INDEX, idx:%u over max_idx:%d\n", __FUNCTION__, __LINE__, size_idx, UI_VIDEO_SIZE_MAX);
		size_idx = pViewParam->videoSize = 0;
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vset: size idx fail=%d", size_idx);
	}
	else if(!ae_period || !awb_period || !awb_interval) {
		/*for ae/awb error handle*/
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vset: ae/awb setting error %d %d %d", ae_period, awb_period, awb_interval);
	}

	framerate = VideoSize[size_idx].frameRate;
	if(framerate < 15)
		framerate = 15;

	/* FIXME!!!   4K2K/2.7K MJPG setting already set in appVideoFormatSet() **/
    if(pViewParam->videoSize == UI_VIDEO_SIZE_4K2K||pViewParam->videoSize == UI_VIDEO_SIZE_27K)
        pUiSetting->videoFormat = VIDEO_FORMAT_MJPG;
    else
         pUiSetting->videoFormat = VIDEO_FORMAT_H264;

	/***********************************************************/
	/* VLC buffer usage is an issue of CPU loading.
	    If CPU is too busy to copy VLC to FIFO, VLC will be run out.
	    Of course, higher frame rate will have higher probability of running out of VLC. */
	if (framerate > 30)
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_CNT, 6);
	else
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_CNT, 4);
	/***********************************************************/

	appAEALG_ParamSet(AE_PARA_PERIOD,ae_period);
	sp5kAwbCfgSet( SP5K_AWB_ACCUM_PERIOD, awb_period);
    appAWBALG_WBParamSet(AWB_PARA_INTERVAL_FRAME, awb_interval);

    #if SP5K_DIQ_FRMWRK
		sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_INTPL_INTVL_SET, awb_interval);
		#if 0	/* Open it if auto gamma is ready */
			sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_AUTO_GMA_INTVL_SET, awb_period);
		#endif
	#endif
	if(pUiSetting->videoFormat == VIDEO_FORMAT_H264) {
		gVideoCB.streamBytesPerSec = qualityRatio_h264[size_idx][quality_idx]*1000000>>3;
		#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
		UINT32 BitRate_2nd;
		BitRate_2nd = qualityRatio_h264_2nd[size_idx][quality_idx]*1000000>>3;
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_VLC_BUF_CNT, 4);
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_VLC_BUF_SIZE, appVideoH264VlcSizeCalculate(1));
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_2ND_VIDEO_AVG_BITRATE, BitRate_2nd<<3);
		#endif
	}
    else {
		gVideoCB.streamBytesPerSec = qualityRatio_mjp[size_idx][quality_idx]*1000000>>3;
	}

	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_BRC_TYPE, SP5K_MEDIA_CBR);
	VLC_size = appVideoH264VlcSizeCalculate(0);
	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_VLC_BUF_SIZE, VLC_size);
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, gVideoCB.streamBytesPerSec<<3);
    printf("%s(#%d) idx=%u,VLC=%u,BR=%d\n",__FUNCTION__, __LINE__,size_idx,
                    VLC_size, gVideoCB.streamBytesPerSec<<3);
}

/*--------------------------------------------------------------------------*
* Function name    : appSendMsg
* Function         : get video size and frame rate
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : size index (UINT8*)
* Parameter2	   : width (UINT32*)
* Parameter3	   : hight (UINT32*)
* Parameter4	   : frame rate (UINT32*)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoSizeGet(
	UINT8 size_idx,
	UINT32 *pW,
	UINT32 *pH,
	UINT32 *pfrmrate
)
{
	#if SUPPORT_RUNTIME_SWITCH_DRAM_CLK
	if (size_idx < UI_VIDEO_SIZE_MAX) {
		/*HOST_ASSERT(size_idx<UI_VID_SIZE_MAX);*/
		if (pW)
			*pW = VideoSize[size_idx].w;
		else
			return FAIL;
		if (pH)
			*pH = VideoSize[size_idx].h;
		else
			return FAIL;
		if (pfrmrate)
			*pfrmrate = VideoSize[size_idx].frameRate;
		else
			return FAIL;
	}
	else {
		/* size_idx >= UI_VID_SIZE_MAX */
		/* Special design for getting current VID_SIZE */
		if (pW)
			*pW = VideoSize[gVideoSizeTag].w;
		else
			return FAIL;
		if (pH)
			*pH = VideoSize[gVideoSizeTag].h;
		else
			return FAIL;
		if (pfrmrate)
			*pfrmrate = VideoSize[gVideoSizeTag].frameRate;
		else
			return FAIL;
	}

	#else /* !SUPPORT_RUNTIME_SWITCH_DRAM_CLK */
	/*HOST_ASSERT(size_idx<UI_VID_SIZE_MAX);*/
	if (pW)
		*pW = VideoSize[size_idx].w;
	else
			return FAIL;
	if (pH)
		*pH = VideoSize[size_idx].h;
	else
			return FAIL;
	if (pfrmrate)
		*pfrmrate = VideoSize[size_idx].frameRate;
	else
			return FAIL;
	#endif
	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRemainSecGet
* Function         : get video remain time in secord
* Return value	   : remain time (UINT32 card full is remaintime=0)
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoRemainSecGet(
	void
)
{
	appDiskInfo_t* pDisk;
	UINT32 remaintime=0, space=0, uV_Format, uV_BitRate, uV_FrameRate, uA_BitRate, uA_Channel, vid_unit;
	UINT32 reserveSpace = APP_DISK_DPOF_RSVD_SIZE_KB + AVI_HDR_KB + APP_CAPTURE_IN_MOVIE + APP_THRESHOLD_SPACE;

    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_VIDEO_CODEC, &uV_Format);
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_VIDEO_AVG_BITRATE, &uV_BitRate);
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_VIDEO_FRAME_RATE, &uV_FrameRate);
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_AUDIO_AVG_BITRATE, &uA_BitRate);
    sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_AUDIO_CHANNELS, &uA_Channel);

	if(uV_Format == SP5K_MEDIA_VIDEO_H264)
        vid_unit = 24;
    else
        vid_unit = 616;
	gVideoCB.aviKbPerSec = (uV_BitRate/8  +uV_FrameRate*vid_unit+128  +(uA_BitRate/8)*uA_Channel+24  +8096)/1024;                        /* other overhead (audio hdr, index, fat... , 8K*/

	pDisk = appCurDiskInfoGet();
	if(pDisk->freeSz > reserveSpace)
		space = pDisk->freeSz - reserveSpace;
	else
		space = 0;

	if(space>0 && gVideoCB.aviKbPerSec!=0)
	{
		remaintime = space / gVideoCB.aviKbPerSec;
		remaintime = remaintime>1? (remaintime-1) : 0;

	}
	if(remaintime == 0)
	{
		gVideoCB.err = VIEW_ERR_MEMORY_FULL;
	}
    DBG_PRINT("%s(#%d) (0x%X, %d, %d, %d, %d) aviKbPerSec=%d\ndskfree=%d KB, remaintime=%d sec\n", __FUNCTION__, __LINE__, uV_Format, uV_BitRate, uV_FrameRate, uA_BitRate, uA_Channel, gVideoCB.aviKbPerSec, pDisk->freeSz, remaintime);
	return remaintime;
}

#if SP5K_CDFS_OPT

/*--------------------------------------------------------------------------*
* Function name    : appSeamlessDelFileThread
* Function         : when you do video seamless, if card is full, 
                     this function will delete the oldest video file
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appSeamlessDelFileThread()
{
	ULONG ulActualEvents;
	UINT32 err;
/*
#if defined(PRJ_17DVR)
	UINT32 elsTime;    
	UINT32 reserveTime;
#endif
*/
	err = sp5kOsQueueCreate(&seamlessFileAddQue, "sigDelFile", 1, NULL, 16*4);
	HOST_ASSERT(err == SP5K_SUCCESS);
    sp5kDiskCfgSet(SP5K_DISK_BUSY_MEAS_CFG, SP5K_DRIVE_SD, measDiskTime);

	while(1){
		err = sp5kOsQueueReceive(&seamlessFileAddQue, &ulActualEvents, TX_WAIT_FOREVER);
		HOST_ASSERT(err == SP5K_SUCCESS);
		profLogPrintf(0, "#S#SEAMLESS#N#Q%x", ulActualEvents);
		/* check disk busy status first. */
        appTimeDelay(/*seamLessDelayMs*/2000, 0);/*why 13 sec? weikai modify 180503*/

#if JET_PANA_CDFS
	if (ulActualEvents & EVENT_DEL_VIDEO_FILE) {  
		while (1) {                
			UINT32 freesec, seamsec;                
			freesec = appVideoRemainSecGet();                
			seamsec = appVideoSeamLessSecGet(appActiveStateGet());                
			HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: free time=%d sec,limit time%d sec",freesec,seamsec);                
			if(freesec < seamsec) {                    
				appCdfsTheOldestFileDel();                    
				HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s: del old file", __FUNCTION__);                
				} else                    
				break;            
				}       
		}
		if(ulActualEvents & EVENT_DEL_FILE_BY_AVN){//weikai modify 180424
				AVN_FILE_DELETE_PARA_T *pDelete;
				UINT8 status;
				queryAvailableSpace_t setting;
				printf("[%s] Received EVENT_DEL_FILE_BY_AVN\n", __FUNCTION__);
				pDelete = jet_avn_file_delete_para_get();
				if(pDelete == NULL){
					printf("pDelete is NULL\n");
					break;
				}
				status = jetFileListDelete(pDelete);
				appFileDeleteFB(status);
				status = jetQueryAvailableSpaceGet(&setting);
				appInfoQueryAvailableSpaceFB(status, setting);
			}
#else

       	UINT32 diskmeas, vfifo, afifo;
		while (1) {
			if(appVideoRemainSecGet() < appVideoSeamLessSecCriticalGet(APP_STATE_VIDEO_REC)) {
				profLogPrintf(0, "#P#SEAMLESS#N#DEL%d<%d", appVideoRemainSecGet(), appVideoSeamLessSecCriticalGet(APP_STATE_VIDEO_REC));
				break;	/* remain space is cirtically low, go deleting file anyway. */
			}
			sp5kDiskCfgGet(SP5K_DISK_BUSY_MEAS_CFG, &diskmeas);
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_REC_STATE_VIDEO_FIFO_USED, &vfifo);
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_REC_STATE_AUDIO_FIFO_USED, &afifo);
            if(diskmeas>=seamLessMeasDiskRatio || vfifo>=seamLessMeasVfifoCnt) {
                appTimeDelay(seamLessMeasDelayMs, 0);
            }
			else {
				break;
			}
		}
		profLogPrintf(0, "#P#SEAMLESS#N#%d,%d,%d", diskmeas, vfifo, afifo);

		if (ulActualEvents & ~EVENT_VIDEO_ALL) {	/* not an event. */
			/* it is a file name buffer pointer */
			UINT8 *fname = (UINT8 *)ulActualEvents;
			sp5kFree(fname);
		}

		if (ulActualEvents == EVENT_VIDEO_FILE_DEL) {
			if(appCdfsCanDelVideoSizeGet())	{
				appCdfsTheOldestFileDel();
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "vset: %s: del old file", __FUNCTION__);
			}
			else {
                HOST_PROF_LOG_PRINT(LEVEL_WARN, "vset: %s: check video size fail", __FUNCTION__);
			}
		}
		else {
			if(appCdfsCanDelVideoSizeGet())	{
				while(appVideoRemainSecGet() < appVideoSeamLessSecGet(APP_STATE_VIDEO_REC) * 2) {
						err = appCdfsTheOldestFileDel();
	                HOST_PROF_LOG_PRINT(LEVEL_INFO, "vset: %s: del old file", __FUNCTION__);
						appTimeDelay(measDiskTime, 0);
						if (err != SUCCESS)
							break;
				}
			}
			else {
                HOST_PROF_LOG_PRINT(LEVEL_WARN, "vset: %s: check video size fail", __FUNCTION__);
			}
		}
		profLogAdd(0, "#E#SEAMLESS#N#.");
#endif
	}
}

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

/*--------------------------------------------------------------------------*
* Function name    : appSeamlessDelNormalVideoFile
* Function         : Set event flag to delete video file
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appSeamlessDelNormalVideoFile(void)
{
    sp5kOsEventFlagsSet(&seamLessDelFileEvt, EVENT_DEL_VIDEO_FILE, TX_OR);
}
/**
 * Set event flag to delete gevent file
 * @param event
 * @return none
 * @author scchang
 * @date 08/29/2017
 *
 */
 /*--------------------------------------------------------------------------*
* Function name    : jet_appSeamlessDelFileEvent
* Function         : Set queue to delete event file
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : event (ULONG EVENT_DEL_VIDEO_FILE/EVENT_DEL_GEVENT_FILE/
                     EVENT_DEL_FILE_BY_AVN/EVENT_DEL_FILE_ALL)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/
void jet_appSeamlessDelFileEvent(ULONG event)
{
	printf("[%s] event = 0x%u\n", __FUNCTION__, event);
    //sp5kOsEventFlagsSet(&seamLessDelFileEvt, event, TX_OR);
	sp5kOsQueueSend(&seamlessFileAddQue , (void*)&event, TX_OR);//weikai added 180424
}
#endif
#endif

/*--------------------------------------------------------------------------*
* Function name    : AppVideoSeamlessCallback
* Function         : you can do something when video seamless
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void AppVideoSeamlessCallback(void)
{
	profLogAdd(0, "#S#SEAMLESS_CB#N#.");
	#if AEAWB_DEBUG
	appCalibData_t *pCalib;
	pCalib = appCalibDataGet();
	if(pCalib->aedebug == TRUE){

	}

	if(pCalib->awbdebug == TRUE){

	}
	#endif

	if (pUiSetting->mediaFormat == MEDIA_FORMAT_AVI)
    	appExif_SetAviChunk();

	#if SP5K_CDFS_OPT
	UINT32 filehandle=0;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	//if(appCdfsAddFileStsGet()) { //paul modify 2018 0323
	if(pUiSetting->mediaFormat == MEDIA_FORMAT_AVI&&appCdfsAddFileStsGet()){
		appCdfsFileSync(appCdfsLastAviNameGet(), filehandle);
	}
#else
	if(appCdfsAddFileStsGet()) {
		appCdfsFileSync(appCdfsLastAviNameGet(), filehandle);
	}
#endif
	#else
	UINT32 fnamesize=128;
	UINT8 *fname = sp5kMallocCache(fnamesize);
    UINT32 totalFile;
    sp5kDcfAttrElm_t pdcfAttr;
	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM,&totalFile);
	if(sp5kDcfFsFileAttrGet(totalFile, &pdcfAttr, fname))
		printf("[RTP] Get dcf attr fail!\n");
	else
		printf("totalFile = %u, media file : %s\n",totalFile, fname);
	#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 msg = EVENT_DEL_VIDEO_FILE;
#else
	UINT32 msg = EVENT_VIDEO_CHECK_SPCAE;
#endif
	if (sp5kOsQueueSend(&seamlessFileAddQue, &msg, TX_NO_WAIT) != SUCCESS) {
		profLogAdd(0, "#P#SEAMLESS_CB#N#QueFull!");
	}
	profLogAdd(0, "#E#SEAMLESS_CB#N#.");
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoSeamLessTimeSet
* Function         : you can set seamless time
* Return value	   : void
* Parameter1	   : val (UINT32 SUCCESS/FAIL)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoSeamLessTimeSet(UINT32 val)
{
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "vset: %s: time=%d", __FUNCTION__, val);
  	sp5kMediaRecCfgSet(SP5K_MEDIA_REC_SEAMLESS_TIME_SLOT, val); /* Second */


    #if defined(HW_PDVR)  /*Isaac Add 20161117*/
	if (pViewParam->videoSeamless == UI_VIDEO_SEAMLESS_OFF)
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_EVENT_TRIGGER_EN, DISABLE);
	else
	{
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_EVENT_TRIGGER_EN, ENABLE);

		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_EVENT_TRIGGER_TYPE,SP5K_MEDIA_REC_INTERRUPT_SEAMLESS);

		UINT32 evttrigtimeslot=20*1000; //ms
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_TRIGGER_TIME_SLOT, evttrigtimeslot);
	}


    #endif

    if (pViewParam->videoSeamless != UI_VIDEO_SEAMLESS_OFF) {
  	    /*disk threshold setting that use for DCF rule, The remaining space is reference to appVideoSeamLessSecGet() and disk threshold setting is useless for CDFS. */
		UINT32 diskthrkb=64*1024; //KBytes
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DISK_THR_KB, diskthrkb);
        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_SEAMLESS_CALLBACK, (UINT32)AppVideoSeamlessCallback);

    	#if GPS_OPEN
    	//sp5kMediaRecCfgSet(SP5K_MEDIA_REC_USRDATA_CALLBACK, (UINT32)appUsrDataWriteEndCallback);
    	#endif

        /*seamless cdfs control*/
        #if SP5K_CDFS_OPT
        if(seamLessThrCreated==FALSE)
        {
    	    seamLessDelFileThr = sp5kOsThreadCreate("thrSeamlessDelFile", appSeamlessDelFileThread, 0/*param*/,
    		    24/*prio*/, 24/*preempt_thr*/, 100/*time_slice*/, TX_AUTO_START/*auto_start*/);

            if(seamLessDelFileThr!=NULL)
            {
                seamLessThrCreated =TRUE;
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "vset: %s: seamless thr create", __FUNCTION__);
            }
            else
            {
                seamLessThrCreated =FALSE;
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vset: %s: seamless thr fail", __FUNCTION__);
            }
        }
        #endif

        //For no 30fps/60fps would make seamless calculate mistake, need set time_scale
        {
            mediaVideoTags_t videoTag = {0};

            sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_REC_TAGS, (UINT32*)&videoTag);

	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	   if(pViewParam->videoSize == UI_VIDEO_SIZE_FHD_30FPS || pViewParam->videoSize == UI_VIDEO_SIZE_FHD_PARK ){
	   	if(appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP)
	   		 pViewParam->videoSize = UI_VIDEO_SIZE_FHD_PARK;
		else
			 pViewParam->videoSize = UI_VIDEO_SIZE_FHD_30FPS;

	   }

	     videoTag.time_scale = VideoSize[pViewParam->videoSize].frameRate*1000;

	#else
	     videoTag.time_scale = VideoSize[pViewParam->videoSize].frameRate*1000;
	#endif
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_REC_TAGS, (UINT32)&videoTag);
        }
    }
    #if SP5K_CDFS_OPT
    else
    {
        if(seamLessThrCreated)
        {
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "vset: %s: seamless thr destory", __FUNCTION__);
		    sp5kOsThreadTerminate(seamLessDelFileThr);
    		seamLessDelFileThr = NULL;
	    	sp5kOsQueueDelete(&seamlessFileAddQue);
		    seamLessThrCreated = seamlessFileAddQue = FALSE;
        }
    }
    #endif
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoLogFileInit
* Function         : video debug buff init
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoLogFileInit(void)
{
   memset(videoDebugBuff, 0, VIDEO_LOG);
   videoLogCount =0	;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoLogFile_Print
* Function         : video debug buff save flow
* Return value	   : void
* Parameter1	   : format (const char *)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoLogFile_Print(const char *format, ...)
{
 	char buf[1024];
	UINT8 tempCount =0 ;
	va_list	ap;
    UINT32 currMode;

	sp5kModeGet(&currMode);
	if(currMode != SP5K_MODE_VIDEO_RECORD)
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vset: %s not rec mode",__FUNCTION__);
		return ;
	}

	if(videoLogCount > VIDEO_LOG )
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vset: %s over video log size",__FUNCTION__);
		return ;
	}

	memset(buf, 0, 1024);
	va_start(ap, format);
	vsprintf(buf,format,ap);
	va_end(ap);

	tempCount = strlen(buf);
	memcpy(&videoDebugBuff[videoLogCount], buf, tempCount);
	videoLogCount += tempCount ;

	if((videoLogCount+100) > VIDEO_LOG )
	{
		appVideoLogFileSave();
		appVideoLogFileInit();
		wcount++;
	}
	DBG_PRINT("vset: %s\n", buf);
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoLogFileSave
* Function         : video debug buff save function
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoLogFileSave(void)
{
#if SP5K_CDFS_OPT
	char log[32];

	/* Get current file index  and set the TXT file */
	strcpy((char*)fname, (const char*)appCdfsLastFileNameGet());

	strtok(fname ,".");
	sprintf(log, "%s_%d.TXT",fname, wcount);
	WriteFile((UINT8*)log, (UINT8*)videoDebugBuff, VIDEO_LOG);
	DBG_PRINT("vset: videoLogCount %s %s %d\n",fname ,log ,wcount);
#endif
}

#if GPS_OPEN

/*--------------------------------------------------------------------------*
* Function name    : appUsrDataWriteEndCallback
* Function         : you can register this callback with SP5K_MEDIA_REC_FILE_CLOSE_CALLBACK,
                     this callback function will write user data(tag) with close video file
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appUsrDataWriteEndCallback(void)
{	/* related mantis ID 0048262 */
	appUsrDataWriteEnd();
	if(LastFileNotRellyClose!=GPS_FILE_CLOSE)
	{
		LastFileNotRellyClose=GPS_RECEIVE_MSG;
	}
	appGGInfoHdrWriteEn(ENABLE);
}
#endif

/*--------------------------------------------------------------------------*
* Function name    : appVideoDateStampResSet
* Function         : set video date stamp resource
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoDateStampResSet(void)
{
    static UINT8 size_idx = 0xff;

    if(size_idx!=pViewParam->videoSize)
    {
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "vset: stamp res change %d->%d",size_idx ,pViewParam->videoSize);
        size_idx = pViewParam->videoSize;
    }
    else
        return;

    if((pViewParam->videoSize==UI_VIDEO_SIZE_FHD_60FPS) && IS_HDMI_IN){
        size_idx = pViewParam->videoSize = UI_VIDEO_SIZE_FHD_30FPS;
	}

    if(size_idx>=UI_VIDEO_SIZE_MAX)
    {
      size_idx = pViewParam->videoSize = 0;
      HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vset: stamp res change idx fail=%d",size_idx);
    }

    /*2nd*/
    #if SP5K_SPC_MODE && defined(SP5K_SENSOR_SONYIMX206)
	if(SENSOR_MODE_1440P_30FPS == VideoSize[size_idx].sensormode)
		sp5kModeCfgSet(SP5K_MODE_CFG_PV_2ND_PATH_ENABLE, ENABLE);
	else if(SENSOR_MODE_VGA_240FPS == VideoSize[size_idx].sensormode)
		sp5kModeCfgSet(SP5K_MODE_CFG_PV_2ND_PATH_ENABLE, DISABLE);
    #endif

    /*res*/
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "vset: stamp res load %s",VideoSize[size_idx].datestamp);

	if(sp5kResourceFileLoad(SP5K_RES_DATE_STAMP_OSD, (const char *)RootGet(VideoSize[size_idx].datestamp)) != SUCCESS)
		 HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vset: stamp res load error %d",__LINE__);
}

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
static UINT8 mtEnable = APP_VIDEO_DUAL_FILE;

/*--------------------------------------------------------------------------*
* Function name    : appVideoMultiTrackSet
* Function         : APP_VIDEO_SINGLE_FILE/APP_VIDEO_DUAL_FILE video multi-track
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : bEnable (UINT32 APP_VIDEO_SINGLE_FILE/APP_VIDEO_DUAL_FILE)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoMultiTrackSet(
    UINT8 bEnable
)
{
	if((bEnable==APP_VIDEO_SINGLE_FILE) || (bEnable==APP_VIDEO_DUAL_FILE))
	{
    	mtEnable = bEnable;
		return SUCCESS;
	}
	else
		return FAIL;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoMultiTrackGet
* Function         : get video multi-track status
* Return value	   : mtEnable (UINT8 APP_VIDEO_SINGLE_FILE/APP_VIDEO_DUAL_FILE)
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT8
appVideoMultiTrackGet(
    void
)
{
    return mtEnable;
}
#endif

/*--------------------------------------------------------------------------*
* Function name    : appReset4k2kSetting
* Function         : reset 4k2k setting
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appReset4k2kSetting(void)
{
	/**SHEN: This is special function for reset MultiTrack command setting, ICOM-2771, CathyLite rev.9416**/
    /**Relative to appVideoSizeSet() for 4K2K/2.7K setting */
	/**Now is use in different state initial for reset, so remove sp5k mode switch**/
    printf("%s(#%d) Current Size=0x%X\n", __FUNCTION__, __LINE__, pViewParam->videoSize);
	if(pViewParam->videoSize == UI_VIDEO_SIZE_4K2K||pViewParam->videoSize == UI_VIDEO_SIZE_27K)    /* 4k video test */
	{
		UINT32 mode, sizefactorcfg=((1<<4) | 1) /*0x11*/;
		sp5kModeGet(&mode);
		printf("%s(#%d) Current mode=0x%X\n", __FUNCTION__, __LINE__, mode);
		sp5kModeCfgSet(SP5K_MODE_CFG_STANDBY_DUP, ENABLE);
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		appModeCfgPvRawSet(SP5K_MODE_PV_RAW_DO_DISABLE);
		/* param1[0:3]width factor, 0 - 1x, 1 - 1/2, 2 - 1/4, param1[4:7] height factor, 0 - 1x, 1 - 1/2, 2 - 1/4. */
		sp5kSystemCfgSet(SP5K_CDSP_2NDPATH_FACTOR_CFG, sizefactorcfg);  /* default */ 
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appResetCmdSetting
* Function         : this is special function for reset MultiTrack command setting
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static BOOL bCmdMT=DISABLE, bCmdVFI=DISABLE;
static UINT8 PreMediaFormat=0xFF;
void appResetCmdSetting(void)
{
	/**SHEN: This is special function for reset MultiTrack command setting, ICOM-2771, CathyLite rev.9416**/
	if (bCmdMT || bCmdVFI)
    {
        printf("\n\nResetCmdSetting MT=%d  pre=%d\n", bCmdMT, PreMediaFormat);
        callStack();
        pUiSetting->mediaFormat = PreMediaFormat;
        if (PreMediaFormat == MEDIA_FORMAT_AVI)
        {
            SET_VIDEO_TYPE(SP5K_MEDIA_AVI);
        }
        else if (PreMediaFormat == MEDIA_FORMAT_MOV)
        {
            SET_VIDEO_TYPE(SP5K_MEDIA_MOV);
        }
	 	else if (PreMediaFormat == MEDIA_FORMAT_MP4)
        {
            SET_VIDEO_TYPE(SP5K_MEDIA_MP4);
        }
        PreMediaFormat = 0xFF;
	    bCmdMT = bCmdVFI = DISABLE;
    }
}

/*--------------------------------------------------------------------------*
* Function name    : pvMainCb
* Function         : used to preview urgent callback, you can get frame information
* Return value	   : result (static UINT32 SUCCESS/FAIL)
* Parameter1	   : stream index (UINT32 nth stream, from 0)
* Parameter2	   : frame id (UINT32)
* Parameter3	   : get buffer information (frameBufInfo_t *)
* Parameter4	   : duplicated buffer address (UINT8 *)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static UINT32 pvCbDone=0, pvmain_flag=1, pv2nd_flag=2;
static UINT32
pvMainCb(
	UINT32 streamId,		/* nth stream, from 0 */
	UINT32 fid, 			/* frame id */
	frameBufInfo_t *pframe, /* buffer information */
	UINT8 *pbufdup			/* duplicated buffer address */
)
{
	if (!(pvCbDone&pvmain_flag)) {
		pvCbDone |= pvmain_flag;
		fsSimpleWrite((UINT8*)"D:\\MAIN.YUV", pframe->pbuf, pframe->width*pframe->height*2);// yuv buffer size= w*h*2
	}
	return 0;
}

/*--------------------------------------------------------------------------*
* Function name    : pv2ndCb
* Function         : used to preview urgent callback, you can get frame information
* Return value	   : result (static UINT32 SUCCESS/FAIL)
* Parameter1	   : stream index (UINT32 nth stream, from 0)
* Parameter2	   : frame id (UINT32)
* Parameter3	   : get buffer information (frameBufInfo_t *)
* Parameter4	   : duplicated buffer address (UINT8 *)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static UINT32
pv2ndCb(
	UINT32 streamId,		/* nth stream, from 0 */
	UINT32 fid, 			/* frame id */
	frameBufInfo_t *pframe, /* buffer information */
	UINT8 *pbufdup			/* duplicated buffer address */
)
{
	if (!(pvCbDone&pv2nd_flag)) {
		pvCbDone |= pv2nd_flag;
		fsSimpleWrite((UINT8*)"D:\\2ND.YUV", pframe->pbuf, pframe->width*pframe->height*2/*for yuv size*/);
	}
	return 0;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoPvCap
* Function         : use sp5kVideoUrgentCallbackSet to get yuv on video preview mode
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static sp5kPrevUrgentCallback_t pvCb[2] = {
	[0] = {.fp = pvMainCb},
	[1] = {.fp = pv2ndCb}
};
static void
appVideoPvCap(
	void
)
{
	UINT32 dup_stream=0;
	sp5kVideoUrgentCallbackSet(INTERVAL_FRAMES_COMES, dup_stream, pvCb, NUMBER_TWO_STREAMS);
	sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
	sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecFileNameGetCb
* Function         : get video file name callback
* Return value	   : fileName (UINT32)
* Parameter1	   : handle (UINT32)
* Parameter1	   : fileName (UINT32)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoRecFileNameGetCb(
	UINT32 handle,
	UINT32 fileName
)
{
	appCdfsNextFileNameByChGet((UINT8 *)"evt", SP5K_DCF_FILETYPE_MOV, (UINT8 *)fileName);

	return fileName;
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoFpsGet
* Function         : get video frame rate
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appVideoFpsGet(
	void
)
{
	return VideoSize[pViewParam->videoSize].frameRate;
}

/*--------------------------------------------------------------------------*
* Function name    : appAddEventFile
* Function         : add event file flow
* Return value	   : void
* Parameter1	   : event trig file name (UINT8 *)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appAddEventFile(UINT8 *evtfilename)
{
	UINT32 TestFileEventHandle=0;
	
	appCdfsFileAdd(evtfilename,&TestFileEventHandle);
	appCdfsFileSync(evtfilename,TestFileEventHandle);	
	CDFS_FILE_TAB_T* evtlock;
	evtlock = (CDFS_FILE_TAB_T *)TestFileEventHandle;
	evtlock->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
	sp5kFsFileAttrSet(evtfilename,evtlock->attr |SP5K_FS_ATTR_READONLY);
}

/*--------------------------------------------------------------------------*
* Function name    : appEventFileChangedCallback
* Function         : event trig flow callback
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : this is dual stream handle, value from appVideoDualStreamStart (UINT32)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appEventFileChangedCallback(UINT32 dualhandle) // This test cb only use for SP5K_MEDIA_REC_INTERRUPT_SEAMLESS case.
{
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
	if((appDualPipModeGet() == DUALSEN_PIP_MAIN) || (appDualPipModeGet() == DUALSEN_PIP_SEC))
	{
		if(dualhandle == DualFile1)
		{
	if(TestEvtFlagF==EVENT_TRIG_OPEN)
			{
		TestEvtFlagF = EVENT_TRIG_CLOSE;
			}
	else if(TestEvtFlagF==EVENT_TRIG_CLOSE)
	{
				TestEvtFlagF = EVENT_TRIG_DONE;
				if(TestEvtFlagR==EVENT_TRIG_CLOSE)
					TestEvtFlagR = EVENT_TRIG_DUAL;
			}
			else if(TestEvtFlagF==EVENT_TRIG_DUAL)
			{
		TestEvtFlagF = EVENT_TRIG_DONE;
		appAddEventFile(TestFilePathF);
				appAddEventFile(TestFilePathR);
			}
		}
		else if(dualhandle == DualFile2)
		{
			if(TestEvtFlagR==EVENT_TRIG_OPEN)
			{
				TestEvtFlagR = EVENT_TRIG_CLOSE;
			}
			else if(TestEvtFlagR==EVENT_TRIG_CLOSE)
			{
				TestEvtFlagR = EVENT_TRIG_DONE;
				if(TestEvtFlagF==EVENT_TRIG_CLOSE)
					TestEvtFlagF = EVENT_TRIG_DUAL;
			}
			else if(TestEvtFlagR==EVENT_TRIG_DUAL)
			{
				TestEvtFlagR=EVENT_TRIG_DONE;
				appAddEventFile(TestFilePathF);
				appAddEventFile(TestFilePathR);
			}
		}
	}
	else
#endif
	{
		if(TestEvtFlagF==EVENT_TRIG_OPEN)
		{
			TestEvtFlagF = EVENT_TRIG_CLOSE;
		}
		else if(TestEvtFlagF==EVENT_TRIG_CLOSE)
		{
			TestEvtFlagF = EVENT_TRIG_DONE;
			appAddEventFile(TestFilePathF);
		}
	}
}

void
cmdVideo(
	UINT32 argc,
	UINT8 *arg[],
	UINT32 v[]
)
{
	if (argc==1) {
		printf("video usage:\n"
		       "video type\n"
		       "video evt\n"
		       "video mt\n"
		       "video acodec\n"
		       "video vfi\n"
		       "video doMd\n"
		       "video setMdLev\n"
		       "video setMdLevPara\n"
		       "video gyro cal"
		       );
	}
#if SP5K_ICAT_EIS
	else if (strcmp((const char *)arg[1], "gyro")==0 ||
		strcmp((const char *)arg[1], "is")==0) {
		cmdAppIs(argc, arg, v);
	}
#endif
	else if (strcmp((char *)arg[1], "pvcap") == 0) {
		appVideoPvCap();
	}
    else if (strcmp((const char *)arg[1], "type")==0)
    {
        if (strcmp((const char *)arg[2], "avi")==0)
        {
            printf("Set file type to avi\n");
            /*sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_AVI);*/
			SET_VIDEO_TYPE(SP5K_MEDIA_AVI);
        }
        else if (strcmp((const char *)arg[2], "mov")==0)
        {
            printf("Set file type to mov\n");
            /*sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_FILE_TYPE, SP5K_MEDIA_MOV);*/
			SET_VIDEO_TYPE(SP5K_MEDIA_MOV);
        }
        else if (strcmp((const char *)arg[2], "mp4")==0)
        {
            printf("Set file type to mp4\n");
            SET_VIDEO_TYPE(SP5K_MEDIA_MP4);
        }
        else
            printf("Please set file type: avi/mov/mp4\n");
    }
	/* Event trigger */
	else if (strcmp((const char *)arg[1], "evt")==0)
    {
        if (argc>2)
        {
            if (strcmp((const char *)arg[2], "init")==0)
            {
                if (argc>3)
                {
                    if (strcmp((const char *)arg[3], "int")==0)
                    {
                        #if SP5K_USB_UVC_SUPPORT
            			#define TestTime 20
                        #else
                        #define TestTime 6
                        #endif
                        printf("Event Trigger Interrupt Mode\n");

                        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_EVENT_TRIGGER_EN, 1);
                        //set event trigger type
                        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_EVENT_TRIGGER_TYPE,SP5K_MEDIA_REC_INTERRUPT_SEAMLESS);

                        // Set event record interval.
                        // Set to 10000, it means recording file includs 5000ms before and 5000ms after trig point .
                        sp5kMediaRecCfgSet(SP5K_MEDIA_REC_TRIGGER_TIME_SLOT, TestTime*1000);
						sp5kMediaRecCfgSet(SP5K_MEDIA_REC_FILE_CHANGED_CALLBACK, (UINT32)appEventFileChangedCallback);
                    }
                }
                else
                {
                    printf("event init usage:\n"
                        "video evt init int\n"
                        "video evt init nint\n"
                    );
                }
            }
			else if(strcmp((const char *)arg[2], "trig")==0)
			{
				UINT32 file_type;
				unsigned char	TestFilePathFtemp[32];
				UINT8 path[20]={0}, fileNameTemp[64]={0};
				appCdfsFolder2Path(CDFS_FILE_FOLDER_EVENT,path);// get event folder path
				printf("Start Event Trigger\n");

				// trigger open and get evt file name
				TestEvtFlagF = EVENT_TRIG_OPEN;

				sp5kMediaRecAttrGet(MEDIA_ATTR_FILE_TYPE ,&file_type);
				appCdfsNextFileNameGet(file_type,TestFilePathFtemp);
				appCdfsAbsolutePathToFile(TestFilePathFtemp, fileNameTemp);// change event folder file name
				sprintf((char *)TestFilePathF,"%s/%s",path,fileNameTemp);
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
				/* unsigned char	TestFilePathRtemp[32]; */
				if (appVideoMultiTrackGet() == APP_VIDEO_DUAL_FILE)
				{
					if((appDualPipModeGet() == DUALSEN_PIP_MAIN) || (appDualPipModeGet() == DUALSEN_PIP_SEC))
					{
						unsigned char	TestFilePathRtemp[32];
						TestEvtFlagR = EVENT_TRIG_OPEN;
						appCdfsNextSubFileNameGet(file_type, FALSE, TestFilePathRtemp);
						appCdfsAbsolutePathToFile(TestFilePathRtemp, fileNameTemp);// change event folder file name
						sprintf((char *)TestFilePathR,"%s/%s",path,fileNameTemp);
					}
				}
#endif
				// set file name
				sp5kMediaRecCfgSet(SP5K_MEDIA_REC_TRIGGER_FILE_NAME, (UINT32)TestFilePathF);
#if SP5K_USB_UVC_SUPPORT
				if(appUvcHostCfgGet(UVC_HOST_CFG_PLUG_COMPLETE))
					sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_TRIGGER_FILE_NAME, (UINT32)TestFilePathR);
				else if(appDualPipModeGet() == DUALSEN_SINGLE_SEC) // single sec case
					sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_TRIGGER_FILE_NAME, (UINT32)TestFilePathF);
#elif SP5K_SENSOR_DUAL_MODE
				if((appDualPipModeGet()>= DUALSEN_PIP_MAIN) && (appVideoMultiTrackGet() == APP_VIDEO_DUAL_FILE))
					sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_TRIGGER_FILE_NAME, (UINT32)TestFilePathR);
				else if(appDualPipModeGet() == DUALSEN_SINGLE_SEC)// single sec case
					sp5kMediaRecCfgSet(SP5K_MEDIA_REC_2ND_TRIGGER_FILE_NAME, (UINT32)TestFilePathF);
#endif
				// Set event record start.
				sp5kMediaRecControl(SP5K_MEDIA_REC_EVENT_TRIGGER_MODE);
			}
		}
		else
		{
			printf("event usage:\n"
			"video evt init\n"
			"video evt trig\n"
			);
		}
	}
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
	else if (strcmp((const char *)arg[1], "mt")==0)
	{
		if (v[2] == 0x01)
		{
			printf("@@ Enable Multi-track video function (JUST support format AVI NOW)\n");
			bCmdMT = ENABLE;
			PreMediaFormat = pUiSetting->mediaFormat;
			pUiSetting->mediaFormat = MEDIA_FORMAT_AVI;
			SET_VIDEO_TYPE(SP5K_MEDIA_AVI);
			appVideoMultiTrackSet(APP_VIDEO_SINGLE_FILE);
		}
		else
		{
			printf("@@ Disable Multi-track video function\n");
			appResetCmdSetting();
			appVideoMultiTrackSet(APP_VIDEO_DUAL_FILE);
		}
	}
#endif
    else if (strcmp((const char *)arg[1], "acodec")==0) {
        if (argc>2) {
            if (strcmp((const char *)arg[2], "PCM")==0) {
                printf("Set audio codec to PCM\n");
                VideoSize[ pViewParam->videoSize].audio = SP5K_MEDIA_AUDIO_PCM;
                sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_PCM);
            }
            else if (strcmp((const char *)arg[2], "AAC")==0) {
                printf("Set audio codec to AAC\n");
                VideoSize[ pViewParam->videoSize].audio = SP5K_MEDIA_AUDIO_AAC;
                sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_CODEC, SP5K_MEDIA_AUDIO_AAC);
                sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_AUDIO_AVG_BITRATE, 128000);
            }
        }
        else {
            printf("acodec usage:\n"
                "video acodec PCM\n"
                "video acodec AAC\n"
            );
        }
    }
    else if (strcmp((const char *)arg[1], "vfi")==0) {
        if (argc>2) {
            if (strcmp((const char *)arg[2], "on")==0) {
                mediaVideoTags_t videoTag = {0};

                printf("Enable VFI\n");

            	bCmdVFI = ENABLE;
				PreMediaFormat = pUiSetting->mediaFormat;
				pUiSetting->mediaFormat = MEDIA_FORMAT_AVI;
				SET_VIDEO_TYPE(SP5K_MEDIA_AVI);
                sp5kMediaRecCfgSet(SP5K_MEDIA_REC_PERIODIC_SYNC_EN, TRUE);
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
                sp5kMediaRecCfgSet(SP5K_MEDIA_REC_RECOVERY_FLUSH_INTERVAL,
                    VideoSize[pViewParam->videoSize].frameRate << 2);
#else
                sp5kMediaRecCfgSet(SP5K_MEDIA_REC_RECOVERY_FLUSH_INTERVAL,
                VideoSize[pViewParam->videoSize].frameRate << 1);
#endif
                sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_REC_TAGS, (UINT32*)&videoTag);
                videoTag.moov_first = TRUE;
                sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_REC_TAGS, (UINT32)&videoTag);
            }
            else if (strcmp((const char *)arg[2], "off")==0) {
                mediaVideoTags_t videoTag = {0};

                printf("Disable VFI\n");

                sp5kMediaRecCfgSet(SP5K_MEDIA_REC_PERIODIC_SYNC_EN, FALSE);
                sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_REC_TAGS, (UINT32*)&videoTag);
                videoTag.moov_first = FALSE;
                sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_REC_TAGS, (UINT32)&videoTag);
                appResetCmdSetting();
            }
        }
        else {
            printf("vfi usage:\n"
                "video vfi on\n"
                "video vfi off\n"
            );
        }
    }
#if SP5K_MOTION_DETECT ||  SP5K_MOTION_DETECT_FRONT || SP5K_MOTION_DETECT_REAR
    else if(strcmp((const char *)arg[1], "doMd")==0){
        if( argc == 4){
            if( 0 == v[2]){                         /* front motion detection */
                if( 1 == v[3]){                     /* enable */
                    appMotionDetectSet(APP_MD_EN, TRUE);
                }
                else{                              /* disable */
                    appMotionDetectSet(APP_MD_EN, FALSE);
                }
            }
            else if( 1 == v[2]){                    /* yuv cam rear motion detection */
                if( 1 == v[3]){                     /* enable */
                    appMotionDetectSet(APP_MD_EN_REAR, TRUE);
                }
                else{                               /* disable */
                    appMotionDetectSet(APP_MD_EN_REAR, FALSE);
                }
            }
            else if( 2 == v[2]){                    /* yuv cam front motion detection */
                if( 1 == v[3]){                     /* enable */
                    appMotionDetectSet(APP_MD_EN_FRONT, TRUE);
                }
                else{                               /* disable */
                    appMotionDetectSet(APP_MD_EN_FRONT, FALSE);
                }
            }
        }
    }
    else if(strcmp((const char *)arg[1], "setMdLev")== 0){
        if(argc == 4){     /*set sensitivity level, 0:high 1:midde, 2:low */
            if( 0 == v[2]){         //raw front cam
                if(v[3] >= 0 && v[3] <= 2){
                    printf("set raw front cam level = %d\n", v[3]);
                    appMotionDetectSet(APP_MD_CHKLV_SET, (v[3] + 0x65));
                }
            }
            else if( 1 == v[2]){    //yuv rear cam
                if(v[3] >= 0 && v[3] <= 2){
                    printf("set yuv rear cam level = %d\n", v[3]);
                    appMotionDetectSet(APP_MD_CHKLV_REAR_SET, v[3]);
                }
            }
            else if( 2 == v[2]){    //yuv front cam
                if(v[3] >= 0 && v[3] <= 2){
                    printf("set yuv front cam level = %d\n", v[3]);
                    appMotionDetectSet(APP_MD_CHKLV_FRONT_SET, v[3]);
                }
            }
        }
    }
    else if(strcmp((const char *)arg[1], "setMdLevPara")== 0){
        if(argc == 6){     /*set sensitivity level parameters */
            printf("set manual sensitivity para: cam:%d lev:%d thr:%d percentage:%d\n", v[2], v[3], v[4], v[5]);
            if( 0 == v[2]){         //raw front cam
                appMotionDetectSet(APP_MD_CUSTOM_CHKLV_SET, v[2], v[3] + 0x65, v[4], v[5]);
            }
            else if( 1 == v[2]) {    //v[2] == 1: yuv rear cam
                appMotionDetectSet(APP_MD_CUSTOM_CHKLV_SET, CAM_REAR_YUV, v[3], v[4], v[5]);
            }
            else if( 2 == v[2]) {    //v[[2] == 2 : yuv front cam
                appMotionDetectSet(APP_MD_CUSTOM_CHKLV_SET, CAM_FRONT_YUV, v[3], v[4], v[5]);
            }
        }
    }
#endif
}



