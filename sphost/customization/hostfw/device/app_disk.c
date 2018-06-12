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
 *	File name    : App_disk.c
 *	Function     : About disk implementation.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.07  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"

#include "sp5k_disk_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_awb_api.h"
#include "app_dev_plug.h"
#include "app_dcf.h"
#include "app_cdfs_api.h"
#include "app_stringid.h"
#include "app_errorosd.h"
#include "customization/dev_init.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define DISK_HEALTH_ERROR 0xE0 // Error or not support health query
#define SYNCHRONOUS       0
#define ASYNCHRONOUS      1
#define CLASS_SPEED_C4    4
#define CLASS_SPEED_C6    6

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
    APP_CARD_MOUNT_OK   = 0x00,
    APP_CARD_MOUNT_FAIL = 0x01,
} appCardMountStatus_e;

typedef struct appDiskStatus_s {
    appDiskInfo_t diskInfo;
    BOOL isMountReady;
    BOOL isMountFailed;
    BOOL isNandMounted;
    BOOL isCardMounted;
} appDiskStatus_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

UINT32 cardTotalSize;    /* in Mega Byte */
static appDiskStatus_t appDiskStatus = { {SP5K_DRIVE_NAND,0,0,0}, FALSE, FALSE, FALSE, FALSE};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

extern UINT8 fileTypeSelect;
extern UINT32 devDiskSdIdGet(UINT32 sel);

#if defined(PRJ_JETDVR) || defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_infohandlerComm.h"
#include "jet_utility.h"
extern void appInfoRegularFB(PanaState state);
extern void JetPowerCanOff(int st);
extern PanaState appInfoPanaStateGet(void);
extern void appInfoFWUpgarde(void);

/*paul 2018.04.16 add for modify compiler warning*/
extern void appInfoSDCardSlowSet(void);
extern PowerDetect appInfoPowerDETStateGet(void);
extern BOOL appInfoDiskSizeCheck(void);
extern void appInfoConnStart(BOOL);
extern void appInfoLastPositionGet(void);
extern void appVideoStop(void);
extern void appInfoDiskErrorFB(DiskStatus state);

#else
UINT32 eventListReady;
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appSdPowerRecycle
* Function         : Sd power recycle
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appSdPowerRecycle(void)
{
    /* MUST measure your PCB exactly delay time from LDO_OFF to LDO_ON. */
    /* Don't let SD IO have power but SD not power on, would cause unexpected problem. */
	
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "disk: %s: s", __FUNCTION__);


    // set CMD,CLK,DAT0~3 to GPIO & output low
    sp5kGpioWrite(SP5K_SD_CLKCMDDAT_GRP, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID, 0);
    sp5kGpioCfgSet(SP5K_SD_CLKCMDDAT_GRP, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID);
    SET_SD_CARD_POWER(DISABLE);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)/*Isaac add at 20180517, if we get the 19DVR PCB with ETV this define(PRJ_19DVR) need remove*/
    SET_SD_CARD_POWER_N(DISABLE);
#endif

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
    APP_SLEEP_MS(450);
    if (appActiveStateGet() != APP_STATE_CARD_PLUG) {
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "appSdPowerRecycle, DISK_ERROR");

        //appReportErrorStatusFB(1,2);  // read slow issues  --> $DMSG046,1,3*2D
        // <return state> 1: Success
        // <Error Status> 2: File Read Error
  	// return;
  	appInfoSDCardSlowSet();
    }
#else
    sp5kOsThreadSleep(100);
#endif
    // set CMD,CLK,DAT0~3 to SD function
    sp5kGpioCfgSet(SP5K_SD_CLKCMDDAT_GRP, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID, INPUT);
    sp5kGpioFuncSet(SP5K_SD_CLKCMDDAT_GRP, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID, 0);
    SET_SD_CARD_POWER(ENABLE);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)/*Isaac add at 20180517, if we get the 19DVR PCB with ETV this define(PRJ_19DVR) need remove*/
    SET_SD_CARD_POWER_N(ENABLE);

#endif
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "disk: %s: e", __FUNCTION__);
}

