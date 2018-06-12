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
 *  File name    : App_video_record.c
 *  Function     : about video record state		   
 *  First editor :   
 *  First date   :   
 *  History      :
 *               : 2018.05.29  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#define HOST_DBG                    0

#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_media_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_utility_api.h"

#include "app_pal.h"
#include "app_icon_def.h"
#include "app_zoom_api.h"
#include "app_face_detect_ex.h"
#include "app_still.h"
#include "app_video.h"
#include "app_error_msg.h"
#include "app_view_param.h"
#include "app_calib_api.h"
#include "app_view_osd.h"
#include "app_osd.h"
#include "app_view_osd.h"
#include "app_osd_api.h"
#include "app_osdfont.h"
#include "app_video.h"
#include "app_aealg_debug.h"
#include "app_error_msg.h"
#include "app_sio_filter.h"

#if GPS_OPEN
#include "app_gps.h"
#endif

#include "sp5k_dbg_api.h" /**< HOST_ASSERT(expr); */
/* Jerry add for 3A info debug */
#if (SP5K_3A_INFO_DBG)
#include "sp5k_dq_api.h"
#include "app_3aalg.h"
#endif

#if SP5K_ICAT_EIS
#include "sp5k_eis_rsc_algo_api.h"
#endif
#if SP5K_USB_UVC_SUPPORT
#include "app_video_uvc.h"
#include "api/sp5k_usbhost_api.h"
#endif

#include "app_ui_para.h"

#if SP5K_MOTION_DETECT
#include "app_md.h"
#endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_infohandlerComm.h"
#include "jet_utility.h"

extern MoveFileState infoFuncMoveFileState;
extern PowerDetect appInfoPowerDETStateGet(void);
extern void appGsensorClean(void);
extern char * appInfoPoisitonEventGet(void);
extern void appInfoPositionEventClean(void);
extern void appInfoConnStart(BOOL);
extern UINT32 ConnectDev_TimerCount ;
extern UINT32 SeamLessTime[UI_VIDEO_SEAMLESS_MAX];
extern void appGsensorClean(void);
extern char * appInfoPoisitonEventGet(void);
extern void appInfoPositionEventClean(void);
extern int appInfo3AGet(char *buf, int buf_size);
extern void appInfoRtcChangeSecSet(int sec);
extern BOOL appInfoRtcChangeCheck(void);
extern int appInfoRtcChangeSecGet(void);
extern BOOL appInfoRTCChangedGet(void);
extern UINT32 appInfoSecTimeBaseGet(tmx_t * time);
extern void appInfoUdatHeaderPosEventClean(void);
extern void appInfoUdatHeaderPEClean(void);
extern void appInfoQueryAvailableSpaceFB(UINT8 state, queryAvailableSpace_t status);
#endif
extern void appWriteUserDataInit(void);

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define VIDEO_REC_STATUS_RECORDING  0
#define VIDEO_REC_STATUS_PAUSE      1
#define VIDEO_REC_STATUS_ABORT      2
#define VIDEO_REC_STATUS_CLOSE      3

/*for loading log*/
/*must be
..\spca6330\src\base.def - Open the definition: DBG_OS_PROC_INFO = 1
..\spca6330\src\api\libcmd\private\cmd_parse.c - Turn off DBG_SW_WDG: #define DBG_SW_WDG 0
*/
#define CPU_LOADING_LOG             0

/*must be
..\spca6330\src\base.def - Close the definition: DBG_OS_PROC_INFO = 0
..\spca6330\src\api\libcmd\private\cmd_parse.c - Turn off DBG_SW_WDG: #define DBG_SW_WDG 0
*/
#define DRAM_BANDWIDTH_LOG          0

#define DEBUG                       0

#define bk_default_priority         16
#define bk_default_blocking         0

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
tmx_t chgFileTime;

static UINT32 VideoRecStatus;
static UINT32 VideoRecPaReMsgReceive =TRUE;

/*capture image in video*/  //V35 only H264 codec is supported
#if SPCA6330
static BOOL capvidimg=FALSE;
UINT32 capvidtimer=0xffffffff;
#endif
UINT8 HDR_En;

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
int tempClear =0;
int VideoSec = 0;
extern CDFS_FILE_TAB_T *lastNode;
BOOL FirstRecording = TRUE;
char firstLineBuffer[32];

UINT32 gRecStartTime;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if SP5K_MOTION_DETECT
extern UINT32 mdTime; 
extern UINT32 mdRecCount;
extern UINT8 mdStop;
#endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern UINT8 PositionData[];
extern BOOL POSDataReady;
#include "app_rtc.h"
UINT32 gRecStartTime;
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
void appAudioRecVolumeRestore(void);
extern void appPIVTrig(void);
extern UINT32 appPIVTaskInit(void);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern char *appInfoPositionSync();
extern char *appGsensorAccPXYZ();
extern char *appInfoEventSync();
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

#if GPS_DEMO //for gps demo

/*--------------------------------------------------------------------------*
* Function name    : appVideoSeamlessGPSFileDel
* Function         : delete first index GPS file
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoSeamlessGPSFileDel(void)
{
	//for demo
	UINT8 sCurrentName[32],tmpTxTBuff[32];
	AppVideoRecFirstIndexFileNameGet(sCurrentName, tmpTxTBuff);
	DBG_PRINT("vrec: DEL GPS 1st Index file=%s\n   %s\n   %s\n", sCurrentName ,tmpTxTBuff ,sFirstTXTfName);
	if(strcmp(sFirstTXTfName, tmpTxTBuff))
	{
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "vrec: del name=%s\n",sFirstTXTfName);
		sp5kFsFileDelete(sFirstTXTfName);
		memset(sFirstTXTfName ,0 ,32);
		strcpy(sFirstTXTfName , tmpTxTBuff );
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "vrec: First file name=%s\n",sFirstTXTfName);
	}
}
#endif

/*for loading log*/
#if (CPU_LOADING_LOG || DRAM_BANDWIDTH_LOG)

