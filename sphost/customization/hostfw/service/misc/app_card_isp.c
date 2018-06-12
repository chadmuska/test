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
 *  File name	: 	App_card_isp.c
 *  Function 	: 	Process the update FW from SD CARD		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.04.11  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_os_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_dbg_api.h"
/* Isaac add 20161214 for updat fw menu*/
#include "app_osd.h"
#include "app_osd_api.h"
#include "app_stringid.h"
#include "app_rtc.h"
#if ICAT_WIFI
#include "app_ptp.h"
#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "../include/app_watch_dog.h"
#include "app_buzzer.h"
#include "app_infohandlerComm.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define FW_FILE   	"D:\\SPHOST.BRN"

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appPsuedoPowerOff(UINT32 powerDown);

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern UINT32 buzzerbackGroundTimer;
#endif
#if SP5K_FW2_SUPPORT    
extern void appInfodualBootRAWCSuccess(void);
extern  void appInfoUpdateErrSet(ErrTypeUpdate type , ErrCodeUpdate code);
extern  ErrTypeUpdate appInfoUpdateTypeGet(UINT8 * filename);
#endif

/*paul 2018.04.16 add for modify compiler warning*/
extern void  appOsdMessageBoxShow(BOOL bShow, unsigned short usStrId);
extern UINT8* copyFile2Mem( char* filePath );

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

  /*--------------------------------------------------------------------------*
 * Function name	: appGetSum16
 * Function	       	: summary the UINT16 buf data
 * Return value   	: summary value (UINT16)
 * Parameter1    	: buffer pointer (--)
 * Parameter2    	: buffer size (UINT32)
 * History	   :
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT16 appGetSum16( UINT16 *Buf, UINT32 Size )
{
	UINT16 Sum = 0;
	while( Size ) 
	{
		Sum += Buf[--Size];
	}
	return Sum;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appGetSum32
 * Function	       	: summary the UINT32 buf data
 * Return value   	: summary value (UINT32)
 * Parameter1    	: buffer pointer (--)
 * Parameter2    	: buffer size (UINT32)
 * History	   :
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

UINT32 appGetSum32( UINT32 *Buf, UINT32 Size )
{
	UINT32 Sum = 0;
	while( Size ) 
	{
		Sum += Buf[--Size];
	}
	return Sum;
}
  
  /*--------------------------------------------------------------------------*
 * Function name	: appByteSum
 * Function	       	: summary the byte buf data
 * Return value   	: summary value (UINT32)
 * Parameter1    	: buffer pointer (--)
 * Parameter2    	: buffer length (UINT32)
 * History	   :
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
UINT32 appByteSum(UINT8 *buf, UINT32 len)
{
	UINT32 i,sum;
	for (i=sum=0 ; i<len ; i++) 
	{
		sum += buf[i];
	}
	return sum;
}

  /*--------------------------------------------------------------------------*
 * Function name	: SuccessThenDelFW
 * Function	       	: Delete the FW file when the FW update sucessed  
 * Return value   	: void
 * Parameter1    	: Elapse file count (UINT32)
 * Parameter2    	: Total file count (UINT32)
 * History	   :
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
SuccessThenDelFW(
    UINT32 elapseFileCnt,
    UINT32 totalFileCnt
)
{
    printf("\n\nelapseFileCnt=%u, totalFileCnt=%u.\n", elapseFileCnt, totalFileCnt);

    if (elapseFileCnt >= totalFileCnt )
	{
	    if(appFileExistCheck((UINT8*)"D:\\SPHOST.BRN"))
	    {			
			sp5kFsFileDelete((UINT8*)FW_FILE);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
			appRTC_t local;
			UINT32 err;
			err = sp5kHostMsgSend(INFO_QUEUEMSG_BUZZERONOFF, BUZZER_CTRL_NONE);
			HOST_ASSERT(err == SP5K_SUCCESS);
			appLedGRInTurnDisable();
			sp5kTimeDelay( SP5K_TIME_DELAY_1MS, 1000 );
			LED_NORMAL_ON;
			err = sp5kHostMsgSend(INFO_QUEUEMSG_BUZZERONOFF, BUZZER_CTRL_UPDATEFINISH);
			HOST_ASSERT(err == SP5K_SUCCESS);
			sp5kTimeDelay( SP5K_TIME_DELAY_1MS, 250 );
			ALL_LED_OFF;
			sp5kTimeDelay( SP5K_TIME_DELAY_1MS, 100 );	
			LED_NORMAL_ON;
			sp5kTimeDelay( SP5K_TIME_DELAY_1MS, 250 );
			ALL_LED_OFF;
			sp5kTimeDelay( SP5K_TIME_DELAY_1MS, 1400 );	
			LED_NORMAL_ON;
			sp5kTimeDelay( SP5K_TIME_DELAY_1MS, 1000 );
			appRtcGet(&local);
			local.sec = local.sec+2;
#endif
	    }
		return;
    }
    return; //SHEN: for different control by product, so don't do anything below
}


  /*--------------------------------------------------------------------------*
 * Function name	: appFirmwareUpdate
 * Function	       	: firmware update process  
 * Return value   	: result (UINT32 SUCCESS/FAIL)
 * Parameter1    	: file name pointer (--)
 * Parameter2    	: buzzer (BOOL)
 * History	   :
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
UINT32
appFirmwareUpdate(
	UINT8 * filename,
	BOOL buzzer
)
#else
UINT32
appFirmwareUpdate(
	void
)
#endif
{

	UINT32 fd, ofs, l;
	UINT8 buf[64], *fw=0;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	UINT32 state = 0,err;
	ErrTypeUpdate errType = ERRTYPE_UPDATE_NOERR;
	ErrCodeUpdate errCode = ERRCODE_UPDATE_FAIL;

#endif

	fd = sp5kFsFileOpen((UINT8*)FW_FILE, SP5K_FS_OPEN_RDONLY);
	if( fd ) {
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	appWatchDogModify(0);
#endif
		sp5kFsFileRead(fd, buf, sizeof(buf));
		ofs = *(UINT32*)(buf+0x1c);
		l = sp5kFsFileSizeGet(fd);
		if (ofs>=l) {
			sp5kFsFileClose(fd);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		errCode = ERRCODE_UPDATE_FILESIZE;
#endif
			goto SDCard_Download_Fail;
		}
		l -= ofs;
		fw = sp5kMalloc(l);
		sp5kFsFileSeek(fd, ofs, SP5K_FS_SEEK_SET);
		sp5kFsFileRead(fd, fw, l);
		sp5kFsFileClose(fd);
        
		printf("start FW update...\n");
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		if (buzzer) {
			appLedGRInTurnEnable( 1000  , 20000);

			err = sp5kHostMsgSend(INFO_QUEUEMSG_BUZZERONOFF, BUZZER_CTRL_UPDATE);
			HOST_ASSERT(err == SP5K_SUCCESS);
		}
		appInfoUpdateErrSet( errType , errCode);/*real update state*/
		state = sp5kDscFirmwareUpdate(filename, SuccessThenDelFW);
		errCode = state&0xFF;
		
		if(errCode == 0xFF)
			errCode = ERRCODE_UPDATE_FAIL;
		if(errCode == 0xFE)
			errCode = ERRCODE_UPDATE_SUCCESS;
			/*errType = ERRCODE_UPDATE_NOCANGE;*/
        
		printf("%s   ErrType = 0x%x   ErrCode = 0x%x\n" , __FUNCTION__ ,errType,errCode);
		if (errCode==ERRCODE_UPDATE_SUCCESS)
		{
			errType = ERRTYPE_UPDATE_NOERR;
		
			printf("FW update OK!\n");

		}else{
			errType = appInfoUpdateTypeGet(filename);
			if(errType == ERRTYPE_MAX){
				errType =  ERRTYPE_UPDATE_FW1|ERRTYPE_UPDATE_FW2|ERRTYPE_UPDATE_RES;
				errCode = ERRCODE_UPDATE_FILEREAD;
			}
		}	

		appInfoUpdateErrSet( errType , errCode);/*real update state*/

		if(errCode){
			
			goto SDCard_Download_Fail;
		}
		else{
			sp5kPowerCtrl(SP5K_POWER_OFF, 0);
	   		while(1);
			return SUCCESS;
		}
	}