/*--------------------------------------------------------------------------*
* Function name    : _diskMountStart
* Function         : Disk mount implementation.
* Return value	   : void
* Parameter1	   : driver (sp5kDriveId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
_diskMountStart(
    UINT32 driver
)
{
    profLogPrintf(0, "mount start(%d)",driver);
    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "disk: Disk Mount(%d)", driver);

    if(IS_CARD_EXIST)
    {
        sp5kDiskCfgSet(SP5K_DISK_PWR_RECYCLE_CALLBACK_CFG,SP5K_DRIVE_SD,appSdPowerRecycle);

        sp5kGpioCfgSet(SP5K_SD_CLKCMDDAT_GRP, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID, INPUT);
        sp5kGpioFuncSet(SP5K_SD_CLKCMDDAT_GRP, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID, 0);
        SET_SD_CARD_POWER(ENABLE);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)/*Isaac add at 20180517, if we get the 19DVR PCB with ETV this define(PRJ_19DVR) need remove*/
    	 SET_SD_CARD_POWER_N(ENABLE);
#endif
		
    }
	
    sp5kDiskMount(driver); 

    appDiskStatus.diskInfo.actDsk = driver;
    appDiskStatus.isMountReady= FALSE;
    appDiskStatus.isMountFailed= FALSE;

#if SP5K_ULTRA_FAST_BOOT
    if(driver==SP5K_DRIVE_SD)
    {
        profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:LogCTRL ON s");
        sp5kLogControl(SP5K_LOG_ENABLE);
        profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:LogCTRL ON e");
    }
#endif

}

/*--------------------------------------------------------------------------*
* Function name    : appSdInfoGet
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

static appDiskCapability_t diskCap;

appDiskCapability_t* appSdInfoGet(void)
{
	return &diskCap;
}

/*--------------------------------------------------------------------------*
* Function name    : _diskMountCompleted
* Function         : Disk mount ok and fail handle.
* Return value	   : void
* Parameter1	   : param (UINT32 APP_CARD_MOUNT_OK/APP_CARD_MOUNT_FAIL)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
_diskMountCompleted(
    UINT32 param
)
{
    DBG_PRINT("disk: Mount Ready(%d) %d\n",param,appDiskStatus.diskInfo.actDsk);
    appDiskStatus.isMountReady= TRUE;
    appDiskStatus.isMountFailed= param;

    if (appDiskStatus.diskInfo.actDsk==SP5K_DRIVE_NAND)
    {
        appDiskStatus.isNandMounted = !param;
    }
    else if (appDiskStatus.diskInfo.actDsk==SP5K_DRIVE_SD)
    {
        appDiskStatus.isCardMounted = !param;
		UINT32 info = devDiskSdIdGet(0);
		diskCap.sdType = info & 0x00ff;
		diskCap.sdMode = info>>8 & 0x00ff;

		UINT32 nsect, prot;
		sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_TOTAL_SECTOR, &nsect);
		sp5kDiskCfgGet(SP5K_DISK_WRITE_PROTECT_CFG, SP5K_DRIVE_SD, &prot);
		if (diskCap.sdType == SD_TYPE_UNKNOWN) {
			printf("SD card is unknown!\n");
			appDiskStatus.isCardMounted = 0;
		}
		else if ((diskCap.sdType == SD_TYPE_V11 && nsect >= 0x400000)
			|| (diskCap.sdType == SD_TYPE_SDHC && nsect >= 0x4000000)) {
			printf("SD card is unavailable due to type%d and sect num=%u!\n", diskCap.sdType, nsect);
			appDiskStatus.isCardMounted = 0;
		}
		if (nsect > 0x3FD8000 && nsect < 0x4000000) {
			printf("SD card is unavailable due to sect num=%u!\n", nsect);
			appDiskStatus.isCardMounted = 0;
		}
		if (prot) {
			printf("SD card is ready only\n");
		}
    }
#if !SP5K_USB_UVC_SUPPORT
    #if USB_DISK_SUPPORT
    else if (appDiskStatus.diskInfo.actDsk==SP5K_DRIVE_USB)
    {
        appDiskStatus.isNandMounted = !param;
        appDiskStatus.isCardMounted = !param;
    }
    #endif
#endif
}

/*--------------------------------------------------------------------------*
* Function name    : _diskUnMountStart
* Function         : Disk unmount implementation.
* Return value	   : void
* Parameter1	   : driver (sp5kDriveId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
_diskUnMountStart(
    UINT32 driver
)
{
    if(!IS_CARD_EXIST)
    {
        /* Don't let SD IO have power but SD not power on, would cause unexpected problem. */

        // set CMD,CLK,DAT0~3 to GPIO & output low
        sp5kGpioWrite(SP5K_SD_CLKCMDDAT_GRP, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID, 0);
        sp5kGpioCfgSet(SP5K_SD_CLKCMDDAT_GRP, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID);
        SET_SD_CARD_POWER(DISABLE);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)/*Isaac add at 20180517, if we get the 19DVR PCB with ETV this define(PRJ_19DVR) need remove*/
        SET_SD_CARD_POWER_N(DISABLE);