/*--------------------------------------------------------------------------*
* Function name    : appVideoLoadLog
* Function         : load cpu or dram log in 
* Return value	   : void
* Parameter1	   : elapsed time (UINT32 5s/10s/20s (unit second))
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoLoadLog(UINT32 elstime)
{
	UINT32 time_5s=5, time_10s=10, proc_en=3, drambwmlogsize=300;
	#if (CPU_LOADING_LOG && DRAM_BANDWIDTH_LOG)
	HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vrec: cpu/dram log fail");
	#endif

	if(elstime==time_5s)
	{
		#if CPU_LOADING_LOG
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "vrec: cpu log -_dbg_proc_ps_en");
		/**
		 * \param[in] en, proc enable mode:
		 * 0:off, 1:thread only; 2:irq too; 3:IRQ contrl; 4:sys return
		 * \return the original "en" value before the change.
		 */
		_dbg_proc_ps_en(proc_en);
		#else
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "vrec: dram log -log start");
		dramBwmLogStart(drambwmlogsize);
		#endif
	}

	if(elstime==time_10s)
	{
		#if CPU_LOADING_LOG
		HOST_ASSERT(0);
		#else
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "vrec: dram log -log dump");
		dramBwmLogDump();
		#endif
	}

	#if DRAM_BANDWIDTH_LOG
	UINT32 time_20s=20;
	if(elstime==time_20s)
	{
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "vrec: dram log -log stop");
		dramBwmLogStop();
	}
	#endif
}
#endif

#if WRITE_USER_DATA
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

UINT8 testData[] = "999999.999,999999,0,35.6812,N,139.7662,E,999.99,65535,"
        "65535,65535,65535,"            /* G sensor X, Y, and Z. */
        "4096,"                          /* Event */
        "65535,65535,65535,"            /* 3A info: Lux, color temp, exp */
        "00,00,00";                     /* agc, CT, and ISO */
#else
#define TEST_DATA_SIZE 11
UINT8 testData[TEST_DATA_SIZE] = "Test String";
#endif

UINT32 nOutputDataOffset[2] = {0, 0}; // [0]:single stream file 1 offsset, reserved [1]: dualstream file 2 offset

/*--------------------------------------------------------------------------*
* Function name    : appVideoSeamlessGPSFileDel
* Function         : When file is changed, will init user data feature.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appFileChangedCallback(void){
    appWriteUserDataInit();
}

/*--------------------------------------------------------------------------*
* Function name    : appWriteUserDataPeriod
* Function         : Write User Data by Period
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appWriteUserDataPeriod(void){

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	int i;
#endif

    switch(pUiSetting->mediaFormat){
        case MEDIA_FORMAT_MOV:
		case MEDIA_FORMAT_MP4:
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
if(nOutputDataOffset[0] == 0)
	return;


    i = snprintf((char *)testData, sizeof(testData),
            "%02d%02d%02d.000,%02d%02d%02d,%s%s,%s,",
            0, 0, VideoSec,
            0, 0,0,
            appInfoPositionSync(), appGsensorAccPXYZ(), appInfoEventSync());
    i += appInfo3AGet((char *)(&testData[i]), sizeof(testData) - i);
	
#if DEBUG_ON
    printf("udat %d bytes - %s\n", i, testData);
#endif
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_USER_DATA, (UINT32)testData,
                i, nOutputDataOffset[0],
                NULL, MEDIA_MOV_USER_DATA);
	
        nOutputDataOffset[0] += i;
	if(tempClear == 0 )
		tempClear++;
	else{
		tempClear = 0 ;
		appInfoPositionEventClean();
	}

	if(appInfoRtcChangeCheck()&&appInfoRTCChangedGet())
		appInfoRtcChangeSecSet(VideoSec);
	 VideoSec++;

        break;
#else
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_USER_DATA, testData, TEST_DATA_SIZE, nOutputDataOffset[0], NULL, MEDIA_MOV_USER_DATA);
            nOutputDataOffset[0] += TEST_DATA_SIZE;
            break;
#endif
        default:
            break;
    }
}

/*--------------------------------------------------------------------------*
* Function name    : initUserDataFileTag
* Function         : Init User Data File Tag
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static void initUserDataFileTag(void){
    //consider vif on, must add tag to video
    mediaVideoTags_t movVideoTag = {0};
    //check vfi is enabled or not
    UINT32 nPeriodSycnEnable = 0;
    sp5kMediaRecCfgGet(SP5K_MEDIA_REC_PERIODIC_SYNC_EN, &nPeriodSycnEnable);
    (1 == nPeriodSycnEnable) ? (movVideoTag.moov_first = TRUE) : (movVideoTag.moov_first = FALSE);
}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

/*--------------------------------------------------------------------------*
* Function name    : appUdataFirstLine
* Function         : Generate first line of udata that include summary event, Conpensation of G sensor, and Video mode.
* Return value	   : length of first line
* Parameter1	   : user data string (char* store first line data)
* Parameter1	   : user data length (int buffer size)
* History          : 2017.07.26 Roger, Huang  "Add function"
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static int appUdataFirstLine(char *buf, int len)
{
    return snprintf(buf, len, "%s,%03hhu,%03hhu,%03hhu,%03hhu,%08X\n", appInfoPoisitonEventGet(),
		    pUiSetting->GSensorComX, pUiSetting->GSensorComY, pUiSetting->GSensorComZ,
		    pUiSetting->videoSize, gRecStartTime);
}
#endif

/*--------------------------------------------------------------------------*
* Function name    : appFileCloseCallback
* Function         : When file close, will do this callback function.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appFileCloseCallback(void){
#if defined(PRJ_17DVR)
	char buf[32];
	int l; 
	l = appUdataFirstLine(buf, sizeof(buf));
	sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_USER_DATA, (UINT32)buf, l, NULL, NULL, MEDIA_MOV_USER_DATA);
	printf("%s(%d)\n", __func__, __LINE__);
#else
    memset(&nOutputDataOffset, 0, sizeof(nOutputDataOffset));
#endif
}

/*--------------------------------------------------------------------------*
* Function name    : appWriteUserDataInit
* Function         : Write User Data in tag feature init
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appWriteUserDataInit(void){
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
    char buf[32];
    int l;
  /*   int i;
   UINT32 event = 0;*/
#endif
    //init user data file tag
    initUserDataFileTag();

    //set file clsoe and file change callback function
    sp5kMediaRecCfgSet(SP5K_MEDIA_REC_FILE_CLOSE_CALLBACK, (UINT32)appFileCloseCallback);
    sp5kMediaRecCfgSet(SP5K_MEDIA_REC_FILE_CHANGED_CALLBACK, (UINT32)appFileChangedCallback);

    //set user data size
    //fixme , how to decide size
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	#define USER_DATA_SIZE 7700/*include first 12 bytes*/
    if(gRecStartTime == 0){
		tmx_t time;
		sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &time);
		gRecStartTime = appInfoSecTimeBaseGet(&time);
