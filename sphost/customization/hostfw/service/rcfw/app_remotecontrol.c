/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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
 *  Author:
 *
 **************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ndk_types.h>
#include <ndk_global_api.h>
#include <ndk_streaming.h>
#if SP5K_NDK2
#include <ndk_tcpip_api.h>
#include <ndk_wpas.h>
#include <ndk_hostap.h>
#include <ndk_dhcpd.h>
#include <ndk_ftpd.h>
#include <ndk_socutils.h>
#include <rtk_netdev.h>
#endif
#include <ctype.h>
#include <api/cmd.h>
#include <sp5k_dbg_api.h>
#include <sp5k_global_api.h>
#include <api/sp5k_fs_api.h>
#include <sp5k_utility_api.h>
#include <proflog.h>
#include <app_timer.h>
#include "app_General.h"
#include <app_osd_api.h>
#include <gpio_custom.h>
#include <sp5k_pb_api.h>
#include "app_com_api.h"
#include "app_sensor.h"
#include "app_video.h"
#include "app_dbg_api.h"
#include "app_util.h"
#include "app_message_server.h"
#include "app_sockel_utility.h"

#if SP5K_BTON
#include "ndk_bluetooth.h"
#include "app_bt_device.h"
#include "app_bt_utility.h"
#endif

#include "app_remotecontrol.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
/* JSON format 
{
    "sequence":10003,		#the data of transmission ID
    "param":[				#parameter data array
    4001,					#array index 1 , #event ID
    3,						#array index 2
    0,						#array index 3
    0						#array index 4
	],						#array end
    "feedback":1 #0->no , 1->immediately feedback , 2-> normal
}
*/


SP5K_EVENT_FLAGS_GROUP  ev_rc_evt_group;

#define RC_THREAD_PRIORITY 				  16
#define RC_THREAD_TIME_SLICE              20
#define RC_DISPATCH_TASK_INIT		  0x0001
#define EVENT_RC_PROCESS              0x0002

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT32 drawsequence = 1;
	
static rcEventQueue_s sRCTXEventQueue={0};
static rcEventQueue_s sRCRXEventQueue={0};

#if defined(SPCA6330) && !defined(SPCA6350) 
	void*  osMemRealloc(void *pori, UINT32 size);
	void *
	appRealloc(void *p, size_t n)
	{
		if (!p) return sp5kMallocCache(n);
		return osMemRealloc(p, n);
	}
#endif

#define default_ifname "wlan0"
int appRCCheckNetLoading(void)
{
    if ( ndk_netif_get_netload(default_ifname) > 70 ) {
		return 1;
	}
	return 0;
}

extern BOOL ndk_st_send_event(UINT32 event, UINT32 data);
extern void ndk_dualstream_set_framerate(UINT32 frmrate);

#if SP5K_NDK2
    #define IOCS_WIFI_QUEUE_CLR RTK_IOCS_WIFI_QUEUE_CLR
#else
    #define IOCS_WIFI_QUEUE_CLR NDK_IOCS_WIFI_QUEUE_CLR
#endif
    
int appRCCleraBuffer(void)
{
    ndk_netif_ioctl(IOCS_WIFI_QUEUE_CLR, (long)default_ifname, 0);
    ndk_st_send_event(NDK_ST_EVT_TXBUFQ_CLEARED, 0);
	return 0;
}

int appSplitCheckPattern(
	void *buf, int len , int *head_pos
)
{
	unsigned char *pbuff = (unsigned char *)buf ;
    int openbrace_counter = 0 ;
    int i = 0 ;

    /* find JSON format { ... } in buffer */

    for (i = 0; i < len; i++) {
		unsigned char c;
		c = *pbuff ;
        if (c == 0x7B) {
            if ( openbrace_counter == 0 ) {
                *head_pos = i;
            }
            openbrace_counter++;
		} else if ((c == 0x7D) && (*head_pos != -1) ) {
            openbrace_counter--;
            if ( openbrace_counter == 0 ) {

                return i+1;
            }
        }
        pbuff++;
	}

    if ( openbrace_counter == 0  ) {
        return -1;
    }
    return 0;
}

