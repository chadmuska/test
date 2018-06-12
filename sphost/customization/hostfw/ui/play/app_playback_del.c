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

#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_global_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_dcf_api.h"
#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_playback_osd.h"
#include "app_dpof.h"
#include "app_cdfs_api.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

//#define DEL_KEY_REPEAT_INTERVAL			200 /* ms */

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	DEL_SXN_START,
	DEL_SXN_END,
	DEL_SXN_DO_ALL_START,
	DEL_SXN_DO_ALL_END,
} protUIKeyCtrl_t;

typedef enum {
	PB_DEL_RIGHT,
	PB_DEL_LEFT,
	//PB_DEL_UP,
	//PB_DEL_DOWN,
} pbProtShiftDir_t;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/*
static UINT32 gprotKeyIntvl = DEL_KEY_REPEAT_INTERVAL;
static UINT32 gprotKeyRptCnt = 0;
*/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
static void appPbImgDelStExit(UINT32 nextSt,	UINT32 stMsg);

#define DEL_ConfirmDlgShow()	DEL_ConfirmDlgShowEx(TRUE)
#define DEL_ConfirmDlgClear()	DEL_ConfirmDlgShowEx(FALSE)

/*-------------------------------------------------------------------------
 *  File Name : DEL_ConfirmDlgConfig
 *------------------------------------------------------------------------*/
static
void
DEL_ConfirmDlgConfig(
	void
)
{
	UINT32 strId;

	DBG_PRINT("pbdel: %s \n", __FUNCTION__);

	strId = (gpbUiParam.selImgDelete==PB_UI_DELETE_SINGLE) ?
				STRTK_DELETE_THIS_FILE : STRTK_DEL_ALL_FILES;
	appPlayOsd_DlgConfirmYesNoConfig(strId, 1);
}

/*-------------------------------------------------------------------------
 *  File Name : DEL_ConfirmDlgShowEx
 *------------------------------------------------------------------------*/
static
void
DEL_ConfirmDlgShowEx(
	BOOL bShow
)
{
	DBG_PRINT("pbdel: %s \n", __FUNCTION__);
	appPlayOsd_DlgBoxShow();
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_3);
	appOsdLib_PIPLayerIconDraw(PB_STARTING_OKICON_X, PB_STARTING_OKICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, 0x0001);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(PB_STARTING_SELECTSTR_X,PB_STARTING_SELECTSTR_Y,SP5K_GFX_ALIGN_TOP_LEFT,STRTK_SEL);
//	appPbOsd_AuxNote_OKSelect(bShow);
	appPlayOsd_FileNumDraw(bShow, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	appPlayOsd_ModeIconDraw(bShow);

	if (bShow)
	{
		appPlayOsd_ProtectDraw(gpbImgCb.isProtected);
	}
	else
	{
		appPlayOsd_ProtectDraw(FALSE);
	}
}
/*-------------------------------------------------------------------------
 *  File Name : delUIKeyControl
 *------------------------------------------------------------------------*/
