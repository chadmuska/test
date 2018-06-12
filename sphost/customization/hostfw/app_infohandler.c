/*--------------------------------------------------------------------------*
 *	File name	: App_infohandler.c
 *	Function	: For Handling information command
 *	First editor: Weikai Hsu
 *	First date	: 
 *	History	:
 *			:2018.04.11  Weikai Hsu "Added File Header"
 *	
 *	
 *	
 *--------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "api/sp5k_global_api.h"
#include "api/sp5k_utility_api.h"
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
#include "sp5k_os_api.h"
#include "sp5k_dbg_api.h"
#include "app_disk.h"

#include "app_cdfs_api.h"
#include "jet_utility.h"
#include "app_infohandler.h"
#include "app_uart2.h"
#include "app_infohandler_func.h"
#include "app_infohandler_feedback.h"
#include "app_infohandlerComm.h"
#include "sys/state_ctrl/app_state_private.h"
#include "app_buzzer.h"
#include "app_rtc.h"
#include "app_jet_sd_cert.h"


#define DEBUG_ON 0
#define INFOHANDLER_INIT_THREAD_PRIORITY 16
#define INFOHANDLER_INIT_THREAD_TIME_SLICE 0

#define COMUNCATION_TIMEOUT 30
#define APP_VIDEO_POS_THREAD_PRIORITY 30
#define APP_QUEUEMSG_EVENT_THREAD_PRIORITY 30
#define REGULAR_TIMEOUT 15

#if 0
SP5K_EVENT_FLAGS_GROUP InfoHandler_sig;
#endif

#define SIZE(x) (sizeof(x) / sizeof(x[0]))

#if 0
SP5K_EVENT_FLAGS_GROUP videoPos_sig;
#endif
SP5K_EVENT_FLAGS_GROUP qEvent_sig;
DiskStatus appInfoDiskStatus;

extern UINT32 appStatePhaseGet(void);

extern void appGsensorLevelCtrl(int mode, int level);
extern UINT8 autoRecRetry;
extern BOOL WaitInitSeqFinish ;
extern void  appUartInit(void);
extern void appUart2Stop(BOOL en);
extern BOOL appBuzzerReady(void);
extern UINT32 appBuzzerReadyWaitTime(void);
extern void jet_appSeamlessDelFileEvent(ULONG event);
extern void JetCdfsJPGSetFilename(BOOL set , char * filename);
extern TemperatureState TEMPstate;
UINT8 CopyPath[PATH_SIZE]={0};
UINT8 finishFileName[FILENAME_SIZE]={0};
BOOL fileReady = FALSE;
MoveFileState infoFuncMoveFileState = MOVEFILE_NONE;
BOOL PlayPos_TimerCount_PAUSE = FALSE;
UINT32 PlayPos_TimerCount = 0xFFFFFFFF ;
UINT32 PosReq_TimerCount = 0xFFFFFFFF ;
UINT32 ConnectDev_TimerCount = 0xFFFFFFFF ;
UINT32 Regular_TimerCount = 0xFFFFFFFF ;
UINT32 SDCARD_TimerCount = 0xFFFFFFFF ;
UINT8 spiWriteCount = 0xFF ;

UINT32 PosReq_TimeOutCount = 0 ;
UINT32 PlayPos_TimeOutCount = 0 ;

UINT32 ConnectDev_TimeOutCount = 0 ;
UINT32 Regular_TimeOutCount = 0 ;
UINT32 ReStartRec_TimeOutCount = 0 ;
PanaState ActiveState = PANA_STATE_MAX;
SDStatus SDActiveState = SD_STATE_MAX;
BuzzerSettingState BuzzerActiveState = BUZZER_SETTING_NONE;
DiskStatus appInfoDiskStatus = DISK_NONE;
PowerDetect powerDetect = POWER_DET_NONE;


UINT32 PARKINGMODE_Count = 0;
BOOL FirstTimeBoot = TRUE;
BOOL SDBootupCheck = TRUE;

// VUP is activated only when DVR boots with already inserted SD Card.
static BOOL SDBootup_UpgCheck = TRUE;  // Bootup Upgrade Check
BOOL NewAVNModel  = TRUE;
#if 0
void appinfoMoveFileStateInit(void);
#endif
PanaState appInfoPanaStateGet(void);

extern PowerDetect appInfoPowerDETStateGet(void);
extern void JetPowerCanOff(int st);
extern UINT32 appFirmwareUpdate(UINT8 * filename, BOOL buzzer);
extern void appSDLogMsg(void);
extern char *jet_cdfs_folder_to_path(int cdfs_folder);
extern UINT32 appPbVideoElapTimeGet(void);
extern void appUart2Init(void);
extern void JetQueueBusySet(int st);


BOOL gIsModeChange=FALSE;
UINT32 gTargetState;
/*--------------------------------------------------------------------------*
* Function name : appInfoCMDHandler 
* Function		: function for Handling information command
* Return value	: BOOL 
* Parameter1	: UINT8 *buf
* History	    : 2017.08.21
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoCMDHandler(UINT8 *buf)
{
	UINT8 status = 0, setting=0,temperature = 0;

	//UINT32 err;
	UserRecFBState temp;
    char *payload;
    UINT id;
    BOOL state;
	UINT8 _x;
	PanaState _state;/*weikai added 180425*/

    if (strncmp((char *)buf, "$DCMD", 5))
      return FALSE;
#if DEBUG_ON
    printf("%s %s\n" , __FUNCTION__, buf);
#endif
    buf += 5; //shift to command ID
    id = strtoul((char *)buf, &payload, 10);
    payload++;  // shift to payload

	switch(id)
	{
		case INFO_HEADERNUM_DEVINFO:
			/*weikai added 180425*/
			for(_x = 0; _x < 3; _x++)
			{
				_state = appInfoPanaStateGet();
			}
			appInfoDevInfo(_state);
			/*appInfoDevInfo(appInfoPanaStateGet());*/
			break;
		case INFO_HEADERNUM_SETOPTMODE:

			gIsModeChange = TRUE;
			status = appInfoModeChange((UINT8 *)payload);
#if 0
			if(Regular_TimerCount != 0xFFFFFFFF){
				Regular_TimerCount = 0;
			}
#endif
			if(status <= Success_INIT) {	/* may on same as previous state, FB immediate  */
				gIsModeChange = FALSE;
				appInfoModeChangeFB( status , appInfoPanaStateGet());
			} else {
#if 0
			if(status == Success_PREVIEW)
				appInfoModeChangeFB( status , PANA_STATE_PREVIEW);
			if(status == Success_RECORDING)
				appInfoModeChangeFB( status , PANA_STATE_RECORDING);
			if(status == Success_PLAYIDLE)
				appInfoModeChangeFB( status , PANA_STATE_PLAY_IDLE);
			if(status == Success_PLAYING)
				appInfoModeChangeFB( status , PANA_STATE_PLAYING);
#endif
				switch(status){
				case Success_PREVIEW:
					gTargetState = PANA_STATE_PREVIEW;
					break;
				case Success_RECORDING:
					gTargetState = PANA_STATE_RECORDING;
					break;
				case Success_PLAYIDLE:
					gTargetState = PANA_STATE_PLAY_IDLE;
					break;
				default:
					gTargetState = PANA_STATE_PREVIEW;
					break;
				}
			}
			break;
		case INFO_HEADERNUM_DEVFWVER:
			
			appInfoDevFWVerFB();
		break;
		case INFO_HEADERNUM_DISKINFO:
			
			{
				UINT32 totalSize =0;
				UINT32 used = 0 ;
				UINT32 sdHealth = 0 ;
				appInfoDISKINFOGet( &totalSize , &used , &sdHealth);
				appInfoDiskInfoFB(totalSize , used , sdHealth,appInfoDiskStatus); // dummy , need to do 
			}
		break;
		case INFO_HEADERNUM_SDFORMAT:
			{
			UINT32 usedTime;
			BOOL bFormat_Success = TRUE;
			queryAvailableSpace_t setting;
			
			usedTime = jetSDUseTimeRead();
			appStateChange(APP_STATE_MENU,STATE_PARAM_MENU_POS_SET);
			if(appInfoSDFormat()) {
				appInfoSDFormatOKFB();  // CID 040 - format success
			}
			else {
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "Fail to format SD Card!\n");
				appInfoSDFormatFB(FALSE);  // CID 005, AVN will stay "formatting" screen until timeout.
				bFormat_Success = FALSE;   // Confirmed with Kadono San, Do not send CID-040 if formatting failure.
			}
			
			jetSDUseTimeInit();
			jetSDUseTimeWrite(usedTime);
			if (bFormat_Success) {
				status = jetQueryAvailableSpaceGet(&setting);
				appInfoQueryAvailableSpaceFB(status, setting);
			}

			appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
			appInfoPanaStateSet(PANA_STATE_PREVIEW);
			}
		break;
		case INFO_HEADERNUM_SETSNAPMARK:
			{
			
			UINT8 type=0, ret=0; char filename[32];
			SnapState err=SnapState_NoERROR;
			queryAvailableSpace_t setting;
			jetSnapShotMarkParseString((char *)buf, &type, filename);
			if(type)
				JetCdfsJPGSetFilename(TRUE , filename);
			ret = jetSnapShotMarkSet(0,NULL,&err);
			appInfoSnapShotMarkFB(ret,(UINT8)err);
			ret = jetQueryAvailableSpaceGet(&setting);
			appInfoQueryAvailableSpaceFB(ret, setting);
			}
			break;
		case INFO_HEADERNUM_DISKERRINFO:
			appInfoDiskErrorFB(appInfoDiskStatus); // dummy , need to do 
		break;
		case INFO_HEADERNUM_STARTUSRTRIGGEREVENT:
			{
			queryAvailableSpace_t setting;
			UINT8 status;			
			temp = appInfoUserRecStateCheck();
			if(temp == USERREC_FB_NOERR)
			{
				temp = appInfoUserRecStateCheck();
				if( temp == USERREC_FB_DISKFULL){
					infoFuncMoveFileState = MOVEFILE_NONE;
					CopyPath[0] = '\0';
					appInfoUdatHeaderPESet(appInfoUdatHeaderPEGet() & (~0x0002));
					appInfoPanaStateSet(PANA_STATE_RECORDING);
				}else{				
					appInfoPanaStateSet(PANA_STATE_USER_RECORDING);
					appInfoPositionEventStampSet("2");
					appInfoPositionEventSet("2");
					appInfoUdatHeaderPosEventSet("2");
				}
			}
			else{
				infoFuncMoveFileState = MOVEFILE_NONE;
				jetErrStorageState(ERROR_SAVE_SD_USERFULL);
			}
			appInfoStartUserRecordingFB(temp);
			status = jetQueryAvailableSpaceGet(&setting);
			appInfoQueryAvailableSpaceFB(status, setting);
			}
		break;
		case INFO_HEADERNUM_FILECOUNT:
			{
			UINT16 filter=0, count=0; UINT8 folder=0;
			
			jetQueryFileCountParseString((char *)buf, &filter, &folder);
			jetQueryReset();
			status = jetQueryFileCountGet(filter, folder, &count);
			jet_file_list_filter_num_set(count);
			appInfoQueryFileCountFB(status, count);
			}
			break;
		case INFO_HEADERNUM_FILELIST:
			{
			UINT16 filter=0, count=0, index=0, total = 0;
			UINT8 folder=0;
			
			jetFileListParseString((char *)buf, &filter, &folder, &index, &count);
			status = jetQueryFileCountGet(filter, folder, &total);
			jet_file_list_filter_num_set(total);
			appInfoFileListFB(index,count);
			}
			break;
		case INFO_HEADERNUM_FILEINFO:
			{
			UINT16 filter=0, index=0; UINT8 folder=0;
			
			jetQueryFileInfoParseString((char *)buf, &filter, &folder, &index);
			appInfoQueryFileInfoFB(index);
			}
			break;
		case INFO_HEADERNUM_FILEDEL:
