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

 /*--------------------------------------------------------------------------*
 *  File name	: 	App_cdfs_api.c
 *  Function 	: 	Process the CDFS file format 		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#include "app_com_def.h"
#include "app_com_api.h"
#include "app_dbg_api.h"

#include "gpio_custom.h"
#include "sp5k_dcf_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_msg_def.h"
#include "sp5k_disk_api.h"
#include "sp5k_global_api.h"
#include "sp5k_global_api.h"
#include "sp5k_dbg_api.h"

#include "app_cdfs_api.h"
#include "app_video.h"
#include "app_state_ctrl.h"
#include "app_osdfont.h"
#include "app_osd_api.h"
#include "app_icon_def.h"
#include "app_pal.h"
#include "app_view_param.h"
#include "app_still.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_watch_dog.h"
#endif

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
#include "app_still.h"
#endif

#if SP5K_USB_UVC_SUPPORT
#include "app_video_uvc.h"
#endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_infohandlerComm.h"
#include "jet_cdfs.h"
#include "sp5k_media_api.h"
extern void appInfoFileNameEventAdd(CDFS_FILE_TAB_T *pCur);
extern PowerDetect appInfoPowerDETStateGet(void);
extern UINT32 appInfoBatteryLvGet(void);
extern UINT32 appInfoParkingBatterySettingGet(void);
extern void appGsensorLevelCtrl(int mode, int level);
extern UINT8 appInfoParkingGSensorLevelGet(void);
extern void appInfoParkingModeTriStateSet(ParkingModeTrigger state);
#endif

/**************************************************************************
 *						   C O N S T A N T S							*
 **************************************************************************/

/**************************************************************************
 *							  M A C R O S							   *
 **************************************************************************/
#define HOST_DBG		0
#define CDFS_DBG_LOG	0

/*cdfs burst idx limit 999/almost 1s 16.65frame*/

/*file format*/
/*single capture : year+mon+day_hour+min+sec
				   %04d%02d%02d_%02d%02d%02d.ext*/

/*burst capture  : year+mon+day_hour+min+sec_burst idx
				   %04d%02d%02d_%02d%02d%02d_%03d.ext*/

#define EXT_LEN         3
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
#define BURST_CAP_FORMAT_DIFF   5
#else
#define BURST_CAP_FORMAT_DIFF   4
#endif
#define CAP_FILE_LEN    20
#define BURST_FILE_LEN  (CAP_FILE_LEN+BURST_CAP_FORMAT_DIFF)

#define DUAL_TAG_POST   24

/*lock file flow*/
/*del the fail file flow*/ /*for file recover flow*/
#define KB_SIZE                 10          /*(1024)*/
#define MB_SIZE                 20          /*(1024*1024)*/
#define GET_KB_SIZE(val)        (val>>KB_SIZE)
#define GET_MB_SIZE(val)        (val>>MB_SIZE)
#define LOCK_RESERVE_SIZE(val)  (val>>2)    /*reserve (card size/4)*/
#define FAIL_FILE_SIZE          0
#define FAIL_FILE_HEADER_SIZE   512

/**************************************************************************
 *						  D A T A	T Y P E S						  *
 **************************************************************************/
typedef void (*fileOperateCb)(UINT8 *fname);

/**************************************************************************
 *						 G L O B A L	D A T A						 *
 **************************************************************************/
#if SP5K_CDFS_OPT
#if JET_PANA_CDFS
static CDFS_FOLDER_LIST_T UNIFY_FOLDER;
#else
static CDFS_FOLDER_LIST_T CDFS_TAB[CDFS_FILE_FOLDER_MAX];
#endif
static UINT32 CDFS_CUR_FILE_FOLDER;
static UINT32 CDFS_EVENT_OCCUR;

static UINT64 CDFS_VIDEO_SIZE;/*unit: byte*/
static UINT64 CDFS_EVENT_SIZE;
#if defined(PRJ_JETDVR) ||defined(PRJ_17DVR)||defined(PRJ_19DVR)
static UINT64 CDFS_USERRECORD_SIZE;
static UINT64 CDFS_USERSAVE_SIZE;
static UINT64 CDFS_AVNIV_SIZE;
UINT8   CDFS_JPG_FILE_NAME[64];
BOOL CDFS_SET_JPG_FILENAME = FALSE;
#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
static UINT64 *CDFS_FOLDER_SIZE_TABLE[] = {
	&CDFS_VIDEO_SIZE,
	&CDFS_EVENT_SIZE,
	&CDFS_USERRECORD_SIZE,
	&CDFS_USERSAVE_SIZE,
	&CDFS_AVNIV_SIZE,
};
#endif
#define SIZE(x) (sizeof(x) / sizeof(x[0]))
static UINT32 CDFS_DEL_VIDEO_METHOD = CDFS_PROTECTED_SKIP;
static UINT8 CDFS_LAST_FILE_NAME[64];
static UINT8 CDFS_LAST_AVI_NAME[64];
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
static UINT8 CDFS_LAST_JPG_NAME[64];/*SCC*/

static UINT32 LAST_NML;
CDFS_FILE_TAB_T *lastNode = NULL;
#endif

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
static UINT8 CDFS_BACKUP_FILE_NAME[64]={0};
static SP5K_SEMAPHORE filename_sem = 0;
#endif
static UINT8 CDFS_ADD_FILE=0;
static UINT32 cdfsSem = 0;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
SP5K_MUTEX headSem;
#endif

UINT32 fileProtected;
extern tmx_t chgFileTime;
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
extern UINT32 eventListReady;
#endif
extern UINT32 eventFolderRemFlag;
extern UINT32 cardTotalSize;

extern BOOL reboot_flag;
static UINT8 FileListCreateFlag = FALSE;
#if defined(PRJ_17DVR) ||defined(PRJ_19DVR)
extern void appInfoPathGet(UINT8 * fileNameWithPath  , UINT8 * Path);
#endif
/*paul 2018.04.16 add for modify compiler warning*/
extern void memdump(const void *mem, UINT32 len);


static fileOperateCb cdfsFileAddCb = NULL;
static fileOperateCb cdfsFileDelCb = NULL;

/* CDFS memory management */
#define CDFS_MAX_POOL_BASE	8
#define CDFS_BASE_POOL_SZ	256	/* which means allocation will be taken every 256 files adding. */
#define CDFS_UNIT_SIZE		128	/* suppose (sizeof(UINT32) + sizeof(CDFS_FILE_TAB_T)) <=128 */
#if !JET_PANA_CDFS
static UINT32 cdfsMemPoolSize = 1;
static UINT32 cdfsMemPoolMaxSize = CDFS_MAX_POOL_BASE;
static UINT32 cdfsMemPoolUsage = 0;
static void **cdfsMemPool = NULL;
#endif
#if CDFS_DBG_LOG
static UINT32 log_tm = 0, log_cnt = 0, log_max_tm = 0;
#endif

static burstTimeGetCb cdfsFileBurstTimeCb = NULL;
static UINT32 burstNum = 0;
static UINT32 burstCnt = 0;
#endif
#if JET_PANA_CDFS
static cdfs_node_update nodeUpdateCb = NULL;
#endif
/**************************************************************************
 *				 E X T E R N A L	R E F E R E N C E S				 *
 **************************************************************************/
extern UINT32 sp5kDiskFormat(UINT32 drive, UINT8 *pfsName, UINT32 opt);

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)/*weikai modify 180426*/
extern UINT32 UdatHeaderPE;
#endif
/**************************************************************************
 *			   F U N C T I O N	D E C L A R A T I O N S			   *
 **************************************************************************/
#if SP5K_CDFS_OPT

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)


/*--------------------------------------------------------------------------*
 * Function name	: appCdfsBiggestNmlGet
 * Function	       	: Get the last subfolder number by NML folder
 * Return value   	: NML folder number (UINT32)
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
*--------------------------------------------------------------------------*/

UINT32 appCdfsBiggestNmlGet()
{
    return LAST_NML;
}
#endif

#if !JET_PANA_CDFS
/*--------------------------------------------------------------------------*
 * Function name	: cdfsNodeAlloc
 * Function	       	: Allocatting the CDFS memory for a list node
 * Return value   	: CDFS file table pointer (--)
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
*--------------------------------------------------------------------------*/

static CDFS_FILE_TAB_T *
cdfsNodeAlloc(
	void
)
{
	SINT32 i, j;
	CDFS_FILE_TAB_T *ret = NULL;

	#if CDFS_DBG_LOG
	UINT32 t = sp5kMsTimeGet();
	#endif
	/*printf("\n%d, %d, %d\n", cdfsMemPoolUsage, cdfsMemPoolSize, cdfsMemPoolMaxSize);*/
	if (cdfsMemPool == NULL) {
		HOST_ASSERT(CDFS_UNIT_SIZE >= sizeof(UINT32) + sizeof(CDFS_FILE_TAB_T));
		UINT32 s_size = sizeof(void*);
		cdfsMemPool = sp5kMallocCache(cdfsMemPoolMaxSize * sizeof(void*));
		if (!cdfsMemPool) {
			printf("allocate %ux%ux bytes for cdfsNodeAlloc failed\n", cdfsMemPoolSize, cdfsMemPoolMaxSize * /*sizeof(void*)*/s_size);
			HOST_ASSERT(0);
		}
		memset(cdfsMemPool, 0, cdfsMemPoolMaxSize * /*sizeof(void*)*/s_size);
		cdfsMemPool[0] = sp5kMallocCache(CDFS_BASE_POOL_SZ * CDFS_UNIT_SIZE);
		memset(cdfsMemPool[0], 0, CDFS_BASE_POOL_SZ * CDFS_UNIT_SIZE);
	}
	/* expand memory pool */
	if (cdfsMemPoolUsage >= cdfsMemPoolMaxSize * CDFS_BASE_POOL_SZ) {
		UINT32 size = cdfsMemPoolMaxSize * sizeof(void*);
		void *tmp = sp5kMallocCache(size * 2);
		if (!cdfsMemPool) {
			printf("allocate %ux%ubytes for cdfsNodeAlloc failed\n", cdfsMemPoolSize, size * 2);
			HOST_ASSERT(0);
		}
		memcpy(tmp, cdfsMemPool, size);
		memset(tmp + size, 0, size);
		sp5kFree(cdfsMemPool);
		cdfsMemPoolMaxSize *= 2;
		cdfsMemPool = tmp;
		profLogPrintf(0, "expanded cdfs arrary to %d", cdfsMemPoolMaxSize);
	}
	if (cdfsMemPoolUsage >= cdfsMemPoolSize * CDFS_BASE_POOL_SZ) {
		cdfsMemPool[cdfsMemPoolSize] = sp5kMallocCache(CDFS_BASE_POOL_SZ * CDFS_UNIT_SIZE);
		if (!cdfsMemPool[cdfsMemPoolSize]) {
			printf("allocate %dx%dbytes for cdfsNodeAlloc failed\n", CDFS_BASE_POOL_SZ, CDFS_UNIT_SIZE);
			HOST_ASSERT(0);
		}
		memset(cdfsMemPool[cdfsMemPoolSize], 0, CDFS_BASE_POOL_SZ * CDFS_UNIT_SIZE);
		cdfsMemPoolSize++;
		profLogPrintf(0, "added new cdfs pool 0x%p", cdfsMemPool[cdfsMemPoolSize-1]);
	}

	/* free node search */
	for (i = cdfsMemPoolSize - 1 ; i >= 0  ; i--) {
		UINT8 *buf = cdfsMemPool[i] + (CDFS_BASE_POOL_SZ - 1) * CDFS_UNIT_SIZE;
		for (j = CDFS_BASE_POOL_SZ - 1 ; j >= 0 ; j--, buf -= CDFS_UNIT_SIZE) {
			/*printf("%d.%d, [%p]=%d\n", i, j, buf, *buf);*/
			if (*buf == 0) {	/* found */
				ret = (CDFS_FILE_TAB_T*)(buf + sizeof(UINT32));
				*buf = 1;
				cdfsMemPoolUsage++;
				break;
			}
		}
		if (ret)
			break;
	}
	HOST_ASSERT(ret);
	#if CDFS_DBG_LOG
	t = sp5kMsTimeGet() - t;
	if (t > log_max_tm)
		log_max_tm = t;
	log_tm += t;
	log_cnt++;	/* overflow handling? no, just for log. */
	#endif

	return ret;
}



 /*--------------------------------------------------------------------------*
 * Function name	: cdfsNodeFree
 * Function	       	: Free the CDFS memory for a list node
 * Return value   	: void
 * Parameter1    	: CDFS file table pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *                :  2018.06.07  Weikai Hsu     "*buf disable for cppcheck"
 *--------------------------------------------------------------------------*/
static void
cdfsNodeFree(
	void *ptr
)
{
	if (ptr) {
		//UINT8 *buf = ptr - sizeof(UINT32);
		//*buf = 0;
		cdfsMemPoolUsage--;
		if (cdfsMemPoolUsage == 0) {
			/* buffer collection. */
			SINT32 i;
			profLogPrintf(0, "#P#CDFS#N#RmvPool%d", cdfsMemPoolSize);
			for (i = cdfsMemPoolSize - 1 ; i >= 0  ; i--) {
				sp5kFree(cdfsMemPool[i]);
			}
			cdfsMemPoolSize = 1;
			sp5kFree(cdfsMemPool);
			cdfsMemPool= NULL;
		}
	}
}
  /*--------------------------------------------------------------------------*
 * Function name	: cdfsNodeInfoDisp
 * Function	       	: Check and print the CDFS memory pool for a list node
 * Return value   	: void 
 * Parameter1    	: check or not (UINT32  0:not check,1:check)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
static void
cdfsNodeInfoDisp(
	UINT32 checkOnly
)
{
	UINT32 i, j;
	UINT8 *buf;

	if (checkOnly) {	/* correctness check only */
		for (i = 0 ; i < cdfsMemPoolSize ; i++) {
			for (j = 0 ; j < CDFS_BASE_POOL_SZ ; j++) {
				buf = cdfsMemPool[i] + j * CDFS_UNIT_SIZE;
				if (*buf != 0 && *buf != 1) {
					printf("CDFS memory pool broken!:");
					memdump((void *)buf, (UINT32)CDFS_UNIT_SIZE);
					break;
				}
			}
			if (j < CDFS_BASE_POOL_SZ)
				break;
		}
	}
	else {	/* dump all used node */
		for (i = 0 ; i < cdfsMemPoolSize ; i++) {
			for (j = 0 ; j < CDFS_BASE_POOL_SZ ; j++) {
				buf = cdfsMemPool[i] + j * CDFS_UNIT_SIZE;
				if (*buf == 1) {
					CDFS_FILE_TAB_T *node;
					node = (CDFS_FILE_TAB_T *)(buf + sizeof(UINT32));
					HOST_PROF_LOG_PRINT(LEVEL_INFO, "(%04d)   <%02d> %10lld %s", i*CDFS_BASE_POOL_SZ+j, node->FileType, node->FileSize, node->FileName);
				}
				else if (*buf != 0) {
					printf("CDFS memory pool broken!:");
					memdump(buf, CDFS_UNIT_SIZE);
					break;
				}
			}
			if (j < CDFS_BASE_POOL_SZ)
				break;
		}
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "CDFS memory pool:");
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "\tMax pool num = %d, cur = %d", cdfsMemPoolMaxSize, cdfsMemPoolSize);
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "Memory usage:");
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "\tTotal %d bytes (%dx%dx%d)", cdfsMemPoolSize * CDFS_BASE_POOL_SZ * CDFS_UNIT_SIZE, cdfsMemPoolSize, CDFS_BASE_POOL_SZ, CDFS_UNIT_SIZE);
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "\tUsing node %d(%d%%)", cdfsMemPoolUsage, cdfsMemPoolUsage*100/(cdfsMemPoolSize*CDFS_BASE_POOL_SZ));
		#if CDFS_DBG_LOG
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfsNodeAlloc cost:");
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "\tAvg = %dms, Max = %dms", log_cnt?log_tm/log_cnt:0, log_max_tm);
		#endif
	}
}
#endif
  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFolder2Path
 * Function	       	: Setting the CDFS folder name & path
 * Return value   	: result (UINT32 SUCCESS/FAIL) 
 * Parameter1    	: folder type(UINT32 folder)
 * Parameter2    	: path buffer pointer (--) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
UINT32 appCdfsFolder2Path(UINT32 folder,UINT8 *path)
{
	UINT8 driver[5]={0};
	UINT8 folderName[10]={0};

	DBG_PRINT("cdfs: %s", __FUNCTION__);

#if USB_DISK_SUPPORT && !SP5K_USB_UVC_SUPPORT
	if (appActiveDiskGet() == SP5K_DRIVE_USB)
	{
		strcpy((char*)driver,(const char*)"I:/");
	}
	else if(IS_CARD_EXIST)
#else
	if(IS_CARD_EXIST)
#endif
	{
		strcpy((char*)driver,(const char*)"D:/");
	}
	else
	{
		strcpy((char*)driver,(const char*)"C:/");
	}

	switch(folder)
	{
		case CDFS_FILE_FOLDER_VIDEO:
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			strcpy((char*)folderName,(const char*)"DCIM");
#else
			strcpy((char*)folderName,(const char*)"VIDEO");
#endif
			break;
		case CDFS_FILE_FOLDER_JPG:
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			strcpy((char*)folderName,(const char*)"PHOTO");
#else
			strcpy((char*)folderName,(const char*)"JPG");
#endif
			break;
		case CDFS_FILE_FOLDER_EVENT:
			strcpy((char*)folderName,(const char*)"GEVENT");
			break;
#if defined(PRJ_JETDVR) ||defined(PRJ_17DVR)||defined(PRJ_19DVR)
		case CDFS_FILE_FOLDER_USERRECORD:
			strcpy((char*)folderName,(const char*)"MANUAL");
			break;
		case CDFS_FILE_FOLDER_USERSAVE:
			strcpy((char*)folderName,(const char*)"USERSAVE");
			break;
		case CDFS_FILE_FOLDER_ANNIV:
			strcpy((char*)folderName,(const char*)"ANNIV");
			break;
#endif
		default:
			break;
	}
	sprintf((char *)path,"%s%s",driver,folderName);

	return SUCCESS;
}

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsPath2Folder
 * Function	       	: Setting the CDFS folder name & path
 * Return value   	: sub folder count (UINT32 0~3) 
 * Parameter1    	: path buffer pointer (--) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
UINT32 appCdfsPath2Folder(UINT8 *path)
{
	UINT32 ret = 0;
	char *p;

	p = strrchr((char *)path, '/');
	if (p) {
		p++;
		ret = atoi(p);
	}

	return ret;
}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFolderInit
 * Function	       	: Folder initial for CDFS folder
 * Return value   	: result (UINT32 SUCCESS/FAIL) 
 * Parameter1    	: folder type(UINT8 folder) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
