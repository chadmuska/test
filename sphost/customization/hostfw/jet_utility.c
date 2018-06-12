/*--------------------------------------------------------------------------*
*  File name	 : Jet_utility.c
*  Function	     : "jet-opto utility function"
*  First editor: SC Chang
*  First date  : 2017.05.12
*  History:
*  2017.05.12 SC Chang Added
*  2018.04.11 Weikai Hsu Added File Header
*  
*  
*--------------------------------------------------------------------------*/ 
#include <stdlib.h>
#include <string.h>
#include "api/sp5k_global_api.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_dbg_api.h"
#include "app_sensor.h"
#include "sp5k_media_api.h"
#include "app_com_api.h"
#include "jet_utility.h"
#include "jet_cdfs.h"
#include "app_infohandlerComm.h"
#include "app_rtc.h"
#include "app_infohandler_func.h"
#include "app_infohandler_feedback.h"
#include "app_watch_dog.h"
#include "sp5k_aud_api.h"
#include "sp5k_exif_api.h"

#define DEBUG_ON 0
#define JET_SD_USE_TIME_FOLDER "D:\\SYSTEM"
#define JET_SD_USE_TIME_NAME	"D:\\SYSTEM\\USETIME.BIN"
#define JET_SD_LOG_NAME "D:\\SDMARK.LOG"
#define JET_SD_LOG_WAKEUP_TIME 3 //second
#define JET_SMART_ASSISTANT_PIV_COUNT 10
#define JET_SMART_ASSISTANT_PIV_TIME_INTERVAL 6000 // msecond
#define JET_SD_ERROR_NAME "D:\\SYSTEM\\ERROR.BIN" //Isaac fong
extern BOOL NewAVNModel;
extern BOOL bPIVEnd;
UINT32 SmartAssitModeTimer = TIMER_NULL;
UINT8 SmartAssitModePIVCount = 0;
extern void appPIVTrig(void);
extern void appPowerOffWakeupSet(UINT32 param);
extern CDFS_FILE_TAB_T *appCdfsFileListLastGet(UINT32 folder);
extern BOOL rtcWakeupPowerOnChk(void);
extern UINT32 jet_file_size_get(UINT8 *filename);
/*paul 2018.04.16 add for compiler warning*/
extern PowerDetect appInfoPowerDETStateGet(void);

static struct {
	UINT32 folder;
	UINT16 filter;
	UINT32 count;
	int index;
	CDFS_FILE_TAB_T *pCur;
} _current_file_filter = {0, 0, 0, 0, NULL};


static UINT16 FILE_LIST_FILTER_NUM=0, FILE_LIST_FILTER_CUR_PLAY=0;
static AVN_FILE_DELETE_PARA_T AVN_FILE_DELETE_PARA;
static BOOL IS_FILE_DELETING = FALSE;
#if 0
static BOOL IS_USERSAVE_FILE_MOVING = FALSE;
#endif


static FILE_UDATA_T FILE_UDATA_LIST[120];
static int FILE_UDATA_LINE_NUM=0;

static AVN_FILE_PB_PARA_T AVN_PB_PARA={0};


extern int appInfoRtcChangeSecGet(void);
extern void appInfoRtcChangeSecClear(void);
BOOL jetTimeEventFormUdat(UINT8 * filename, UINT32 *base_time, UINT32 *event);
void jetBaseTimeUdatSet(UINT8 * filename  , UINT32 totalsec);


/*--------------------------------------------------------------------------*
 * Function name   	: jetSDUseTimeWrite
 * Function	       	: calculate usage of sdcard write time
 * Return value   	:  void 
 * Parameter1    : UINT32 time
 * History	   : 2017.08.21
 *             : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetSDUseTimeWrite(UINT32 time)
{
	UINT32 fd=0, tmp=0;
	UINT8 buff[4]={0},i;
	printf("[%s] time : %d\n",__FUNCTION__, time);
	if(!IS_CARD_EXIST){
		printf("[%s] NO CARD\n",__FUNCTION__);
		return;
	}
	for(i=0; i<4; i++){
		tmp = (time>>(i*8)) & 0x000000FF;
		buff[i] = (UINT8)tmp;
	}
	fd = sp5kFsFileOpen((const UINT8*)JET_SD_USE_TIME_NAME, SP5K_FS_OPEN_CREATE);
	if(fd){
		fsFileWrite(fd, buff, 4);
		fsFileClose(fd);
	}else{
		printf("open file fail\n");
	}
}


/*--------------------------------------------------------------------------*
* Function name	: jetSDUseTimeInit
* Function 		: calculate usage of sdcard write time initalization
* Return value 	:  void 
* Parameter1    : void
* History	   : 2017.08.21
*			   : 2018.04.10  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetSDUseTimeInit(void)
{
	UINT32 fd=0;
	printf("[%s]\n",__FUNCTION__);
	if(!IS_CARD_EXIST){
		printf("[%s] NO CARD\n",__FUNCTION__);
		return;
	}
	sp5kFsDirMake((UINT8 *)JET_SD_USE_TIME_FOLDER);
	fd = sp5kFsFileOpen((const UINT8*)JET_SD_USE_TIME_NAME, SP5K_FS_OPEN_RDONLY);
	if(fd){
		fsFileClose(fd);
	}else{
		printf("open file fail\n");
		jetSDUseTimeWrite(0);
	}
	sp5kFsFileAttrSet((const UINT8*)JET_SD_USE_TIME_NAME, SP5K_FS_ATTR_HIDDEN);
}


/*--------------------------------------------------------------------------*
* Function name	: jetSDUseTimeRead
* Function 		: calculate usage of sdcard read time
* Return value 	:  UINT32 
* Parameter1    : void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 jetSDUseTimeRead(void)
{
	UINT32 fd=0, time=0;
	UINT8 buff[4]={0}, i;
	if(!IS_CARD_EXIST){
		printf("[%s] NO CARD\n",__FUNCTION__);
		return 0;
	}
	fd = sp5kFsFileOpen((const UINT8*)JET_SD_USE_TIME_NAME, SP5K_FS_OPEN_RDONLY);
	if(fd){
		fsFileRead(fd, &buff[0], 4);
		for(i=0; i<4; i++){
			time |= ((UINT32)buff[i] << (i*8));
	}
		fsFileClose(fd);
	}else{
		printf("open file fail\n");
	}
	if(time > 2500)
		jetErrStorageState(ERROR_SAVE_SD_USETIME);
	printf("[%s] time : %d\n",__FUNCTION__, time);
	return time;
}


/*--------------------------------------------------------------------------*
* Function name	: jetSDUseTimeCheck
* Function 		: calculate usage of sdcard time usage check
* Return value 	:  void 
* Parameter1    : void
* History	   : 2017.08.21
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetSDUseTimeCheck(void)
{
	static UINT32 totalSizeMB = 0;
	UINT32 fd=0, curSizeByte=0, curSizeMB=0, times;
	UINT8 curFileName[64];
	appDiskInfo_t* pDiskInfo;
	if(!IS_CARD_EXIST){
		printf("[%s] NO CARD\n",__FUNCTION__);
		return;
	}
	pDiskInfo = appCurDiskInfoGet();
	sprintf((char *)curFileName, "%s", appCdfsLastFileNameGet());
	printf("[%s] curFileName : %s\n", __FUNCTION__, curFileName);
	fd = sp5kFsFileOpen(curFileName, SP5K_FS_OPEN_RDONLY);
	if(fd){
		curSizeByte = sp5kFsFileSizeGet(fd);
		fsFileClose(fd);
	}else{
		printf("open file fail\n");
	}
	curSizeMB = curSizeByte>>20;
	totalSizeMB += curSizeMB;
	times = totalSizeMB/pDiskInfo->dskMbSz;
	printf("[%s] totalSizeMB %d \n", __FUNCTION__, totalSizeMB);
	printf("[%s] curSizeMB %d \n", __FUNCTION__, curSizeMB);
	printf("[%s] curSizeByte %d \n", __FUNCTION__, curSizeByte);
	printf("[%s] dskMbSz %d \n", __FUNCTION__, pDiskInfo->dskMbSz);
	printf("[%s] times %d \n", __FUNCTION__, times);
	jetSDUseTimeWrite(jetSDUseTimeRead() + times);
	if(times>0){
		times=0;
		totalSizeMB=0;
	}	
}


/*--------------------------------------------------------------------------*
* Function name	: jetSmartAssitModeTimerID
* Function 		: function for get smart assitant mode id
* Return value 	:  UINT32 
* Parameter1    : void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 jetSmartAssitModeTimerID(void)
{
	return SmartAssitModeTimer;
}


/*--------------------------------------------------------------------------*
* Function name	: jetSmartAssitModeParseString
* Function 		: function for parsr string on smart assitant mode
* Return value 	:  void 
* Parameter1    : void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetSmartAssitModeParseString(char *str, UINT8 *setting)
{
	//056,<Setting>
	char strtmp[32];
	char *last_comma;
	printf("[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen(str));
	strcpy(strtmp, str);
	last_comma = strrchr(strtmp,',');
	if(last_comma == NULL){
		printf("last comma is null\n");
		return;
	}
	*setting = atoi(last_comma+1);
	printf("[%s] setting = %d\n", __FUNCTION__, *setting);
}


/*--------------------------------------------------------------------------*
* Function name	: jetSmartAssitModeInit
* Function 		: function for initalize for smart assitant mode 
* Return value 	:  UINT8 
* Parameter1    : void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 jetSmartAssitModeInit(void)
{
	printf("[%s]\n", __FUNCTION__);
	if((appActiveStateGet()) != APP_STATE_VIDEO_REC){
		printf("not in recording mode\n");
		return 0;
	}
	if(SmartAssitModeTimer != TIMER_NULL){
		if (SmartAssitModeTimer<SP5K_TIMER_MAX) 
			return 0;
		else
			jetSmartAssitModeDestroy();
	}
	SmartAssitModePIVCount = 0;
	SmartAssitModeTimer = appTimerSet(JET_SMART_ASSISTANT_PIV_TIME_INTERVAL,"SmartAssitModeTimer");
	return 1;
}


/*--------------------------------------------------------------------------*
* Function name	: jetSmartAssitModeDestroy
* Function 		: function for initalize for smart assitant mode 
* Return value 	:  UINT8 
* Parameter1    : void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 jetSmartAssitModeDestroy(void)
{
	printf("[%s]\n", __FUNCTION__);
	appTimerClear(&SmartAssitModeTimer);
	SmartAssitModeTimer = TIMER_NULL;
	SmartAssitModePIVCount = 0;
	return 1;
}


/*--------------------------------------------------------------------------*
* Function name	: jetSmartAssitModeTimer
* Function 		: function for timer of smart assitant mode 
* Return value 	:  void 
* Parameter1    : void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetSmartAssitModeTimer(void)
{
	printf("[%s] bPIVEnd : %d, SmartAssitModePIVCount : %d\n", __FUNCTION__, bPIVEnd, SmartAssitModePIVCount);
	SnapState err=SnapState_NoERROR;
	if(SmartAssitModePIVCount > JET_SMART_ASSISTANT_PIV_COUNT ||appActiveStateGet()!=APP_STATE_VIDEO_REC){
		SmartAssitModePIVCount = 0;
		jetSmartAssitModeDestroy();
		return;
	}
	jetSnapShotMarkSet(0,NULL,&err);
	SmartAssitModePIVCount ++;
}

 
/*--------------------------------------------------------------------------*
* Function name	: jetSnapShotMarkParseString
* Function 		: function for parsing string of snapshot mark 
* Return value 	:  void 
* Parameter1    : char *str
* Parameter2    : UINT8 *type
* Parameter3    : char *filename
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetSnapShotMarkParseString(char *str, UINT8 *type, char *filename)
{
	//007, <Type>,<File Name>
	char strtmp[32];
	char *last_comma,*last_2nd_comma=NULL;
	printf("[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen(str));
	strcpy(strtmp, str);
	last_comma = strrchr(strtmp,',');
	if(last_comma == NULL){
		printf("last comma is null\n");
		return;
	}
	strcpy(filename, last_comma+1);
	*last_comma = '\0';
	last_2nd_comma = strrchr(strtmp,',');
	if(last_2nd_comma == NULL){
		printf("last 2nd comma is null\n");
		return;
	}
	*type = atoi(last_2nd_comma+1);
	printf("[%s] type = %d, filename = %s\n", __FUNCTION__, *type, filename);

}


/*--------------------------------------------------------------------------*
* Function name	: jetSnapShotMarkSet
* Function 		: function for set snapshot mark
* Return value 	:  UINT8 
* Parameter1    : UINT8 type
* Parameter2    : char *filename
* Parameter3    : SnapState *err
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 jetSnapShotMarkSet(UINT8 type, char *filename, SnapState *err)
{
	printf("[%s]\n", __FUNCTION__);
	*err = appInfoSnapStateGet();
	if(*err > SnapState_NoERROR){
		return 0;
	}
	if(bPIVEnd == FALSE){
		printf("piv did not end\n");
		return 2;
	}
	/*capture image in video*/
	appCdfsEventFlagSet(JPG_EVENT_IN_VIDEO);
	bPIVEnd = FALSE;
	appPIVTrig();	
	return 1;
}