#if 1
			{
				AVN_FILE_DELETE_PARA_T FileDelete;
				
				memset(&FileDelete, 0, sizeof(FileDelete));
				jetFileDeleteParseString((char *)buf, &FileDelete);
				FileDelete.total_count = jetFolderFilterTotal();
				jet_avn_file_delete_para_set(FileDelete);
				if(FileDelete.file_index > FileDelete.total_count){
					FileDelete.file_index = 1;
				}
				jet_appSeamlessDelFileEvent(EVENT_DEL_FILE_BY_AVN);
			}
#endif
			break;
		case INFO_HEADERNUM_FILEPLAYBAKSTART:
			{
#if 1
			AVN_FILE_PB_PARA_T para, *t;
			UINT16 total;
			if(jetFilePBStartParseString((char *)buf, &para) == FAIL){
				appFilePBStartFB(0);
				break;
			}
			if(STATE_GROUP(appActiveStateGet())==APP_STATE_GROUP_PLAYBACK) {
				t = jet_avn_pb_para_get();
				para.filter = t->filter;
				para.folder = t->folder;
				para.file_index = jet_file_list_filter_current_play_get() - 1;
			}
			status = jetQueryFileCountGet(para.filter, para.folder, &total);
			if (status == 0 || total == 0) {
				appFilePBStartFB(0);
				break;
			}
			jet_file_list_filter_num_set(total);
			para.file_index %= total;
			para.file_index++;
			jet_avn_pb_para_set(&para);
			printf("%s(%d) %u of %u\n", __func__, __LINE__, para.file_index, total);
			if(STATE_GROUP(appActiveStateGet())==APP_STATE_GROUP_PLAYBACK && appNextStateGet()==APP_STATE_NULL){
				if(jet_avn_pb_para_get()->mode_branch == AVN_PB_PARA_MODE_BRANCH_SKIP_NEXT 
					|| jet_avn_pb_para_get()->mode_branch == AVN_PB_PARA_MODE_BRANCH_SKIP_PREVIOUS){
					if(appActiveStateGet()==APP_STATE_PB_MAIN){/*photo*/
						sp5kHostMsgSend(APP_UI_MSG_PB_SHIFT_NEXT,0);
					}else{
						sp5kHostMsgSend(JET_APP_UI_MSG_PB_VIDEO_NEXT,0);
					}
				}else{
					jet_file_list_filter_current_play_set(para.file_index);
					if(appActiveStateGet()==APP_STATE_PB_MAIN){/*photo*/
						sp5kHostMsgSend(APP_UI_MSG_PB_SHIFT_NEXT,1/*keep current file*/);//photo
					}else{
						if(jet_avn_pb_para_get()->speed == AVN_PB_PARA_SPEED_X4){ /* 4X speed*/
							sp5kHostMsgSend(APP_KEY_PRESS_RIGHT,0); 
						}else if(jet_avn_pb_para_get()->speed == AVN_PB_PARA_SPEED_X1){ /* normal speed*/
							if(appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_PAUSE
								|| appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_FF
								|| appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_REW){
								sp5kHostMsgSend(APP_KEY_PRESS_UP,0);
							}else if(appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_PLAY_STARTING
								||appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_PLAYING){
								appFilePBStartFB(0);
							}else{
								sp5kHostMsgSend(JET_APP_UI_MSG_PB_VIDEO_NEXT,0);
							}
						}else{
							appFilePBStartFB(0);
						}
					}
				}
			}else{
				if(para.filter == 1024){
					pUiSetting->pbfileType = PB_FILETYPE_IMAGE;
				}else{
					pUiSetting->pbfileType = PB_FILETYPE_VIDEO;
				}
				jet_file_list_filter_current_play_set(para.file_index);
				appStateChange(APP_STATE_PB_MAIN,STATE_PARAM_NORMAL_INIT);
			}
#endif
			}
			break;
		case INFO_HEADERNUM_FILEPLAYBAKSTOP:
			
			if(appActiveStateGet()==APP_STATE_PB_MAIN || appActiveStateGet()==APP_STATE_PB_VIDEO){
				sp5kHostMsgSend(JET_APP_UI_MSG_PB_STOP,0);
			}else{
				appFilePBStopFB(0, jet_avn_pb_para_get(), jet_file_list_filter_current_play_get()-1);
			}
			//jet_pbvideo_stclose(); move to APP_STATE_MSG_CLOSE
			break;
		case INFO_HEADERNUM_FILEPLAYBAKPAUSE:
			
			if(appActiveStateGet()==APP_STATE_PB_VIDEO){
				
				if(appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_PLAYING){
					sp5kHostMsgSend(APP_KEY_PRESS_UP,0);
					
					break;
				}
			}
			appFilePBPauseFB(0, jet_avn_pb_para_get());
			break;
		case INFO_HEADERNUM_SETRES :
			
			if(appInfoPanaStateGet() != PANA_STATE_PREVIEW)
			{
				appInfoSetResFB( appInfoPanaStateGet() );// dummy , need to do 
				break;
			}
			
			appInfoSetRes((UINT8 *)payload);

			appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
			appInfoPanaStateSet(PANA_STATE_PREVIEW);
			

			appInfoSetResFB(appInfoPanaStateGet() );
		break;
		case INFO_HEADERNUM_SETFACTORYREST:

			if(appInfoPanaStateGet() != PANA_STATE_PREVIEW)
			{
				if(appActiveStateGet() == APP_STATE_VIDEO_REC){
					appVideoStop();
				}
				else{
					
					appInfoFactoryResetFB(FALSE);
					break;
				}
			}
		
			appInfoFactoryReset();
			appStateChange(APP_STATE_MENU,STATE_PARAM_MENU_POS_SET);
			appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
			appInfoFactoryResetFB(TRUE);
			
		break;
		case INFO_HEADERNUM_SDNUMTIME:
			
			status = (IS_CARD_EXIST)? 1:0;
			appInfoSDUseTimeFB(status, jetSDUseTimeRead());
		break;
		case INFO_HEADERNUM_SCANSD:
		break;
		case INFO_HEADERNUM_SDFORMATNECESSITY :
			appInfoSDCardFormatNecessityFB(); // dummy , need to do 
		break;
		case INFO_HEADERNUM_HITEMPPROTECTED:
			status = jetTempGet(&temperature);
			TEMPstate = status;
			appInfoHighTempProtectedFB(TEMPstate, temperature);
			if(TEMPstate == TEMP_HIGH)
				jetErrStorageState(ERROR_SAVE_TEMP_HIGH);
		break;
		case INFO_HEADERNUM_ACAILABLESPACE:
			{
			queryAvailableSpace_t setting;
			
			status = jetQueryAvailableSpaceGet(&setting);
			appInfoQueryAvailableSpaceFB(status, setting);
			}
		break;
		case INFO_HEADERNUM_WRITEENDURANCE:
			{
			queryWriteEdurance_t edu;
			
			jet_query_write_endurance_get(&edu);
			appInfoWriteEndUranceFB(&edu);
			}
			break;
		case INFO_HEADERNUM_STOPUSRTRIGGEREVENT:
			{
				if( appInfoPanaStateGet() == PANA_STATE_USER_RECORDING){
				infoFuncMoveFileState = MOVEFILE_WAIT_VIDEOFINISH;
				appInfoStopUserRecordingFB(TRUE);
			}
			else{
				infoFuncMoveFileState = MOVEFILE_NONE;
				appInfoStopUserRecordingFB(FALSE);
			}
		   	appInfoPanaStateSet(PANA_STATE_RECORDING);
			}

		break;
		case INFO_HEADERNUM_MOVEFILESVAEFOLDER:
			{
			UINT16 filter=0, file_index=0, total;
			UINT8 ret, err;
			
			jetMoveFileToSaveFolderParseString((char *)buf,&filter, &file_index);
			status = jetQueryFileCountGet(filter, 0, &total);
			if (status == 0) {
				/* Can not build file index return fail, and file name error */
				appInfoMoveFileToSaveFolderFB(0, 2);
			} else {
				jet_file_list_filter_num_set(total);

				ret = jetMoveFileHandle(file_index, &err);
				appInfoMoveFileToSaveFolderFB(ret, err);
				queryAvailableSpace_t setting;
				status = jetQueryAvailableSpaceGet(&setting);
				appInfoQueryAvailableSpaceFB(status, setting);
			}
			}
			break;

		case INFO_HEADERNUM_SETGSENSOR:
			state = appInfoGSensorSetFunc(payload);
			appInfoSetGSensorFB(state);   
			break;

		case INFO_HEADERNUM_SETGSENSORCALIBRATION:
			state = appInfoGSensorCompensation();
			appInfoGSensorCompensationFB(state);
			break;

		case INFO_HEADERNUM_SETBEEPSOUND:
			appInfoSetBeepSound((UINT8 *)payload);
			appInfoSetBeepSoundFB((UINT8 *)payload);
			break;

		case INFO_HEADERNUM_SETRECORDER:
			state = appInfoSetRecorderSound(payload);
			appInfoSetRecorderSoundFB(state);
			break;

		case INFO_HEADERNUM_SETPARKMODE:
			
			appInfoSetParkingMode((UINT8 *)payload);
			appInfoParkingModeFB(appInfoParkingModeTriStateGet());
			appInfoParkingModeTriStateSet(ParkingModeTigger_NONE);
		break;
		case INFO_HEADERNUM_SETASSISTMODE:
			
			jetSmartAssitModeParseString((char *)buf,&setting);
			status = (setting)? jetSmartAssitModeInit():jetSmartAssitModeDestroy();
			appInfoSmartAssitModeFB(status);
			break;
		case INFO_HEADERNUM_SDLOGSET:
			
			jetSDLogParseString((char *)buf,&setting);
			appInfoSDLogFB(jetSDLogSet(setting));
			break;
		case INFO_HEADERNUM_GETSETTINGVALUE:
		    appInfoDevSetting();
		    break;
		case INFO_HEADERNUM_FORMATOK:
		case INFO_HEADERNUM_ANNOUNCECONNECTDEV:
		case INFO_HEADERNUM_REGULARCOM:
			break;
		case INFO_HEADERNUM_ERRORSTATUSINFO:
			appInfoReportErrStatusFB();
			break;
		case INFO_HEADERNUM_SDPLUGINOUT:
		case INFO_HEADERNUM_REQPOSITIONMSG:
		case INFO_HEADERNUM_PLAYPOSITIONMSG:
			/*Do nothing These command DVR is sender not recevice*/
		break;

		default:
		    break;

	}
	return TRUE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoMSGHandler 