UINT32  appCdfsFolderInit(UINT32 folder)/*OK*/
{
	UINT32 ret = SUCCESS;
	UINT8 path[20];

	if (cdfsSem == 0)
		sp5kOsSemaphoreCreate(&cdfsSem, "cdfsSem", 1);

	appCdfsFolder2Path(folder,path);
	DBG_PRINT("cdfs: %s %s\n", __FUNCTION__, path);

	if(sp5kFsDirChange(path)==FAIL)
	{
		sp5kFsDirMake(path);
		ret = sp5kFsDirChange(path);
		if(ret==FAIL)
		{
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s sp5kFsDirChange fail", __FUNCTION__);
        }
	if (cdfsFileAddCb) {
			UINT8 *fname = NULL;
            switch(folder)
            {
                case CDFS_FILE_FOLDER_VIDEO:
		      strcpy((char *)fname, (const char*)"D:\\VIDEO");
                    //fname = "D:\\VIDEO";
                    break;
                case CDFS_FILE_FOLDER_JPG:
		      strcpy((char *)fname, (const char*)"D:\\JPG");
                    //fname = "D:\\JPG";
                    break;
                case CDFS_FILE_FOLDER_EVENT:
		      strcpy((char *)fname,(const char*)"D:\\EVENT");
                   // fname = "D:\\EVENT";
                    break;
#if  defined(PRJ_JETDVR) ||defined(PRJ_17DVR)||defined(PRJ_19DVR)
				case CDFS_FILE_FOLDER_USERRECORD:
					strcpy((char *)fname,(const char*)"D:\\MANUAL");
					//fname = "D:\\MANUAL";
					break;
				case CDFS_FILE_FOLDER_USERSAVE:
					strcpy((char *)fname,(const char*)"D:\\USERSAVE");
					//fname = "D:\\USERSAVE";
					break;
				case CDFS_FILE_FOLDER_ANNIV:
					strcpy((char *)fname,(const char*)"D:\\ANNIV");
					 //fname = "D:\\ANNIV";
				break;
#endif
                default:
                    break;
            }
			if (fname)
				cdfsFileAddCb(fname);
			
		}
	}

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
if(!filename_sem)
    sp5kOsSemaphoreCreate(&filename_sem, "dual_file_name_sem", TX_INHERIT);
#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if (!headSem) {
		sp5kOsMutexCreate(&headSem, "cdfs node", TX_INHERIT);
	} else {
		printf("headSem has create\n");
	}
#endif
	return ret;
}


  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsAddFileStsGet
 * Function	       	: Get the add file status of CDFS
 * Return value   	: CDFS add file flag (UINT32 0,1) 
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/ 
UINT32 appCdfsAddFileStsGet(void)
{
	DBG_PRINT("cdfs: %s", __FUNCTION__);
	return CDFS_ADD_FILE;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsNextFileNameByChGet
 * Function	       	: Get the next file name by change file time
 * Return value   	: result (UINT32 SP5K_ERR_SUCCESS/SP5K_ERR_DCF_PARAM_GROUP) 
 * Parameter1    	: channel Name pointer (--)
 * Parameter2    	: file type (UINT32 0x0001~0x8000)
 * Parameter3    	: File Name pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/ 
UINT32
appCdfsNextFileNameByChGet(
	UINT8 *channelName,
	UINT32 filetype,
	UINT8 *pFileName
)
{
	UINT32 ret = SP5K_ERR_SUCCESS;

	HOST_ASSERT(pFileName);

	/*burst yyyymmdd_hhmmss_xx*/
	if (cdfsFileBurstTimeCb)
	{
		cdfsFileBurstTimeCb(&chgFileTime, burstCnt);
		sprintf((char *)pFileName,"%s/%04d%02d%02d_%02d%02d%02d_%02u", sp5kFsGetCurPath(),
																chgFileTime.tmx_year+1900,
																chgFileTime.tmx_mon,
																chgFileTime.tmx_mday,
																chgFileTime.tmx_hour,
																chgFileTime.tmx_min,
																chgFileTime.tmx_sec,
																burstCnt);
		burstCnt ++;
		if(burstNum==burstCnt)
		{
			burstCnt = 0;
			cdfsFileBurstTimeCb = NULL;
		}
	}
	/*single capture yyyymmdd_hhmmss*/
	else
	{	
		sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &chgFileTime);
		sprintf((char *)pFileName,"%s/%04d%02d%02d_%02d%02d%02d", sp5kFsGetCurPath(),
																chgFileTime.tmx_year+1900,
																chgFileTime.tmx_mon,
																chgFileTime.tmx_mday,
																chgFileTime.tmx_hour,
																chgFileTime.tmx_min,
																chgFileTime.tmx_sec);
	}
	
	appCdfsFileNameExistCheck(pFileName);/*It's  for resolve same name file is covered by new file*/
	if (channelName)
		strcat((char *)pFileName, (char *)channelName);

	switch(filetype)
	{
		case SP5K_DCF_FILETYPE_JPG:
			strcat((char *)pFileName, ".JPG");
			break;
		case SP5K_DCF_FILETYPE_AVI:
			strcat((char *)pFileName, ".AVI");
			break;
		case SP5K_DCF_FILETYPE_MOV:
			strcat((char *)pFileName, ".MOV");
			break;
		case SP5K_DCF_FILETYPE_MP4:
			strcat((char *)pFileName, ".MP4");
			break;
		default:
			HOST_ASSERT(0);
			ret = SP5K_ERR_DCF_PARAM_GROUP;
			break;
	}

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s %s", __FUNCTION__, pFileName);

	return ret;
}


  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsNextFileNameGet
 * Function	       	: Get the next file name of CDFS
 * Return value   	: result (UINT32 SP5K_ERR_SUCCESS/SP5K_ERR_DCF_PARAM_GROUP) 
 * Parameter1    	: file type (UINT32 0x0001~0x8000)
 * Parameter2    	: File Name pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *                :  2018.04.16  Paul.Chang     "remove unused variable"
 *--------------------------------------------------------------------------*/ 

UINT32 appCdfsNextFileNameGet(UINT32 filetype,UINT8 *pFileName)/*OK*/
{
	UINT8 fname[64]={0};
	UINT32 ret = SP5K_ERR_SUCCESS;

    #if SP5K_USB_UVC_SUPPORT
    UINT32 cap_ret = appUvcCapModeStsGet();
    #endif
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: %s", __FUNCTION__);
	if (pFileName == NULL)
	{
		return SP5K_ERR_DCF_PARAM_GROUP;
	}
	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &chgFileTime);
    #if SP5K_SENSOR_DUAL_MODE
    if(appGetYUVSaveStatus())
    {
    	sprintf((char *)fname,"%s/%04d%02d%02d_%02d%02d%02dS",
    				sp5kFsGetCurPath(),
    				chgFileTime.tmx_year+1900,
    				chgFileTime.tmx_mon,
    				chgFileTime.tmx_mday,
    				chgFileTime.tmx_hour,
    				chgFileTime.tmx_min,
    				chgFileTime.tmx_sec);
        strcat((char *)fname, ".JPG");
    	strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
        if(appDualPipModeGet() > DUALSEN_SINGLE_SEC)
        	strcpy((char*)CDFS_BACKUP_FILE_NAME,(const char*)fname);
    }
	else
	{
        if(appDualPipModeGet()==DUALSEN_SINGLE_MAIN)
        {
            appCdfsNextFileNameByChGet("M", filetype, fname);
        	strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
        }
        else if(appDualPipModeGet()==DUALSEN_SINGLE_SEC)
        {
            appCdfsNextFileNameByChGet("S", filetype, fname);
        	strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
        }
        else if(appVideoMultiTrackGet() == APP_VIDEO_SINGLE_FILE && filetype!=SP5K_DCF_FILETYPE_JPG) /*For Multi-track use, but exclude capture mode.*/
        {
            appCdfsNextFileNameByChGet("MT", filetype, fname);
        	strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
        }
		else
		{
            sp5kOsSemaphoreGet(&filename_sem, TX_WAIT_FOREVER);
            if(CDFS_BACKUP_FILE_NAME[0]==0)
            {
				appCdfsNextFileNameByChGet("M", filetype, fname);
            	strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
            	strcpy((char*)CDFS_BACKUP_FILE_NAME,(const char*)fname);
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: M: rtc-%s\n", fname);
            }
            else
            {
            	sprintf((char *)fname,"%s",CDFS_BACKUP_FILE_NAME);  /*SEC*/
                memset(CDFS_BACKUP_FILE_NAME, 0, sizeof(UINT8)*32);
            	switch(filetype)
            	{
            		case SP5K_DCF_FILETYPE_JPG:
						if(strlen(fname) >= 30)
						{
							fname[DUAL_TAG_POST+1] = 'M';
						}
						else
						{
	                    	fname[DUAL_TAG_POST-2] = 'M';
						}
            			break;
            		case SP5K_DCF_FILETYPE_AVI:
            		case SP5K_DCF_FILETYPE_MOV:
						if(strlen(fname) >= 32)
						{
							fname[DUAL_TAG_POST+3] = 'M';
						}
						else
						{
	                    	fname[DUAL_TAG_POST] = 'M';
						}
	            			break;
					default:
						break;
	            		}
                		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: M: non rtc-%s\n", fname);
			}
			sp5kOsSemaphorePut(&filename_sem);
        	}
	}
	strcpy((char*)pFileName, (const char*)fname);
#elif SP5K_USB_UVC_SUPPORT
    if(cap_ret)
    {
    	static int cap_sec, cap_min, cap_hr, cap_day, cap_mon, cap_year ;
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s cap flow: %d", __FUNCTION__, cap_ret);
		if(appCdfsFileFolderGet()!=CDFS_FILE_FOLDER_JPG)
		{
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "cdfs: %s set jpg folder", __FUNCTION__);
			appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
			appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
		}
        if(cap_ret==1)
        {
            sprintf((char *)fname,"%s/%04d%02d%02d_%02d%02d%02dM",
                    sp5kFsGetCurPath(),
                    cap_year+1900,
                    cap_mon,
                    cap_day,
                    cap_hr,
                    cap_min,
                    cap_sec);
        }
        else
        {
        	cap_sec = chgFileTime.tmx_sec;
			cap_min = chgFileTime.tmx_min;
			cap_hr = chgFileTime.tmx_hour;
			cap_day = chgFileTime.tmx_mday;
			cap_mon = chgFileTime.tmx_mon;
			cap_year = chgFileTime.tmx_year;
            sprintf((char *)fname,"%s/%04d%02d%02d_%02d%02d%02dS",
                    sp5kFsGetCurPath(),
                    chgFileTime.tmx_year+1900,
                    chgFileTime.tmx_mon,
                    chgFileTime.tmx_mday,
                    chgFileTime.tmx_hour,
                    chgFileTime.tmx_min,
                    chgFileTime.tmx_sec);     /*fix bug ICOM-2413*/
        }
        strcat((char *)fname, ".JPG");
        strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
    }
    else
    {
		if(appUvcHostCfgGet(UVC_HOST_CFG_PLUG_IN))/*uvc cam plug in*/
		{
            sp5kOsSemaphoreGet(&filename_sem, TX_WAIT_FOREVER);
            if(CDFS_BACKUP_FILE_NAME[0]==0)
            {
				appCdfsNextFileNameByChGet("M", filetype, fname);
            	strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
            	strcpy((char*)CDFS_BACKUP_FILE_NAME,(const char*)fname);
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: M: rtc-%s\n", fname);
            }
            else
            {
            	sprintf((char *)fname,"%s",CDFS_BACKUP_FILE_NAME);  /*SEC*/
                memset(CDFS_BACKUP_FILE_NAME, 0, sizeof(UINT8)*32);
            	switch(filetype)
            	{
            		case SP5K_DCF_FILETYPE_JPG:
						if(strlen(fname) >= 30)
						{
							fname[DUAL_TAG_POST+1] = 'M';
						}
						else
						{
	                    	fname[DUAL_TAG_POST-2] = 'M';
						}
            			break;
            		case SP5K_DCF_FILETYPE_AVI:
            		case SP5K_DCF_FILETYPE_MOV:
						if(strlen(fname) >= 32)
						{
							fname[DUAL_TAG_POST+3] = 'M';
						}
						else
						{
	                    	fname[DUAL_TAG_POST] = 'M';
						}
            			break;
			default:
				break;
            	}
                HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: M: non rtc-%s\n", fname);
            }
            sp5kOsSemaphorePut(&filename_sem);
        }
        else /*uvc cam plug out*/
        {
			sp5kOsSemaphoreGet(&filename_sem, TX_WAIT_FOREVER);
            appCdfsNextFileNameByChGet("M", filetype, fname);
			strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
			if(filetype == SP5K_DCF_FILETYPE_MOV)
			{
            	strcpy((char*)CDFS_BACKUP_FILE_NAME,(const char*)fname);
			}
            sp5kOsSemaphorePut(&filename_sem);
        }
    }
	strcpy((char*)pFileName, (const char*)fname);

/*other cdfs flow*/
#else

#if JET_PANA_CDFS
	if(filetype == SP5K_DCF_FILETYPE_JPG){	

		if(CDFS_SET_JPG_FILENAME)
			sprintf((char *)fname,"D:/PHOTO/%s.JPG", CDFS_JPG_FILE_NAME);		
		else	
			sprintf((char *)fname,"%s", jet_cdfs_next_jpg_filename_get(chgFileTime));		
		printf("fname = %s\n", fname);
	}
	else{
		sprintf((char *)fname,"%s", 	jet_cdfs_next_video_filename_get(chgFileTime));		
		printf("fname = %s\n", fname);	
	}
	/*dw++@20120519. need copy path to des buffer.*/	
	strcpy((char*)pFileName, (const char*)fname);	
	if(filetype==SP5K_DCF_FILETYPE_JPG)		
		strcpy((char*)CDFS_LAST_JPG_NAME,(const char*)fname);	
	else			
		strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
#else
	appCdfsNextFileNameByChGet(NULL, filetype, fname);
	strcpy((char*)pFileName, (const char*)fname);
	strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