#endif
    }

	if (driver == SP5K_DRIVE_NAND)
		appDiskStatus.isNandMounted = FALSE;
	else if (driver == SP5K_DRIVE_SD)
        appDiskStatus.isCardMounted = FALSE;
    sp5kDiskUnMount(SP5K_DRIVE_REMOVE | driver);
}

/*--------------------------------------------------------------------------*
* Function name    : _diskInfoGet
* Function         : The implementation which get action disk information by sp5kDiskInfoGet.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
_diskInfoGet(
    void
)
{
    UINT32 val,drive,ret;
    //disk info get
    drive = appDiskStatus.diskInfo.actDsk;
    ret = sp5kDiskInfoGet(drive, SP5K_SIZE_BY_MEGA_BYTE, &val);
    if (ret != SUCCESS) {
        printf("sp5kDiskInfoGet(0x%x, SP5K_SIZE_BY_MEGA_BYTE, %u)\n", drive, val);
    }else{
        appDiskStatus.diskInfo.dskMbSz = val;
        DBG_PRINT("disk: Drive %d Total Size [%d] MB\n",drive,val);
    }
    ret = sp5kDiskInfoGet(drive, SP5K_FREE_SIZE_BY_KBYTE, &val);
    if (ret != SUCCESS) {
        printf("sp5kDiskInfoGet(0x%x, SP5K_FREE_SIZE_BY_KBYTE, %u)\n", drive, val);
    }else{
        appDiskStatus.diskInfo.freeSz = val;
        DBG_PRINT("disk: Drive %d Free  Size [%d] KByte\n",drive,val);
    }
    ret = sp5kDiskInfoGet(drive, SP5K_CLUSTER_SIZE_BY_BYTES, &val);
    if (ret != SUCCESS) {
        printf("sp5kDiskInfoGet(0x%x, SP5K_CLUSTER_SIZE_BY_BYTES, %u)\n", drive, val);
        sp5kHostMsgSend(APP_UI_MSG_VIEW_WAIT_DISK, 0);
    }else{
        appDiskStatus.diskInfo.cluSz= val;
        DBG_PRINT("disk: Drive %d Cluster Size [%d] KByte\n",drive, appDiskStatus.diskInfo.cluSz);
    }
}

/*--------------------------------------------------------------------------*
* Function name    : _diskInit
* Function         : Disk init setting.
* Return value	   : void
* Parameter1	   : driver (sp5kDriveId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
_diskInit(
    UINT32 drive
)
{
    appDiskStatus.diskInfo.actDsk = drive;

    if (SUCCESS != appDcfInit(drive))
    {
        appDiskStatus.isMountFailed = TRUE;
        if (appDiskStatus.diskInfo.actDsk==SP5K_DRIVE_NAND)
        {
            appDiskStatus.isNandMounted = FALSE;
        }
        else if (appDiskStatus.diskInfo.actDsk==SP5K_DRIVE_SD)
        {
            appDiskStatus.isCardMounted = FALSE;
        }

        HOST_PROF_LOG_ADD(LEVEL_ERROR, "disk: dcf init fail");
        return FAIL;
    }
    _diskInfoGet();

    return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appCdfsInit
* Function         : About cdfs feature init.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appCdfsInit(void)
{	
#if SP5K_CDFS_OPT
    UINT8 i;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
  /*  const UINT8 normal_scan_path[] = {
	CDFS_FILE_FOLDER_EVENT,
	CDFS_FILE_FOLDER_VIDEO,
	CDFS_FILE_FOLDER_USERRECORD,
	CDFS_FILE_FOLDER_USERSAVE,
	CDFS_FILE_FOLDER_JPG,
	0xFF,
    };
    const UINT8 parking_scan_path[] = {
	    CDFS_FILE_FOLDER_EVENT,
	    CDFS_FILE_FOLDER_VIDEO,
	    0xFF,
    };
    const UINT8 *scan_path = normal_scan_path;*/