#if DEBUG_ON
		printf("%s(%d) init RTC %u\n", __func__, __LINE__, gRecStartTime);
#endif
    }else{
		gRecStartTime += SeamLessTime[pViewParam->videoSeamless];
#if DEBUG_ON
		printf("%s(%d) acc RTC %u, seamless %u\n", __func__, __LINE__, gRecStartTime, SeamLessTime[pViewParam->videoSeamless]);
#endif
    }
#else 
    #define USER_DATA_SIZE 10000
#endif
/*    UINT8 medformat = pUiSetting->mediaFormat;*/
    switch(pUiSetting->mediaFormat){
        case MEDIA_FORMAT_MOV:
	 case MEDIA_FORMAT_MP4:
#if (SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE)&&!defined(PRJ_19DVR)
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_USER_DATA_SIZE, USER_DATA_SIZE, DualFile1, MEDIA_MOV_USER_DATA);
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_USER_DATA_SIZE, USER_DATA_SIZE, DualFile2, MEDIA_MOV_USER_DATA);
#else
            //single stream
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_USER_DATA_SIZE, USER_DATA_SIZE, NULL, MEDIA_MOV_USER_DATA);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		 VideoSec = 0;
		 memset(firstLineBuffer,0x00 , sizeof(firstLineBuffer));
	 	 memset(&nOutputDataOffset, 0, sizeof(nOutputDataOffset));
		l = appUdataFirstLine(buf, sizeof(buf));
		strcpy(firstLineBuffer , buf);
#if DEBUG_ON
		printf("%s(%d) (%s) %d %s-%s-%s\n", __func__, __LINE__, buf, l, buf[l-2],buf[l-1], buf[l]);
		printf("%s(%d) (%s) %s-%s-%s\n", __func__, __LINE__, firstLineBuffer, firstLineBuffer[l-2],firstLineBuffer[l-1], firstLineBuffer[l]);
#endif
		sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_USER_DATA, (UINT32)buf, l, nOutputDataOffset[0], NULL, MEDIA_MOV_USER_DATA);//To do the one time infomation
		nOutputDataOffset[0] = l;
		appInfoUdatHeaderPosEventClean();
#endif
#endif
            break;
        case MEDIA_FORMAT_AVI:
#if (SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE)&&!defined(PRJ_19DVR)
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_USER_DATA_SIZE, USER_DATA_SIZE, DualFile1, MEDIA_AVI_CHUNK_STRD);
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_USER_DATA_SIZE, USER_DATA_SIZE, DualFile2, MEDIA_AVI_CHUNK_STRD);
#else
            sp5kMediaRecAttrSet(SP5K_MEDIA_ATTR_USER_DATA_SIZE, USER_DATA_SIZE, NULL, MEDIA_AVI_CHUNK_STRD);
#endif
            break;
        default:
            break;
    }
}
#endif