#endif
#endif

    	HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s %s", __FUNCTION__, pFileName);
	return ret;
}


  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsLastFileNameGet
 * Function	       	: Get the last file name of CDFS
 * Return value   	: file name pointer(--) 
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/ 
UINT8 * appCdfsLastFileNameGet(void)
{
	DBG_PRINT("cdfs: %s", __FUNCTION__);
	return CDFS_LAST_FILE_NAME;
}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsLastFileNameSet
 * Function	       	: Set the last file name of CDFS
 * Return value   	: void 
 * Parameter1    	: file name pointer(--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/ 
void appCdfsLastFileNameSet(UINT8 *fname)//sc added
{	
	DBG_PRINT("cdfs: %s", __FUNCTION__);	
	strcpy((char *)CDFS_LAST_FILE_NAME,(char *) fname);
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsLastJpgNameGet
 * Function	       	: Set the last Jpg file name of CDFS
 * Return value   	: last jpg name pointer (--)
 * Parameter1    	: void 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/ 
UINT8 * appCdfsLastJpgNameGet(void)//SC added
{	
	DBG_PRINT("cdfs: %s", __FUNCTION__);	
	return CDFS_LAST_JPG_NAME;
}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsLastAviNameGet
 * Function	       	: Set the last AVI file name of CDFS
 * Return value   	: last AVI name pointer (--)
 * Parameter1    	: void 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/ 
UINT8 * appCdfsLastAviNameGet(void)
{
	DBG_PRINT("cdfs: %s", __FUNCTION__);
	return CDFS_LAST_AVI_NAME;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsLastAviNameGet
 * Function	       	: Set the last AVI file name of CDFS
 * Return value   	: result (SUCCESS/FAIL)
 * Parameter1    	: full path pointer (--)
 * Parameter2    	: file name pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/ 
UINT32 appCdfsAbsolutePathToFile(UINT8 *fullpath, UINT8 *filename)
{
	char *pfname;
	DBG_PRINT("cdfs: %s", __FUNCTION__);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	pfname = strrchr((char*)fullpath, '\\');	
	if(pfname==NULL){
		pfname = strrchr((char*)fullpath, '/');
	}
	if(pfname!=NULL)		
		strcpy((char*)filename,(const char*)(pfname+1));
	else
		strcpy((char*)filename,(char*)fullpath);//test
#else
	pfname = strrchr((char *)fullpath, '/');
	if (pfname) {
		strcpy((char *)filename, (char *)pfname+1);
	}
	else {
		strcpy((char *)filename, (char *)fullpath);
	}
#endif
	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsEventProcess
 * Function	       	: Cdfs Event Process
 * Return value   	: result (SUCCESS/FAIL)
 * Parameter1    	: file name pointer (--)
 * Parameter2    	: event (UINT32 0x01,0x10)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/  
UINT32 appCdfsEventProcess(UINT8 *filename,UINT32 event)
{
	UINT8 src[64]={0}, path_src[64]={0};
	UINT8 dst[64]={0}, path_dst[64]={0};
	DBG_PRINT("cdfs: %s", __FUNCTION__);

	appCdfsFolder2Path(CDFS_FILE_FOLDER_VIDEO,path_src);
	if(event == JPG_EVENT_IN_VIDEO)
	{
		appCdfsFolder2Path(CDFS_FILE_FOLDER_JPG,path_dst);

	}
	else if(event == VIDEO_EVENT_IN_VIDEO)
	{
		appCdfsFolder2Path(CDFS_FILE_FOLDER_EVENT,path_dst);
	}
	sprintf((char *)src,"%s/%s",path_dst,filename);
	sprintf((char *)dst,"%s/%s",path_dst,filename);
	sp5kFsRename(src,dst);

	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileNameToFileType
 * Function	       	: Check file type by file name
 * Return value   	: file type (UINT32 0x0000~0x8000)
 * Parameter1    	: file name pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
UINT32 appCdfsFileNameToFileType(UINT8 *filename)
{
    char *p = strchr((char *)filename, '.');

	DBG_PRINT("cdfs: %s %s", __FUNCTION__, filename);
    	if(p)
	{
		char ch = *(p+1);
    	if (((ch == 'J')||(ch == 'j')))
    	{
    		return SP5K_DCF_FILETYPE_JPG;
    	}
	else if ((*(p+1) == 'M')||((*(p+1) == 'm')&&(*(p+3) == '4') ))    	
	{    		
	 	return SP5K_DCF_FILETYPE_MP4;    	
	}
    	else if ((ch == 'M')||(ch == 'm'))
    	{
    		return SP5K_DCF_FILETYPE_MOV;
    	}
    	else if ((ch == 'A')||(ch == 'a'))
    	{
    		return SP5K_DCF_FILETYPE_AVI;
    	}
	}

	HOST_PROF_LOG_PRINT(LEVEL_INFO,"Function:%s Line:%u, The %s is unknown type...\n",__FUNCTION__,__LINE__, filename);
	__FUNC_TRACK__;

	return 0;
}

/*dual cdfs flow*/
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsNextSubFileNameGet
 * Function	       	: Get next subfile name
 * Return value   	: result (UINT32 SP5K_ERR_SUCCESS/SP5K_ERR_DCF_PARAM_GROUP)
 * Parameter1    	: file type (UINT32)
 * Parameter2    	: subfile type (UINT32)
 * Parameter3    	: file name pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
UINT32 appCdfsNextSubFileNameGet(UINT32 fileType,UINT32 subFileType,UINT8 *pFileName)
{
	UINT32 ret = SP5K_ERR_SUCCESS;
	UINT8 fname[64]={0};

	sp5kOsSemaphoreGet(&filename_sem, TX_WAIT_FOREVER);
	if(CDFS_BACKUP_FILE_NAME[0]==0)
	{
		appCdfsNextFileNameByChGet("S", fileType, fname);
		strcpy((char*)CDFS_LAST_FILE_NAME,(const char*)fname);
		strcpy((char*)CDFS_BACKUP_FILE_NAME,(const char*)fname);
	    	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: S: rtc-%s\n", fname);
	}
    	else
    	{
    		sprintf((char *)fname,"%s",CDFS_BACKUP_FILE_NAME);  /*SEC*/
	        memset(CDFS_BACKUP_FILE_NAME, 0, sizeof(UINT8)*32);
			if(strlen(fname) >= 32)
			{
				fname[DUAL_TAG_POST+3] = 'S';
			}
			else
			{
		    	fname[DUAL_TAG_POST] = 'S';
			}
	        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: S: non rtc-%s\n", fname);
    	}
    	sp5kOsSemaphorePut(&filename_sem);

	strcpy((char*)pFileName, (const char*)fname);
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: daul %s %s", __FUNCTION__, pFileName);
	return ret;
}
#endif

#if JET_PANA_CDFS

 /*--------------------------------------------------------------------------*
 * Function name	: isFolder
 * Function	       	: Check currently video folder
 * Return value   	: result (BOOL true/false)
 * Parameter1    	: current CDFS file table pointer (--)
 * Parameter2    	: condition CDFS file table pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
static BOOL  isFolder(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pCond){	
	UINT32 i;

	i = pCur->folder;        /* Check NORMAL video folder */
	if (i >= JET_CDFS_DCIM_SUBFOLDER_START_INDEX)
		i = CDFS_FILE_FOLDER_VIDEO_BIT;
	return (i & pCond->folder) != 0;
}

#if 0
UINT32 appCdfsNodeIndexGet(CDFS_FILE_TAB_T *pTarget)
{
	UINT32 ret = 0;
	int count = 0;
	CDFS_FILE_TAB_T *pHead, *pCur;

	pHead = UNIFY_FOLDER.head_node;
	pCur = 
	if (pCur == NULL || pHead == NULL)
		return ret;	
	pCur = pHead;	
	while (pCur != pTarget) {		
		count++;		
		if (pCur->next == pHead) {			/* Not found !*/			
			count = -1;			
			break;		
		}		
		pCur = pCur->next;	
		}	
	ret = count + 1;	
	return ret;
}
#endif

 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsNextNodeFind
 * Function	       	: Find the next node of CDFS file
 * Return value   	: CDFS file table pointer (--)
 * Parameter1    	: current CDFS file table pointer (--)
 * Parameter2    	: condition CDFS file table pointer (--)
 * Parameter3    	: cdfs compare value (true/false) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
CDFS_FILE_TAB_T *appCdfsNextNodeFind(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pCond, cdfs_compare cmp)
{
	CDFS_FILE_TAB_T *pHead, *ret = NULL;

	pHead = UNIFY_FOLDER.head_node;
#if DEBUG_ON
	if(pHead->folder != NULL)
		printf("%s(%d) - 0X%04X \n", __func__, __LINE__, pHead->folder);
	else
		printf("%s(%d) - NULL\n", __func__, __LINE__);
#endif
	if (pHead == NULL || pCond == NULL)
		goto out;    
	if (pCur == NULL)        
		pCur = pHead;   
	else if (pCur->next == pHead)
		goto out;
	else
		pCur = pCur->next;

	while (1) {
		if (cmp(pCur, pCond) == TRUE) {
			ret = pCur;
			break;
		}

		pCur = pCur->next;
		if (pCur == pHead)
			break;
	};
out:
	return ret;
}

 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsPrevNodeFind
 * Function	       	: Find the previous node of CDFS file
 * Return value   	: CDFS file table pointer (--)
 * Parameter1    	: current CDFS file table pointer (--)
 * Parameter2    	: condition CDFS file table pointer (--)
 * Parameter3    	: cdfs compare value (true/false) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/ 
CDFS_FILE_TAB_T *appCdfsPrevNodeFind(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pCond, cdfs_compare cmp)
{
	CDFS_FILE_TAB_T *pHead, *ret = NULL;

	pHead = UNIFY_FOLDER.head_node;

	if (pHead == NULL || pCond == NULL)
		goto out;    
	if (pCur == NULL)
		pCur = pHead;   
	else if (pCur == pHead)
		goto out;
	else
		pCur = pCur->pre;

	while (1) {       
		if (cmp(pCur, pCond) == TRUE) {           
			ret = pCur;            
			break;        
		}
		if (pCur == pHead)            
			break;        
		pCur = pCur->pre;    
	};
out:
		return ret;
}

 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFolderNodeFind
 * Function	       	: Find the  node of CDFS file
 * Return value   	: CDFS file table pointer (--)
 * Parameter1    	: folder number (UINT32)
 * Parameter2    	: current CDFS file table pointer  (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/  
CDFS_FILE_TAB_T *appCdfsFolderNodeFind(UINT32 folder, CDFS_FILE_TAB_T *pCur)
{    
	CDFS_FILE_TAB_T cond;    
	memset(&cond, 0, sizeof(cond));    
	cond.folder = folder;   
	return appCdfsNextNodeFind(pCur, &cond, isFolder); 
}

 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsTableHeadGet
 * Function	       	: Get the folder head node
 * Return value   	: CDFS file table pointer (--)
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
CDFS_FILE_TAB_T *appCdfsTableHeadGet()
{    
	return UNIFY_FOLDER.head_node;
}
 
 /*--------------------------------------------------------------------------*
 * Function name	: add_prefix_node
 * Function	       	: Add prefix node
 * Return value   	: void
 * Parameter1    	: current CDFS file table pointer  (--)
 * Parameter2    	: New CDFS file table pointer  (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/  
inline void add_prefix_node(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pNew)
{    
	pNew->pre = pCur->pre;    
	pNew->pre->next = pNew;    
	pCur->pre = pNew;    
	pNew->next = pCur;
}

 /*--------------------------------------------------------------------------*
 * Function name	: add_postfix_node
 * Function	       	: Add postfix node
 * Return value   	: void
 * Parameter1    	: current CDFS file table pointer  (--)
 * Parameter2    	: New CDFS file table pointer  (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
inline void add_postfix_node(CDFS_FILE_TAB_T *pCur, CDFS_FILE_TAB_T *pNew)
{    
	pNew->next = pCur->next;   
	pCur->next = pNew;    
	pNew->pre = pCur;    
	pNew->next->pre = pNew;
}

#endif

 /*--------------------------------------------------------------------------*
 * Function name	: AddFile
 * Function	       	: CDFS file add
 * Return value   	: result (UINT32 SP5K_ERR_SUCCESS/SP5K_ERR_DCF_PARAM_GROUP)
 * Parameter1    	: file name pointer  (--)
 * Parameter2    	: file attribute (UINT32)
 * Parameter3    	: file size  (UINT64)
 * Parameter4    	: dirPath pointer (--) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/    
 #if JET_PANA_CDFS
 static UINT32 AddFile(UINT8 *filename, UINT32 attr, UINT64 filesize, UINT8* dirPath)
 #else
static UINT32 AddFile(UINT8 *filename, UINT32 attr, UINT64 filesize)
#endif
{
	CDFS_FILE_TAB_T *pNew=NULL;
	CDFS_FILE_TAB_T *pCur=NULL;
#if !JET_PANA_CDFS
	CDFS_FILE_TAB_T *pNext=NULL;
#endif
	CDFS_FILE_TAB_T *pHead=NULL;
	UINT32 fileCount=0;
	UINT32 filetype;

	static CDFS_FILE_TAB_T *last_insert_video = NULL;
	static CDFS_FILE_TAB_T *last_insert_jpg = NULL;
#if JET_PANA_CDFS    
	UINT8 fileNameTemp[64]={0};
	UINT32 subfileCount = 0;    
	int folder = 0;    
/*	char strtmp[64];*/
#endif	
	DBG_PRINT("cdfs: %s %x", __FUNCTION__,filename);

	sp5kOsSemaphoreGet(&cdfsSem, TX_WAIT_FOREVER);
	/*del the fail file flow*/
	if ((filesize == FAIL_FILE_SIZE) && (FileListCreateFlag==TRUE))
	{
		sp5kFsFileDelete(filename);
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "cdfs: file fail %s %lld", filename, filesize);
		sp5kOsSemaphorePut(&cdfsSem);

		return SP5K_ERR_SUCCESS;
	}

#if JET_PANA_CDFS

	appCdfsAbsolutePathToFile(filename ,fileNameTemp);
		
#endif
	filetype = appCdfsFileNameToFileType(filename);

    #if 0
	if ((CDFS_EVENT_OCCUR & JPG_EVENT_IN_VIDEO)
		&& ((filetype & SP5K_DCF_FILETYPE_AVI)
		 || (filetype & SP5K_DCF_FILETYPE_MOV)))
	{
		HOST_PROF_LOG_ADD(LEVEL_INFO, "cdfs: avoid adding video file when adding jpg");
		CDFS_ADD_FILE=1;
		strcpy((char*)CDFS_LAST_AVI_NAME,(char*)filename);
		sp5kOsSemaphorePut(&cdfsSem);

		return 0;/*avoid adding video file when adding jpg*/
	}

	if ((CDFS_EVENT_OCCUR & VIDEO_EVENT_IN_VIDEO)
		&& ((filetype & SP5K_DCF_FILETYPE_AVI)
		 || (filetype & SP5K_DCF_FILETYPE_MOV)))
	{
		CDFS_CUR_FILE_FOLDER = CDFS_FILE_FOLDER_EVENT;
	}
	else if ((CDFS_EVENT_OCCUR & JPG_EVENT_IN_VIDEO)
			&& (filetype & SP5K_DCF_FILETYPE_JPG))
	{
		HOST_PROF_LOG_ADD(LEVEL_INFO, "cdfs: video capture image");
		CDFS_CUR_FILE_FOLDER = CDFS_FILE_FOLDER_JPG;
	}
    #endif

    /*get table header*/
#if JET_PANA_CDFS
	/*Cause in the PANA_CDFS mode , need sort all file with time*/
	pHead = UNIFY_FOLDER.head_node;
	fileCount = UNIFY_FOLDER.folder_info.file_num;
	pNew = sp5kMalloc(sizeof(CDFS_FILE_TAB_T));
	memset(pNew, 0, sizeof(CDFS_FILE_TAB_T));
#else
	pHead = appCdfsFileListHeadGet(CDFS_CUR_FILE_FOLDER);
	fileCount = CDFS_TAB[CDFS_CUR_FILE_FOLDER].folder_info.file_num;
	pNew = cdfsNodeAlloc();
#endif

	HOST_ASSERT(pNew);

	/****************************************/
#if JET_PANA_CDFS
	strncpy((char *) pNew->FileName, (const char *)fileNameTemp, sizeof(pNew->FileName));
	
#else
	strncpy((char *)pNew->FileName,(const char *)filename,sizeof(pNew->FileName));
#endif
	if (attr & SP5K_FS_ATTR_READONLY)
	{
		pNew->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
	}
	else if (CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
	{
		pNew->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
		if (!(attr | SP5K_FS_ATTR_READONLY)) {
			attr |= SP5K_FS_ATTR_READONLY;
			sp5kFsFileAttrSet(filename, attr);
		}
	}
	
	pNew->attr = attr;
	pNew->FileSize = filesize;
	pNew->FileType = filetype;
	pNew->next = pNew->pre = NULL;

#if JET_PANA_CDFS
	folder = 1 << CDFS_CUR_FILE_FOLDER;
	subfileCount = UNIFY_FOLDER.folder_info.file_num_in_subfolder[CDFS_CUR_FILE_FOLDER];
	*CDFS_FOLDER_SIZE_TABLE[CDFS_CUR_FILE_FOLDER] += filesize;
	if (folder == (1 << CDFS_FILE_FOLDER_VIDEO)){
		//paul modify 2018/0328
		 //folder = appCdfsPath2Folder(filename);
		folder = appCdfsPath2Folder(dirPath);   
		}
	pNew->folder = folder;
	DBG_PRINT("%s (%d) Folder %d Total size %lld File size : %lld\n",__FUNCTION__ ,__LINE__ , CDFS_CUR_FILE_FOLDER,*CDFS_FOLDER_SIZE_TABLE[CDFS_CUR_FILE_FOLDER] , filesize );
#endif
	/****************************************/
	
	fileCount++;

#if JET_PANA_CDFS
	subfileCount++;
#endif

#if !JET_PANA_CDFS

	if (pNew->FileSize > 0)
	{
		if (CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
		{
    		if (CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
    		{
    			if (!(pNew->dcfattr & SP5K_DCF_FILE_ATTR_LOCK))
    			{
    				CDFS_VIDEO_SIZE += (UINT64)pNew->FileSize;
    			}
    		}
    		else
			{
				CDFS_VIDEO_SIZE+=pNew->FileSize;
			}
			/*__FUNC_TRACK__;*/
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: size = %u(B),%u(MB)"\
				,(UINT32)CDFS_VIDEO_SIZE,((UINT32)CDFS_VIDEO_SIZE)>>20);
		}
		else if (CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
		{
			CDFS_EVENT_SIZE += pNew->FileSize;
		}
	}
#endif

	/*****************************************/
	if (pHead == NULL)/*create first object of table*/
	{
		/*add file@head*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		pNew->next = pNew;/*paul 2018.04.20 fixed bug#3426 #3435*/
#else
		pNew->next = NULL;
#endif
		pNew->pre = pNew;
		pHead = pNew;
	}
	else
	{
	#if !JET_PANA_CDFS	
		int try_insert = 1;
	#endif
		CDFS_FILE_TAB_T *last_insert;
	

		switch(CDFS_CUR_FILE_FOLDER){
			case CDFS_FILE_FOLDER_VIDEO:
				last_insert = last_insert_video;
				break;
			case CDFS_FILE_FOLDER_JPG:
				last_insert = last_insert_jpg;
				break;
			default:
				last_insert = NULL;
				break;
		}
#if !JET_PANA_CDFS		
		if(pCur == NULL)
			pCur = pNext = pHead->pre;
#else
		pCur = pHead;
#endif

		while(1)
		{
			
			SINT32 result = strcmp((const char *)pNew->FileName,(const char *)pCur->FileName);
			
#if JET_PANA_CDFS
			if (result < 0) { /*paul 2018.04.20 fixed bug#3426 #3435*/
			    add_prefix_node(pCur, pNew);
			    if (pCur == pHead)
			        pHead = pNew;
			    break;
			} else if (result == 0)
			{
			 	if (pNew->folder == pCur->folder) {
				        pCur->FileSize = pNew->FileSize;
				        pCur->attr = pNew->attr;
				        pCur->dcfattr = pNew->dcfattr;
				        fileCount--;

			        	sp5kFree(pNew);
			    	} else {
			        	add_postfix_node(pCur, pNew);
			    	}
			    	break;
			}

			if (pCur->next == pHead) {
				add_postfix_node(pCur, pNew);
			    	break;
			}
			pCur = pCur->next;
		      
		        switch (CDFS_CUR_FILE_FOLDER) {
			        case CDFS_FILE_FOLDER_VIDEO:
			            last_insert_video = pNew;
			            break;
			        case CDFS_FILE_FOLDER_JPG:
			            last_insert_jpg = pNew;
			            break;
			        default:
			            break;
		        }			
#else
			if (result > 0)/*sort by date*/
			{
				/*add pNew after pCur*/
				pCur->next = pNew;
				pNew->pre = pCur;
				if (pCur == pNext)
				{/*pNew is the last*/
					pNew->next = NULL;
					pHead->pre = pNew;
				}
				else if(pNext != NULL)
				{/*insert pNew between pCur and pNext*/
					pNew->next = pNext;
					pNext->pre = pNew;
				}
				break;
			}
			else if(result == 0)/*the same name*/
			{/*the new file replace the old file when two file name is same*/
				pCur->FileSize = pNew->FileSize;
				pCur->attr = pNew->attr;
				pCur->dcfattr = pNew->dcfattr;
				fileCount--;
				cdfsNodeFree(pNew);
				pNew = pCur;
				break;
			}
			else
			{
				if (pCur != pHead)
				{
					pNext = pCur;
					pCur = pCur->pre;

					if(try_insert && last_insert)
					{
						try_insert = 0;
						if(strcmp((const char *)pNew->FileName,(const char *)last_insert->FileName) >= 0)
						{
							if(last_insert->next)
							{
								if(strcmp((const char *)pNew->FileName,(const char *)last_insert->next->FileName)<0)
								{
									pCur = last_insert;
									pNext = last_insert->next;

									/* insert */
									pCur->next = pNew;
									pNew->pre = pCur;
									pNew->next = pNext;
									pNext->pre = pNew;

									switch(CDFS_CUR_FILE_FOLDER){
										case CDFS_FILE_FOLDER_VIDEO:
											last_insert_video = pNew;
											break;
										case CDFS_FILE_FOLDER_JPG:
											last_insert_jpg = pNew;
											break;
										default:									
											break;
									}

									break;

									//HOST_PROF_LOG_PRINT(LEVEL_WARN, "ins : %s",  pCur->FileName);
									//HOST_PROF_LOG_PRINT(LEVEL_WARN, "new : %s",  pNew->FileName);
								}
							}
						}
							
					}
					
				}
				else
				{
					/*add pNew as new head*/
					pNew->pre = pHead->pre;
					pNew->next = pHead;
					pHead->pre = pNew;
					pHead = pNew;

					switch(CDFS_CUR_FILE_FOLDER){
						case CDFS_FILE_FOLDER_VIDEO:
							last_insert_video = pNew;
							break;
						case CDFS_FILE_FOLDER_JPG:
							last_insert_jpg = pNew;
							break;
						default:									
							break;
					}
					
					break;
				}
			}
#endif
		}
	}

    /*save table header*/
#if JET_PANA_CDFS
	UNIFY_FOLDER.head_node = pHead;
	UNIFY_FOLDER.folder_info.file_num = fileCount;

	UNIFY_FOLDER.folder_info.file_num_in_subfolder[CDFS_CUR_FILE_FOLDER] = subfileCount;
#else
	CDFS_TAB[CDFS_CUR_FILE_FOLDER].head_node = pHead;
	CDFS_TAB[CDFS_CUR_FILE_FOLDER].folder_info.file_num = fileCount;
#endif
	if((CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO) || (CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT))
	{
		CDFS_ADD_FILE = 0;
	}

	#if 0
	if((CDFS_EVENT_OCCUR & VIDEO_EVENT_IN_VIDEO)
		&&(filetype & SP5K_DCF_FILETYPE_AVI))

	if ((CDFS_EVENT_OCCUR & VIDEO_EVENT_IN_VIDEO)
		&& ((filetype & SP5K_DCF_FILETYPE_AVI)
		 || (filetype & SP5K_DCF_FILETYPE_MOV)))
	{
		appCdfsEventProcess(fileNameTemp,VIDEO_EVENT_IN_VIDEO);
		CDFS_CUR_FILE_FOLDER = CDFS_FILE_FOLDER_VIDEO;
		appCdfsEventFlagClean(VIDEO_EVENT_IN_VIDEO);
	}
	else if ((CDFS_EVENT_OCCUR & JPG_EVENT_IN_VIDEO)
		&& (filetype & SP5K_DCF_FILETYPE_JPG))
	{
		HOST_PROF_LOG_ADD(LEVEL_INFO, "cdfs: appCdfsEventFlagClean(JPG_EVENT_IN_VIDEO)");
		appCdfsEventProcess(fileNameTemp,JPG_EVENT_IN_VIDEO);
		CDFS_CUR_FILE_FOLDER = CDFS_FILE_FOLDER_VIDEO;
		appCdfsEventFlagClean(JPG_EVENT_IN_VIDEO);
	}
	#endif

	sp5kOsSemaphorePut(&cdfsSem);
#if JET_PANA_CDFS
	nodeUpdateCb(pNew);
#endif
	return (UINT32)pNew;
}

 /*--------------------------------------------------------------------------*
 * Function name	: AddJpegFile
 * Function	       	: add jpeg file
 * Return value   	: void
 * Parameter1    	: file name pointer  (--)
 * Parameter2    	: file type (UINT32) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/     
static void AddJpegFile(UINT8 *filename, UINT32 filetype)
{
	CDFS_FILE_TAB_T *pNew;
	CDFS_FILE_TAB_T *pCur=NULL;
#if !JET_PANA_CDFS	
	CDFS_FILE_TAB_T *pNext=NULL;
#endif
	CDFS_FILE_TAB_T *pHead=NULL;
	UINT32 fileCount=0;
	UINT32 fd, attr;
#if JET_PANA_CDFS
 	tmx_t /*atime , mtime ,*/ctime;
#endif
	DBG_PRINT("cdfs: %s %x", __FUNCTION__,filename);

#if JET_PANA_CDFS
	pNew = sp5kMalloc(sizeof(CDFS_FILE_TAB_T));
#else
	pNew = cdfsNodeAlloc();
#endif

	memset(pNew,0,sizeof(CDFS_FILE_TAB_T));
	DBG_ASSERT(pNew);

	fd = sp5kFsFileOpen(filename,SP5K_FS_OPEN_RDONLY);
	if (fd)
	{
		pNew->FileSize = sp5kFsFileSizeGetI64(fd);
		sp5kFsFileAttrGet(filename, &attr);
		sp5kFsFileClose(fd);
	}
	else
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s, file open fail", __FUNCTION__);
#if JET_PANA_CDFS 
		sp5kFree(pNew);
#else
        	cdfsNodeFree(pNew);
#endif
        return;
	}
#if JET_PANA_CDFS
	jet_exif_datetime_get(filename,&ctime);
#endif

	if (CDFS_EVENT_OCCUR & JPG_EVENT_IN_VIDEO)
	{
	    	appCdfsEventProcess(filename,JPG_EVENT_IN_VIDEO);
		CDFS_CUR_FILE_FOLDER = CDFS_FILE_FOLDER_VIDEO;
		appCdfsEventFlagClean(JPG_EVENT_IN_VIDEO);
	}

	/****************************************/
	strncpy((char *)pNew->FileName,(const char *)filename,sizeof(pNew->FileName));
	if (attr & SP5K_FS_ATTR_READONLY)
	{
		pNew->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
	}
	pNew->attr = attr;
	pNew->FileType = filetype;
#if JET_PANA_CDFS
	pNew->folder = 1 << CDFS_FILE_FOLDER_JPG;
	pNew->event = 0x400;
	sprintf(pNew->mtime,"%04d/%02d/%02d/%02d:%02d:%02d" ,(ctime.tmx_year) , ctime.tmx_mon , ctime.tmx_mday,ctime.tmx_hour,ctime.tmx_min,ctime.tmx_sec);
	printf("%s %s %s\n",__FUNCTION__ ,  pNew->FileName , pNew->mtime);
	/*****************************************/
	pHead = UNIFY_FOLDER.head_node;
	fileCount = UNIFY_FOLDER.folder_info.file_num_in_subfolder[CDFS_FILE_FOLDER_JPG];
#else
	/*****************************************/
	pHead = CDFS_TAB[CDFS_FILE_FOLDER_JPG].head_node;
	fileCount = CDFS_TAB[CDFS_FILE_FOLDER_JPG].folder_info.file_num;
#endif
    	fileCount++;

	if (pHead == NULL)/*create first object of table*/
	{
#if JET_PANA_CDFS
	        pNew->next = pNew;
	        pNew->pre = pNew;
	        UNIFY_FOLDER.head_node = pNew;
#else
		/*add file@head*/
		pNew->next = NULL;
		pNew->pre = pNew;
		pHead = pNew;
#endif
	}
	else
	{
#if JET_PANA_CDFS
		 pCur = pHead;
	        while (1) {
		    int cmp;

		    cmp = strcmp((const char*) pNew->FileName, (const char*) pCur->FileName);
	            if (cmp == 0) {
	                /*the new file replace the old file when two file name is same*/
	                pCur->FileSize = pNew->FileSize;
	                pCur->attr = pNew->attr;
	                pCur->dcfattr = pNew->dcfattr;
	                pCur->folder = pNew->folder;
	                fileCount--;
	                sp5kFree(pNew);
	                break;
	            } else if (cmp < 0) {
			    add_prefix_node(pCur, pNew);
			    break;
		    } else {
	            	if (pCur->next == pHead) {
	                	add_postfix_node(pCur, pNew);
	                	break;
	            	}
	            	pCur = pCur->next;
		    }
	        }
#else
		pCur = pNext = pHead->pre;
		while(1)
		{
			if (strcmp((const char *)pNew->FileName,(const char *)pCur->FileName) > 0)/*sort by date*/
			{
				/*add pNew after pCur*/
				pCur->next = pNew;
				pNew->pre = pCur;
				if (pCur == pNext)
				{/*pNew is the last*/
					pNew->next = NULL;
					pHead->pre = pNew;
				}
				else
				{/*insert pNew between pCur and pNext*/
					pNew->next = pNext;
					pNext->pre = pNew;
				}
				break;
			}
			else if(strcmp((const char *)pNew->FileName,(const char *)pCur->FileName) == 0)/*the same name*/
			{/*the new file replace the old file when two file name is same*/
				pCur->FileSize = pNew->FileSize;
				pCur->attr = pNew->attr;
				pCur->dcfattr = pNew->dcfattr;
				fileCount--;
				cdfsNodeFree(pNew);
				pNew = pCur;
				break;
			}
			else
			{
				if (pCur != pHead)
				{
					pNext = pCur;
					pCur = pCur->pre;
				}
				else
				{/*add pNew as new head*/
					pNew->pre = pHead->pre;
					pNew->next = pHead;
					pHead->pre = pNew;
					pHead = pNew;
					break;
				}
			}
		}
#endif
	}
#if JET_PANA_CDFS
	UNIFY_FOLDER.folder_info.file_num_in_subfolder[CDFS_FILE_FOLDER_JPG] = fileCount;
#else
    	CDFS_TAB[CDFS_FILE_FOLDER_JPG].head_node = pHead;
    	CDFS_TAB[CDFS_FILE_FOLDER_JPG].folder_info.file_num = fileCount;
#endif
}

#if JET_PANA_CDFS

 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsLastNmlFolderUpdate
 * Function	       	: Update last Nml folder of CDFS
 * Return value   	: void
 * Parameter1    	: folder type(UINT32)
 * Parameter2    	: sub folder number(UINT32) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/      
static void appCdfsLastNmlFolderUpdate(UINT32 folder, UINT32 sub)
{

    if (folder != CDFS_FILE_FOLDER_VIDEO)
        return;

    printf("%s(%d) folder idx %u sub %u vs %u\n", __func__, __LINE__, folder,
            sub, LAST_NML);
    if (LAST_NML <= sub) {
        LAST_NML = sub;
        while (1) {
		char path[32];
		UINT32 cnt;
		strcpy(path, jet_cdfs_folder_to_path(LAST_NML));

		cnt = sp5kFsDirFileCntGet((UINT8 *)path);
		if (cnt >= JET_CDFS_DCIM_FILE_NUM_IN_SUBFOLDER_MAX) {
		    LAST_NML++;
		    if (LAST_NML > JET_CDFS_DCIM_SUBFOLDER_INDEX_MAX)
		        LAST_NML = JET_CDFS_DCIM_SUBFOLDER_START_INDEX;
		} else
		    break;
        }
    }
}

 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFullPath2Folder
 * Function	       	: Set the full path to folder
 * Return value   	: folder index (UINT32)
 * Parameter1    	: path pointer (--)
 * Parameter2    	: sub folder pointer(--) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/  
UINT32 appCdfsFullPath2Folder(UINT8 *path, UINT32 *sub)
{
    UINT32 folder, folder_idx;

    if (strcmp((char *)path, EVENT_PATH) == 0) {
        folder_idx = CDFS_FILE_FOLDER_EVENT;
    } else if (strcmp((char *)path, USER_RECORD_PATH) == 0) {
        folder_idx = CDFS_FILE_FOLDER_USERRECORD;
    } else if (strcmp((char *)path, USERSAVE_PATH) == 0) {
        folder_idx = CDFS_FILE_FOLDER_USERSAVE;
    } else {
	folder = appCdfsPath2Folder(path);
	folder_idx = CDFS_FILE_FOLDER_VIDEO;
	appCdfsLastNmlFolderUpdate(folder_idx, folder);
	
    	if (sub)
	    *sub = folder;
    }

    return folder_idx;
}
#endif

#if JET_PANA_CDFS

  /*--------------------------------------------------------------------------*
 * Function name	: AddVideoFile
 * Function	       	: Add video file
 * Return value   	: void
 * Parameter1    	: file name pointer (--)
 * Parameter2    	: handle(UINT32) 
 * Parameter3    	: path pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/ 
static void AddVideoFile(UINT8 *filename, UINT32 handle , UINT8 *path)
#else
static void AddVideoFile(UINT8 *filename, UINT32 filetype, UINT32 handle)
#endif
{
#if JET_PANA_CDFS
	CDFS_FILE_TAB_T *pNew = NULL;
	CDFS_FILE_TAB_T *pCur = NULL;
	CDFS_FILE_TAB_T *pHead = NULL;
/*	UINT32 PRE_CDFS_CUR_FILE_FOLDER = CDFS_CUR_FILE_FOLDER;*/
	UINT32 fd, attr;
	/*UINT8 *ptr = NULL;*/
	UINT32 folder, folder_idx;
	UINT32 fileCount;

	UINT32 subfileCount = 0;
	char full_path[64];
#else
	CDFS_FILE_TAB_T *pNew=NULL;
	CDFS_FILE_TAB_T *pCur=NULL;
	CDFS_FILE_TAB_T *pNext=NULL;
	CDFS_FILE_TAB_T *pHead=NULL;
	UINT32 fileCount=0;
	UINT32 fd, attr, CDFS_CUR_FILE_FOLDER_TEMP = CDFS_FILE_FOLDER_FAIL;
	char *pfname;
#endif
	DBG_PRINT("cdfs: %s %x", __FUNCTION__,filename);
    if (handle)
        pNew = (CDFS_FILE_TAB_T *)handle;
    else
    {
 #if JET_PANA_CDFS 
	printf("handle is NULL\n");
	return;
#else
	pNew=cdfsNodeAlloc();
	memset(pNew,0,sizeof(CDFS_FILE_TAB_T));
	DBG_ASSERT(pNew);
#endif
    }
	
 #if JET_PANA_CDFS 
    sprintf(full_path, "%s/%s", path, filename);

    folder_idx = appCdfsFullPath2Folder(path, &folder);
    if (pNew->folder == 0) {
	    printf("WRAN  node's folder is empty\n");
	    folder = 0;

	    pNew->folder = folder_idx == 0 ? folder : (1 << folder_idx);
    }
	fd = sp5kFsFileOpen((UINT8 *)full_path,SP5K_FS_OPEN_RDONLY);
#else

	
	fd = sp5kFsFileOpen(filename,SP5K_FS_OPEN_RDONLY);
#endif


	if (fd) {
		pNew->FileSize = sp5kFsFileSizeGet(fd);
 #if JET_PANA_CDFS 
		sp5kFsFileAttrGet((UINT8 *)full_path, &attr);
 #else
 		sp5kFsFileAttrGet(filename, &attr);
 #endif
		sp5kFsFileClose(fd);
	}
	else
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s, file open fail", __FUNCTION__);
 #if JET_PANA_CDFS
 	sp5kFree(pNew);
 #else
        cdfsNodeFree(pNew);
 #endif
        return;
	}

	if (attr & SP5K_FS_ATTR_READONLY)
	{
		pNew->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
	}
	pNew->attr = attr;
#if JET_PANA_CDFS
    

    *CDFS_FOLDER_SIZE_TABLE[folder_idx] += pNew->FileSize;

    /*get table header*/
    pHead = UNIFY_FOLDER.head_node;
    fileCount = UNIFY_FOLDER.folder_info.file_num;
    subfileCount = UNIFY_FOLDER.folder_info.file_num_in_subfolder[folder_idx];

    fileCount++;
    subfileCount++;
#if DEBUG_ON
    debug_file_t(pHead);
    debug_file_t(pCur);
    debug_file_t(pNew);
#endif
#else
		pfname = strstr((char *)filename, ":");
	//printf("pfname=%s\n",pfname);
	if(pfname)
	{
		char ch = *(pfname+2);
		if(ch == 'V')
			CDFS_CUR_FILE_FOLDER_TEMP = CDFS_FILE_FOLDER_VIDEO;
		else if(ch == 'E')
			CDFS_CUR_FILE_FOLDER_TEMP = CDFS_FILE_FOLDER_EVENT;
		else
			printf("AddVideoFile add CDFS_TAB ERROR!!\n");
	}
	else
		printf("AddVideoFile pfname fail\n");

	if (CDFS_CUR_FILE_FOLDER_TEMP == CDFS_FILE_FOLDER_VIDEO)
	{
		if (CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
		{
			if (!(pNew->dcfattr & SP5K_DCF_FILE_ATTR_LOCK))
			{
				CDFS_VIDEO_SIZE += (UINT64)pNew->FileSize;
			}
		}
		else
		{
			CDFS_VIDEO_SIZE += pNew->FileSize;
		}
		/*__FUNC_TRACK__;*/
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: video size = %u(B),%u(MB)"\
			,(UINT32)CDFS_VIDEO_SIZE,((UINT32)CDFS_VIDEO_SIZE)>>20);
	}
	else if (CDFS_CUR_FILE_FOLDER_TEMP == CDFS_FILE_FOLDER_EVENT)
	{
		CDFS_EVENT_SIZE += pNew->FileSize;
	}


    /*get table header*/
    pHead = appCdfsFileListHeadGet(CDFS_CUR_FILE_FOLDER_TEMP);
    fileCount = CDFS_TAB[CDFS_CUR_FILE_FOLDER_TEMP].folder_info.file_num;

    fileCount++;
#endif

	if (pHead == NULL)/*create first object of table*/
	{
		/*add file@head*/
#if JET_PANA_CDFS
		pNew->next = pNew;
#else
		pNew->next = NULL;
#endif
		pNew->pre = pNew;
		pHead = pNew;
	}
	else
	{
#if JET_PANA_CDFS
	 pCur = pHead;
        while (1) {
	    int cmp;
	    cmp = strcmp((const char *) pNew->FileName,
			    (const char *) pCur->FileName);

            if (cmp == 0) {
                if (CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO
                        && pCur->folder == pNew->folder) {
                    /*the new file replace the old file when two file name is same*/
                    pCur->FileSize = pNew->FileSize;
                    pCur->attr = pNew->attr;
                    pCur->dcfattr = pNew->dcfattr;
                    sp5kFree(pNew);
                    fileCount--;
                    subfileCount--;
                    break;
                }
            } else if (cmp < 0) {
		    add_prefix_node(pCur, pNew);
		    break;
	    } else {
            	if (pCur->next == pHead) {
                    add_postfix_node(pCur, pNew);
                    break;
            	}
            	pCur = pCur->next;
	    }
        }
#else
		pCur = pNext = pHead->pre;
		while(1)
		{
			SINT32 diff = strcmp((const char *)pNew->FileName,(const char *)pCur->FileName);
			if (diff > 0)/*sort by date*/
			{
				/*add pNew after pCur*/
				pCur->next = pNew;
				pNew->pre = pCur;
				if (pCur == pNext)
				{/*pNew is the last*/
					pNew->next = NULL;
					pHead->pre = pNew;
				}
				else
				{/*insert pNew between pCur and pNext*/
					pNew->next = pNext;
					pNext->pre = pNew;
				}
				break;
			}
			else if(diff == 0)/*the same name*/
			{/*the new file replace the old file when two file name is same*/
				pCur->FileSize = pNew->FileSize;
				pCur->attr = pNew->attr;
				pCur->dcfattr = pNew->dcfattr;
				fileCount--;
				cdfsNodeFree(pNew);
				pNew = pCur;
				break;
			}
			else
			{
				if (pCur != pHead)
				{
					pNext = pCur;
					pCur = pCur->pre;
				}
				else
				{/*add pNew as new head*/
					pNew->pre = pHead->pre;
					pNew->next = pHead;
					pHead->pre = pNew;
					pHead = pNew;
					break;
				}
			}
		}
#endif
	}
#if JET_PANA_CDFS
    /*save table header*/
    UNIFY_FOLDER.head_node = pHead;
    UNIFY_FOLDER.folder_info.file_num = fileCount;
    UNIFY_FOLDER.folder_info.file_num_in_subfolder[folder_idx] =
                    subfileCount;

    nodeUpdateCb(pNew);

    if ((folder_idx == CDFS_FILE_FOLDER_VIDEO)
            || (folder_idx == CDFS_FILE_FOLDER_EVENT)) {
        CDFS_ADD_FILE = 0;
    }
#if DEBUG_ON
    appCdfsFileListPrintf(CDFS_FILE_FOLDER_VIDEO);
#endif
#else
    /*save table header*/
    CDFS_TAB[CDFS_CUR_FILE_FOLDER].head_node = pHead;
    CDFS_TAB[CDFS_CUR_FILE_FOLDER].folder_info.file_num = fileCount;

	if((CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO) || (CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT))
	{
		CDFS_ADD_FILE = 0;
	}
#endif
}

  /*--------------------------------------------------------------------------*
 * Function name	: OpenVideoFile
 * Function	       	: open video file
 * Return value   	: New CDFS file table pointer address (UINT32)
 * Parameter1    	: file name pointer (--)
 * Parameter2    	: file type(UINT32) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/  
static UINT32 OpenVideoFile(UINT8 *filename, UINT32 filetype)
{
	CDFS_FILE_TAB_T *pNew;
#if JET_PANA_CDFS
	pNew=sp5kMalloc(sizeof(CDFS_FILE_TAB_T));
#else
	pNew=cdfsNodeAlloc();
#endif
	memset(pNew,0,sizeof(CDFS_FILE_TAB_T));
	DBG_ASSERT(pNew);

	pNew->FileType = filetype;
	strncpy((char *)pNew->FileName,(const char *)filename,sizeof(pNew->FileName));

    return (UINT32)pNew;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileAdd
 * Function	       	: Add files to CDFS
 * Return value   	: result (UINT32 SP5K_ERR_SUCCESS/SP5K_ERR_DCF_PARAM_GROUP)
 * Parameter1    	: file name pointer (--)
 * Parameter2    	: handle pointer (--) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
UINT32 appCdfsFileAdd(UINT8 *filename, UINT32 *phandle)
{
	UINT32 ret = SP5K_ERR_SUCCESS;
	UINT8 fileNameTemp[64]={0};
	UINT32 filetype;
#if  JET_PANA_CDFS
//	CDFS_FILE_TAB_T *pNew;
#endif
	char *pfname;

	HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s %s", __FUNCTION__, filename);

    while ((pfname = strchr((char *)filename, '\\')))
		*pfname = '/';
	
	appCdfsAbsolutePathToFile(filename, fileNameTemp);
	
	filetype = appCdfsFileNameToFileType(fileNameTemp);
    if(filetype & SP5K_DCF_FILETYPE_JPG)
    {
#if !JET_PANA_CDFS
		if (cdfsFileAddCb) {
			cdfsFileAddCb(filename);
		}
#endif
        AddJpegFile(fileNameTemp, filetype);
    }
    else
    {
#if JET_PANA_CDFS
	UINT32 folder_idx, sub = 0;
	UINT8 path[32];
	CDFS_FILE_TAB_T *pNew;
	pNew =(CDFS_FILE_TAB_T *)OpenVideoFile((UINT8 *)fileNameTemp, filetype);
	
	appInfoPathGet(filename, path);
	folder_idx = appCdfsFullPath2Folder(path, &sub);
	
	if (sub)
		pNew->folder = sub;
	else
		pNew->folder = 1 << folder_idx;
	*phandle =(UINT32)pNew;
#else
    	if (phandle)
            *phandle = OpenVideoFile(fileNameTemp, filetype);
        else
    		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s, Handle is null", __FUNCTION__);
#endif
    }

    return ret;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileSync
 * Function	       	: Sync File of CDFS
 * Return value   	: result (UINT32 SP5K_ERR_SUCCESS/SP5K_ERR_DCF_PARAM_GROUP)
 * Parameter1    	: file name pointer (--)
 * Parameter2    	: handle pointer (--) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
UINT32 appCdfsFileSync(UINT8 *filename, UINT32 handle)
{

#if JET_PANA_CDFS
	UINT8 path[32];
	UINT32  duration;

	CDFS_FILE_TAB_T *pCur = (CDFS_FILE_TAB_T *) handle;
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s %s", __FUNCTION__, filename);
	
	
	if (!IS_CARD_EXIST)
		return SP5K_ERR_SUCCESS;
	sp5kMediaPlayAttrSet(SP5K_MEDIA_ATTR_FILE_NAME,(UINT32) filename);
	if(SUCCESS==sp5kMediaPlayAttrParse()){
		sp5kMediaPlayAttrGet(SP5K_MEDIA_ATTR_DURATION, &duration);
		printf("[%s] duration = %u\n", __func__, duration);
		if(duration<=3000){
			sp5kFsFileDelete(filename);
			sp5kFree(pCur);
			printf("[%s] duration is less than 3 sec. Delete file!\n", __func__);
			return SP5K_ERR_SUCCESS;
		}
	}else{
		printf("[%s] %s - fail sp5kMediaPlayAttrParse\n", __func__, filename);
		return SP5K_ERR_SUCCESS;
	}
#else
	UINT8 fileNameTemp[64]={0};
	UINT32 filetype;


	HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s %s", __FUNCTION__, filename);

	if (cdfsFileAddCb) {
		cdfsFileAddCb(filename);
	}
#endif

#if JET_PANA_CDFS
	jet_rec_video_post(pCur);

	strcpy((char *)path, jet_cdfs_folder_to_path(pCur->folder));
	printf("%s  PATH :  %s\n" , __FUNCTION__ , path );
	AddVideoFile(pCur->FileName, handle ,path);

	appInfoFileNameEventAdd(pCur);
	if (reboot_flag == FALSE &&
			appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP) {
			
		if(appInfoBatteryLvGet() < appInfoParkingBatterySettingGet()){
			if((appInfoParkingBatterySettingGet() - appInfoBatteryLvGet() ) <= 200)
				 appGsensorLevelCtrl(2, ((int) appInfoParkingGSensorLevelGet() - 1)); /*Parking mode enable*/
			else{
				 appGsensorLevelCtrl(3, 4);
				 appInfoParkingModeTriStateSet(ParkingModeTigger_LOWPOWER);
			}
		}
		else	
			appInfoParkingModeTriStateSet(ParkingModeTigger_EVENT);
		appPowerOffUiMsgSend(APP_POWER_OFF_USB_PLUG); /*Power off*/
		
	} else
		lastNode = pCur;
#else

	appCdfsAbsolutePathToFile(filename, fileNameTemp);
	filetype = appCdfsFileNameToFileType(fileNameTemp);

       AddVideoFile(fileNameTemp, filetype, handle);
#endif
    #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
    memset(CDFS_BACKUP_FILE_NAME, 0, sizeof(UINT8)*32);
    #endif

	return SP5K_ERR_SUCCESS;
}

#if JET_PANA_CDFS

  /*--------------------------------------------------------------------------*
 * Function name	: appCfdsCheckEmptySubfolder
 * Function	       	: Check empty video subfolder and remove 
 * Return value   	: void
 * Parameter1    	: subfolder number (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
static void appCfdsCheckEmptySubfolder(UINT32 subfolder)
{
	UINT8 path[16];
	UINT8 full_path[32];
	UINT32 num;

	appCdfsFolder2Path((UINT32)CDFS_FILE_FOLDER_VIDEO, (UINT8 *)path);
	
	snprintf((char *)full_path, sizeof(full_path), "%s/%03u_NML", (char *)path, subfolder);

	num = sp5kDirFileCntGet(full_path);
	if (num == 0)
		sp5kFsDirRemove(full_path);
}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileDel
 * Function	       	: Delete the file of CDFS
 * Return value   	: result (BOOL true/false)
 * Parameter1    	: current CDFS file table pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
 #if JET_PANA_CDFS

 BOOL appCdfsFileDel(CDFS_FILE_TAB_T *pCur)
{
	UINT8 fileName[64];
	UINT32 folder;

	if (pCur == NULL)
		return FAIL;

	folder = pCur->folder;
	printf("cdfs: %s %s(%d)\n", __FUNCTION__, pCur->FileName, pCur->folder);

	if (folder >= JET_CDFS_DCIM_SUBFOLDER_START_INDEX)
		folder = CDFS_FILE_FOLDER_VIDEO_BIT;

    sp5kOsMutexGet(&headSem, TX_WAIT_FOREVER);

	sprintf((char *)fileName,"%s/%s",
			jet_cdfs_folder_to_path(pCur->folder), pCur->FileName);
	printf("%s %d %s ", __func__, __LINE__, fileName);
	appCdfsRemoveNode(pCur);
	if(sp5kFsFileDelete(fileName)==SUCCESS)/*file name with dir path*/
	{
		printf("deleted successed\n");
		if (folder == CDFS_FILE_FOLDER_VIDEO_BIT)
			appCfdsCheckEmptySubfolder(pCur->folder);
	} else
		printf("remove fail\n");

    sp5kOsMutexPut(&headSem);

	return SUCCESS;

}

 #else
