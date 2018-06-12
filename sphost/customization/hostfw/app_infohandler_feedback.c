/*--------------------------------------------------------------------------*
 *	File name	: App_infohandler_feedback.c
 *	Function	: Info feedback to AVN
 *	First editor: Weikai Hsu
 *	First date	: 	
 *	History		: 	
 *	            : 2018.04.11  Weikai Hsu "Added File Header"
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

#include "api/sp5k_disk_api.h"

#include "app_infohandler.h"
#include "app_uart2.h"
#include "app_infohandler_func.h"
#include "jet_utility.h"



#define INT16 short

#define DEBUG_ON 1


#define INFO_FB_HEADER_LEN 9

#define INFO_FB_MODECHANGE_TOTALLEN 17
#define INFO_FB_MODECHANGE_DATALEN 12

#define INFO_FB_DEVFWVER_TOTALLEN 22
#define INFO_FB_DEVFWVER_DATALEN 17


#define INFO_FB_SDFORMAT_TOTALLEN 17
#define INFO_FB_SDFORMAT_DATALEN 12

#define INFO_FB_SDFORMATOK_TOTALLEN 13
#define INFO_FB_SDFORMATOK_DATALEN 8

#define INFO_FB_DISKINFO_TOTALLEN 25
#define INFO_FB_DISKINFO_DATALEN 8

#define INFO_FB_SETRES_TOTALLEN 15
#define INFO_FB_SETRES_DATALEN 10

#define INFO_FB_POSREQ_TOTALLEN 13
#define INFO_FB_POSREQ_DATALEN 8

#define INFO_FB_CONNECTDEV_TOTALLEN 15
#define INFO_FB_CONNECTDEV_DATALEN 10

#define INFO_FB_REGULAR_TOTALLEN 15
#define INFO_FB_REGULAR_DATALEN 10

#define INFO_FB_DISKERROR_TOTALLEN 18

#define INFO_FB_SDINFO_TOTALLEN 15

#define INFO_FB_FACTORYRESET_TOTALLEN 15

#define INFO_FB_SETRECORDERSOUND_TOTALLEN 15

#define INFO_FB_SETBEEPSOUND_TOTALLEN 17

#define INFO_FB_USERRECS_TOTALLEN 17

#define INFO_FB_STOPUSERREC_TOTALLEN 15

extern UINT32 appDiskSDMountReady(void);

extern UINT8 appInfoSDCardSlowGet(void);

extern DiskStatus appInfoDiskStatus;

extern SP5K_MUTEX feedbackMutex;

extern BOOL NewAVNModel;

extern BOOL gIsModeChange;


/*--------------------------------------------------------------------------*
* Function name : appInfoInttoChar 
* Function		: conver int format to char format 
* Return value	: UINT8 
* Parameter1	: UINT8 data
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 appInfoInttoChar(UINT8 data)
{
	UINT8 CharData[16] = {'0','1' ,'2' ,'3','4','5','6','7','8','9','A','B','C','D','E','F'};
	return CharData[data] ;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoChenkSumGet 
* Function		: get checksum function
* Return value	: UINT8 
* Parameter1	: UINT8 *buf
* Parameter2	: UINT32 len
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 appInfoChenkSumGet(UINT8 *buf, UINT32 len)
{
	UINT8 temp =0;
	UINT32 i = 0; 
	temp = buf[1];

	for( i = 2 ; i < len ; i++ )
		temp^=buf[i];

	return temp;
}


/*--------------------------------------------------------------------------*
* Function name : appCSumUart2Send 
* Function		: send checksum to uart
* Return value	: static UINT32 
* Parameter1	: UINT8 *buf
* Parameter2	: UINT32 len
* Parameter3	: BOOL debug
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT32 appCSumUart2Send(UINT8 *buf, UINT32 len, BOOL debug)
{
	UINT8 csum_buf[6];
	UINT8 csum;
	UINT32 ret = 0;
	UINT8 buf2[256];

	int i;
	memcpy(buf2, buf, sizeof(buf2));
	csum = appInfoChenkSumGet(buf2, len);
	i = snprintf((char *) csum_buf, sizeof(csum_buf), "*%02hhX\r\n", csum);
	if (debug) {
		if(len>=sizeof(buf2)){
			return ret;
		}
		buf2[len] = '\0';
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "%s%s", buf2, csum_buf);
	}
	sp5kOsMutexGet(&feedbackMutex, TX_WAIT_FOREVER);
	ret = Uart2Send(buf2, len);
	ret += Uart2Send(csum_buf, i);
	sp5kOsMutexPut(&feedbackMutex);

	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDevInfo 
* Function		: get device information function
* Return value	: void 
* Parameter1	: PanaState state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
void appInfoDevInfo(PanaState state)
{
	UINT8 header[INFO_FB_DISKERROR_TOTALLEN] = "$DMSG001,";
	UINT32 tempDatalen = 0;
	char st = '0';

	tempDatalen = 12;
	if(state   < PANA_STATE_MAX )
		st = '1';
	header[INFO_FB_HEADER_LEN] = st;
	header[INFO_FB_HEADER_LEN+1] = ',';
	header[INFO_FB_HEADER_LEN+2] =appInfoInttoChar(state); 

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoDevInfo\t");
#endif
	appCSumUart2Send(header , tempDatalen, DEBUG_ON);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDevSetting 
* Function		: set device information function
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoDevSetting()
{
    char header[42];
    int l;

    PanaState state;

    extern PanaState appInfoPanaStateGet();
    extern char *appGsensorAccPXYZ();

    state = appInfoPanaStateGet();
    appGsensorAccPXYZ();

    l = snprintf(header, sizeof(header), "$DMSG998,1,%1d,%s",
                state, appGsensorAccPXYZ());

#if DEBUG_ON
   HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoDevSetting\t");
#endif
    appCSumUart2Send((UINT8 *)header , l, DEBUG_ON);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoModeChangeFB 
* Function		: function for change mode
* Return value	: UINT32 
* Parameter1	: UINT8 state
* Parameter2	: PanaState status
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
UINT32 appInfoModeChangeFB( UINT8 state  , PanaState status)
{
	/* $DMSG002,<return state>,<Device Status>*hh<CR><LF>*/
	UINT32 ret = 0 ;
	UINT8 header[INFO_FB_MODECHANGE_TOTALLEN] = "$DMSG002,";


	gIsModeChange = FALSE;
	
	switch(state){
			case STATUS_NONE:
 			case Success_INIT:
			case Success_PREVIEW:
			case Success_RECORDING:
			case Success_PLAYIDLE:
			case Success_PLAYING:
				header[INFO_FB_HEADER_LEN] = '1';
				header[INFO_FB_HEADER_LEN+1] = ',';
				break;
			case Fail_INIT:
			case Fail_PREVIEW:
			case Fail_RECORDING:
			case Fail_PLAYIDLE:
			case Fail_PLAYING:
			default:
				header[INFO_FB_HEADER_LEN] = '0';
				header[INFO_FB_HEADER_LEN+1] = ',';
				break;
	}
	header[INFO_FB_HEADER_LEN+2] = '0' + status;
