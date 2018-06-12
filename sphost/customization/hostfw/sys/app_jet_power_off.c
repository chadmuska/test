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
 *  File name	: 	App_jet_power_off.c
 *  Function 	: 	power off process 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:2017.08.21  	             "First edited"
 *  			:2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#include "sp5k_global_api.h"
#include "ndk_global_api.h"
#include "app_com_def.h"
#include "app_sensor.h"
#include "app_disk.h"
#include "sp5k_aud_api.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_infohandlerComm.h"

extern PowerDetect appInfoPowerDETStateGet(void);
extern MoveFileState appInfoGetMoveFileState(void);
extern void appinfoMoveFileState(void);
extern void appInfoFinishFileNameset(UINT8 * buf);
extern void appinfoMoveFileStateInit(void);
extern BOOL fileReady;
extern void appInfoLastPositionSave(void);
extern void jet_lineout_out(BOOL on);
#define READ8(a) (*((volatile UINT8*)(a)))
#define WRITE8(_reg_, _value_)  (*((volatile UINT8  *)(_reg_)) = (_value_))
#endif


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

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
extern UINT8 appGsensorSettingGet(void);
extern void appGsensorLevelCtrl(int mode, int level);
extern void appUart2Stop(BOOL en);
extern void appModeSet(UINT32 mode);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/


  /*--------------------------------------------------------------------------*
 * Function name	: appJetPowerOffInit
 * Function	       	: Jet poweroff init sequence.
 * Return value   	: void
 * Parameter1    	: void
 * History	   :  2017.08.21  	                  "First edited"
 *                :  2018.04.12  Paul.Chang     "Add description"
 *                :  2018.04.16  Paul.Chang     "Remove unused variable"
 *--------------------------------------------------------------------------*/
 
void appJetPowerOffInit(void)
{
	ALL_LED_OFF;
	appUart2Stop(TRUE);
	/* Entry stanby to black TV out*/
	appModeSet(SP5K_MODE_STANDBY);

	/* unmount SD card */
if(IS_CARD_EXIST){
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#if 0
	jet_lastfile_rename_by_rtc_changed();
	
	t = appCdfsLastFileNameGet();
	if (t != NULL && strlen(t) != 0) {
	       //delete the last file
	       appInfoFileNameEventAdd(appCdfsLastFileNameGet());	
		if(appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP){
			printf("%s    %s\n" ,__FUNCTION__, appCdfsLastFileNameGet());
			sp5kFsFileDelete(appCdfsLastFileNameGet());
		}
	}

	//if event trigger , move last file
	if(appInfoGetMoveFileState() != MOVEFILE_NONE)
	{
		printf("%s    %s\n" ,__FUNCTION__, appCdfsLastFileNameGet());
		appInfoFinishFileNameset(appCdfsLastFileNameGet());
		fileReady = TRUE;
		appinfoMoveFileState();
	}
			/* FIXME: otignal code */
	if(appInfoUdatHeaderPEGet() & 0x0008){//GEVENT
		jet_cdfs_oldest_file_delete_by_folder(CDFS_FILE_FOLDER_EVENT);
	}
#endif
#endif
	printf("%s(%d) check file !!!!!\n", __func__, __LINE__);

		sp5kDiskUnMount(SP5K_DRIVE_REMOVE | SP5K_DRIVE_SD);
	}
	jet_lineout_out(FALSE);
	sp5kAudDevVrefSet(5,4); 	

	printf("%s @@6454,64b8=%x,%x\n",__FUNCTION__ ,READ8(0xb0006454),READ8(0xb00064B8)); 
	
	/* Disable G sensor */
	appGsensorLevelCtrl(0, 0);
}


  /*--------------------------------------------------------------------------*
 * Function name	: appJetPowerOffClose
 * Function	       	: Jet poweroff close sequence.
 * Return value   	: void
 * Parameter1    	: void
 * History	   :  2017.08.21  	                  "First edited"
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
void appJetPowerOffClose(void)
{
    /* Check UI setting for G sensor, if needs to enable */
    if (appInfoPowerDETStateGet() == POWER_DET_ACC_ON) {
        appUart2Stop(FALSE);
    }

}


  /*--------------------------------------------------------------------------*
 * Function name	: appJetPowerOffState
 * Function	       	: Jet poweroff sequence state change.
 * Return value   	: void
 * Parameter1    	: state messages (UINT32)
 * Parameter2    	: messages  parameter (UINT32)
 * History	   :  2017.08.21  	                  "First edited"
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
void
appJetPowerOffState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
		case APP_STATE_MSG_INIT:
			appJetPowerOffInit();
		break;
		case APP_STATE_MSG_CLOSE:
			appJetPowerOffClose();
		    appStateCloseDone();
		    break;
		default:
			break;
	}
}