int appRCDoFeedbacks(json_t *jroot , rcDevicetype_t dev)
{
    int ret = 0 ;
    rcDevicetype_t devid = dev ;
    char *JsonDump = NULL ;

    if ( devid == DEVICE_NONE ) {
        /* get dev from EVENTCODE */
    }

    JsonDump = json_dumps(jroot,JSON_ENCODE_ANY);

    #if RCFW_DEBUG_MESSAGE
        memdump(JsonDump,strlen(JsonDump));
    #endif

    switch (devid) {
    case DEVICE_NET:
        ret = appSocketMessageOperation(APP_MESSAGE_SVR_SEND,JsonDump,strlen(JsonDump));
        break;
#if SP5K_BTON
    case DEVICE_BT_GATT:
        ret = appBlueToothOperation(APP_BT_OPERATION_SEND,JsonDump,strlen(JsonDump),APP_BT_GATT);
        break;
    case DEVICE_BT_GATT_INDICATE:
        ret = appBlueToothOperation(APP_BT_OPERATION_SEND,JsonDump,strlen(JsonDump),APP_BT_GATT_INDICATE);
        break;
    case DEVICE_BT_DEVTTY:
        ret = appBlueToothOperation(APP_BT_OPERATION_SEND,JsonDump,strlen(JsonDump),APP_BT_SPP);
        break;
#endif
    default:
        HOST_ASSERT(0);
        break;
    }
    /* free JSON */
    if ( JsonDump) {
        sp5kFree(JsonDump);
    }
    json_decref(jroot);
	return ret;
}


int appRCDoActions(UINT32 *sParam , int *iParam)
{
	RCEventCode_e EventCode;
	EventCode = iParam[0];
	int ret = 0 ;
	int i = 0 ;
	UINT32 temp5kmsgID = 0;
    UINT16 strlens ;
    UINT8* pstr ;
    switch (EventCode) {
	case RC_EVENTCODE_TEST_SOUND:
        appSundVolumeSet(3);
        for(i = 0 ; i < iParam[1] ; i++ ) {	
            appBeep(FALSE, TRUE);
            appTimeDelay(100,1);						
        }	
        appSundVolumeSet(0);
		break;
	case RC_EVENTCODE_5KAPI_OP:
		SET_LED_SELFTIMER(iParam[1]);
		SET_LED_AF(iParam[1]);
		break;
	case RC_EVENTCODE_5KMSG_OP:
	    temp5kmsgID = ((iParam[1]) << 16) + iParam[2];
        sp5kHostMsgSend( temp5kmsgID, 0);
		break;
	case RC_EVENTCODE_UART_WRITE:						
            pstr = (UINT8 *)sParam[1];
			strlens = strlen((const char *)pstr);
            /* USE UART2 as sample */
			sp5kUartBufWrite(2,(UINT8 *)pstr, strlens);
		break;
	case RC_EVENTCODE_UART_READ:
		 break;
	default:
		break;
	}

	return ret;
}

