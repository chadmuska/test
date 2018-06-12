/*--------------------------------------------------------------------------*
 *	File name	: App_infohandler_func.c
 *	Function	: infohandler processed
 *	First editor: Weikai Hsu
 *	First date	: 	
 *	History		: 
 *	            : 2018.04.11  Weikai Hsu "Added File Header"
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api/sp5k_global_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_dbg_api.h"
#include "middleware/sysg_msg_def.h"
#include "sp5k_modesw_api.h"
#include "app_icon_def.h"
#include "app_res_def.h"
#include "app_sensor.h"
#include "app_icon_def.h"
#include "sp5k_media_api.h"
#include "app_view_param_def.h"
#include "app_video.h"
#include "sp5k_sensor_api.h"
#include "app_error_msg.h"
#include "app_playback_def.h"
#include "app_com_api.h"
#include "app_rtc.h"
#include "app_infohandlerComm.h"
#include "app_infohandler.h"
#include "app_aealg_api.h"
#include "app_awbalg_api.h"
#include "sp5k_dq_api.h"
#include "app_uart2.h"
#include "app_view_param.h"
#include "app_cdfs_api.h"
#include "jet_cdfs.h"
#include "app_infohandler_func.h"

#define HWVER_LV_1S 16000
#define HWVER_LV_TOLERANCE 1000
#define APP_BPLUS_EVENT_THREAD_PRIORITY 15

extern UINT32 PlayPos_TimerCount ;
extern UINT32 PosReq_TimerCount ;
extern UINT32 ConnectDev_TimerCount  ;
extern UINT32 Regular_TimerCount  ;
extern UINT32 SDCARD_TimerCount ;
extern UINT8 finishFileName[FILENAME_SIZE];
extern DiskStatus appInfoDiskStatus;
extern CDFS_FILE_TAB_T *lastNode;
extern int VideoSec;
extern UINT32 gRecStartTime;

extern MoveFileState infoFuncMoveFileState ;

extern BuzzerSettingState BuzzerActiveState;
extern PanaState ActiveState;
extern UINT32 ConnectDev_TimerCount;
extern void appPowerOffUiMsgSend(UINT32 param);
extern UINT32 appUIParaSave(void);
extern void _userSettingDefaultSet(	uiParamSetting_t *pbuf);
extern UINT32 appUserSettingSave(uiParamSetting_t *pbuf);
extern void appAudioRecFlagSet(BOOL onoff);
extern void appAudioRecVolumeConfig(void);
extern void appUIParaReset(void);
extern PowerDetect appInfoPowerDETStateGet(void);
extern void appVideoSizeSet(void);
extern void appVideoSeamLessTimeSet(UINT32 val);
extern void appModeSet(UINT32 mode);
extern BOOL jet_pb_image_pip_is_in_pip_get(void);
extern void jet_pb_image_pip_exit(void);
extern int jet_cdfs_file_num_limit_by_card_size_get(UINT8 cdfs_folder);
extern TemperatureState jetTempGet(UINT8 *temp);
extern BOOL appInfoSD_isWriteProtect(void);
extern void appInfoRegularHandler(void);
extern UINT32 appInfoSDFormatFB(UINT32 state);
extern void  appOsdMessageBoxShow(BOOL bShow, unsigned short usStrId);
extern UINT32 appInfoModeChangeFB( UINT8 state  , PanaState status);
extern void jet_pbvideo_stclose(void);
extern PanaState appInfoPanaStateGet(void);
extern UINT32 appVideoCheckReady(UINT32 question);
extern void appInfoDiskErrorFB(DiskStatus state);
extern void appGsensorLevelCtrl(int mode, int level);
extern void appGsensorUpdateCompensation(UINT8 *offset);
extern void appInfoHighTempProtectedFB(UINT8 state, UINT8 temp);
extern int JetPowerCanOffState(void);
extern uiParamSetting_t *pUiSetting;
extern UINT8 spiWriteCount;
extern UINT32 SeamLessTime[UI_VIDEO_SEAMLESS_MAX];
extern view_param_t *pViewParam;
extern BOOL NewAVNModel;
extern UINT8 CopyPath[PATH_SIZE];
extern BOOL PlayPos_TimerCount_PAUSE;
extern UINT32 appInfoSecTimeBaseGet(tmx_t * time);

ParkingMode_t parkingMode;
UINT32 UdatHeaderPE=1;
UINT32 PositionEvent = 1;
UINT32 PositionStampEvent = 1;
UINT32 AVNEvent = 0 ;
char templastFileName[64]={0};
char NIndicator[] = "N";
char EIndicator[] = "E";
char AvnLatitude[] = "9999.9999";
char AvnLongitude[] = "99999.9999";
char CarSpeed[] = "000.00";
char CarAngle[] = "000";
char AvnPositionInfo[] = "1,9999.9999,N,99999.9999,E,65535,65535,";
UINT8 autoRecRetry = 0;
BOOL  POSDataReady = FALSE;
BOOL WaitInitSeqFinish = FALSE;
BOOL bRTCChanged = FALSE;
appRTC_t AVNTime;
TemperatureState TEMPstate = TEMP_NONE;
#define EVENT_MASK 0xFFFE
#define DEBUG_ON 0
int RtcChangesec = -1;
SP5K_EVENT_FLAGS_GROUP bPlusEvent_sig;
BOOL appInfoDiskSizeCheck(void);
#if 0
/*--------------------------------------------------------------------------*
* Function name : appInfoCharToInt 
* Function		: function for user rese
* Return value	: UINT8 
* Parameter1	: char data
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 appInfoCharToInt(char data)
{
	UINT8 CharData[16] = {'0','1' ,'2' ,'3','4','5','6','7','8','9','A','B','C','D','E','F'};
	UINT8 i = 0;

	for( i = 0 ; i < 16 ; i ++ )
		if( data == CharData[i])
			return i ;
	return 0;
}
#endif


/*--------------------------------------------------------------------------*
* Function name : appUserResSet 
* Function		: function for user rese
* Return value	: void 
* Parameter1	: UINT8 size
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appUserResSet(UINT8 size)
{
	pViewParam->videoSize =pUiSetting->videoSize = size;
	if(WaitInitSeqFinish)
		spiWriteCount = 3;
}


/*--------------------------------------------------------------------------*
* Function name : appBuzzerSet 
* Function		: function for set buzzer
* Return value	: void 
* Parameter1	: UINT8 set
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appBuzzerSet(UINT8 set)
{
	pUiSetting->buzzerSet= set;
	if(WaitInitSeqFinish)
		spiWriteCount = 3;
}


/*--------------------------------------------------------------------------*
* Function name : appGsensorSet 
* Function		: function for set gsensor
* Return value	: void 
* Parameter1	: UINT8 set
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appGsensorSet(UINT8 set)
{
	pUiSetting->Gsensor= set;
	if(WaitInitSeqFinish)
		spiWriteCount = 3;	
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSDFormat 
* Function		: function for format sd card
* Return value	: UINT32 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoSDFormat(void)
{

	UINT32 ret;
	UINT32 time_t0 = 0, time_t1 = 0;
	UINT32 err;
	UINT32 state = INFO_QUEUEMSG_NONE;
	
	
	appBtnDisable(BTN_ALL);

	/*CLEAR_OSD_SRC;*/
	/*CLEAR_PIP_SRC;*/
	PB_BLACK_BKGND;
	if (IS_CARD_EXIST && IS_CARD_LOCK)
	{
		HOST_PROF_LOG_ADD(LEVEL_WARN, "set lib: Card Lock, Fail to format");
		appErrMsgWithSound(ERR_MSG_CARD_LOCKED,ERR_MSG_WITH_WARNING_SOUND,ERR_MSG_SHOW_2SEC);
		state = INFO_QUEUEMSG_DISKSTATE;
		err = sp5kHostMsgSend(state, 0);
		HOST_ASSERT(err == SP5K_SUCCESS);
		ret= FALSE;
	}
	else if (!IS_CARD_EXIST )
	{
		HOST_PROF_LOG_ADD(LEVEL_WARN, "set lib: Card Lock, Fail to format");
		appErrMsgWithSound(ERR_MSG_CARD_LOCKED,ERR_MSG_WITH_WARNING_SOUND,ERR_MSG_SHOW_2SEC);
		state = INFO_QUEUEMSG_DISKSTATE;
		err = sp5kHostMsgSend(state, 0);
		HOST_ASSERT(err == SP5K_SUCCESS);
		ret= FALSE;
	}
	else if ( appInfoSD_isWriteProtect() )
	{
		HOST_PROF_LOG_ADD(LEVEL_WARN, "set lib: Write Protected, Fail to format");
		ret= FALSE;
	}
	else if (appInfoDiskStatus == DISK_SIZE_FAIL)
	{
		HOST_PROF_LOG_ADD(LEVEL_WARN, "set lib: DISK_SIZE_FAIL, Fail to format");
		ret= FALSE;
	}
	else
	{
		appInfoPanaStateSet(PANA_STATE_FORMATTING);
		appInfoRegularHandler();
		appInfoSDFormatFB(TRUE);
		LED_FLICKER_FOMATE;
		appOsdMessageBoxShow(TRUE,STRTK_BUSY_PLS_WAIT);
		time_t0 = appMsTimeGet();
		DBG_PRINT("set lib: Format disk%d start ..\n",appActiveDiskGet());
		ret = appDiskFormat(appActiveDiskGet());
		DBG_PRINT("set lib: Format disk%d done ..\n",appActiveDiskGet());

		time_t1 = appMsTimeGet() - time_t0;
		if (time_t1 < 500)
		{
			sp5kTimeDelay( SP5K_TIME_DELAY_1MS , (500 - time_t1));
		}
#if 0
		appOsdMessageBoxShow(FALSE,STRTK_BUSY_PLS_WAIT);
		//appPbShowBG();
		if (ret == SUCCESS)
			appErrMsgWithSound(ERR_MSG_COMPLETED,ERR_MSG_ONLY,500);
		else
			appErrMsgWithSound(ERR_MSG_FORMAT_ERRPR,ERR_MSG_WITH_WARNING_SOUND,500);
#endif	
		state = INFO_QUEUEMSG_DISKSTATE;
		err = sp5kHostMsgSend(state, 0);
		HOST_ASSERT(err == SP5K_SUCCESS);
		if(IS_CARD_EXIST){
			GREEN_LED_FLICKER(1000);
		}
		else{/*paul 2018.04.16 add*/
			GREEN_LED_OFF;
			RED_LED_ON;
		}
		ret = TRUE;
		appInfoRegularHandler();
	}

	appBtnEnable(BTN_ALL);

	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoVideoAutoRecRetryCB 
* Function		: function for retry auto recovery condition
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoVideoAutoRecRetryCB(void)
{
	
	printf("%s s\n",__FUNCTION__);

	if(appActiveStateGet()==APP_STATE_VIDEO_REC){
#if 0
		appInfoModeChangeFB( Success_RECORDING , PANA_STATE_RECORDING);
		appInfoPanaStateSet(PANA_STATE_RECORDING);
#endif
		autoRecRetry = 0;
		return;
	}
	
	if(autoRecRetry <=0){
		appInfoModeChangeFB( Fail_RECORDING , PANA_STATE_PREVIEW);
		return;
	}


	if(appActiveStateGet() == APP_STATE_PB_MAIN){
		autoRecRetry--;
		if(jet_pb_image_pip_is_in_pip_get()==TRUE){
				jet_pb_image_pip_exit();
		}
		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
		sp5kTimerIsrOneShotReg(0, 1000, appInfoVideoAutoRecRetryCB);
		return;
	}


	if(appActiveStateGet() == APP_STATE_PB_VIDEO){
		autoRecRetry--;
		jet_pbvideo_stclose();
		sp5kHostMsgSend(APP_KEY_PRESS_DOWN,0);
		sp5kTimerIsrOneShotReg(0, 1000, appInfoVideoAutoRecRetryCB);
		return;
	}
		
	
	if(appActiveStateGet()!=APP_STATE_VIDEO_PREVIEW){
		autoRecRetry--;
		sp5kTimerIsrOneShotReg(0, 500, appInfoVideoAutoRecRetryCB);
		return;
	}
			
		
	
	if( (appActiveStateGet() == APP_STATE_VIDEO_PREVIEW) ){
		sp5kHostMsgSend(APP_KEY_PRESS_S2, 0);
#if 0
		appInfoModeChangeFB( Success_RECORDING , PANA_STATE_RECORDING);
#endif
		autoRecRetry = 0;
		return;
	}
	
	if((appActiveStateGet()== APP_STATE_VIDEO_AAA)&&
		(appPreviousStateGet()==APP_STATE_VIDEO_PREVIEW)){
		sp5kHostMsgSend(APP_KEY_PRESS_S2, 0);
#if 0
		appInfoModeChangeFB( Success_RECORDING , PANA_STATE_RECORDING);
#endif
		autoRecRetry = 0;
		return;
	}
	
}


