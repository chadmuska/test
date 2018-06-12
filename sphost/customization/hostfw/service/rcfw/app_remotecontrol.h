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

#ifndef __APP_REMOTECONTROL_H__
#define __APP_REMOTECONTROL_H__

#if SP5K_NDK2
#include "jansson.h"
#endif

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
typedef enum RCEventCode_enum {
	RC_EVENTCODE_UNDEFINED		    ,	
	RC_EVENTCODE_CANCEL_TRANSACTION	,
	RC_EVENTCODE_TEST_SOUND         = 0002,	
	RC_EVENTCODE_5KAPI_OP	        ,	
	RC_EVENTCODE_5KMSG_OP	        ,
	RC_EVENTCODE_UART_WRITE         ,
    RC_EVENTCODE_UART_READ          ,
} RCEventCode_e;



typedef enum {
   DEVICE_NONE ,
   DEVICE_NET ,
   DEVICE_BT_GATT,
   DEVICE_BT_GATT_INDICATE,
   DEVICE_BT_DEVTTY
} rcDevicetype_t;


#define RC_EVENT_MAX_Q_NUM (10)
#define RCFW_DEBUG_MESSAGE 0
#define RC_DATA_TIMEOUT     5000 /* 5000 ms*/
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct RCEventQ_S{
    SP5K_MUTEX mutex;
    json_t* jroot[RC_EVENT_MAX_Q_NUM]; 
    rcDevicetype_t dev[RC_EVENT_MAX_Q_NUM];
	UINT8  CurQ1stIndex;
	UINT8  EventQIndex;
	UINT8  TotalQNum;
} rcEventQueue_s;

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
void appRCTXEventQProcess(rcEventQueue_s *rcEventQueue);
void appRCDispatchEventSet(UINT32 event);
void appRCRXEvent2Queue(rcEventQueue_s *rcEventQueue,json_t *jroot,rcDevicetype_t dev);
void appRCTXEvent2Queue(rcEventQueue_s *rcEventQueue,json_t *jroot,rcDevicetype_t dev);
void appRCTXSendEvent(json_t *jroot,rcDevicetype_t dev);
void appRCRXSendEvent(json_t *jroot,rcDevicetype_t dev);
void appRCDispatchInit(void);

#if defined(SPCA6330) || defined(SPCA6350)
	#if defined(SPCA6350)
		#if !defined(appRealloc)
			#define appRealloc realloc
		#endif
	#elif defined(SPCA6330)
		void *appRealloc(void *p, size_t n);
	#endif
#endif

void memdump(void *mem, UINT32 len);

#endif  /* __APP_REMOTECONTROL_H__ */