int appRCParamParser(json_t *jitem , UINT32 *sParam, int *iParam)
{
	int arraysize = 0 ;
	int i = 0 ;
	

	/* HOST_ASSERT(jitem); */
    if ( jitem == NULL ) {
        return -1 ;
    }
	arraysize = json_array_size(jitem);
	/* HOST_ASSERT(arraysize); */
    if ( arraysize == 0 ) {
        return -1 ;
    }
	while ( i < arraysize ) {
		json_t *jarrayitem;
		jarrayitem = json_array_get(jitem, i);

		switch ( json_typeof (jarrayitem) ) {
		case JSON_STRING:
			sParam[i] = (UINT32) json_string_value(jarrayitem);
			#if RCFW_DEBUG_MESSAGE
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] param(%d) %s",__FUNCTION__,__LINE__,i,(char *)sParam[i]);
			#endif
			break;
		case JSON_INTEGER:
			iParam[i] = json_integer_value(jarrayitem);
			#if RCFW_DEBUG_MESSAGE
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] param(%d) %d",__FUNCTION__,__LINE__,i,iParam[i]);
			#endif
			break;
		case JSON_REAL:
			iParam[i] = json_real_value(jarrayitem);
			#if RCFW_DEBUG_MESSAGE
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] param(%d) %d",__FUNCTION__,__LINE__,i,iParam[i]);
			#endif
			break;
		case JSON_TRUE:
		case JSON_FALSE:
			iParam[i] = json_boolean_value(jarrayitem);
			#if RCFW_DEBUG_MESSAGE
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] param(%d) %d",__FUNCTION__,__LINE__,i,iParam[i]);
			#endif
			break;
		case JSON_OBJECT:
		case JSON_ARRAY:
		case JSON_NULL:
		default:
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] JSON type ERROR",__FUNCTION__,__LINE__);
			HOST_ASSERT(0);
			break;
		}
		i++ ;
	}
	return 0;
}

/*
    Process RX
*/


void appRCTXSendEvent(json_t *jroot,rcDevicetype_t dev)
{
	appRCTXEvent2Queue(&sRCTXEventQueue,jroot,dev);
}

void appRCTXEvent2Queue(rcEventQueue_s *rcEventQueue,json_t *jroot,rcDevicetype_t dev)
{

 	if (!rcEventQueue->mutex) {
		sp5kOsMutexCreate(&rcEventQueue->mutex, "hostRCTXMut", TX_INHERIT);
		profLogPrintf(0, "hostRCTXMut=%x", rcEventQueue->mutex);
	}
	if (rcEventQueue->mutex)	sp5kOsMutexGet(&rcEventQueue->mutex, TX_WAIT_FOREVER);
	if(rcEventQueue->TotalQNum >= RC_EVENT_MAX_Q_NUM)
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] [RC TX EvtQ] total queue number exceed the MAX",__FUNCTION__,__LINE__);
		if (rcEventQueue->mutex)	sp5kOsMutexPut(&rcEventQueue->mutex);
		return;
	}
	rcEventQueue->TotalQNum++;

    /* Send to Queue */
    rcEventQueue->jroot[rcEventQueue->EventQIndex] = jroot ;
    rcEventQueue->dev[rcEventQueue->EventQIndex] = dev ;

    CIRCULAR_INCREASE(rcEventQueue->EventQIndex,0,RC_EVENT_MAX_Q_NUM-1); /* queue : [0] [1] ........ [8] [9] */
	if (rcEventQueue->mutex)	sp5kOsMutexPut(&rcEventQueue->mutex);
	appRCDispatchEventSet(EVENT_RC_PROCESS);
}

void appRCTXEventQProcess(rcEventQueue_s *rcEventQueue)
{

	UINT8 processedQNum = 0;
	UINT8 curQindex;
	UINT8 totalQNum;
    
	if (rcEventQueue->mutex)	sp5kOsMutexGet(&rcEventQueue->mutex, TX_WAIT_FOREVER);
	curQindex = rcEventQueue->CurQ1stIndex;
	totalQNum = rcEventQueue->TotalQNum;
	processedQNum = totalQNum;

	if( totalQNum ) {
        json_t *jroot = NULL ;
        rcDevicetype_t dev;
		UINT8 i = 0;
		for(i=curQindex;i < RC_EVENT_MAX_Q_NUM;i++) /* check 1st Q NO. in the ring buffer */
		{
            jroot = rcEventQueue->jroot[i];
            if( jroot )
            {
                curQindex = i;
                HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] [RC RX EvtQ] current Queue no:%d",__FUNCTION__,__LINE__,curQindex);
                break;
            }
		}

		for(i = curQindex ;i < totalQNum + curQindex;i++)
		{
            jroot = rcEventQueue->jroot[i];
            dev = rcEventQueue->dev[i];

            /* HOST_ASSERT(jroot); */
            if ( jroot != NULL ) {
                appRCDoFeedbacks(jroot, dev);
            }
            rcEventQueue->jroot[i] = NULL ;
            rcEventQueue->dev[i] = DEVICE_NONE ;
			CIRCULAR_DECREASE(processedQNum,0,RC_EVENT_MAX_Q_NUM);
		}

		rcEventQueue->EventQIndex = 0;
		rcEventQueue->CurQ1stIndex = 0 ;
		rcEventQueue->TotalQNum = processedQNum;
	}

	if (rcEventQueue->mutex)	sp5kOsMutexPut(&rcEventQueue->mutex);
}