* Function		: Handling information messages
* Return value	:  BOOL 
* Parameter1	: UINT8 *buf
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL appInfoMSGHandler(UINT8 *buf)
{
    UINT id;
    char *payload;

    if (strncmp((char *) buf, "$DMSG", 5))
        return FALSE;

    buf += 5; // shift to command ID
    id = strtoul((char *) buf, &payload, 10);
    payload++;  // shift to payload

	switch(id)
	{
		case INFO_HEADERNUM_DEVINFO:
			break;
		case INFO_HEADERNUM_DISKERRINFO:
			break;
		case INFO_HEADERNUM_FILECOUNT:
			break;
		case INFO_HEADERNUM_FILELIST:
			break;
		case INFO_HEADERNUM_FILEINFO:
			break;
		case INFO_HEADERNUM_FILEDEL:
			break;
		case INFO_HEADERNUM_FILEPLAYBAKSTART:
			break;
		case INFO_HEADERNUM_FILEPLAYBAKSTOP:
			break;
		case INFO_HEADERNUM_FILEPLAYBAKPAUSE:
			break;
		case INFO_HEADERNUM_SETRES :
			break;
		case INFO_HEADERNUM_SETFACTORYREST:
			break;
		case INFO_HEADERNUM_SDNUMTIME:
			break;
		case INFO_HEADERNUM_SCANSD:
			break;
		case INFO_HEADERNUM_SDFORMATNECESSITY :
			break;
		case INFO_HEADERNUM_HITEMPPROTECTED:
			break;
		case INFO_HEADERNUM_SDPLUGINOUT:
			SDCARD_TimerCount= 0xFFFFFFFF;
			printf("%s INFO_HEADERNUM_SDPLUGINOUT\n" , __FUNCTION__);
			break;
		case INFO_HEADERNUM_ACAILABLESPACE:
			break;
		case INFO_HEADERNUM_WRITEENDURANCE:
			break;
		case INFO_HEADERNUM_STOPUSRTRIGGEREVENT:
			break;
		case INFO_HEADERNUM_MOVEFILESVAEFOLDER:
			break;
		case INFO_HEADERNUM_FORMATOK:
			break;
		case INFO_HEADERNUM_ANNOUNCECONNECTDEV:
			Regular_TimerCount  = 0;	
			PosReq_TimerCount =0;
			//sc removed
			//PlayPos_TimerCount =0;
			ConnectDev_TimeOutCount=0;
			ConnectDev_TimerCount=0xFFFFFFFF;
			WaitInitSeqFinish = TRUE;
			NewAVNModel = FALSE;
/*			sp5kTimerIsrOneShotReg(0 , 60000 ,appInfoWaitInitSeqFinishCB);*/
			break;
		case INFO_HEADERNUM_REGULARCOM:
			Regular_TimeOutCount = 0;
			break;
		case INFO_HEADERNUM_ERRORSTATUSINFO:
			printf("%s\tINFO_HEADERNUM_ERRORSTATUSINFO\n", __func__);
			break;
		case INFO_HEADERNUM_SETGSENSOR:
			break;
		case INFO_HEADERNUM_SETGSENSORCALIBRATION:
			break;
		case INFO_HEADERNUM_SETPARKMODE:
			break;
		case INFO_HEADERNUM_SETASSISTMODE:
			break;
		case INFO_HEADERNUM_REQPOSITIONMSG:
			appInfoPositionSet((char * )payload);
			PosReq_TimeOutCount = 0;
			break;
		case INFO_HEADERNUM_PLAYPOSITIONMSG:
			break;

		default:
		    break;
	}
	return TRUE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoHandler 
* Function		: main function of information hander
* Return value	:  void 
* Parameter1	: UINT8 *buf
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoHandler(UINT8 *buf)
{
    if (appInfoCMDHandler(buf) == TRUE) {
#if 0
        printf("appInfoHandler  INFO_HEADER_CMD %s\n ",buf);
#endif
    } else if (appInfoMSGHandler(buf) == TRUE) {
#if 0
        printf("appInfoHandler  INFO_HEADER_MSG %s\n ",buf);
#endif
    } else {
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG,"appInfoHandler  INFO_HEADER_ERR %s\n ",buf);
    }
}


static BOOL bParkingMode_WakeUp = FALSE;
/*--------------------------------------------------------------------------*
* Function name : appInfoBuzzerCtrl 
* Function		: information buzzer control
* Return value	:  void 
* Parameter1	: BuzzerCTRLState state
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoBuzzerCtrl(BuzzerCTRLState state)
{
	UINT32 ms =0 , count = 0 , delayTimes = 0;
	
	printf("%s %d\n" , __FUNCTION__ , state);
	//HOST_PROF_LOG_PRINT(LEVEL_WARN, "%s:%d: buzz state=%d", __FILE__, __LINE__, state);
	//HOST_PROF_LOG_PRINT(LEVEL_WARN, "%s:%d: appInfoPowerDETStateGet=%d\n", __FILE__, __LINE__, appInfoPowerDETStateGet());

	if (bParkingMode_WakeUp == TRUE) {
    	//HOST_PROF_LOG_PRINT(LEVEL_WARN, "jssn ...... %s:%d: state=%d\n", __FILE__, __LINE__, state);
    	//delayTimes = 5000;
    	bParkingMode_WakeUp = FALSE;
    	return;
	}
	
	/*
	AppBuzzerOff();
	AppBuzzerFlickOFF();*/
	switch(state)
	{
		case BUZZER_CTRL_NONE:
			ms = 0;
			count = 0;
		break;
		case BUZZER_CTRL_POWERON:
			if((pUiSetting->buzzerSet&BUZZER_SETTING_BOOT_ONOFF)){
				ms = 100;
				count = 1;
			}
		break;
		case BUZZER_CTRL_GSENSOR:
			if((pUiSetting->buzzerSet&BUZZER_SETTING_WARNING_ONOFF)){
				ms = 100;
				count = 8;
			}
		break;
		case BUZZER_CTRL_UPDATE:
			ms = 100;
			count = 2;
		break;
		case BUZZER_CTRL_UPDATEFINISH:
			ms = 500;
			count = 1;
		break;	
		case BUZZER_CTRL_UPDATEERR:
			ms = 300;
			count = 4;
			break;
		case BUZZER_CTRL_DISKERR:
			ms = 500;
			count = 4;
		break;
		case BUZZER_CTRL_OTHERR:
			ms = 300;
			count = 4;
		break;

		default:
		    break;

	}
	if(delayTimes > 0 )
		sp5kTimeDelay(SP5K_TIME_DELAY_1MS , delayTimes);

	if(ms != 0 && count != 0&&appBuzzerReady() )
		AppBuzzerFlick( ms  , count);
