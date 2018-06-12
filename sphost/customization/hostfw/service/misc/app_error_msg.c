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
 *	File name    : App_error_msg.c
 *	Function     : About show error message.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.04  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "app_errorosd.h"
#include "app_sound.h"
#include "app_error_msg.h"
#include "app_playback_osd.h"
#include "app_view_osd.h"
#include "app_video.h"
#include "app_still.h"
#include "sp5k_dbg_api.h"

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
 
BOOL isClrMsg=0;
BOOL isErrorMsgShowing=0;
ushort gErrMsg;
UINT32 timrHdlr = TIMER_NULL;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : _errorMsgDraw
* Function         : Show error message by ERR_MSG switch case.
* Return value	   : void
* Parameter1	   : errMsg, error message your want show (appErrMsgCaseOption_e)
* Parameter2	   : show (UINT32 TRUE/FALSE)
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
_errorMsgDraw(
	appErrMsgCaseOption_e errMsg,
	UINT32 show
)
{
	switch(errMsg)
	{
		case ERR_MSG_CLEAR:
			appOsdMessageBoxShow(FALSE,STRTK_NULL);
			break;
		case ERR_MSG_CARD_LOCKED:
			appOsdMessageBoxShow(show,STRTK_CARD_LOCKED);
			break;
		case ERR_MSG_FORMAT_ERRPR:
			appOsdMessageBoxShow(show,STRTK_FORMAT_FAIL);
			break;
		case ERR_MSG_COMPLETED:
			appOsdMessageBoxShow(show,STRTK_COMPLETED);
			break;
		case ERR_MSG_FILE_ERROR:
			appOsdMessageBoxShow(show,STRTK_F_ERROR);
			break;
		case ERR_MSG_OUT_OF_BATTERY:
			appViewOsd_AllClean();
			appOsdMessageBoxShow(show,STRTK_OUT_OF_BATTERY);
			break;
		case ERR_MSG_CARD_ERROR:
			appOsdMessageBoxShow(show,STRTK_CARD_ERROR);
			break;
		case ERR_MSG_NO_MEMORY_CARD:
			appOsdMessageBoxShow(show,STRTK_NO_CARD);
			break;
		case ERR_MSG_MEMORY_FULL:
			appOsdMessageBoxShow(show,STRTK_MEM_FULL);
			break;
		case ERR_MSG_NO_IMG:
			appOsdMessageBoxShow(show,STRTK_NO_FILE);
			break;
		case ERR_MSG_FILETYPE_ERROR:
			appOsdMessageBoxShow(show,STRTK_NOT_AVAILABLE_THIS_FILE);
			break;
		case ERR_MSG_FILE_LOCKED:
			appOsdMessageBoxShow(show,STRTK_FILE_LOCKED);
			break;
		case ERR_MSG_BUSY_WAIT:
			appOsdMessageBoxShow(show,STRTK_BUSY_PLS_WAIT);
			break;
		case ERR_MSG_IMG_SAVED:
			appOsdMessageBoxShow(show,STRTK_IMG_SAVED);
			break;
		case ERR_MSG_COPY_ERROR:
			appOsdMessageBoxShow(show,STRTK_COPY_ER);
			break;
		case ERR_MSG_CARD_REMAIN_SIZE:
			appOsdMessageBoxShow(show,STRTK_CAPTURE_NOT_EN_SIZE);
			break;
		case ERR_MSG_INSUFFICIENT_DISK_SPACE:
			appOsdMessageBoxShow(show,STRTK_INSUFFICIENT_DISK_SPACE);
			break;
		case ERR_MSG_CAPTURE_DONE:
			appOsdMessageBoxShow(show,STRTK_CAPTURE_DONE);
			break;
		case ERR_MSG_DISK_SPEED_TOO_SLOW:
			appOsdMessageBoxShow(show,STRTK_DISK_SPEED_TOO_SLOW);
			break;
		default:
			break;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appErrMsgIsMessageClear
* Function         : Clear Message by isClrMsg, 
                     isClrMsg=TRUE : Clear Message
                     isClrMsg=FALSE : Show Message Forever
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appErrMsgIsMessageClear(void){

	if( isClrMsg) {
		_errorMsgDraw(gErrMsg,FALSE);
		isClrMsg =0;
		isErrorMsgShowing=FALSE;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appErrMsgTimeISRWithSound
* Function         : Show errMsg flow by ISR, your os task(thread) can continue.
                     You can choice add warning sound case.
* Return value	   : void
* Parameter1	   : errMsg (appErrMsgCaseOption_e)
* Parameter2	   : errSound (appErrMsgSoundOption_e ERR_MSG_ONLY/ERR_MSG_WITH_WARNING_SOUND)
* Parameter3	   : showTimeInMs (UINT32)
                     After show errMsg showTimeInMs ms, will clear.
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appErrMsgTimeISRWithSound(
	appErrMsgCaseOption_e errMsg,
	appErrMsgSoundOption_e errSound,
	UINT32 showTimeInMs
)
{

	if (errMsg==ERR_MSG_CLEAR) {
		_errorMsgDraw(errMsg, FALSE);
		return;
	}

	//draw error msg
	_errorMsgDraw(errMsg,TRUE);
	gErrMsg=errMsg ;
	//play warning sound
	isClrMsg = (showTimeInMs & ERR_MSG_SHOW_FOREVER) ? FALSE : TRUE;
	isErrorMsgShowing=TRUE;
	showTimeInMs &= (~ERR_MSG_SHOW_FOREVER);
	switch(errSound) {
		case ERR_MSG_ONLY:
			timrHdlr=sp5kTimerIsrReg(DISABLE, showTimeInMs,appErrMsgIsMessageClear);
			break;
		case ERR_MSG_WITH_WARNING_SOUND:
			appPreloadSoundPlay(SOUND_ID_WARNING);
			timrHdlr=sp5kTimerIsrReg(DISABLE, showTimeInMs,appErrMsgIsMessageClear);
			break;				
		default:
			HOST_ASSERT(0);
	}

}

/*--------------------------------------------------------------------------*
* Function name    : appErrMsgWithSound
* Function         : Show errMsg flow by os task(thread), your os task will time delay together.
                     You can choice add warning sound case.
* Return value	   : void
* Parameter1	   : errMsg (appErrMsgCaseOption_e)
* Parameter2	   : errSound (appErrMsgSoundOption_e ERR_MSG_ONLY/ERR_MSG_WITH_WARNING_SOUND)
* Parameter3	   : showTimeInMs (UINT32)
                     After show errMsg showTimeInMs ms, will clear.
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appErrMsgWithSound(
	appErrMsgCaseOption_e errMsg,
	appErrMsgSoundOption_e errSound,
	UINT32 showTimeInMs
)
{
	BOOL isClrMsg;

	if (errMsg==ERR_MSG_CLEAR)
	{
		_errorMsgDraw(errMsg, FALSE);
		return;
	}

	//draw error msg
	_errorMsgDraw(errMsg,TRUE);

	//play warning sound
	isClrMsg = (showTimeInMs & ERR_MSG_SHOW_FOREVER) ? FALSE : TRUE;
	showTimeInMs &= (~ERR_MSG_SHOW_FOREVER);
	switch(errSound)
	{
		case ERR_MSG_ONLY:
			appTimeDelayMs(showTimeInMs);
			break;
		case ERR_MSG_WITH_WARNING_SOUND:
			appPreloadSoundPlay(SOUND_ID_WARNING);
			appTimeDelayMs( showTimeInMs);
			break;
		default:
			HOST_ASSERT(0);
	}
	if(showTimeInMs && isClrMsg)
	{
		_errorMsgDraw(errMsg,FALSE);
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appErrMsgShow
* Function         : Show error message by errMsg only.
* Return value	   : void
* Parameter1	   : errMsg, error message your want show (appErrMsgCaseOption_e)
* Parameter2	   : showTimeInMs (UINT32)
                     After show errMsg showTimeInMs ms, will clear.
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appErrMsgShow(
	appErrMsgCaseOption_e errMsg,
	UINT32 showTimeInMs
)
{
	appErrMsgWithSound(errMsg, ERR_MSG_ONLY, showTimeInMs);
}