/*
    Process RX
*/

void appRCRXSendEvent(json_t *jroot,rcDevicetype_t dev)
{
	appRCRXEvent2Queue(&sRCRXEventQueue,jroot,dev);
}


void appRCRXEvent2Queue(rcEventQueue_s *rcEventQueue,json_t *jroot,rcDevicetype_t dev)
{

    int feedback = 0 ;
    json_t *jitem;
    
    
    

    if (NULL != (jitem = json_object_get(jroot, "feedback"))) {
        /* feedback */
        feedback = json_integer_value(jitem);
    } else {
        json_object_set_new( jroot, "feedback", json_integer(0) );
    }

    if ( feedback == 1 ) {
		int iParam[4]={0};
		UINT32 sParam[4] ={0};
		json_t *jarrayitem;
        /* immediately feedback */
        jarrayitem = json_object_get(jroot, "param");
        if ( jarrayitem == NULL ) {
            HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] [RC TX EvtQ] param ERROR",__FUNCTION__,__LINE__);
            return ;
        }
        if ( appRCParamParser(jarrayitem,sParam,iParam) != 0 ) {
            HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] [RC TX EvtQ] param ERROR",__FUNCTION__,__LINE__);
            HOST_ASSERT(0);
        } else {
        	int ret  ;
            ret = appRCDoActions(sParam,iParam);
            /* feedback it */
            json_integer_set(jitem,ret); /* set feedback == err number */
            if ( dev == DEVICE_BT_GATT ) {
                dev = DEVICE_BT_GATT_INDICATE ;
            }
            appRCDoFeedbacks(jroot, dev);
        }
        return ;
    }

    if (!rcEventQueue->mutex) {
		sp5kOsMutexCreate(&rcEventQueue->mutex, "hostRCRXMut", TX_INHERIT);
	}

	if (rcEventQueue->mutex)	sp5kOsMutexGet(&rcEventQueue->mutex, TX_WAIT_FOREVER);

	if(rcEventQueue->TotalQNum >= RC_EVENT_MAX_Q_NUM)
	{
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] [RC RX EvtQ] total queue number exceed the MAX",__FUNCTION__,__LINE__);
		if (rcEventQueue->mutex)	sp5kOsMutexPut(&rcEventQueue->mutex);
		return;
	}

	rcEventQueue->TotalQNum++;

    /* Send to Queue */
    rcEventQueue->jroot[rcEventQueue->EventQIndex] = jroot ;
    rcEventQueue->dev[rcEventQueue->EventQIndex] = dev ;

	CIRCULAR_INCREASE(rcEventQueue->EventQIndex,0,RC_EVENT_MAX_Q_NUM-1); /* queue : [0] [1] ........ [8] [9] */
	if (rcEventQueue->mutex)	sp5kOsMutexPut(&rcEventQueue->mutex);
	appRCDispatchEventSet(EVENT_RC_PROCESS);
}