/*--------------------------------------------------------------------------*
* Function name	: jetTotalFileSizeInFolderGet
* Function 		: function for get total file size in floder 
* Return value 	:  UINT64 
* Parameter1    : UINT8 folder
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT64 jetTotalFileSizeInFolderGet(UINT8 folder)
{
#if 0
	CDFS_FILE_TAB_T *pCur = NULL;
	UINT64 total_file_size=0;

	#if DEBUG_ON
	printf("[%s] folder = %d %u\n", __FUNCTION__, folder, appCdfsFolderCount(folder));
	#endif
	while(1) {
		pCur = appCdfsFolderNodeFind(folder, pCur);
		if (pCur == NULL)
			break;

		total_file_size += pCur->FileSize;
		#if DEBUG_ON
		printf("fname : %s, size(byte): %llu\n", pCur->FileName, pCur->FileSize);
		#endif
	}
	#if DEBUG_ON
	printf("[%s] total_file_size = %llu\n", __FUNCTION__,total_file_size);
	#endif
	return total_file_size;
#else
#if DEBUG_ON
	printf("%s used space %llu\n", __func__, appCdfsUsedSpace(folder));
#endif
	return appCdfsUsedSpace(folder);
#endif
}


/*--------------------------------------------------------------------------*
* Function name : jetQueryAvailableSpaceGet
* Function		: function for get available space 
* Return value	:  UINT8 
* Parameter1	: queryAvailableSpace_t *status
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 jetQueryAvailableSpaceGet(queryAvailableSpace_t *status)
{
	UINT64 disk_free_size_byte=0, gevent_size=0, normal_size=0;
	printf("[%s]\n", __FUNCTION__);
	status->deleted_file_size = 0;
	status->gsensor_movie_file_size = 0;
	status->unuse_space = 0;
	if(!IS_CARD_EXIST){
		printf("[%s] NO CARD\n",__FUNCTION__);
		return 0;
	}
	normal_size = jetTotalFileSizeInFolderGet(CDFS_FILE_FOLDER_VIDEO);
	gevent_size = jetTotalFileSizeInFolderGet(CDFS_FILE_FOLDER_EVENT);
	disk_free_size_byte = (UINT64)appCurDiskInfoGet()->freeSz <<10;
	status->deleted_file_size = normal_size;
	status->gsensor_movie_file_size = gevent_size;
	status->unuse_space = disk_free_size_byte;
	#if 0
	printf("[%s] normal_size\tgevent_size\tdisk_free_size_byte\n", __FUNCTION__);
	printf("[%s] %llu\t%llu\t%llu\n", __FUNCTION__, normal_size, gevent_size, disk_free_size_byte);
	#endif
	return 1;
}


/*--------------------------------------------------------------------------*
* Function name : jetSDLogParseString
* Function		: function for parse string in sd log  
* Return value	:  UINT8 
* Parameter1	: char *str
* Parameter2	: UINT8 *setting
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetSDLogParseString(char *str, UINT8 *setting)
{
	//999,<Setting>
	char strtmp[32];
	char *last_comma;
	printf("[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen(str));
	strcpy(strtmp, str);
	last_comma = strrchr(strtmp,',');
	if(last_comma == NULL){
		printf("last comma is null\n");
		return;
	}
	*setting = atoi(last_comma+1);
	printf("[%s] setting = %d\n", __FUNCTION__, *setting);
}


/*--------------------------------------------------------------------------*
* Function name : jetSDLogFileDelete 
* Function		: function for delete log in sd card 
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetSDLogFileDelete(void)
{
	UINT32 fd=0;
	printf("[%s]\n",__FUNCTION__);
	if(!IS_CARD_EXIST){
		printf("[%s] NO CARD\n",__FUNCTION__);
		return;
	}
	if(rtcWakeupPowerOnChk()== TRUE){
		printf("[%s] rtc wake up\n",__FUNCTION__);
		return;
	}
	fd = sp5kFsFileOpen((const UINT8*)JET_SD_LOG_NAME, SP5K_FS_OPEN_RDONLY);
	if(fd){
		fsFileClose(fd);
		fsFileDelete((const UINT8*)JET_SD_LOG_NAME);
	}
	fd = sp5kFsFileOpen((const UINT8*)"D:\\ICATCH00.LOG", SP5K_FS_OPEN_RDONLY);
	if(fd){
		fsFileClose(fd);
		fsFileDelete((const UINT8*)"D:\\ICATCH00.LOG");
	}

}


/*--------------------------------------------------------------------------*
* Function name : jetSDLogSet 
* Function		: function for log set in sd card
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 jetSDLogSet(UINT8 setting)
{
	UINT32 fd=0;
	BOOL fileExist = FALSE;
	printf("[%s] setting = %d\n",__FUNCTION__, setting);
	if(!IS_CARD_EXIST){
		printf("[%s] NO CARD\n",__FUNCTION__);
		return 0;
	}
	fd = sp5kFsFileOpen((const UINT8*)JET_SD_LOG_NAME, SP5K_FS_OPEN_RDONLY);
	if(fd){
		fsFileClose(fd);
		fileExist = TRUE;
	}else{
		fileExist = FALSE;
	}
	printf("[%s] fileExist = %d\n",__FUNCTION__, fileExist);
	if(setting){
		if(!fileExist){
			fd = sp5kFsFileOpen((const UINT8*)JET_SD_LOG_NAME, SP5K_FS_OPEN_CREATE);
			if(fd){
				fsFileClose(fd);
				printf("start reboot\n");
				appPowerOffWakeupSet(JET_SD_LOG_WAKEUP_TIME);
			}else{
				printf("failed to create file\n");
				return 0;
			}
		}
	}else{
		if(fileExist)
			fsFileDelete((const UINT8*)JET_SD_LOG_NAME);
	}
	return 1;
}


/*--------------------------------------------------------------------------*
* Function name : jetResistanceToTemp 
* Function		: Get temperature from MRT03F103ZZ1C SPEC table
* Return value	:  UINT8 
* Parameter1	: float r
* History	   : 2017.08.21  Isaac Fong
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
	/*
	*Temp	Minimum 				Maxmun
	*	 0	27,350.00	27,910.00	28,490.00
	*	 5	22,030.00	22,440.00	22,850.00
	*  10	17,870.00	18,160.00	18,460.00
	*  15	14,590.00	14,790.00	15,000.00
	*  20	11,980.00	12,130.00	12,270.00
	*  25	9,900.00		10,000.00	10,100.00
	*  30	8,194.00		8,293.00		8,392.00
	*  35	6,820.00		6,914.00		7,010.00
	*  40	5,706.00		5,795.00		5,886.00
	*  45	4,798.00		4,882.00		4,967.00
	*  50	4,055.00		4,132.00		4,211.00
	*  55	3,442.00		3,514.00		3,587.00
	*  60	2,936.00		3,002.00		3,068.00
	*  65	2,515.00		2,575.00		2,636.00
	*  70	2,163.00		2,218.00		2,274.00
	*  75	1,868.00		1,918.00		1,970.00
	*  80	1,619.00		1,665.00		1,713.00
	*  85	1,409.00		1,451.00		1,494.00
	*  90	1,231.00		1,269.00		1,309.00
	*  95	1,079.00		1,114.00		1,150.00
	0	32330
	5	25194
	10	19785
	15	15651
	20	12468
	25	10000
	30	8072
	35	6556
	40	5356
	45	4401
	50	3635
	55	3019
	60	2521
	65	2115
	70	1781
	75	1509
	80	1284
	85	1097
	90	941
	95	810
	*/
UINT8 jetResistanceToTemp(float r){

	if( 19785 < r )
		return 0;
	if( 15651 < r )
		return 5;
	if( 12468 < r )
		return 10;
	if( 10000 < r )
		return 15;
	if( 8072 < r )
		return 20;
	if( 6556 < r )
		return 25;
	if( 5356 < r )
		return 30;
	if( 4401 < r )
		return 35;
	if( 3635 < r )
		return 40;
	if( 3019 < r )
		return 45;
	if( 2521 < r )
		return 50;
	if( 2115 < r )
		return 55;
	if( 1781 < r )
		return 60;
	if( 1509 < r )
		return 65;
	if( 1284 < r )
		return 70;
	if( 1097 < r )
		return 75;
	if( 941 < r )
		return 80;
	if( 810 < r )
		return 85;
	if( 701 < r )
		return 90;
	if( 608 < r )
		return 95;
	if( 530 < r )
		return 100;
	if( 463 < r )
		return 105;
	if( 406 < r )
		return 110;
	if( 358 < r )
		return 115;	
/*
	if( 27350 < r )
		return 0;
	if( 22030 < r )
		return 5;
	if( 17870 < r )
		return 10;
	if( 14590 < r )
		return 15;
	if( 11980 < r )
		return 20;
	if( 9900 < r )
		return 25;
	if( 8194 < r )
		return 30;
	if( 6820 < r )
		return 35;
	if( 5706 < r )
		return 40;
	if( 4798 < r )
		return 45;
	if( 4055 < r )
		return 50;
	if( 3442 < r )
		return 55;
	if( 2936 < r )
		return 60;
	if( 2515 < r )
		return 65;
	if( 2163 < r )
		return 70;
	if( 1868 < r )
		return 75;
	if( 1619 < r )
		return 80;
	if( 1409 < r )
		return 85;
	if( 1231 < r )
		return 90;
	if( 1079 < r )
		return 95;
	if( 949 < r )
		return 100;
	if( 837 < r )
		return 105;
	if( 741 < r )
		return 110;
	if( 658 < r )
		return 115;
	if( 586 < r )
		return 120;
	if( 524 < r )
		return 125;
	*/

	return 255;
}


/*--------------------------------------------------------------------------*
* Function name : jetTempGet 
* Function		: function for get temperature
* Return value	:  TemperatureState 
* Parameter1	: UINT8 *temp
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
TemperatureState jetTempGet(UINT8 *temp)
{
	UINT32 ad = 0 , adTotal = 0;
	float V = 0;
	float tempValue=0;
	UINT8 temper = 0;
	
	int i  = 0;
	for( i = 0 ; i < 10 ; i++){
		sp5kAdcRead(SP5K_ADC_SARIN1,&ad);
		adTotal = adTotal+ad;
	}
	ad = adTotal/10;
	
	V=(float) ad*3/16383;/*get Voltage*/
	tempValue = V/3;
	tempValue = ((tempValue*10)/(1-tempValue))*1000;/*get resistance*/
	temper =  jetResistanceToTemp(tempValue); // hardware is not ready and need to translate degree unit.
	*temp = temper;
	#if DEBUG_ON
		printf("[%s] ad = %d, temp = %d   V = %f tempValue = %f\n", __FUNCTION__, ad, *temp , V , tempValue);
	#endif
	if(temper == 255)
		return TEMP_NONE;
	if(temper < 90 ){
		if(appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP&& temper>=80 )
			return TEMP_HIGH;
		else
			return TEMP_NORMAL;
	}
	if(temper >= 90)
		return TEMP_HIGH;

	
	return TEMP_NONE;
}