#if 1 /* DEBUG_ON */
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoModeChangeFB\t");	
#endif
	ret = appCSumUart2Send(header , INFO_FB_MODECHANGE_DATALEN, 1);

	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDevFWVerFB 
* Function		: function for get firmware version
* Return value	: UINT32 
* Parameter1	: void
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoDevFWVerFB( void )
{
	/* $DMSG003,<return state>,<firmware version>*hh<CR><LF>*/
	UINT32 ret = 0;
	int l;
	UINT8 header[INFO_FB_DEVFWVER_TOTALLEN] = {0};

	l = sprintf((char*)header,"$DMSG003,1,%02d.%03d",APP_MODELNAME,APP_VERSION);
#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_INFO, DEBUG_COLOR_RED "%s %s" DEBUG_COLOR_NORMAL , __func__, header);
#endif
	ret = appCSumUart2Send(header , l, 1);
	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSDFormatFB 
* Function		: function for get sd card format information
* Return value	: UINT32 
* Parameter1	: UINT32 state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
UINT32 appInfoSDFormatFB( UINT32 state )
{
	/* $DMSG005,<return state>,<Error Code>*hh<CR><LF>*/
	UINT32 ret = 0 ;
	UINT8 header[INFO_FB_SDFORMAT_TOTALLEN] = {0};

	sprintf((char*)header,"$DMSG%03d,%d,0", INFO_HEADERNUM_SDFORMAT, state);
#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoSDFormatFB\t");
#endif
	ret = appCSumUart2Send(header , INFO_FB_SDFORMAT_DATALEN, DEBUG_ON);
	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSDFormatOKFB 
* Function		: function for get sd card format ok information
* Return value	: UINT32 
* Parameter1	: void
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
UINT32 appInfoSDFormatOKFB( void )
{
	/* $DMSG040*hh<CR><LF>*/
	UINT32 ret = 0 ;
	UINT8 header[] = "$DMSG040,";
	
#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "appInfoSDFormatOKFB\t");
#endif
	ret = appCSumUart2Send(header , sizeof(header), DEBUG_ON);

	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSetResFB 
* Function		: function for get sd card format ok information
* Return value	: UINT32 
* Parameter1	: PanaState state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
UINT32 appInfoSetResFB( PanaState state )
{
	/* $DMSG020,<return state>*hh<CR><LF>*/
	UINT32 ret = 0 ;
	UINT8 header[INFO_FB_SETRES_TOTALLEN] = "$DMSG020,";
	if(state  >= PANA_STATE_MAX)
		header[INFO_FB_HEADER_LEN] = '0';
	else{

		if(state == PANA_STATE_INIT)
			header[INFO_FB_HEADER_LEN] = '2';
		else
			header[INFO_FB_HEADER_LEN] = '1';
	}
		

#if DEBUG_ON	
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoSDFormatFB\t");
#endif
	ret = appCSumUart2Send(header , INFO_FB_SETRES_DATALEN, DEBUG_ON);

	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDiskInfoFB 
* Function		: function for get disk information
* Return value	: UINT32 
* Parameter1	: UINT32 totalSize
* Parameter2	: UINT32 used
* Parameter3	: UINT32 sdhealth
* Parameter4	: DiskStatus state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
UINT32 appInfoDiskInfoFB( UINT32 totalSize, UINT32 used, UINT32 sdhealth, DiskStatus state)
{
/* $DMSG004,<return state>,<Disk Status>,<Capability>,<Usage>*/

	UINT8 header[32];
	int i;

	i = sprintf( (char*)header ,"$DMSG%03d,1,%d,%llu,%d",
	        INFO_HEADERNUM_DISKINFO,state,((UINT64)totalSize*1024*1024),used);

	#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s      %s\n" ,__FUNCTION__, header);	
	#endif
	return  appCSumUart2Send(header, i, DEBUG_ON);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPOSReqFB 
* Function		: function for get post request information
* Return value	: UINT32 
* Parameter1	: void
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoPOSReqFB(void)
{
	/*$DCMD100*/
	UINT32 ret = 0 ;
	UINT8 header[] = "$DCMD100";
	
#if 0
	printf("appInfoPOSReqFB\t");
#endif
	ret = appCSumUart2Send(header , sizeof(header), 0);

	return ret;

}
UINT16 event_simulation=0; // 0: Don't simulate, >0 :simulate


/*--------------------------------------------------------------------------*
* Function name : jet_pos_play_event_simulation_set 
* Function		: function for set event on posplay
* Return value	: void 
* Parameter1	: UINT16 event
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_pos_play_event_simulation_set(UINT16 event)// for test command
{
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] event_simulation = %d\n", __FUNCTION__, event_simulation);
	event_simulation = event;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoPOSPlayFB 
* Function		: function for postition play information
* Return value	: UINT32 
* Parameter1	: FILE_UDATA_T *pUdata
* Parameter2	: UINT16 file_index
* Parameter3	: UINT8 play_status
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 appInfoPOSPlayFB(FILE_UDATA_T *pUdata, UINT16 file_index, UINT8 play_status)
{
	//$DCMD101,< UTC Position Time >,<Date>,< Datum >,<Latitude>,<N/S Indicator>,
	//<Longitude>,<E/W Indicator>,<Speed>,<Angle>,<X-Axis>,<Y-Axis>,<Z-Axis>,<Events>,
	//<File index>,< Play Status >
	char str[128];
	UINT32 ret = 0 ;
	UINT16 event;
	int i;
	extern BOOL gIsUdataReset;
	extern UINT32 PlayPos_TimerCount;
	if(gIsUdataReset == TRUE){
		printf("udata is reset ...\n");
		return ret;
	}
	if(PlayPos_TimerCount == 0xFFFFFFFF){
		printf("PlayPos_TimerCount was stopped ...\n");
		return ret;
	}
	event = pUdata->event & 0xFFFE;
	/* PANA file index samll then CDFS index 1 */
	file_index--;
	if(event_simulation){
		event = event_simulation;
	}
	if(pUdata->gpsTime[0] == '\0' || pUdata->gpsDate[0] == '\0' || pUdata->gpsLat[0] == '\0'){
		printf("gpsTime or gpsDate or gpsLat is 0...\n");
		return ret;
	}
	i = snprintf(str, sizeof(str), "$DCMD%03d,%s,%s,%d,%s,%s,%s,%s,%s,%d,%d,%d,%d,%d,%d,%d", INFO_HEADERNUM_PLAYPOSITIONMSG,
		pUdata->gpsTime, pUdata->gpsDate, pUdata->gpsDatum, pUdata->gpsLat, pUdata->gpsNSIndicator,
		pUdata->gpsLong,pUdata->gpsEWIndicator,pUdata->gpsSpeed,pUdata->gpsAngle,pUdata->gsensorX,
		pUdata->gsensorY,pUdata->gsensorZ,event,file_index,play_status);
	
#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] %s(%d) \n",__FUNCTION__, str, i);
#endif
	ret = appCSumUart2Send((UINT8 *)str, i, 0);
	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoConnectDevFB 
* Function		: function for get connnedcted device information
* Return value	: UINT32 
* Parameter1	: void
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
UINT32 appInfoConnectDevFB(void)
{
	/*$DCMD044,<device>      1*/
	UINT32 ret = 0 ;
	UINT8 header[INFO_FB_CONNECTDEV_TOTALLEN] = "$DCMD044,1";
	
#if DEBUG_ON	
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoConnectDevFB\t");	
#endif
	ret = appCSumUart2Send(header, INFO_FB_CONNECTDEV_DATALEN, DEBUG_ON);

	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoRegularFB 
* Function		: function for get regular device status information
* Return value	: void 
* Parameter1	: PanaState state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
void appInfoRegularFB(PanaState state)
{

	/*$DCMD045,<Device Status >*/


	/*	Only Item 1~5 can get form iCatchtek state .the others need to do form function
		0: INIT
		1: PREVIEW
		2: RECORDING
		3: PLAY IDLE
		4: PLAYING
		5: USER_RECORDING
		6: PARKING_RECORDING
		7: FORMATTING
		8: UPDATING
		9: LOW_BATTERY
	*/

	UINT8 header[] = "$DCMD045,y";

	/* system not ready  */
	if (state >= PANA_STATE_MAX) {
		printf("Warning at %s:%d: state=%d.\n", __func__, __LINE__, state);
		return;
	}

	header[sizeof(header) - 2] = appInfoInttoChar(state);
#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoRegularFB\t");	
#endif
	appCSumUart2Send(header , sizeof(header) - 1, DEBUG_ON);

	return;
}


/*--------------------------------------------------------------------------*
* Function name : appInfoDiskErrorFB 
* Function		: function for get disk error status information
* Return value	: void 
* Parameter1	: DiskStatus state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoDiskErrorFB(DiskStatus state)
{
	UINT8 header[INFO_FB_DISKERROR_TOTALLEN] = {0};
	UINT32 len;
	char st = '0';


	if (gIsModeChange)
		return;

	if(state <= DISK_FULL_USER )
		st = '1';

	len = sprintf((char*)header, "$DMSG%03d,%c,%d", INFO_HEADERNUM_DISKERRINFO, st, state);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoDiskErrorFB      %s\n" , header);
#endif

	appCSumUart2Send(header, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoDiskErrorFB_WriteSlow 
* Function		: function for get disk error status information (writting slow)
* Return value	: void 
* Parameter1	: DiskStatus state
* History	    : 2018.04.03
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoDiskErrorFB_WriteSlow(DiskStatus state)
{
    static DiskStatus preStatus = DISK_FULL_USER;
	static UINT32 now, preTime = 0;

	UINT8 header[INFO_FB_DISKERROR_TOTALLEN] = {0};
	UINT32 len;
	char st = '0';

    now = sp5kOsTimeGet();
	if (state == preStatus && now <= (preTime + 1100)) {
		preTime = now;
		return;
	}
	preTime = now;
	preStatus = state;

	if(state <=DISK_FULL_USER )
		st = '1';

	len = sprintf((char*)header, "$DMSG%03d,%c,%d", INFO_HEADERNUM_DISKERRINFO, st, state);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoDiskErrorFB      %s\n" , header);
#endif

	appCSumUart2Send(header, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoSDInOutFB 
* Function		: function for get sd card in and out status information
* Return value	: void 
* Parameter1	: SDStatus state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSDInOutFB(SDStatus state)
{
	static SDStatus preStatus = SD_STATE_MAX;
	static UINT32 preTime = 0;

	UINT8 header[INFO_FB_DISKERROR_TOTALLEN] = {0};
	UINT32 len, now;

	now = sp5kOsTimeGet();
	if (state == preStatus && now <= (preTime + 1100)) {
		preTime = now;
		return;
	}
	preTime = now;
	preStatus = state;

	len = sprintf((char*)header ,"$DCMD%03d,%d", INFO_HEADERNUM_SDPLUGINOUT, state);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoSDInOutFB      %s\n" , header);	
#endif

	appCSumUart2Send(header, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoFactoryResetFB 
* Function		: function for get factory reset status information
* Return value	: void 
* Parameter1	: BOOL state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
void appInfoFactoryResetFB(BOOL state)
{

	UINT8 header[INFO_FB_FACTORYRESET_TOTALLEN] = "$DMSG023,";
	UINT32 tempDatalen = 10;


	if(state)
		header[INFO_FB_HEADER_LEN] ='1';
	else
		header[INFO_FB_HEADER_LEN] ='0';

#if DEBUG_ON	
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoFactoryResetFB\t");	
#endif
	appCSumUart2Send(header , tempDatalen, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoSetRecorderSoundFB 
* Function		: function for set recorder sound
* Return value	: void 
* Parameter1	: BOOL state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSetRecorderSoundFB(BOOL state)
{
	//UINT32 TotalLen = 15 ;
	UINT8 header[INFO_FB_SETRECORDERSOUND_TOTALLEN] = "$DMSG053,";
	UINT32 tempDatalen = 10;

	// return state
	if(state)
		header[INFO_FB_HEADER_LEN] ='1';  // Success
	else
		header[INFO_FB_HEADER_LEN] ='0';  // Failed

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s\t", __FUNCTION__);
#endif
	appCSumUart2Send(header , tempDatalen, DEBUG_ON);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSetBeepSoundFB 
* Function		: function for set beep sound
* Return value	: void 
* Parameter1	: const UINT8 *buf
* History	    : 2017.08.22
*               :  2017.09.18 Jason Modify
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSetBeepSoundFB(const UINT8 *buf)
{
	BOOL state;
	UINT8 header[INFO_FB_SETBEEPSOUND_TOTALLEN] = "$DMSG052,";
	UINT32 tempDatalen = 12;
	char c = buf[0];
	//UINT8 beepSound = pUiSetting->buzzerSet & BUZZER_SETTING_BOOT_ONOFF; // debug use

	if(c >= '0' && c <= '2' ) {
		state = TRUE;
	}
	else {
		state = FALSE;
	}

	header[INFO_FB_HEADER_LEN]  = c;
	header[INFO_FB_HEADER_LEN+1] = ',';

	// return state
	if(state) {
		header[INFO_FB_HEADER_LEN+2] = '1';  // Success
	}
	else {
		header[INFO_FB_HEADER_LEN+2] = '0';  // Failed
	}

#if DEBUG_ON	
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s\t", __FUNCTION__);
#endif
	appCSumUart2Send(header , tempDatalen, DEBUG_ON);
}

/*--------------------------------------------------------------------------*
* Function name : appInfoStartUserRecordingFB 
* Function		: function for handle start user recording state
* Return value	: void 
* Parameter1	: UserRecFBState state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
void appInfoStartUserRecordingFB(UserRecFBState state)
{
	UINT8 header[INFO_FB_USERRECS_TOTALLEN] = "$DMSG011,";
	UINT32 tempDatalen = 12;


	switch(state)
	{
		case USERREC_FB_NOERR:
			header[INFO_FB_HEADER_LEN]  = '1';
			header[INFO_FB_HEADER_LEN+1] =',';
			header[INFO_FB_HEADER_LEN+2] ='0';
			break;
		case USERREC_FB_DISKNONE:
			header[INFO_FB_HEADER_LEN]  = '0';
			header[INFO_FB_HEADER_LEN+1] =',';
			header[INFO_FB_HEADER_LEN+2] ='1';
			break;
		case USERREC_FB_STOPREC:
			header[INFO_FB_HEADER_LEN]  = '0';
			header[INFO_FB_HEADER_LEN+1] =',';
			header[INFO_FB_HEADER_LEN+2] ='2';
			break;
		case USERREC_FB_DISKFULL:
			header[INFO_FB_HEADER_LEN]  = '0';
			header[INFO_FB_HEADER_LEN+1] =',';
			header[INFO_FB_HEADER_LEN+2] ='3';
			break;
		case USERREC_FB_OTHERERR:
			header[INFO_FB_HEADER_LEN]  = '0';
			header[INFO_FB_HEADER_LEN+1] =',';
			header[INFO_FB_HEADER_LEN+2] ='6';
			break;
		default:
			break;	
	}

#if DEBUG_ON	
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoStartUserRecordingFB\t");	
#endif
	appCSumUart2Send(header , tempDatalen, DEBUG_ON);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoStopUserRecordingFB 
* Function		: function for handle stop user recording state
* Return value	: void 
* Parameter1	: BOOL state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
void appInfoStopUserRecordingFB(BOOL state)
{
	UINT8 header[INFO_FB_STOPUSERREC_TOTALLEN] = "$DMSG037,";
	UINT32 tempDatalen = 10;


	if(state)
		header[INFO_FB_HEADER_LEN]  = '1';
	else
		header[INFO_FB_HEADER_LEN]  = '0';


#if DEBUG_ON	
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "appInfoStopUserRecordingFB\t");	
#endif
	appCSumUart2Send(header , tempDatalen, DEBUG_ON);
}


/*--------------------------------------------------------------------------*
* Function name : appInfoSDUseTimeFB 
* Function		: function for handling sd time usage 
* Return value	: void 
* Parameter1	: UINT8 state
* Parameter2	: UINT32 time
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSDUseTimeFB(UINT8 state, UINT32 time)//SCC
{
	//< Response >
	//[Format]
	//$DMSG024,<return state>,<return value>
	UINT8 str[32];
	UINT32 len;

	len = sprintf((char*)str,"$DMSG%03d,%d,%d", INFO_HEADERNUM_SDNUMTIME, state, time);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoSmartAssitModeFB 
* Function		: function for get information of smart assistant mode 
* Return value	: void 
* Parameter1	: UINT8 state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSmartAssitModeFB(UINT8 state)//SCC
{
	//< Response >
	//[Format]
	//$DMSG056,<return state >
	UINT8 str[32];
	UINT32 len;

	len = sprintf((char*)str,"$DMSG%03d,%d", INFO_HEADERNUM_SETASSISTMODE, state);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}

/*--------------------------------------------------------------------------*
* Function name : appInfoSnapShotMarkFB 
* Function		: function for get information of smart assistant mode  
* Return value	: void 
* Parameter1	: UINT8 state
* Parameter2	: UINT8 status
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSnapShotMarkFB(UINT8 state, UINT8 status)//SCC
{
	//< Response >
	//[Format]
	//$DMSG007,<return state>,<Error Status>
	UINT8 str[32];
	UINT32 len;

	len = sprintf((char*)str,"$DMSG%03d,%d,%d", INFO_HEADERNUM_SETSNAPMARK, state, status);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoQueryAvailableSpaceFB 
* Function		: function for get information of available space  
* Return value	: void 
* Parameter1	: UINT8 state
* Parameter2	: queryAvailableSpace_t status
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoQueryAvailableSpaceFB(UINT8 state, queryAvailableSpace_t status)//SCC
{
	//< Response >
	//[Format]
	//$DMSG035,<Unused space>, <Deletable files space>,<G-sensor movie files >, <return state>
	UINT8 str[64];
	UINT32 len;

	len = sprintf((char*)str,"$DMSG%03d,%llu,%llu,%llu,%d", INFO_HEADERNUM_ACAILABLESPACE, status.unuse_space,
		status.deleted_file_size, status.gsensor_movie_file_size, state);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoSDLogFB 
* Function		: function for get information of logs in sdcard 
* Return value	: void 
* Parameter1	: UINT8 state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSDLogFB(UINT8 state)//SCC
{
	//< Response >
	//[Format]
	//$DMSG999,<return state >
	UINT8 str[32];
	UINT32 len;

	len = sprintf((char*)str,"$DMSG%03d,%d", INFO_HEADERNUM_SDLOGSET, state);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoHighTempProtectedFB 
* Function		: function for high temputure protect  
* Return value	: void 
* Parameter1	: UINT8 state
* Parameter2	: UINT8 temp
* History	    : 2017.08.22  Isaac Fong "First edit"
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoHighTempProtectedFB(UINT8 state, UINT8 temp)
{
	//< Response >
	//[Format]
	//$DMSG032,<return state>,<temp value>
	UINT8 str[32];
	UINT32 len;

	len = sprintf((char*)str,"$DMSG%03d,%d,%d", INFO_HEADERNUM_HITEMPPROTECTED, state, temp);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoSetGSensorFB 
* Function		: function for get gsensor state 
* Return value	: void 
* Parameter1	: BOOL state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSetGSensorFB(BOOL state) 
{
	//< Response >
	//[Format]
	//$DMSG050,<return state >
	UINT8 str[32];
	UINT32 len;

	len = sprintf((char*)str, "$DMSG%03d,%d", INFO_HEADERNUM_SETGSENSOR, state);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoSDCardFormatNecessityFB 
* Function		: function for get sd card format necessity
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoSDCardFormatNecessityFB(void)
{
	//[Format]
	//$DMSG026,<return state>,<return value>
	UINT8 str[32];
	UINT32 len;
	char state = '0' ;
	if (appInfoSDCardSlowGet()) {
		state = '1';
		appInfoSDCardSlowClr();
	}

	len = sprintf((char*)str, "$DMSG%03d,%c,%c", INFO_HEADERNUM_SDFORMATNECESSITY, '1', state);
	//	len = sprintf((char*)str, "$DMSG%03d,%c,%c", INFO_HEADERNUM_SDFORMATNECESSITY, '1','0');

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}

 
/*--------------------------------------------------------------------------*
* Function name : appInfoWriteEndUranceFB 
* Function		: function for write end urance
* Return value	: void 
* Parameter1	: queryWriteEdurance_t *edu
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoWriteEndUranceFB(queryWriteEdurance_t *edu)
{
	//[Format]
	//$DMSG036,<return state >, < endurance >,<NWritten>
	UINT8 str[32];
	UINT32 len;
	len = sprintf((char*)str, "$DMSG%03d,%d,%d,%llu", INFO_HEADERNUM_WRITEENDURANCE,
		edu->ret,edu->endurance,edu->nWritten);
#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen((char*)str));	
#endif
	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoParkingModeFB 
* Function		: function for get parking mode information
* Return value	: void 
* Parameter1	: ParkingModeTrigger state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoParkingModeFB(ParkingModeTrigger state)
{
//< Response >
//[Format]
//$DMSG055, <return state>, <Return Value>

	UINT8 str[64];
	UINT32 len;

	if(state  <= ParkingModeTigger_LOWPOWER )
		len = sprintf((char*)str, "$DMSG%03d,%c,%d", INFO_HEADERNUM_SETPARKMODE, '1', state);
	else
		len = sprintf((char*)str, "$DMSG%03d,%c,%d", INFO_HEADERNUM_SETPARKMODE, '0', state);
		
#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoQueryFileCountFB 
* Function		: function for get query file count number
* Return value	: void 
* Parameter1	: UINT8 state
* Parameter2	: UINT16 count
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoQueryFileCountFB(UINT8 state, UINT16 count)//SCC
{
	//< Response >
	//[Format]
	//$DMSG012,<return state>,<file count>
	UINT8 str[64];
	UINT32 len;

	//UINT8 chkSum=0;
	len = sprintf((char*)str, "$DMSG%03d,%d,%d", INFO_HEADERNUM_FILECOUNT, state, count);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}

#if 0
static BOOL isFileListSending = FALSE;

/*--------------------------------------------------------------------------*
* Function name : jet_filelist_sending_status_set 
* Function		: filelist sending status set
* Return value	: void 
* Parameter1	: BOOL s
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_filelist_sending_status_set(BOOL s){
	isFileListSending = s;
}



/*--------------------------------------------------------------------------*
* Function name : jet_filelist_sending_status_get 
* Function		: filelist sending status get
* Return value	: BOOL 
* Parameter1	: void
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL jet_filelist_sending_status_get(void){
	return isFileListSending;
}
#endif


/*--------------------------------------------------------------------------*
* Function name : appInfoFileListFB 
* Function		: function for get file list
* Return value	: void 
* Parameter1	: UINT16 index
* Parameter2	: UINT16 count
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*			    : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
void appInfoFileListFB(UINT16 index, UINT16 count)
{
	//$DMSG013,<return state>
	//$DMSG013,<index #0>,<create time>,<file size>,<file name>,<event information>
	char str[128];
	UINT8 state =0;
	UINT32 total_count;
	int len;
	CDFS_FILE_TAB_T *pCur;

	total_count = jetFolderFilterTotal();
	if(count==0 || total_count==0 || (index + count - 1) > total_count){
		state = 0;
	}else{
		state = 1;
	}
	len = sprintf(str,"$DMSG%03d,%d", INFO_HEADERNUM_FILELIST, state);
	appCSumUart2Send((UINT8 *)str, len, 1);
	if (state == 0) {
		return;
	}

	while (count) {
		pCur = jetFolderFilterFileNodeFind(index);
		if (pCur == NULL) {
			break;
		}
		/*Isaac add try to fixe old AVN doesn't support parking video*/
		if(NewAVNModel == FALSE){
			if(pCur->event & BIT4)
				pCur->event = (pCur->event|BIT3);
		}
		len = sprintf(str,"$DMSG%03d,%d,%s,%llu,%s,%d", INFO_HEADERNUM_FILELIST, index - 1, pCur->mtime, pCur->FileSize,
			pCur->FileName, pCur->event & 0x03ff);
		appCSumUart2Send((UINT8 *)str, len, DEBUG_ON);
		#if 1/*DEBUG_ON*/
		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen(str));	
		#endif
		count--;
		index++;
	}
}


/*--------------------------------------------------------------------------*
* Function name : appInfoQueryFileInfoFB 
* Function		: function for query file infomation
* Return value	: void 
* Parameter1	: UINT16 index
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoQueryFileInfoFB(UINT16 index)
{
	//$DCMD014,<file type filter>,<folder>,<file index>
	//$DMSG014,<return state>,<file index>,<create time>,<file size>,<file name>
	UINT8 str[64];
	UINT32 len;
	UINT8 state =0;
	UINT32 total_count;
	CDFS_FILE_TAB_T *pCur;

	total_count = jetFolderFilterTotal();
	if (total_count==0 || index>(total_count-1)){
		state = 0;
	} else {
		state = 1;
	}

	pCur = jetFolderFilterFileNodeFind(index + 1);
	if(pCur == NULL){
		len = sprintf((char*)str, "$DMSG%03d,%d,%d,%s,%d,%s",   INFO_HEADERNUM_FILEINFO, state, 0, "", 0, "");
	}else{
		len = sprintf((char*)str, "$DMSG%03d,%d,%d,%s,%llu,%s", INFO_HEADERNUM_FILEINFO, state, index, pCur->mtime, pCur->FileSize,
			pCur->FileName);
	}

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appFileDeleteFB 
* Function		: function for file delete
* Return value	: void 
* Parameter1	: UINT8 state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appFileDeleteFB(UINT8 state)
{
	//$DCMD015,<file type filter>,<folder>,<file index>,<delete all>
	//$DMSG015,<return state>
	UINT8 str[64];
	UINT32 len;
	//UINT8 chkSum=0;
	len = sprintf((char*)str, "$DMSG%03d,%d", INFO_HEADERNUM_FILEDEL, state);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appFilePBStartFB 
* Function		: function for file playback start command
* Return value	: void 
* Parameter1	: UINT8 state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
 void appFilePBStartFB(UINT8 state)
{
	 //[Format]
	//$DMSG016,<return state>
	UINT8 str[16];
	UINT32 len;
	//UINT8 chkSum=0;
	len = snprintf((char*)str, sizeof(str), "$DMSG%03d,%d", INFO_HEADERNUM_FILEPLAYBAKSTART, state);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s] str = %s, len = %d\n", __FUNCTION__, str, len);
#endif
	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appFilePBStopFB 
* Function		: function for file playback stop command
* Return value	: void 
* Parameter1	: UINT8 state
* Parameter2	: AVN_FILE_PB_PARA_T *para
* Parameter3	: UINT16 file_index
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appFilePBStopFB(UINT8 state, AVN_FILE_PB_PARA_T *para, UINT16 file_index)
{
	//< Response >
	//[Format]
	//$DMSG017,<return state>,<file type filter>,<folder>,<file index>
	UINT8 str[64];
	UINT32 len;

	AVN_FILE_PB_PARA_T empty;

	if(para==NULL){
		memset(&empty, 0, sizeof(empty));
		len = sprintf((char*)str, "$DMSG%03d,%d,%d,%d,%d", INFO_HEADERNUM_FILEPLAYBAKSTOP, 0, empty.filter, empty.folder, file_index);
	}else{
		len = sprintf((char*)str, "$DMSG%03d,%d,%d,%d,%d", INFO_HEADERNUM_FILEPLAYBAKSTOP, state, para->filter, para->folder,file_index);
	}

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen((char *)str));
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appFilePBPauseFB 
* Function		: function for file playback pause command
* Return value	: void 
* Parameter1	: UINT8 state
* Parameter2	: AVN_FILE_PB_PARA_T *para
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appFilePBPauseFB(UINT8 state, AVN_FILE_PB_PARA_T *para)
{
	//< Response >
	//[Format]
	//$DMSG018,<return state>,<file type filter>,<folder>,<file index>
	UINT8 str[64];
	UINT32 len;

	len = sprintf((char*)str, "$DMSG%03d,%d,%d,%d,%d", INFO_HEADERNUM_FILEPLAYBAKPAUSE, state, para->filter, para->folder,
		jet_file_list_filter_current_play_get()-1);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoMoveFileToSaveFolderFB 
* Function		: function for move file to saved floder command
* Return value	: void 
* Parameter1	: UINT8 state
* Parameter2	: UINT8 error
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoMoveFileToSaveFolderFB(UINT8 state, UINT8 error)//SCC
{
	//< Response >
	//[Format]
	//$DMSG038,<return state >,<error>
	UINT8 str[64];
	UINT32 len;

	len = sprintf((char*)str, "$DMSG%03d,%d,%d", INFO_HEADERNUM_MOVEFILESVAEFOLDER, state, error);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char *)str));	
#endif

	appCSumUart2Send(str, len, 1);

}

/**
 * @brief Report Error code of system
 */
static UINT8 __errorCodeReport = 0;


/*--------------------------------------------------------------------------*
* Function name : appInfoReportErrStatusFB 
* Function		: respond AVN INFO_HEADERNUM_ERRORSTATUSINFO
* Return value	: void 
* Parameter1	: void
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoReportErrStatusFB()
{
	/* FIXME: global error code of play status */
	/* $DMSG046,<return state>,<Error Code>*hh<CR><LF>*/
	UINT8 header[16];
	int len;
	char st = '0';
	if(__errorCodeReport <  4)
		st = '1';
	len = snprintf((char *) header, sizeof(header), "$DMSG%03d,%c,%hhu",
			INFO_HEADERNUM_ERRORSTATUSINFO,st, __errorCodeReport);
#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "%s(%hhu)\t", __func__, __errorCodeReport);
#endif
	appCSumUart2Send(header, len, DEBUG_ON);
}


/*--------------------------------------------------------------------------*
* Function name : appReportErrorStatusFB 
* Function		: respond error status
* Return value	: void 
* Parameter1	: UINT8 state
* Parameter2	: UINT8 error
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appReportErrorStatusFB(UINT8 state, UINT8 error)
{
	//[Format]
	//$DMSG046,<return state>,<Error Status>
	//0: Failed
	//1: Success
	//Error Status	 0: No Error
	//1: File Open Error
	//2: File Read Error
	//3: Decode Error
	UINT8 str[64];
	UINT32 len;

	if(error >= 4)
		state = 0;

	/* Update system error code */
	__errorCodeReport = error;
	len = sprintf((char*)str, "$DMSG%03d,%d,%d", INFO_HEADERNUM_ERRORSTATUSINFO, state, error);

#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n",__FUNCTION__, str, strlen((char*)str));
#endif

	appCSumUart2Send(str, len, DEBUG_ON);

}


/*--------------------------------------------------------------------------*
* Function name : appInfoGSensorCompensationFB 
* Function		: set gsensor calibration
* Return value	: void
* Parameter1	: UINT8 state
* History	    : 2017.08.22
*			    : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void appInfoGSensorCompensationFB(UINT8 state)
{
	UINT8 str[64];
	UINT32 len;

	state = state ? 1: 0;
	
	len = sprintf((char*)str, "$DMSG%03d,%d", INFO_HEADERNUM_SETGSENSORCALIBRATION, state);
	
#if DEBUG_ON
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "[%s] str = %s, len = %d\n", __FUNCTION__, str, len);	
#endif
	appCSumUart2Send(str, len, DEBUG_ON);

}