void appRCRXEventQProcess(rcEventQueue_s *rcEventQueue)
{
	UINT8 i = 0, processedQNum = 0;
	UINT8 curQindex;
	UINT8 totalQNum;
    
	if (rcEventQueue->mutex)	sp5kOsMutexGet(&rcEventQueue->mutex, TX_WAIT_FOREVER);
	curQindex = rcEventQueue->CurQ1stIndex;
	totalQNum = rcEventQueue->TotalQNum;
	processedQNum = totalQNum;

	if( totalQNum ) {
        json_t *jroot = NULL ;
        int feedback = 0 ;
        json_t *jitem;
        int iParam[4]={0};
        UINT32 sParam[4] ={0};
        json_t *jarrayitem;
        int ret = 0 ;
        rcDevicetype_t dev;
        json_t *jitem_sequence;

		#if RCFW_DEBUG_MESSAGE
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] [RC RX EvtQ] total Queue b4 process:%d",__FUNCTION__,__LINE__,totalQNum);
        #endif
        for(i=curQindex;i < RC_EVENT_MAX_Q_NUM;i++) /* check 1st Q NO. in the ring buffer */
		{
            jroot = rcEventQueue->jroot[i];
			if( jroot )
			{
				curQindex = i;
                #if RCFW_DEBUG_MESSAGE
				HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] [RC RX EvtQ] current Queue no:%d",__FUNCTION__,__LINE__,curQindex);
                #endif
				break;
			}
		}

		for(i = curQindex ;i < totalQNum + curQindex;i++)
		{
            jroot = rcEventQueue->jroot[i];
            dev = rcEventQueue->dev[i];
            /* HOST_ASSERT(jroot); */
            if ( jroot == NULL ) {
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] [RC TX EvtQ] param ERROR",__FUNCTION__,__LINE__);
                goto NEXT_ITEM;
            }
            jitem = json_object_get(jroot, "feedback");
            /* HOST_ASSERT(jitem); */
            if ( jitem == NULL ) {
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] [RC TX EvtQ] param ERROR",__FUNCTION__,__LINE__);
                json_decref(jroot);
                goto NEXT_ITEM;
            }
            feedback = json_integer_value(jitem);
            jarrayitem = json_object_get(jroot, "param");
            if ( jarrayitem == NULL ) {
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] [RC TX EvtQ] param ERROR",__FUNCTION__,__LINE__);
                json_decref(jroot);
                goto NEXT_ITEM;
            }
            if ( appRCParamParser(jarrayitem,sParam,iParam) != 0 ) {
                HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] [RC TX EvtQ] param ERROR",__FUNCTION__,__LINE__);
                HOST_ASSERT(0);
            } else {
                if (NULL != (jitem_sequence = json_object_get(jroot, "sequence"))) {
                    drawsequence = json_integer_value(jitem_sequence);
                } 
                ret = appRCDoActions(sParam,iParam);
                /* feedback it */
                if ( feedback ) {
                    /* Send to TX Queue */
                    json_integer_set(jitem,ret); /* set feedback == err number */
                    appRCTXSendEvent(jroot,dev);
                } else {
                    json_decref(jroot);
                }
            }
NEXT_ITEM:
            rcEventQueue->jroot[i] = NULL ;
            rcEventQueue->dev[i] = DEVICE_NONE ;
			CIRCULAR_DECREASE(processedQNum,0,RC_EVENT_MAX_Q_NUM);
		}
		rcEventQueue->EventQIndex = 0;
		rcEventQueue->CurQ1stIndex = 0 ;
		rcEventQueue->TotalQNum = processedQNum;
		#if RCFW_DEBUG_MESSAGE
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] [RC RX EvtQ] total Queue after process:%d",__FUNCTION__,__LINE__,rcEventQueue->TotalQNum);
        #endif
	} 
    if (rcEventQueue->mutex)	sp5kOsMutexPut(&rcEventQueue->mutex);
}


