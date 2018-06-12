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

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/* queue msg during msg wait and dump them back to msg queue later */

#define WAIT_MSG_HISTORY_MAX	128
UINT32 msgQidx,msgQ[WAIT_MSG_HISTORY_MAX];

void 
_MsgWaitHistoryInit(
	void
)
{
	msgQidx = 0;
}

BOOL
_MsgWaitIgnoreMsgCheck(
	UINT32 msg, 
	UINT32 param
)
{
	switch(msg) {
		//sp5k msg
		case SP5K_MSG_POWER_OFF:
		case SP5K_MSG_DISK_INSERT:
		case SP5K_MSG_DISK_REMOVAL:	
		case SP5K_MSG_STILL_STORE_COMPLETE:	
		case SP5K_MSG_STILL_STORAGE_FULL:
		//ui msg
		case APP_UI_MSG_POWER_OFF:
		case APP_UI_MSG_POWER_SAVE_TIME_OUT:
		case APP_UI_MSG_BATT_LEVEL_CHANGE:
		//case APP_UI_MSG_STROBE_CHARGE_READY:
		case APP_UI_MSG_STROBE_CHARGE_ERR:
		case APP_UI_MSG_LENS_ERR:
		case APP_UI_MSG_LENS_CLOSE	:				
			return FALSE;
		default:
			break;
	}	
	return TRUE;
}

void 
_MsgWaitHistoryPush(
	UINT32 msg, 
	UINT32 param
)
{
	if (msgQidx>=WAIT_MSG_HISTORY_MAX) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "msgext: %s : overflow", __FUNCTION__);
		return;
	}
	if(_MsgWaitIgnoreMsgCheck(msg,param)) {
		msgQ[msgQidx++] = msg;
		msgQ[msgQidx++] = param;
	}
	else {
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "Ignore msg : (0x%x,0x%x)", msg,param);
	}
}

void 
_MsgWaitRemainMsgPuch(
	void
)	
{
	UINT32 msg, param;
	sp5kHostMsgSend(APP_UI_MSG_WAIT_MSG_REMAIN_CHECK, 0x00);
	while (1) {
		sp5kHostMsgReceive(&msg, &param);
		if ((msg==APP_UI_MSG_WAIT_MSG_REMAIN_CHECK) && (param==0x00))
			break;
		_MsgWaitHistoryPush(msg,param);
	}
}

void 
_MsgWaitHistoryDumpToHost(
	void
)
{
	UINT32 i;
	for (i=0 ; i<msgQidx ; i+=2) {
		sp5kHostMsgSend(msgQ[i], msgQ[i+1]);
	}
	msgQidx = 0;
}

UINT32
_MsgWaitHistoryFind(
	UINT32 msg, 
	UINT32 fMatchParam, 
	UINT32 param
)
{
	UINT32 i,c;
	for (i=c=0 ; i<msgQidx ; i+=2) {        
		DBG_PRINT("msgext: %x/%x %d %x/%x\n", msg, msgQ[i], fMatchParam, param, msgQ[i+1]);
		if (msgQ[i]==msg && (fMatchParam==0 || msgQ[i+1]==param))
			c++;
	}
	DBG_PRINT("msgext: MsgWaitHistoryFind:%d\n", c);
	return c;
}

/* core MsgWait */
static UINT32
_MsgWait(
	UINT32 waitMsg,
	UINT32 *pWaitParam,
	UINT32 timeoutMs
)
{
	UINT32 timer=TIMER_NULL;
	UINT32 msg, param, ret;

	DBG_PRINT("msgext: MsgWait:%x start\n", waitMsg);
	if (timeoutMs) {
		timer = appTimerSet(timeoutMs, "MsgWait");
	}
	while (1) {
		sp5kHostMsgReceive(&msg, &param);

		if (msg==waitMsg) {
			if (pWaitParam) {
				*pWaitParam = param;
			}
			ret = SUCCESS;
			break;
		}
		if (waitMsg==SP5K_MSGX_KEY && IS_SP5K_KEY_PRESS_MSG(msg, param)) {
			ret = SUCCESS;
			break;
		}
		if (msg==SP5K_MSG_TIMER && (param==timer)) {
			DBG_PRINT("msgext: MsgWait:%x Timeout\n", waitMsg);
			ret = FAIL;
			break;
		}		
		_MsgWaitHistoryPush(msg, param);
	}
	appTimerClear(&timer);	
	DBG_PRINT("msgext: MsgWait:%x ret=%d\n", waitMsg, ret);
	return ret;
}

UINT32 
appHostMsgWait(
	UINT32 waitMsg,
	UINT32 *pParam,
	UINT32 timeoutMs
)
{
	UINT32 ret;
	profLogAdd(0, "msgext: MsgWait:s");
	_MsgWaitHistoryInit();
	ret = _MsgWait(waitMsg, pParam, timeoutMs);
	_MsgWaitRemainMsgPuch();
	_MsgWaitHistoryDumpToHost();
	profLogAdd(0, "msgext: MsgWait:e");
	return ret;
}

UINT32 
appHostMsgWaitExact(
	UINT32 waitMsg,
	UINT32 waitParam,
	UINT32 timeoutMs
)
{
	UINT32 param, ret, t0;
	profLogAdd(0, "msgext: MsgWaitExact:s");
	_MsgWaitHistoryInit();
	t0 = appMsTimeGet();	
	while (1) 
	{
		UINT32 t1,t;
		ret = _MsgWait(waitMsg, &param, timeoutMs); /* todo: multiple wait will have longer timeout */
		if (ret==SUCCESS && param==waitParam)
		{
			break;
		}
		t1 = appMsTimeGet();
		t = MS_DIFF(t0, t1);
		if (t > timeoutMs) 
		{
			ret = FAIL; 
			break;
		} 
		else 
		{
			timeoutMs -= t;
		}
		t0 = t1;
	}
	_MsgWaitHistoryDumpToHost();
	profLogAdd(0, "msgext: MsgWaitExact:e");
	return ret;
}

UINT32 
appHostMsgWaitResultFind(
	UINT32 msg,
	UINT32 fMatchParam,
	UINT32 param
)
{
	return _MsgWaitHistoryFind(msg, fMatchParam, param);
}