/*--------------------------------------------------------------------------*
* Function name : _jet_folder_filter 
* Function		: Check file folder match or not
* Return value	:  BOOL 
* Parameter1	: CDFS_FILE_TAB_T *pCur
* Parameter2	: CDFS_FILE_TAB_T *pCond
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL _jet_folder_filter(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pCond)
{
	BOOL ret = FAIL;
	UINT32 folder;

	if (pCur == NULL || pCond == NULL) {
		return ret;
	}
	folder = pCur->folder;
	if (folder >= JET_CDFS_DCIM_SUBFOLDER_START_INDEX)
		folder = CDFS_FILE_FOLDER_VIDEO_BIT;

	ret = (folder & pCond->folder) &&
		(pCur->event & pCond->event);

	/* default to have not index */
	pCur->index = 0;
#if DEBUG_ON
	printf("%s(%d) %s - %s 0x%04X, 0x%04X,%d\n",
			__func__, __LINE__, ret == TRUE ? "Match": "Not Match",
			pCur->FileName, pCur->folder, pCur->event,pCur->index);
#endif
	return ret;
}


/*--------------------------------------------------------------------------*
* Function name : jetFileCountInFolderGet 
* Function		: function for get file count in current floder
* Return value	:  UINT16 
* Parameter1	: UINT32 folder
* Parameter2	: UINT16 filter
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT16 jetFileCountInFolderGet(UINT32 folder, UINT16 filter)
{

	CDFS_FILE_TAB_T *pCur = NULL;
	CDFS_FILE_TAB_T cond;
	UINT16 count = 0;

	memset(&cond, 0, sizeof(cond));
	cond.folder = folder;

	/*Isaac add try to fixe old AVN doesn't support parking video*/
	if(NewAVNModel == FALSE&&filter != 1024){
		if(filter & BIT3)
			filter = filter|BIT4;
	}
	
	cond.event = filter;

	if (filter == 0)
		return count;

#if DEBUG_ON
	printf("%s{%d) - 0X%04X 0X%04X\n", __func__, __LINE__, folder, filter);
#endif
	while(1) {
		pCur = appCdfsNextNodeFind(pCur, &cond, _jet_folder_filter);
		if (pCur == NULL)
			break;
		count++;
		pCur->index = count;
	}

#if DEBUG_ON
	printf("%s{%d) - %u\n", __func__, __LINE__, count);
#endif
	return count;
}


/*--------------------------------------------------------------------------*
* Function name : jetFileCountInFolderGet 
* Function		: function for get file count in current floder
* Return value	:  void 
* Parameter1	: char *str
* Parameter2	: UINT16 *filter
* Parameter3	: UINT8 *folder
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetQueryFileCountParseString(char *str, UINT16 *filter, UINT8 *folder)
{
	//[Format]
	//$DCMD012,<file type filter>,<folder>
	char strtmp[32];
	char *p;
	printf("[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen(str));
	strcpy(strtmp, str);
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last comma is null\n");
		return;
	}
	*folder = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last second comma is null\n");
		return;
	}
	*filter = atoi(p+1);
	printf("[%s] filter = %d, folder = %d\n", __FUNCTION__, *filter, *folder);
}


/*--------------------------------------------------------------------------*
* Function name : _pana_folder_to_folder_map 
* Function		: function for get file count in current floder
* Return value	:  static UINT32 
* Parameter1	: UINT32 folder
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
static UINT32 _pana_folder_to_folder_map(UINT32 folder)
{
	UINT32 i = 0, j;

	switch (folder){
		case 0:
			for(j=CDFS_FILE_FOLDER_VIDEO; j<CDFS_FILE_FOLDER_MAX; j++){
				if(j==CDFS_FILE_FOLDER_JPG || j==CDFS_FILE_FOLDER_USERSAVE){
					continue;
				}
				i |= (1 << j);
			}
			break;
		case 1:
			i = 1 << CDFS_FILE_FOLDER_USERSAVE;
			break;
		case 2:
			i = 1 << CDFS_FILE_FOLDER_JPG;
			break;
		case 3:
			i = 1 << CDFS_FILE_FOLDER_ANNIV;
			break;
		default:
			printf("[%s] WRONG folder!\n", __FUNCTION__);
			break;
	}

	return i;

}


/*--------------------------------------------------------------------------*
* Function name : jetQueryReset 
* Function		: Reset the queried file count
* Return value	:  void 
* Parameter1	: void
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetQueryReset()
{
    /* Reset filter */
    memset(&_current_file_filter, 0, sizeof(_current_file_filter));
}


/*--------------------------------------------------------------------------*
* Function name : jetQueryFileCountGet
* Function		: function for get queried file count
* Return value	:  void 
* Parameter1	: UINT16 filter
* Parameter2	: UINT8 folder
* Parameter3	: UINT16 *total_count
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*			   : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
UINT8 jetQueryFileCountGet(UINT16 filter, UINT8 folder, UINT16 *total_count)
{
	UINT32 o = 0;

	*total_count=0;

	if(!IS_CARD_EXIST) {
		printf("[%s] NO CARD!\n", __FUNCTION__);
		return 0;
	}

	o = _pana_folder_to_folder_map(folder);
	if(NewAVNModel == FALSE && filter != 1024){
		if(filter & BIT3)
		filter = filter|BIT4;
	}

	if (_current_file_filter.folder == o &&
	        _current_file_filter.filter == filter) {
	    printf("use previous filter & filter\n");
	    *total_count = _current_file_filter.count;
	    return 1;
	}

	/* rebuild current filter and folder file index */
	memset(&_current_file_filter, 0, sizeof(_current_file_filter));
	_current_file_filter.count = jetFileCountInFolderGet(o, filter);
	_current_file_filter.folder = o;
	_current_file_filter.filter = filter;

	*total_count = _current_file_filter.count;
	return 1;
}

#if 0
static BOOL _is_same_current_file_filter(UINT32 folder, UINT16 filter)
{
	UINT32 o;

	o = _pana_folder_to_folder_map(folder);
	return _current_file_filter.folder == o && _current_file_filter.filter == filter;
}
#endif


/*--------------------------------------------------------------------------*
* Function name : jetFolderFilterTotal
* Function		: Get folder filter count
* Return value	:  UINT32 
* Parameter1	: void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 jetFolderFilterTotal()
{
	return _current_file_filter.count;
}


/*--------------------------------------------------------------------------*
* Function name : _jet_match_index
* Function		: Check file index match
* Return value	:  BOOL 
* Parameter1	: CDFS_FILE_TAB_T *pCur
* Parameter2	: CDFS_FILE_TAB_T *pCond
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL _jet_match_index(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pCond)
{
#if DEBUG_ON
	printf("%s - %p %s, %d\n", __func__, pCur, pCur->FileName, pCur->index);
#endif
	return pCond->index == pCur->index;
}


/*--------------------------------------------------------------------------*
* Function name : *jetFolderFilterFileNodeFind
* Function		: Find the file by folder filter
* Return value	:  CDFS_FILE_TAB_T 
* Parameter1	: UINT32 idx
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
CDFS_FILE_TAB_T *jetFolderFilterFileNodeFind(UINT32 idx)
{
	CDFS_FILE_TAB_T cond;
	CDFS_FILE_TAB_T *pCur = NULL;

	if (_current_file_filter.folder == 0 || _current_file_filter.filter == 0 ||
			idx > _current_file_filter.count) {
		printf("%s(%d) folder(%u) filter(%u) idx %u total count %u\n",
				__func__, __LINE__,
			       	_current_file_filter.folder, _current_file_filter.filter,
				idx, _current_file_filter.count);
		goto out;
	}

	/* file index start from 1 */
	if (idx == 0) {
		goto out;
	}

	if (_current_file_filter.index == 0) {
		pCur = NULL;
	} else {
		pCur = _current_file_filter.pCur;
		/* Cache */
		if (idx == _current_file_filter.index && pCur != NULL) {
			goto out;
		}
	}

	memset(&cond, 0, sizeof(cond));
	cond.index = idx;
#if DEBUG_ON
	printf("%s(%d) pCur %p, idx %u %u\n", __func__, __LINE__, pCur, idx, _current_file_filter.index);
#endif 

	 if (idx >= _current_file_filter.index) {
		pCur = appCdfsNextNodeFind(pCur, &cond, _jet_match_index);
	 } else {
		pCur = appCdfsPrevNodeFind(pCur, &cond, _jet_match_index);
	 }

	_current_file_filter.pCur = pCur;
	if (pCur == NULL)
		_current_file_filter.index = 0;
	else
		_current_file_filter.index = pCur->index;
#if DEBUG_ON
	printf("%s(%d) pCur %p, idx %u %u %s\n", __func__, __LINE__, pCur, idx, _current_file_filter.index, pCur ? pCur->FileName : "(NIL)");
#endif 
out:
	return pCur;
}


/*--------------------------------------------------------------------------*
* Function name : *jetFolderFilterFileNodeFind
* Function		: Find the file by folder filter
* Return value	:  void 
* Parameter1	: char *str
* Parameter2	: UINT16 *filter
* Parameter3	: UINT8 *folder
* Parameter4	: UINT16 *file_index
* Parameter5	: UINT16 *file_count
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetFileListParseString(char *str, UINT16 *filter, UINT8 *folder, UINT16 *file_index, UINT16 *file_count)
{
	//[Format]
	//$DCMD013,<file type filter>,<folder>,<file index>,<file count>
	char strtmp[32];
	char *p;
	printf("[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen(str));
	strcpy(strtmp, str);
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last comma is null\n");
		return;
	}
	*file_count = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last second comma is null\n");
		return;
	}
	*file_index = atoi(p+1) + 1;
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last 3rd comma is null\n");
		return;
	}
	*folder = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last 4th comma is null\n");
		return;
	}
	*filter = atoi(p+1);
	printf("[%s] filter = %d, folder = %d, file_index = %d, file_count=%d\n", __FUNCTION__,
		*filter, *folder, *file_index, *file_count);
}




#if 0
/**
 * @brief function for add file to list
 * @param FILE_LIST_PANA_T - New
 * @return void
 * @aothur 
 * @date 08/24/2017
 *
 */
void jetFileListAddFile(FILE_LIST_PANA_T *New)
{
	FILE_LIST_PANA_T *pHead=NULL;
	FILE_LIST_PANA_T *pCurDest=NULL;
	FILE_LIST_PANA_T *pNext=NULL;
	FILE_LIST_PANA_T *pPre=NULL;
	FILE_LIST_PANA_T *pNew=NULL;

	pHead = pFILE_LIST_FILTER;
	pNew = sp5kMalloc(sizeof(FILE_LIST_PANA_T));
	if (pNew == NULL) {
		printf("%s(%d) no mem\n", __func__, __LINE__);
		return;
	}
	memset(pNew,0,sizeof(FILE_LIST_PANA_T));
	memcpy(pNew,New,sizeof(FILE_LIST_PANA_T));
	pNew->next = NULL;
	pNew->pre = pNew;

	if (pHead == NULL)/*create first object of table*/
	{
		/*add file@head*/
		pNew->pre = pNew;
		pHead = pNew;
	}
	else
	{
		if(pCurDest == NULL){
			pCurDest = pNext = pHead->pre;
		}
		while(1)
		{
			if (strncmp((const char *)pNew->FileName,(const char *)pCurDest->FileName, 7) > 0)/*sort by date*/
			{
				if (pCurDest->next)
					pCurDest = pCurDest->next;
				else {	/* cannot find any more node */
					pCurDest->next = pNew;
					pNew->pre = pCurDest;
					break;
				}
				break;
			}
			else if(0/*strcmp((const char *)pNew->FileName,(const char *)pCurDest->FileName) == 0*/)/*the same name*/
			{/*the new file replace the old file when two file name is same*/
				strcpy(pCurDest->FileName,(char *)pNew->FileName);
				pCurDest->event = pNew->event;
				pCurDest->cdfs_folder = pNew->cdfs_folder;
				pCurDest->FileSize = pNew->FileSize;
				strcpy(pCurDest->mtime,(char *)pNew->mtime);
				sp5kFree(pNew);
				pNew = pCurDest;
				break;
			}
			else
			{
				if (pCurDest != pHead)
				{
					pNext = pCurDest;
					pCurDest = pCurDest->pre;
					
					if(try_insert && last_insert)
					{
						try_insert = 0;
						if(strcmp((const char *)pNew->FileName,(const char *)last_insert->FileName) >= 0)
						{
							if(last_insert->next)
							{
								if(strcmp((const char *)pNew->FileName,(const char *)last_insert->next->FileName)<0)
								{
									pCurDest = last_insert;
									pNext = last_insert->next;
									/* insert */
									pCurDest->next = pNew;
									pNew->pre = pCurDest;
									pNew->next = pNext;
									pNext->pre = pNew;
									break;
								}
							}
						}
					}
				}

			} else {
				/* pCurDest is head, now pNew is head */
				if (pHead == pCurDest) {
					pHead = pNew;
					pNew->next = pCurDest;
					pCurDest->pre = pNew;
				} else {
					pPre = pCurDest->pre;
					pNext = pCurDest->next;

					pNew->next = pCurDest;
					pNew->pre = pPre;

					pPre->next = pNew;
					pCurDest->pre = pNew;
				}

				break;
			}
		}
		appWatchDogRefresh();
	}
	pFILE_LIST_FILTER =pHead;
}
#endif