/*--------------------------------------------------------------------------*
* Function name : appInfoModeChange 
* Function		: function for set mode change status
* Return value	: ModeChangeStatus 
* Parameter1	: UINT8 *buf
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
ModeChangeStatus appInfoModeChange(UINT8 *buf)
{
	ModeChangeStatus tempStatus = STATUS_NONE;
	UINT32 mode = 0;

	sp5kModeGet(&mode);	
	switch(strtoul((char *)buf, NULL, 10))
	{
		case INFO_SETOPTMODE_NONE:
			
		#if DEBUG_ON	
			printf("appInfoHeaderDeCode   INFO_SETOPTMODE_NONE \n" );
		#endif
			break;
		case INFO_SETOPTMODE_PRE:
			
		#if DEBUG_ON	
			printf("appInfoHeaderDeCode   INFO_SETOPTMODE_PRE \n" );
		#endif
			if(appInfoPanaStateGet() != PANA_STATE_PREVIEW && mode != SP5K_MODE_VIDEO_PREVIEW)
			{
				if(appActiveStateGet() == APP_STATE_VIDEO_REC){
					appVideoStop();
				}else{
					if(appActiveStateGet() == APP_STATE_PB_MAIN){
						if(jet_pb_image_pip_is_in_pip_get()==TRUE){
							jet_pb_image_pip_exit();
						}
						//appStateChange(APP_STATE_MENU,STATE_PARAM_MENU_POS_SET);
					}
					#if 0 // it will appear color garbage when chage to preview
					sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
					sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
					#endif
					appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
				}
			}
			/* If not on preview mode, start 'istate2pstate' to send feedback */
			if (appInfoPanaStateGet() != PANA_STATE_PREVIEW)
				tempStatus = Success_PREVIEW;
			break;
		case INFO_SETOPTMODE_REC:
			
		#if DEBUG_ON	
			printf("appInfoHeaderDeCode   INFO_SETOPTMODE_REC \n" );
		#endif
		
			if(appInfoPanaStateGet() == PANA_STATE_RECORDING ){
				/* If on recodring mode feedback 'STATUS NONE'  */
				break;
			}
			if(appInfoDiskSizeCheck() == FALSE){
				tempStatus = Fail_RECORDING;
				break;
			}
			if(TEMPstate== TEMP_HIGH){
				tempStatus = Fail_RECORDING;
				printf("%s High temperatur \n" , __FUNCTION__);
				break;
			}
					
			if(appVideoCheckReady(CAN_I_START_RECORD)==VIDEO_STATE_YES)
			{

				tempStatus = Success_RECORDING;
			/*-- shall wait Lens completes before going focus */
				if(appInfoPanaStateGet() != PANA_STATE_RECORDING &&
						appActiveStateGet() == APP_STATE_VIDEO_PREVIEW) {
					
					appStateChange(APP_STATE_VIDEO_AAA, STATE_PARAM_FREE_CARRY);
				} else  if(IS_PLAYBACK_GROUP( appActiveStateGet() )){
				printf("%s state 0x%x\n", __FUNCTION__ , appActiveStateGet());
					if(appActiveStateGet() == APP_STATE_PB_MAIN){
						
						if(jet_pb_image_pip_is_in_pip_get()==TRUE){
							jet_pb_image_pip_exit();
						}
					}

					appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
					
					autoRecRetry =5;
					sp5kTimerIsrOneShotReg( 0 , 500, appInfoVideoAutoRecRetryCB);
				}
				else{

					autoRecRetry =5;
					sp5kTimerIsrOneShotReg( 0 , 500, appInfoVideoAutoRecRetryCB);
				}
					
			} else {
					autoRecRetry =5;
					sp5kTimerIsrOneShotReg( 0 , 500, appInfoVideoAutoRecRetryCB);

			}
			break;

	}
	
			
	return tempStatus;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSetRes 
* Function		: function for set mode video preview result
* Return value	: void 
* Parameter1	: UINT8 *buf
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSetRes(UINT8 *buf)
{
	UINT tempCmd;

	tempCmd = strtoul((char *)buf, NULL, 10);
	
	appUserResSet(tempCmd);
	appModeSet(SP5K_MODE_STANDBY);
	appVideoSizeSet();
	appModeSet(SP5K_MODE_VIDEO_PREVIEW);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDISKINFOGet 
* Function		: function for get disk information
* Return value	: void 
* Parameter1	: UINT32 *totalSize
* Parameter2	: UINT32 *used
* Parameter3	: UINT32 *sdhealth
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.13  Paul Chang "Remved unused variable"
*--------------------------------------------------------------------------*/
void appInfoDISKINFOGet( UINT32 * totalSize, UINT32 * used, UINT32 * sdhealth )
{
	
	UINT32 TotalSizeMBytes = 0;
	UINT32 FreeSizekBytes = 0;
	UINT16 retry = 3;
	UINT32 ret = 0, Sd_health = 0;

            ret = sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_HEALTH_QUERY, &Sd_health);
            if (ret == 0) {
                /* Success, Toshiba spec Max Health = 0xFF */
                printf("SD card Health = %d%%\n", Sd_health);
            }
            else {
                /* Fail, Maybe read error or this card did not support health command */
                printf("Fail to get SD Health. Error = %#X\n", Sd_health);
            }
	if( appInfoDiskStatus == DISK_NORMAL)
	{
		while(retry >0 ){
		 	sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_SIZE_BY_MEGA_BYTE, &TotalSizeMBytes);
			sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_FREE_SIZE_BY_KBYTE, &FreeSizekBytes);
			if(appDiskSDMountReady()&&TotalSizeMBytes > 0 )
				break;
			else
				retry--;
		}

		if(TotalSizeMBytes > 0){
			*totalSize = TotalSizeMBytes;
			*used = 100 -(FreeSizekBytes*100/(TotalSizeMBytes*1024));
		}
		else{
			*totalSize = 0;
			*used  = 0;
		}
			
		*sdhealth = Sd_health;
		
		


		printf("appInfoDiskInfoFB   TotalSizekBytes  Total  %d KB\n" ,*totalSize);

		printf("appInfoDiskInfoFB  FreeSizekBytes  Free  %d KB\n" , FreeSizekBytes);

		printf("appInfoDiskInfoFB  used percentage of SD card  Free  %d \n" , *used);
	}

		
}


/*--------------------------------------------------------------------------*
* Function name : appInfoFactoryReset 
* Function		: function for factory reset
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoFactoryReset(void)
{
	_userSettingDefaultSet(pUiSetting);
	appUserSettingSave(pUiSetting);
	appUIParaReset();
}


/*--------------------------------------------------------------------------*
* Function name : appInfoFileReName 
* Function		: function for rename file
* Return value	: void 
* Parameter1	: UINT8 *srcPath
* Parameter2	: UINT8 *dstPath
* Parameter3	: UINT8 *srcFileName
* Parameter4	: UINT8 *dstFileName
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoFileReName(UINT8 * srcPath, UINT8 * dstPath, UINT8 * srcFileName, UINT8 *dstFileName)
{
	UINT8 src[64],dst[64],i,j,index,tmpDstFileName[64];
	BOOL bExist = TRUE;
	char c[2];
	sprintf((char *)src,"%s/%s",srcPath,srcFileName);
	sprintf((char *)dst,"%s/%s",dstPath,dstFileName);
	printf("[%s] src: %s dst : %s\n", __FUNCTION__ , src , dst);
	strcpy((char *)tmpDstFileName,(char *)dstFileName);
	c[0]=tmpDstFileName[6];
	c[1]='\0';
	index = jet_htoi((UINT8 *)c);
	printf("[%s] tmpDstFileName = %s, index = %d\n", __FUNCTION__, tmpDstFileName, index);
	for(i=0; i<=0xF; i++){
		j = index + i;
		if(j>0xF){
			j=j-0xF;
		}
		//printf("[%s] j = %d\n", __FUNCTION__, j);
		sprintf(c,"%01X",j);
		tmpDstFileName[6] = c[0];
		sprintf((char *)dst,"%s/%s",dstPath,tmpDstFileName);
		bExist = appFileExistCheck(dst);
		if(bExist==FALSE){
			strcpy((char *)dstFileName, (char *)tmpDstFileName);
			printf("file %s does not exist, use this file name to rename!\n", dst);
			break;
		}
	}
	if(bExist == TRUE){
		sprintf((char *)dst,"%s/%s",dstPath,dstFileName);
		printf("file %s exist, delete this file first.\n", dst);
		sp5kFsFileDelete(dst);
	}
	printf("[%s]2 src: %s dst : %s\n", __FUNCTION__ , src , dst);
	if(sp5kFsRename(src,dst)==FAIL){
		printf("[%s] failed to file rename\n", __FUNCTION__);
	}
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPathGet 
* Function		: function for get file path
* Return value	: void 
* Parameter1	: UINT8 *fileNameWithPath
* Parameter2	: UINT8 *Path
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPathGet(UINT8 * fileNameWithPath, UINT8 * Path){
	char tempData[64] = {0};

   	char *token;

	strcpy(tempData, (char *)fileNameWithPath);
	 token = strrchr(tempData, '/'); 
	*token = '\0';
	sprintf((char *)Path , "%s", (char *)tempData);	
}

#if 0
/*--------------------------------------------------------------------------*
* Function name : appInfoFileNameGet 
* Function		: function for get filename with whole path
* Return value	: char* 
* Parameter1	: UINT8 *fileNameWithPath
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char * appInfoFileNameGet(UINT8 * fileNameWithPath )
{
	UINT8 src[64] = {0};
	static char dstTemp[30] = {0};
   	UINT8 *token;

	
	sprintf((char *)src,"%s",fileNameWithPath);
	 token = strrchr(src, '/'); 
	token++;
	sprintf(dstTemp , "%s", token);	

	return dstTemp;

}
#endif

#if 0
/*--------------------------------------------------------------------------*
* Function name : appInfoFileNameAdd 
* Function		: function for append filename
* Return value	: char* 
* Parameter1	: UINT8 *fileName
* Parameter2	: UINT8 addChar
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char * appInfoFileNameAdd(UINT8 * fileName , UINT8 addChar)
{
	char srcFileName[30] = {0};
	UINT32 id;
	char *payload;
	
	
	sprintf((char *)srcFileName,"%s",(char *)fileName);
	id = strtoul((char *)srcFileName, &payload, 16);
	id = id >>4 ;

	sprintf(fileName ,"%07X%c.MP4" ,id,addChar );

	printf("%s      %s\n" ,__FUNCTION__, fileName);

	return fileName;
}

/*--------------------------------------------------------------------------*
* Function name : appinfoMoveFileNameGet 
* Function		: function for get full file name after move file
* Return value	: char* 
* Parameter1	: UINT8 *path
* Parameter2	: UINT8 *fileName
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char * appinfoMoveFileNameGet(UINT8 * path , UINT8 * fileName)
{

	char dstFileNameTemp[30] = {0};
	if(strcmp(path,USER_RECORD_PATH) == 0)
		sprintf((char *)dstFileNameTemp,"%s", appInfoFileNameAdd(fileName , '1'));
	else if(strcmp(path, EVENT_PATH) == 0)
		sprintf((char *)dstFileNameTemp,"%s", appInfoFileNameAdd(fileName , '3'));

	sprintf((char *)fileName,"%s", dstFileNameTemp);
	return fileName;
}
#endif


/*--------------------------------------------------------------------------*
* Function name : appBPlusEventThread 
* Function		: function for creat Gsensor event thread
* Return value	: void 
* Parameter1	: ULONG param
* History	    : 2018.04.03
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appBPlusEventThread(ULONG param)
{
       UINT32 flags, err;
       UINT32 BP = 0 ;
       int i = 0 ;

    err = sp5kOsEventFlagsCreate(&bPlusEvent_sig, "GsensorEvent");
    HOST_ASSERT(err == SP5K_SUCCESS);

       while (1) {
               sp5kOsEventFlagsGet(&bPlusEvent_sig, 1, TX_OR_CLEAR, &flags,
                       TX_WAIT_FOREVER);
               printf("%s \n",__FUNCTION__);
               gpioRead(SP5K_GPIO_GRP_FML ,BIT1, &BP);
               for (i = 0; i < 5; i++) {
                       printf("%s BP:0x%x\n" , __FUNCTION__ , BP);
                       if (BP & BIT1)
                               break;
                       sp5kTimeDelay(SP5K_TIME_DELAY_1MS,20);
               		gpioRead(SP5K_GPIO_GRP_FML ,BIT1, &BP);
               }
               if((BP & BIT1) == 0){
			  //appInfoPowerDETStateSet(POWER_DET_ACC_OFF);
                       appPowerOffUiMsgSend(APP_POWER_OFF_USB_PLUG);
               }
       }
}


/*--------------------------------------------------------------------------*
* Function name : appInfoBPlusCB 
* Function		: function for power off usb plug
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoBPlusCB(void)
{
       UINT32 err;

       if (appActiveStateGet() == APP_STATE_POWER_OFF || JetPowerCanOffState() == 0)
	       return;

       err = sp5kOsEventFlagsSet(&bPlusEvent_sig, 1, TX_INT_ENABLE);
       HOST_ASSERT(err == SP5K_SUCCESS);
}


/*--------------------------------------------------------------------------*
* Function name : appBPlusIsrInit 
* Function		: function for isr initalization
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.13  Paul Chang "Remved unused variable"
*--------------------------------------------------------------------------*/
void appBPlusIsrInit(void)
{
	
	static SP5K_THREAD *ptrdInfoHandler = NULL;
	UINT32 hwVer= 0xffffffff;
	sp5kAdcEnable(1);
	sp5kAdcRead(4,&hwVer);
	printf("%s HW %d\n",__FUNCTION__,hwVer);


	if (ptrdInfoHandler == NULL) {
    ptrdInfoHandler = sp5kOsThreadCreate("BPlus Thread",
            appBPlusEventThread, 0,
            APP_BPLUS_EVENT_THREAD_PRIORITY,
            APP_BPLUS_EVENT_THREAD_PRIORITY,
            0,
            TX_AUTO_START);
    HOST_ASSERT(ptrdInfoHandler);
	}
#if 0
	if( hwVer >  (HWVER_LV_1S - HWVER_LV_TOLERANCE ) ){/*1S HW*/
		sp5kGpioCfgSet( SP5K_GPIO_GRP_FML, 1<<PIN_0, 0); /*Input*/
		sp5kGpioIntEnable(SP5K_GPIO_INT_FM00, 0);
		sp5kGpioIsrReg(SP5K_GPIO_INT_FM00, SP5K_TRIG_LVL_FALL, appInfoBPlusCB);
		sp5kGpioIntEnable(SP5K_GPIO_INT_FM00, SP5K_TRIG_LVL_FALL);
	}
	else
#endif
	{/*1.5S HW*/

		sp5kGpioCfgSet( SP5K_GPIO_GRP_FML, 1<<PIN_1, 0); /*Input*/
		sp5kGpioIntEnable(SP5K_GPIO_INT_FM01, 0);
		sp5kGpioIsrReg(SP5K_GPIO_INT_FM01, SP5K_TRIG_LVL_FALL, appInfoBPlusCB);
		sp5kGpioIntEnable(SP5K_GPIO_INT_FM01, SP5K_TRIG_LVL_FALL);
	}

}