int appRCRXDataParser(void *buf, unsigned int len,rcDevicetype_t dev)
{

	#if RCFW_DEBUG_MESSAGE
		memdump(buf,(UINT32)len);
	#endif

   	json_t *jroot;
	json_error_t jerror;
	
	jroot = json_loadb(buf , (UINT32)len , JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK , &jerror);
	if(jroot)
	{
		json_t *jitem;
	        if (NULL != (jitem = json_object_get(jroot, "sequence"))) {
				/* RC frame work */
	            appRCRXSendEvent(jroot,dev);
		} else {
				HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] JSON format ERROR",__FUNCTION__,__LINE__);
		}
	} else {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] line(%d),column(%d),position(%d),text(%s)",__FUNCTION__,__LINE__,jerror.line,jerror.column ,jerror.position,jerror.text);
        #if RCFW_DEBUG_MESSAGE
    	memdump(buf,(UINT32)len);
        #endif
        return 1 ;
	}
    return 0 ;
}

void
appRCDispatchEventSet(UINT32 event)
{
	if (!ev_rc_evt_group) {
		sp5kOsEventFlagsCreate(&ev_rc_evt_group, "RCDispatchEvent");
	}
	sp5kOsEventFlagsSet(&ev_rc_evt_group,event,TX_OR);	
}

void
appRCDispatchThread(
	ULONG init_data
)
{
	ULONG ulActualEvents;	
	
	while(1) {		
		sp5kOsEventFlagsGet(&ev_rc_evt_group,RC_DISPATCH_TASK_INIT  | EVENT_RC_PROCESS ,TX_OR_CLEAR,
			&ulActualEvents,TX_WAIT_FOREVER);
		if (ulActualEvents & RC_DISPATCH_TASK_INIT) {			
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] [RC EVT] RC_DISPATCH_TASK_INIT",__FUNCTION__,__LINE__);
		} else if (ulActualEvents & EVENT_RC_PROCESS) {
            #if RCFW_DEBUG_MESSAGE
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] [RC EVT] EVENT_RC_PROCESS",__FUNCTION__,__LINE__);
            #endif
			appRCRXEventQProcess(&sRCRXEventQueue);
            appRCTXEventQProcess(&sRCTXEventQueue);
		} 
		
	}
}

static UINT32 netLastLen = 0;
static UINT8 *netLastData = NULL;
static UINT32 netLastTime = 0 ;

void appRCRXMsgSvrReadCallBack(void *buf, unsigned int len)
{
    int ret = 0 ;

    unsigned char *pbuff = (unsigned char *)buf ;
    int json_len = 0 ;
    int head_pos = 0 ;
    int leave_size = len ;
    int closenbrace_index = 0 ;
    UINT32 CurrentTime = 0 ;

    #if RCFW_DEBUG_MESSAGE
	memdump(buf,(UINT32)len);
    #endif
    printf("get(%u)\n",len);
    if (len == 0) {
        if ( netLastLen == 0 )
            return;
        len = 0 ;
    }

    if ( netLastTime ) {
        CurrentTime = sp5kMsTimeGet();
        if ( (CurrentTime - netLastTime) > RC_DATA_TIMEOUT ) {
            /* Free Last Data Saved */
            netLastTime = 0 ;
            netLastLen = 0 ;
            if ( netLastData ) {
                sp5kFree(netLastData);
                netLastData = NULL ;
            }
            if (len == 0) {
                return;
            }
        } else if ( len > 0 ) {
            netLastData = appRealloc(netLastData,netLastLen + len );
            memcpy(netLastData + netLastLen , buf , len);
            netLastLen = netLastLen + len ;
            pbuff = (unsigned char *)netLastData ;
            leave_size = netLastLen ;
        } else {
            pbuff = (unsigned char *)netLastData ;
            leave_size = netLastLen ;
        }
    }

SPLIT_JSON:
    /* Multi-JSON sting */
    head_pos = -1 ;
    closenbrace_index = appSplitCheckPattern(pbuff,leave_size,&head_pos) ;

    if ( closenbrace_index < 0 ) {
        /* Memory ERROR Skip All */
        if ( netLastData ) {
            sp5kFree(netLastData);
            netLastData = NULL ;
        }
        netLastTime = 0 ;
        netLastLen = 0 ;
        return;
    }

    if ( closenbrace_index > 0 ) {
        leave_size = leave_size - head_pos ;
        pbuff = pbuff + head_pos ;
        json_len = closenbrace_index - head_pos ;
        #if RCFW_DEBUG_MESSAGE
    	memdump(pbuff,(UINT32)json_len);
        #endif
        ret = appRCRXDataParser(pbuff,json_len,DEVICE_NET);

        if (ret) {
            /* Some Thing Wrong */
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] JSON data ERROR",__FUNCTION__,__LINE__);
        }

        netLastTime = sp5kMsTimeGet();
        pbuff = pbuff + json_len ;
        leave_size = leave_size - json_len;
        if ( leave_size ) {
            goto SPLIT_JSON;
        } else {
            if ( netLastData ) {
                sp5kFree(netLastData);
                netLastData = NULL ;
            }
            netLastTime = 0 ;
            netLastLen = 0 ;
        }   

    } else {
        if ( head_pos ) {
            leave_size = leave_size - head_pos;
        }
        netLastTime = sp5kMsTimeGet();
        netLastLen = leave_size ;
        UINT8 *nettemp = sp5kMallocCache(leave_size);
        memcpy(nettemp,pbuff,leave_size);
        if ( netLastData ) {
            sp5kFree(netLastData);
        }
        netLastData = nettemp ;
        #if RCFW_DEBUG_MESSAGE
        memdump(netLastData,(UINT32)netLastLen);
        #endif
    }
    return;
}