/*	else
		AppBuzzerFlickOFF();*/
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDiskErrShowCtrl 
* Function		: information handler about showing disk error
* Return value	:  void 
* Parameter1	: DiskStatus state
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoDiskErrShowCtrl(DiskStatus state)
{
	UINT32 err;

    //HOST_PROF_LOG_PRINT(LEVEL_WARN, "%s:%d: state=%d", __FILE__, __LINE__, state);
    
	if (bParkingMode_WakeUp == TRUE) {
    	//HOST_PROF_LOG_PRINT(LEVEL_WARN, "%s:%d: state=%d", __FILE__, __LINE__, state);
    	bParkingMode_WakeUp = FALSE;
    	return;
	}
	
	switch(state)
	{
		case DISK_NORMAL :
			if(SDBootupCheck==FALSE &&appActiveDiskGet() ==SP5K_DRIVE_SD&&appPreviousStateGet() != APP_STATE_POWER_ON)//Isaac reopen it and use flag to cover the boot up issue
			{	
				printf("%s  preState 0x%x State 0x%x nextstate 0x%x Pstate 0x%x \n " ,__FUNCTION__ , appPreviousStateGet() , appActiveStateGet() , appNextStateGet() , appInfoPanaStateGet());
				ALL_LED_OFF;  // Jason: Comment this to avoid GREEN LED OFF upon DVR Boot.
			}
			GREEN_LED_FLICKER(1000);
			break;

		case DISK_FILE_DAMAGE :
		case DISK_SIZE_LACKING :
		case DISK_FULL_USER :
			LED_NORMAL_ON;
			break;
		case DISK_FORMAT_ERROR :
		case DISK_FORMAT_WARNING :
		case DISK_FORMAT_BREAK :
		case DISK_SIZE_FAIL :
		case DISK_FULL  :
		case DISK_ERROR  :
		case DISK_NONE :
			GREEN_LED_OFF;
			RED_LED_ON;
			  err = sp5kHostMsgSend(INFO_QUEUEMSG_BUZZERONOFF, BUZZER_CTRL_DISKERR);
   			 HOST_ASSERT(err == SP5K_SUCCESS);
			break;

		default:
		    break;	
		
	}

}


/*--------------------------------------------------------------------------*
* Function name : appInfoDiskGoBackToDo 
* Function		: information handler about go back sequence
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoDiskGoBackToDo(void)
{
	/* protect from reentry */
	static BOOL locker = FALSE;

	if (locker == TRUE)
		return;

	locker = TRUE;
	if((SDBootupCheck == FALSE) && IS_CARD_EXIST)
	{
		if ((Regular_TimeOutCount != 0 || ConnectDev_TimeOutCount != 0) &&
				appInfoPanaStateGet() == PANA_STATE_PREVIEW && appInfoDiskSizeCheck()) {
				printf("%s Auto REC \n",__FUNCTION__);
				sp5kHostMsgSend(APP_KEY_PRESS_S2, 0);

				/* Auto REC happended on dis-connected */
#if 0
				appInfoModeChangeFB( Success_RECORDING , PANA_STATE_RECORDING);
#endif
		}
	}
	locker = FALSE;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoFWUpgarde 