UINT32 appCdfsFileDel(UINT8 *filename)
{
	CDFS_FILE_TAB_T *pCur=NULL;
	CDFS_FILE_TAB_T *pPre=NULL;
	CDFS_FILE_TAB_T *pNext=NULL;
	CDFS_FILE_TAB_T *pHead;
	UINT32 fileCount=0;
	UINT8 fileNameTemp[64]={0};

	DBG_PRINT("cdfs: %s %s", __FUNCTION__, filename);

	sp5kOsSemaphoreGet(&cdfsSem, TX_WAIT_FOREVER);

    /*get table header*/
    pHead = appCdfsFileListHeadGet(CDFS_CUR_FILE_FOLDER);
    fileCount = CDFS_TAB[CDFS_CUR_FILE_FOLDER].folder_info.file_num;

	if(!filename || !pHead)
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s Fail", __FUNCTION__);
		sp5kOsSemaphorePut(&cdfsSem);
		return FAIL;
	}

	/*get file name*/
	appCdfsAbsolutePathToFile(filename, fileNameTemp);

	pCur=pPre=pHead;

	while(1)
	{
		if(strcmp((const char *)pCur->FileName,(const char *)fileNameTemp)==0)/*find the file*/
		{
			pPre->next = pCur->next;
			pNext = pCur->next;
			if (pNext) {
				pNext->pre = pPre;
			}
			else {
				pHead->pre = pPre;
			}

			if (pCur == pHead)
			{
				if (pCur->next)
				{
					pHead = pCur->next;
					pHead->pre = pCur->pre;
				}
				else
				{
					HOST_PROF_LOG_ADD(LEVEL_WARN, "cdfs: table is null");
					pHead = NULL;
				}
			}
			/****************************************/
			if(CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
			{
				if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
				{
					if(!(pCur->dcfattr & SP5K_DCF_FILE_ATTR_LOCK))
					{
						CDFS_VIDEO_SIZE-=pCur->FileSize;
						HOST_PROF_LOG_ADD(LEVEL_DEBUG, "cdfs: CDFS_PROTECTED_SKIP");
					}
				}
				else
				{
					CDFS_VIDEO_SIZE-=pCur->FileSize;
					HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: cur file size %u %u",
                        ((UINT32)CDFS_VIDEO_SIZE)>>20,((UINT32)pCur->FileSize)>>20);
				}

			}
			else if(CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_EVENT)
			{
				CDFS_EVENT_SIZE -= pCur->FileSize;
				HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: event cur file size %u %u",
                    ((UINT32)CDFS_VIDEO_SIZE)>>20,((UINT32)pCur->FileSize)>>20);
			}
			/*****************************************/

			cdfsNodeFree(pCur);
			pCur = NULL;
			fileCount--;
			/*if(fIdx > fileCount)
			{
				fIdx = fileCount;
			}*/
			break;
		}
		else/*not find file*/
		{
			if(pCur->next)/*keep on finding*/
			{
				pPre=pCur;
				pCur=pCur->next;
				/*fIdx++;*/
			}
			else
			{
				HOST_PROF_LOG_ADD(LEVEL_WARN, "cdfs: there isn't this file");
				break;/*there isn't this file in tab*/
			}
		}
	}

    /*save table header*/
    CDFS_TAB[CDFS_CUR_FILE_FOLDER].head_node = pHead;
    CDFS_TAB[CDFS_CUR_FILE_FOLDER].folder_info.file_num = fileCount;

	sp5kOsSemaphorePut(&cdfsSem);

	if (cdfsFileDelCb) {
		cdfsFileDelCb(filename);
	}

	return SUCCESS;

}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileListHeadGet
 * Function	       	: Get file list head
 * Return value   	: CDFS file table pointer (--)
 * Parameter1    	: folder (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/ 
 #if JET_PANA_CDFS
 CDFS_FILE_TAB_T *appCdfsFileListHeadGet(UINT32 folder)
{
	DBG_PRINT("cdfs: %s", __FUNCTION__);

    return UNIFY_FOLDER.head_node;
}
 #else
CDFS_FILE_TAB_T *appCdfsFileListHeadGet(UINT32 folder)
{
	DBG_PRINT("cdfs: %s", __FUNCTION__);

    if(folder<CDFS_FILE_FOLDER_MAX)
        return CDFS_TAB[folder].head_node;
    else
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s %d CDFS Folder Fail", __FUNCTION__,__LINE__);
        DBG_ASSERT(0);
	}
    return NULL;
}
 #endif

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileListLastGet
 * Function	       	: Get the last file list 
 * Return value   	: CDFS file table pointer (--)
 * Parameter1    	: folder (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/  
#if JET_PANA_CDFS
CDFS_FILE_TAB_T *appCdfsFileListLastGet(UINT32 folder)
{
	DBG_PRINT("cdfs: %s", __FUNCTION__);

	if (UNIFY_FOLDER.head_node == NULL)
	    return NULL;

	return UNIFY_FOLDER.head_node->pre;
}
#else
CDFS_FILE_TAB_T *appCdfsFileListLastGet(UINT32 folder)
{
	DBG_PRINT("cdfs: %s", __FUNCTION__);
	CDFS_FILE_TAB_T *pHead=NULL;
	CDFS_FILE_TAB_T *pLast=NULL;
	CDFS_FILE_TAB_T *pCur=NULL;
	

    if(folder<CDFS_FILE_FOLDER_MAX)
	{
		UINT32 idx;
		pHead = appCdfsFileListHeadGet(folder);
		idx = CDFS_TAB[folder].folder_info.file_num;
		pCur = pHead ;

		if(pCur == NULL)
		{
			return pCur;
		}

		if(idx)
		{
			idx--;
		}
		while(idx)
		{
			if(pCur->next)
			{
				pCur = pCur->next;
			}
			else
			{
				pCur = NULL;
				break;
			}
			idx--;
		}
		pLast = pCur;
 		return pLast;
	}
    else
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s %d CDFS Folder Fail", __FUNCTION__,__LINE__);
        DBG_ASSERT(0);
	}
    return NULL;
}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileListTabInit
 * Function	       	: Inital File list
 * Return value   	: result (sucess/fail)
 * Parameter1    	: folder (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
UINT32 appCdfsFileListTabInit(UINT32 folder)
{
	CDFS_FILE_TAB_T *pCur;
	DBG_PRINT("cdfs: %s", __FUNCTION__);

#if JET_PANA_CDFS
	printf("cdfs: %s %u", __FUNCTION__, folder);

	/* Clean current query info */
	jetQueryReset();
	pCur = UNIFY_FOLDER.head_node;
	if (pCur == NULL || folder != CDFS_FILE_FOLDER_MAX)
		return SUCCESS;

	do {
		CDFS_FILE_TAB_T *pPre;
		pPre = pCur;
		pCur = pCur->next;
		sp5kFree(pPre);
	} while (pCur != UNIFY_FOLDER.head_node);

	memset(&UNIFY_FOLDER, 0, sizeof(UNIFY_FOLDER));

	CDFS_VIDEO_SIZE = 0;
	CDFS_EVENT_SIZE = 0;
	CDFS_USERRECORD_SIZE = 0;
	CDFS_USERSAVE_SIZE = 0;
	CDFS_AVNIV_SIZE = 0;

#else


    if(folder<CDFS_FILE_FOLDER_MAX)
    {
        pCur = CDFS_TAB[folder].head_node;
		if(pCur == NULL)
		{
            return SUCCESS;
		}
		while(pCur)
		{
			CDFS_FILE_TAB_T *pPre;
			pPre = pCur;
			pCur = pCur->next;
			cdfsNodeFree(pPre);
		}
        CDFS_TAB[folder].head_node = NULL;
        CDFS_TAB[folder].folder_info.file_num = 0;
    }
    else
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s %d CDFS Folder Fail", __FUNCTION__,__LINE__);
        DBG_ASSERT(0);
	}

	switch(folder)
	{
		case CDFS_FILE_FOLDER_VIDEO:
			CDFS_VIDEO_SIZE = 0;
			break;
		case CDFS_FILE_FOLDER_JPG:
			break;
		case CDFS_FILE_FOLDER_EVENT:
			CDFS_EVENT_SIZE = 0;
			eventListReady = 0;
			break;
		default:
			break;
	}
#endif
	return SUCCESS;
}