#endif

    appBtnDisable(BTN_ALL);

    #if SP5K_DISK_SPI
#if JET_PANA_CDFS
	appCdfsNodeUpdateCBSet(jet_file_udata_update);

//	if (appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP)
//		scan_path = parking_scan_path;
        appCdfsFileListTabInit(CDFS_FILE_FOLDER_MAX);
#endif
        if(IS_CARD_EXIST)
        {

        	UINT8 path[20]={0};
#if defined(JET_PANA_CDFS)
		UINT8 j;
		/*paul modify bug #3440 Short-term solution 
		[ trun off AE & turn on AE when search files]*/
	   	sp5kAeModeSet(SP5K_AE_MODE_OFF);
		sp5kAwbModeSet(SP5K_AWB_MODE_OFF);
		printf("\n=====AE/AEB Off\n");
		
		//weiaki modify for cppcheck 180607
		const UINT8 parking_scan_path[] = {
		CDFS_FILE_FOLDER_EVENT,
		CDFS_FILE_FOLDER_VIDEO,
		0xFF,
		};
		
		const UINT8 normal_scan_path[] = {
		CDFS_FILE_FOLDER_EVENT,
		CDFS_FILE_FOLDER_VIDEO,
		CDFS_FILE_FOLDER_USERRECORD,
		CDFS_FILE_FOLDER_USERSAVE,
		CDFS_FILE_FOLDER_JPG,
		0xFF,
		};
		
		const UINT8 *scan_path = normal_scan_path;
		if (appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP)
			scan_path = parking_scan_path;
            for(i = 0; (j = scan_path[i]) != 0xFF; i++)
            {
		printf("%s - %hhu - %hhu\n", __func__, i, j);
                if (appCdfsFolderInit(j) == FAIL)
			goto err;
                appCdfsFolder2Path(j,path);
                //sp5kFsDirCacheSet(path, 1);/*speed up*/
                if (appCdfsCurrFolderFileListCreate(j, path) == FAIL)
			goto err;
                //sp5kFsDirCacheSet(path, 0);

                //if(j==CDFS_FILE_FOLDER_EVENT)
                    //eventListReady = 1;
            }
		sp5kAeModeSet(SP5K_AE_MODE_INFO_ONLY);
		sp5kAwbModeSet(SP5K_AWB_MODE_AUTO_SET);
		printf("=====AE/AEB ON\n");	
	    return;
err:
            appDiskStatus.isMountFailed= FALSE;
            return;

        }
#else
            for(i=CDFS_FILE_FOLDER_VIDEO; i<CDFS_FILE_FOLDER_MAX; i++)
            {
                appCdfsFolderInit(i);
                appCdfsFolder2Path(i,path);
                sp5kFsDirCacheSet(path, 1);/*speed up*/ /*1 to cache directory data*/
                appCdfsCurrFolderFileListCreate(i, path);/*create cdfs file list*/
                sp5kFsDirCacheSet(path, 0);/*0 to remove the cache*/

                if(i==CDFS_FILE_FOLDER_EVENT)
                    eventListReady = 1;
            }
        }
        else
        {
            for(i=CDFS_FILE_FOLDER_VIDEO; i<CDFS_FILE_FOLDER_MAX; i++)
                appCdfsFileListTabInit(i);
        }
#endif
    #else
		UINT8 path[20]={0};
        for(i=CDFS_FILE_FOLDER_VIDEO; i<CDFS_FILE_FOLDER_MAX; i++)
        {
            appCdfsFolderInit(i);
            appCdfsFolder2Path(i,path);
            sp5kFsDirCacheSet(path, 1);/*speed up*/ /*1 to cache directory data*/
            appCdfsCurrFolderFileListCreate(i, path);/*create cdfs file list*/
            sp5kFsDirCacheSet(path, 0);/*0 to remove the cache*/
        }
    #endif
#if !JET_PANA_CDFS
    if(appActiveStateGet()==APP_STATE_STILL_PREVIEW)
    {
        appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
        appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
    }
    else if(appActiveStateGet()==APP_STATE_PB_MAIN){
        if (pUiSetting->pbfileType == PB_FILETYPE_IMAGE){
            appCdfsFileFolderSet(CDFS_FILE_FOLDER_JPG);
            appCdfsFolderInit(CDFS_FILE_FOLDER_JPG);
        }
        else{
            appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
               appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
        }
    }
    else
    {
        appCdfsFileFolderSet(CDFS_FILE_FOLDER_VIDEO);
        appCdfsFolderInit(CDFS_FILE_FOLDER_VIDEO);
    }