* Function		: support Normal mode and Factory mode(SPHOSTF.BRN)
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.10.16
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoFWUpgarde(void)
{
    UINT32 fd = 0;
    UINT32 fSize = 0;
//    UINT32 err;
    
    static struct {
	    UINT8 *filename;
	    BOOL version_check;
	    BOOL buzzer;
	    BOOL poweroff;
    } firmwares[] = {
	    {(UINT8 *)"D:\\SPHOSTF.BRN", FALSE, FALSE, TRUE},
	    {(UINT8 *)"D:\\SPHOST.BRN", TRUE, TRUE, FALSE},
    };

	// Keep the same behavior of the previous variable <SDBootupCheck>
	// to avoid side effects.
	if (SDBootupCheck && appInfoPowerDETStateGet() == POWER_DET_ACC_ON) {
		SDBootupCheck = FALSE;
	}

	if (SDBootup_UpgCheck && appInfoPowerDETStateGet() == POWER_DET_ACC_ON) {
		SDBootup_UpgCheck = FALSE;
	int i;
	for (i = 0; i < SIZE(firmwares); i++) {
		if (firmwares[i].version_check)
			if (appInfoFWUPDateVerGet(firmwares[i].filename) == FALSE)
				continue;
		fd = sp5kFsFileOpen(firmwares[i].filename, SP5K_FS_OPEN_RDONLY);
		if (fd == 0)
			continue;

		fSize = fsFileSizeGet(fd);
		fsFileClose(fd);

		printf("%s %s(%d)\n", __FUNCTION__, firmwares[i].filename, fSize);
		appUart2Stop(TRUE);
		JetPowerCanOff(FALSE);
		sp5kModeSet(SP5K_MODE_STANDBY);
		sp5kModeWait(SP5K_MODE_STANDBY);
		sp5kAeModeSet(SP5K_AE_MODE_OFF);
		sp5kAwbModeSet(SP5K_AWB_MODE_OFF);


		if (firmwares[i].poweroff) {
		    HOST_PROF_LOG_PRINT(LEVEL_WARN, "%s:%d: appInfoFWUpgarde, poweroff", __FILE__, __LINE__);
			appInfoSWWDTWakeup(0);  // Need to disable WDT to avoid reboot.
		}

		// VUP - upgrade
		if (appFirmwareUpdate(firmwares[i].filename, firmwares[i].buzzer) != SUCCESS)
			continue;

	} // END for loop
        printf("update File not exist !!!\n");

        if (appInfoPowerDETStateGet() == POWER_DET_ACC_ON) {
            printf("parse JET_BOOTUP.CMD\n");
            sp5kCmdExec("jetcmd d:/JET_BOOTUP.CMD");
        }

    }
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDiskInfoGet 
* Function		: information handler about getting disk information
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*			   : 2018.0426   Paul  Chang "modfiy bug  #3445 
*		    				 add SD Free size check & set appInfoDiskStatus"
*			   : 2018.0601   Paul Chang "modify bug #3671
*						 add check DISK FULL after the appCdfsFreeSpaceCheck"
*--------------------------------------------------------------------------*/
void appInfoDiskInfoGet(void)
{
    //UINT32 hwVer = 0xffffffff;
    UINT32 FreeSizekBytes = 0;
    UINT32 err =0;
   //UINT32 mode = 0;
    //appRTC_t local;

    if(appActiveDiskGet() !=SP5K_DRIVE_SD&&SDBootupCheck==FALSE)
		return;
    if (IS_CARD_EXIST) {
        if (appDiskMountReady() == TRUE) {
            appInfoDiskStatus = DISK_NORMAL;
            SDActiveState = SD_STATE_IN;
            /*  when the power on auto update  */

            // Jason: Also judge as unusable by the SDXC Certifications.
            if ((appInfoDiskSizeCheck() == FALSE) || (appInfoSD_CertCheck() == FALSE)) {
                SDActiveState = SD_STATE_IN;
                appInfoDiskStatus = DISK_SIZE_FAIL;
                /*sp5kDiskUnMount( SP5K_DRIVE_SD);*/
                jetErrStorageState(ERROR_SAVE_SD_CANNOTUSE);
		
            }else
                appSDLogMsg();

	    	sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_FREE_SIZE_BY_KBYTE, &FreeSizekBytes);

		if(  ( (FreeSizekBytes)  < 102400) && (appInfoPowerDETStateGet()  == POWER_DET_PARKING_WAKEUP)) {
			printf("%s Free Size %d\n" ,__FUNCTION__ ,FreeSizekBytes);
			appInfoDiskStatus = DISK_FULL;
			appInfoDiskErrShowCtrl(DISK_FULL);
			if(appBuzzerReady())
				err = sp5kHostMsgSend(INFO_QUEUEMSG_POWER, POWER_DET_ACC_OFF);
			else
				sp5kTimerIsrOneShotReg(0, appBuzzerReadyWaitTime(), appInfoParkingLowPowerCB);

		}
		/*paul modfiy bug  #3445 & #3671*/
		if (FreeSizekBytes  < 102400&&appInfoDiskSizeCheck()
			&&(appCdfsFreeSpaceCheck(CDFS_FILE_FOLDER_VIDEO) == FAIL)) {
	        	printf("%s ===Free Size %d\n" ,__FUNCTION__ ,FreeSizekBytes);
	                appInfoDiskStatus = DISK_FULL;
	                
	        }	
        }

	
		
        if (appDiskMountFailed() == TRUE) {
            SDActiveState = SD_STATE_MOUNTFAIL;
            appInfoDiskStatus = DISK_ERROR;
        }
    } else {
        appInfoDiskStatus = DISK_NONE;
        SDActiveState = SD_STATE_OUT;
        SDBootup_UpgCheck = FALSE;  // Avoid later inserting SD Card to trigger VUP.
    } // END if (IS_CARD_EXIST)

    if ((SD_STATE_MAX > SDActiveState) ) {
        SDCARD_TimerCount = 0;
        appInfoSDInOutFB(SDActiveState);
    }

   if((appInfoPanaStateGet()  == PANA_STATE_PLAYING||appInfoPanaStateGet()  == PANA_STATE_PLAY_IDLE)&& !IS_CARD_EXIST){
	  appInfoDiskErrorFB(appInfoDiskStatus);
	  appInfoDiskErrShowCtrl(appInfoDiskStatus);
	  appInfoPanaStateSet(PANA_STATE_PLAY_IDLE);
	  appInfoRegularFB(PANA_STATE_PLAY_IDLE);
	  return;
   }

	// Jason fix redmine_2721: Unplug SDCard after wake-up from parking mode.
	PowerDetect sd_pwrState = appInfoPowerDETStateGet();
	if ((sd_pwrState == POWER_DET_ACC_ON ) || (sd_pwrState == POWER_DET_PARKING_WAKEUP)) {
		printf("%s:%d appInfoDiskStatus  0x%x\n" , __FUNCTION__, __LINE__, appInfoDiskStatus);
		appInfoDiskErrShowCtrl(appInfoDiskStatus);
		appInfoDiskErrorFB(appInfoDiskStatus);
		if (sd_pwrState == POWER_DET_PARKING_WAKEUP) {
			// Jason: Fix redmine_2733, turn off power after wake up from parking mode.
			if ((!IS_CARD_EXIST) || (appInfoDiskStatus == DISK_SIZE_FAIL)) {
				if(appBuzzerReady())
					err = sp5kHostMsgSend(INFO_QUEUEMSG_POWER, POWER_DET_ACC_OFF);
				else
					sp5kTimerIsrOneShotReg(0, appBuzzerReadyWaitTime(), appInfoParkingLowPowerCB);
					
				HOST_ASSERT(err == SP5K_SUCCESS);
			}
		}
	}

    if (appInfoDiskStatus == DISK_NORMAL) {
        if (Regular_TimeOutCount >REGULAR_TIMEOUT || ConnectDev_TimeOutCount >REGULAR_TIMEOUT) {
		if(appInfoPanaStateGet() == PANA_STATE_PREVIEW&&appActiveStateGet() != APP_STATE_JET_POWER_OFF&&IS_CARD_EXIST&& appInfoTemperaturStateGet()!=TEMP_HIGH)
            		sp5kTimerIsrOneShotReg(0, 500, appInfoDiskGoBackToDo);
        }
    }
#if 0
    printf("%s %d %d\n" , __FUNCTION__ , appInfoDiskStatus ,appInfoDiskStatus);
#endif
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDiskStateChangeHandeler 
* Function		: information handler about disk state change
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoDiskStateChangeHandeler(void)
{
	appInfoDiskInfoGet();
}


/*--------------------------------------------------------------------------*
* Function name : appInfoStateChangeHandeler 
* Function		: information handler about information state change
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoStateChangeHandeler(void)
{
	appInfoiStateToPState(appActiveStateGet());
	printf("appInfoStateChangeHandeler activeState state 0x%x\n",appActiveStateGet());
}


/*--------------------------------------------------------------------------*
* Function name : appInfoGetMoveFileState 
* Function		: information handler about get file movie state
* Return value	:  MoveFileState 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
MoveFileState appInfoGetMoveFileState(void)
{
	return infoFuncMoveFileState;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoFinishFileNameset 
* Function		: information handler about get finish file name
* Return value	:  void 
* Parameter1	: UINT8 * buf
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoFinishFileNameset(UINT8 * buf)
{
	sprintf((char *)finishFileName,"%s",buf);
}


#if 0
/*--------------------------------------------------------------------------*
* Function name : appInfoRECStateGet 
* Function		: information handler about get rec state
* Return value	: RECState 
* Parameter1	: VOID
* History	   	: 2017.08.21
*			   	: 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
RECState appInfoRECStateGet(void){

	if(strcmp(CopyPath, USER_RECORD_PATH)==0)
		return REC_STATE_USER;
	if(strcmp(CopyPath, EVENT_PATH)==0)
		return REC_STATE_EVENT;
	return REC_STATE_NORMAL;
}
#endif



#if 0
/*--------------------------------------------------------------------------*
* Function name : appinfoMoveFileStateInit 
* Function		: information handler about initalize move file state
* Return value	: VOID 
* Parameter1	: VOID
* History	   	: 2017.08.21
*			   	: 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appinfoMoveFileStateInit(void){
	UINT8 lastFileName[FILENAME_SIZE]={0};
	UINT8 dstFileName[FILENAME_SIZE]={0};
	UINT8 dstFileNameTemp[FILENAME_SIZE]={0};
	UINT8 Path[FILENAME_SIZE]={0};
	UINT8 finalFileName[64]={0};
	UINT8 str[5] = {0};
	UINT16 event = 0;
	UINT32 fd = 0 ;
	tmx_t atime , mtime , ctime;
	printf("[%s] finishFileName = %s\n", __FUNCTION__,finishFileName);
#if 0
	sprintf((char *)lastFileName,"%s",appInfoFileNameGet(finishFileName));
	sprintf((char *)dstFileNameTemp,"%s",lastFileName);
	
	appInfoPathGet(finishFileName , Path);

	jet_file_udata_event_get((char *)finishFileName, &event);
	
	if(appInfoRECStateGet() == REC_STATE_USER)
	{
		infoFuncMoveFileState = MOVEFILE_STILL;
		event= event|BIT1;

	}
	else if(appInfoRECStateGet() == REC_STATE_EVENT)
	{
		infoFuncMoveFileState = MOVEFILE_WAIT_VIDEOFINISH;
		event = event|BIT3;

	}
	
	
	sp5kFsFileTimeGet(finishFileName , &atime , &mtime , &ctime);
	
	fd = sp5kFsFileOpen((const UINT8*)finishFileName, FS_OPEN_RDWR);
	
	if(jet_file_udata_tag_seek(fd)==FAIL){
		fsFileClose(fd);
	}
	else{
		sprintf(str , "%04d," , event);
		fsFileWrite(fd, str, 5);
		fsFileClose(fd);
	}

	sp5kFsFileTimeSet(finishFileName , &atime , &mtime , &ctime);
	sprintf((char *)dstFileName,"%s",appinfoMoveFileNameGet(CopyPath ,dstFileNameTemp ));
	appInfoFileReName(Path  , CopyPath ,  lastFileName , dstFileName);
	sprintf((char *)finalFileName,"%s/%s",CopyPath,dstFileName);
	

	printf("appFileExistCheck   %s   %s    %d\n"   , __FUNCTION__,finalFileName,appFileExistCheck(finalFileName));
	jet_appCdfsFileDel(lastFileName ,jet_cdfs_pathname_to_cdfs(Path));
	appCdfsFileSync(finalFileName, 0);
	strcpy(finishFileName, finalFileName);
	fileReady = FALSE;
#endif
}
#endif

#if 0
 /*--------------------------------------------------------------------------*
* Function name : appinfoMoveFileState 
* Function		: move file state
* Return value	: VOID 
* Parameter1	: VOID
* History	    : 2018.04.11  Weikai Hsu "Added Function Header"
*		
*--------------------------------------------------------------------------*/
void appinfoMoveFileState(void)
{
	UINT8 lastFileName[FILENAME_SIZE]={0};
	UINT8 dstFileName[FILENAME_SIZE]={0};
	UINT8 dstFileNameTemp[FILENAME_SIZE]={0};
	UINT8 Path[FILENAME_SIZE]={0};
	UINT8 finalFileName[64]={0};
	sprintf((char *)lastFileName,"%s",appInfoFileNameGet(finishFileName));
	sprintf((char *)dstFileNameTemp,"%s",lastFileName);
	appInfoPathGet(finishFileName , Path);
	sprintf((char *)dstFileName,"%s",appinfoMoveFileNameGet(CopyPath ,dstFileNameTemp ));
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s %s %s\n" , __FUNCTION__ , lastFileName , finishFileName);
#if 0
	switch(infoFuncMoveFileState)
	{

		case MOVEFILE_STILL:/*still  move and rename file*/		
		case MOVEFILE_WAIT_VIDEOFINISH:/*Wait file ready and move file*/			
		case MOVEFILE_FINISH:/*to move and rename the last video file , */
			
			if(fileReady)
			{
				
				appInfoFileReName(Path  , CopyPath ,  lastFileName , dstFileName);
				if(infoFuncMoveFileState == MOVEFILE_WAIT_VIDEOFINISH)
				{
					infoFuncMoveFileState = MOVEFILE_FINISH;
					break;
				}
				if(infoFuncMoveFileState == MOVEFILE_FINISH)
				{
					infoFuncMoveFileState = MOVEFILE_NONE;
					
					break;
					
				}
			}
			
			break;

	}
	
	sprintf((char *)finalFileName,"%s/%s",CopyPath,dstFileName);
	printf("appFileExistCheck   %s   %s    %d\n"   , __FUNCTION__,finalFileName,appFileExistCheck(finalFileName));
	jet_appCdfsFileDel(lastFileName ,jet_cdfs_pathname_to_cdfs(Path));
	appCdfsFileSync(finalFileName, 0);
	
	fileReady = FALSE;
#endif
}
#endif


/*--------------------------------------------------------------------------*
* Function name : appInfoPOSReqHandler 
* Function		: request handler about pos request timeout
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPOSReqHandler(void)
{
	if((appActiveStateGet()==APP_STATE_PB_VIDEO || appActiveStateGet()==APP_STATE_PB_MAIN)){
		return;
	}
	
	appInfoPOSReqFB();
	if(appInfoPowerDETStateGet() == POWER_DET_ACC_ON) {
		PosReq_TimeOutCount++;
		if (PosReq_TimeOutCount >= COMUNCATION_TIMEOUT){
			appinfoPosReqTimeoutClear();
		}
	}

	PosReq_TimerCount = 0;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPlayPOSHandler 
* Function		: request handler about play pos
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPlayPOSHandler(void)
{
	FILE_UDATA_T *pMP4Udata = NULL, JPGUdata;
	CDFS_FILE_TAB_T *pCur;
	//tmx_t atime, mtime, ctime;
	tmx_t ctime;
	UINT16 file_index = 0;
	UINT8 play_status = 0;	
	UINT32 filetype = SP5K_DCF_FILETYPE_JPG;
	pbImgCb_t *pimgCb = NULL;

	if(PlayPos_TimeOutCount < COMUNCATION_TIMEOUT){
		PlayPos_TimeOutCount++;
		//printf("appInfoPlayPOSHandler \n");
	}
	else{
		//printf("appInfoPlayPOSHandler to do timeout function \n");
	}
	if((appActiveStateGet()==APP_STATE_PB_VIDEO || appActiveStateGet()==APP_STATE_PB_MAIN)){
		if(!IS_CARD_EXIST)
			return;
#if 0
		pList =jet_file_list_filter_get();
#endif
		file_index = jet_file_list_filter_current_play_get();
		pCur = jetFolderFilterFileNodeFind(file_index);
		if (pCur == NULL) {
			HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s(%d) pCur %p\n", __func__, __LINE__, pCur);
			return;
		}

		filetype = pCur->FileType;
		if(filetype == SP5K_DCF_FILETYPE_JPG){
			char filename[64];
			char tmpfilename[64];
			
			pimgCb = appPb_ImageCbPtrGet();
			memset(&JPGUdata,0, sizeof(FILE_UDATA_T));
			strcpy(filename, jet_cdfs_folder_to_path(pCur->folder));
			sprintf(tmpfilename,"%s/%s",filename, pCur->FileName);
			sprintf(filename,"%s",tmpfilename);
			jet_exif_datetime_get((UINT8 *)filename,&ctime);
			sprintf(JPGUdata.gpsTime,"%02d%02d%02d.000", ctime.tmx_hour, ctime.tmx_min, ctime.tmx_sec);
			sprintf(JPGUdata.gpsDate,"%02d%02d%02d", ctime.tmx_mday, ctime.tmx_mon, ctime.tmx_year-2000);
			if(pimgCb!=NULL){
				if(pimgCb->pExifInfo!=NULL){
					strcpy(JPGUdata.gpsLat,(char *)pimgCb->pExifInfo->latitude);
					strcpy(JPGUdata.gpsLong,(char *)pimgCb->pExifInfo->longitude);
					strcpy(JPGUdata.gpsNSIndicator, (char *)pimgCb->pExifInfo->latitudeRef);
					strcpy(JPGUdata.gpsEWIndicator, (char *)pimgCb->pExifInfo->longitudeRef);
					strcpy(JPGUdata.gpsSpeed, (char *)pimgCb->pExifInfo->speed);
					JPGUdata.gpsAngle=atoi((char *)pimgCb->pExifInfo->direction);
				}
			}
			appInfoPOSPlayFB(&JPGUdata, file_index, 0);
		}else if(filetype == SP5K_DCF_FILETYPE_MP4 && appActiveStateGet()==APP_STATE_PB_VIDEO){  
			int line_count = 0;
			pMP4Udata=jet_file_udata_info_handle_get();
			line_count=jet_file_udata_info_line_num_get();
			if(pMP4Udata == NULL){
				printf("[%s] pUdata is null\n",__FUNCTION__);
				return;
			}
			if(line_count==0){
				printf("[%s] line_count is 0\n",__FUNCTION__);
				return;
			}
			if(appPb_MainCbPtrGet()==NULL){
				printf("[%s] appPb_MainCbPtrGet is null\n",__FUNCTION__);
				return;
			}
			if(appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_STANDBY){
				play_status = 0; //stop
			}else if(appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_PLAY_STARTING
				||appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_PLAYING){
				play_status = 1; //playing
			}else if(appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_PAUSE){
				play_status = 2; //pause
			}else if(appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_FF){
				play_status = 3; //fast forward
			}else if(appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_REW){
				play_status = 4; //rewind
			}
			if(appPb_MainCbPtrGet()->subStat != PB_VIDEO_STAT_STANDBY){
				int elap_time_sec = 0;
				elap_time_sec = appPbVideoElapTimeGet()/1000;
				#if 0
				if(appPbVideoElapTimeGet()%1000 > 500){
					elap_time_sec ++;
				}
				#endif
				if(elap_time_sec >= line_count){
					elap_time_sec = line_count-1;
				}
				printf("[%s] line_count = %d, gElapTime = %d, elap_time_sec = %d\n", __FUNCTION__,line_count,
					appPbVideoElapTimeGet(), elap_time_sec);
				appInfoPOSPlayFB(pMP4Udata+elap_time_sec, file_index, play_status);
			}
		}
	}
}


/*--------------------------------------------------------------------------*
* Function name : appInfoConnectDevHandler 
* Function		: information handler about connected devices
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoConnectDevHandler(void)
{

	if(ConnectDev_TimeOutCount < COMUNCATION_TIMEOUT)
	{
		appInfoConnectDevFB();
		ConnectDev_TimerCount = 0;
		ConnectDev_TimeOutCount++;
		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoConnectDEVHandler connect device retry \n");
	}
	else if(ConnectDev_TimeOutCount == 0xFFFFFFFF)
	{
		ConnectDev_TimerCount = 0xFFFFFFFF ;
		ConnectDev_TimeOutCount = 0;
		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoConnectDEVHandler already connected \n");
		return;
	}
	else
	{
		appInfoConnectDevFB();
		ConnectDev_TimerCount = 0;
		ConnectDev_TimeOutCount++;
		#if 0
			printf("appInfoConnectDEVHandler to do timeout function \n");
		#endif
	}

	if (ConnectDev_TimeOutCount >= REGULAR_TIMEOUT &&
			(appActiveStateGet() != APP_STATE_JET_POWER_OFF || appActiveStateGet() != APP_STATE_POWER_OFF) &&
			IS_CARD_EXIST&& appInfoTemperaturStateGet()!=TEMP_HIGH) {
		sp5kTimerIsrOneShotReg(0, 500, appInfoDiskGoBackToDo);
	}
}


/*--------------------------------------------------------------------------*
* Function name : appInfoRegularHandler 
* Function		: information handler about regular state information
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoRegularHandler(void)
{
	if(appInfoPowerDETStateGet() == POWER_DET_ACC_ON)
		Regular_TimeOutCount++;
	Regular_TimerCount = 0;
	appInfoRegularFB(ActiveState);
	
	if(spiWriteCount != 0xFF){
		
		if(spiWriteCount > 0 ){
			spiWriteCount--;
		}
		else{
			spiWriteCount = 0xff;
			appUserSettingSave(pUiSetting);
		}
	}
	if(Regular_TimeOutCount == REGULAR_TIMEOUT &&
			(appActiveStateGet() != APP_STATE_JET_POWER_OFF || appActiveStateGet() != APP_STATE_POWER_OFF) &&
			IS_CARD_EXIST && appInfoTemperaturStateGet()!=TEMP_HIGH){
		if(IS_PLAYBACK_GROUP(appActiveStateGet())){
			autoRecRetry = 10;
			sp5kTimerIsrOneShotReg(0, 50, appInfoVideoAutoRecRetryCB);
		}
		else{
			sp5kTimerIsrOneShotReg(0, 500, appInfoDiskGoBackToDo);
		}
	}
	if(Regular_TimeOutCount == COMUNCATION_TIMEOUT && appInfoPowerDETStateGet() == POWER_DET_ACC_ON &&
			(appInfoPanaStateGet() == PANA_STATE_USER_RECORDING || appInfoPanaStateGet() == PANA_STATE_RECORDING)){
		appinfoPosReqTimeoutClear();
		if( appInfoPanaStateGet() == PANA_STATE_USER_RECORDING){
			printf("user recording : communication time out %s (%d)\n", __func__, __LINE__);
			infoFuncMoveFileState = MOVEFILE_WAIT_VIDEOFINISH;
#if 0
			CopyPath[0] = '\0';
			appInfoUdatHeaderPESet(appInfoUdatHeaderPEGet() & (~0x0002));
#endif
			appInfoPanaStateSet(PANA_STATE_RECORDING);
		}
	}
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPowerDETStateGet 
* Function		: information handler about get power detect state
* Return value	:  PowerDetect 
* Parameter1	: void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
PowerDetect appInfoPowerDETStateGet(void){

	return powerDetect;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPowerDETStateSet 
* Function		: information handler about set power detect state
* Return value	:  void 
* Parameter1	: PowerDetect state
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPowerDETStateSet(PowerDetect state){

	 powerDetect  = state;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPowerDetectFuncion 
* Function		: information handler about power detect state
* Return value	:  void 
* Parameter1	: PowerDetect state
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPowerDetectFuncion(PowerDetect state)
{
    
    appInfoPowerDETStateSet(state);
    switch (state) {
    case POWER_DET_ACC_OFF_SIGNAL:/*need go back */
	appVideoStop();
        appStateChange(APP_STATE_JET_POWER_OFF, STATE_PARAM_NORMAL_INIT);
        break;
    case POWER_DET_ACC_ON:
	printf("ACC on\n");
        appInfSWWDT(); /* Set RTC wackup */
        break;
    case POWER_DET_ACC_OFF:
        //HOST_PROF_LOG_PRINT(LEVEL_WARN, "%s:%d: POWER_DET_ACC_OFF", __FILE__, __LINE__);
	appVideoStop();
        if(appBuzzerReady()) {
            sp5kTimerIsrOneShotReg(0, 500, appInfoParkingLowPowerCB);
        }
        else {
            sp5kTimerIsrOneShotReg(0, appBuzzerReadyWaitTime(), appInfoParkingLowPowerCB);
        }
        break;
    case POWER_DET_PARKING_WAKEUP:
        //HOST_PROF_LOG_PRINT(LEVEL_WARN, "%s:%d: POWER_DET_PARKING_WAKEUP", __FILE__, __LINE__);
        /*if (appInfoBatteryLvGet() < appInfoParkingBatterySettingGet()) {
            printf("%s   Low power  %d %d\n", __FUNCTION__,
                    appInfoBatteryLvGet(), appInfoParkingBatterySettingGet());

            appGsensorLevelCtrl(3, 4);
            appInfoParkingModeTriStateSet(ParkingModeTigger_LOWPOWER);
            sp5kTimerIsrOneShotReg(0, 500, appInfoParkingLowPowerCB);
        } else 
	*/
	if (!IS_CARD_EXIST) {
    		//UINT32 err;

            //HOST_PROF_LOG_PRINT(LEVEL_WARN, "%s:%d: trace.", __FILE__, __LINE__);
            bParkingMode_WakeUp = TRUE;
    		appInfoDiskStatus = DISK_NONE;
    		appInfoDiskErrShowCtrl(DISK_NONE);
    		// Jason: Fix redmine_2733 by avoiding power off after wake up from parking mode.
    		//err = sp5kHostMsgSend(INFO_QUEUEMSG_POWER, POWER_DET_ACC_OFF);
    		//HOST_ASSERT(err == SP5K_SUCCESS);
    	} else {
    		//UINT32 err;
    		printf("%s appInfoDiskStatus 0x%x" ,__FUNCTION__ ,appInfoDiskStatus);

		/* Disable G sensor */
            	//appGsensorLevelCtrl(1, 3);
    		PARKINGMODE_Count = appInfoParkingReCordingTimeGet() / 60;
    		appUart2Stop(TRUE);
    		appInfoPanaStateSet(PANA_STATE_PARKING_RECORDING);
    		appInfoPositionEventStampSet("16");
    		appInfoPositionEventSet("16");
    		appInfoUdatHeaderPosEventSet("16");

    		FirstTimeBoot = FALSE;
    		
        }
        break;
    default:
		break;
    } // END switch

}