int appRCRXMsgSessTimeOutCallBack(int timeout_counter)
{
    int ret = 0 ;
    static int checkloading = 0 ;
    #if RCFW_DEBUG_MESSAGE
    HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] (%d)",__FUNCTION__,__LINE__,timeout_counter);
    #endif

    if ( timeout_counter > 10 ) {
        ret = appRCCheckNetLoading();
        if ( ret ) {
            appRCCleraBuffer();
            ndk_dualstream_set_framerate(5);
            checkloading = 5 ;
            return 1; /* reset timeout */
        }
    } else {
        if ( checkloading ) {
            ret = appRCCheckNetLoading();
            if ( ret == 0 ) {
                checkloading--;
            }

            if ( checkloading == 0 ) {
                ndk_dualstream_set_framerate(30);
            }
        }
    }
    return 0;
}

#if SP5K_BTON
static btData_t *btGATTdata = NULL;
static btData_t *btSPPdata = NULL;
static UINT32   btGATTLastTime = 0 ;
static UINT32   btSPPLastTime = 0 ;

void appRCRXBTReadCallBack(void *buf)
{	
    btData_t *btdata = (btData_t *)buf;
    
    rcDevicetype_t dev = 0;

    #if RCFW_DEBUG_MESSAGE
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] len(%d)",__FUNCTION__,__LINE__,btdata->len);
    #endif

    if ( btdata->len ) {
	int ret;
	btData_t **btSaved = 0 ;
	UINT32 *LastTime = 0 ;
	UINT32 CurrentTime = 0;
        #if RCFW_DEBUG_MESSAGE
        memdump(btdata->data, (UINT32)btdata->len);
        #endif

        if ( btdata->btProfile ==  APP_BT_GATT ) {
            dev = DEVICE_BT_GATT ;
            btSaved = &btGATTdata ;
            LastTime = &btGATTLastTime;
        } else if ( btdata->btProfile ==  APP_BT_SPP ) {
            dev = DEVICE_BT_DEVTTY ;
            btSaved = &btSPPdata ;
            LastTime = &btSPPLastTime;
        } else {
            HOST_ASSERT(0);
        }

        /* Check Saved Data */
        CurrentTime = sp5kMsTimeGet();
        if ( *LastTime ) {
            if ( (CurrentTime - *LastTime) > RC_DATA_TIMEOUT ) {
                /* Free Last Data Saved */
                *LastTime = 0 ;
                if ( (*btSaved)->data ) {
                    sp5kFree((*btSaved)->data);
                }
                sp5kFree(*btSaved);
                *btSaved = NULL ;
                ret = appRCRXDataParser(btdata->data,btdata->len,dev);
            } else {
                (*btSaved)->data = appRealloc((*btSaved)->data,(*btSaved)->len + btdata->len);
                memcpy((*btSaved)->data + (*btSaved)->len , btdata->data , btdata->len);
                (*btSaved)->len = (*btSaved)->len + btdata->len ;
                #if RCFW_DEBUG_MESSAGE
                memdump((*btSaved)->data, (UINT32)(*btSaved)->len);
                #endif
                ret = appRCRXDataParser((*btSaved)->data,(*btSaved)->len,dev);
            }
        } else {
            ret = appRCRXDataParser(btdata->data,btdata->len,dev);
        }

        if (ret) {
            /* Keep data in memory and wait timeout */
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] Need more data",__FUNCTION__,__LINE__);
            *LastTime = sp5kMsTimeGet();
            if ( *btSaved == NULL) {
                *btSaved = btdata;
            }
            return ;
        } else if ( *LastTime ) {
            *LastTime = 0 ;
            if ( *btSaved ) {
                if ((*btSaved)->data) {
                    sp5kFree((*btSaved)->data);
                }
                sp5kFree(*btSaved);
                *btSaved = NULL ;
            }
        }

        if ( btdata->data ) {
            sp5kFree(btdata->data);
        }
    }

    sp5kFree(btdata);
}
#endif