#if JET_PANA_CDFS

  /*--------------------------------------------------------------------------*
 * Function name	: debug_file_t
 * Function	       	: Check file table
 * Return value   	: void
 * Parameter1    	: CDFS file table pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
void debug_file_t(CDFS_FILE_TAB_T *pCur)
{
	if (pCur != NULL)
		printf("fn(%s) P(%p) n(%s) p(%s)\n",
			pCur->FileName, pCur,
			pCur->next->FileName, pCur->pre->FileName);
	else
		printf("Node is NULL\n");
}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileListPrintf
 * Function	       	: Print File list
 * Return value   	: void
 * Parameter1    	: folder (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
void appCdfsFileListPrintf(UINT32 folder)
{
#if JET_PANA_CDFS
	UINT32 num=0;
	CDFS_FILE_TAB_T *pObj, *pHead;
	DBG_PRINT("cdfs: %s %u", __FUNCTION__, folder);

	pObj = UNIFY_FOLDER.head_node;
	pHead = pObj;

	while(pObj) {
	        HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: file[%d]---->%s "
        	        "attr=0x%x, filesize=%u, folder=%u event=0X%X index=%d type=0x%02x",
               		num, pObj->FileName,
                	pObj->dcfattr, ((UINT32)pObj->FileSize)>>20,
                	pObj->folder, pObj->event, pObj->index, pObj->FileType);
#if DEBUG_ON
	debug_file_t(pObj);
#endif
        if (pObj->next == pHead)
        	break;
       	pObj = pObj->next;
       	num++;
	}

#else	
	CDFS_FILE_TAB_T *pObj = NULL;
	DBG_PRINT("cdfs: %s", __FUNCTION__);

    if(folder<CDFS_FILE_FOLDER_MAX)
    {
        pObj = CDFS_TAB[folder].head_node;
	UINT32 num=0;
    	while(pObj)
    	{
    		printf("%-5u   %-26s  %x  %10u\n",num,pObj->FileName,
		                pObj->dcfattr, (UINT32)pObj->FileSize);

    		if(pObj->next)
    		{
    			pObj=pObj->next;
    			num++;
    		}
    		else
    		{
    			break;
    		}
    	}
    	HOST_PROF_LOG_ADD(LEVEL_INFO, "cdfs: list printf finish");
    }
    else
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s %d CDFS Folder Fail", __FUNCTION__,__LINE__);
        DBG_ASSERT(0);
	}
#endif
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileNameExistCheck
 * Function	       	: Check file name exist
 * Return value   	: result (UINT32 true/false)
 * Parameter1    	: file name pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
UINT32 appCdfsFileNameExistCheck(UINT8 *pCheckFileName)
{
	CDFS_FILE_TAB_T *pHead;
	CDFS_FILE_TAB_T *pLast;
	CDFS_FILE_TAB_T *pCurr=NULL;
	char fHName[25]={0};
	char fLName[25]={0};
	char fName[25]={0};
	char fHPathName[64]={0};
	char fLPathName[64]={0};
	char fPathName[64]={0};
	char attName[4]={0};
	UINT32 sameNameNum=0,attNum=0,sameNameMax=0;
	UINT32 ret =FALSE;
	BOOL headMark = TRUE;


	pHead = appCdfsFileListHeadGet(CDFS_CUR_FILE_FOLDER);
	pLast = appCdfsFileListLastGet(CDFS_CUR_FILE_FOLDER);

	#if 0
	printf("\033[32m^pHead=%s^\033[0m\n",pHead);
	printf("\033[32m^pLast=%s^\033[0m\n",pLast);
	appCdfsListInfoDump(pHead, pLast);
	#endif
	while(pHead && pLast)
	{
		strcpy(fHName, (char *)pHead->FileName);
		fHName[15] = '\0';
		sprintf(fHPathName,"%s/%s",sp5kFsGetCurPath(),fHName);
		strcpy(fLName,(char *)pLast->FileName);
		fLName[15] = '\0';
		sprintf(fLPathName,"%s/%s",sp5kFsGetCurPath(),fLName);
		/*printf("%s  ,%s  ,%s\n",fHPathName,pCheckFileName,fLPathName);*/
		if(strcmp(fLPathName,(char *)pCheckFileName)<0)
		{
			/*printf("\033[36m name is newer then pLast %s\033[0m\n",pLast->FileName);*/
			break;
		}
		else if(strcmp(fHPathName,(char *)pCheckFileName)>0)
		{
			/*printf("\033[36m name is older then pHead %s\033[0m\n",pHead->FileName);*/
			break;
		}
		else if((strcmp(fHPathName,(char *)pCheckFileName)==0)||(strcmp(fLPathName,(char *)pCheckFileName)==0))
		{
			if(strcmp(fHPathName,(char *)pCheckFileName)==0)
			{
				headMark =TRUE;
				pCurr = pHead;
				/*printf("\033[36m H name is same\033[0m\n");*/
			}
			else
			{
				headMark =FALSE;
				pCurr = pLast;
				/*printf("\033[36m L name is same\033[0m\n");*/
			}
			if(pCurr->FileName[15]=='_')/*save and find max attach name */
			{
				attNum =((pCurr->FileName[16]-48)*10+pCurr->FileName[17]-48);/*get attach num*/
				printf("attNum = %u,sameNameMax = %u\n",attNum,sameNameMax);/**/
				sameNameMax = attNum>sameNameMax?attNum:sameNameMax;/*record max attach num*/
			}
			sameNameNum = sameNameMax +1;
			if(sameNameNum == 99)
			{
				sameNameNum = 99;
				RPrintf("same name file is over 100,cover the last file\n");
			}
			/*printf("file---->%s,sameNameNum =%d\n",fHPathName,sameNameNum);*/
			if(TRUE == headMark)
			{
				pHead = pHead->next;
				pCurr = pHead;
			}
			else
			{
				pLast = pLast->pre;
				pCurr=pLast;
			}
			if(pCurr!=NULL)
			{
				strcpy(fName,(char *)pCurr->FileName);
				fName[15] = '\0';
				sprintf(fPathName,"%s/%s",sp5kFsGetCurPath(),fName);
				/*printf(" filepath-->%s  ,%s\n",fPathName,pCheckFileName);*/
				if(strcmp(fPathName,(char *)pCheckFileName)!=0)
				{
					if(sameNameNum)
					{
						sprintf(attName,"_%02u",sameNameNum);
						strcat((char *)pCheckFileName, attName);
						ret = TRUE;
					}
					else
					{
						ret = FALSE;
					}
					printf("final file---->%s\n",pCheckFileName);/**/
					break;
				}
			}
			else
			{
				if(sameNameNum)
				{
					sprintf(attName,"_%02u",sameNameNum);
					strcat((char *)pCheckFileName, attName);
					ret = TRUE;
				}
				else
				{
					ret = FALSE;
				}
				printf("final file---->%s\n",pCheckFileName);/**/
				break;
			}
		}
		else
		{
			pHead=pHead->next;
			pLast=pLast->pre;
		}
	}
	printf("File name check finish!\n");
	return ret;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsListInfoDump
 * Function	       	: Dump the CDFS list infomation
 * Return value   	: void
 * Parameter1    	: Head of CDFS file table pointer (--)
 * Parameter2    	: Last of CDFS file table pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/    
