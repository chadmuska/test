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
#ifndef _APP_MESSAGE_SERVER_H_
#define _APP_MESSAGE_SERVER_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MAX_MSGSVR_LEN 			1460
#define PORT_MESSAGE_SVR	 	2001
#if 0
#define PORT_MESSAGE_SVR	 	4080
#endif

typedef enum {
	APP_MESSAGE_SVR_START = 0,
	APP_MESSAGE_SVR_STOP ,
	APP_MESSAGE_SVR_SEND ,
    APP_MESSAGE_SESSION_TIMEOUT_CB ,
} APP_MESSAGE_SVR;

#define SOCKET_DEBUG_MESSAGE 0
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef void (*appMsgSvrReadCb_t)(void *buf, unsigned int len);
typedef int (*appMsgSessTimeOutCb_t)(int timeout_counter);

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#ifndef __TODO__
#define __TODO__
#define TODO_STRINGIFY(S) #S
#define TODO_DEFER_STRINGIFY(S) TODO_STRINGIFY(S)
#define PRAGMA_MESSAGE(MSG) _Pragma(TODO_STRINGIFY(message(MSG)))
#define FORMATTED_MESSAGE(MSG) "[TODO-" TODO_DEFER_STRINGIFY(__COUNTER__) "] " MSG " " \
		TODO_DEFER_STRINGIFY(__FILE__) " line " TODO_DEFER_STRINGIFY(__LINE__)

#if 0
#define KEYWORDIFY try {} @catch (...) {}
#else
#define KEYWORDIFY
#endif

#define _TODO_(MSG) KEYWORDIFY PRAGMA_MESSAGE(FORMATTED_MESSAGE(MSG))
#endif

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
SINT32 appSocketMessageOperation(APP_MESSAGE_SVR sel , ...);
void memdump(void *mem, UINT32 len);

#endif  /* _APP_MESSAGE_SERVER_H_ */