#endif
    /*appCdfsRtcSet();*/
    appBtnEnable(BTN_ALL);

#endif
}

/*
 * Jason: Fix redmine_2724 by introducing a new variable: <bError_DiskSize>.
 * Reset it to FALSE whenever SD Card is inserted.
 *
 */

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern DiskStatus appInfoDiskStatus;
static BOOL bError_DiskSize = FALSE;
#endif

/**
 * @param msg, param
 * @return void
 * @author
 * @data 09/25/2017
 *
 */
 /*--------------------------------------------------------------------------*
* Function name    : appDiskMountMsgHandler
* Function         : Disk mount message handle
* Return value	   : void
* Parameter1	   : message (UINT32)
* Parameter2	   : parameter (param)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/
void
appDiskMountMsgHandler(
    UINT32 msg,
    UINT32 param
)
{
	#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 err;
	UINT32 state = INFO_QUEUEMSG_NONE;
	#endif

    switch( msg){
    case SP5K_MSG_DISK_MOUNT_START:
        //do nothing
        break;
    case SP5K_MSG_DISK_MOUNT_COMPLETE:
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "disk: SP5K_MSG_DISK_MOUNT_COMPLETE(%d)", param);

        if(IS_CARD_EXIST)
        {
            if(param == SP5K_DRIVE_NAND)
                break;
            if(param == SP5K_DRIVE_SD)
            {
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "disk: setting anti fargment(%d)", param);
                sp5kDiskCfgSet(SP5K_FS_ANTI_FRAGMENT_CFG, SP5K_DRIVE_SD, 0x80000);
				
				#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
                bError_DiskSize = FALSE;
                if (appInfoDiskSizeCheck() == FALSE) {
                    HOST_PROF_LOG_PRINT(LEVEL_WARN, "%s:%d: <bError_DiskSize> is set TRUE.", __FILE__, __LINE__);
                    bError_DiskSize = TRUE;
                }
				#endif

            }
        }
        else
        {
            if(param == SP5K_DRIVE_SD)
                break;

        }
        _diskMountCompleted(APP_CARD_MOUNT_OK);
        _diskInit(param);

        #if SP5K_CDFS_OPT
			#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			if (IS_CARD_EXIST) {
				appInfoFWUpgarde();
			        appCdfsInit();
				if (param == SP5K_DRIVE_SD)
					appInfoConnStart(FALSE);
				appInfoLastPositionGet();
				state = INFO_QUEUEMSG_DISKSTATE;
				err = sp5kHostMsgSend(state, 0);
				HOST_ASSERT(err == SP5K_SUCCESS);
			}
			#else
	        appCdfsInit();
			#endif
        #endif

        #if FILE_RECOVER    /*for file recover flow*/
        sp5kDcfRecoverFileList_t  *pList;
        sp5kDcfRecoverListGet(&pList);
        if(pList){
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "disk: recover plist->path:%s\n",pList->pathName);
            sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_RECOVER_FILE, (UINT32)pList, 0, 0, 0);
        }
        #endif

        if(appPreviousStateGet() == APP_STATE_POWER_ON)
        {
            if(!((appActiveStateGet() == APP_STATE_MENU)&&(fileTypeSelect == 1)))/*ez_menu for pb filetype */
                appOsdMessageBoxShow(FALSE,STRTK_WAITING_FOR_CARD);
        }

		if (param == SP5K_DRIVE_SD)
        {
            UINT32 ret = 0, sd_health = 0;

            ret = sp5kDiskInfoGet(SP5K_DRIVE_SD, SP5K_DISK_HEALTH_QUERY, &sd_health);
            if (ret == 0) {
                /* Success, Toshiba spec Max Health = 0xFF */
                printf("SD card Health = %u%%\n", sd_health);
            }
            else if (sd_health == DISK_HEALTH_ERROR) {
				/* This card did not support health command */
                printf("This SD card not support health query command. \n");
			}
            else {
                /* Fail or read error */
                printf("Fail to get SD Health. Error = %#X\n", sd_health);
            }
        }

		#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		jetSDUseTimeInit();
		jetSDLogFileDelete();
		#endif
        break;
    case SP5K_MSG_DISK_MOUNT_FAIL: /* simulate card remove if mount fail */
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "disk: SP5K_MSG_DISK_MOUNT_FAIL(%d)", param);
        _diskMountCompleted(APP_CARD_MOUNT_FAIL);
	
		#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if (IS_CARD_EXIST && param == SP5K_DRIVE_SD) {
			appInfoConnStart(FALSE);
		}
		#endif

        #if SP5K_CDFS_OPT
			#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			appCdfsFileListTabInit(CDFS_FILE_FOLDER_MAX);
			#else
	        appCdfsFileListTabInit(CDFS_FILE_FOLDER_VIDEO);
	        appCdfsFileListTabInit(CDFS_FILE_FOLDER_JPG);
	        appCdfsFileListTabInit(CDFS_FILE_FOLDER_EVENT);
			#endif
        #endif
		
		#if defined(PRJ_JETDVR) || defined(PRJ_17DVR)||defined(PRJ_19DVR)
		state = INFO_QUEUEMSG_DISKSTATE;
		err = sp5kHostMsgSend(state, 0);
		HOST_ASSERT(err == SP5K_SUCCESS);
		#endif
        //show card error
        break;
    case SP5K_MSG_DISK_ERROR:
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "disk: SP5K_MSG_DISK_ERROR(%d)", param);
		
		#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if (IS_CARD_EXIST && param == SP5K_DRIVE_SD) {
			appInfoConnStart(FALSE);
		}
		#endif
        _diskMountCompleted(APP_CARD_MOUNT_FAIL);

		#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		/* DISK error stop recording */
		appVideoStop();
		extern UINT8 dropFrameRecFlag;
		if (dropFrameRecFlag) {
			dropFrameRecFlag = 0;
		}
		appInfoDiskErrorFB(DISK_ERROR);
		#endif
		
        #if SP5K_CDFS_OPT
			#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			appCdfsFileListTabInit(CDFS_FILE_FOLDER_MAX);
			#else
	        appCdfsFileListTabInit(CDFS_FILE_FOLDER_VIDEO);
	        appCdfsFileListTabInit(CDFS_FILE_FOLDER_JPG);
	        appCdfsFileListTabInit(CDFS_FILE_FOLDER_EVENT);
			#endif
        #endif
		
		#if defined(PRJ_JETDVR) || defined(PRJ_17DVR)||defined(PRJ_19DVR)
		state = INFO_QUEUEMSG_DISKSTATE;
		err = sp5kHostMsgSend(state, 0);
		HOST_ASSERT(err == SP5K_SUCCESS);
		#endif
        //show card error
        break;
    default:
        //do nothing
        break;
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appActiveDiskSet
* Function         : Set active disk and do init setting.
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : driver (sp5kDriveId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appActiveDiskSet(
    UINT32 dskId
)
{
    DBG_PRINT("disk: appActiveDiskSet %d \n",dskId);
    switch (dskId)
    {
        case SP5K_DRIVE_NAND:
            if (appDiskStatus.isNandMounted==FALSE)
            {
                DBG_PRINT("disk: nand mount fail\n ");
                return FAIL;
            }
            break;
        case SP5K_DRIVE_SD:
            if (appDiskStatus.isCardMounted==FALSE)
            {
                DBG_PRINT("disk: sd mount fail\n ");
                return FAIL;
            }
            break;
#if !SP5K_USB_UVC_SUPPORT
        #if USB_DISK_SUPPORT
        case SP5K_DRIVE_USB:
            _diskInit(dskId);
            appDiskStatus.isMountReady = TRUE;
            appDiskStatus.isMountFailed= FALSE;
            return SUCCESS;
            break;
        #endif
#endif
        default:
            DBG_PRINT("disk: %s: Unknown drive(%d) !!\n", __FUNCTION__, dskId);
            return FAIL;
    }

    appDiskStatus.diskInfo.actDsk = dskId;
    if (FAIL==_diskInit(dskId))
    {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "disk: %s(%d)", __FUNCTION__,__LINE__);
        return FAIL;
    }

    return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appActiveDiskGet
* Function         : Get active disk.
* Return value	   : active disk (UINT32)
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appActiveDiskGet(
    void
)
{
    return (appDiskStatus.diskInfo.actDsk);
}

/*--------------------------------------------------------------------------*
* Function name    : appDiskMountQuery
* Function         : Return is disk mount for nandm, sd, usb.
* Return value	   : result (BOOL TRUE/FALSE)
* Parameter1	   : driver (sp5kDriveId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

BOOL
appDiskMountQuery(
    UINT32 dskId
)
{
#if !SP5K_USB_UVC_SUPPORT
    #if USB_DISK_SUPPORT
    UINT32 ret;
    #endif
#endif
    if (dskId==SP5K_DRIVE_NAND)
    {
        return (!!appDiskStatus.isNandMounted);
    }
    else if (dskId==SP5K_DRIVE_SD)
    {
        return (!!appDiskStatus.isCardMounted);
    }
#if !SP5K_USB_UVC_SUPPORT
    #if USB_DISK_SUPPORT
    else if (dskId==SP5K_DRIVE_USB)
    {
        sp5kDiskInfoGet(SP5K_DRIVE_USB, SP5K_DISK_STATE, &ret);
        if(ret == 0)    return TRUE;
        else    return FALSE;
    }
    #endif
#endif
    else
    {
        return FALSE;
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appDiskMountReady
* Function         : Return is disk mount ready.
* Return value	   : appDiskStatus.isMountReady (UINT32)
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appDiskMountReady(
    void
)
{
    return (appDiskStatus.isMountReady);
}

/*--------------------------------------------------------------------------*
* Function name    : appDiskMountFailed
* Function         : Return is disk mount failed.
* Return value	   : appDiskStatus.isMountFailed (UINT32)
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appDiskMountFailed(
    void
)
{
    return (appDiskStatus.isMountFailed);
}

/*--------------------------------------------------------------------------*
* Function name    : appDiskSDMountReady
* Function         : Return is sd mount ready.
* Return value	   : appDiskStatus.isCardMounted (UINT32)
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

/*Isaac add for pass SD card Mountd*/
UINT32
appDiskSDMountReady(
    void
)
{
    return (appDiskStatus.isCardMounted);
}

/*--------------------------------------------------------------------------*
* Function name    : appCurDiskInfoGet
* Function         : Get current disk information
* Return value	   : appDiskStatus.diskInfo (appDiskInfo_t)
* Parameter1	   : void
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

appDiskInfo_t*
appCurDiskInfoGet(
    void
)
{
    _diskInfoGet();
    return (&appDiskStatus.diskInfo);
}

/*--------------------------------------------------------------------------*
* Function name    : appDiskMount
* Function         : Disk mount
* Return value	   : void
* Parameter1	   : driver (sp5kDriveId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appDiskMount(
    UINT32 driver
)
{
    _diskMountStart(driver);
}

/*--------------------------------------------------------------------------*
* Function name    : appDiskUnMount
* Function         : Disk unmount
* Return value	   : void
* Parameter1	   : driver (sp5kDriveId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appDiskUnMount(
    UINT32 driver
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 err;
	UINT32 state = INFO_QUEUEMSG_NONE;
#endif
    _diskUnMountStart(driver);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if (appActiveStateGet() == APP_STATE_JET_POWER_OFF) {
		printf("psudo power off: do nothing\n");
		return;
	}
   
	state = INFO_QUEUEMSG_DISKSTATE;
	err = sp5kHostMsgSend(state, 0);
	HOST_ASSERT(err == SP5K_SUCCESS);
   #endif
}

/*--------------------------------------------------------------------------*
* Function name    : appDiskFormat
* Function         : Format disk
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : driver (sp5kDriveId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appDiskFormat(
    UINT32 driver
)
{
    UINT32 ret;

    if(driver != appDiskStatus.diskInfo.actDsk)
    {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "disk: format disk is not active disk %s(%d)", __FUNCTION__,__LINE__);
        return FAIL;
    }

    #if SP5K_CDFS_OPT
    ret = appCdfsFileOperate(SP5K_DCF_FILEOP_FORMAT,(UINT32)DISK_NAME,DISABLE,NULL,DISABLE);
    #else
    ret = sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_FORMAT,(UINT32)DISK_NAME,DISABLE,NULL,DISABLE);
    #endif
    if (ret != SUCCESS)
    {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "disk: %s(%d)", __FUNCTION__,__LINE__);
        return ret;
    }
    ret = sp5kDcfFsInit(driver,DISABLE,SYNCHRONOUS);
    if (ret != SUCCESS)
    {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "disk: %s(%d)", __FUNCTION__,__LINE__);
        return ret;
    }
    ret = sp5kDcfFsActive(driver);
    if (ret != SUCCESS)
    {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "disk: %s(%d)", __FUNCTION__,__LINE__);
        return ret;
    }
    return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appBytes2ClusterCalculate
* Function         : Calculate disk cluster size.
                     Because unit bytes is too big, 
                     so use this function to conversion cluster size to use.
* Return value	   : bytes (UINT32 input disk bytes size)
* Parameter1	   : clusNum (UINT32 output clusNum)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appBytes2ClusterCalculate(
    UINT32 bytes
)
{
    UINT32 clusNum, cluSz;

    DBG_PRINT("disk: %s: bytes(%d) \n", __FUNCTION__, bytes);

    cluSz = appDiskStatus.diskInfo.cluSz; /* in Bytes */

    clusNum = (bytes+(cluSz-1))/cluSz;

    DBG_PRINT("disk: bytes(%d), clusterSize(%d) [Bytes] ==> clusters(%d) \n", bytes, cluSz, clusNum);

    return clusNum;
}