/*--------------------------------------------------------------------------*
* Function name    : appVideoLogTagInitial
* Function         : add video tag
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoLogTagInitial(void){
#define VLC_FOURCC( a, b, c, d ) \
		 ( ((UINT32)a) | ( ((UINT32)b) << 8 ) \
			| ( ((UINT32)c) << 16 ) | ( ((UINT32)d) << 24 ) )
	 UINT32 default_compatible_brands[2] = {VLC_FOURCC(' ', ' ', 't', 'q'), 0};
	 mediaVideoTags_t movVideoTag = {0};

	 movVideoTag.major_brand = VLC_FOURCC(' ', ' ', 't', 'q');
	 movVideoTag.minor_version = 0;
	 movVideoTag.compatible_brands = (UINT32*)&default_compatible_brands;
	 movVideoTag.compatible_brands_cnt = 2;
	 movVideoTag.udta_atom.name = VLC_FOURCC('u', 'd', 'a', 't');
	 movVideoTag.udta_atom.update_mode = 1;
	 movVideoTag.udta_atom.location = 1;
	 movVideoTag.udta_atom.nDataSize = 0;

	 movVideoTag.pref_volume = 0x100;
	 movVideoTag.vendor = VLC_FOURCC('t', 'a', 'c', 'i');
	 movVideoTag.component_manuf = VLC_FOURCC('t', 'a', 'c', 'i');;
	 movVideoTag.data_ref_type = VLC_FOURCC('a', 'l', 'i', 's');
	 movVideoTag.tkhd_flags = 0xf;

	 movVideoTag.time_scale = 90000;
	 movVideoTag.video_track_matrix[0] = 0x10000;
	 movVideoTag.video_track_matrix[4] = 0x10000;
	 movVideoTag.video_track_matrix[8] = 0x40000000;
	 movVideoTag.temporal_quality = 0x200;
	 movVideoTag.spatial_quality = 0x200;
	 movVideoTag.video_sample_per_chunk = 1;
	 sprintf((char *)movVideoTag.video_handler_comp_name, "iCatchTek Video Handler");
	 sprintf((char *)movVideoTag.video_handler_ref_comp_name, "iCatchTek Alias Data Handler");

	 movVideoTag.audio_compression_id = -2;
	 sprintf((char *)movVideoTag.audio_handler_comp_name, "iCatchTek Audio Handler");
	 sprintf((char *)movVideoTag.audio_handler_ref_comp_name, "iCatchTek Alias Data Handler");
	 sp5kMediaRecAttrSet( SP5K_MEDIA_ATTR_REC_TAGS, (UINT32)&movVideoTag);
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecLogFileAdd
* Function         : save video record log in file
* Return value	   : void
* Parameter1	   : message (UINT32 your log)
* Parameter2	   : parameter (UINT32)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appVideoRecLogFileAdd(UINT32 msgadd ,UINT32 param){
	char buf[80];
	tmx_t time;
	UINT8 fname[32];
	UINT32 witrefilesize=80;
	static UINT16 fileCount =1 ;
	memset(buf, 0, 80);
	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &time);
	sprintf(buf,"msg=%x  param=%x time %04d:%02d:%02d:%02d:%02d:%02d",msgadd ,param ,
						time.tmx_year+1900,
						time.tmx_mon,
						time.tmx_mday,
						time.tmx_hour,
						time.tmx_min,
						time.tmx_sec
	);
	sprintf((char *)fname, "D:\\dropblock_%d.TXT", fileCount);
	fileCount ++;
	WriteFile(fname , (UINT8*)buf,witrefilesize);
}

/*--------------------------------------------------------------------------*
* Function name    : DoVideoPauseControl
* Function         : Control Video Pause/Resume
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
DoVideoPauseControl(void)
{
	UINT32 elsTime;

	if (VideoRecStatus == VIDEO_REC_STATUS_RECORDING )
	{
		if (gVideoCB.CapSec<2)
		{
			VideoRecPaReMsgReceive =TRUE;
			return;
		}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if(IS_CARD_EXIST)
		{
		
			LED_NORMAL_ON;

		}
		else
		{
			GREEN_LED_OFF;
			RED_LED_OFF;
		}
#else
			LED_NORMAL_ON;
#endif


        sp5kTimerEnable(gVideoCB.CapTimer, DISABLE);
		gVideoCB.fPause = 1;
		VideoRecStatus = VIDEO_REC_STATUS_PAUSE;
		sp5kMediaRecControl(SP5K_MEDIA_REC_PAUSE);
		DBG_PRINT("vrec: Video rec msg: SP5K_MEDIA_REC_PAUSE\n");
		appViewOsd_VideoRecControlDraw(VIDEO_REC_RECORDING);

	}
	else
	{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		LED_NORMAL_ON;
#else
		LED_FLICKER_VIDEO_RECORDING;
#endif
       	sp5kTimerCfgSet(gVideoCB.CapTimer, 500/*ms*/);
    	sp5kTimerEnable(gVideoCB.CapTimer, ENABLE);
		gVideoCB.fPause = 0;
		VideoRecStatus = VIDEO_REC_STATUS_RECORDING;
		sp5kMediaRecControl(SP5K_MEDIA_REC_RESUME);
		DBG_PRINT("vrec: Video rec msg: SP5K_MEDIA_REC_RESUME\n");
		appViewOsd_VideoRecControlDraw(VIDEO_REC_PAUSE);

		// show the real recording time.
		if(pViewParam->videoStamp){

		}else{
			sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);
			gVideoCB.CapSec = (elsTime+500)/1000;/*second*/
			appVideoOsdElsTimeShow();
		}
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoStop
* Function         : let video stop
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoStop(void)
{

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	/* UINT32 err = 0; */
	UINT32 state;
	printf("vrec: video stop\n");
	callStack();
	state = appActiveStateGet();
	if (state != APP_STATE_VIDEO_REC && state != APP_STATE_VIDEO_AAA)
		return;
#else
	static UINT32 stopping = 0;

	if ((stopping++) == 0)
#endif
	{
		DBG_PRINT("vrec: video stop\n");
		gStillCB.cap.CurViewMode = VIDEO_MODE;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		sp5kAeModeSet(SP5K_AE_MODE_OFF);
		sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
#endif
		VideoRecStatus = VIDEO_REC_STATUS_CLOSE;
		appViewOsd_VideoRecIconDraw(FALSE);
		sp5kTimerEnable(gVideoCB.CapTimer, DISABLE);

	    /* DIST + RSC OFF */
		sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DIS_LEVEL,MEDIA_REC_RSC_DIS_OFF);

	    #ifdef VIDEO_REC_STAMP
	    #if GPS_DEMO //for gps demo
		appVideoSeamlessGPSFileDel();
		App_videoGPSDataSave();
	    #endif

		UINT32 left_x0=30, top_y0=30, right_x1=320, bottom_y1=60;
		appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0, left_x0, top_y0, right_x1, bottom_y1);
		left_x0=0; top_y0=30; right_x1=320; bottom_y1=240;
		appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_0, left_x0, top_y0, right_x1, bottom_y1);
		appVideoRecStampExit();
	    #endif

	    #if GPS_OPEN
		if(pUiSetting->mediaFormat == MEDIA_FORMAT_MOV){
	    	appUsrDataWriteEnd();
	    }
	    #endif

		appModeSet(SP5K_MODE_VIDEO_PREVIEW);

	    #if SP5K_DIQ_HDR
	    if (HDR_En)
	    {
	        #if SP5K_SENSOR_DUAL_MODE
	            if (appSingleModeCheck() == APP_VIDEO_MODE_SINGLE)
	        #endif
	        appVideoHDRModeSetting(DISABLE);
	    }
	    #endif

	    #if SP5K_ICAT_EIS
		    if (pViewParam->VideoEISMode != UI_EIS_OFF)
			{
			    if (pViewParam->videoSize == UI_VIDEO_SIZE_FHD_30FPS)
			        appVideo_SetRscEis(DISABLE, DISABLE, SP5K_EIS_ALGO_COMP_TYPE_IME);
			    else
			        appVideo_SetRscEis(DISABLE, DISABLE, SP5K_EIS_ALGO_COMP_TYPE_DIST);
		    }
	    #endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
	sp5kAwbModeSet(SP5K_AWB_MODE_INFO_ONLY);
#endif
	    #if SP5K_CDFS_OPT
		//if(appCdfsAddFileStsGet())//paul modify 2018 0323
		if(pUiSetting->mediaFormat == MEDIA_FORMAT_AVI&&appCdfsAddFileStsGet())
	    {
	    	appCdfsFileSync(appCdfsLastAviNameGet(), DISABLE);
	    }
	    #endif

		UINT32 factor_x=1000, factor_y=1000;
		sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, factor_x, factor_y, DISABLE, DISABLE);
	    /* set font id */
	    if (pViewParam->videoStamp) {
	        appOsd_GfxFontIDSet(SP5K_GFX_FONT_ID_1);
	    }
	}
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
	stopping--;
#endif
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStateInit
* Function         : Video record state management init
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*				   : 2018.06.07  Weikai Hsu  "Modify for cppcheck"
*--------------------------------------------------------------------------*/

