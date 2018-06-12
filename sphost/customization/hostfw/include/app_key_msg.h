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
#ifndef _APP_KEY_MSG_H_
#define _APP_KEY_MSG_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum { 
	APPSTATE_MSG_DEFAULT, /* pass to active state */ 	
	APPSTATE_MSG_IGNORE,   /* don't let active state handle the msg, mode handler eat it */
} appState_msg_flow_e;


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

UINT32 appHostMsgWait(UINT32 waitMsg, UINT32 *pParam, UINT32 timeoutMs);
UINT32 appHostMsgWaitExact(UINT32 waitMsg, UINT32 waitParam, UINT32 timeoutMs);
UINT32 appHostMsgWaitResultFind(UINT32 msg, UINT32 fMatchParam, UINT32 param);

void appHostMsgDump(UINT32 msDuration); /* for test */

void appHostMsgHandlerEnable(UINT32 enable);
UINT32 appHostMsgEnableGet(void);



#endif  /* _APP_KEY_MSG_H_ */