/*--------------------------------------------------------------------------*
* Function name : appInfoPositionEventStampClean 
* Function		: function for update event flag in position stamp postition
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPositionEventStampClean(void )
{
	PositionStampEvent = 1;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPositionEventStampSet 
* Function		: function for set in position stamp event
* Return value	: void 
* Parameter1	: char *event
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPositionEventStampSet(char * event)
{
	UINT32 temp =strtoul(event , NULL , 10);

	if( temp > BIT0)
	{
		PositionStampEvent = PositionStampEvent & EVENT_MASK;
		PositionStampEvent = PositionStampEvent|temp;
	}
#if DEBUG_ON
	printf("%s %d\n" , __FUNCTION__,PositionStampEvent);
#endif

}


/*--------------------------------------------------------------------------*
* Function name : appInfoPositionEventSet 
* Function		: function for set postition event
* Return value	: void 
* Parameter1	: char *event
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPositionEventSet(char * event )
{
	UINT32 temp =strtoul(event , NULL , 10);
	if( temp > BIT0)
	{
		PositionEvent = PositionEvent & EVENT_MASK;
		PositionEvent = PositionEvent|temp;
	}
#if DEBUG_ON
	printf("%s %d\n" , __FUNCTION__,PositionEvent);
#endif
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPositionEventNumGet 
* Function		: function for get in position event number
* Return value	: int 
* Parameter1	: UINT32 event
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
int appInfoPositionEventNumGet(UINT32 event)
{
	int c =0;
	int i = 0;

	for( i = 9 ; i >=1 ; i-- ){
		if(((event >> i) &BIT0) == 1)
		{
			
			c =  i;
			
		}
	}
	return c;
}

#if 0
/*--------------------------------------------------------------------------*
* Function name : appInfoPositionEventNumGet 
* Function		: function for change pre-filename
* Return value	: void 
* Parameter1	: UINT32 event
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoChangePreFileName(UINT32 event){
	UINT8 Path[FILENAME_SIZE]={0};
	UINT8 lastFileName[64]={0};
	UINT8 finalFileName[64]={0};
	UINT8 dstFileName[64]={0};
	UINT32 preEvent = 0 ;
	UINT32 tempFileName = 0;
	int eventNum;
	UINT32 preUdatEvent = 0;
	UINT32 fd = 0 ;
	UINT8 str[6] = {0};
	tmx_t atime , mtime , ctime;

	if (strlen(templastFileName) == 0)
		return;
	if(appFileExistCheck(templastFileName)  == FALSE)
		return;
	appInfoPathGet(templastFileName , Path);
	sprintf((char *)lastFileName,"%s",appInfoFileNameGet(templastFileName));
	if(strstr(Path , "DCIM") != 0){

		tempFileName = strtoul(lastFileName, NULL, 16);
		preEvent = tempFileName&0xF;
		eventNum = appInfoPositionEventNumGet(event);
		printf("%s %X %X \n"   , __FUNCTION__ , preEvent ,eventNum);
		jet_file_udata_event_get((char *)templastFileName, &preUdatEvent);
		preUdatEvent = preUdatEvent| event;

		fd = sp5kFsFileOpen((const UINT8*)templastFileName, FS_OPEN_RDWR);

		if(jet_file_udata_tag_seek(fd)==FAIL){
			fsFileClose(fd);
		}
		else{
			sprintf(str , "%04d," , preUdatEvent);
			fsFileWrite(fd, str, 5);
			fsFileClose(fd);
		}





		
		if(preEvent > eventNum || preEvent == 0){
			tempFileName = ( (tempFileName &0xFFFFFFF0)|eventNum );
			sprintf(finalFileName , "%08X.MP4" , tempFileName);
			sprintf(dstFileName  , "%s/%s" , Path  , finalFileName);
			if(sp5kFsRename((UINT8 *)templastFileName, (UINT8 *)dstFileName)==FAIL){
				return FALSE;
			}
	/*FIXME: jet_appCdfsFileDel */
#if 0
			jet_appCdfsFileDel(templastFileName ,jet_cdfs_pathname_to_cdfs(templastFileName) );
			appCdfsFileSync((UINT8 *)dstFileName, 0);
#endif	
		}

	}
	else{

		
		jet_file_udata_event_get((char *)templastFileName, &preUdatEvent);
		preUdatEvent = preUdatEvent| event;
		printf("%s  change udat only %X %X\n"   , __FUNCTION__ , preUdatEvent,event);
		fd = sp5kFsFileOpen((const UINT8*)templastFileName, FS_OPEN_RDWR);

		if(jet_file_udata_tag_seek(fd)==FAIL){
			fsFileClose(fd);
		}
		else{
			sprintf(str , "%04d," , preUdatEvent);
			fsFileWrite(fd, str, 5);
			fsFileClose(fd);
		}
	}
}
#endif


/*--------------------------------------------------------------------------*
* Function name : appInfoPositionEventClean 
* Function		: function for reset position event
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPositionEventClean(void)
{
	PositionEvent = 1;	
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPositionEventFileNameGet 
* Function		: function for get position event file name
* Return value	: char 
* Parameter1	: UINT16 event
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char appInfoPositionEventFileNameGet(UINT16 event)
{
	char c = '0';
	int i = 0;

	for( i = 9 ; i >=0 ; i-- ){
		if(((event >> i) &BIT0) == 1)
		{
			
			c = '0' + i;
			
		}
	}
	return c;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoFileNameEventAdd 
* Function		: function for add file name event
* Return value	: void 
* Parameter1	: CDFS_FILE_TAB_T *pCur
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoFileNameEventAdd(CDFS_FILE_TAB_T *pCur)
{

	UINT8 Path[FILENAME_SIZE]={0};
	UINT8 finalFileName[16]={0};
	UINT8 srcFileName[32]={0};
	UINT8 dstFileName[32]={0};
	UINT8 eventstr[8]= { 0 };
	char eventC = '0';
	UINT32 tempFileName = 0;
	UINT32 event = 0, fd = 0 ;
	UINT16 folder;
	int l;
	
	/* Parking mode: force change to Parking */
	if (appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP) {
		UdatHeaderPE = BIT4;
	}
	/* Do nothing in this case */
	if (pCur == NULL || UdatHeaderPE == 1||appActiveStateGet()!= APP_STATE_VIDEO_REC){

		return;
	}
	folder = jet_folder_bit_2_cdfs_folder(pCur->folder);

	printf("%s %s event 0x%02x\n" , __FUNCTION__ , pCur->FileName, UdatHeaderPE);
	strcpy((char *)Path, jet_cdfs_folder_to_path(pCur->folder));
	snprintf((char *)srcFileName, sizeof(srcFileName), "%s/%s", Path, pCur->FileName);
	
	jetTimeEventFormUdat(srcFileName, NULL, &event);
	fd = sp5kFsFileOpen((const UINT8*)srcFileName, FS_OPEN_RDWR);
	
	if(jet_file_udata_tag_seek(fd)==FAIL){
		printf("%s - seek udata fail\n", __func__);
		fsFileClose(fd);
	} else {
		event &= ~0x1;
		event |= UdatHeaderPE;
		l = snprintf((char *)eventstr, sizeof(eventstr), "%04d," , event);
		fsFileWrite(fd, eventstr, l);
		fsFileClose(fd);
		printf("%s(%d) change EVENT field: %s(%d)\n", __func__, __LINE__, eventstr, l);
	}
	event |= (0x01 << FILE_TYPE_TOTAL_FILE) | (0x01 << FILE_TYPE_TOTAL_VIDEO);
	pCur->event = event;

	/* Check in user save folder  */
	if (folder == CDFS_FILE_FOLDER_USERSAVE || folder == CDFS_FILE_FOLDER_USERRECORD)
		return;
	if (UdatHeaderPE & BIT1) /* user save */
		goto chnage;
	if (UdatHeaderPE & BIT3 && folder == CDFS_FILE_FOLDER_EVENT)
		return;

chnage:
	eventC = appInfoPositionEventFileNameGet(event);
	tempFileName = strtoul((char *)pCur->FileName, NULL, 16);
	tempFileName = tempFileName >> 4;
	sprintf((char *)finalFileName, "%07X%c.MP4", tempFileName, eventC);

	if (strlen((char *)CopyPath) != 0)
		strcpy((char *)Path, (char *)CopyPath);
		
	sprintf((char *)dstFileName, "%s/%s", Path, finalFileName);

	if (strcmp((char *)srcFileName, (char *)dstFileName) == 0) {
		printf("%s - src and dest are same name %s\n", __func__, dstFileName);
		return;
	}

	fd = jet_cdfs_pathname_to_cdfs((char *)Path);
	if (fd == CDFS_FILE_FOLDER_EVENT) /* event */
		jet_cdfs_oldest_file_delete_for_free_by_folder(CDFS_FILE_FOLDER_EVENT);
	if(sp5kFsRename(srcFileName, dstFileName) == FAIL){
		printf("[%s] failed to rename file : src = %s, dst = %s\n",
				__FUNCTION__ , srcFileName, dstFileName);
		return;
	} else
		printf("rename file: %s -> %s\n", srcFileName, dstFileName);

	if (fd != CDFS_FILE_FOLDER_VIDEO) {
		appCdfsFolderNodeCountRemove(pCur);
		pCur->folder = 1 << fd;
		appCdfsFolderNodeCountAdd(pCur);
	}
	strcpy((char *)pCur->FileName, (char *)finalFileName);

 	if(appInfoPanaStateGet()==PANA_STATE_USER_RECORDING){
		if(appInfoUserRecStateCheck() == USERREC_FB_DISKFULL){

			printf("%s(%d) USERSAVE full\n", __func__, __LINE__);
			infoFuncMoveFileState = MOVEFILE_NONE;
			CopyPath[0] = '\0';
			appInfoUdatHeaderPESet(appInfoUdatHeaderPEGet() & (~0x0002));
			appInfoPanaStateSet(PANA_STATE_RECORDING);
			appInfoDiskErrorFB(DISK_FULL_USER);
			return;
		}
	}

	/*FIXME: jet_appCdfsFileDel */
#if 0
	jet_appCdfsFileDel(str ,jet_cdfs_pathname_to_cdfs(str) );
	appCdfsFileSync((UINT8 *)dstFileName, 0);
	appCdfsLastFileNameSet((UINT8 *)dstFileName);
#endif
	
}


/*--------------------------------------------------------------------------*
* Function name : appInfoUserRecSpace 
* Function		: function for get user record space
* Return value	: int 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
int appInfoUserRecSpace()
{
    return jet_cdfs_file_num_limit_by_card_size_get(CDFS_FILE_FOLDER_USERRECORD)
            - appCdfsFolderCount(CDFS_FILE_FOLDER_USERRECORD);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoUdatHeaderPosEventSet 
* Function		: function for set post event of update handler
* Return value	: void 
* Parameter1	: char *event
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoUdatHeaderPosEventSet(char * event)
{
    UINT32 temp = strtoul(event, NULL, 10);

    temp &= EVENT_MASK;
    if (temp != (AVNEvent & temp)) {
        UdatHeaderPE = UdatHeaderPE & EVENT_MASK;
        UdatHeaderPE = UdatHeaderPE | temp;
        AVNEvent = AVNEvent | temp;

        if (temp & (BIT1)) {
            strcpy((char *) CopyPath, USER_RECORD_PATH);
            infoFuncMoveFileState = MOVEFILE_STILL;
        } else if (temp & (BIT3) && (UdatHeaderPE & (BIT1)) == 0) {
            strcpy((char *) CopyPath, EVENT_PATH);
            infoFuncMoveFileState = MOVEFILE_WAIT_VIDEOFINISH;
        }

        if (temp & BIT4) {
            strcpy((char *) CopyPath, EVENT_PATH);
        } else if ((temp & BIT1) == 0 || appInfoUserRecSpace() > 1)
            appInfoFileNameEventAdd(lastNode);
    }
#if DEBUG_ON
    printf("%s 0x%02x 0x%02x\n" , __FUNCTION__, UdatHeaderPE, AVNEvent);
#endif
}


/*--------------------------------------------------------------------------*
* Function name : appInfoUdatHeaderPosEventClean 
* Function		: function for clean post event of update handler
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoUdatHeaderPosEventClean(void)
{
#if 0
	UINT32 err= 0 ;
	UINT32 state = 0 ;
#endif
	if ((AVNEvent & (BIT4 | BIT3 | BIT1)) == 0) {
		CopyPath[0] = '\0';
	}

       	/*For next file name*/
	if (AVNEvent == 0){
		UdatHeaderPE = 1;	/* Normal recording*/
		
	}
	else{
		UdatHeaderPE = AVNEvent;

	}
	if (AVNEvent & BIT1 && infoFuncMoveFileState == MOVEFILE_STILL)
		AVNEvent = BIT1;
	else
		AVNEvent = 0 ;
		
	if (infoFuncMoveFileState == MOVEFILE_WAIT_VIDEOFINISH) {
			/* clean change folder and status*/
			infoFuncMoveFileState = MOVEFILE_NONE;
	}

	printf("%s 0x%02x 0x%02x\n", __func__, 	UdatHeaderPE, AVNEvent);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoUdatHeaderPEGet 