void
appVideoRecStateInit(void)
{
//	UINT32 mode=0, videorec_roix, videorec_roiy;
	UINT32 mode=0;
	#if SP5K_CDFS_OPT
	appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
	appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
	#endif

	sp5kTimerCfgSet(gVideoCB.CapTimer, 500/*ms*/);
	sp5kTimerEnable(gVideoCB.CapTimer, ENABLE);

	appPreloadSoundAbort();
#if defined(HW_PDVR)
	if(IN_GSENSOR_DETECT)
		sp5kHostMsgSend(APP_KEY_EVENT_FINISH,NULL);
#endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(IS_CARD_EXIST &&(appInfoPowerDETStateGet()==POWER_DET_ACC_ON ||appInfoPowerDETStateGet()==POWER_DET_PARKING_WAKEUP))
	{
		LED_NORMAL_ON;
	}

#else
	LED_FLICKER_VIDEO_RECORDING;
#endif

	#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    appVideoSizeSet(); /*because appVideoPvStateClose disable dual stream*/
	appVideoDualStreamStart();
    #if SP5K_USB_UVC_SUPPORT
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
    APP_OSD_REFRESH_ON;
    #endif
	#endif
	#ifdef VIDEO_REC_STAMP
	//weikai modify for cppcheck 180607
	UINT32 videorec_roix, videorec_roiy;
	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(pViewParam->videoSize == UI_VIDEO_SIZE_FHD_30FPS||pViewParam->videoSize == UI_VIDEO_SIZE_FHD_PARK)
	{
		videorec_roix=(1920/10)*3; videorec_roiy=1030;
		appVideoRecStampInit(mode, videorec_roix, videorec_roiy);
	}
	else
	{
		videorec_roix=(1280/10)*2; videorec_roiy=680;
		appVideoRecStampInit(mode, videorec_roix, videorec_roiy);
	}
	#else
		videorec_roix=8; videorec_roiy=4;
		appVideoRecStampInit(mode, videorec_roix, videorec_roiy);
	#endif
		APP_OSD_REFRESH_OFF;
		CLEAR_OSD_SRC;
		appViewOsd_VideoRecIconDraw(TRUE);
		APP_OSD_REFRESH_ON;
	#endif
	#if SP5K_ICAT_EIS
        #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    	appVideo_SetRscEis(ENABLE, ENABLE, SP5K_EIS_ALGO_COMP_TYPE_DIST);
        #else
        if (pViewParam->VideoEISMode != UI_EIS_OFF)
    	{
            if(pViewParam->videoSize == UI_VIDEO_SIZE_4K2K||pViewParam->videoSize == UI_VIDEO_SIZE_27K)
            	appVideo_SetRscEis(DISABLE, ENABLE, SP5K_EIS_ALGO_COMP_TYPE_DIST);
            else if((pViewParam->videoSize == UI_VIDEO_SIZE_FHD_30FPS))
        		appVideo_SetRscEis(ENABLE, ENABLE, SP5K_EIS_ALGO_COMP_TYPE_IME);
            else
            	appVideo_SetRscEis(ENABLE, ENABLE, SP5K_EIS_ALGO_COMP_TYPE_DIST);
    	}
        #endif
	#endif
    #if SP5K_DIQ_HDR
	/////MUST set HDR behind dram increase to 450MHz.
    if (HDR_En)
    {
        #if SP5K_SENSOR_DUAL_MODE
            if (appSingleModeCheck() == APP_VIDEO_MODE_SINGLE)
        #endif
        appVideoHDRModeSetting(HDR_En);
    }
    #endif

	sp5kModeGet(&mode);
	if (mode != SP5K_MODE_VIDEO_PREVIEW) {
		sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
		sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
	}

	appVideoFifoSet();

  /* To add for enable Y-Remap */
	sp5kSystemCfgSet(SP5K_CDSP_SYS_CFG,CDSP_SYS_CFG_DIGI_EFF_Y_REMAP,1);
	sp5kModeSet(SP5K_MODE_VIDEO_RECORD);
	//paul modify 2018 0330(add lost func to this merger code)
	sp5kModeWait(SP5K_MODE_VIDEO_RECORD);/*Isaac try to fixed udat loss the first data 20170815*/

	appAePreviewRestore();

	gVideoCB.fCapture=1;
	gVideoCB.fPause = 0;
	gVideoCB.CapSec = 0;
	gVideoCB.fStop = 0;
	VideoRecStatus = VIDEO_REC_STATUS_RECORDING;

	if (pViewParam->videoStamp == UI_DATESTAMP_OFF) {
		appOsd_GfxFontIDSet(SP5K_GFX_FONT_ID_1);
		appViewOsd_VideoRecControlDraw(VIDEO_REC_PAUSE);
	}
	else{
		//appVideoOsdElsTimeShow();
	}
	appPowerSavingEnableSet(FALSE);

	gVideoCB.isAutoPowerOffOn=0;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	FirstRecording =TRUE;
#endif
    #if SP5K_USB_UVC_SUPPORT
    appViewOsd_DualModeDraw();
    #endif
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStateOnMsg
* Function         : Video record state management for message use.
* Return value	   : void
* Parameter1	   : message (UINT32 app_state_ctrl.h/sysg_msg_def.h)
* Parameter2	   : parameter (UINT32)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoRecStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 elsTime;

	extern UINT32 appAeInfoExpidxGet();
	extern UINT32 appAeInfoAgcidxGet();

	switch ( msg) {
	case SP5K_MSG_MODE_READY:
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		printf("%s SP5K_MSG_MODE_READY param 0x%x , appInfoPowerDETStateGet() 0x%x appActiveStateGet 0x%x\n" , __FUNCTION__,param ,appInfoPowerDETStateGet(),appActiveStateGet() );
#endif
		if((param==SP5K_MODE_VIDEO_PREVIEW) && (gVideoCB.err==VIEW_ERR_MEMORY_FULL))
		{
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vrec: %s %d rec fail",__FUNCTION__, __LINE__);
            		sp5kTimerEnable(gVideoCB.CapTimer, DISABLE);
			VideoRecStatus = VIDEO_REC_STATUS_CLOSE;
			gVideoCB.err = VIEW_ERR_MEMORY_FULL;
			appVideoOsdShow(DISABLE,DISABLE,gVideoCB.osd);
            		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
		}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		else if(appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP&&param == 0x21){
				//cover this case , when parking recording ,then acc on need too change seamless time and resolution 
				appModeWaitCfg(TRUE);

				/* BEEP is too short before power off*/
				if (IS_CARD_EXIST == FALSE)
					APP_SLEEP_MS(4000);
				appPowerOffUiMsgSend(APP_POWER_OFF_USB_PLUG); 

		}
#endif
		else
        {
#if GPS_OPEN
            if(pUiSetting->mediaFormat == MEDIA_FORMAT_MOV)
            {
                appUsrDataWrite();
            }
#endif
            DBG_PRINT("vrec: mode ready: 0x%x\n", param);
        }
		break;

	case SP5K_MSG_MEDIA_REC_START:
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "vrec: _OnMsg:SP5K_MSG_MEDIA_REC_START:0x%x",param);
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY)
		{
			HOST_PROF_LOG_ADD(LEVEL_INFO, "vrec: rec start");

            #ifdef VIDEO_REC_STAMP
            #if (SP5K_VREC_YUV_SAVE) /* Jerry add for YUV save */
            appVideoYuvSaveStart();
            #endif
            #endif
		}
		else
		{
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vrec: %s %d rec start fail",__FUNCTION__, __LINE__);
            sp5kTimerEnable(gVideoCB.CapTimer, DISABLE);
			appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			VideoRecStatus = VIDEO_REC_STATUS_CLOSE;
			gVideoCB.err = VIEW_ERR_TO_POWER_OFF;
		}
		break;

	case SP5K_MSG_MEDIA_REC_STOP:
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "vrec: _OnMsg:SP5K_MSG_MEDIA_REC_STOP:0x%x",param);
		if (param==SP5K_MSG_MEDIA_PARAM_OKAY)
		{
			if (VideoRecStatus == VIDEO_REC_STATUS_CLOSE )
			{
				if(pViewParam->videoStamp)
				{
					appOsd_GfxFontIDSet(SP5K_GFX_FONT_ID_1);
				}
				sp5kTimerEnable(gVideoCB.CapTimer, DISABLE);
                #ifdef VIDEO_REC_STAMP
				appVideoRecStampCbUnReg();
                #endif

				if(gVideoCB.err == VIEW_ERR_TO_POWER_OFF)
				{
					HOST_PROF_LOG_PRINT(LEVEL_ERROR, "vrec: error to power-off");
					appPowerOffUiMsgSend(APP_POWER_OFF_VIDEO_REC_ERROR);
				}
				else
				{
					if(appNextStateGet() !=APP_STATE_NULL)
					{
                        sp5kTimerEnable(gVideoCB.CapTimer, DISABLE);
						appStateCloseDone();
					}
					else
					{
                        HOST_PROF_LOG_PRINT(LEVEL_INFO, "vrec: rec change mode ok %d", __LINE__);
						appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
						#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
						appVideoDualStreamStop();
						#endif
					}
				}
			}
			HOST_PROF_LOG_ADD(LEVEL_INFO, "vrec: rec stop ok");

            #ifdef VIDEO_REC_STAMP
            #if (SP5K_VREC_YUV_SAVE) /* Jerry add for YUV save */
            appVideoYuvSaveStop();
			#endif
			#endif
		}
		else
		{
			/*-- There wont have this msg out of middleware.
			 * Instead, middleware will send SP5K_MSG_DISK_ERROR.
			 */

			/*rec and plug card error handle*/
			if(!IS_CARD_EXIST && appActiveDiskGet() == SP5K_DRIVE_SD)
			{
				HOST_PROF_LOG_ADD(LEVEL_WARN, "vrec: _OnMsg: stop -rec and plug out card");
				appDiskMount(SP5K_DRIVE_NAND);
				sp5kHostMsgSend(SP5K_MSG_MEDIA_REC_STOP, SP5K_MSG_MEDIA_PARAM_OKAY);
			}
			else
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "vrec: %s %d rec stop fail",__FUNCTION__, __LINE__);
		}
		#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		jetSDUseTimeCheck();
		#endif
		break;

	/* view handling */
	case SP5K_MSG_AE_READY:
	#if SP5K_MOTION_DETECT
		if(appUiParaGet()->MotionDetect == TRUE)
		{
	        appMotionDetectHdr(NULL);
		}
	#endif
		break;

	case SP5K_MSG_MEDIA_SEAMLESS_CHANGE_FILE:
		gVideoCB.CapSec=0;
		sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &chgFileTime);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		jetSDUseTimeCheck();
		queryAvailableSpace_t setting;
		UINT8 status;
		status = jetQueryAvailableSpaceGet(&setting);
		appInfoQueryAvailableSpaceFB(status, setting);