#if 0
/**
 * function for get flie list 
 * @param UINT8 - folder
 * @param UINT16 - filter
 * @return void
 * @aothur 
 * @date 08/24/2017
 *
 */
void jetFileListGet(UINT8 folder, UINT16 filter)
{
	UINT32 total_count;

	printf("[%s] folder = %d, filter = %d\n", __FUNCTION__, folder, filter);
	if(!IS_CARD_EXIST){
		printf("[%s] NO CARD!\n", __FUNCTION__);
		return;
	}

	if (!_is_same_current_file_filter(folder, filter)) {
		jetQueryFileCountGet(filter, folder, &total_count);
	}
}
#endif


/*--------------------------------------------------------------------------*
* Function name : jetQueryFileInfoParseString
* Function		: function for parsing string of information in queried file
* Return value	:  void 
* Parameter1	: char *str
* Parameter2	: UINT16 *filter
* Parameter3	: UINT8 *folder
* Parameter4	: UINT16 *file_index
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetQueryFileInfoParseString(char *str, UINT16 *filter, UINT8 *folder, UINT16 *file_index)
{
	//[Format]
	//$DCMD014,<file type filter>,<folder>,<file index>
	char strtmp[32];
	char *p;
	printf("[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen(str));
	strcpy(strtmp, str);
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last comma is null\n");
		return;
	}
	*file_index = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last second comma is null\n");
		return;
	}
	*folder = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last 3rd comma is null\n");
		return;
	}
	*filter = atoi(p+1);
	printf("[%s] filter = %d, folder = %d, file_index = %d\n", __FUNCTION__, *filter, *folder, *file_index);
}


/*--------------------------------------------------------------------------*
* Function name : jetFileDeleteParseString
* Function		: function for get flie list
* Return value	:  void 
* Parameter1	: char *str
* Parameter2	: AVN_FILE_DELETE_PARA_T *pFileDelete
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetFileDeleteParseString(char *str, AVN_FILE_DELETE_PARA_T *pFileDelete)
{
	//[Format]
	//$DCMD015,<file type filter>,<folder>,<file index>,<delete all>
	char strtmp[32];
	char *p;
	printf("[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen(str));
	strcpy(strtmp, str);
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last comma is null\n");
		return;
	}
	pFileDelete->del_all = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last second comma is null\n");
		return;
	}
	pFileDelete->file_index = atoi(p+1) + 1;
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last 3rd comma is null\n");
		return;
	}
	pFileDelete->folder = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last 4th comma is null\n");
		return;
	}
	pFileDelete->filter = atoi(p+1);

	printf("[%s] filter = %d, folder = %d, file_index = %d, del_all = %d\n", __FUNCTION__,
		pFileDelete->filter, pFileDelete->folder, pFileDelete->file_index, pFileDelete->del_all); 
}


/*--------------------------------------------------------------------------*
* Function name : jetFileListDeleteOne
* Function		: function for delete one file in current play list
* Return value	:  UINT8 
* Parameter1	: UINT16 file_index
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 jetFileListDeleteOne(UINT16 file_index)
{
	UINT8 rval=AVN_PB_DELETE_STATUS_FAIL;
	CDFS_FILE_TAB_T *pCur=NULL;

	if(!IS_CARD_EXIST){
		printf("[%s] NO CARD!\n", __FUNCTION__);
		goto out;
	}
	pCur = jetFolderFilterFileNodeFind(file_index);
	if (pCur == NULL) {
		printf("%s(%d) not found index %u\n", __func__, __LINE__, file_index);
		goto out;
	}

	if (appCdfsFileDel(pCur) == SUCCESS) {
		rval = AVN_PB_DELETE_STATUS_SUCCESS;
		/* reset index of cache */
		_current_file_filter.index = 0;
	}
#if 0
	pCur = jet_cdfs_current_play_handle_get(file_index);
	cdfs_folder = (pCur)->cdfs_folder;

	
	strcpy(dirName, jet_cdfs_folder_to_path(cdfs_folder, (pCur)->subfolder));
	if(sp5kFsFileDelete((const UINT8 *)fileName)==SUCCESS){/*file name with dir path*/
	/*FIXME: jet_appCdfsFileDel */
		jet_appCdfsFileDel((UINT8 *)(pCur)->FileName, cdfs_folder);
		rval = AVN_PB_DELETE_STATUS_SUCCESS;
	}else{
		printf("failed to delete... file_index = %d, fileName = %s\n", file_index, fileName);
		rval = AVN_PB_DELETE_STATUS_FAIL;
	}
#endif
out:
	return rval;
}


/*--------------------------------------------------------------------------*
* Function name : jetFileListDelete
* Function		: function for delete play list
* Return value	:  UINT8 
* Parameter1	: AVN_FILE_DELETE_PARA_T *pFileDelete
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 jetFileListDelete(AVN_FILE_DELETE_PARA_T *pFileDelete)
{
	UINT16 i=0;
	UINT8 state = AVN_PB_DELETE_STATUS_FAIL;
	if(pFileDelete==NULL){
		printf("[%s] pFileDelete is null\n", __FUNCTION__);
		return state;
	}
	printf("[%s] del_all = %d, file_index = %d, filter = %d, folder = %d, total_count = %d\n", __FUNCTION__, pFileDelete->del_all, pFileDelete->file_index, pFileDelete->filter,
		pFileDelete->folder, pFileDelete->total_count);	
	IS_FILE_DELETING = TRUE;
	if(pFileDelete->del_all== 0){
		for (i = 1; i <= pFileDelete->total_count; i++) {
			state = jetFileListDeleteOne(i);
			if(state == AVN_PB_DELETE_STATUS_FAIL){
				break;
			}
		}
	}else{
		state = jetFileListDeleteOne(pFileDelete->file_index);
	}

	IS_FILE_DELETING = FALSE;
	jetQueryReset();

	return state;
}


/*--------------------------------------------------------------------------*
* Function name : jet_filelist_delete_status_check
* Function		: check and feedback the status of file deleting
* Return value	:  void 
* Parameter1	: void
* History	   : 2017.09.02  SC Chang
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_filelist_delete_status_check(void)
{
	static UINT32 preOsTimeDeleting=0, preOsTimeTimeOut=0;
	UINT32 curOsTime=0;
	curOsTime = sp5kOsTimeGet();
	if(IS_FILE_DELETING == FALSE){
		preOsTimeDeleting = curOsTime;
		preOsTimeTimeOut = curOsTime;
		return;
	}
	//printf("[%s]\n", __FUNCTION__);
	if(curOsTime>preOsTimeTimeOut+100*60){ // 60 second
		appFileDeleteFB(AVN_PB_DELETE_STATUS_TIMEOUT);
		printf("[%s] time out to delete files\n", __FUNCTION__);
		preOsTimeTimeOut = curOsTime;
		return;
	}
	if(curOsTime>preOsTimeDeleting+100){ // 1 second
		appFileDeleteFB(AVN_PB_DELETE_STATUS_DELETING);
		printf("[%s] deleting files\n", __FUNCTION__);
		preOsTimeDeleting = curOsTime;
	}
}


/*--------------------------------------------------------------------------*
* Function name : jetFilePBStartParseString
* Function		: function for start parse in string  
* Return value	:  BOOL 
* Parameter1	: char *str
* Parameter2	: AVN_FILE_PB_PARA_T *para
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL jetFilePBStartParseString(char *str, AVN_FILE_PB_PARA_T *para)
{
	//[Format]
	//$DCMD016,<file type filter>,<folder>,<file index>,<mode>,<mode branch>,<speed>,< speed branch>
	char strtmp[32];
	char *p;
	printf("[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen(str));
	para->file_index=0;
	para->filter=0;
	para->folder=0;
	para->mode=0;
	para->mode_branch=0;
	para->speed=0;
	para->speed_branch=0;
	strcpy(strtmp, str);
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last comma is null\n");
		return FAIL;
	}
	para->speed_branch = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last second comma is null\n");
		return FAIL;
	}
	para->speed = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last 3rd comma is null\n");
		return FAIL;
	}
	para->mode_branch = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last 4th comma is null\n");
		return FAIL;
	}
	para->mode = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last 5th comma is null\n");
		return FAIL;
	}
	para->file_index = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last 6th comma is null\n");
		return FAIL;
	}
	para->folder = atoi(p+1);
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last 7th comma is null\n");
		return FAIL;
	}
	para->filter = atoi(p+1);
	printf("filter %d, folder %d, file_index %d, mode %d, mode_branch %d, speed %d, speed_branch %d\n",
		para->filter, para->folder, para->file_index, para->mode, para->mode_branch, para->speed, para->speed_branch);
	return SUCCESS;
}


/*--------------------------------------------------------------------------*
* Function name : jet_avn_file_delete_para_set
* Function		: Set the AVN file delete parameter
* Return value	:  void 
* Parameter1	: AVN_FILE_DELETE_PARA_T para
* Parameter2	: AVN_FILE_PB_PARA_T *para
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_avn_file_delete_para_set(AVN_FILE_DELETE_PARA_T para)
{
	printf("[%s]\n", __FUNCTION__);
	memcpy(&AVN_FILE_DELETE_PARA, &para, sizeof(para));
	printf("del_all=%d,file_index=%d,filter=%d,folder=%d,total_count=%d\n",AVN_FILE_DELETE_PARA.del_all,AVN_FILE_DELETE_PARA.file_index,AVN_FILE_DELETE_PARA.filter,
		AVN_FILE_DELETE_PARA.folder, AVN_FILE_DELETE_PARA.total_count);	
}


/*--------------------------------------------------------------------------*
* Function name : *jet_avn_file_delete_para_get
* Function		: Get the AVN file delete parameter
* Return value	:  AVN_FILE_DELETE_PARA_T 
* Parameter1	: void
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
AVN_FILE_DELETE_PARA_T *jet_avn_file_delete_para_get(void)
{
	return &AVN_FILE_DELETE_PARA;
}
#if 0
/**
 * function for set file list fliter
 * @param AVN_FILE_PB_PARA_T - p
 * @return void
 * @aothur 
 * @date 08/24/2017
 *
 */

void jet_file_list_filter_set(FILE_LIST_PANA_T *p)
{
	pFILE_LIST_FILTER = p;
	//printf("[%s] pFILE_LIST_FILTER = %x \n", __FUNCTION__, pFILE_LIST_FILTER);
}
#endif

#if 0
/**
 * function for get file list fliter
 * @param void
 * @return FILE_LIST_PANA_T
 * @aothur 
 * @date 08/24/2017
 *
 */

FILE_LIST_PANA_T *jet_file_list_filter_get(void)
{
	//printf("[%s] pFILE_LIST_FILTER = %x \n", __FUNCTION__, pFILE_LIST_FILTER);
	return pFILE_LIST_FILTER;
}
#endif