/*--------------------------------------------------------------------------*
* Function name : appInfoQueueMsgHandler 
* Function		: information handler about message in queue
* Return value	:  UINT32 
* Parameter1	: UINT32 *msg
* Parameter2	: UINT32 *param
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoQueueMsgHandler(UINT32 *msg, UINT32 *param)
{
    /* default message is handled */
    UINT32 ret = SP5K_HOST_MSG_CALLBACK_RET_SKIP;
    UINT32 err;
    //UINT8 status=0;
    UINT32 BP;
	switch(*msg)
	{
#if 0
		case INFO_QUEUEMSG_STATECHANGE:
			{
				queryAvailableSpace_t setting;
				
				if(appPreviousStateGet()  == APP_STATE_CARD_PLUG&& appActiveStateGet() == APP_STATE_VIDEO_PREVIEW&&IS_CARD_EXIST){
					status = jetQueryAvailableSpaceGet(&setting);
					appInfoQueryAvailableSpaceFB(status, setting);
					HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoCMDHandler   INFO_HEADERNUM_ACAILABLESPACE \n" );
				}
				appInfoStateChangeHandeler();
			}
			break;

		case INFO_QUEUEMSG_VIDEOCALLBACK:
			printf("%s(%d) INFO_QUEUEMSG_VIDEOCALLBACK\n", __func__, __LINE__);
			if(appInfoPowerDETStateGet()== POWER_DET_PARKING_WAKEUP )
			{
				if(PARKINGMODE_Count > 0)
					PARKINGMODE_Count--;
				appInfoParkingModeCtrl( PARKINGMODE_Count ,finishFileName);
				break;
			}
#if 0
			jet_lastfile_rename_by_rtc_changed();
			appInfoFileNameEventAdd(appCdfsLastFileNameGet());	
					
			sprintf((char *)finishFileName,"%s",appCdfsLastFileNameGet());
			printf("appInfoHandlerThread   INFO_QUEUEMSG_VIDEOCALLBACK  %s\n" , finishFileName);

			
			fileReady = TRUE;

			if(infoFuncMoveFileState != MOVEFILE_NONE){
				appinfoMoveFileState();
			}
 			if(appInfoPanaStateGet()==PANA_STATE_USER_RECORDING){
				if(appInfoUserRecStateCheck() == USERREC_FB_DISKFULL){
					infoFuncMoveFileState = MOVEFILE_NONE;
					appInfoUdatHeaderPESet(appInfoUdatHeaderPEGet() & (~0x0002));
					appInfoPanaStateSet(PANA_STATE_RECORDING);
					appInfoDiskErrorFB(DISK_FULL_USER);
				}
			}
			/* FIXME: otignal code */
#if 0
			if(appInfoUdatHeaderPEGet() & 0x0008){//GEVENT
				jet_cdfs_oldest_file_delete_by_folder(CDFS_FILE_FOLDER_EVENT);
			}
#endif
#endif
			break;

		case INFO_QUEUEMSG_UARTDATAREV:
			appInfoHandler((UINT8*)*param);
			break;
#endif
		case INFO_QUEUEMSG_EVENTTIGGER:
			gpioRead(SP5K_GPIO_GRP_FML ,BIT1, &BP);
			if (((appInfoPanaStateGet() != PANA_STATE_RECORDING) &&
					(appInfoPanaStateGet() != PANA_STATE_USER_RECORDING)) ||
					(appInfoPowerDETStateGet() != POWER_DET_ACC_ON) || BP == 0)
				break;
			
			err = sp5kHostMsgSend(INFO_QUEUEMSG_BUZZERONOFF, BUZZER_CTRL_GSENSOR);
	   		HOST_ASSERT(err == SP5K_SUCCESS);

			appInfoPositionEventStampSet("8");
			appInfoPositionEventSet("8");
			appInfoUdatHeaderPosEventSet("8");

			if(ActiveState != PANA_STATE_RECORDING)
			{
				HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "INFO_QUEUEMSG_EVENTTIGGER    %d\n",ActiveState);
	
				break;
			}
		
			 queryAvailableSpace_t setting;
			 UINT8 status;
			 status = jetQueryAvailableSpaceGet(&setting);
			 appInfoQueryAvailableSpaceFB(status, setting);			 

			break;