void appCdfsListInfoDump(CDFS_FILE_TAB_T *pHead,CDFS_FILE_TAB_T *pLast)
{
	UINT32 num;
	CDFS_FILE_TAB_T *pObj;
	num=0;
	pObj = pHead;
#if JET_PANA_CDFS
	while(pObj->next == pHead)
#else
	while(pObj)
#endif
	{
		printf("0x%p-->NEXT[%u]:%s\n",pObj,num,pObj->FileName);
		pObj = pObj->next;
		num++;
	}
	pObj = pLast;
#if JET_PANA_CDFS
	while(pObj->pre == pHead)
#else
	while(pObj)
#endif
	{
		num--;
		printf("0x%p-->PRE[%u]:%s\n",pObj,num,pObj->FileName);
		pObj = pObj->pre;
		if(num == 0)
		{
			break;
		}
	}
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsCurrFolderFileListCreate
 * Function	       	: Create current folder and file list
 * Return value   	: result (UINT32 true/false)
 * Parameter1    	: folder (UINT32)
 * Parameter2    	: dirPath pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/    
UINT32 appCdfsCurrFolderFileListCreate(UINT32 folder,UINT8* dirPath)
{

	UINT32 fnLen = 256;
	UINT8 *fname = NULL;
	UINT32 attr;
	UINT64 filesize;
#if JET_PANA_CDFS
	UINT32 fnoLen = 64;
	char filename[13] = { 0 };
	UINT32 flen = sizeof(filename);
	tmx_t tm;
	char dirPathtmp[64];
	UINT32 cdfs_idx;
	int subfolder_index, i;
	BOOL subfolder_array[JET_CDFS_DCIM_SUBFOLDER_NUM_MAX];
  	
#endif
	DBG_PRINT("cdfs: %s", __FUNCTION__);

#if !JET_PANA_CDFS
	profLogAdd(0, "#S#CDFS#N#FFC");
#endif
	if (dirPath!= NULL)
	{
		int cntItem=0 ;
       	 	FileListCreateFlag = TRUE;
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: Current Root %s", dirPath);
#if JET_PANA_CDFS
       	 	if (sp5kFsDirChange(dirPath) == FAIL) {
                    HOST_PROF_LOG_PRINT(LEVEL_INFO,
                            "cdfs: chang folder fail %s has %d child", dirPath,
                            cntItem);
                    HOST_PROF_LOG_PRINT(LEVEL_INFO,
                            "cdfs: chang folder fail.%s", dirPath);
                if (appCdfsFolderInit(folder) == FAIL)
			return FAIL;
       	}
	appCdfsFileFolderSet(folder);
        if (folder == CDFS_FILE_FOLDER_VIDEO) {
		UINT32 ret=0;
            //appCdfsFileListTabInit(folder);
            
            strcpy(dirPathtmp, (char *)dirPath);
            memset(subfolder_array, 0, sizeof(subfolder_array));
            LAST_NML = JET_CDFS_DCIM_SUBFOLDER_START_INDEX;
            while (ret == 0) {
		PowerDetect pd;
		pd = appInfoPowerDETStateGet();
                ret = sp5kFsDirNextFileGet(filename, &flen, &attr, &filesize,
                        &tm);
                if (ret == 0) {
                    if (attr & SP5K_FS_ATTR_DIR) {
                        //filename : 101_NML
                        if (strlen(filename) == 7
                                && strcmp(filename + 3, "_NML") == 0) {
                            filename[3] = '\0';
                            subfolder_index = atoi(filename);
                            if (subfolder_index >= JET_CDFS_DCIM_SUBFOLDER_START_INDEX &&
					    subfolder_index <= JET_CDFS_DCIM_SUBFOLDER_INDEX_MAX) {
                                subfolder_array[subfolder_index - JET_CDFS_DCIM_SUBFOLDER_START_INDEX] = TRUE;
				if (pd == POWER_DET_PARKING_WAKEUP)
					goto parking;

                            }
                        }
                    }
                }
            }
	    /* FIXME below can marge to above code */
parking:
		 //monitortime = sp5kOsTimeGet();
            for (i = 0; i < JET_CDFS_DCIM_SUBFOLDER_NUM_MAX; i++) {
                if (subfolder_array[i] == TRUE) {

                        sprintf((char *)dirPath, "%s/%03d_NML",dirPathtmp, i + JET_CDFS_DCIM_SUBFOLDER_START_INDEX);
                    if (jet_appCdfsCurrFolderFileListAdd(CDFS_FILE_FOLDER_VIDEO,
                            dirPath) == FAIL){

                        	    return FAIL;
                    	}
                }
		
            }

		/* in case of 101_NML is newest */
		if (subfolder_array[i-1]) {
			i = 0;
			while (subfolder_array[i]) i++;
			if (i) {
				LAST_NML = (i - 1) + JET_CDFS_DCIM_SUBFOLDER_START_INDEX;
			}
		}
		
        } else {
        	int cntItem ;
            cntItem = sp5kDirFileCntGet(dirPath);
            //appCdfsFileListTabInit(folder);
           /* PAIS spec PHOTO count is 300 */
           if (folder == CDFS_FILE_FOLDER_JPG && cntItem > 300)
                   cntItem = 300;
            if (cntItem != 0) {
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s has %d child",
                        dirPath, cntItem);
		fname = (UINT8 *) sp5kMalloc(fnLen);
		cdfs_idx = 0;	/* record CDFS index */
		    //sp5kFsDirCacheSet(dirPath, 1);/*speed up*/

                while (cntItem && fname && IS_CARD_EXIST) {
			UINT32 ret;
			UINT8 *flgname;/*file long name*/
			cdfs_idx++;	/* CDFS start from 1 */
                        fnoLen = fnLen;

                        ret = sp5kFsDirNextFileGet((char *) fname, &fnLen,
                                &attr, &filesize, NULL);
			
                        if (ret != 0) {
                            if (fnLen > fnoLen) { /* filename buffer is not large enough */
                                if (fname)
                                    sp5kFree(fname);
                                fname = (UINT8 *) sp5kMalloc(fnLen);
                                if (fname) {
                                    ret = sp5kFsNextFileNameGet((UINT8*) fname,
                                            &fnLen);
                                    if (ret != 0) {
                                        break;
                                    }
                                }
                            } else {
                                break;
                            }
                        }
			
                        flgname = sp5kFsAliasNameGet((const UINT8*) fname,
                                SP5K_FS_STR_NATIVE);
                        if (!flgname) {
                            /*Isaac add this one to fixed if file name is to short , sp5kFsAliasNameGet will return the "(NULL)"*/
                            flgname = (UINT8 *) fname;
                        }
			AddFile((UINT8*) flgname, attr, filesize, dirPath);	
                        cntItem--;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
                        appWatchDogRefresh();
#endif
                    }
		    
                    if (fname)
                        sp5kFree(fname);
                    HOST_PROF_LOG_ADD(LEVEL_INFO, "cdfs: Search finish");
		    if (!IS_CARD_EXIST)
			    return FAIL;
            } else {
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: has no child in.%s",
                        dirPath);
            }

        }
    
#else
		cntItem = sp5kDirFileCntGet(dirPath);
		appCdfsFileListTabInit(folder);

		if (cntItem != 0)
		{
			HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s has %d child", dirPath, cntItem);
			if (sp5kFsDirChange(dirPath) == 0)
			{
				appCdfsFileFolderSet(folder);
				fname = sp5kMallocCache(fnLen);
				while (cntItem && fname)
				{	
					UINT32 ret;
					UINT8 *flgname;/*file long name*/
					tmx_t  t4unused;
					ret = sp5kFsDirNextFileGet((char *)fname, &fnLen, &attr, &filesize,  &t4unused);
					if (ret != SUCCESS)
					{
						if (fnLen > 256){	/* For FAT32, filename length should less than 256 */
							printf("file name length is %u?\n", fnLen);
							HOST_ASSERT(0);
						}
						else{
							break;
						}
					}
					flgname = sp5kFsAliasNameGet(fname,  SP5K_FS_STR_NATIVE);
					if(flgname)
					{
						UINT32 len;
                        			len = strlen((const char *)flgname);
						if(len < (CAP_FILE_LEN-1) || len > (BURST_FILE_LEN-1)){
							/* file name is too long or short*/
                            				HOST_PROF_LOG_PRINT(LEVEL_WARN, "cdfs: %s filename too long or too short", flgname);
							continue;
						}
					}
					else {
						flgname = fname;
					}

					AddFile((UINT8*)flgname, attr, filesize);
					cntItem--;
				}
				if (fname)	sp5kFree(fname);
			}
			else
			{
				HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: chang folder fail %s has %d child", dirPath, cntItem);
				if (sp5kFsDirChange(dirPath) == 0)
				{
					appCdfsFileFolderSet(folder);
				}
				HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: chang folder fail.%s",dirPath);
			}
			HOST_PROF_LOG_ADD(LEVEL_INFO, "cdfs: Search finish");
		}
		else
		{
			HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: has no child in.%s",dirPath);
			appCdfsFileFolderSet(folder);
			appCdfsFolderInit(folder);/*add by 20121112*/
		}
#endif
	}

    	FileListCreateFlag = FALSE;

	profLogAdd(0, "#E#CDFS#N#");

	return SUCCESS;
}


  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsEventFlagSet
 * Function	       	: Set event flag for CDFS
 * Return value   	: void
 * Parameter1    	: flag (JPG_EVENT_IN_VIDEO	= 1<<0 , VIDEO_EVENT_IN_VIDEO = 1<<1)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/    