/*--------------------------------------------------------------------------*
* Function name : jet_file_list_filter_num_get
* Function		: function for get file list fliter number
* Return value	:  UINT16
* Parameter1	: void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT16 jet_file_list_filter_num_get(void)
{
	return FILE_LIST_FILTER_NUM;
}


/*--------------------------------------------------------------------------*
* Function name : jet_file_list_filter_num_set
* Function		: function for get file list fliter number
* Return value	:  void
* Parameter1	: UINT16 num
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_file_list_filter_num_set(UINT16 num)
{
	FILE_LIST_FILTER_NUM = num;
}


/*--------------------------------------------------------------------------*
* Function name : jet_file_list_filter_current_play_get
* Function		: function for get current play file list fliter 
* Return value	:  UINT16
* Parameter1	: void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT16 jet_file_list_filter_current_play_get(void)
{
	printf("[%s] FILE_LIST_FILTER_CUR_PLAY = %d \n", __FUNCTION__, FILE_LIST_FILTER_CUR_PLAY);
	return FILE_LIST_FILTER_CUR_PLAY;
}


/*--------------------------------------------------------------------------*
* Function name : jet_file_list_filter_current_play_set
* Function		: function for set current play file list fliter
* Return value	:  void
* Parameter1	: UINT16 idx
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_file_list_filter_current_play_set(UINT16 idx)
{
	FILE_LIST_FILTER_CUR_PLAY = idx;
	printf("[%s] FILE_LIST_FILTER_CUR_PLAY = %d \n", __FUNCTION__, FILE_LIST_FILTER_CUR_PLAY);
}

 
/*--------------------------------------------------------------------------*
* Function name : *jet_avn_pb_para_get
* Function		: function for get avn pb parameter
* Return value	:  AVN_FILE_PB_PARA_T
* Parameter1	: void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
AVN_FILE_PB_PARA_T *jet_avn_pb_para_get(void)
{
	return &AVN_PB_PARA;
}


/*--------------------------------------------------------------------------*
* Function name : jet_avn_pb_para_set
* Function		: function for set avn pb parameter
* Return value	:  void
* Parameter1	: AVN_FILE_PB_PARA_T *p
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_avn_pb_para_set(AVN_FILE_PB_PARA_T *p)
{
	memcpy(&AVN_PB_PARA, p, sizeof(AVN_FILE_PB_PARA_T));
}


/*--------------------------------------------------------------------------*
* Function name : jet_file_tag_seek
* Function		: function for seek file tag
* Return value	:  BOOL
* Parameter1	: UINT32 fd
* Parameter2	: UINT8 *tag
* Parameter3	: UINT32 offset_size_in
* Parameter4	: UINT32 *tag_size_out
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL jet_file_tag_seek(UINT32 fd, UINT8 *tag, UINT32 offset_size_in, UINT32 *tag_size_out)
{
	UINT8 tag_buf[4], tag_size_buf[4];
	

	if(fd == 0 || tag == NULL){
		return FAIL;
	}

	if (fsFileSeek(fd, offset_size_in, SP5K_FS_SEEK_CUR) != SUCCESS) {
		return FAIL;
	}

	if (fsFileRead(fd, tag_size_buf, sizeof(tag_size_buf)) >= 0x80000000)
		return FAIL;

	if (fsFileRead(fd, tag_buf, sizeof(tag_buf)) >= 0x80000000)
		return FAIL;

	//printf("[%s] tag_buf = 0x%x, 0x%x, 0x%x, 0x%x\n", __FUNCTION__,
	//	tag_buf[0], tag_buf[1], tag_buf[2], tag_buf[3]);
	*tag_size_out = 0;
	if(tag_buf[0]== *tag && tag_buf[1]== *(tag+1) && tag_buf[2]== *(tag+2) && tag_buf[3]== *(tag+3)){
		int i;
		for(i=0; i<4; i++){
			*tag_size_out |= ((UINT32)tag_size_buf[3-i] << (i*8));
		}
	}else{
		return FAIL;
	}
	return SUCCESS;
}


/*--------------------------------------------------------------------------*
* Function name : jet_file_udata_tag_seek
* Function		: function for seek file udata tag
* Return value	:  BOOL
* Parameter1	: UINT32 fd
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL jet_file_udata_tag_seek(UINT32 fd)
{
	UINT32 offset_size_in=0, tag_size_out=0;
	UINT8 tag[4];
	tag[0]='f';tag[1]='t'; tag[2]='y'; tag[3]='p';

	offset_size_in = 0;
	if (jet_file_tag_seek(fd, tag, offset_size_in, &tag_size_out)==FAIL) {
		printf("[%s] can not seek tag ftyp\n", __FUNCTION__);
		fsFileClose(fd);
		return FAIL;
	}

	tag[0]='m';tag[1]='d'; tag[2]='a'; tag[3]='t';
	offset_size_in = tag_size_out-8;
	if(jet_file_tag_seek(fd, tag, offset_size_in, &tag_size_out)==FAIL){
		printf("[%s] can not seek tag mdat\n", __FUNCTION__);
		fsFileClose(fd);
		return FAIL;
	}

	tag[0]='m';tag[1]='o'; tag[2]='o'; tag[3]='v';
	offset_size_in = tag_size_out-8;
	if(jet_file_tag_seek(fd, tag, offset_size_in, &tag_size_out)==FAIL){
		printf("[%s] can not seek tag moov\n", __FUNCTION__);
		return FAIL;
	}
	tag[0]='m';tag[1]='v'; tag[2]='h'; tag[3]='d';
	offset_size_in = 0;
	if(jet_file_tag_seek(fd, tag, offset_size_in, &tag_size_out)==FAIL){
		printf("[%s] can not seek tag mvhd\n", __FUNCTION__);
		return FAIL;
	}
	tag[0]='t';tag[1]='r'; tag[2]='a'; tag[3]='k';
	offset_size_in = tag_size_out-8;
	if(jet_file_tag_seek(fd, tag, offset_size_in, &tag_size_out)==FAIL){
		printf("[%s] can not seek tag trak\n", __FUNCTION__);
		return FAIL;
	}
	tag[0]='t';tag[1]='r'; tag[2]='a'; tag[3]='k';
	offset_size_in = tag_size_out-8;
	if(jet_file_tag_seek(fd, tag, offset_size_in, &tag_size_out)==FAIL){
		printf("[%s] can not seek tag trak\n", __FUNCTION__);
		return FAIL;
	}
	tag[0]='u';tag[1]='d'; tag[2]='a'; tag[3]='t';
	offset_size_in = tag_size_out-8;
	if(jet_file_tag_seek(fd, tag, offset_size_in, &tag_size_out)==FAIL){
		printf("[%s] can not seek tag udat\n", __FUNCTION__);
		return FAIL;
	}
	//printf("[%s] seek tag udat\n", __FUNCTION__);
	return SUCCESS;
}


/*--------------------------------------------------------------------------*
* Function name : jet_file_udata_line_read
* Function		: function for read udata line in file
* Return value	:  BOOL
* Parameter1	: UINT32 fd
* Parameter2	: char* line
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL jet_file_udata_line_read(UINT32 fd, char* line)
{
	UINT8 ch=0, line_buffer[256];
	int count=0;
    if (fd == 0) {
		return FAIL;
	}
	if(fsFileRead(fd, &ch, 1)>=0x7FFFFFFF){
		return FAIL;
	}
	if(ch == '\0' || ch == EOF){
		return FAIL;
	}
    while ((ch != '\n') && (ch != EOF)) {
		if(count>=sizeof(line_buffer)){
			return FAIL;
		}
        line_buffer[count] = ch;
        count++;
        if(fsFileRead(fd, &ch, 1)>=0x7FFFFFFF){
			return FAIL;
        }
    }
	if(count>=sizeof(line_buffer)){
		return FAIL;
	}
    line_buffer[count] = '\0';
    strcpy(line, (char *)line_buffer);
    return SUCCESS;
}


/*--------------------------------------------------------------------------*
* Function name : jet_file_udata_data_parse
* Function		: function for parsing udata data
* Return value	:  void
* Parameter1	: char *line
* Parameter2	: FILE_UDATA_T *udata
* Parameter3	: tmx_t *time
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_file_udata_data_parse(char *line, FILE_UDATA_T *udata, tmx_t *time)
{
	char *p;
	int token_index=0;
	int tempSec = 0;
	char tempStr[15];

	//line = 004150.000,010114,0,35.6812,N,139.7662,E,999.99,65535,116,-8,235,0,8992,349,122,0,-1,-1
	memset(udata, 0, sizeof(FILE_UDATA_T));
	strcpy(udata->gpsTime,"");
	strcpy(udata->gpsDate,"");
	strcpy(udata->gpsEWIndicator,"");
	strcpy(udata->gpsNSIndicator,"");

	p = strtok(line, ",");
	while (p != NULL){
		if(token_index == 0){
			
			strcpy(tempStr,"");
			tempSec = atoi(p);
			 if(appInfoNewDayCheck(time, tempSec)){
				
			#if DEBUG_ON
				printf("%s Line %d New day\n",__FUNCTION__,__LINE__);
			#endif
				sprintf(tempStr ,"%02d%02d%02d.000" , (time->tmx_hour+1-24), (time->tmx_min+1-60) ,(time->tmx_sec+tempSec-60));
			}
			else if(appInfoNewHourCheck(time, tempSec)){
				
			#if DEBUG_ON
				printf("%s Line %d New Hour\n",__FUNCTION__,__LINE__);
			#endif
				sprintf(tempStr ,"%02d%02d%02d.000" , time->tmx_hour+1,(time->tmx_min+1-60),(time->tmx_sec+tempSec-60));
			
			}
			else if(appInfoNewMinCheck(time, tempSec)){
				
			#if DEBUG_ON
				printf("%s Line %d New Min\n",__FUNCTION__,__LINE__);
			#endif
				sprintf(tempStr ,"%02d%02d%02d.000" , time->tmx_hour,time->tmx_min+1,(time->tmx_sec+tempSec-60));
			}
			else
				sprintf(tempStr ,"%02d%02d%02d.000" , time->tmx_hour,time->tmx_min,time->tmx_sec+tempSec);
			strcpy(udata->gpsTime,tempStr);

		}else if(token_index == 1){

			strcpy(tempStr,"");
			
			if(appInfoNewYearCheck(time, tempSec)){
			#if DEBUG_ON
				printf("%s Line %d New year tempSec %d\n",__FUNCTION__,__LINE__,tempSec);
			#endif
				sprintf(tempStr ,"%02d%02d%02d" , 1,1,(time->tmx_year-2000+1));
			}
			else if(appInfoNewMonthCheck(time, tempSec)){
			#if DEBUG_ON
				printf("%s Line %d New Month\n",__FUNCTION__,__LINE__);
			#endif
				sprintf(tempStr ,"%02d%02d%02d" ,1,time->tmx_mon+1,(time->tmx_year-2000));
			}
			else if(appInfoNewDayCheck(time, tempSec)){
			#if DEBUG_ON
				printf("%s Line %d New day\n",__FUNCTION__,__LINE__);
			#endif
				sprintf(tempStr ,"%02d%02d%02d" , time->tmx_mday+1,time->tmx_mon,(time->tmx_year-2000));
			}

			else
				sprintf(tempStr ,"%02d%02d%02d" ,time->tmx_mday,time->tmx_mon, (time->tmx_year-2000));
			strcpy(udata->gpsDate,tempStr);

		}else if(token_index == 2){
			udata->gpsDatum = atoi(p);

		}else if(token_index == 3){
			strcpy(udata->gpsLat,p);

		}else if(token_index == 4){
			strcpy(udata->gpsNSIndicator,p);

		}else if(token_index == 5){
			strcpy(udata->gpsLong,p);

		}else if(token_index == 6){
			strcpy(udata->gpsEWIndicator,p);

		}else if(token_index == 7){
			strcpy(udata->gpsSpeed,p);

		}else if(token_index == 8){
			udata->gpsAngle = atoi(p);

		}else if(token_index == 9){
			udata->gsensorX = atoi(p);

		}else if(token_index == 10){
			udata->gsensorY = atoi(p);

		}else if(token_index == 11){
			udata->gsensorZ = atoi(p);
		}else if(token_index == 12){
			udata->event = atoi(p);
		}
		token_index++;
		p = strtok(NULL, ",");
	}
}



#if 0
/**

 * function for get udata file event 
 * @param char - filename
 * @param UINT16 - event
 * @return BOOL
 * @aothur 
 * @date 08/24/2017
 *
 */