#if 0
		case INFO_QUEUEMSG_REGULAR:
			/*For CID 045 */
			if(Regular_TimerCount == 0xffffffff)
				break;
			else
				appInfoRegularHandler();
			break;

		case INFO_QUEUEMSG_CONNECTDEV:
			/*For CID 044*/
			if(ConnectDev_TimerCount == 0xffffffff)
				break;
			else
				appInfoConnectDevHandler();
			break;

		case INFO_QUEUEMSG_POSREQUEST:
			/*For CID 100 */
			if(PosReq_TimerCount == 0xffffffff)
				break;
			else{
				appInfoPOSReqHandler();

			}
			break;
#endif
#if 0
		case INFO_QUEUEMSG_PLAYPOS:
			/*For CID 101*/
			if(PlayPos_TimerCount == 0xffffffff)
				break;
			else{
				err = sp5kOsEventFlagsSet(&videoPos_sig, 1, TX_INT_ENABLE);
        			HOST_ASSERT(err == SP5K_SUCCESS);
			}
			break;
#endif
		case INFO_QUEUEMSG_DISKSTATE:
			appInfoDiskStateChangeHandeler();
			break;
#if 0
		case INFO_QUEUEMSG_SDPLUGTIMER:
			if(SDCARD_TimerCount == 0xFFFFFFFF)
				break;
			else
				appInfoSDInOutFB(SDActiveState);
			break;
#endif
		case INFO_QUEUEMSG_BUZZERONOFF:
				appInfoBuzzerCtrl(*param);
			break;
		case INFO_QUEUEMSG_POWER:
			appInfoPowerDetectFuncion( (PowerDetect ) *param);
			break;
#if 0
		case INFO_QUEUEMSG_PREAVNE:
			appInfoChangePreFileName(*param);
			break;
#endif
        default:
          	ret = 0;  // the message is not handled
          	break;
	}
	
	return ret;
}
#if 0
/*--------------------------------------------------------------------------*
* Function name : appInfoHandlerThread 
* Function		: information handler thread
* Return value	: VOID 
* Parameter1	: ULONG param
* History	    : 2018.04.11  Weikai Hsu "Added Function Header"
*		
*--------------------------------------------------------------------------*/
void appInfoHandlerThread(ULONG param)
{
	/*
	 *  XXX: this code just run one time when appInfoPanaStateSet(state)
	 *       Maybe reduce this to one shot function
	 */

	ULONG flags;
	UINT32 err;


	while(1)
	{
			
		err = sp5kOsEventFlagsGet(&InfoHandler_sig, 1, TX_OR_CLEAR, &flags, TX_WAIT_FOREVER);
		HOST_ASSERT(err == SP5K_SUCCESS);

		if (FirstTimeBoot) {
			printf("%s PANA_STATE_RECORDING  power state %d\n" , __FUNCTION__ , appInfoPowerDETStateGet());
			if(!IS_CARD_EXIST){
				printf("No SD card\n");
				err = sp5kHostMsgSend(INFO_QUEUEMSG_DISKSTATE, 0);
				HOST_ASSERT(err == SP5K_SUCCESS);
				FirstTimeBoot =FALSE;
				if(appInfoPowerDETStateGet()== POWER_DET_PARKING_WAKEUP){
					err = sp5kHostMsgSend(INFO_QUEUEMSG_POWER, POWER_DET_ACC_OFF);
					HOST_ASSERT(err == SP5K_SUCCESS);
				}
				appInfoConnectDevFB();
				ConnectDev_TimerCount = 0;
			}		
			
			if(appInfoPanaStateGet() == PANA_STATE_RECORDING){
				if(appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP)
					appInfoPanaStateSet(PANA_STATE_PARKING_RECORDING);
				else{
					FirstTimeBoot =FALSE;
					appInfoConnectDevFB();
					ConnectDev_TimerCount = 0;
					appInfoPanaStateSet(PANA_STATE_RECORDING);
				}
			}
		}
		

	}
	
}
#endif

#if 0
/*--------------------------------------------------------------------------*
* Function name : appVideoPosEventThread 
* Function		: Video pos event thread
* Return value	: VOID 
* Parameter1	: ULONG param
* History	    : 2018.04.11  Weikai Hsu "Added Function Header"
*		
*--------------------------------------------------------------------------*/
void appVideoPosEventThread(ULONG param)
{
    ULONG flags;
    UINT32 err;

    err = sp5kOsEventFlagsCreate(&videoPos_sig, "Video Pos Event");

    while (1) {
        err = sp5kOsEventFlagsGet(&videoPos_sig, 1, TX_OR_CLEAR, &flags,
                TX_WAIT_FOREVER);
        HOST_ASSERT(err == SP5K_SUCCESS);
	appInfoPlayPOSHandler();
    }
}
#endif