#endif
		break;

	//case SP5K_MSG_MEDIA_SEMALESS_CNT_NOTIFY :
	case SP5K_MSG_TIMER:
		#if SP5K_MOTION_DETECT
		if(param == mdTime)
		{
			if(puiPara->Seamless == UI_VIDEO_SEAMLESS_OFF)
			{
				if(mdRecCount)
				{
					mdRecCount--;
				}
				else
				{
					if(mdTime != TIMER_NULL)
					{
						appTimerClear(&mdTime);
					}
					__FUNC_TRACK__;
					appVideoStop();
				}
			}
			else
			{
				if((elsTime + 300)/1000 == appVideoSeamLessSecGet(appActiveStateGet()))
				{
					if(mdStop)
					{
						if(mdTime != TIMER_NULL)
						{
							appTimerClear(&mdTime);
						}
						__FUNC_TRACK__;
						appVideoStop();
					}
				}
			}
		}
		#endif

        if (gVideoCB.CapTimer==TIMER_NULL)
        {
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "vrec: %s %d rec timer fail",__FUNCTION__, __LINE__);
        }

        if (param == gVideoCB.CapTimer)
        {
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
            if(appDualPipModeGet()!=DUALSEN_SINGLE_SEC)
                sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);
            else
                sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_2ND_ELAPSED_TIME, &elsTime);
#else
            sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);/*ms*/
#endif
            elsTime = (elsTime+500)/1000;/*second*/

            /*for loading log*/
#if (CPU_LOADING_LOG || DRAM_BANDWIDTH_LOG)
            appVideoLoadLog(elsTime);