static
void
delUIKeyControl(
	UINT8 ctrl
)
{
	DBG_PRINT("pbdel: KeySxN(%d) !!\n", ctrl);

	switch (ctrl){
	case DEL_SXN_START:
	case DEL_SXN_DO_ALL_START:
		appBtnDisable(BTN_ALL);
		break;
	case DEL_SXN_END:
	case DEL_SXN_DO_ALL_END:
		appBtnEnable(BTN_ALL);
		break;
	default:
    	PB_DBG(LEVEL_WARN,"pbdel: %s: undefined key session", __FUNCTION__);        
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : delValidityCheckup
 *------------------------------------------------------------------------*/
static
UINT32
delValidityCheckup(
	void
)
{
	DBG_PRINT("pbdel: %s \n", __FUNCTION__);

	/*-- My opinion, the following shall be checked
	 * before entering PB_DEL state !!
	 */
	if (gpbDiskCb.isDiskErr) {
		gpbMainCb.err = PB_ERR_CARD_ERROR;
		return FAIL;
	} else if (gpbDiskCb.isCardLock) {
		gpbMainCb.err = PB_ERR_CARD_LOCKED;
		return FAIL;
	}
	#if 0
	else if (gpbMainCb.isNoImage) {
		gpbMainCb.err = PB_ERR_NO_IMAGE;
		return FAIL;
	}
	#endif
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : delAllAsyncPostProc
 *------------------------------------------------------------------------*/
static
void
delAllAsyncPostProc(
	BOOL isSuccess
)
{
	DBG_PRINT("pbdel: (all) Async PostProc (%d) \n", isSuccess);

	if (gpbMainCb.subStat != PB_DEL_STAT_ALL_BUSY) {
		/* Not Aysnc DEL_ALL operation, ignore */
		DBG_PRINT("pbdel: (all) not Async OP !!\n");
		return;
	}
	gtotalImgCount = appPbTotalFileCountGet();
	if (isSuccess) {
		DBG_PRINT("pbdel: (all) FileOp OK !!\n");              
		gcurrDcfIdx = gtotalImgCount;
		if (gtotalImgCount>0) {
			/* some files are protected !! */
			gpbMainCb.err = PB_ERR_IMG_PROTECTED;
			gpbMainCb.isNoImage = FALSE;
		} else {
			/* all files deleted */
			gpbMainCb.err = PB_ERR_NO_IMAGE;
			gpbMainCb.isNoImage = TRUE;
		}
	} else {
    	PB_DBG(LEVEL_WARN,"pbdel: %s: FileOp NG", __FUNCTION__);          
		gpbMainCb.err = PB_ERR_DEL_ERROR;
	}

	gpbMainCb.subStat = PB_DEL_STAT_STANDBY;
	/*-- Enable Key Control */
	delUIKeyControl(DEL_SXN_DO_ALL_END);

}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelShift
 *------------------------------------------------------------------------*/
static
void
appPbImgDelShift(
	UINT8 shiftCtrl
)
{
	UINT32 ret=SUCCESS;

	DBG_PRINT("pbdel: SHIFT(%d) \n", shiftCtrl);


	if (gpbMainCb.subStat!=PB_DEL_STAT_CONFIRM_MENU) {
		DBG_PRINT("pbdel: (%s) busy !!\n", (shiftCtrl ? "Left" : "Right"));
		return;
	}
	if (gtotalImgCount==1) {
		DBG_PRINT("pbdel: (%s) only 1 IMG, ignore !!\n", (shiftCtrl ? "Left" : "Right"));
		return;
	}

	switch (shiftCtrl) {
	case PB_SHIFT_NEXT:
	case PB_SHIFT_PRE:
		if (FAIL==appPbImageShift_PreCheck()) {
			PB_DBG(LEVEL_WARN,"pbdel: Shift_PreChk failed \n");            
			break;
		}
		//PB_BEEP;
		//ret = appPbImageShift(shiftCtrl);
		PB_BEEP;
		if (FAIL==appPbImageShift(shiftCtrl)) {
			/*-- For PROTECT, the FILE_ERROR case just show BLACK BG
			 */
			//appPbErrorProcess(PB_ERR_FILE_ERROR);
			PB_BLACK_BKGND;
		}
		/*-- Emulate Key Repeat */
		appPbImageShiftRepeatCheck(0);
		break;

	default:
		DBG_PRINT("pbdel: undefined shiftCtrl(%d) \n", shiftCtrl);
		return;
	}

	/*-- Update FileNum & Protect ICON  */
	appPlayOsd_FileNumDraw(TRUE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
	appPlayOsd_ProtectDraw(gpbImgCb.isProtected);

	appPbFilePageCtrlParaSync();

	if (ret==FAIL)
	{
		appPbErrorProcess(gpbMainCb.err);
		DEL_ConfirmDlgShow();
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelSingle
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgDelSingle(
	void
)
{
	UINT32 ret;

	/*-- Validity check for Del Single
	 * is File Protected ?
	 */
	if (gpbImgCb.isProtected)
	{
        PB_DBG(LEVEL_INFO,"pbdel: %s: (Single) IMG Protected ", __FUNCTION__);         
		gpbMainCb.err = PB_ERR_IMG_PROTECTED;
		appPbErrorProcess(gpbMainCb.err);
		return FAIL;
	}

	/*-- Abort current IMG Playback !?
	 * DO NOT NEED to abort, because the request will be ignored
	 * at OnKey service.
	 */

	/*-- Go on */
	DBG_PRINT("pbdel: Deleting image (%s) !! \n\n", gpbImgCb.szDcfFileName);
		/*-- Del DPOF as well */
	if((gpbImgCb.fileType==PB_FT_JPG) && (gpbImgCb.isDpofSet ==TRUE))
	{

		appDpofJobDelete(APP_DPOF_MODE_STD, APP_DPOF_JOB_CURR);

		gpbImgCb.isDpofSet=FALSE;
		gpbImgCb.dpofPrintNum = 0;
		gpbImgCb.isDpofStamp = FALSE;
		appDpofFileWrite();

	}
	//dpofDebugDump(1/*isQryDpof*/);
    #if SP5K_CDFS_OPT
    ret = appCdfsFileOperate(SP5K_DCF_FILEOP_DEL, 0, PB_OP_SYNC, 0, 0);
    #else          
	ret = sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_DEL, 0, PB_OP_SYNC, 0, 0);
    #endif

	if (ret == FAIL)
	{
        PB_DBG(LEVEL_ERROR,"pbdel: %s: (Single) Failed", __FUNCTION__);                 
		gpbMainCb.err = PB_ERR_DEL_ERROR;
		return FAIL;
	}
    
	/*-- Re-calculate DCF index */
	if (gtotalImgCount==1)
	{
		gtotalImgCount = 0;
		gcurrDcfIdx = 0;
		gpbMainCb.err = PB_ERR_NO_IMAGE;
		gpbMainCb.isNoImage = TRUE;
		return SUCCESS;
	}
	#if 1
	sp5kDcfFsInfoGet(SP5K_DCF_INFO_TOTAL_FILENUM, &gtotalImgCount);
	#else
	gtotalImgCount--;
	#endif
	if (gcurrDcfIdx>=gtotalImgCount)
	{
		gcurrDcfIdx=gtotalImgCount;
	}

	sp5kDcfFsCurFileIdxSet(gcurrDcfIdx);

	appPbFilePageCtrlParaSync();

	/*-- Playback current image */
	if (FAIL==appPbImagePlay(gcurrDcfIdx))
	{
		PB_DBG(LEVEL_ERROR,"pbdel: %s: Play failed", __FUNCTION__);
        gpbMainCb.err = PB_ERR_NO_IMAGE;
        #if 0
		gpbMainCb.err = PB_ERR_FILE_ERROR;
		appPbErrorProcess(gpbMainCb.err);
        #endif
	}

	return SUCCESS;
}


/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelAll
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgDelAll(
	void
)
{
	DBG_PRINT("pbdel: (ALL-OneByOne) !!\n");

	delUIKeyControl(DEL_SXN_DO_ALL_START);

	sp5kHostMsgSend(APP_UI_MSG_PB_DELALL_1BY1, gtotalImgCount);
	gpbMainCb.subStat = PB_DEL_STAT_ALL_BUSY;

	DBG_PRINT("pbdel: ========================================\n");
	DBG_PRINT("pbdel: (All-1by1) total(%d) go on !!\n", gtotalImgCount);
	DBG_PRINT("pbdel: ========================================\n");
	PB_BLACK_BKGND;
	appPbMsg_BusyWait();

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelExecute
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgDelExecute(
	void
)
{
	UINT32 ret;

	ret = FAIL;
	switch (gpbUiParam.selImgDelete)
	{
		default:
			break;
		case PB_UI_DELETE_SINGLE:
			delUIKeyControl(DEL_SXN_START);
			ret = appPbImgDelSingle();
			delUIKeyControl(DEL_SXN_END);
			break;
		case PB_UI_DELETE_ALL:
			ret = appPbImgDelAll();
			break;
	}

	if (FAIL==ret)
	{
		return FAIL;
	}
	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelCreate
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgDelCreate(
	void
)
{
	DBG_PRINT("pbdel: %s \n", __FUNCTION__);

	/*-- Validity check :
	 */
	if (FAIL==delValidityCheckup())
	{
		appPbErrorProcess(gpbMainCb.err);
		return FAIL;
	}

	/*-- Configure Confirm Dialog Box
	 */
	DEL_ConfirmDlgConfig();
	DEL_ConfirmDlgShow();
	gpbMainCb.subStat = PB_DEL_STAT_CONFIRM_MENU;

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelStClose
 *------------------------------------------------------------------------*/
static
void
appPbImgDelStClose(
	void
)
{
	DBG_PRINT("pbdel: State closing \n");

	if(appPbYUVPIPStatusGet())
	{
		AppPbDestroyshowJPGInPIP();
	}

	gpbMainCb.lastImgIdx = gcurrDcfIdx;
	gpbMainCb.subStat = PB_DEL_STAT_NULL;
	gpbMainCb.mainStat = PB_MAIN_STAT_IMG_DEL_SUSPEND;
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelStExit
 *------------------------------------------------------------------------*/
static
void
appPbImgDelStExit(
	UINT32 nextSt,
	UINT32 stMsg
)
{
	DBG_PRINT("pbdel: State exiting \n");
	appPbImgDelStClose();
	appStateChange(nextSt, stMsg);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelStInit
 *------------------------------------------------------------------------*/
static
UINT32
appPbImgDelStInit(
	void
)
{
	DBG_PRINT("pbdel: State initing \n");
	if(gpbMainCb.isDelHotKey)
	{
		gpbMainCb.mainStat = PB_MAIN_STAT_IMG_DEL_SUSPEND;
	}
	gpbMainCb.subStat = PB_DEL_STAT_STANDBY;
	sp5kHostMsgSend(APP_UI_MSG_PB_DEL_CREATE, 0);
	return SUCCESS;
}



/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelStOnMsg
 *------------------------------------------------------------------------*/
void
appPbImgDelStOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 tmpU32;

	DBG_PRINT("pbdel: _OnMsg: 0x%08x, 0x%08x ... \n", msg, param);

	switch (msg)
	{
	case APP_UI_MSG_PB_DELALL_1BY1:
		DBG_PRINT("pbdel: APP_UI_MSG_PB_DELALL_1BY1(0x%08x)\n", param);
		sp5kDcfFsCurFileIdxSet(param);
		gpbImgCb.dcfIdx = param;
		playImgDcfAttrGet(&gpbImgCb);
		//printf("isdpofset=%d,isprotect=%d\n",gpbImgCb.isDpofSet,gpbImgCb.isProtected);
		if((gpbImgCb.fileType==PB_FT_JPG) &&(!gpbImgCb.isProtected))
		{
			#if SP5K_CDFS_OPT
			#else
			appDpofJobDelete(APP_DPOF_MODE_STD, APP_DPOF_JOB_CURR);
			#endif
		}
		//dpofDebugDump(1/*isQryDpof*/);

        #if SP5K_CDFS_OPT
        if(!gpbImgCb.isProtected)
            appCdfsFileOperate(SP5K_DCF_FILEOP_DEL, 0, PB_OP_SYNC, 0, 0);
        #else          
		    sp5kDcfFsFileOperate(SP5K_DCF_FILEOP_DEL, 0, PB_OP_SYNC, 0, 0);
        #endif

		if (param==1)
		{
            #if !SP5K_CDFS_OPT
			appDpofFileWrite();
            #endif
            
			sp5kHostMsgSend(APP_UI_MSG_PB_DELALL_DONE, 0);
			break;
		}

		PB_INDEX_SUB(param, 1, param);
		sp5kHostMsgSend(APP_UI_MSG_PB_DELALL_1BY1, param);
		break;

	case APP_UI_MSG_PB_DELALL_DONE:
		delUIKeyControl(DEL_SXN_DO_ALL_END);
		PB_OSD_CLR;
		gtotalImgCount = appPbTotalFileCountGet();
		gcurrDcfIdx = gtotalImgCount;

		appPbFilePageCtrlParaSync();

		if (gtotalImgCount>0)
		{
			appPbImagePlay(gcurrDcfIdx);
			gpbMainCb.err = PB_ERR_IMG_PROTECTED;
			appPbErrorProcess(gpbMainCb.err);
			DEL_ConfirmDlgShow();
			gpbMainCb.subStat=PB_DEL_STAT_CONFIRM_MENU;
		}
		else
		{
			appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;

	case SP5K_MSG_STILLPLAY_DONE:
		DBG_PRINT("pbdel: SP5K_MSG_STILLPLAY_DONE(0x%08x)\n", param);
		/*-- NOTE :
		 *   Common Error Process has been done in
		 *   appGroupPlaybackProc() @ app_state_preproc.c
		 */
		tmpU32 = appPbStillPlayDoneParse(param);
		if (tmpU32 & PB_DONE_ERROR)
		{
			PB_DBG(LEVEL_ERROR,"pbdel: %s: PB_DONE error(0x%x)", __FUNCTION__, tmpU32);
            
			PB_BLACK_BKGND;
				/*-- In PROTEC case, FILE_ERROR wont show center message!!
				 */
			break;
		}

		if ( (IS_PB_QV_PLAYING && tmpU32==PB_DONE_QV_FINISH)
			 ||(IS_PB_THUMB_PLAYING && tmpU32==PB_DONE_THUMB_FINISH) )
		{
			DBG_PRINT("pbdel: THUMB/QV Done !!\n");
			gpbImgCb.isThumbImgOK = TRUE;
			gpbMainCb.isPlayDone = TRUE;
			appPbJpgMainPlayStart();
		}
		else if (IS_PB_MAIN_PLAYING && tmpU32==PB_DONE_MAIN_FINISH)
		{
			DBG_PRINT("pbdel: MAIN Done !!\n");
			gpbMainCb.playStat = PB_PLAY_STAT_STANDBY;
			gpbMainCb.isPlayDone = TRUE;
			gpbImgCb.isMainImgOK = TRUE;
		}
		break;


	case SP5K_MSG_DCF_FILEOP_COMPLETE:
		DBG_PRINT("pbdel: (all) FILEOP OK \n");
		delAllAsyncPostProc(TRUE);
		if (gpbMainCb.err != PB_ERR_NO_IMAGE)
		{
			/* Delete ALL has encountered ERROR !! */
			appPbErrorProcess(gpbMainCb.err);
		}
		appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case SP5K_MSG_DCF_FILEOP_FAIL:
		PB_DBG(LEVEL_ERROR,"pbdel:%s (all) FILEOP NG",__FUNCTION__);
		delAllAsyncPostProc(FALSE);
		appPbErrorProcess(gpbMainCb.err);
		appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		break;
	case APP_UI_MSG_PB_SHIFT_NEXT:
		DBG_PRINT("pbdel: APP_UI_MSG_PB_SHIFT_NEXT \n");
		appPbImgDelShift(PB_DEL_RIGHT);
		break;
	case APP_UI_MSG_PB_SHIFT_PRE:
		DBG_PRINT("pbdel: APP_UI_MSG_PB_SHIFT_PRE \n");
		appPbImgDelShift(PB_DEL_LEFT);
		break;
	case APP_UI_MSG_PB_DEL_CREATE:
		if (FAIL==appPbImgDelCreate())
		{
            PB_DBG(LEVEL_ERROR,"pbdel:%s Creation failed",__FUNCTION__);
			appPbImgDelStExit(APP_STATE_MENU, STATE_PARAM_NORMAL_INIT);
			return;
		}
		break;

	default :
		break;
	}
}


/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelStOnKey
 *------------------------------------------------------------------------*/
void
appPbImgDelStOnKey(
	UINT32 msg,
	UINT32 param
)
{
	DBG_PRINT("pbdel: _OnKey : [0x%x] [0x%x]\n", msg,param);

	switch (msg) {
	case APP_KEY_PRESS_UP:
	case APP_KEY_PRESS_DOWN:
		if (gpbMainCb.subStat==PB_DEL_STAT_CONFIRM_MENU)
		{
			PB_BEEP;
			appPlayOsd_DlgBoxScroll(((msg==APP_KEY_PRESS_UP) ? TRUE : FALSE));
		}
		break;
	case APP_KEY_PRESS_SET:
		DBG_PRINT("pbdel: APP_KEY_PRESS_SET \n");
		if (gpbMainCb.subStat != PB_DEL_STAT_CONFIRM_MENU)
		{
			DBG_PRINT("pbdel: Ignore SET Key, Wrong DEL state !! \n");
			break;
		}

		PB_BEEP;
		if (0==appPlayOsd_DlgBoxSelectGet())
		{
			/*-- YES */
			if (FAIL==appPbImgDelExecute())
			{
				PB_DBG(LEVEL_ERROR,"pbdel: %s: DEL excute failed !! \n", __FUNCTION__);                
			}
			else
			{
				/*-- Execute SUCCESS !! */
				/*-- Doing DelAll 1by1,
				 * DO NOTHING while Execute returns
				 */
                #if !SP5K_PVQV_DISABLE 				 
				appViewOsd_PvQVInfoFree();
                #endif
                
				if (gpbUiParam.selImgDelete==PB_UI_DELETE_ALL)
				{
                    
					break;
				}

				if (PB_ERR_NO_IMAGE==gpbMainCb.err )
				{
					PB_OSD_CLR;
					appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
					break;
				}
			}

			/*-- UI */
			if (gpbUiParam.selImgDelete==PB_UI_DELETE_SINGLE)
			{
				APP_OSD_REFRESH_OFF;
				/*-- UI : Prepare DLG box */
				appPlayOsd_DlgBoxSelectSet(1); /* change default */
				DEL_ConfirmDlgShow();
				/*-- UI : Update FileNum & Protect ICON */
				appPlayOsd_FileNumDraw(TRUE, gpbMainCb.curImgIdx, gpbMainCb.totalImg);
				appPlayOsd_ProtectDraw(gpbImgCb.isProtected);
				APP_OSD_REFRESH_ON;
			}

		}
		else
		{
			/*-- NO */
			PB_OSD_CLR;
            appPbImgDelStExit(APP_STATE_PB_MAIN, STATE_PARAM_NORMAL_INIT);
		}
		break;
	case APP_KEY_PRESS_LEFT:
		if (gpbUiParam.selImgDelete==PB_UI_DELETE_SINGLE)
		{
			DBG_PRINT("pbdel: APP_KEY_PRESS_LEFT \n");
			delUIKeyControl(DEL_SXN_START);
			appPbImgDelShift(PB_SHIFT_PRE);
			delUIKeyControl(DEL_SXN_END);
		}
		break;
	case APP_KEY_PRESS_RIGHT:
		if (gpbUiParam.selImgDelete==PB_UI_DELETE_SINGLE)
		{
			DBG_PRINT("pbdel: APP_KEY_PRESS_RIGHT \n");
			delUIKeyControl(DEL_SXN_START);
			appPbImgDelShift(PB_SHIFT_NEXT);
			delUIKeyControl(DEL_SXN_END);
		}
		break;
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbImgDelState
 *------------------------------------------------------------------------*/
void
appPbImgDelState(
	UINT32 msg,
	UINT32 param
)
{
	DBG_PRINT("pbdel: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:
		RPrintf("appPbImgDelState");
		appPbImgDelStInit();
		/* do nothing */
		break;
	case APP_STATE_MSG_CLOSE:
		appPbImgDelStClose();
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appPbImgDelStOnKey(msg,param);
		}
		else
		{
			appPbImgDelStOnMsg(msg,param);
		}
		break;
	}

}