BOOL jet_file_udata_event_get(char *filename, UINT32 *event)
{
	UINT32 fd=0;
	char line[256];
	#if DEBUG_ON
	printf("[%s] filename = %s\n", __FUNCTION__, filename);
	#endif
	fd = sp5kFsFileOpen((const UINT8*)filename, SP5K_FS_OPEN_RDONLY);
	*event=0;
	if(fd == 0){
		printf("[%s] open file error\n", __FUNCTION__);
		return FAIL;
	}
	if(jet_file_udata_tag_seek(fd)==FAIL){
		fsFileClose(fd);
		return FAIL;
	}
	if(jet_file_udata_line_read(fd,line) == SUCCESS){
		#if DEBUG_ON
		printf("[%s] line = %s\n", __FUNCTION__, line);
		#endif
		*event = strtoul(line,NULL,10);
		//*event = atoi(line);
	}
	fsFileClose(fd);
	return SUCCESS;
}
#endif


/*--------------------------------------------------------------------------*
* Function name : jet_node_full_path
* Function		: Get full path name by file
* Return value	:  void
* Parameter1	: CDFS_FILE_TAB_T *pCur
* Parameter2	: char *full_name
* Parameter3	: int size
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_node_full_path(CDFS_FILE_TAB_T *pCur, char *full_name, int size)
{
	char path[20];
	UINT32 folder;

	folder = pCur->folder;
	/* Check it VIDEO folder */
	if (folder >= JET_CDFS_DCIM_SUBFOLDER_START_INDEX) {
		appCdfsFolder2Path(CDFS_FILE_FOLDER_VIDEO, (UINT8 *)path);
		snprintf(full_name, size, "%s/%03d_NML/%s",
				path, pCur->folder, pCur->FileName);
		
	} else {
		appCdfsFolder2Path(jet_folder_bit_2_cdfs_folder(folder), (UINT8 *)path);
		snprintf(full_name, size, "%s/%s", path, pCur->FileName);
	}

}


/*--------------------------------------------------------------------------*
* Function name : jet_file_udata_update
* Function		: Updata udata by file
* Return value	:  void
* Parameter1	: CDFS_FILE_TAB_T *pCur
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_file_udata_update(CDFS_FILE_TAB_T *pCur)
{
	char full_name[48];
	UINT32 event;
	UINT32 totalSec = 0;
	BOOL pana_file;
	tmx_t realTime;
#if DEBUG_ON
	printf("%s(%d)\n", __func__, __LINE__);
#endif
	if (pCur == NULL)
		return;

	if (pCur->folder == CDFS_FILE_FOLDER_EVENT_BIT ||
			CDFS_FILE_FOLDER_USERRECORD_BIT == pCur->folder) {
#if DEBUG_ON
	    printf("%s(%d) GEVENT\n", __func__, __LINE__);
#endif
	    jet_cdfs_check_same_mins(pCur->FileName);
	}
	pana_file = jet_cdfs_pana_filename_rule_check((char *)pCur->FileName);
	if (pana_file == FALSE) {
		pCur->event = 0;
		return;
	}
	
	jet_node_full_path(pCur, full_name, sizeof(full_name));
	if (pCur->FileType == SP5K_DCF_FILETYPE_JPG) {
		event = (0x01 << FILE_TYPE_PHOTO);
		jet_exif_datetime_get((UINT8 *)full_name,&realTime);
	} else {
		if (jetTimeEventFormUdat((UINT8 *)full_name, &totalSec, &event) == FAIL) {
			event = (0x01 << FILE_TYPE_VIDEO_NORMAL);
		}
		if(event == 0) {
			event = (0x01 << FILE_TYPE_VIDEO_NORMAL);
		}
		event |= (0x01 << FILE_TYPE_TOTAL_FILE);
		if (pCur->FileType == SP5K_DCF_FILETYPE_MP4)
			event |= (0x01 << FILE_TYPE_TOTAL_VIDEO);

		appInfoSecTimeToRealGet( totalSec ,&realTime);
	}

	pCur->event = event;
	snprintf(pCur->mtime, sizeof(pCur->mtime),
			"%04d/%02d/%02d/%02d:%02d:%02d",
			realTime.tmx_year, realTime.tmx_mon, realTime.tmx_mday,
			realTime.tmx_hour, realTime.tmx_min, realTime.tmx_sec);

}


/*--------------------------------------------------------------------------*
* Function name : jet_file_udata_info_get
* Function		: function for parsing udata data
* Return value	:  BOOL
* Parameter1	: char *filename
* Parameter2	: FILE_UDATA_T *udataList
* Parameter3	: int *line_count
* Parameter4	: UINT32 *event
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL jet_file_udata_info_get(char *filename, FILE_UDATA_T *udataList, int *line_count, UINT32 *event)
{
	UINT32 fd=0;
	UINT32 baseTimeSec=0;
	tmx_t time;
	UINT8 *pbuf;
	char *p, *s;
	int len;
	int line_index = 0;
	FILE_UDATA_T udata_tmp;

	printf("[%s] %s\n", __FUNCTION__, filename);
	fd = sp5kFsFileOpen((const UINT8*)filename, SP5K_FS_OPEN_RDONLY);
	if(fd == 0){
		printf("[%s] open file error\n", __FUNCTION__);
		return FAIL;
	}
	if(jet_file_udata_tag_seek(fd)==FAIL){
		fsFileClose(fd);
		return FAIL;
	}

	jetTimeEventFormUdat((UINT8 *)filename, &baseTimeSec, NULL);
	appInfoSecTimeToRealGet(baseTimeSec , &time);
	if(udataList==NULL){
		fsFileClose(fd);
		return FAIL;
	}
	*line_count = 0;

	pbuf = sp5kMalloc(7200);

	if ((len = fsFileRead(fd, pbuf, 7199)) >= 0x80000000) {
		fsFileClose(fd);
		printf("%s(%d)\n", __func__, __LINE__);
		sp5kFree(pbuf);
		return FAIL;
	}

	pbuf[len] = '\0';

	p = strtok_r((char *)pbuf, "\n", &s);
	while(p != NULL) {
#if DEBUG_ON
		printf("[%s][%d] line = %s\n", __FUNCTION__, line_index, p);
#endif
		if(line_index>0){
			jet_file_udata_data_parse(p, &udata_tmp,&time);
			memcpy(udataList+(*line_count), &udata_tmp, sizeof(udata_tmp));
			#if DEBUG_ON
			printf("line_count = %d\n", *line_count);
			printf("gpsTime = %s\n", (udataList+(*line_count))->gpsTime);
			printf("gpsDate = %s\n", (udataList+(*line_count))->gpsDate);
			printf("gpsDatum = %d\n", (udataList+(*line_count))->gpsDatum);
			printf("gpsLat = %f\n", (udataList+(*line_count))->gpsLat);
			printf("gpsLong = %f\n", (udataList+(*line_count))->gpsLong);
			printf("gpsNSIndicator = %s\n", (udataList+(*line_count))->gpsNSIndicator);
			printf("gpsEWIndicator = %s\n", (udataList+(*line_count))->gpsEWIndicator);
			printf("gpsSpeed = %f\n", (udataList+(*line_count))->gpsSpeed);
			printf("gpsAngle = %d\n", (udataList+(*line_count))->gpsAngle);
			printf("gsensorX = %d\n", (udataList+(*line_count))->gsensorX);
			printf("gsensorY = %d\n", (udataList+(*line_count))->gsensorY);
			printf("gsensorZ = %d\n", (udataList+(*line_count))->gsensorZ);
			#endif
			(*line_count)++;
		}else{
			//*event = atoi(line);
			*event = strtoul(p, NULL, 10);
		}
		line_index++;
		p = strtok_r(NULL, "\n", &s);
	}

	fsFileClose(fd);
	sp5kFree(pbuf);

	if(*line_count==0){
		return FAIL;
	}
	return SUCCESS;
}


/*--------------------------------------------------------------------------*
* Function name : jet_file_udata_info_line_num_get
* Function		: function for get line number of udata file
* Return value	:  int
* Parameter1	: void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
int jet_file_udata_info_line_num_get(void)
{
	return FILE_UDATA_LINE_NUM;
}


/*--------------------------------------------------------------------------*
* Function name : *jet_file_udata_info_handle_get
* Function		: function for get udata list
* Return value	:  FILE_UDATA_T
* Parameter1	: void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
FILE_UDATA_T *jet_file_udata_info_handle_get(void)
{
	return &FILE_UDATA_LIST[0];
}


/*--------------------------------------------------------------------------*
* Function name : jet_pb_video_file_udata_info_get
* Function		: function for get udata of video file
* Return value	:  BOOL
* Parameter1	: void
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL gIsUdataReset = FALSE;;
BOOL jet_pb_video_file_udata_info_get(void)
{
#if 0
	UINT8 cdfs_folder=0;
	char filename[64], dirName[64]; // redmine_2474
	UINT16 event=0;
	FILE_LIST_PANA_T *pCur=NULL;
	printf("[%s]\n", __FUNCTION__);
	if(pFILE_LIST_FILTER == NULL){
		return FAIL;
	}
	if(FILE_LIST_FILTER_NUM == 0){
		return FAIL;
	}
	pCur = jet_cdfs_current_play_handle_get(FILE_LIST_FILTER_CUR_PLAY);
	cdfs_folder = (pCur)->cdfs_folder;
	if(appCdfsFileListHeadGet(cdfs_folder) == NULL){
		return FAIL;
	}
	strcpy(dirName, jet_cdfs_folder_to_path((pCur)->cdfs_folder, 
		(pCur)->subfolder));
	sprintf(filename, "%s/%s", dirName, (pCur)->FileName);
	memset(FILE_UDATA_LIST,0, sizeof(FILE_UDATA_LIST));
	return jet_file_udata_info_get(filename,&FILE_UDATA_LIST[0], &FILE_UDATA_LINE_NUM, &event);
#else
	UINT32 event=0;
	CDFS_FILE_TAB_T *pCur;
	char dirName[32], filename[32];
	BOOL rval;

	printf("[%s]\n", __FUNCTION__);
	pCur = jetFolderFilterFileNodeFind(FILE_LIST_FILTER_CUR_PLAY);
	if(pCur==NULL){
		return FAIL;
	}
	strcpy(dirName, jet_cdfs_folder_to_path(pCur->folder)); 
	sprintf(filename, "%s/%s", dirName, (pCur)->FileName);
	gIsUdataReset = TRUE;
	memset(FILE_UDATA_LIST,0, sizeof(FILE_UDATA_LIST));
	rval = jet_file_udata_info_get(filename,&FILE_UDATA_LIST[0], &FILE_UDATA_LINE_NUM, &event);
	gIsUdataReset = FALSE;
	return rval;
#endif
}


/*--------------------------------------------------------------------------*
* Function name : jetMoveFileToSaveFolderParseString
* Function		: function for parse string: move file to destination floder
* Return value	:  void
* Parameter1	: char *str
* Parameter2	: UINT16 *filter
* Parameter3	: UINT16 *file_index
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetMoveFileToSaveFolderParseString(char *str, UINT16 *filter, UINT16 *file_index)
{
	//< Command >
	//[Format]
	//$DCMD038,<file type filter>,<file index>
	char strtmp[32];
	char *p;
	printf("[%s] str = %s, len = %d\n", __FUNCTION__, str, strlen(str));
	strcpy(strtmp, str);
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last comma is null\n");
		return;
	}
	*file_index = atoi(p+1) + 1;
	*p = '\0';
	p = strrchr(strtmp,',');
	if(p == NULL){
		printf("last second comma is null\n");
		return;
	}
	*filter = atoi(p+1);
	printf("[%s] filter = %d, file_index = %d\n", __FUNCTION__, *filter, *file_index);
}

#if 0
void jet_usersave_movefile_status_check(void)
{
	static UINT32 preOsTimeMoving=0;
	UINT32 curOsTime=0;
	curOsTime = sp5kOsTimeGet();
	if(IS_USERSAVE_FILE_MOVING == FALSE){
		preOsTimeMoving = curOsTime;
		return;
	}
	//printf("[%s]\n", __FUNCTION__);
	if(curOsTime>preOsTimeMoving+100){ // 1 second
		appInfoMoveFileToSaveFolderFB(2, 0);//moving
		printf("[%s] moving files\n", __FUNCTION__);
		preOsTimeMoving = curOsTime;
	}
}
#endif


/*--------------------------------------------------------------------------*
* Function name : jetMoveFileHandle
* Function		: function for handling move file
* Return value	:  UINT8
* Parameter1	: UINT16 file_index
* Parameter2	: UINT8 *err
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT8 jetMoveFileHandle(UINT16 file_index, UINT8 *err)
{
	UINT8 Path[16]={0};
	UINT8 srcFileName[32]={0};
	UINT8 dstFileName[32]={0};
	CDFS_FILE_TAB_T *pCur;
	BOOL ret = FAIL;
	*err = 0;

	if(appCdfsFolderCount(CDFS_FILE_FOLDER_USERSAVE)
		>= jet_cdfs_file_num_limit_by_card_size_get(CDFS_FILE_FOLDER_USERSAVE)){
		printf("over file num limit\n");
		*err = 3;
		return 3;
	}	
	pCur = jetFolderFilterFileNodeFind(file_index);
	if(pCur==NULL){
		*err = 2;
		return 0;
	}

	strcpy((char *)Path, jet_cdfs_folder_to_path(pCur->folder));
	snprintf((char *)srcFileName, sizeof(srcFileName), "%s/%s", Path, pCur->FileName);
	appCdfsFolder2Path(CDFS_FILE_FOLDER_USERSAVE, Path);
	snprintf((char *)dstFileName, sizeof(dstFileName), "%s/%s", Path, pCur->FileName);

	if(appFileExistCheck((UINT8 *)srcFileName) == FALSE){
		printf("file %s does not exist!\n", srcFileName);
		*err = 2;
		return 0;
	}
	if(appFileExistCheck((UINT8 *)dstFileName) == TRUE){
		printf("file %s already exist!\n", dstFileName);
		*err = 2;
		return 0;
	}

	ret = sp5kFsRename((UINT8 *)srcFileName, (UINT8 *)dstFileName);
	if(ret == FAIL){
		printf("sp5kFsRename fail\n");
		*err = 2;
#if 0
		IS_USERSAVE_FILE_MOVING = FALSE;
#endif
		return 0;
	}

	appCdfsFolderNodeCountRemove(pCur);
	pCur->folder = CDFS_FILE_FOLDER_USERSAVE_BIT;
	appCdfsFolderNodeCountAdd(pCur);
#if 0
	//return state
	//  0: Failed
	//	1: Success
	//	2: Moving
	//	3: File count limited
	//error	0: none
	//	1: Size error
	//	2: file name error
	//	3: File count error
	UINT32 cdfs_folder_tmp=0;
	FILE_LIST_PANA_T *pCur = jet_file_list_filter_get();
	char filename[64], dest_filename[64];
	char dirName[64]; // redmine_2474
	BOOL ret = FAIL;
	*err = 0;
	if(appCdfsFolderCount(CDFS_FILE_FOLDER_USERSAVE)
		>= jet_cdfs_file_num_limit_by_card_size_get(CDFS_FILE_FOLDER_USERSAVE)){
		printf("over file num limit\n");
		*err = 3;
		return 3;
	}	
	if(pCur==NULL){
		*err = 2;
		return 0;
	}
	pCur = jet_cdfs_current_play_handle_get(file_index);
	strcpy(dirName, jet_cdfs_folder_to_path((pCur)->cdfs_folder, (pCur)->subfolder));
	sprintf(filename,"%s/%s", dirName, (pCur)->FileName);
	sprintf(dest_filename,"D:/USERSAVE/%s", (pCur)->FileName);
	printf("filename = %s\n",filename);
	printf("dest_filename = %s\n",dest_filename);
	if(appFileExistCheck((UINT8 *)filename) == FALSE){
		printf("file %s does not exist!\n",filename);
		*err = 2;
		return 0;
	}
	if(jet_file_size_get((UINT8 *)filename)==0){
		*err = 1;//size error
		return 0;
	}
	if(appFileExistCheck((UINT8 *)dest_filename) == TRUE){
		printf("file %s already exist!\n", dest_filename);
		*err = 2;
		return 0;
	}
	IS_USERSAVE_FILE_MOVING = TRUE;
	ret = sp5kFsRename((UINT8 *)filename, (UINT8 *)dest_filename);
	printf("sp5kFsRename = %d\n", ret);
	if(ret == FAIL){
		printf("sp5kFsRename fail\n");
		*err = 2;
		IS_USERSAVE_FILE_MOVING = FALSE;
		return 0;
	}
	cdfs_folder_tmp = appCdfsFileFolderGet();
	appCdfsFileFolderSet((pCur)->cdfs_folder);
	appCdfsFileDel(pCur);
	appCdfsFileFolderSet(cdfs_folder_tmp);
	appCdfsFileSync((UINT8 *)dest_filename, 0);
	IS_USERSAVE_FILE_MOVING = FALSE;
#endif
	return 1;
}


/*--------------------------------------------------------------------------*
* Function name : jetDoTimeBackCheck
* Function		: Check the time back by sec
* Return value	:  void
* Parameter1	: tmx_t *time
* Parameter2	: int sec
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetDoTimeBackCheck( tmx_t *time  , int sec){

	static UINT8 month[] =
        { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	BOOL leap;
	leap = (time->tmx_year % 400) == 0 || ((time->tmx_year % 4) == 0 && (time->tmx_year % 100) != 0);

	if(sec > time->tmx_sec){
				
		 if( time->tmx_min == 0 && time->tmx_hour == 0 && time->tmx_mday == 1 &&time->tmx_mon == 1){/*for year back*/
		 	time->tmx_year --;
		 	time->tmx_mon = 12 ;
			time->tmx_mday = 31;
			time->tmx_hour= (time->tmx_hour+24-1);
			time->tmx_min = (time->tmx_sec+60-1);
		}
		else if( time->tmx_min == 0 && time->tmx_hour == 0 && time->tmx_mday == 1 ){/*for month back*/
		 	time->tmx_mon--;
			
			if(leap && time->tmx_mon == 2)
				time->tmx_mday = 29;
			else
				time->tmx_mday = month[time->tmx_mon];
			
			time->tmx_hour= (time->tmx_hour+24-1);
			time->tmx_min = (time->tmx_sec+60-1);
		}
		else if( time->tmx_min == 0 && time->tmx_hour == 0 ){/*for Hour back*/
			time->tmx_mday--;
			time->tmx_hour= (time->tmx_hour+24-1);
			time->tmx_min = (time->tmx_sec+60-1);
		}
		else if( time->tmx_min == 0){/*for min back*/
			time->tmx_hour--;
			time->tmx_min = (time->tmx_sec+60-1);
		}
		else{/*for Sec back*/
			time->tmx_min--;
		}
		
		time->tmx_sec = (time->tmx_sec+60-sec);
	}
	else
		time->tmx_sec = (time->tmx_sec-sec);
}