#endif

            if((gVideoCB.CapSec != elsTime) || (elsTime <= 2))
            {
#ifdef VIDEO_REC_STAMP
                appVideoRecStampUpdate();

#if GPS_DEMO //for gps demo
                if (pViewParam->videoStamp && elsTime%15 ==0)
                    appVideoSeamlessGPSFileDel();
#endif
#endif

                gVideoCB.CapSec = elsTime;
                if(pViewParam->videoStamp == UI_DATESTAMP_OFF)
                {
                    appOsd_GfxFontIDSet(SP5K_GFX_FONT_ID_1);
                        appVideoOsdElsTimeShow();
                }

                if(pUiSetting->mediaFormat == MEDIA_FORMAT_MOV||pUiSetting->mediaFormat == MEDIA_FORMAT_MP4){
#if WRITE_USER_DATA
                    appWriteUserDataPeriod();

#elif GPS_OPEN
                    appUsrDataWrite();
#endif
                }

                if(gVideoCB.CapSec%2==1)
                        appViewOsd_VideoRecIconDraw(TRUE);
                else
                        appViewOsd_VideoRecIconDraw(FALSE);
            }

            if(appVideoRemainSecGet()<1 && !pViewParam->videoSeamless)
            {
                appVideoStop();
            }

            // avoid the pb key interrupt video record less than one second.
            if(gVideoCB.CapSec == 2)
            {
                appBtnEnable(BTN_ALL);
                //appBtnEnable(BTN_PB);
            }
            /*DBG_PRINT("vrec: TIMER: elsTime=%d, CapSec=%d (dif=%d)\n", elsTime,gVideoCB.CapSec,gVideoCB.CapSec*1000 - elsTime);*/

            // this is for s1-press before record time greater than one.
            if(gVideoCB.fStop == 1 && gVideoCB.CapSec>1	&& !pViewParam->videoSeamless )
            {
                HOST_PROF_LOG_ADD(LEVEL_INFO, "vrec: Seamless recording stop");
                appVideoStop();
            }
        }

		#if SPCA6330
		if (param == capvidtimer) /*capture image in video*/
		{
			appOsd_GfxFontIDSet(SP5K_GFX_FONT_ID_1);
			appErrMsgTimeISRWithSound(ERR_MSG_CAPTURE_DONE,ERR_MSG_ONLY, 1000);
			appTimerClear(&capvidtimer);
			capvidimg =FALSE;

			if (pViewParam->videoStamp != UI_DATESTAMP_OFF) {
				appOsd_GfxFontIDSet(SP5K_GFX_STAMP);
			}
		}
		#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if(param == jetSmartAssitModeTimerID()){//SCC
			jetSmartAssitModeTimer();
		}
#endif
		break;

	/* media rec */
	case SP5K_MSG_MEDIA_ELAPSED_TIME_UPDATE:
		DBG_PRINT("vrec: _OnMsg: MEDIA_ELAPSED_TIME_UPDATE\n");
		break;

	case SP5K_MSG_MEDIA_REC_DROP_FRAME:
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "vrec: MEDIA_REC_DROP_FRAME param=%x",param & 0xf0000000);
		#if WRITE_FILE_DEBUG	/* Write file for debug, but worse condition add file would let FAT more worse */
    		appVideoRecLogFileAdd(msg ,param);
        #endif
		//Ethan if drop frame stop recording (H624)
		if(VideoRecStatus == VIDEO_REC_STATUS_CLOSE)
			return;

		appBackgroundFuncExec(appVideoStop, 0, bk_default_priority, bk_default_blocking);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		extern UINT8 dropFrameRecFlag;
		dropFrameRecFlag = 1;
#endif
		#if DUMP_ALL_DEBUG	/* dump log for debug. */
		    profLogDumpAll();
		#endif
		break;

	case SP5K_MSG_MEDIA_REC_PAUSE_FINISH:
	#if SPCA6330
	case SP5K_MSG_MEDIA_REC_RESUME_FINISH:
	#endif
		VideoRecPaReMsgReceive =TRUE;
		break;

	case SP5K_MSG_MEDIA_FILE_SIZE_NOTIFY:
		if (param == VIDEO_SIZE_REACH_4G) /*for FAT32 system*/
			appBackgroundFuncExec(appVideoStop, 0, bk_default_priority, bk_default_blocking);
		DBG_PRINT("vrec: _OnMsg: SP5K_MSG_MEDIA_FILE_SIZE_NOTIFY\n");
		break;

	case SP5K_MSG_MEDIA_AVI_REC_FPS_LOW://happen with slow card and enough space(>2G)
		break;

	case SP5K_MSG_MEDIA_REC_DISK_FULL:
		HOST_PROF_LOG_ADD(LEVEL_WARN, "vrec: _OnMsg: SP5K_MSG_MEDIA_REC_DISK_FULL");
        //SHEN:[ICOM-4012] seamless mode maybe cannot delete old file, for simply flow, remove this  or sometime always keep in record mode.
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if (pViewParam->videoSeamless) {
			break;
		}
#endif
	case SP5K_MSG_DISK_ERROR:
		/*rec and plug card error handle*/
		if(!IS_CARD_EXIST && appActiveDiskGet() == SP5K_DRIVE_SD)
		{
			HOST_PROF_LOG_ADD(LEVEL_WARN, "vrec: _OnMsg: diskerr -rec and plug out card");
		}
		else
		{
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "vrec: _OnMsg: SP5K_MSG_DISK_ERROR param=%x", param);
			appBackgroundFuncExec(appVideoStop, 0, bk_default_priority, bk_default_blocking);
			gVideoCB.err = VIEW_ERR_MEMORY_FULL;
			appVideoOsdShow(DISABLE,DISABLE,gVideoCB.osd);
		}
		break;

	case APP_STATE_MSG_ABORT:
		HOST_PROF_LOG_ADD(LEVEL_WARN, "vrec: _OnMsg: APP_STATE_MSG_ABORT");
		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
		break;

	/* TELE/WIDE Self Repeat */
	case APP_UI_MSG_LENS_DZOOM_TELE:
		if ( IN_BUTTON_TELE )
		{
			appVideoDZoomTele();
		}
		break;

	case APP_UI_MSG_LENS_DZOOM_WIDE:
		if ( IN_BUTTON_WIDE )
		{
			appVideoDZoomWide();
		}
		break;
    #if SP5K_SENSOR_DUAL_MODE
        case APP_UI_MSG_WAIT_MSG_YUV_PLUG_OUT:
            printf("APP_UI_MSG_WAIT_MSG_YUV_PLUG_OUT: Rear sensor plug out\n");
            appVideoRecStampUpdate();
            break;
    #endif
	#if SP5K_MOTION_DETECT
	case APP_UI_MSG_MD_START:
		mdStop = 0;
		break;
	case APP_UI_MSG_MD_STOP:
		mdStop = 1;
		break;
	#endif
	default:
		break;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecStateOnKey
* Function         : Video record state management for key use.
* Return value	   : void
* Parameter1	   : message (UINT32 app_state_ctrl.h/sysg_msg_def.h)
* Parameter2	   : parameter (UINT32)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoRecStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	extern BOOL bPIVEnd;
	UINT32 sp5kmode;

	switch (msg) {
	case APP_KEY_PRESS_UP:
#if SP5K_USB_UVC_SUPPORT
        appUvcEventSet(EVENT_TRIG_PROC);
#endif
		break;

	case APP_KEY_PRESS_S1:
		break;
	case APP_KEY_PRESS_S2:
		if (VideoRecStatus == VIDEO_REC_STATUS_RECORDING) {
			sp5kModeGet(&sp5kmode);
			if (sp5kmode != SP5K_MODE_VIDEO_RECORD)
				sp5kModeWait(SP5K_MODE_VIDEO_RECORD);
		}
		if (VideoRecStatus != VIDEO_REC_STATUS_CLOSE) {
			#if GPS_OPEN
			if(pUiSetting->mediaFormat == MEDIA_FORMAT_MOV){
				appUsrDataWriteEnd();
			}
			#endif
			appBackgroundFuncExec(appVideoStop, 0, bk_default_priority, bk_default_blocking);
		}
		break;

	case APP_KEY_PRESS_MODE:
#if SP5K_USB_UVC_SUPPORT
		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
#else
		/*appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);*/
#endif
		break;

	case APP_KEY_PRESS_PB:
		/*appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);*/
		break;

	case APP_KEY_PRESS_RIGHT:

		if (VideoRecPaReMsgReceive ==FALSE){
			break;
		}else{
			VideoRecPaReMsgReceive =FALSE;
		}

		if (VideoRecStatus==VIDEO_REC_STATUS_RECORDING
			||VideoRecStatus==VIDEO_REC_STATUS_PAUSE)
		{
			DoVideoPauseControl();
		}
		break;

	case APP_KEY_PRESS_TELE:
		if (pViewParam->videoStamp == UI_DATESTAMP_OFF)
			appVideoZoomTele();
		break;

	case APP_KEY_PRESS_WIDE:
		if (pViewParam->videoStamp == UI_DATESTAMP_OFF)
			appVideoZoomWide();
		break;

	case APP_KEY_RELEASE_WIDE:
	case APP_KEY_RELEASE_TELE:
		appZoomStop();
		break;

	#if SPCA6330
	case APP_KEY_PRESS_MENU:
        #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
            break;
        #else
        //ICOM-3962, For memory not enough in V35M(1G RAM), remove 4K2K/2.7K + PIV function.
        if(pViewParam->videoSize == UI_VIDEO_SIZE_4K2K||pViewParam->videoSize == UI_VIDEO_SIZE_27K)
            break;
        #endif

		#if SPCA6330
		/*capture image in video*/
		#if SP5K_CDFS_OPT
		if(bPIVEnd && appCdfsFileNameValidCheck(SP5K_DCF_FILETYPE_JPG) == SUCCESS)
		{
			appCdfsEventFlagSet(JPG_EVENT_IN_VIDEO);
		#else
		{
		#endif
		bPIVEnd = FALSE;
		appPIVTrig();
		}
		#endif
		break;
	#endif

	/* Jerry add for YUV save */
    #ifdef VIDEO_REC_STAMP
	#if (SP5K_VREC_YUV_SAVE)
	case APP_KEY_RELEASE_DOWN:
		appVideoYuvFlagSet(TRUE);
		break;
	#endif
    #endif

	default:
		break;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appVideoRecState
* Function         : Video record state management in common use.
* Return value	   : void
* Parameter1	   : message (UINT32 app_state_ctrl.h/sysg_msg_def.h)
* Parameter2	   : parameter (UINT32)
* History          :
*                  : 2018.05.29  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appVideoRecState(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 mode;

	switch (msg)
	{
	case APP_STATE_MSG_INIT:
		RPrintf("appVideoRecState");

#if SUPPORT_RUNTIME_SWITCH_DRAM_CLK
		appSysClkSwitch(1); //sel?RUNTIME_SWITCH_DRAM_CLK_MAX_KHZ:RUNTIME_SWITCH_DRAM_CLK_DEF_KHZ
#endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		/*CLean lastNode for mode change */
		lastNode = NULL;

		/* @Allen added for enable Y-Remap */
		sp5kSystemCfgSet(SP5K_CDSP_SYS_CFG,CDSP_SYS_CFG_DIGI_EFF_Y_REMAP,DISABLE);
#endif


#if WRITE_USER_DATA
        if(pUiSetting->mediaFormat == MEDIA_FORMAT_MOV||pUiSetting->mediaFormat == MEDIA_FORMAT_MP4){
		
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		    gRecStartTime = 0;
#endif
            appWriteUserDataInit();
        }
#elif GPS_OPEN
        if(pUiSetting->mediaFormat == MEDIA_FORMAT_MOV)
        {
            appGpathSave();
            appVideoLogTagInitial();
        }
#endif

        #if SP5K_USB_UVC_SUPPORT
        #else
        appPIVTaskInit();
		appUrgentSet( URGENT_PIV, ENABLE);
        #endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		appInfoUdatHeaderPEClean();
#endif
		appVideoRecStateInit();
		break;
	case APP_STATE_MSG_CLOSE:
		/* @Allen added for disable Y-Remap */
		sp5kSystemCfgSet(SP5K_CDSP_SYS_CFG,CDSP_SYS_CFG_DIGI_EFF_Y_REMAP,DISABLE);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		/*paul 2018.04.18 modify Bug #3424*/
		if(IS_CARD_EXIST &&(appInfoPowerDETStateGet()==POWER_DET_ACC_ON ||appInfoPowerDETStateGet()==POWER_DET_PARKING_WAKEUP))
		{
			LED_NORMAL_ON;
		}
#endif
		printf("%s : close  param 0x%x\n",__FUNCTION__,param);
		sp5kModeGet(&mode);
		if(mode==SP5K_MODE_VIDEO_RECORD)
		{
			appVideoStop();
		}

		#if !SP5K_USB_UVC_SUPPORT
		appUrgentSet( URGENT_PIV, DISABLE);
		#endif

		VideoRecStatus = VIDEO_REC_STATUS_CLOSE;
		appStateCloseDone();
#if SUPPORT_RUNTIME_SWITCH_DRAM_CLK
		appSysClkSwitch(0); //sel?RUNTIME_SWITCH_DRAM_CLK_MAX_KHZ:RUNTIME_SWITCH_DRAM_CLK_DEF_KHZ
#endif
		break;
	case APP_STATE_MSG_ABORT:
		HOST_PROF_LOG_ADD(LEVEL_WARN, "vrec: rec state APP_STATE_MSG_ABORT");

		/*rec and plug card error handle*/
		appVideoStop();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appVideoRecStateOnKey(msg,param);
		}
		else
		{
			appVideoRecStateOnMsg(msg,param);
		}
		break;
	}
}