void appCdfsEventFlagSet(UINT32 flag)
{
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: %s %x", __FUNCTION__, flag);

	CDFS_EVENT_OCCUR |= flag;
	if(flag == VIDEO_EVENT_IN_VIDEO)
	{
		appOsd_FontIconFontSet(ID_FONT_TK_ICON);
		//appOsdLib_OsdLayIconDraw(4,32,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_PLAYBACK_THUMBNAIL_PROTECT);
		fileProtected = 1;
	}
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsEventFlagClean
 * Function	       	: Clean event flag for CDFS
 * Return value   	: void
 * Parameter1    	: flag (JPG_EVENT_IN_VIDEO	= 1<<0 , VIDEO_EVENT_IN_VIDEO = 1<<1)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
void appCdfsEventFlagClean(UINT32 flag)
{
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: %s %x", __FUNCTION__, flag);

	CDFS_EVENT_OCCUR &= ~flag;
	if(flag == VIDEO_EVENT_IN_VIDEO)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,4, 32,36,32,0,0);
		fileProtected = 0;
	}
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsEventFlagGet
 * Function	       	: Get event flag for CDFS
 * Return value   	: flag (JPG_EVENT_IN_VIDEO	= 1<<0 , VIDEO_EVENT_IN_VIDEO = 1<<1)
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/    
UINT32 appCdfsEventFlagGet(void)
{
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: %s %x", __FUNCTION__, CDFS_EVENT_OCCUR);
	return CDFS_EVENT_OCCUR;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileFolderSet
 * Function	       	: Set folder for CDFS
 * Return value   	: result (UINT32 succes/fail)
 * Parameter1    	: folder (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/     
UINT32 appCdfsFileFolderSet(UINT32 folder)
{
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: %s %x", __FUNCTION__, folder);
	CDFS_CUR_FILE_FOLDER = folder;
	return SUCCESS;
}


  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileFolderGet
 * Function	       	: Get CDFS folder
 * Return value   	: folder (UINT32)
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/    
UINT32 appCdfsFileFolderGet(void)
{
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s %x", __FUNCTION__, CDFS_CUR_FILE_FOLDER);
	return CDFS_CUR_FILE_FOLDER;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsCurFileIdxSet
 * Function	       	: Set Current File index
 * Return value   	: result (UINT32 SP5K_ERR_SUCCESS/SP5K_ERR_DCF_PARAM_GROUP)
 * Parameter1    	: file index (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/     
UINT32 appCdfsCurFileIdxSet(UINT32 fileIdx)
{
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: %s %x", __FUNCTION__, fileIdx);

    if(CDFS_CUR_FILE_FOLDER<CDFS_FILE_FOLDER_MAX)
    {
#if JET_PANA_CDFS
	UNIFY_FOLDER.folder_info.cur_file_idx = fileIdx;
#else
        CDFS_TAB[CDFS_CUR_FILE_FOLDER].folder_info.cur_file_idx = fileIdx;
#endif
    }
    else
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s %d CDFS Folder Fail", __FUNCTION__,__LINE__);
        	DBG_ASSERT(0);
	}

	return SP5K_ERR_SUCCESS;
}


 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsCurFileIdxGet
 * Function	       	: Get Current File index
 * Return value   	: result (UINT32 SP5K_ERR_SUCCESS/SP5K_ERR_DCF_PARAM_GROUP)
 * Parameter1    	: file index pointer(--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/      
UINT32 appCdfsCurFileIdxGet(UINT32 *fileIdx)
{
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: %s %x",__FUNCTION__ ,CDFS_CUR_FILE_FOLDER);

    	if(CDFS_CUR_FILE_FOLDER<CDFS_FILE_FOLDER_MAX)
    	{
#if JET_PANA_CDFS
        	*fileIdx = UNIFY_FOLDER.folder_info.cur_file_idx;
#else
       		*fileIdx = CDFS_TAB[CDFS_CUR_FILE_FOLDER].folder_info.cur_file_idx;
#endif
    	}
    	else
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s %d CDFS Folder Fail", __FUNCTION__,__LINE__);
        	DBG_ASSERT(0);
	}

	return SP5K_ERR_SUCCESS;
}


 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileNumGet
 * Function	       	: Get the file number of CDFS
 * Return value   	: result (UINT32 true/false)
 * Parameter1    	: file number pointer(--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/      
UINT32 appCdfsFileNumGet(UINT32 *filenum)
{
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: %s %x",__FUNCTION__ ,CDFS_CUR_FILE_FOLDER);

	  if(CDFS_CUR_FILE_FOLDER<CDFS_FILE_FOLDER_MAX)
	 {
#if JET_PANA_CDFS
    		*filenum = UNIFY_FOLDER.folder_info.file_num;
#else
    		*filenum = CDFS_TAB[CDFS_CUR_FILE_FOLDER].folder_info.file_num;
#endif
    	}
    	else
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s %d CDFS Folder Fail", __FUNCTION__,__LINE__);
        	DBG_ASSERT(0);
	}

    return SUCCESS;
}

#if !JET_PANA_CDFS
 /**
 * Fucntion name :appCdfsFileHandleGet
 * @brief Get the file Handleof CDFS(idx start from 1)
 * @param UINT32 idx
 * @return  CDFS_FILE_TAB_T
 * @aothur 
 * @date 04/10/2018
 *
 */
 
CDFS_FILE_TAB_T * appCdfsFileHandleGet(UINT32 idx)
{
	CDFS_FILE_TAB_T *pCur=NULL;
	DBG_PRINT("cdfs: %s", __FUNCTION__);

	if(idx == 0)
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s idx=%d", __FUNCTION__, idx);
		return NULL;
	}

	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: %s idx: %d", __FUNCTION__, idx);

    /*get table header*/
    pCur = appCdfsFileListHeadGet(CDFS_CUR_FILE_FOLDER);

	if(pCur == NULL)
	{
		return pCur;
	}

	if(idx)
	{
		idx--;
	}
	while(idx)
	{
		if(pCur->next)
		{
			pCur = pCur->next;
		}
		else
		{
			pCur = NULL;
			break;
		}
		idx--;
	}

	return pCur;
}
#endif

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileAttrGet
 * Function	       	: Get the Attr table file info
 * Return value   	: result (UINT32 success/fail)
 * Parameter1    	: file index (UINT32)
 * Parameter2    	: file info attribute table pointer(--)
 * Parameter3    	: file name pointer(--) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/      
UINT32 appCdfsFileAttrGet(UINT32 fileIdx, sp5kDcfAttrElm_t* pfileInfoAtr, UINT8* pname)
{
	CDFS_FILE_TAB_T *pCur;
#if JET_PANA_CDFS
	char path[32];
#else
	UINT8 fname[64]={0};
#endif
	UINT32 fileNum;
	UINT32 date,time;
	UINT8 *pTime;

	DBG_PRINT("cdfs: %s", __FUNCTION__);
#if JET_PANA_CDFS
	pCur = jetFolderFilterFileNodeFind(fileIdx);
#else
	pCur=appCdfsFileHandleGet(fileIdx);
#endif
	if(!pCur)
	{
		HOST_PROF_LOG_ADD(LEVEL_ERROR, "cdfs: NO file In this index");
		return FAIL;
	}

#if JET_PANA_CDFS
	strncpy(path, jet_cdfs_folder_to_path(pCur->folder), sizeof(path));
#endif
	pTime = &(pCur->FileName[9]);
	date = strtol((char*)pCur->FileName,NULL,10);
	time = strtol((char*)pTime,NULL,10);
	appCdfsFileNumGet(&fileNum);
#if JET_PANA_CDFS
	sprintf((char *)pname,"%s/%s", path, (char *)pCur->FileName);/*absolute path*/
#else
	sprintf((char *)fname,"%s/",sp5kFsGetCurPath());
	strcat((char*)fname, (char *)pCur->FileName);/*absolute path*/
	strcpy((char*)pname, (const char*)fname);
#endif
	
	strcpy((char*)pfileInfoAtr->filename,(const char*)pCur->FileName);
	strcpy((char*)pfileInfoAtr->dirname,(const char*)sp5kFsGetCurPath());
	
	pfileInfoAtr->attribute = pCur->dcfattr;
	pfileInfoAtr->filesize = pCur->FileSize;
	pfileInfoAtr->fileType = pCur->FileType;
	pfileInfoAtr->dirNum = 1;
	pfileInfoAtr->fileNum = fileNum;
	pfileInfoAtr->time.year = date/10000;
	pfileInfoAtr->time.mon = date%10000/100;
	pfileInfoAtr->time.day =  date%10000%100;
	pfileInfoAtr->time.hour = time/10000;
	pfileInfoAtr->time.min = time%10000/100;
	pfileInfoAtr->time.sec = time%10000%100;
	return SUCCESS;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsInfoGet
 * Function	       	: Get CDFS infomation
 * Return value   	: result (UINT32 success/fail)
 * Parameter1    	: option (UINT32)
 * Parameter2    	: value pointer(--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/      
UINT32 appCdfsInfoGet(UINT32 option, UINT32* pValue)
{
	UINT32 ret = SUCCESS;
	DBG_PRINT("cdfs: %s %d", __FUNCTION__,option);

	switch(option)
	{
		case SP5K_DCF_INFO_TOTAL_FILENUM:
			appCdfsFileNumGet(pValue);
			break;
		default:
			break;
	}

	return ret;
}



  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsAllFileProcess
 * Function	       	: Process all the file of CDFS
 * Return value   	: result (UINT32 success/fail)
 * Parameter1    	: option (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/      
UINT32 appCdfsAllFileProcess(UINT32 opt)
{
	UINT32 num=0;
	CDFS_FILE_TAB_T *pObj;
	UINT8 fileName[64];
	UINT8 dirName[20];
	DBG_PRINT("cdfs: %s %d", __FUNCTION__,opt);

    /*get table header*/
    pObj = appCdfsFileListHeadGet(CDFS_CUR_FILE_FOLDER);

	strcpy((char*)dirName,(const char*)sp5kFsGetCurPath());

	if(CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
		{
			CDFS_VIDEO_SIZE=0;
		}
	}

	while(pObj)
	{
		switch(opt)
		{
			case SP5K_DCF_FILEOP_LOCKALL:
				sprintf((char *)fileName,"%s/%s",dirName,pObj->FileName);
				pObj->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
				sp5kFsFileAttrSet(fileName,pObj->attr | SP5K_FS_ATTR_READONLY);
				break;

			case SP5K_DCF_FILEOP_UNLOCKALL:
				sprintf((char *)fileName,"%s/%s",dirName,pObj->FileName);
				pObj->dcfattr &= ~SP5K_DCF_FILE_ATTR_LOCK;
				sp5kFsFileAttrSet(fileName,pObj->attr &(~ SP5K_FS_ATTR_READONLY));
				if(CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
				{
					if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
					{
						CDFS_VIDEO_SIZE += pObj->FileSize;
					}
				}

				break;
			default:
				break;

		}

		if(pObj->next)
		{
			pObj=pObj->next;
			num++;
		}
		else
		{
			break;
		}
	}

	return SUCCESS;
}


  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileOperate
 * Function	       	: CDFS file operate process
 * Return value   	: result (UINT32 success/fail)
 * Parameter1    	: option (UINT32)
 * Parameter2    	: param (UINT32)
 * Parameter3    	: async (UINT32)
 * Parameter4    	: fpsp5kCallback_t callback function(--)
 * Parameter5    	: length (UINT32) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/      
UINT32 appCdfsFileOperate(UINT32 option, UINT32 param, UINT32 async, fpsp5kCallback_t pcallback, UINT32 length)
{
	CDFS_FILE_TAB_T *pCur;
	UINT32 fileIdx, ret = SUCCESS;
	UINT8 fileName[64];

	/*get file index and handle*/
	appCdfsCurFileIdxGet(&fileIdx);
	
#if JET_PANA_CDFS
	pCur = jetFolderFilterFileNodeFind(fileIdx);
	if (pCur)
		sprintf((char *)fileName,"%s/%s",
			  jet_cdfs_folder_to_path(pCur->folder), pCur->FileName);
#else
	UINT8 fileName_1[64];
	pCur = appCdfsFileHandleGet(fileIdx);
	strcpy((char*)fileName_1,(const char*)sp5kFsGetCurPath());
	sprintf((char *)fileName,"%s/%s",fileName_1,pCur->FileName);
#endif
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s filename=%s option=%d", __FUNCTION__, fileName, option);

	switch(option)
	{
		case SP5K_DCF_FILEOP_DEL:
#if JET_PANA_CDFS
			appCdfsFileDel(pCur);
#else
			sp5kFsFileDelete(fileName);
		    appCdfsFileDel(fileName);
#endif
			/*paul modify Bug #3419,saw the log crush on this function*/
		       	/*appCdfsFileDel(fileName);*/
			break;

		case SP5K_DCF_FILEOP_LOCK:
			pCur->dcfattr |= SP5K_DCF_FILE_ATTR_LOCK;
			sp5kFsFileAttrSet(fileName,pCur->attr |SP5K_FS_ATTR_READONLY);
			if(CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
			{
				if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
				{
					CDFS_VIDEO_SIZE-=pCur->FileSize;
				}
			}
			break;

		case SP5K_DCF_FILEOP_LOCKALL:
			appCdfsAllFileProcess(SP5K_DCF_FILEOP_LOCKALL);
			break;

		case SP5K_DCF_FILEOP_UNLOCK:
			pCur->dcfattr &= ~SP5K_DCF_FILE_ATTR_LOCK;
			sp5kFsFileAttrSet(fileName,pCur->attr &(~SP5K_FS_ATTR_READONLY));
			if(CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
			{
				if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
				{
					CDFS_VIDEO_SIZE+=pCur->FileSize;
				}
			}
			break;

		case SP5K_DCF_FILEOP_UNLOCKALL:
			appCdfsAllFileProcess(SP5K_DCF_FILEOP_UNLOCKALL);
			break;

		case SP5K_DCF_FILEOP_FORMAT:
			if(IS_CARD_EXIST)
			{
				sp5kDiskFormat(DRIVE_SD, NULL, 0);
				sp5kDiskMount(DRIVE_SD);
			}
			else
			{
				sp5kDiskFormat(DRIVE_NAND, NULL, 0);
				sp5kDiskMount(DRIVE_NAND);
			}
#if !JET_PANA_CDFS
			appCdfsFileListTabInit(CDFS_FILE_FOLDER_VIDEO);
			appCdfsFileListTabInit(CDFS_FILE_FOLDER_JPG);
			appCdfsFileListTabInit(CDFS_FILE_FOLDER_EVENT);
			appCdfsFolderInit(CDFS_FILE_FOLDER_EVENT);
			appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
			appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
#endif
			break;

		case SP5K_DCF_FILEOP_COPY:
		case SP5K_DCF_FILEOP_COPYALL:
		case SP5K_DCF_FILEOP_DELALL:
		case SP5K_DCF_FILEOP_ABORT:
		case SP5K_DCF_FILEOP_COPY_BY_DIR:
		case SP5K_DCF_FILEOP_COPYALL_BY_DIR:
			break;

		default:
			break;
	}
	return ret;
}


  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsCanDelVideoSizeGet
 * Function	       	: Get the video size which can be delete
 * Return value   	: video size(UINT64)
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/       
UINT64 appCdfsCanDelVideoSizeGet(void)
{
	DBG_PRINT("cdfs: %s %d", __FUNCTION__,CDFS_VIDEO_SIZE);
	return CDFS_VIDEO_SIZE;/*unit: Byte*/
}


  /*--------------------------------------------------------------------------*
 * Function name	: appCdfsTheOldestFileDel
 * Function	       	: Delete the oldest file 
 * Return value   	: result (UINT32 success/fail)
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/        
UINT32 appCdfsTheOldestFileDel(void)
{	
	UINT8 fileName[64];
	UINT32 ret=SUCCESS;
	DBG_PRINT("cdfs: %s", __FUNCTION__);

    #if 0
    profLogAdd(0, "appCdfsTheOldestFileDel s");
    #endif
	printf("%s(%d) %u\n", __func__, __LINE__, CDFS_CUR_FILE_FOLDER);
	if(CDFS_CUR_FILE_FOLDER == CDFS_FILE_FOLDER_VIDEO)
	{
		CDFS_FILE_TAB_T *pCur;
		UINT8 dirName[20];
#if !JET_PANA_CDFS
		strcpy((char*)dirName,(const char*)sp5kFsGetCurPath());
		pCur = appCdfsFileListHeadGet(CDFS_FILE_FOLDER_VIDEO);
#else
		pCur = appCdfsFolderNodeFind(CDFS_FILE_FOLDER_VIDEO_BIT, NULL);
#endif
		if(pCur != NULL)
			printf("%s(%d) oldest %s 0x%04x\n", __func__, __LINE__, pCur->FileName, pCur->dcfattr);
		else
			printf("%s(%d) pCur NULL\n", __func__, __LINE__);
		if (pCur == NULL) {
			HOST_PROF_LOG_ADD(LEVEL_ERROR, "cdfs: file list is empty.");
			ret = __LINE__;
		}
		else if(CDFS_DEL_VIDEO_METHOD == CDFS_PROTECTED_SKIP)
		{

			while(1)
			{
	
				if(pCur->dcfattr & SP5K_DCF_FILE_ATTR_LOCK)
				{
#if JET_PANA_CDFS
					pCur = appCdfsFolderNodeFind(CDFS_FILE_FOLDER_VIDEO_BIT, pCur);
					if (pCur == NULL) {
					    ret = FAIL;
					    HOST_PROF_LOG_ADD(LEVEL_ERROR, "cdfs: del file error");
					    break;
					}

#else
					if(pCur->next)
					{
						pCur = pCur->next;
					}
					else
					{
						ret = FAIL;
						HOST_PROF_LOG_ADD(LEVEL_ERROR, "cdfs: del file error");
						break;
					}
#endif
				}
				else
				{
#if JET_PANA_CDFS
					printf("%s %d deleted\n", __func__, __LINE__);
					appCdfsFileDel(pCur);
					break;
#else
		                    #if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
		                    /*delete 2file - dual record file*/
				    CDFS_FILE_TAB_T *pNext;
				    char cmp1[64] = {0} , cmp2[64] = {0},*p;
		                    pNext = pCur->next;
		                    p = strchr((char *)pCur->FileName, 'M');
		                    if(p)
		                    {
		                    	UINT8 len;
		                        len = strlen((const char *)pCur->FileName)-strlen((const char *)p);
		                        strncpy(cmp1, (const char*)pCur->FileName, len);
		                        p = strchr((char *)pNext->FileName, 'S');
		                        if(p)
		                        {
		                            len = strlen((const char *)pNext->FileName)-strlen((const char *)p);
		                            strncpy(cmp2, (const char*)pNext->FileName, len);
		                        }
		                    }
		                    #endif

					sprintf((char *)fileName,"%s/%s",dirName,pCur->FileName);

		                    profLogPrintf(4, "#S#CDFS#N#DEL:%s", fileName);
							ret = sp5kFsFileDelete(fileName);
		                    HOST_PROF_LOG_PRINT(LEVEL_INFO,"cdfs: del file %s\n", fileName);
						    appCdfsFileDel(fileName);
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
		                    if(strcmp((const char *)cmp1,(const char *)cmp2) == 0) {
		                        sprintf((char *)fileName,"%s/%s",dirName,pNext->FileName);
		                        ret = sp5kFsFileDelete(fileName);
		                        HOST_PROF_LOG_PRINT(LEVEL_INFO,"cdfs: del pair file %s\n", fileName);
		                        appCdfsFileDel(fileName);
                    			}
#endif
                  				  profLogPrintf(4, "#E#CDFS#N#DEL");
					break;
#endif
				}

			}
		}
		else
		{
			sprintf((char *)fileName,"%s/%s",dirName,pCur->FileName);
#if JET_PANA_CDFS
			if (sp5kFsFileDelete(fileName) == SUCCESS)/*file name with dir path*/
			{
			    appCdfsFileDel(pCur);
			} else
			HOST_PROF_LOG_PRINT(LEVEL_INFO,"cdfs: Function:%s Line:%d, del fail\n", __FUNCTION__, __LINE__);
#else
			sp5kFsFileDelete(fileName);
   		   	appCdfsFileDel(fileName);
#endif
		}
	}
	else
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "cdfs: %s %d", __FUNCTION__,CDFS_CUR_FILE_FOLDER);
	}

    #if 0
    profLogAdd(0, "appCdfsTheOldestFileDel e");
    #endif

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: Del file : %s",fileName);
	return ret;
}


 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsTheOldestProtectedFileDel
 * Function	       	: Delete the oldest  protected file  
 * Return value   	: result (UINT32 success/fail)
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/        
UINT32 appCdfsTheOldestProtectedFileDel(void)
{
	CDFS_FILE_TAB_T *pCur;
	
	UINT8 dirName[20];
#if !JET_PANA_CDFS
	UINT8 dirNameBak[20];
	UINT32 cdfsFolderBak;
#endif
	UINT32 ret=SUCCESS;
	DBG_PRINT("cdfs: %s", __FUNCTION__);
#if !JET_PANA_CDFS
	cdfsFolderBak = appCdfsFileFolderGet();
	appCdfsFolder2Path(cdfsFolderBak, dirNameBak);
	appCdfsFolder2Path(CDFS_FILE_FOLDER_EVENT,dirName);
	appCdfsFileFolderSet(CDFS_FILE_FOLDER_EVENT);
#endif
	pCur= appCdfsFileListHeadGet(CDFS_FILE_FOLDER_EVENT);
	
	if(pCur!=NULL)
	{

	if(sp5kFsDirChange(dirName) == 0)
	{
		UINT8 fileName[64];
		if(pCur!=NULL)
		{
			sprintf((char *)fileName,"%s/%s",dirName,pCur->FileName);
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: Del file : %s",fileName);
		pCur->dcfattr &= ~SP5K_DCF_FILE_ATTR_LOCK;

		sp5kFsFileAttrSet(fileName, pCur->attr&=(~SP5K_FS_ATTR_READONLY));

		sp5kFsFileDelete(fileName);
		}
#if JET_PANA_CDFS
		if (pCur)
			appCdfsFileDel(pCur);
#else
	    	appCdfsFileDel(fileName);
#endif
	}
	}
#if !JET_PANA_CDFS
	sp5kFsDirChange(dirNameBak);
	appCdfsFileFolderSet(cdfsFolderBak);
#endif
	return ret;
}



 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsEventFolderSizeCheck
 * Function	       	: Check Event folder size
 * Return value   	: result (UINT32 0~2)
 * Parameter1    	: void
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/         
UINT32 appCdfsEventFolderSizeCheck(void)
{
	UINT32 eventFileSize;
	DBG_PRINT("cdfs: %s", __FUNCTION__);

	eventFileSize = (UINT32)(CDFS_EVENT_SIZE>>20);
	//printf("eventsize:%d,cardsize:%d\n", eventFileSize, cardTotalSize);

	if(eventFileSize >= cardTotalSize/3){
		return EVENT_WARN_STAGE_TWO;
	}
	else if(eventFileSize >= cardTotalSize*7/30){
		return EVENT_WARN_STAGE_ONE;
	}
	else{
		return 0;
	}
}

 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFreeSpaceCheck
 * Function	       	: Check free space
 * Return value   	: result (UINT32 success/fail)
 * Parameter1    	: folder (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/     
UINT32 appCdfsFreeSpaceCheck(UINT32 folder)
{
#if 0
	CDFS_FILE_TAB_T *pCur;
	UINT8 dirName[20];
#endif
	UINT32 ret=SUCCESS;
	UINT32 pvalue=0;
	UINT32 freesec=0;
	UINT32 seamsec;
	DBG_PRINT("cdfs: %s", __FUNCTION__);

	if(folder != CDFS_FILE_FOLDER_VIDEO)
	{
		return SUCCESS;
	}

	if(appCdfsFileFolderGet()!=CDFS_FILE_FOLDER_VIDEO)
	{
		appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
		appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
	}

	freesec = appVideoRemainSecGet();
	seamsec = appVideoSeamLessSecGet(appActiveStateGet());

	HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: free time=%d sec,limit time%d sec",freesec,seamsec);

	if(sp5kAdcRead(ADC_BATTERY_CH, &pvalue)== SUCCESS)
	{
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: battery value=%d",pvalue);
	}
#if JET_PANA_CDFS
	if(appActiveStateGet() == APP_STATE_VIDEO_REC)
#endif
	{
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: free time=%d sec,limit time %d sec    2",freesec,seamsec);
		if (freesec >= seamsec)
			return SUCCESS;

		HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: free time=%d sec,limit time %d sec  3",freesec,seamsec);
	}
#if JET_PANA_CDFS
	else
#endif
	#if 0
	strcpy((char*)dirName,(const char*)sp5kFsGetCurPath());
	pCur = appCdfsFileListHeadGet(CDFS_FILE_FOLDER_VIDEO);
	#endif

	/*seamless cdfs control*/
	{
		while(1)
		{
			freesec = appVideoRemainSecGet();

			if(freesec < seamsec)
			{
				if(appCdfsCanDelVideoSizeGet())
				{
					HOST_PROF_LOG_ADD(LEVEL_INFO, "cdfs: del the oldest file");
					ret = appCdfsTheOldestFileDel();
					if(ret==FAIL)
					{
					   HOST_PROF_LOG_ADD(LEVEL_ERROR, "cdfs: del the oldest file fail");
					   break;
					}
				}
				else
				{
					HOST_PROF_LOG_ADD(LEVEL_ERROR, "cdfs: video size get error");
					ret = FAIL;
					break;
				}
			}
			else
			{
				ret = SUCCESS;
				break;
			}
		}
	}
	return ret;
}


 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileNameValidCheck
 * Function	       	: Check file name valid
 * Return value   	: result (UINT32 success/fail)
 * Parameter1    	: file type (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/  
UINT32 appCdfsFileNameValidCheck(UINT32 filetype)
{
	UINT8 lastName[64];
	UINT8 filename[64];
	DBG_PRINT("cdfs: %s", __FUNCTION__);

	strcpy((char*)lastName,(const char*)appCdfsLastFileNameGet());
	appCdfsNextFileNameGet(filetype,filename);
	if(strcmp((const char *)lastName,(const char *)filename) != 0)
	{
		return SUCCESS;
	}
	else
	{
		HOST_PROF_LOG_ADD(LEVEL_ERROR, "cdfs: name invalid");
		return FAIL;
	}
}


 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsFileNameToDateTime
 * Function	       	: check file name by date time
 * Return value   	: result (UINT32 success/fail)
 * Parameter1    	: file name pointer (--)
 * Parameter2    	: time table pointer (--) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/   
UINT32 appCdfsFileNameToDateTime(UINT8 *filename,tmx_t *rtc)
{
	UINT32 date,time;
	UINT8 *pChar;
	DBG_PRINT("cdfs: %s", __FUNCTION__);

	if(!filename)
	{
		HOST_PROF_LOG_ADD(LEVEL_ERROR, "cdfs: file name error");
		return FAIL;
	}
	pChar = filename;
	date = strtol((const char *)pChar,NULL,10);
	pChar = filename+9;/*format : 20120601_121013   */
	time = strtol((const char *)pChar,NULL,10);
	if(date<19000000)
	{
		HOST_PROF_LOG_ADD(LEVEL_ERROR, "cdfs: date error");
		return FAIL;
	}
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "cdfs: %s,%d,%d",filename,date,time);
	rtc->tmx_year = date/10000-1900;
	rtc->tmx_mon = date%10000/100;
	rtc->tmx_mday=  date%10000%100;
	rtc->tmx_hour = time/10000;
	rtc->tmx_min = time%10000/100;
	rtc->tmx_sec = time%10000%100;

	return SUCCESS;
}

#if !JET_PANA_CDFS
 /**
 * Fucntion name :appCdfsRtcSet
 * @brief Set Rtc to file time
 * @param void
 * @return  UINT32
 * @aothur 
 * @date 04/10/2018
 *
 */
UINT32 appCdfsRtcSet(void)
{
	tmx_t time,fileTime;	
	UINT8 maxName[64]={0};
    	UINT32 folder;
	DBG_PRINT("cdfs: %s", __FUNCTION__);

	sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &time);

	for(folder=0; folder<CDFS_FILE_FOLDER_MAX; folder++)
	{
		CDFS_FILE_TAB_T *pObj;
		pObj = appCdfsFileHandleGet(CDFS_TAB[folder].folder_info.file_num);
		if(pObj)
		{
			if(strcmp((const char *)pObj->FileName,(const char *)maxName)>0)
			{
				strcpy((char*)maxName,(const char*)pObj->FileName);
			}
		}
	}

	if(appCdfsFileNameToDateTime(maxName,&fileTime) == FAIL)
	{
		return SUCCESS;
	}
	/*printf("filetime=%d_%d_%d   %d:%d:%d \n",fileTime.tmx_year,
		fileTime.tmx_mon,fileTime.tmx_mday,fileTime.tmx_hour,fileTime.tmx_min,fileTime.tmx_sec);*/
	fileTime.tmx_min += 3;
	if(fileTime.tmx_year >time.tmx_year)
	{
		sp5kRtcDateTimeSet(SP5K_DATE_TIME_OPTION,&fileTime);
	}
	else if(fileTime.tmx_mon>time.tmx_mon)
	{
		sp5kRtcDateTimeSet(SP5K_DATE_TIME_OPTION,&fileTime);
	}
	else if(fileTime.tmx_mday>time.tmx_mday)
	{
		sp5kRtcDateTimeSet(SP5K_DATE_TIME_OPTION,&fileTime);
	}
	else if(fileTime.tmx_hour>time.tmx_hour)
	{
		sp5kRtcDateTimeSet(SP5K_DATE_TIME_OPTION,&fileTime);
	}
	else if(fileTime.tmx_min>time.tmx_min)
	{
		sp5kRtcDateTimeSet(SP5K_DATE_TIME_OPTION,&fileTime);
	}
	else if(fileTime.tmx_sec>time.tmx_sec)
	{
		sp5kRtcDateTimeSet(SP5K_DATE_TIME_OPTION,&fileTime);
	}
	return SUCCESS;
}
#endif

#if JET_PANA_CDFS

 /*--------------------------------------------------------------------------*
 * Function name	: _check_type
 * Function	       	: check CDFS file folder type
 * Return value   	: result (BOOL true/falsel)
 * Parameter1    	: type (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/    
static BOOL _check_type(UINT32 v)
{
	BOOL ret = FALSE;
	if (v >= CDFS_FILE_FOLDER_FAIL) {
		printf("[type] is wrong, can not be '0' or larger then '%u'\n",
				    CDFS_FILE_FOLDER_FAIL);
	} else
		ret = TRUE;

	return ret;
}
#endif

 /*--------------------------------------------------------------------------*
 * Function name	: appBurstTimeCbReg
 * Function	       	: Burst time call back Reg
 * Return value   	: void
 * Parameter1    	: function pointer (--)
 * Parameter2    	: number (UINT32) 
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/     
void appBurstTimeCbReg(void *pfunc, UINT32 num)
{
	cdfsFileBurstTimeCb = (burstTimeGetCb)pfunc;
	burstNum = num;
}

 /*--------------------------------------------------------------------------*
 * Function name	: appCdfsCbReg
 * Function	       	: CDFS call back Reg
 * Return value   	: void
 * Parameter1    	: sel (UINT32)
 * Parameter2    	: function pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/      
void
appCdfsCbReg(
	UINT32 sel,
	void *pfunc
)
{
	switch (sel) {
		case APP_CDFS_ADD_FILE_CB:
			cdfsFileAddCb = (fileOperateCb)pfunc;
			break;
		case APP_CDFS_DELETE_FILE_CB:
			cdfsFileDelCb = (fileOperateCb)pfunc;
			break;
		default:
			break;
	}
}

 /*--------------------------------------------------------------------------*
 * Function name	: cmdCDFS
 * Function	       	: Command process for CDFS
 * Return value   	: void
 * Parameter1    	: argc(UINT32)
 * Parameter2    	: arg[] pointer (--)
 * Parameter2    	: value[] (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/       
void
cmdCDFS(	UINT32 argc,	UINT8 *arg[],	UINT32 v[]
)
{
	if (argc==1) {
	    printf("cdfs usage:\n"
	           "cdfs dir\n"
	           );
	}
#if JET_PANA_CDFS
	else if (strcmp((const char *) arg[1], "cnt") == 0 && argc == 3) {
		if (_check_type(v[2]) == FALSE) {
			return;
		}

		UINT64 l = appCdfsUsedSpace(v[2]);
		UINT32 cnt = appCdfsFolderCount(v[2]);
		printf("Total size of %u folder(%u) = %llu BYTE\n", v[2], cnt, l);

	}
	else if (strcmp((const char *) arg[1], "dir") == 0 && argc == 3) {
		int num = 0;
		UINT64 fs = 0;
		CDFS_FILE_TAB_T *pCur = NULL;

		if (_check_type(v[2]) == FALSE) {
			return;
		}
		pCur = appCdfsFolderNodeFind(1 << v[2], pCur);
		while (pCur != NULL) {
			HOST_PROF_LOG_PRINT(LEVEL_INFO,
			    "cdfs: file[%d]---->%s " "attr=0x%x, filesize=%llu, folder=%u event=0X%X index=%d",
			    num, pCur->FileName, pCur->dcfattr, pCur->FileSize,
			    pCur->folder, pCur->event, pCur->index);
#if DEBUG_ON
				debug_file_t(pCur);
#endif
			num++;
			fs += pCur->FileSize;
			pCur = appCdfsFolderNodeFind(1 << v[2], pCur);
		};

		HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs[%d]: file size %llu\n", v[2], fs);
	}
	else if (strcmp((const char *) arg[1], "dir") == 0 && argc == 2) {
		appCdfsFileListPrintf(CDFS_FILE_FOLDER_VIDEO);

	}
#else
	else if (strcmp((const char *)arg[1], "dir")==0) {
        if (argc>2) {
            if (strcmp((const char *)arg[2], "jpg")==0)
                appCdfsFileListPrintf(CDFS_FILE_FOLDER_JPG);
            else if (strcmp((const char *)arg[2], "video")==0)
                appCdfsFileListPrintf(CDFS_FILE_FOLDER_VIDEO);
            else if (strcmp((const char *)arg[2], "event")==0)
                appCdfsFileListPrintf(CDFS_FILE_FOLDER_EVENT);
            else
                printf("Only jpg/video/event are available\n");
        }
        else {
            appCdfsFileListPrintf(CDFS_FILE_FOLDER_JPG);
            appCdfsFileListPrintf(CDFS_FILE_FOLDER_VIDEO);
            appCdfsFileListPrintf(CDFS_FILE_FOLDER_EVENT);
        }
    }
	else if (strcmp((char *)arg[1], "usage") == 0) {
		cdfsNodeInfoDisp(0);
	}
#endif
}


#if JET_PANA_CDFS

/*--------------------------------------------------------------------------*
 * Function name	: jet_appCdfsTheOldestFileDel
 * Function	       	: Delete the oldest file 
 * Return value   	: result (UINT32 SUCCESS/FAIL)
 * Parameter1    	: cdfs folder(UINT8)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/       
UINT32 jet_appCdfsTheOldestFileDel(UINT8 cdfs_folder)
{
	CDFS_FILE_TAB_T *pCur;
	UINT32 ret=SUCCESS;
	DBG_PRINT("cdfs: %s", __FUNCTION__);

	pCur = appCdfsFolderNodeFind(1 << cdfs_folder, NULL);
	if (pCur)
		appCdfsFileDel(pCur);
	else
		HOST_PROF_LOG_PRINT(LEVEL_INFO,"cdfs: Function:%s Line:%d, del fail\n",
				__FUNCTION__,__LINE__);
	return ret;
}

/*--------------------------------------------------------------------------*
 * Function name	: appCdfsCheckLastNmlFolder
 * Function	       	: Check the last video subfolder(NML)
 * Return value   	: void
 * Parameter1    	: folder(UINT32)
 * Parameter2    	: path pointer(--)
 * Parameter3    	: folder count (int )
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/        
static void appCdfsCheckLastNmlFolder(UINT32 folder, UINT8* path, int cnt)
{
    char *p;
    int i;

    if (folder != CDFS_FILE_FOLDER_VIDEO)
        return;

    p = strrchr((char *)path, '/');
    if (p == NULL)
        return;
    p++;
    i = atoi(p);
    if (i >= JET_CDFS_DCIM_SUBFOLDER_START_INDEX &&
		i <= JET_CDFS_DCIM_SUBFOLDER_INDEX_MAX) {
        if (i >= LAST_NML) {
            LAST_NML = i;
            if (cnt >= 200) {
                LAST_NML++;

                if (LAST_NML > JET_CDFS_DCIM_SUBFOLDER_INDEX_MAX)
                    LAST_NML = JET_CDFS_DCIM_SUBFOLDER_START_INDEX;
            }
        }
    }
}

/*--------------------------------------------------------------------------*
 * Function name	: jet_appCdfsCurrFolderFileListAdd
 * Function	       	: Add current folder file list
 * Return value   	: result (UINT32 success/fail)
 * Parameter1    	: folder(UINT32)
 * Parameter2    	: dirpath pointer(--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/        
UINT32 jet_appCdfsCurrFolderFileListAdd(UINT32 folder,UINT8* dirPath)
{
	UINT32 cntItem = 0;
	UINT32 fnLen = 64;
	char *fname = NULL;
	UINT32 attr;
	UINT64 filesize;	

	cntItem = sp5kDirFileCntGet(dirPath);

	if (cntItem > 0)
	{
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: %s has %d child", dirPath, cntItem);
	
		appCdfsCheckLastNmlFolder(folder, dirPath, cntItem);	
		
		if (sp5kFsDirChange(dirPath) == 0)
		{
			UINT32 cdfs_idx;
			fname = (char*)sp5kMalloc(fnLen);
			cdfs_idx = 0;
			
			//sp5kFsDirCacheSet(dirPath, 1);/*speed up*/
			while (cntItem && fname && IS_CARD_EXIST)
			{
				UINT32 fnoLen;
				UINT32 ret;
				const UINT8 *flgname;/*file long name*/
				cdfs_idx++;
				fnoLen = fnLen;
				ret = sp5kFsDirNextFileGet((char *)fname, &fnLen, &attr, &filesize, NULL);
				if (ret != 0)
				{
					if (fnLen > fnoLen){	/* filename buffer is not large enough */
						if (fname) sp5kFree(fname);
						fname = (char*)sp5kMalloc(fnLen);
						if (fname){
							ret = sp5kFsNextFileNameGet((UINT8*)fname, &fnLen);
							if (ret != 0){
								break;
							}
						}
					}
					else{
						break;
					}
				}
				flgname = sp5kFsAliasNameGet((UINT8*)fname,  SP5K_FS_STR_NATIVE);
				if(!flgname)
				{
					/*Isaac add this one to fixed if file name is to short , sp5kFsAliasNameGet will return the "(NULL)"*/
					flgname = (UINT8*)fname;
				}				
				AddFile((UINT8*)flgname, attr, filesize, dirPath);
				cntItem--;//paul modify 2018 0331
				#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
				appWatchDogRefresh();
				#endif
			}
			//sp5kFsDirCacheSet(dirPath, 0);/*speed up*/
			if (fname)	sp5kFree(fname);
			if (!IS_CARD_EXIST)
				return FAIL;
		} else {
			HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: chang folder fail %s has %d child", dirPath, cntItem);
			if (sp5kFsDirChange(dirPath) == 0)
			{
				appCdfsFileFolderSet(folder);
			}
			HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: chang folder fail.%s",dirPath);
		}
		
		HOST_PROF_LOG_ADD(LEVEL_INFO, "cdfs: Search finish");
	}
	else
	{
		/* Empty folder delete */
		printf("%s(%d) delete empty folder %s\n", __func__, __LINE__, dirPath);
		if (!IS_CARD_EXIST)
			return FAIL;
		sp5kFsDirRemove(dirPath);
		//HOST_PROF_LOG_PRINT(LEVEL_INFO, "cdfs: has no child in.%s",dirPath);
		//appCdfsFileFolderSet(folder);
		//appCdfsFolderInit(folder);/*add by 20121112*/
	}
	return SUCCESS;
}