#else
		//if (sp5kDscFirmwareUpdate( brnFile )==SUCCESS){
		//if (sp5kDscFirmwareUpdate( FW_FILE,0 )==SUCCESS)
		if (sp5kDscFirmwareUpdate( (UINT8*)FW_FILE, SuccessThenDelFW)==SUCCESS)
		{
			printf("FW update OK!\n");
			return SUCCESS;
		}

	}
#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	else{
			errType =  ERRTYPE_UPDATE_FW1|ERRTYPE_UPDATE_FW2|ERRTYPE_UPDATE_RES;
			errCode = ERRCODE_UPDATE_FILEOPEN;
			appInfoUpdateErrSet( errType , errCode);
	}
#endif
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
ALL_LED_OFF;
LED_NORMAL_ON;
#endif

SDCard_Download_Fail:

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	/*Isaac add for update fw ui 20161214*/
	appBtnEnable(BTN_ALL);
	appOsdMessageBoxShow(FALSE,STRTK_BUSY_PLS_WAIT);
#endif
	printf("Update err\n");
	if (fw)
        sp5kFree(fw);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if (buzzer) {
		LED_NORMAL_ON;
		sp5kHostMsgSend(INFO_QUEUEMSG_BUZZERONOFF, BUZZER_CTRL_POWERON);
	}
#endif
	return FAIL;
}