/*--------------------------------------------------------------------------*
* Function name : jet_lastfile_rename_by_rtc_changed
* Function		: function for last file rename from time
* Return value	:  BOOL
* Parameter1	: CDFS_FILE_TAB_T *pCur
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*			   : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
BOOL jet_lastfile_rename_by_rtc_changed(CDFS_FILE_TAB_T *pCur)
{
	char tmpfname[32],srcfname[32],destfname[32], *p=NULL;

	UINT32 minutes;
	tmx_t time;
	appRTC_t *pAvnTime;
	printf("[%s] bRTCChanged = %d\n",__FUNCTION__, appInfoRTCChangedGet());
	if(appInfoRTCChangedGet() == FALSE){
		return FALSE;
	}
	appInfoRTCChangedSet(FALSE);
	pAvnTime = appInfoAVNTimeGet();
	if(pAvnTime == NULL){
		printf("[%s] pAvnTime is null\n",__FUNCTION__);
		return FALSE;
	}

	if(pCur->folder < JET_CDFS_DCIM_SUBFOLDER_START_INDEX){
		printf("%s(%d) Not in video folder\n", __func__, __LINE__);
		return FALSE;
	}

	strcpy(tmpfname,(char *)pCur->FileName);

	time.tmx_year= pAvnTime->year-1900;
	time.tmx_mon= pAvnTime->mon;
	time.tmx_mday= pAvnTime->day;
	time.tmx_hour= pAvnTime->hour;
	time.tmx_min= pAvnTime->min;
	time.tmx_sec= pAvnTime->sec;

	jetDoTimeBackCheck(&time ,appInfoRtcChangeSecGet() );
	appInfoRtcChangeSecClear();
	minutes = jet_cdfs_datetime_to_minute(time);
	p = strrchr(tmpfname,'.');
	if(p==NULL){
		return FALSE;
	}
	*p = '\0';	
	snprintf(srcfname, sizeof(srcfname), "%s/%s", jet_cdfs_folder_to_path(pCur->folder), pCur->FileName);
	snprintf((char *)pCur->FileName, sizeof(pCur->FileName), "%06X%s.MP4", minutes, p-2);
	snprintf(destfname, sizeof(destfname), "%s/%s", jet_cdfs_folder_to_path(pCur->folder), pCur->FileName);

	printf("[%s] src = %s, dst = %s\n",__FUNCTION__, srcfname, destfname);
	if(strcmp(srcfname, destfname) != 0 &&
			sp5kFsRename((UINT8 *)srcfname, (UINT8 *)destfname)==FAIL){
		printf("%s: fail rename, delete %s\n", __func__, srcfname);
		return FALSE;
	}

	printf("%s %08X %08X\n" ,__FUNCTION__ , minutes, (minutes*60+time.tmx_sec) );
#if 0

	appCdfsLastFileNameSet((UINT8 *)destfname);
#endif
	return TRUE;
}


/*--------------------------------------------------------------------------*
* Function name : jet_rec_video_post
* Function		: Video record post
* Return value	:  void
* Parameter1	: CDFS_FILE_TAB_T *pCur
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_rec_video_post(CDFS_FILE_TAB_T *pCur)
{
	if (pCur == NULL) {
		printf("%s(%d) pCur == NULL\n", __func__, __LINE__);
		return;
	}
	printf("%s(%d) %s\n", __func__, __LINE__, pCur->FileName);
	jet_lastfile_rename_by_rtc_changed(pCur);
}


/*--------------------------------------------------------------------------*
* Function name : jet_LatiLogi_string_to_rational
* Function		: function for convert latilogi string to rational
* Return value	:  void
* Parameter1	: char *str
* Parameter2	: UINT32 *dwbuf
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_LatiLogi_string_to_rational(char *str, UINT32 *dwbuf) //for exif
{
	UINT32 gpsLen = 0;
	UINT32 gpsVal = 0;
	UINT8 tmpBuf[32], tmpBuf2[32];
	char *token, *delm = ".";
	printf("[%s]str = %s\n", __FUNCTION__,str);
	strcpy((char*)tmpBuf,str);
	strcpy((char*)tmpBuf2,"");
	token = strtok((char*)tmpBuf, delm);
	while(token!=NULL){
		strcat((char*)tmpBuf2, token);
		token = strtok(NULL, delm);
	}
	gpsLen = strlen((char*)tmpBuf2);
	if(gpsLen > 9)
		tmpBuf2[9] = '\0';
	gpsVal = atoi((char*)tmpBuf2);
	dwbuf[0] = gpsVal/1000000;
	dwbuf[1] = 1;
	dwbuf[2] = (gpsVal%1000000);
	dwbuf[3] = 10000;
	dwbuf[4] = 0;
	dwbuf[5] = 1;
}


/*--------------------------------------------------------------------------*
* Function name : jet_LatiLogi_rational_to_string
* Function		: function for convert latilogi rational to string
* Return value	:  void
* Parameter1	: UINT8 *buf
* Parameter2	: char *str
* History	   : 2017.08.24
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_LatiLogi_rational_to_string(UINT8 *buf, char *str)
{
	UINT32 dwbuf[6];
	UINT8 i,j;
	memset(dwbuf,0, sizeof(dwbuf));
	#if 0
	for(i=0; i<32; i++){
		printf("buf[%d] = %d\n", i, buf[i]);
	}
	#endif
	for(i=0; i<6; i++){
		for(j=0; j<4; j++){
			dwbuf[i] += (((UINT32)buf[i*4+j]) << (8*j));
		}
		//printf("dwbuf[%d] = %u\n", i, dwbuf[i]);
	}
	sprintf(str,"%d%02d.%04d", dwbuf[0]/dwbuf[1], dwbuf[2]/dwbuf[3],dwbuf[2]%dwbuf[3]);
	//printf("[%s] str = %s\n", __FUNCTION__,str);
}


/*--------------------------------------------------------------------------*
* Function name : jet_exif_speed_rational_to_string
* Function		: function for convert speed rational to string
* Return value	:  void
* Parameter1	: UINT8 *buf
* Parameter2	: char *str
* History	   : 2017.09.05
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_exif_speed_rational_to_string(UINT8 *buf, char *str)
{
	UINT32 dwbuf[2];
	UINT8 i,j;
	memset(dwbuf,0, sizeof(dwbuf));
	for(i=0; i<2; i++){
		for(j=0; j<4; j++){
			dwbuf[i] += (((UINT32)buf[i*4+j]) << (8*j));
		}
	}
	sprintf(str,"%05.2f", (float)((float)dwbuf[0]/(float)dwbuf[1]));
	//printf("[%s] str = %s\n", __FUNCTION__,str);
}


/*--------------------------------------------------------------------------*
* Function name : jet_exif_direction_rational_to_string
* Function		: function for convert direction rational to string
* Return value	:  void
* Parameter1	: UINT8 *buf
* Parameter2	: char *str
* History	   : 2017.09.05
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_exif_direction_rational_to_string(UINT8 *buf, char *str)
{
	UINT32 dwbuf[2];
	UINT8 i,j;
	memset(dwbuf,0, sizeof(dwbuf));
	for(i=0; i<2; i++){
		for(j=0; j<4; j++){
			dwbuf[i] += (((UINT32)buf[i*4+j]) << (8*j));
		}
	}
	if(dwbuf[1]<1){
		dwbuf[1]=1;
	}
	sprintf(str,"%03d", dwbuf[0]/dwbuf[1]);
	//printf("[%s] str = %s\n", __FUNCTION__,str);
}


/*--------------------------------------------------------------------------*
* Function name : jet_htoi
* Function		: conver hex string to integer
* Return value	:  UINT64
* Parameter1	: UINT8 *ptr
* History	   : 2017.08.24  scchang
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT64 jet_htoi(UINT8 *ptr)
{
	UINT64 value = 0;
	UINT8 ch = *ptr;
	// Skip leading space & \t
	while (ch == ' ' || ch == '\t')
		ch = *(++ptr);
	while(1) {
		if (ch >= '0' && ch <= '9')
			value = (value << 4) + (ch - '0');
		else if (ch >= 'A' && ch <= 'F')
			value = (value << 4) + (ch - 'A' + 10);
		else if (ch >= 'a' && ch <= 'f')
			value = (value << 4) + (ch - 'a' + 10);
		else
			return value;
		ch = *(++ptr);
	}
}


/*--------------------------------------------------------------------------*
* Function name : jetErrStorageState
* Function		: storage error on SD card
* Return value	:  void
* Parameter1	: ErrSave state
* History	   : 2017.09.12  Isaac Fong
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetErrStorageState(ErrSave state)
{
	UINT32 fd=0;
	UINT8 buf[5]={0};

	if(!IS_CARD_EXIST)
		return;
	fd =	sp5kFsFileOpen((const UINT8*)JET_SD_ERROR_NAME, SP5K_FS_OPEN_RDWR);
	if(fd == 0)
		fd = sp5kFsFileOpen((const UINT8*)JET_SD_ERROR_NAME, SP5K_FS_OPEN_CREATE);
	if(fd ==0 )
		return;

	buf[0] = state & 0xFF00;
	buf[1] = ((state & 0x00FF)>>8);
		
	if(fd){
		fsFileWrite(fd, buf, 5);
		fsFileClose(fd);
	}
}


/*--------------------------------------------------------------------------*
* Function name : jet_query_write_endurance_get
* Function		: get the writting endurance on SD card
* Return value	:  void
* Parameter1	: queryWriteEdurance_t *edu
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_query_write_endurance_get(queryWriteEdurance_t *edu)
{
	appDiskInfo_t* pDiskInfo = appCurDiskInfoGet();
	printf("[%s]\n", __func__);
	memset(edu,0,sizeof(*edu));
	if(!IS_CARD_EXIST){
		edu->ret=0;
		return;
	}
	edu->endurance = (jetSDUseTimeRead()>=2500)? 1:0;
	edu->nWritten = pDiskInfo->dskMbSz * jetSDUseTimeRead();
	edu->ret=1;
	printf("endurance %d, nWritten %llu, ret %d\n", edu->endurance, edu->nWritten, edu->ret);
}


/*--------------------------------------------------------------------------*
* Function name : jetBaseTimeFormUdat
* Function		: storage error on SD card
* Return value	:  BOOL
* Parameter1	: UINT8 *filename
* Parameter2	: UINT32 *base_time
* Parameter3	: UINT32 *event
* History	   : 2017.09.30  Isaac Fong
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
BOOL jetTimeEventFormUdat(UINT8 *filename, UINT32 *base_time, UINT32 *event)
{
    UINT32 fd = 0;
    UINT8 temp[32];
    int index = 0;

    char *token;
    char *delm = ",";

    fd = sp5kFsFileOpen((const UINT8*) filename, SP5K_FS_OPEN_RDONLY);
    if (fd == 0) {
        printf("[%s] open file error\n", __FUNCTION__);
        return FAIL;
    }
    if (jet_file_udata_tag_seek(fd) == FAIL) {
		printf("[%s] seek UDATA fail\n", __FUNCTION__);
        fsFileClose(fd);
        return FAIL;
    }
    fsFileRead(fd, temp, sizeof(temp));
    fsFileClose(fd);
    temp[sizeof(temp) - 1] = '\0';

    token = strtok((char *)temp, delm);

    while (token != NULL) {
#if DEBUG_ON
        printf("%s(%d) %s\n", __func__, __LINE__, token);
#endif
        if (index == 0 && event)
            *event = strtoul(token, NULL, 10);
        if (index == 5) {
            if (base_time) {
                *base_time = strtoul(token, NULL, 16);
            }
            break;
        }
        token = strtok(NULL, delm);
        index++;
    }
#if DEBUG_ON
    printf("[%s]  udat Header TimeBase 0x%X 0x%X\n", __FUNCTION__ , base_time ? *base_time : 0, event ? *event : 0);
#endif
    return SUCCESS;
}


/*--------------------------------------------------------------------------*
* Function name : jetBaseTimeUdatSet
* Function		: Set Udata time to the file
* Return value	:  void
* Parameter1	: UINT8 *filename
* Parameter2	: UINT32 totalsec
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*			   : 2018.04.16  Paul Chang "Remove unused variable "
*--------------------------------------------------------------------------*/
void jetBaseTimeUdatSet(UINT8 * filename  , UINT32 totalsec){
	UINT32 fd=0 ;
	UINT8 temp[32] = {0};
	UINT8 FinalData[30] = {0};
	char * p;
	int index = 0;


	fd = sp5kFsFileOpen((const UINT8*)filename, SP5K_FS_OPEN_RDWR);
	if(fd == 0){
		printf("[%s] open file error %s\n", __FUNCTION__,filename);
		return;
	}
	if(jet_file_udata_tag_seek(fd)==FAIL){
		printf("[%s] seek UDATA fail\n", __FUNCTION__);
		fsFileClose(fd);
		return;
	}
	fsFileRead(fd, temp, sizeof(temp) - 1);

	temp[sizeof(temp) - 1] = '\0';

	while(temp[index] != '\n'){

		index++;
	};
	
    strncpy((char *)FinalData, (char *)temp, index);

    p = strrchr((char *)FinalData, ',');
    p++;

   sprintf(p , "%08X" , totalsec);
   

   fsFileWrite(fd , FinalData, index);
   fsFileClose(fd);
#if DEBUG_ON
    printf("[%s]  udat Header p %s\n", __FUNCTION__ , p);
    printf("[%s]  udat Header totalsec 0x%08X\n", __FUNCTION__ , totalsec);
    printf("[%s]  udat Header FinalData %s\n", __FUNCTION__ , FinalData);
#endif
}