/*--------------------------------------------------------------------------*
 * Function name	: appCdfsFolderCount
 * Function	       	: Get folder count
 * Return value   	: folder count(UINT32)
 * Parameter1    	: folder(UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/        
UINT32 appCdfsFolderCount(UINT32 folder)
{
	return UNIFY_FOLDER.folder_info.file_num_in_subfolder[folder];
}

/*--------------------------------------------------------------------------*
 * Function name	: appCdfsFolderNodeCountRemove
 * Function	       	: Remove the folder node count
 * Return value   	: void
 * Parameter1    	: current CDFS file table pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/      
void appCdfsFolderNodeCountRemove(CDFS_FILE_TAB_T *pCur)
{
	UINT32 folder = jet_folder_bit_2_cdfs_folder(pCur->folder);

	UNIFY_FOLDER.folder_info.file_num_in_subfolder[folder]--;
	*CDFS_FOLDER_SIZE_TABLE[folder] -= pCur->FileSize;
}

/*--------------------------------------------------------------------------*
 * Function name	: appCdfsFolderNodeCountAdd
 * Function	       	: Add the folder node count
 * Return value   	: void
 * Parameter1    	: current CDFS file table pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/       
void appCdfsFolderNodeCountAdd(CDFS_FILE_TAB_T *pCur)
{
	UINT32 folder = jet_folder_bit_2_cdfs_folder(pCur->folder);

	UNIFY_FOLDER.folder_info.file_num_in_subfolder[folder]++;
	*CDFS_FOLDER_SIZE_TABLE[folder] += pCur->FileSize;
}

/*--------------------------------------------------------------------------*
 * Function name	: appCdfsRemoveNode
 * Function	       	: Remove the folder node 
 * Return value   	: void
 * Parameter1    	: current CDFS file table pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/        
void appCdfsRemoveNode(CDFS_FILE_TAB_T *pCur)
{
    CDFS_FILE_TAB_T *pHead = UNIFY_FOLDER.head_node;
    CDFS_FILE_TAB_T *pNext;
    CDFS_FILE_TAB_T *pPre;

#if DEBUG_ON
    debug_file_t(pHead);
#endif
    if (pHead == pCur) {
        if (pCur->next == pHead)
            pHead = NULL;
        else
            pHead = pCur->next;
        UNIFY_FOLDER.head_node = pHead;
	printf("%s(%d) pCur is pHead(%p)\n",
			__func__, __LINE__, UNIFY_FOLDER.head_node);
    }
    appCdfsFolderNodeCountRemove(pCur);

    pNext = pCur->next;
    pPre = pCur->pre;

#if DEBUG_ON
    debug_file_t(pCur);
    debug_file_t(pNext);
    debug_file_t(pPre);
#endif

    pNext->pre = pCur->pre;
    pPre->next = pCur->next;
    sp5kFree(pCur);

#if DEBUG_ON
    debug_file_t(pNext);
    debug_file_t(pPre);
#endif
}

/*--------------------------------------------------------------------------*
 * Function name	: appCdfsNodeUpdateCBSet
 * Function	       	: Set CDFS node update
 * Return value   	: void
 * Parameter1    	: cdfs_node_update pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/         
void appCdfsNodeUpdateCBSet(cdfs_node_update cb)
{
	nodeUpdateCb = cb;
}

/*--------------------------------------------------------------------------*
 * Function name	: appCdfsUsedSpace
 * Function	       	: Get CDFS used space
 * Return value   	: size (UINT64)
 * Parameter1    	: folder (UINT32)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/    
UINT64 appCdfsUsedSpace(UINT32 folder)
{
    if (folder >= SIZE(CDFS_FOLDER_SIZE_TABLE))
        return 0;
    return *CDFS_FOLDER_SIZE_TABLE[folder];
}

 /*--------------------------------------------------------------------------*
 * Function name	: JetCdfsJPGSetFilename
 * Function	       	: Set JPG file name
 * Return value   	: void
 * Parameter1    	: set or not set (BOOL true/false)
 * Parameter2   	: file name pointer (--)
 * History	   :
 *                :  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/    
void JetCdfsJPGSetFilename(BOOL set , char * filename){
	CDFS_SET_JPG_FILENAME = set;
	sprintf((char *)CDFS_JPG_FILE_NAME , "%s" ,filename);
};


#endif

#endif