void appRCDispatchInit(	void)	
{
	SP5K_THREAD *ptrdRCDispatchThr;

	HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] start",__FUNCTION__,__LINE__);
	
	sp5kOsEventFlagsCreate(&ev_rc_evt_group, "RCDispatchEvent");

	ptrdRCDispatchThr = sp5kOsThreadCreate( 
					  "RCDispatchThread",
			           appRCDispatchThread,
			           0,
			           RC_THREAD_PRIORITY,
			           RC_THREAD_PRIORITY,
			           RC_THREAD_TIME_SLICE,
			           TX_AUTO_START );

	  if(NULL == ptrdRCDispatchThr){
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] [RC] task create error",__FUNCTION__,__LINE__);

	  }      
}


extern void appUart2Init(void);

void appRCGlobalInit(void)
{

	UINT8 ifname[IF_NAMESIZE];

    /* for UART Sample */
    appUart2Init();

#if SP5K_BTON
	/* Check BT enable ? */
	if ( bt_device_status() == FALSE ) {
		/* Enable BT */
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "Force Enable BT");
        appBlueToothOperation(APP_BT_OPERATION_SETCB,appRCRXBTReadCallBack);
		appBlueToothOperation(APP_BT_OPERATION_START,BT_DO_ASYNC|BT_HCI_LOAD|BT_GATT_INIT|BT_SPP_INIT);
	} else {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "Don't Enable BT");
        return ;
    }

#endif

	/* Check WiFi enable ? */
	if (ndk_netif_ioctl(NDK_IOCG_IF_INDEXTONAME, 1, (long *)ifname) != 0) {
		#include "app_wifi_utility.h"
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "Force Enable WiFi");
		/* Use utility in app_wifi_utility.c */
		appWiFiStartConnection(WIFI_LOAD|DHCP_SERVER|HOST_AP);
	}
	/* Init Message Socket Server */
    appSocketMessageOperation(APP_MESSAGE_SESSION_TIMEOUT_CB,(void *)appRCRXMsgSessTimeOutCallBack);
	appSocketMessageOperation(APP_MESSAGE_SVR_START,(void *)appRCRXMsgSvrReadCallBack);

    appRCDispatchInit();

}