/*--------------------------------------------------------------------------*
* Function name : jetBaseTimeUdatWithStampSet
* Function		: Change Udata time to timestamp
* Return value	:  void
* Parameter1	: UINT8 * buffer
* Parameter2	: UINT32 totalsec
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jetBaseTimeUdatWithStampSet(UINT8 * buffer , UINT32 totalsec){
	char * p;
	
    p = strrchr((char *)buffer, ',');
    p++;

   sprintf(p , "%08X" , totalsec);
	   
#if DEBUG_ON
    printf("[%s]  udat Header  (buffer+index) %s\n", __FUNCTION__ , (buffer) );
    printf("[%s]  udat Header totalsec 0x%08X\n", __FUNCTION__ , totalsec);
#endif
}


/*--------------------------------------------------------------------------*
* Function name : jet_lineout_out
* Function		: Set line out for speaker
* Return value	:  void
* Parameter1	: BOOL on
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_lineout_out(BOOL on)
{
	if(on){
		printf("speaker on\n");
		sp5kAudDevVolumeSet(SP5K_AUD_DEV_PLAY_VOL_GAIN, 0x000000);
		sp5kGpioWrite(SP5K_GPIO_GRP_AUD_LINEOUT, 0x01, 0x01); 
		sp5kGpioDirSet(SP5K_GPIO_GRP_AUD_LINEOUT , 0x01 , 0x01);
	}else{
		printf("speaker off\n");
	 	sp5kAudDevVolumeSet(SP5K_AUD_DEV_PLAY_VOL_GAIN, 0x000F00);
		sp5kGpioWrite(SP5K_GPIO_GRP_AUD_LINEOUT, 0x01, 0x00); 
		sp5kGpioDirSet(SP5K_GPIO_GRP_AUD_LINEOUT , 0x01 , 0x01);		
	}
}


/*--------------------------------------------------------------------------*
* Function name : jetBaseTimeUdatRevise
* Function		: Revise the time to the base time by sec
* Return value	:  UINT32
* Parameter1	: UINT8 * buffer
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
UINT32 jetBaseTimeUdatRevise(UINT8 * buffer ){
	char * p;
	UINT32 tempSec = 0;
    p = strrchr((char *)buffer, ',');
    p++;

	tempSec  = strtoul(p ,NULL , 16);
	tempSec++;
#if DEBUG_ON
    printf("[%s]  udat Header  (buffer+index) %s\n", __FUNCTION__ , (buffer) );
    printf("[%s]  udat Header totalsec 0x%08X\n", __FUNCTION__ , tempSec);
#endif
	return tempSec;

}


/*--------------------------------------------------------------------------*
* Function name : jet_exif_datetime_get
* Function		: function for get the exif of the datatime
* Return value	:  void
* Parameter1	: UINT8 *fname
* Parameter2	: tmx_t *time
* History	   : 
*			   : 2018.04.11  Weikai Hsu "Added Function Header"
*--------------------------------------------------------------------------*/
void jet_exif_datetime_get(UINT8 *fname, tmx_t *time)
{
	//2017:10:04 18:25:39
	extern void appPbImgExifDateTimeGet(UINT8 *pszDTBuf /* min size : 24 */);	
	char *p,str[32];
	UINT8 token_index=0,pflag;
	sp5kExifImageQuery(fname, &pflag);
	appPbImgExifDateTimeGet((UINT8 *)&str);
#if DEBUG_ON
	printf("[%s] fname %s time %s\n",__func__, fname, str);
#endif
	p = strtok(str, " :");
	while(p!=NULL){
		if(token_index==0){
			time->tmx_year = atoi(p);;
		}else if(token_index==1){
			time->tmx_mon = atoi(p);
		}else if(token_index==2){
			time->tmx_mday = atoi(p);
		}else if(token_index==3){
			time->tmx_hour= atoi(p);
		}else if(token_index==4){
			time->tmx_min= atoi(p);
		}else if(token_index==5){
			time->tmx_sec= atoi(p);
		}
		p = strtok(NULL, " :");
		token_index++;
	}
	#if DEBUG_ON 
	printf("[%s] year %d mon %d day %d hr %d min %d sec %d\n",__func__,
		time->tmx_year, time->tmx_mon, time->tmx_mday, time->tmx_hour,
		time->tmx_min, time->tmx_sec);
	#endif
}