/*--------------------------------------------------------------------------*
* Function name : appQEventThread 
* Function		: queue the event thread
* Return value	:  void 
* Parameter1	: ULONG param
* History	   : 2018.04.03
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appQEventThread(ULONG param)
{
    ULONG flags;
    UINT32 err;

    err = sp5kOsEventFlagsCreate(&qEvent_sig, "Queue Msg event");

    while (1) {
        err = sp5kOsEventFlagsGet(&qEvent_sig, INFO_QUEUEMSG_THREAD_MASK,
                TX_OR_CLEAR, &flags,
                TX_WAIT_FOREVER);
        HOST_ASSERT(err == SP5K_SUCCESS);

        if (flags & INFO_QUEUEMSG_THREAD_PLAYPOS) {
		appInfoPlayPOSHandler();
        }

        if (flags & INFO_QUEUEMSG_THREAD_POSREQUEST) {
            /*For CID 100 */
            if (PosReq_TimerCount != 0xffffffff)
                appInfoPOSReqHandler();
        }

        if (flags & INFO_QUEUEMSG_THREAD_CONNECTDEV) {
            /*For CID 044*/
            if (ConnectDev_TimerCount != 0xffffffff)
                appInfoConnectDevHandler();
        }

        if (flags & INFO_QUEUEMSG_THREAD_REGULAR) {
            /*For CID 045 */
            if (Regular_TimerCount != 0xffffffff) {
                appInfoRegularHandler();
                jet_filelist_delete_status_check();
#if 0
                jet_usersave_movefile_status_check();
#endif
            }
        }

        if (flags & INFO_QUEUEMSG_THREAD_SDPLUGTIMER) {
            if (SDCARD_TimerCount != 0xFFFFFFFF)
                appInfoSDInOutFB(SDActiveState);
        }

        if (flags & INFO_QUEUEMSG_THREAD_STATECHANGE) {
            queryAvailableSpace_t setting;
            UINT8 status = 0;
            if (appPreviousStateGet() == APP_STATE_CARD_PLUG &&
                    appActiveStateGet() == APP_STATE_VIDEO_PREVIEW &&
                    IS_CARD_EXIST) {
                status = jetQueryAvailableSpaceGet(&setting);
                appInfoQueryAvailableSpaceFB(status, setting);
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG,
                        "appInfoCMDHandler   INFO_HEADERNUM_ACAILABLESPACE \n");
            }
            appInfoStateChangeHandeler();
        }

#if DEBUG_ON
        printf("%s event 0x%x\n", __FUNCTION__, flags);
#endif

    }
}


SP5K_MUTEX feedbackMutex;
/*--------------------------------------------------------------------------*
* Function name : AppInfoHandlerInit 
* Function		: Information handler about initalize
* Return value	: VOID 
* Parameter1	: VOID
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoHandlerInit(void)
{
#if 0
    SP5K_THREAD *ptrdInfoHandler;
#endif
    sp5kOsMutexCreate(&feedbackMutex, "info handler feedback", TX_INHERIT);

#if 0
    ptrdInfoHandler = sp5kOsThreadCreate("POS play",
            appVideoPosEventThread, 0,
            APP_VIDEO_POS_THREAD_PRIORITY,
            APP_VIDEO_POS_THREAD_PRIORITY,
            0,
            TX_AUTO_START);
#endif
    sp5kOsThreadCreate("Queue Msg event Ctrl",
            appQEventThread, 0,
            APP_QUEUEMSG_EVENT_THREAD_PRIORITY,
            APP_QUEUEMSG_EVENT_THREAD_PRIORITY,
            0,
            TX_AUTO_START);
#if 0
    UINT32 err;
    SP5K_THREAD *ptrdInfoHandler;

    err = sp5kOsEventFlagsCreate(&InfoHandler_sig, "infohandlerApp");
    HOST_ASSERT(err == SP5K_SUCCESS);

    ptrdInfoHandler = sp5kOsThreadCreate("thrAppInfoHandler",
            appInfoHandlerThread, 0,
            INFOHANDLER_INIT_THREAD_PRIORITY,
            INFOHANDLER_INIT_THREAD_PRIORITY,
            INFOHANDLER_INIT_THREAD_TIME_SLICE,
            TX_AUTO_START);
    HOST_ASSERT(ptrdInfoHandler);
#endif
    sp5kHostMsgFilterCallbackSet(appInfoQueueMsgHandler);

   HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoHandlerInit finish\n");
}


/*--------------------------------------------------------------------------*
* Function name : appInfoConnStart 
* Function		: Initial subsystem for AVN communication that include UART2 initial, infoApp stack
* Return value	:  void 
* Parameter1	: BOOL force
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoConnStart(BOOL force)
{
    if (FirstTimeBoot || force) {
        printf("%s PANA_STATE_RECORDING  power state %d\n", __FUNCTION__,
                appInfoPowerDETStateGet());
        FirstTimeBoot = FALSE;

#if defined(PRJ_JETDVR) || defined( PRJ_SBC) || defined( PRJ_17DVR)|| defined( PRJ_19DVR)/*Add for UART data recdvicer and handler command to do  Isaac 20170322 */
        appUart2Init();
#endif
        if (!IS_CARD_EXIST) {
            UINT32 err;

            err = sp5kHostMsgSend(INFO_QUEUEMSG_DISKSTATE, 0);
            HOST_ASSERT(err == SP5K_SUCCESS);
        }
        /* flag of allow updating RTC when AVN providing GPS */
        /* start AVN connection */
        ConnectDev_TimerCount = 0;

#if 0
        if(appInfoPanaStateGet() == PANA_STATE_RECORDING) {
            if(appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP)
            appInfoPanaStateSet(PANA_STATE_PARKING_RECORDING);
            else {
                FirstTimeBoot =FALSE;
                appInfoPanaStateSet(PANA_STATE_RECORDING);
            }
        }
#endif
    }
}


/*--------------------------------------------------------------------------*
* Function name : appInfoiStateToPState 
* Function		: information of Panastate state handling
* Return value	:  void 
* Parameter1	: UINT32 iState
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoiStateToPState(UINT32 iState)
{

	PanaState ret = appInfoPanaStateGet();
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "$ [%s] iState = %d, PanaState = %d\n", __FUNCTION__, iState, ret);
	switch(iState)
	{

		case APP_STATE_POWER_ON:
			sp5kTimerIsrOneShotReg( 0 , 60000, appInfoTemperaturDetectCB);
			ret = PANA_STATE_INIT;
			break;
		case APP_STATE_POWER_OFF:
			break;
		case APP_STATE_JET_POWER_OFF:
			ret = PANA_STATE_PREVIEW;
			break;
		case APP_STATE_VIDEO_PREVIEW:
		/*MENU GROUP*/
		case APP_STATE_MENU:
		case APP_STATE_MENU_SYSTEM:			
			ret = PANA_STATE_PREVIEW;
			break;
		case APP_STATE_VIDEO_REC:
			if (ActiveState == PANA_STATE_USER_RECORDING)
				ret = ActiveState;
			else
				ret = PANA_STATE_RECORDING;
			break;
		/* playback group */
		case APP_STATE_PB_MAIN:
			ret = PANA_STATE_PLAY_IDLE;
			break;
		case APP_STATE_PB_VIDEO:
			if(appPb_MainCbPtrGet()!=NULL){
				if(appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_PLAY_STARTING
					||appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_PLAYING
					||appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_FF
					||appPb_MainCbPtrGet()->subStat == PB_VIDEO_STAT_REW){
					ret = PANA_STATE_PLAYING;
				}else{
					ret = PANA_STATE_PLAY_IDLE;
				}
			}else{
				ret = PANA_STATE_PLAY_IDLE;
			}
			break;
		case APP_STATE_PB_IMG_PROT:
			break;
		case APP_STATE_PB_IMG_DEL:
			break;
				/* CARD PLUG */
		case APP_STATE_CARD_PLUG:
			break;
		case APP_STATE_NULL:
		case APP_STATE_POWER_ON_SET_LANGUAGE:
		case APP_STATE_POWER_HIBERNATE  :
		/* still view group */
		case APP_STATE_STILL_PREVIEW:
		case APP_STATE_STILL_CAPTURE:
		case APP_STATE_BURST_CAPTURE:
		case APP_STATE_STILL_AAA:
		case APP_STATE_STILL_BUSY:
		/* video group */
		case APP_STATE_VIDEO_AAA :
		/* audio group */
		case APP_STATE_AUDIO_REC:

		/* USB */
		case APP_STATE_USB_CREATE:
		case APP_STATE_USB_MSDC:
		case APP_STATE_USB_PCCAM:
		case APP_STATE_USB_BATTERY_CHARGING:

		/* CALIB */
		case APP_STATE_CALIBRATION:
		case APP_STATE_CALIB_ZOOM:
		case APP_STATE_CALIB_AWB:
		/* TEST */
		case APP_STATE_TEST_MAIN:
		case APP_STATE_TEST_CAMCFG	:
		case APP_STATE_TEST_UNIT_TEST :
		case APP_STATE_TEST_BAT_CURVE :
		/* Customer */
		case APP_STATE_CUSTOMER :
		case APP_STATE_CUSTOMER_VIDEO_PREVIEW :
		case APP_STATE_CUSTOMER_VIDEO_REC :
			/*Do nothing*/
			break;
		/* TV PLUG */
		case APP_STATE_TV_PLUG:
			/* It is ready to power off */
			JetPowerCanOff(TRUE);
			//JetQueueBusySet(0);
			if (IS_CARD_EXIST == 0)
				appInfoConnStart(FALSE);
			break;
			
		default:
		    break;	
	}

	appInfoPanaStateSet(ret);
	if(gIsModeChange == TRUE && ret == gTargetState){
		appInfoModeChangeFB(STATUS_NONE, ret);
		gTargetState = PANA_STATE_MAX;
		gIsModeChange = FALSE;
#if 0
		appInfoRegularHandler();
		Regular_TimerCount = 0;
#endif
	}

}


/*--------------------------------------------------------------------------*
* Function name : appInfoPanaStateSet 
* Function		: information of set Panastate state
* Return value	:  void 
* Parameter1	: PanaState state
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoPanaStateSet(PanaState state)
{
#if 0
    UINT32 err;
#endif
	ActiveState = state;
#if 0
    err = sp5kOsEventFlagsSet(&InfoHandler_sig, 1, TX_INT_ENABLE);
    HOST_ASSERT(err == SP5K_SUCCESS);
#endif
}


 /*--------------------------------------------------------------------------*
 * Function name : appInfoPanaStateGet 
 * Function 	 : information of get Panastate state
 * Return value  :	PanaState 
 * Parameter1	 : void
 * History		: 2017.08.21
 *				: 2018.04.11  Weikai Hsu "Added Function Header"
 *--------------------------------------------------------------------------*/
PanaState appInfoPanaStateGet(void)
{
	return ActiveState;
}