/*--------------------------------------------------------------------------*
* Function name    : appDiskFreeClusterGet
* Function         : Get disk free cluster
* Return value	   : remaining clusters (UINT32 SUCCESS:remaining, FAIL:0)
* Parameter1	   : actDsk (sp5kDriveId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appDiskFreeClusterGet(
    UINT32 actDsk
)
{
    UINT32 remaining, cluSz;
    UINT32 totalSize=0;
    DBG_PRINT("disk: %s: actDsk(%d) \n", __FUNCTION__, actDsk);

    /*-- disk remaining calculation */
    sp5kDiskInfoGet(actDsk, SP5K_FREE_SIZE_BY_KBYTE, &remaining);
    if (remaining < APP_DISK_DPOF_RSVD_SIZE_KB)
    {
        DBG_PRINT("disk: %s: remaining(%d) less than DPOF_RSVD(%d) !!\n", __FUNCTION__, remaining, APP_DISK_DPOF_RSVD_SIZE_KB);
        return 0;
    }
    else
    {
        remaining -= APP_DISK_DPOF_RSVD_SIZE_KB;
    }
    /*-- transform to clusters */
    sp5kDiskInfoGet(actDsk, SP5K_CLUSTER_SIZE_BY_BYTES, &cluSz);

    sp5kDiskInfoGet(actDsk, SP5K_SIZE_BY_MEGA_BYTE, &totalSize);
    if(totalSize>0x000003ff)//large capacity >1GB
    {
        cluSz >>= 10; /* in KBytes */
        remaining = ((remaining+cluSz-1)/cluSz);
    }
    else//small capacity
    {
        remaining = (remaining <<10);/* in Bytes */
        remaining = ((remaining+cluSz-1)/cluSz);
    }

    DBG_PRINT("disk: drive(%d), remaining clusters(%d) \n", actDsk, remaining);

    return remaining;
}

/*--------------------------------------------------------------------------*
* Function name    : appCardSpeedGet
* Function         : Get card speed to do something, you can expansion it.
* Return value	   : result (UINT32 SUCCESS/FAIL)
* Parameter1	   : actDsk (sp5kDriveId_t)
* History          :
*                  : 2018.06.07  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appCardSpeedGet(UINT32 actDsk)
{
    UINT32 clock;
    UINT32 speed;

    //To query the current clock setting of external card.
    sp5kDiskInfoGet(actDsk, SP5K_DISK_CLOCK_QUERY, &clock);
	//To query the speed class, grade of SDHC/SDXC card. Calling this after SD is mounted. 
	//Value is speed class number (6:C6, 10:C10) or speed grade number x 10 (10:U1, 30:U3).
    sp5kDiskInfoGet(actDsk, SP5K_DISK_CLASS_SPEED_QUERY, &speed);

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "disk: clock [%d] speed [%d]", clock, speed);
    if (( (clock==SP5K_CLOCK_48MHz) && (speed==6) )||( (clock==SP5K_CLOCK_48MHz) && (speed==4) ))
    {
        return SUCCESS;
    }
    else
    {
        return FAIL;
    }
}