* Function		: Get UdatHeaderPE
* Return value	: UINT32 
* Parameter1	: void
* History	    : 2017.08.01
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoUdatHeaderPEGet(void)
{
	printf("[%s] UdatHeaderPE = 0x%x\n",__FUNCTION__, UdatHeaderPE);
	return UdatHeaderPE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoUdatHeaderPESet 
* Function		: Set UdatHeaderPE
* Return value	: void 
* Parameter1	: UINT32 data
* History	    : 2017.08.01
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoUdatHeaderPESet(UINT32 data)
{
	UdatHeaderPE = data;
	AVNEvent = data;

	if (UdatHeaderPE == 0)
		UdatHeaderPE = 1;
	printf("[%s] UdatHeaderPE = 0x%x\n",__FUNCTION__, UdatHeaderPE);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoUdatHeaderPEClean 
* Function		: Clean UdatHeaderPE
* Return value	: void 
* Parameter1	: void
* History	    : 2018.04.03
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoUdatHeaderPEClean(void)
{
    UdatHeaderPE = 1;
    AVNEvent = 0;
    printf("[%s] UdatHeaderPE = 0x%x\n",__FUNCTION__, UdatHeaderPE);
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoPoisitonEventGet 
* Function		: function for get current position event
* Return value	: char 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char * appInfoPoisitonEventGet(void)
{
	static char str[8]={0};

#if 0
	if(appInfoPowerDETStateGet()  == POWER_DET_PARKING_WAKEUP){
		appInfoUdatHeaderPosEventSet("16");
		strcpy((char *)CopyPath, EVENT_PATH);	
	}

	else if((infoFuncMoveFileState != MOVEFILE_NONE)){

		if(appInfoRECStateGet()    ==  REC_STATE_USER)
			appInfoUdatHeaderPosEventSet("2");
		
	}
#endif
	printf("%s(%d) 0x%x\n", __func__, __LINE__, UdatHeaderPE);
	sprintf(str , "%04d",UdatHeaderPE);

	return str;
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoPoisitonEventStampGet 
* Function		: function for get current poisition event stamp
* Return value	: char 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char * appInfoPoisitonEventStampGet(void)
{
	static char str[12];

/*
¡u1 1 1 1 1 1¡v
From left
G sensor
risky point
confluence point
big curve
Parking Mode
Smart Assist warning
1:set  ¡þ  0:clear
*/
	if(appInfoPowerDETStateGet()  == POWER_DET_PARKING_WAKEUP)
		sprintf(str , "%d %d %d %d %d %d",
		        0, 0, 0, 0, 1, 0);
	else
		sprintf(str , "%d %d %d %d 0 %d",
		        ((PositionStampEvent&BIT3)>>3), ((PositionStampEvent&BIT6)>>6),
		        ((PositionStampEvent&BIT7)>>7), ((PositionStampEvent&BIT8)>>8),
		        ((PositionStampEvent&BIT2)>>2));
	appInfoPositionEventStampClean();

	return str;
}



char *appInfoPositionSync();
void appInfoStoragePosDataStampGet(char * src , char * dst);
/*--------------------------------------------------------------------------*
* Function name : *appInfoPostionALLStampGet 
* Function		: function for get current poisition event stamp
* Return value	: char 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *appInfoPostionALLStampGet(void)
{
	static char str[64];
	static char str_tmp2[64];
	if(POSDataReady){
		static char str_tmp1[64];
		float gpsValue;
		int dd,mm,ss;
		
		gpsValue = atof(AvnLatitude);
		dd = (int)(gpsValue/100);
		mm = ((int)gpsValue)%100;
		ss = (int)((gpsValue-(int)gpsValue)*60);
		sprintf(str_tmp1,"%s %02d.%02d.%02d", NIndicator, dd, mm, ss);
		gpsValue = atof(AvnLongitude);
		dd = (int)(gpsValue/100);
		mm = ((int)gpsValue)%100;
		ss = (int)((gpsValue-(int)gpsValue)*60);
		sprintf(str_tmp2,"%s %s %03d.%02d.%02d", str_tmp1, EIndicator, dd, mm, ss);
	}else {
	
		 appInfoStoragePosDataStampGet(appInfoPositionSync() , str_tmp2);
	}

	sprintf(str,"%s %s", str_tmp2, appInfoPoisitonEventStampGet());
    return str;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoTimeGetFormAVN 
* Function		: function for get current poisition event stamp
* Return value	: BOOL 
* Parameter1	: char *buf
* Parameter2	: appRTC_t *ptime
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoTimeGetFormAVN(char * buf, appRTC_t * ptime)
{
    UINT32 time = 0;
    time = strtoul(buf, NULL, 10);
    BOOL ret = FALSE;

    ptime->hour = time / 10000;
    if (ptime->hour >= 24)
        goto error;
    ptime->min = (time % 10000) / 100;
    if (ptime->min >= 60)
        goto error;
    ptime->sec = (time % 100);
    if (ptime->sec >= 60)
        goto error;

    ret = TRUE;
    error: return ret;
}


/*--------------------------------------------------------------------------*
* Function name : isValidDay 
* Function		: check valid of day of system time
* Return value	: BOOL 
* Parameter1	: appRTC_t *ptime
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL isValidDay(appRTC_t *ptime)
{
    static UINT8 month[] =
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    UINT8 day = month[ptime->mon - 1];
    int year = ptime->year;
    BOOL leap;

    leap = (year % 400) == 0 || ((year % 4) == 0 && (year % 100) != 0);

    /* check February of leap year */
    if ((ptime->mon-1) == 1 && leap)
        day += 1; /* 29 day */

    return (ptime->day <= day);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDateGetFormAVN 
* Function		: convert ddmmyy format to system time format
* Return value	: BOOL 
* Parameter1	: char *buf
* Parameter2	: appRTC_t *ptime
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoDateGetFormAVN(char * buf, appRTC_t * ptime)
{
    UINT32 date = 0;
    date = strtoul(buf, NULL, 10);
    BOOL ret = FALSE;

    ptime->mon = (date % 10000) / 100;
    if (ptime->mon > 12)
        goto error;
    ptime->year = ((date % 100) + 2000);
/*
    if (ptime->year > 2038)
        goto error;
  */
    ptime->day = date / 10000;
    if (!isValidDay(ptime))
        goto error;

    ret = TRUE;
    error: return ret;
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoPositionSync 
* Function		: Get Avn Position Info
* Return value	: char 
* Parameter1	: void
* History	    : 2018.04.03
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *appInfoPositionSync()
{
    return AvnPositionInfo;
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoPositionLatitudeGet 
* Function		: function for get avn latitude
* Return value	: char 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *appInfoPositionLatitudeGet(void)
{
    return AvnLatitude;
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoPositionLongitudeGet 
* Function		: function for get avn longitude
* Return value	: char 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *appInfoPositionLongitudeGet(void)
{
    return AvnLongitude;
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoPositionNIndicatorGet 
* Function		: function for get NIndicator
* Return value	: char 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *appInfoPositionNIndicatorGet(void)
{
    return NIndicator;
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoPositionEIndicatorGet 
* Function		: function for get EIndicator
* Return value	: char 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *appInfoPositionEIndicatorGet(void)
{
    return EIndicator;
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoPositionSpeedGet 
* Function		: function for get avn speed
* Return value	: char 
* Parameter1	: void
* History	    : 2017.09.05
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *appInfoPositionSpeedGet(void)
{
    return CarSpeed;
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoPositionAngleGet 
* Function		: function for get avn direction
* Return value	: char 
* Parameter1	: void
* History	    : 2017.09.05
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *appInfoPositionAngleGet(void)
{
    return CarAngle;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoRTCChangedGet 
* Function		: function for get the flag of rtc changed
* Return value	: BOOL 
* Parameter1	: void
* History	    : 2017.09.05
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoRTCChangedGet(void)
{
    return bRTCChanged;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoRTCChangedSet 
* Function		: function for set the flag of rtc changed
* Return value	: BOOL 
* Parameter1	: BOOL set
* History	    : 2017.09.05  SC Chang
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoRTCChangedSet(BOOL set)
{
    bRTCChanged = set;
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoAVNTimeGet 
* Function		: function for get avn time
* Return value	: appRTC_t 
* Parameter1	: void
* History	    : 2017.09.05  SC Chang
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
appRTC_t *appInfoAVNTimeGet(void)
{
    return &AVNTime;
}


/*--------------------------------------------------------------------------*
* Function name : *appInfoEventSync 
* Function		: function for get position event
* Return value	: char 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
char *appInfoEventSync()
{
    static char buf[6];
   if(appInfoPowerDETStateGet()  == POWER_DET_PARKING_WAKEUP){
	   	PositionEvent = PositionEvent & EVENT_MASK;
		PositionEvent = PositionEvent|16;
   }
    snprintf(buf, sizeof(buf), "%d", PositionEvent);

    return buf;
}


/*--------------------------------------------------------------------------*
* Function name : appinfoPosReqTimeoutClear 
* Function		: If AVN time out 30 times , clear poaition date to default
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appinfoPosReqTimeoutClear(void){
	/*
	*AvnPositionInfo :
	*Char array, for storage position data form AVN
	*/
	printf("Clear Position data\n");
	strncpy(AvnPositionInfo, "1,9999.9999,N,99999.9999,E,65535,65535,",
			sizeof(AvnPositionInfo));
}


/*--------------------------------------------------------------------------*
* Function name : time2num 
* Function		: convert time to number
* Return value	: long long 
* Parameter1	: appRTC_t *t
* History	    : 2018.04.03
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
long long time2num(appRTC_t *t)
{
    long long d;
    d = t->sec + t->min * 60 + t->hour * (60 * 60) + t->day * (24 * 60 * 60)
            + t->mon * (31 * 24 * 60 * 60) + t->year * (12 * 31 * 24 * 60 * 60);

    return d;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPositionSet 
* Function		: AVN set poaition
* Return value	: void 
* Parameter1	: char *buf
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPositionSet(char *buf)
{
    /*Get info format*/
    /*$DMSG100,<Time hhmmss.sss>,<Date ddmmyy>,<Datum>,<Latitude>,<N/S Indicator>, <Longitude>,<E/W Indicator>,<Speed>,<Angle >,<Events>,< Play Status >*/
    /*$DCMD101,<Time hhmmss.sss>,<Date ddmmyy>,< Datum >,<Latitude>,<N/S Indicator>,<Longitude>,<E/W Indicator>,<Speed>,<Angle>,<X-Axis>,<Y-Axis>,<Z-Axis>,<Events>,<File index>,< Play Status >*/
    /*$DCMD101,      143112.000,          120217,        1,        3530.6158,          N            ,13933.4604 ,          E             ,   100       ,     262  ,     409    ,      397   ,      506   ,       1     ,        0         ,     1*/
#if 0
    char testString[] = "999999.999,999999,0,9999.9999,0,99999.9999,0,65535,65535,256,00";
    char temp[25] = {0};
    char x[] = {"65535"};
    char y[] = {"65535"};
    char z[] = {"65535"};
    char lv[] = {"10"};
#endif
    UINT32 index = 0;
    char *token;
    char *delm = ",";
    POSDataReady = FALSE;
    int i = 0;
    BOOL validRtcTime = FALSE;
    appRTC_t t, s;
    static UINT32 RtcChangeCount = 0;
if(appActiveStateGet()!= APP_STATE_VIDEO_REC){
	UdatHeaderPE = 1;	
	PositionEvent =1;
	AVNEvent = 0;
	return;
}
#if DEBUG_ON
    printf("POS : %s -> ", buf);
#endif
    token = strtok(buf, delm);

    while (token != NULL) {
#if DEBUG_ON
        printf("(%s) ", token);
#endif
        if (index > 1 && index < 9) {
            i += snprintf(&AvnPositionInfo[i], sizeof(AvnPositionInfo) - i,
                    "%s,", token);
        }

        switch (index) {
        case 0:
#if (DEBUG_ON)
            printf("POS time %s \n", token);
#endif
            validRtcTime = appInfoTimeGetFormAVN(token, &t);
            break;
        case 1:
            if (validRtcTime) {
#if (DEBUG_ON)
                printf("POS date %s \n", token);
#endif
                if (appInfoDateGetFormAVN(token, &t)) {
                    long long a, d;

                    appRtcGet(&s);
                    d = time2num(&s);
                    a = time2num(&t);

                    if ((a - d) > 10 || (d - a) > 10) {
#if DEBUG_ON
                        printf("%lld - %lld\n", a, d);
                        printf("%u/%u/%u %u:%u:%u - %u/%u/%u %u:%u:%u\n",
                                s.year, s.mon, s.day, s.hour, s.min, s.sec,
                                t.year, t.mon, t.day, t.hour, t.min, t.sec);
#endif
                        RtcChangeCount++;

                        if (RtcChangeCount > 10) {
				tmx_t rtc;

				rtc.tmx_year = t.year - 1900;
				rtc.tmx_mon  = t.mon;
				rtc.tmx_mday = t.day;
				rtc.tmx_hour = t.hour;
				rtc.tmx_min  = t.min;
				rtc.tmx_sec  = t.sec;
                            /* GCC specific usage */
                            AVNTime = t;
                            appRtcSet(&AVNTime);
			    gRecStartTime = appInfoSecTimeBaseGet(&rtc) - VideoSec;
                            printf("rtc changed!\n");
                            bRTCChanged = TRUE;
                        }
                    } else
                        RtcChangeCount = 0;
                }
            }
            break;
        case 3:
            strcpy(AvnLatitude, token);
            break;
        case 4:
            strcpy(NIndicator, token);
            break;
        case 5:
            strcpy(AvnLongitude, token);
            break;
        case 6:
            strcpy(EIndicator, token);
            break;
        case 7:
            strncpy(CarSpeed, token, sizeof(CarSpeed));
            break;
        case 8:
            strncpy(CarAngle, token, sizeof(CarAngle));
            break;
        case 9:
            appInfoPositionEventSet(token);
            appInfoPositionEventStampSet(token);
            appInfoUdatHeaderPosEventSet(token);
            break;
        default:
            break;
        };

        index++;
        token = strtok(NULL, delm);
    }
    POSDataReady = TRUE;
#if DEBUG_ON
    printf("\n");
#endif
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSetRecorderSound 
* Function		: set recorder sound
* Return value	: BOOL 
* Parameter1	: const char *buf
* History	    : 2017.08.23
*        	    : 2017.09.21  Jason add checking on input parameters
*	              Test Pattern: AVNinfo $DCMD053,0
*	              with different parameters.
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoSetRecorderSound(const char *buf) /*Dummy*/
{

	UINT setting = 1;  // by Default

	if (strcmp(buf, "0")==0) {
		setting = 0;  // OFF
	}
	else if (strcmp(buf, "1")==0) {
		setting = 1;  // ON
	}
	else {
		printf("Invalid <device> setting: %s\n", buf);
		return FALSE;
	}

	setting = strtoul(buf, NULL, 10);
	
	//printf("L.%d ..... tempCmd = %d#\n", __LINE__, setting);
	if (setting > 2) {
		printf("Invalid <device> setting: %d.\n", setting);
		return FALSE;
	}

	pUiSetting->voiceSet = setting;
	appAudioRecFlagSet(pUiSetting->voiceSet );
	appAudioRecVolumeConfig();
	if(WaitInitSeqFinish)
		spiWriteCount = 3;

	return TRUE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSetBeepSound_Usgae 
* Function		: set beep sound parameters print
* Return value	: static void 
* Parameter1	: void
* History	    : 2017.08.23
*        	    : 2017.09.21  Jason add checking on input parameters
*	              Test Pattern: AVNinfo $DCMD052,0
*	              with different parameters.
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static void appInfoSetBeepSound_Usgae(void)
{
	printf("Usage: $DCMD052,<type>,<setting>\n");
	printf("     : type: 0,1,2\n");
	printf("     : setting: 0,1\n");
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSetBeepSound 
* Function		: set beep sound
* Return value	: void 
* Parameter1	: UINT8 *buf
* History	    : 2018.04.03
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSetBeepSound(UINT8 *buf)
{

	UINT type;
	UINT setting;
	char *ret = NULL;  	// possible output from strtoul.
	char *ret1 = NULL;  // possible output from strtoul.
	BuzzerSettingState temp = BUZZER_SETTING_NONE;

	type = strtoul((char *) buf, &ret, 10);

	// check param <type>
	switch(type)
	{
		case 0:
			temp = BUZZER_SETTING_BOOT_ONOFF;
			break;
		case 1:
			temp = BUZZER_SETTING_WARNING_ONOFF;
			break;
		case 2:
			temp = BUZZER_SETTING_OTHER_ONOFF;
			break;
		default:
			appInfoSetBeepSound_Usgae();
			return;
	}

	ret1 = strchr((char *) buf , ',' );
	if (ret1 == NULL) {
		printf("ERROR: Need comma and the next parameter.\n");
		return;
	}

	// read the second param.
	setting = strtoul( (char *) (ret1 + 1), NULL , 10);

	switch(setting)
	{
		case 0:  // OFF
			BuzzerActiveState &= ~temp;
			break;
		case 1:  // ON
			BuzzerActiveState |= temp;
			break;
		default:
			appInfoSetBeepSound_Usgae();
			return;
	}

	if(setting)
		BuzzerActiveState |= temp;
	else
		BuzzerActiveState &= ~temp;
		
	appBuzzerSet(BuzzerActiveState);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoBatteryLvGet 
* Function		: get battery level
* Return value	: UINT32 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoBatteryLvGet(void)
{
	/* pvalADC = 16383*(detected voltage)/(SARADC reference voltage) */
	UINT32 pvalADC= 0xffffffff;
	UINT32 avgADC= 0xffffffff;
	
	int i;
	sp5kAdcInit();
	sp5kAdcEnable(1);
	for(i = 0 ; i < 10 ; i ++){
		sp5kAdcRead(SP5K_ADC_SARIN2,&pvalADC);
		avgADC+=pvalADC;
		pvalADC = 0xffffffff;
		
	}

	avgADC= avgADC/10;
	printf("ADC channel avgADC  %d\n",avgADC);
	return avgADC;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoUserRecStateCheck 
* Function		: check user record state
* Return value	: UserRecFBState 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UserRecFBState appInfoUserRecStateCheck(void)
{

	UserRecFBState ret = USERREC_FB_OTHERERR;

	if(appInfoDiskStatus   == DISK_NORMAL)
	{
		if(ActiveState == PANA_STATE_RECORDING || ActiveState == PANA_STATE_USER_RECORDING)
			 ret = USERREC_FB_NOERR;
		else
			ret =USERREC_FB_STOPREC;

	}
	else
	{
		if(appInfoDiskStatus == DISK_NONE)
			 ret = USERREC_FB_DISKNONE;
		
		if(appInfoDiskStatus == DISK_FULL)
			 ret = USERREC_FB_DISKFULL;
	}
	if(ret == USERREC_FB_NOERR){
	/*
		if(FreeMbyte <3900){
			printf("free space is less 3.9 GBytes\n");
			ret = USERREC_FB_DISKFULL;
		}else
	*/
		if(!IS_CARD_EXIST){
			ret = USERREC_FB_DISKNONE;
		} else if(appCdfsFolderCount(CDFS_FILE_FOLDER_USERRECORD)
			>= jet_cdfs_file_num_limit_by_card_size_get(CDFS_FILE_FOLDER_USERRECORD)){
			printf("over file num limit\n");
			ret = USERREC_FB_DISKFULL;
		}
	}
	printf("%s %d  %d %d\n" ,__FUNCTION__ ,appInfoDiskStatus ,ActiveState  ,ret);
	return ret;

}


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingSettings 
* Function		: check restore parking settings
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoParkingSettings( void )
{
	parkingMode.setting =pUiSetting->ParkingModesetting;

	parkingMode.reCordingTime=(UINT16)(pUiSetting->ParkingModereCordingTimeH <<8 | pUiSetting->ParkingModereCordingTimeL);

	parkingMode.batteryVoltageS=pUiSetting->ParkingModebatteryVoltageS;

	parkingMode.gSensorLevel =pUiSetting->ParkingModegSensorLevel;

	parkingMode.parkingEvntToAVN =pUiSetting->ParkingModeparkingEvntToAVN;
	
	parkingMode.byPassTime=(UINT16)(pUiSetting->ParkingModebyPassTimeH<<8|pUiSetting->ParkingModebyPassTimeL);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingToAVNGet 
* Function		: Get the parking trigger event to AVN setting
* Return value	: UINT8 
* Parameter1	: void
* History	    : 2017.08.08  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 appInfoParkingToAVNGet(void)
{
	return parkingMode.parkingEvntToAVN;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingGSensorLevelGet 
* Function		: check get the parking trigger event to AVN setting
* Return value	: UINT8 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 appInfoParkingGSensorLevelGet(void)
{
	return parkingMode.gSensorLevel;//Need check
}


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingBatterySettingGet 
* Function		: check get the parking battery setting
* Return value	: UINT32 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoParkingBatterySettingGet(void)
{

	UINT32 batteryToADC = 0 ;
	/*UINT32 resistor = 203;
	UINT32 InputToSys;*//*Removed: assigned a value but never used*/
	/*(UINT32) parkingMode.batteryVoltageS;*/
	/*InputToSys = 118;
	InputToSys = (InputToSys*100)-320;
	InputToSys = InputToSys*resistor/1000;*/
	
	/*batteryToADC = (16383*(InputToSys-50))/3/1000-30;*/

	batteryToADC = 12700;
	printf("%s batteryToADC %d\n",__FUNCTION__,batteryToADC);
	return batteryToADC;

}


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingSettingGet 
* Function		: check get the setting of parking mode
* Return value	: UINT32 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoParkingSettingGet(void)
{
	return parkingMode.setting;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingReCordingTimeGet 
* Function		: check get the recording time of parking mode
* Return value	: UINT32 
* Parameter1	: void
* History	   : 2017.08.23
*			   : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoParkingReCordingTimeGet(void)
{
	return parkingMode.reCordingTime;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingByPassTimeGet 
* Function		: check get the bypass of parking mode
* Return value	: UINT32 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoParkingByPassTimeGet(void)
{
	return parkingMode.byPassTime;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingSettingsSave 
* Function		: check save parking settings
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoParkingSettingsSave(void)
{
	pUiSetting->ParkingModesetting =parkingMode.setting ;

	pUiSetting->ParkingModereCordingTimeH = (UINT8)((parkingMode.reCordingTime&0xFF00) >>8);
	pUiSetting->ParkingModereCordingTimeL = (UINT8)((parkingMode.reCordingTime&0x00FF) );

	pUiSetting->ParkingModebatteryVoltageS = parkingMode.batteryVoltageS;

	pUiSetting->ParkingModegSensorLevel = parkingMode.gSensorLevel ;

	pUiSetting->ParkingModeparkingEvntToAVN = parkingMode.parkingEvntToAVN;

	pUiSetting->ParkingModebyPassTimeH= (UINT8)((parkingMode.byPassTime&0xFF00) >>8);
	pUiSetting->ParkingModebyPassTimeL= (UINT8)((parkingMode.byPassTime&0x00FF) );

	if(WaitInitSeqFinish)
		spiWriteCount = 3;
	
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSetParkingMode 
* Function		: check set parking mode parameters
* Return value	: void 
* Parameter1	: UINT8 *buf
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSetParkingMode(UINT8 * buf)
{

//[Format]
//$DCMD055,<Setting>,<Recording time of Parking Recording>,<Threshold Battery Voltage to start recording>,
//< G sensor Level >,<Notify new Parking Recording file to NAVI>,<Disable time before Power off and after Power on>
   	char *token;

	parkingMode.setting = (UINT8)strtoul((char *)buf , &token , 10);
	token++;
	parkingMode.reCordingTime=strtoul(token , &token , 10);
	token++;
	parkingMode.batteryVoltageS=(UINT8)(strtof(token, &token)*10);
	token++;
	parkingMode.gSensorLevel =(UINT8)strtoul(token , &token , 10);
	token++;
	parkingMode.parkingEvntToAVN =(UINT8)strtoul(token , &token , 10);
	token++;
	parkingMode.byPassTime=strtoul(token , &token , 10);
	appInfoParkingSettingsSave();
	NewAVNModel = TRUE;
	
}

#define APP_PARKING_EVENT 0x10


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingModeTriStateSet 
* Function		: check set parking mode trigger state
* Return value	: void 
* Parameter1	: ParkingModeTrigger state
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoParkingModeTriStateSet(ParkingModeTrigger state)
{
	pUiSetting->ParkingTrigger = state;

#if 0
	UINT8 *pbuf; 
        pbuf = (UINT8 *)sp5kMalloc(512); 
        memset(pbuf,0x00,512); 
	sp5kRawSegmentRead(0, 512,pbuf);
	*(pbuf+6) = state;

	sp5kRawSegmentWrite(0, 512,pbuf);
        sp5kFree(pbuf);
#endif

#ifdef DEBUG_ON
	printf("Parking Event set: %u, size %d\n", pUiSetting->ParkingTrigger,
			sizeof(pUiSetting->ParkingTrigger));
#endif
	sp5kRtcExtraSet(&(pUiSetting->ParkingTrigger), APP_PARKING_EVENT,
		       sizeof(pUiSetting->ParkingTrigger));
}


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingModeTriStateGet 
* Function		: check get parking mode trigger state
* Return value	: ParkingModeTrigger 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
ParkingModeTrigger appInfoParkingModeTriStateGet(void)
{
#if 0
	UINT8 *pbuf; 
        pbuf = (UINT8 *)sp5kMalloc(512); 
        memset(pbuf,0x00,512); 
	sp5kRawSegmentRead(0, 512,pbuf);
        printf("@1,2=%x,%x",*pbuf, *(pbuf+6)); 
	pUiSetting->ParkingTrigger = *(pbuf+6);
       sp5kFree(pbuf);
#endif
	if(appInfoParkingToAVNGet())
	sp5kRtcExtraGet(&(pUiSetting->ParkingTrigger), APP_PARKING_EVENT, 
		sizeof(pUiSetting->ParkingTrigger));
	else
		pUiSetting->ParkingTrigger =ParkingModeTigger_NONE;
#if DEBUG_ON
	printf("Parking Event get: %u, size %d\n", pUiSetting->ParkingTrigger,
			sizeof(pUiSetting->ParkingTrigger));
#endif
	return (ParkingModeTrigger)pUiSetting->ParkingTrigger;
}

#if 0
/*--------------------------------------------------------------------------*
* Function name : appInfoWaitInitSeqFinishCB 
* Function		: check waiting for initial sequence complete
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoWaitInitSeqFinishCB(void)
{
	WaitInitSeqFinish = TRUE;
	spiWriteCount = 0xff;
	appUserSettingSave(pUiSetting);
}
#endif


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingLowPowerCB 
* Function		: check handel parking low power condition
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoParkingLowPowerCB(void)
{
	appPowerOffUiMsgSend(APP_POWER_OFF_USB_PLUG); /*Power off*/
}


/*--------------------------------------------------------------------------*
* Function name : appInfHWWDTCB 
* Function		: check hardware dt clock 
* Return value	:  void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfHWWDTCB(void)
{
	SET_HW_WDT_CLK(0);
}


/*--------------------------------------------------------------------------*
* Function name : appInfHWWDT 
* Function		: check hardware wdt clock 
* Return value	:  void 
* Parameter1	: UINT32 ms
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfHWWDT(UINT32 ms)
{
	SET_HW_WDT_CLK(1);
	sp5kTimerIsrOneShotReg(0 , ms ,appInfHWWDTCB);
}

extern void appUart2Stop(BOOL en);

#if 0
/*--------------------------------------------------------------------------*
* Function name : appInfHWWDT 
* Function		: check parking mode control
* Return value	: void 
* Parameter1	: UINT32 count
* Parameter2	: UINT8* filename
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoParkingModeCtrl( UINT32 count ,UINT8 * filename)
{

	UINT32 pwrVal = 0;
#if 0
	char lastFileName[FILENAME_SIZE]={0};
	char dstFileName[FILENAME_SIZE]={0};
	UINT8 Path[FILENAME_SIZE]={0};
	char filenameWithPath[64];

	

	sprintf((char *)lastFileName,"%s",appInfoFileNameGet(filename));
	sprintf((char *)dstFileName,"%s",appInfoFileNameAdd(lastFileName , '4'));
	/*Move parking record file in here*/
	jet_cdfs_oldest_file_delete_for_free_by_folder(CDFS_FILE_FOLDER_EVENT);
	appInfoPathGet(filename , Path);
	appInfoFileReName(Path  , (UINT8 *)EVENT_PATH ,  (UINT8 *)lastFileName , (UINT8 *)dstFileName);
#endif
	/*FIXME: jet_appCdfsFileDel */
#if 0
	jet_appCdfsFileDel(lastFileName ,jet_cdfs_pathname_to_cdfs(Path));
	sprintf(filenameWithPath,"%s/%s", EVENT_PATH, dstFileName);
	appCdfsFileSync((UINT8 *)filenameWithPath, 0);
#endif
	sp5kGpioRead(SP5K_GPIO_GRP_POCU,0xffffffff,&pwrVal);
	if(count ==0 ){
	
		if( (pwrVal & BIT1) == 0){
		    /* FIXME: parking mode code change */

			printf("%s    %s\n" ,__FUNCTION__, appCdfsLastFileNameGet());
			printf("%s    %d    %d   %d\n",__FUNCTION__ ,appInfoPowerDETStateGet(),appInfoBatteryLvGet() , appInfoParkingBatterySettingGet());
			if (appInfoBatteryLvGet() < appInfoParkingBatterySettingGet()) {
				appGsensorLevelCtrl(0, 0);
				appInfoParkingModeTriStateSet(ParkingModeTigger_LOWPOWER);
			}
			appPowerOffUiMsgSend(APP_POWER_OFF_USB_PLUG); /*Power off*/
			
		}
		else
		{
			printf("1    %s    %s\n" ,__FUNCTION__, appCdfsLastFileNameGet());
			
			if(appActiveStateGet() == APP_STATE_VIDEO_REC){
				appModeWaitCfg(FALSE);
				appVideoStop();
			}
     			if(appGsensorSettingGet())
				appGsensorLevelCtrl(1, (appGsensorSettingGet()-1));

		}
		printf("%s e\n",__FUNCTION__);
	}
}
#endif


/*--------------------------------------------------------------------------*
* Function name : appInfoGSensorSetFunc 
* Function		: function for set gsensor level control 
* Return value	: BOOL 
* Parameter1	: const char *buf
* History	    : 2017.08.23
*       	    : 2017.09.22  Jason add checking on input parameters.
*                 Test Pattern: AVNinfo $DCMD050,0
*                 with different parameters.
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoGSensorSetFunc(const char *buf)
{
    UINT32 set = 0;

	//printf("%s %s\n", __FUNCTION__, buf);

	if (strlen(buf) != 1) {
		printf("Invalid <value>: %s\n", buf);
		return FALSE;
	}

	if ((buf[0] < '0') || (buf[0] > '5')) {
		printf("Invalid <value>: %s\n", buf);
		return FALSE;
	}

    // Now only '0' to '5' is valid.
    set = buf[0] - '0';
    if (set)
        appGsensorLevelCtrl(1, (UINT8) (set - 1));
    else
        appGsensorLevelCtrl(0, 0);

    appGsensorSet(set);

    return TRUE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfo3AGet 
* Function		: function for iso idex, ct index, exp and agc index
* Return value	: int 
* Parameter1	: char *buf
* Parameter2	: int buf_size
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
int appInfo3AGet(char *buf, int buf_size)
{
    UINT32 clrtmp, ctIdx, isoIdx, agcidx;
    SINT32 expidx;
    AEStatus_t AEStatus;
    int ret;
   
    appAWBALG_WBParamGet(AWB_PARA_CLRTMP, &clrtmp);

    /* Get Exp & Agc index */
    appAEPreviewExpAgcGet(&expidx, &agcidx);
    appAEALG_ParamGet(AE_PARA_STATUS, (SINT32*)&AEStatus);

    /* Get CT index */
    sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_CT_IDX_GET, &ctIdx);

    /* Get ISO index */
    sp5kDynIqCfgGet(SP5K_DYN_IQ_CFG_ISO_IDX_GET, &isoIdx);

    ret = snprintf((char *)buf, buf_size, "%d,%d,%d,%d,%d,%d\n",
            AEStatus.LVx1000, clrtmp, expidx, agcidx, ctIdx, isoIdx);

    return ret;
}


/*--------------------------------------------------------------------------*
* Function name : PlayPosCountPause 
* Function		: Set play position timer count to Pause
* Return value	: void 
* Parameter1	: void
* History	    : 2018.04.03
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void PlayPosCountPause(void){
	printf("*********** [%s]\n", __FUNCTION__);
	PlayPos_TimerCount_PAUSE = TRUE;
}


/*--------------------------------------------------------------------------*
* Function name : PlayPosCountReSume 
* Function		: Set play position timer count to ReSume
* Return value	: void 
* Parameter1	: void
* History	    : 2018.04.03
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void PlayPosCountReSume(void){
	printf("*********** [%s]\n", __FUNCTION__);
	PlayPos_TimerCount_PAUSE = FALSE;
}


/*--------------------------------------------------------------------------*
* Function name : PlayPosCountStart 
* Function		: function for play position timer count start
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void PlayPosCountStart(void){
	printf("*********** [%s]\n", __FUNCTION__);
	PlayPos_TimerCount = 0;
}


/*--------------------------------------------------------------------------*
* Function name : PlayPosCountStop 
* Function		: function for play position timer count stop
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.23
*			   : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void PlayPosCountStop(void){
	printf("*********** [%s]\n", __FUNCTION__);
	PlayPos_TimerCount = 0xFFFFFFFF;
}


/*--------------------------------------------------------------------------*
* Function name : PosReqCountStart 
* Function		: function for posreq timer count start
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void PosReqCountStart(void){
	PosReq_TimerCount = 0;
}


/*--------------------------------------------------------------------------*
* Function name : PosReqCountStop 
* Function		: function for posreq timer count stop
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void PosReqCountStop(void){
	PosReq_TimerCount = 0xFFFFFFFF;
}


/*--------------------------------------------------------------------------*
* Function name : RegularCountStart 
* Function		: function for regular timer count start
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void RegularCountStart(void){
	Regular_TimerCount = 0;
}


/*--------------------------------------------------------------------------*
* Function name : RegularCountStop 
* Function		: function for regular timer count stop
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void RegularCountStop(void){
	Regular_TimerCount = 0xFFFFFFFF;
}


/*--------------------------------------------------------------------------*
* Function name : ConnectDevCountStart 
* Function		: function for connected device timer count start
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void ConnectDevCountStart(void){
	ConnectDev_TimerCount = 0;
}


/*--------------------------------------------------------------------------*
* Function name : ConnectDevCountStop 
* Function		: function for connected device timer count stop
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void ConnectDevCountStop(void){
	ConnectDev_TimerCount = 0xFFFFFFFF;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSnapStateGets 
* Function		: function for get snap state handling
* Return value	: SnapState 
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
SnapState appInfoSnapStateGet(void)
{
	if(!IS_CARD_EXIST){
		return SnapState_DiskNone;
	}
	if(appActiveStateGet()!=APP_STATE_VIDEO_REC){
		printf("not in recording mode\n");
		return SnapState_StopRec;
	}
	if(appCdfsFolderCount(CDFS_FILE_FOLDER_JPG)
		>= jet_cdfs_file_num_limit_by_card_size_get(CDFS_FILE_FOLDER_JPG)){
		printf("over file num limit\n");
		return SnapState_Over5000PIC;
	}
	if(appCdfsFileNameValidCheck(SP5K_DCF_FILETYPE_JPG) == FAIL){
		return SnapState_OtherErr;
	}	
	return SnapState_NoERROR;
}

static BOOL _SW_WDT_STOP = FALSE;


/*--------------------------------------------------------------------------*
* Function name : appInfoSWWDTReset 
* Function		: function for software wakeup clear
* Return value	: void
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSWWDTReset()
{
    _SW_WDT_STOP = FALSE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSWWDTWakeup 
* Function		: function for software wakeup
* Return value	: void
* Parameter1	: UINT32 t
* History	    : 2018.04.12  Weikai Hsu "Added Function Header"
*			
*--------------------------------------------------------------------------*/
void appInfoSWWDTWakeup(UINT32 t)
{
    appRTC_t local;
    static appRTC_t pre;

    if (_SW_WDT_STOP) {
            appRtcWakeUpSet(&pre);

	    printf("%s: WDT has stop\n", __func__);
	    return;
    }

    printf("%s: set WDT %u\n", __func__, t);
    _SW_WDT_STOP = TRUE;
    appRtcGet(&local);

    local.sec += t;
    appRtcWakeUpSet(&local);

    pre = local;

#if DEBUG_ON
    printf("%s\n" , __FUNCTION__);
#endif

}


/*--------------------------------------------------------------------------*
* Function name : appInfoSWWDTCB 
* Function		: function for software wakeup
* Return value	: void
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSWWDTCB(void){
	
	appRTC_t local;
	if(_SW_WDT_STOP)
		return;

		/* RTC wackup is for reboot itself */
	appRtcGet(&local);
	local.sec = local.sec + 2;
	appRtcWakeUpSet(&local);

	sp5kTimerIsrOneShotReg(0 , 1000 ,appInfoSWWDTCB);
#if DEBUG_ON
	printf("%s %02d:%02d:%02d\n" , __FUNCTION__ , local.hour , local.min ,  local.sec);
#endif
}


/*--------------------------------------------------------------------------*
* Function name : appInfSWWDT 
* Function		: function for register ISR 
* Return value	: void
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfSWWDT(void)
{
	sp5kTimerIsrOneShotReg(0 , 1000 ,appInfoSWWDTCB);
}

#define INT16 short

extern void _appGsensorAccXYZ(INT16 *x, INT16 *y, INT16 *z);
extern BOOL appGsensorCal(INT16* acc, UINT8* offset);


/*--------------------------------------------------------------------------*
* Function name : appInfoGSensorCompensation 
* Function		: function for gsensor compensation 
* Return value	: BOOL
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoGSensorCompensation(void){
	INT16 acc[3] = {0, 0, 256};
	UINT8 compensation[] = { 0 , 0 ,0};
	UINT16 cl[] = {0, 0, 0};
	int i, j;
	UINT16 x_max=0, y_max=0, z_max=0, x_mini=255, y_mini=255, z_mini=255;/*weikai added 180423*/
	BOOL ret, success = TRUE;
	UINT8 offset = 10;

	for (j = 0, i = 0; i < 10; i++) {
		ret = appGsensorCal(acc, compensation);
		if (ret == FAIL) {
			printf("%s(%d) G sensor cal fail\n", __func__, __LINE__);
			continue;
		}
		/*weikai added 180423*/
		if (compensation[0] > x_max) 
			x_max = compensation[0]; 
		if (compensation[0] < x_mini) 
			x_mini = compensation[0];
			
		if (compensation[1] > y_max) 
			y_max = compensation[1]; 
		if (compensation[1] < y_mini) 
			y_mini = compensation[1]; 

		if (compensation[2] > z_max) 
			z_max = compensation[2]; 
		if (compensation[2] < z_mini) 
			z_mini = compensation[2]; 

        printf("G sensor compensation : 0X%02hhX 0X%02hhX 0X%02hhX\n",
                compensation[0], compensation[1], compensation[2]);

		printf("loop:%d x_max:%d mini:%d\n",j,x_max,x_mini);
		printf("loop:%d y_max:%d mini:%d\n",j,y_max,y_mini);
		printf("loop:%d z_max:%d mini:%d\n",j,z_max,z_mini);

		j++;

		cl[0] += (UINT16)compensation[0];
		cl[1] += (UINT16)compensation[1];
		cl[2] += (UINT16)compensation[2];
		
	}

	/* Fail to calibration of g sensor*/
	if (j == 0)
		success = FALSE;
	/*weikai added 180423*/

	if(abs(x_max-x_mini) > offset)
		success = FALSE;
	if(abs(y_max-y_mini) > offset)
		success = FALSE;
	if(abs(z_max-z_mini) > offset)
		success = FALSE;

	cl[0] /= j; compensation[0] = 0xff & cl[0];
	cl[1] /= j; compensation[1] = 0xff & cl[1];
	cl[2] /= j; compensation[2] = 0xff & cl[2];

	pUiSetting->GSensorComX = compensation[0];
	pUiSetting->GSensorComY = compensation[1];
	pUiSetting->GSensorComZ = compensation[2];

        printf("G sensor compensation(%d,%d,%d) - 0X%02hhX 0X%02hhX 0X%02hhX\n",
			acc[0], acc[1], acc[2],
                compensation[0], compensation[1], compensation[2]);
	if(WaitInitSeqFinish)
	{
		if(success)
			spiWriteCount = 1;
	}
	/*	appUserSettingSave(pUiSetting);*/

	return success;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoGSensorCompensationSet 
* Function		: function for set gsensor compensation 
* Return value	: void
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoGSensorCompensationSet(void){
	UINT8 compensation[] = { 0 , 0 ,0};

	compensation[0] = pUiSetting->GSensorComX;
	compensation[1] = pUiSetting->GSensorComY;
	compensation[2] = pUiSetting->GSensorComZ;
	appGsensorUpdateCompensation(compensation);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDualBootRAWCInit 
* Function		: function for dual boot init
* Return value	: BOOL
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoDualBootRAWCInit(void){

	UINT8 *pbuf;
	UINT8 count;
	BOOL state = FALSE;
	pbuf = sp5kMalloc(512);		
	memset(pbuf,0,512);
	
	sp5kRawSegmentRead(0, 512,pbuf);
	if( *(pbuf+1)  > 3 ){		 	
		fsFileDelete((UINT8 *)"D:\\SPHOST.BRN");
		 *(pbuf) = 0;
		 *(pbuf+1) = 0;

		 state =  FALSE;
	 }
	else{
		count = *(pbuf+1);
		*(pbuf) = 0;
		*(pbuf+1) =  count++;
		state = TRUE;
	}
	
	sp5kRawSegmentWrite(0, 512,pbuf);
	sp5kFree(pbuf); 
	return state;
}


/*--------------------------------------------------------------------------*
* Function name : appInfodualBootRAWCSuccess 
* Function		: function for dual boot success
* Return value	: void
* Parameter1	: void
* History	    : 2017.08.23
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfodualBootRAWCSuccess(void)
{
	UINT8 *pbuf;
	pbuf = sp5kMalloc(512);		
	memset(pbuf,0,512);
	
	sp5kRawSegmentRead(0, 512,pbuf);

	*(pbuf) = 1;
	*(pbuf+1) = 0;

	sp5kRawSegmentWrite(0, 512,pbuf);
    sp5kFree(pbuf); 

}


/*--------------------------------------------------------------------------*
* Function name : appInfoTemperaturDetectCB 
* Function		: Detect temperatur call back function
* Return value	: void
* Parameter1	: void
* History	    : 2017.08.23  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoTemperaturDetectCB(void){
	UINT8 temp = 0 ;
	UINT32 mode = 0;
	TemperatureState state;
	state = jetTempGet(&temp);
	sp5kModeGet(&mode);	
	printf("%s state = 0x%x temp = %d\n" , __FUNCTION__ ,state , temp);
	switch(state){
		case TEMP_NONE:
			TEMPstate = TEMP_NONE;
			appInfoHighTempProtectedFB(TEMPstate, temp);
		break;
		case TEMP_NORMAL:
			if(TEMPstate == TEMP_HIGH  )
			{
				if( temp < 75){
					if(appVideoCheckReady(CAN_I_START_RECORD)==VIDEO_STATE_YES)
					{
						/*-- shall wait Lens completes before going focus */
						if(appInfoPanaStateGet() != PANA_STATE_RECORDING &&
							appActiveStateGet() == APP_STATE_VIDEO_PREVIEW) {
							ConnectDev_TimerCount = 0;
							appStateChange(APP_STATE_VIDEO_AAA, STATE_PARAM_FREE_CARRY);
						}
					}
					temp = 75;
					TEMPstate = TEMP_NORMAL;
					appInfoHighTempProtectedFB(TEMPstate, temp);
			  	}
				else
					TEMPstate = TEMP_HIGH;
			}
			else
				TEMPstate = TEMP_NORMAL;
		break;
		case TEMP_HIGH:
			if(TEMPstate == TEMP_NORMAL){
				TEMPstate = TEMP_HIGH;
				appInfoHighTempProtectedFB(TEMPstate, temp);	
				jetErrStorageState(ERROR_SAVE_TEMP_HIGH);
				PlayPosCountStop();
				RegularCountStop();
				ConnectDevCountStop();
			}
				
			if(appInfoPanaStateGet() != PANA_STATE_PREVIEW && mode != SP5K_MODE_VIDEO_PREVIEW)
			{
					if(appActiveStateGet() == APP_STATE_VIDEO_REC){
					appVideoStop();
					}else{
					if(appActiveStateGet() == APP_STATE_PB_MAIN){
						if(jet_pb_image_pip_is_in_pip_get()==TRUE){
							jet_pb_image_pip_exit();
						}
					}
					sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
					sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
					appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
				}
			}

		break;
		case TEMP_MAX:
			break;

	}

	sp5kTimerIsrOneShotReg( 0 , 60000, appInfoTemperaturDetectCB);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoTemperaturStateGet 
* Function		: Get temperatur state
* Return value	: TemperatureState
* Parameter1	: void
* History	    : 2018.04.12  Weikai Hsu "Added Function Header"
*		
*--------------------------------------------------------------------------*/
TemperatureState appInfoTemperaturStateGet(void){

	return TEMPstate;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoUpdateErrSet 
* Function		: Get temperatur state
* Return value	: UINT8
* Parameter1	: ErrTypeUpdate type
* Parameter2	: ErrCodeUpdate code
* History	    : 2017.08.23  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 appInfoUpdateErrSet(ErrTypeUpdate type, ErrCodeUpdate code)
{
	UINT8 ret=0;
	UINT8 *pbuf;
	pbuf = sp5kMalloc(512);
	if(pbuf==NULL){
		printf("failed to allocate memory!");
		return ret;
	}
	memset(pbuf, 0, 512);
	sp5kRawSegmentRead(0, 512, pbuf);
	if(pbuf[RAWC_UPDATA_ERRTYPE_INDEX]){
		pbuf[RAWC_UPDATA_ERRTYPE_INDEX] = pbuf[RAWC_UPDATA_ERRTYPE_INDEX] |type;
		pbuf[RAWC_UPDATA_ERRCOUNT_INDEX] = pbuf[RAWC_UPDATA_ERRCOUNT_INDEX] +1;
	}
	else{
		pbuf[RAWC_UPDATA_ERRTYPE_INDEX] = type;
		pbuf[RAWC_UPDATA_ERRCOUNT_INDEX] =0;
	}

	pbuf[RAWC_UPDATA_ERRCODE_INDEX]= code;

	ret = pbuf[RAWC_UPDATA_ERRCOUNT_INDEX];

	printf("%s   ErrType = 0x%x   ErrCode = 0x%x Err count = 0x%x\n" , __FUNCTION__ ,
		pbuf[RAWC_UPDATA_ERRTYPE_INDEX],pbuf[RAWC_UPDATA_ERRCODE_INDEX],pbuf[RAWC_UPDATA_ERRCOUNT_INDEX]);
	sp5kRawSegmentWrite(0, 512,pbuf);
	sp5kFree(pbuf);

	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSDCardSlowSet 
* Function		: Storage SD card write slow event
* Return value	: void
* Parameter1	: void
* History	    : 2018.01.25  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*			    : 2018.04.13  Paul Chang "Modufy 'return ret' to 'return' , this function is returning void"
*--------------------------------------------------------------------------*/
void appInfoSDCardSlowSet(void)
{
	
	UINT8 *pbuf;
	pbuf = sp5kMalloc(512);
	if(pbuf==NULL){
		printf("failed to allocate memory!");
		return;
	}
	memset(pbuf, 0, 512);
	sp5kRawSegmentRead(0, 512, pbuf);

	if (pbuf[RAWC_SDCARD_SLOW_INDEX] == 0x00) {
		pbuf[RAWC_SDCARD_SLOW_INDEX] = 0x01;
		sp5kRawSegmentWrite(0, 512,pbuf);
	}
	sp5kFree(pbuf);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSDCardSlowClr 
* Function		: Storage SD card slow control
* Return value	: void
* Parameter1	: void
* History	    : 2018.01.25  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*			    : 2018.04.13  Paul Chang "Modufy 'return ret' to 'return' , this function is returning void"
*--------------------------------------------------------------------------*/
void appInfoSDCardSlowClr(void)
{
	
	UINT8 *pbuf;
	pbuf = sp5kMalloc(512);
	if(pbuf==NULL){
		printf("failed to allocate memory!");
		return;
	}
	memset(pbuf, 0, 512);
	sp5kRawSegmentRead(0, 512, pbuf);
	pbuf[RAWC_SDCARD_SLOW_INDEX] = 0x00;
	sp5kRawSegmentWrite(0, 512,pbuf);
	sp5kFree(pbuf);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSDCardSlowGet 
* Function		: Storage SD card slow read
* Return value	: void
* Parameter1	: void
* History	    : 2018.01.25  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 appInfoSDCardSlowGet(void)
{
	UINT8 ret=0;
	UINT8 *pbuf;
	pbuf = sp5kMalloc(512);
	if(pbuf==NULL){
		printf("failed to allocate memory!");
		return ret;
	}
	memset(pbuf, 0, 512);
	sp5kRawSegmentRead(0, 512, pbuf);
	ret = pbuf[RAWC_SDCARD_SLOW_INDEX];
	sp5kFree(pbuf);

	return ret;
}


 /*--------------------------------------------------------------------------*
 * Function name : appInfoUpdateErrGet 
 * Function 	 : Get Error type & Error code & Error count
 * Return value  :	ErrTypeUpdate
 * Parameter1	 : ErrCodeUpdate * code
 * Parameter2	 : UINT8 * count
 * History		 : 2017.08.23  Isaac Fong
 *				 : 2018.04.12  Weikai Hsu "Added Function Header"
 *--------------------------------------------------------------------------*/
ErrTypeUpdate appInfoUpdateErrGet(ErrCodeUpdate * code, UINT8 * count){

	UINT8 * pbuf;
	static ErrTypeUpdate type = ERRTYPE_UPDATE_NOERR;
	pbuf = sp5kMalloc(512);
	memset(pbuf, 0, 512);
	sp5kRawSegmentRead(0, 512, pbuf);

	type =pbuf[RAWC_UPDATA_ERRTYPE_INDEX] ; 
	*code = pbuf[RAWC_UPDATA_ERRCODE_INDEX];
	*count = pbuf[RAWC_UPDATA_ERRCOUNT_INDEX] ;
	printf("%s   ErrType = 0x%x   ErrCode = 0x%x Err count =0x%x\n" , 
		__FUNCTION__ ,pbuf[RAWC_UPDATA_ERRTYPE_INDEX],pbuf[RAWC_UPDATA_ERRCODE_INDEX],pbuf[RAWC_UPDATA_ERRCOUNT_INDEX]);	
	return type;

}


#define VERSION_OFFSET 0x384
/*--------------------------------------------------------------------------*
* Function name : appInfoFWUPDateVerGet 
* Function		: Get Update file type form update file
* Return value	: BOOL
* Parameter1	: UINT8 * filename
* History	    : 2017.10.06  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoFWUPDateVerGet(UINT8 * filename){

	UINT32 fd = 0;
	UINT8 buf[64];
	
	char *p = NULL, *t;
	UINT32 fw1AOffsetddr = 0 ;
	UINT32 fw2AOffsetddr = 0 ;
	UINT32 updateMode = 0 ;
	UINT32 updateVersion = 0 ;

	fd = sp5kFsFileOpen(filename, SP5K_FS_OPEN_RDONLY);
	if(fd){
			char v[] = "ver"; 
			sp5kFsFileRead(fd, buf, sizeof(buf));
			fw1AOffsetddr = buf[0x1f]<< 24 |buf[0x1e]<< 16|buf[0x1d]<< 8|buf[0x1c];/*FW1 addr*/
			fw2AOffsetddr = buf[0x2f]<< 24 |buf[0x2e]<< 16|buf[0x2d]<< 8|buf[0x2c];/*FW2 addr*/
			sp5kFsFileClose(fd);
			fd = sp5kFsFileOpen(filename, SP5K_FS_OPEN_RDONLY);
			if(fw1AOffsetddr)
				sp5kFsFileSeek(fd , fw1AOffsetddr+VERSION_OFFSET , SP5K_FS_SEEK_SET);
			else if(fw2AOffsetddr)
				sp5kFsFileSeek(fd , fw2AOffsetddr+VERSION_OFFSET , SP5K_FS_SEEK_SET);
			memset(buf , 0x00 , sizeof(buf));
			
			sp5kFsFileRead(fd, buf, sizeof(buf));
			sp5kFsFileClose(fd);

			p = strstr((char *)buf, v); 
			
			if(p!=NULL){
				p=p+3;
				updateMode = strtoul(p , &t , 10);
				t++;
				updateVersion = strtoul(t , NULL,10);
				printf("%s(%d) %d %d\n", __func__, __LINE__, updateMode, updateVersion);

				if(updateMode != APP_MODELNAME){
					printf("Model number  Error %s %s %s 0x%x 0x%x\n" , __FUNCTION__ , buf,p,fw1AOffsetddr,fw2AOffsetddr);
					return FALSE;
				}
				if(updateVersion > APP_VERSION){
					printf("Find version %s %s %s 0x%x 0x%x\n" , __FUNCTION__ , buf,p,fw1AOffsetddr,fw2AOffsetddr);
					return TRUE;
				}
				else{
					printf("Version error %s %s %s 0x%x 0x%x\n" , __FUNCTION__ , buf,p,fw1AOffsetddr,fw2AOffsetddr);
					return FALSE;
				}
				printf("Find version %s %s %s 0x%x 0x%x\n" , __FUNCTION__ , buf,p,fw1AOffsetddr,fw2AOffsetddr);
			}
			else{
				printf("%s Can't found updat version buf : %s\n" , __FUNCTION__ ,buf);
				return FALSE;
			}
	}
	else {
		sp5kFsFileClose(fd);
	}

	return FALSE;

}


 /*--------------------------------------------------------------------------*
 * Function name : appInfoUpdateTypeGet 
 * Function 	 : Get Update file type form update file
 * Return value  : ErrTypeUpdate
 * Parameter1	 : UINT8 * filename
 * History		 : 2017.08.23  Isaac Fong
 *				 : 2018.04.12  Weikai Hsu "Added Function Header"
 *--------------------------------------------------------------------------*/
ErrTypeUpdate appInfoUpdateTypeGet(UINT8 * filename){

	UINT32 fd = 0;
	UINT8 buf[64];
	UINT32 size = 0 ;
	static ErrTypeUpdate type = ERRTYPE_UPDATE_NOERR;
	fd = sp5kFsFileOpen(filename, SP5K_FS_OPEN_RDONLY);
	
	if(fd){
			sp5kFsFileRead(fd, buf, sizeof(buf));
			size = buf[0x1f]<< 24 |buf[0x1e]<< 16|buf[0x1d]<< 8|buf[0x1c];/*FW1 size*/

			if(size)
				type=type|ERRTYPE_UPDATE_FW1;
			
			size = buf[0x2f]<< 24 |buf[0x2e]<< 16|buf[0x2d]<< 8|buf[0x2c];/*FW2 size*/

			if(size)
				type=type|ERRTYPE_UPDATE_FW2;


			size = buf[0x1b]<< 24 |buf[0x1a]<< 16|buf[0x19]<< 8|buf[0x18];/*Resource size*/

			if(size)
				type=type|ERRTYPE_UPDATE_RES;
	}
	else{

			type=ERRTYPE_MAX;
	}
	sp5kFsFileClose(fd);
	return type;

}


#define BOOTERRCOUNT_INDEX 0x12
/*--------------------------------------------------------------------------*
* Function name : appInfoBootErrSet 
* Function		: Count & set Boot up error
* Return value	: void
* Parameter1	: void
* History	    : 2017.08.24  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoBootErrSet(void)
{	
	UINT8 rbuf;	

	sp5kRtcExtraGet(&rbuf, BOOTERRCOUNT_INDEX,sizeof(rbuf));	
	rbuf = rbuf+1;	
	sp5kRtcExtraSet(&rbuf, BOOTERRCOUNT_INDEX,sizeof(rbuf));
}


/*--------------------------------------------------------------------------*
* Function name : appInfoBootErrGet 
* Function		: Get how many times boot up error 
* Return value	: UINT8
* Parameter1	: void
* History	    : 2017.08.24  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 appInfoBootErrGet(void){

	UINT8 rbuf;		
	sp5kRtcExtraGet(&rbuf, BOOTERRCOUNT_INDEX,sizeof(rbuf));	
	
	return rbuf;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoBootErrGClear 
* Function		: Clear boot up error count 
* Return value	: void
* Parameter1	: void
* History	    : 2017.08.24  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoBootErrClear(void){

	UINT8 rbuf = 0;		
	sp5kRtcExtraSet(&rbuf, BOOTERRCOUNT_INDEX,sizeof(rbuf));	
}


/*--------------------------------------------------------------------------*
* Function name : appInfoLastPositionSave 
* Function		: Storage to last position data to RAW , offset is 512
* Return value	: void
* Parameter1	: void
* History	    : 2017.08.30  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoLastPositionSave(void){
	UINT8 * pbuf;	
	char temp[128] = {0};	
	char *token;	
	char *delm = ",";	
	UINT32 i = 0 , index = 0;	
	pbuf = sp5kMalloc(512);	
	if(pbuf==NULL){
		printf("failed to allocate memory!");
		return;
	}
	memset(pbuf, 0, 512);	
	sp5kRawSegmentRead(4096, 512, pbuf);	
	sprintf(temp , "%s" ,AvnPositionInfo);

	if(pbuf[0] !=0xFF){
		if( strcmp((char *)pbuf , temp) ){
			if(strlen(temp) > 0 ){
				memset(pbuf, 0, 512);
			  	token = strtok(temp, delm);
				  while (token != NULL) {
				  	if(index== 5)
				            i += snprintf((char *)&pbuf[i], 512, "%s,", "65535");
					else
					     i += snprintf((char *)&pbuf[i], 512, "%s,", token);
					
				       token = strtok(NULL, delm);
					index++;
				  }
				
			}
			sp5kRawSegmentWrite(4096, 512, pbuf);
		}
	}
	else{
			if(strlen(temp) > 0 ){
				memset(pbuf, 0, 512);
			  	token = strtok(temp, delm);
				  while (token != NULL) {
				  	if(index== 5)
				            i += snprintf((char *)&pbuf[i], 512, "%s,", "65535");
					else
					     i += snprintf((char *)&pbuf[i], 512, "%s,", token);
					
				       token = strtok(NULL, delm);
					index++;
				  }
				
			}
			sp5kRawSegmentWrite(4096, 512, pbuf);
	}
		
	#if 1
		if(pbuf[0] != 0xFF)
			printf("%s pbuf %s\n" , __FUNCTION__ , pbuf);
		printf("%s AvnPositionInfo %s\n" , __FUNCTION__ , AvnPositionInfo);
	#endif

	sp5kFree(pbuf);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoLastPositionGet 
* Function		: Get last data form RAWC  , offset is 512
* Return value	: void
* Parameter1	: void
* History	    : 2017.08.30  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoLastPositionGet(void){
	UINT8 * pbuf;

	if (POSDataReady == TRUE)
		return;
	pbuf = sp5kMalloc(512);
	if(pbuf==NULL){
		printf("failed to allocate memory!");
		return;
	}	
	memset(pbuf, 0, 512);

	sp5kRawSegmentRead(4096, 512, pbuf);
	if(pbuf[0] == 0x00 || pbuf[0] == 0xFF || appCdfsFolderCount(CDFS_FILE_FOLDER_VIDEO) == 0) //for first time boot up system
		strcpy((char *)pbuf, "1,9999.9999,N,99999.9999,E,65535,65535,");

	if (appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP) {
		char *delm = ",", *p;
		p = strtok((char *)pbuf, delm);
		int index = 0;
		int i = 0;
		while (p) {
			if (index == 5)
				i += sprintf(&AvnPositionInfo[i], "0,");
			else
				i += sprintf(&AvnPositionInfo[i], "%s,", p);
			index++;
			p = strtok(NULL, delm);
		}
	} else
		strncpy(AvnPositionInfo, (char *)pbuf, sizeof(AvnPositionInfo));
	#if 1
		printf("%s(%d) pbuf %s\n" , __FUNCTION__ , appInfoPowerDETStateGet(), pbuf);
		printf("%s AvnPositionInfo %s\n" , __FUNCTION__ , AvnPositionInfo);
	#endif
	sp5kFree(pbuf);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDiskSizeCheck 
* Function		: Get Disk size check 
* Return value	: BOOL
* Parameter1	: void
* History	    : 2017.08.30  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoDiskSizeCheck(void){
	UINT32 TotalSizeMBytes;

	if(IS_CARD_EXIST){
		sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_SIZE_BY_MEGA_BYTE, &TotalSizeMBytes);
		printf("%s SD Size %d\n" , __FUNCTION__ , TotalSizeMBytes);
		if(TotalSizeMBytes    < 6500 )
			return FALSE;
		else if ( appInfoSD_isWriteProtect() ) {
			return FALSE;
		}
		else {
			return TRUE;
		}
	}
	return FALSE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoStoragePosDataStampGet 
* Function		: Get storage position data 
* Return value	: BOOL
* Parameter1	: char *src
* Parameter2	: char *dst
* History	    : 2017.09.13  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoStoragePosDataStampGet(char * src, char * dst){
/*1,9999.9999,N,99999.9999,E,65535,65535,*/
/*3530.6158,N,13933.4604,E*/
/*strcpy(str,"N --.--.-- E ---.--.-- 0 0 0 0 0 0");*/
    char *token;
    char *delm = ",";
    char tempSrc[64] = {0};
    char tempStr[64] = {0};
    int index = 0 ;
    double gpsValue;
    int dd,mm,ss;

	
    if(strlen(src) == 0 )
	return;

    sprintf(tempSrc , "%s" , src);

    token = strtok(tempSrc, delm);

    while (token != NULL) {

        if (index == 1 || index ==3) {
		
		gpsValue = atof(token);

		if(gpsValue == 9999.9999){
			strcpy(dst,"N --.--.-- E ---.--.-- ");
			break;
		}
		dd = (int)(gpsValue/100);
		mm = ((int)gpsValue)%100;
		ss = (int)((gpsValue-(int)gpsValue)*60);
		if(index == 1 )
			sprintf(tempStr,"N %02d.%02d.%02d", dd, mm, ss);
		if(index == 3 ){
			sprintf(dst,"%s E %02d.%02d.%02d",tempStr ,dd, mm, ss);
			break;
		}
        }
	 token = strtok(NULL, delm);
	 index++;
    }
	
}


/*--------------------------------------------------------------------------*
* Function name : appInfoYdayGet 
* Function		: Get datetime for year
* Return value	: int
* Parameter1	: tmx_t *time
* History	    : 2018.04.12  Weikai Hsu "Added Function Header"
*		
*--------------------------------------------------------------------------*/
int appInfoYdayGet(tmx_t *time){
	int year = 0 ,yday = 0 ,mon=0 , mday =0;
	static UINT8 month[] =
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	BOOL leap;
	mon = time->tmx_mon;
	mday = time->tmx_mday;
	year = time->tmx_year+1900;
	
	leap = (year % 400) == 0 || ((year % 4) == 0 && (year % 100) != 0);
	if(mon == 1)
		yday = mday;
	else{
		int i;
		for(i = 0 ; i < (mon-1) ; i ++)
			yday = yday + month[i];

		yday = yday + mday;
	}
	if(leap&& mon > 2)
		yday++;
	#if DEBUG_ON
	printf("%s yday %d  year %d leap %d  mday %d mon %d\n",__FUNCTION__, yday,year,leap,mday,mon);
	#endif
	return yday;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSecTimeBaseGet 
* Function		: Get Time base
* Return value	: UINT32
* Parameter1	: tmx_t *time
* History	    : 2017.09.29  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoSecTimeBaseGet(tmx_t * time){

	int year ,  yday , sec ,totalday ,hour ,min;
	UINT32 totalsec;

	sec = time->tmx_sec;
	yday = appInfoYdayGet(time);
	year  = (time->tmx_year+1900-2014);
	hour = time->tmx_hour;
	min =time->tmx_min;
	
	totalday = yday+(year*365);

	totalsec = (totalday*24*60*60) + hour*60*60 + min*60+sec;
#if DEBUG
	printf("%s  year %d  yday %d sec %d totalday %d totalsec %d \n" , __FUNCTION__ , year , yday , sec , totalday ,totalsec);
	printf("%s   time->tmx_year %d  time->tmx_yday %d  time->tmx_mon  %d  time->tmx_mday %d time->tmx_hour  %d time->tmx_min %d time->tmx_sec  %d \n" \
			, __FUNCTION__ , time->tmx_year , time->tmx_yday, time->tmx_mon,time->tmx_mday,time->tmx_hour , time->tmx_min, time->tmx_sec );
#endif

	return totalsec;

}


#define YEAR_SEC_BASE  (365*24*60*60)
#define DAY_SEC_BASE  (24*60*60)
/*--------------------------------------------------------------------------*
* Function name : appInfoSecTimeToRealGet 
* Function		: Get the real time
* Return value	: void
* Parameter1	: UINT32 totalsec
* Parameter2	: tmx_t *time
* History	    : 2017.09.29  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*			    : 2018.04.13  Paul Chang "Remved unused variable"
*--------------------------------------------------------------------------*/
void appInfoSecTimeToRealGet(UINT32 totalsec, tmx_t * time){

	int year = 0,yday = 0 ,sec = 0 ,hour =0 ,min = 0,mon = 0 , day = 0;
	static UINT8 month[] =
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	BOOL leap;
	UINT32 temp;

	temp = totalsec;
	sec = (int)(temp%60);
	temp = (temp-sec)/60;/* sec to min*/
	min =(temp%60);
	temp = (temp-min); 
	hour = ((temp)/60%24) ; /*get hour*/
	temp =(totalsec- sec - min*60 - hour*60*60);

	
	yday = ((temp)%YEAR_SEC_BASE)/DAY_SEC_BASE;
	if(yday == 0){
		yday =365;/*for 12/31*/
	}
	
	year =  (int)( ((temp-yday*DAY_SEC_BASE) /YEAR_SEC_BASE ) +2014);

	leap = (year % 400) == 0 || ((year % 4) == 0 && (year % 100) != 0);
	temp = 0;


	for(mon = 0 ; mon < 12 ; mon ++)
	{
		temp = temp +month[mon];
		if(leap && mon == 1)
			temp=temp+1;
		if(temp  >= yday)
		{
			temp = temp-month[mon];
			mon++;
			break;
		}
		
	}
	
	if(leap && mon == 2)
		temp--;
	if(yday == 365)
	{
		mon =12;
		day = 31;
	}
	else
		day = yday - temp;

	time->tmx_sec = sec;
	time->tmx_min = min;
	time->tmx_hour =hour;
	time->tmx_mday = day;
	time->tmx_mon =mon;
	time->tmx_yday =yday;
	time->tmx_year =year;
#if DEBUG_ON
	printf("%s  year %d  yday %d month %d day %d hour %d min %d sec %d totalsec %d \n" , __FUNCTION__ , year , yday ,mon,day,hour, min , sec ,totalsec);
#endif

}


/*--------------------------------------------------------------------------*
* Function name : appInfoRtcChangeSecSet 
* Function		: Set sec to sotrage when rtc change
* Return value	: void
* Parameter1	: int sec
* History	    : 2017.09.29  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoRtcChangeSecSet(int sec){
	RtcChangesec =sec;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoRtcChangeCheck 
* Function		: Get check status for RtcChange
* Return value	: BOOL
* Parameter1	: void
* History	    : 2017.09.29  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoRtcChangeCheck(void){

	if(RtcChangesec >= 0){
		return FALSE;
	}else if(RtcChangesec == -1){
		return TRUE;
	}else{
		return FALSE;
	}
}


/*--------------------------------------------------------------------------*
* Function name : appInfoRtcChangeSecGet 
* Function		: Get which sec rtc is change
* Return value	: int
* Parameter1	: void
* History	    : 2017.09.29  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
int appInfoRtcChangeSecGet(void){
	return RtcChangesec;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoRtcChangeSecClear 
* Function		: Clear storage RTC change sec
* Return value	: void
* Parameter1	: void
* History	    : 2017.09.29  Isaac Fong
*			    : 2018.04.12  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoRtcChangeSecClear(void){
	RtcChangesec = -1;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoNewMinCheck 
* Function		: check time status for new minute
* Return value	: BOOL
* Parameter1	: tmx_t *time
* Parameter2	: int sec
* History	    : 2018.04.12  Weikai Hsu "Added Function Header"
*		
*--------------------------------------------------------------------------*/
BOOL appInfoNewMinCheck(tmx_t * time, int sec){

	if(time->tmx_sec + sec >= 60)
		return TRUE;
	else
		return FALSE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoNewHourCheck 
* Function		: check time status for new Hour
* Return value	: BOOL
* Parameter1	: tmx_t *time
* Parameter2	: int sec
* History	    : 2018.04.12  Weikai Hsu "Added Function Header"
*		
*--------------------------------------------------------------------------*/
BOOL appInfoNewHourCheck(tmx_t * time, int sec){

	if(appInfoNewMinCheck(time ,sec)&&time->tmx_min+1 >= 60)
		return TRUE;
	else
		return FALSE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoNewDayCheck 
* Function		: check time status for new day
* Return value	: BOOL
* Parameter1	: tmx_t *time
* Parameter2	: int sec
* History	    : 2018.04.12  Weikai Hsu "Added Function Header"
*			
*--------------------------------------------------------------------------*/
BOOL appInfoNewDayCheck(tmx_t * time, int sec){

	if(appInfoNewHourCheck(time ,sec)&&time->tmx_hour+1>23)
		return TRUE;
	else
		return FALSE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoNewMonthCheck 
* Function		: check time status for new month
* Return value	: BOOL
* Parameter1	: tmx_t *time
* Parameter2	: int sec
* History	    : 2018.04.12  Weikai Hsu "Added Function Header"
*		
*--------------------------------------------------------------------------*/
BOOL appInfoNewMonthCheck(tmx_t * time, int sec){
	BOOL leap;
	static UINT8 month[] =
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	
	leap = (time->tmx_year % 400) == 0 || ((time->tmx_year  % 4) == 0 && (time->tmx_year  % 100) != 0);
	if(leap == FALSE){
		if(appInfoNewDayCheck(time ,sec)&&time->tmx_mday+1 > month[time->tmx_mon-1])
			return TRUE;
		else
			return FALSE;
	}
	else{
		if(time->tmx_mon == 2){
			if(appInfoNewDayCheck(time ,sec)&&time->tmx_mday+1 > (month[time->tmx_mon-1]+1))
				return TRUE;
			else
				return FALSE;
		}
		else{
			if(appInfoNewDayCheck(time ,sec)&&time->tmx_mday+1 > month[time->tmx_mon-1])
				return TRUE;
			else
				return FALSE;
		}
	}
}


/*--------------------------------------------------------------------------*
* Function name : appInfoNewYearCheck 
* Function		: check time status for new year
* Return value	: BOOL
* Parameter1	: tmx_t *time
* Parameter2	: int sec
* History	    : 2018.04.12  Weikai Hsu "Added Function Header"
*			
*--------------------------------------------------------------------------*/
BOOL appInfoNewYearCheck(tmx_t * time ,int sec){
	if(appInfoNewMonthCheck(time ,sec)  && time->tmx_mon ==12 )
		return TRUE;
	else
		return FALSE;
}


