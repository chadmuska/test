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
#include <ctype.h>
#include <stdarg.h>
#include <sys/errno.h>
#include "common.h"
#include "app_dbg_api.h"
#include "ndk_global_api.h"
#include "app_timer.h"
#include "app_message_server.h"
#include "app_sockel_utility.h"

#if SP5K_NDK2
	#include "ndk_tcpip_api.h"
#else
	#include "tcpip_port.h"
#endif


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define APP_MSGSVR_SOCKEL_MAX 4
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static appMsgSvrReadCb_t appMsgSvrReadCb = NULL ;
static appMsgSessTimeOutCb_t appMsgMessTimeOutCb = NULL ;

struct MsgSvr {
	int skfd;
	AppSockelHandle sockel;
	int skel_timeout;
	SP5K_QUEUE txQueue ;
};

struct MsgSvrSession {
	struct MsgSvr* pMsgSvr;
	int 	skfd;
	int		skel_timeout;
    int		timeout_counter;
};


struct MsgSvrData
{
	char* data;
	int	datalen;
};


static struct MsgSvr gMsgSvr;
static SP5K_THREAD *thrAppMsgSvr = NULL;


/**************************************************************************
 *                         F U N C T I O N     D E F I N I T I O N        *
 **************************************************************************/

static int appMsgSvrConfigTCP(int skfd)
{
	int optval = 0 ;
	int ret = 0 ;
	UINT32 tcp_timeout = 10;
	UINT32 polling_period = tcp_timeout/2;
	UINT32 retry_times = tcp_timeout - polling_period;

	#if 0
	if ((ret = setsockopt(skfd, SOL_SOCKET, SO_REUSEADDR, &optval , sizeof(optval))) < 0) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] setsockopt ERROR",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}
	if ((ret = setsockopt(skfd, SOL_SOCKET, SO_REUSEPORT, &optval , sizeof(optval))) < 0) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] setsockopt ERROR",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}
	#endif


	optval = 1 ;
	if ((ret = setsockopt(skfd, SOL_SOCKET, SO_KEEPALIVE, &optval , sizeof(optval))) < 0) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] setsockopt ERROR",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	} 
	else if ((ret = setsockopt(skfd, IPPROTO_TCP, TCP_KEEPIDLE, &polling_period , sizeof(polling_period))) < 0) {	/* time unit is 1s */
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] setsockopt ERROR",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}
	else if ((ret = setsockopt(skfd, IPPROTO_TCP, TCP_KEEPINTVL, &optval , sizeof(optval))) < 0) {	/* time unit is 1s */
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] setsockopt ERROR",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}
	else if ((ret = setsockopt(skfd, IPPROTO_TCP, TCP_KEEPCNT, &retry_times , sizeof(retry_times))) < 0) {		/* retry times */
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] setsockopt ERROR",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}
	else if ((ret = setsockopt(skfd, IPPROTO_TCP, TCP_NODELAY, &optval , sizeof(optval))) < 0) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] setsockopt ERROR",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}


	#if 0
	/* LwIP NO SO_LINGER */

	struct linger so_linger;

    so_linger.l_onoff = 1;
    so_linger.l_linger = 300;

    if ((ret = setsockopt(skfd, SOL_SOCKET, SO_LINGER, &so_linger , sizeof(so_linger))) < 0) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] setsockopt ERROR",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}
	#endif

	return 0 ;

ERROR_EXIT:

	return -1 ;
}

static void appMsgSvrGlobalRoutine(UINT32 param)
{
	AppSockelRun((AppSockelHandle )param, NULL);
}

static void appMsgSvrSessionRemove(struct MsgSvrSession* pMsgSvrSession)
{
	if (pMsgSvrSession->skfd >= 0) {
		AppSockelRemoveSocket(pMsgSvrSession->pMsgSvr->sockel, pMsgSvrSession->skfd);
		closesocket(pMsgSvrSession->skfd);
		pMsgSvrSession->skfd = -1 ;
	}
	#if SOCKET_DEBUG_MESSAGE
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] free sess(0x%x)",__FUNCTION__,__LINE__,(unsigned int)pMsgSvrSession);
	#endif
	sp5kFree(pMsgSvrSession);
}


static int appMsgSvrSelectInterruptible(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, void *UserEvent)
{
	return select_interruptible(maxfdp1, readset, writeset, exceptset, timeout, UserEvent);
}

static void appMsgSvrSelectWakeup(void *UserEvent)
{
	select_wakeup(UserEvent);
}


static int appMsgSvrSessionRead(struct MsgSvrSession* pMsgSvrSession)
{
	char buffer[MAX_MSGSVR_LEN];
	int size_recv ;
	int sock_err = 0 ;
	int ret = 0 ;
	socklen_t lon;
	int valopt;


SOCKET_REGET:
	size_recv = recv( pMsgSvrSession->skfd , buffer , MAX_MSGSVR_LEN , MSG_DONTWAIT );

	#if SOCKET_DEBUG_MESSAGE
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] size_recv %d",__FUNCTION__,__LINE__,size_recv);
	#endif

	if ( size_recv > 0 ) {
		/* Receive OK */
		if ( appMsgSvrReadCb ) {
			appMsgSvrReadCb(buffer,size_recv);
		} else {
			/* Another Way to Precoess Data */
			#if SOCKET_DEBUG_MESSAGE
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] Another Way to Precoess Data",__FUNCTION__,__LINE__);
				memdump(buffer,(UINT32)size_recv);
			#endif
		}
		return 0; /* Continute receive data */
	} else {
		/* Check Connection */
		#if SOCKET_DEBUG_MESSAGE
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] size_recv %d",__FUNCTION__,__LINE__,size_recv);
		#endif

		lon = sizeof(int);
		ret = getsockopt(pMsgSvrSession->skfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);

		#if SOCKET_DEBUG_MESSAGE
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] ret %d",__FUNCTION__,__LINE__,ret);
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] valopt %d",__FUNCTION__,__LINE__,valopt);
		#endif

		if ( !ret ) {
			sock_err = lwip_geterr(pMsgSvrSession->skfd);
			#if SOCKET_DEBUG_MESSAGE
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "sock_err %d",sock_err);
			#endif
			if (sock_err == EWOULDBLOCK || sock_err == EAGAIN) {
				goto SOCKET_REGET;
			} else if (sock_err == EINTR) {
				goto SOCKET_REGET;
			}
		}
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] valopt %d",__FUNCTION__,__LINE__,valopt);
		appMsgSvrSessionRemove(pMsgSvrSession);
		goto ERROR_EXIT;
	}

	return 0;

ERROR_EXIT:

	return -1;
}

static int appMsgSvrSessionWrite(struct MsgSvrSession* pMsgSvrSession)
{
	int size_send = 0;
	int total_size = 0 ;
	int sock_err = 0 ;
	char *pbuff ;
//	int ret = 0 ;
	struct MsgSvrData sessMsgSvrData;


	while (1) {
		int ret;
		ret = sp5kOsQueueReceive(&gMsgSvr.txQueue, (void*)&sessMsgSvrData, 20 /* wait 20 ms or TX_NO_WAIT */ );
		if ( ret != 0 ) {
			break;
		}

		if ( (sessMsgSvrData.data) && (sessMsgSvrData.datalen > 0) ) {
			#if SOCKET_DEBUG_MESSAGE
			HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] ",__FUNCTION__,__LINE__);
			#endif
			pbuff = sessMsgSvrData.data ;
			total_size = sessMsgSvrData.datalen ;
			do {
				size_send = send(pMsgSvrSession->skfd, pbuff, total_size, MSG_DONTWAIT);
                #if SOCKET_DEBUG_MESSAGE
				HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] total_size %d size_send %d",__FUNCTION__,__LINE__,total_size,size_send);
                #endif
				if (size_send < 0) {
					sock_err = lwip_geterr(pMsgSvrSession->skfd);
					HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] APP_SOCKEL_EVT_WRITE sock_err %d",__FUNCTION__,__LINE__,sock_err);
					ret = -1 ;
					break;
				} else if (size_send > 0) {
					total_size = total_size - size_send ;
					pbuff = pbuff + size_send ;
				} else {
					HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] size_send = 0",__FUNCTION__,__LINE__);
					ret = -1 ;
					break;
				}
			} while (total_size > 0);

			sp5kFree(sessMsgSvrData.data);
		}

		if ( ret ) {
			return -1 ;
		}
	}
	return 0 ;
}


static int appMsgSvrSessionHandler(AppSockelHandle AppSkel, int skfd, APP_SOCKEL_EVENT evt, struct MsgSvrSession* pMsgSvrSession)
{

	int ret = pMsgSvrSession->skel_timeout ;
	int sock_err = 0 ;

	switch (evt) {
	case APP_SOCKEL_EVT_TERMINATED:
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] APP_SOCKEL_EVT_TERMINATED",__FUNCTION__,__LINE__);
		closesocket(pMsgSvrSession->skfd);
		pMsgSvrSession->skfd = -1 ;
		break;
	case APP_SOCKEL_EVT_WRITE:
		#if 0 && SOCKET_DEBUG_MESSAGE
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] APP_SOCKEL_EVT_WRITE",__FUNCTION__,__LINE__);
		#endif
		sock_err = appMsgSvrSessionWrite(pMsgSvrSession);
		if ( sock_err ) {
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] APP_SOCKEL_EVT_WRITE",__FUNCTION__,__LINE__);
		}
		break;
	case APP_SOCKEL_EVT_READ:
		#if SOCKET_DEBUG_MESSAGE
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] APP_SOCKEL_EVT_READ",__FUNCTION__,__LINE__);
		#endif
		sock_err = appMsgSvrSessionRead(pMsgSvrSession);
		if ( sock_err ) {
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] APP_SOCKEL_EVT_READ",__FUNCTION__,__LINE__);
		} else {
            pMsgSvrSession->timeout_counter = 0 ; /* get data and reset timer */
        }
		break;
    case APP_SOCKEL_EVT_TIMEOUT:
        #if SOCKET_DEBUG_MESSAGE
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] APP_SOCKEL_EVT_TIMEOUT",__FUNCTION__,__LINE__);
        #endif
        if ( appMsgMessTimeOutCb ) {
            pMsgSvrSession->timeout_counter++;
            sock_err = appMsgMessTimeOutCb(pMsgSvrSession->timeout_counter);
            if ( sock_err ) {
                pMsgSvrSession->timeout_counter = 0 ;
            }
        }
        ret = 0 ;
        break;
	case APP_SOCKEL_EVT_ERROR:
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] APP_SOCKEL_EVT_ERROR",__FUNCTION__,__LINE__);
		break;
	default:
		ret = -1 ;
		break;
	}
	return ret;
}



static int appMsgSvrSessionStart(struct MsgSvr* pMsgSvr, int skfd_client)
{
	struct MsgSvrSession* sess;

	sess = sp5kMallocCache(sizeof(struct MsgSvrSession));
	if (!sess) {
		HOST_ASSERT(0);
	}

	memset(sess, 0, sizeof(struct MsgSvrSession));

	#if SOCKET_DEBUG_MESSAGE
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] sess(0x%x)",__FUNCTION__,__LINE__,(unsigned int)sess);
	#endif

	sess->skfd		   = skfd_client;
	sess->pMsgSvr      = pMsgSvr;
	sess->skel_timeout = 20 ;

	if (AppSockelAddSocket(pMsgSvr->sockel , skfd_client, (AppSockelEventHandler)appMsgSvrSessionHandler
						   , (void *)sess, TRUE , sess->skel_timeout , 1 ) < 0) 
	{
		/* todo : free resource */
		return -1;
	}

	return 0;
}


static int appMsgSvrSessionSend(const char *content , unsigned int len)
{
	struct MsgSvrData sessMsgSvrData;

	if (( content ) && ( len > 0 ) ) {
		sessMsgSvrData.data = sp5kMallocCache(len);
		if ( sessMsgSvrData.data == NULL ) {
			HOST_ASSERT(0);
		}
		sessMsgSvrData.datalen = len ;
		memset(sessMsgSvrData.data,0,len);
		memcpy(sessMsgSvrData.data,content,len);
		if (sp5kOsQueueSend(&gMsgSvr.txQueue, (void*)&sessMsgSvrData, 20 /* wait 20 ms or TX_NO_WAIT */ ) != 0) {
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] sp5kOsQueueSend ERROR",__FUNCTION__,__LINE__);
			sp5kFree(sessMsgSvrData.data);
			return FAIL ;
		}
		return SUCCESS ;
	}

	return FAIL ;
}



static int appMsgSvrListenHandler(AppSockelHandle AppSkel, int skfd, APP_SOCKEL_EVENT evt, struct MsgSvr* pMsgSvr)
{
	HOST_ASSERT(pMsgSvr->sockel == AppSkel);
	int ret = pMsgSvr->skel_timeout ;

	switch (evt) {
	case APP_SOCKEL_EVT_TERMINATED:
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] APP_SOCKEL_EVT_TERMINATED",__FUNCTION__,__LINE__);
		closesocket(pMsgSvr->skfd);
		pMsgSvr->skfd = -1 ;
		break;
	case APP_SOCKEL_EVT_WRITE:
		HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] APP_SOCKEL_EVT_WRITE",__FUNCTION__,__LINE__);
		break;
	case APP_SOCKEL_EVT_READ:
		#if SOCKET_DEBUG_MESSAGE
			HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] APP_SOCKEL_EVT_READ",__FUNCTION__,__LINE__);
		#endif
		{
			struct sockaddr_in from_addr;
			socklen_t from_len = sizeof(from_addr);

			int skfd_client = accept(pMsgSvr->skfd, (struct sockaddr*)&from_addr, &from_len);

			if ( skfd_client < 0 ) {
				HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] accept ERROR",__FUNCTION__,__LINE__);
				ret = -1 ;
				break;
			}
			#if SOCKET_DEBUG_MESSAGE
				HOST_PROF_LOG_PRINT(LEVEL_WARN, "[%s(%d)] accept from %s socket(%d)",__FUNCTION__,__LINE__,inet_ntoa(from_addr.sin_addr),skfd_client);
			#endif

			if ( appMsgSvrConfigTCP(skfd_client) < 0 ){
				closesocket(skfd_client);
				ret = -1 ;
				break;
			}
			int optval = 0 ;
			optval = lwip_fcntl(gMsgSvr.skfd, F_GETFL, 0);

			if (lwip_fcntl(gMsgSvr.skfd, F_SETFL, optval | O_NONBLOCK) != 0) {
				HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] lwip_fcntl ERROR",__FUNCTION__,__LINE__);
				return FAIL;
			}

			if (appMsgSvrSessionStart(pMsgSvr, skfd_client) != 0) {
				HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] appMsgSvrSessionStart ERROR",__FUNCTION__,__LINE__);
				closesocket(skfd_client);
				ret = -1 ;
			}
			break;
		}
	case APP_SOCKEL_EVT_ERROR:
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] APP_SOCKEL_EVT_ERROR",__FUNCTION__,__LINE__);
		break;
    case APP_SOCKEL_EVT_TIMEOUT:
        ret = 0 ;
        break;
	default:
		ret = -1 ;
		break;
	}
	return ret;
}


void appMsgSvrReadCallBack(void *buf, unsigned int len)
{
	HOST_PROF_LOG_PRINT(LEVEL_INFO, "[%s(%d)] TEST\n",__FUNCTION__,__LINE__);
	memdump(buf,(UINT32)len);
}


BOOL 
appSocketMsgSvrInit(int Port)
{
	int optval = 0 ;
	int ret = 0 ;
	struct sockaddr_in server;
    
	if ( thrAppMsgSvr  != NULL ) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] socket created ERROR",__FUNCTION__,__LINE__);
		return FAIL;
	}
	/* create socket */
	gMsgSvr.skfd  = socket(AF_INET, SOCK_STREAM, 0);
	if( gMsgSvr.skfd < 0 ) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] create socket ERROR",__FUNCTION__,__LINE__);
		return FAIL;
	}

	if ( appMsgSvrConfigTCP(gMsgSvr.skfd) < 0 )
		goto ERROR_EXIT;

	optval = lwip_fcntl(gMsgSvr.skfd, F_GETFL, 0);

	if (lwip_fcntl(gMsgSvr.skfd, F_SETFL, optval | O_NONBLOCK) != 0) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] lwip_fcntl ERROR",__FUNCTION__,__LINE__);
		return FAIL;
	}


    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(Port);
    ret = bind(gMsgSvr.skfd, (struct sockaddr*)&server, sizeof(server));

	if ( ret < 0 ) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] bind ERROR",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}
	
    ret = listen(gMsgSvr.skfd, 0);
	if ( ret < 0 ) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] listen ERROR",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}


	if ( gMsgSvr.sockel == NULL ) {
		AppSockelHandle h = AppSockelCreate(APP_MSGSVR_SOCKEL_MAX, appMsgSvrSelectInterruptible, appMsgSvrSelectWakeup);
		if (!h) {
			goto ERROR_EXIT;
		}
		gMsgSvr.sockel = h ;

	}

	if ( gMsgSvr.txQueue == 0 ) {
		if (sp5kOsQueueCreate(&gMsgSvr.txQueue, "MsgSvrTXQueue", 2, NULL, 32 * sizeof(struct MsgSvrData *)) != 0) {
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "[%s(%d)] sp5kOsQueueCreate",__FUNCTION__,__LINE__);
			goto ERROR_EXIT;
		}
	}
	if (!thrAppMsgSvr) {
		thrAppMsgSvr = sp5kOsThreadCreate("AppGlobSockEvtLoop", appMsgSvrGlobalRoutine , (UINT32) gMsgSvr.sockel
										 , 20, 0, 0
										 , TX_AUTO_START);
		if ( !thrAppMsgSvr ) {
			goto ERROR_EXIT;
		}
	}

	gMsgSvr.skel_timeout = 500 ;

	ret = AppSockelAddSocket(gMsgSvr.sockel, gMsgSvr.skfd, 
							 (AppSockelEventHandler)appMsgSvrListenHandler,
							 (void *)&gMsgSvr, TRUE 
							 ,gMsgSvr.skel_timeout , 0 );
	if ( ret < 0 ) {
		goto ERROR_EXIT;
	}

	return SUCCESS;

ERROR_EXIT:
	/* todo : free resource */
	closesocket(gMsgSvr.skfd);
	gMsgSvr.skfd = -1 ;
	return FAIL;
}

SINT32 appSocketMessageOperation(APP_MESSAGE_SVR sel , ...)
{
	SINT32 ret = FAIL;
	UINT32 paraTot;
//	UINT32 para1, para2, para3, para4;
//	para4 = para3 = para2 = para1 = 0;
	va_list args;

	switch(sel)
	{
		case APP_MESSAGE_SVR_START:     	 		paraTot = 1; break;
		case APP_MESSAGE_SVR_STOP:     	 		 	paraTot = 0; break;
		case APP_MESSAGE_SVR_SEND:     	 		 	paraTot = 2; break;
        case APP_MESSAGE_SESSION_TIMEOUT_CB:     	paraTot = 1; break;
		default: paraTot = 0; break;
	}

	if(paraTot) {
		va_start(args, sel);
		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "sel=%d", sel);
		UINT32 para1;
		para1 = va_arg(args, UINT32);
		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "para1=%d(0x%X)", para1, para1);
		if(paraTot >= 2) {
			UINT32 para2;
			para2 = va_arg(args, UINT32);
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "para2=%d(0x%X)", para2, para2);
			if(paraTot >= 3) {
				UINT32 para3;
				para3 = va_arg(args, UINT32);
				HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "para3=%d(0x%X)", para3, para3);
				if(paraTot >= 4) {
					UINT32 para4;
					para4 = va_arg(args, UINT32);
					HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "para4=%d(0x%X)", para4, para4);
				}
			}
		}
	}

	switch(sel)
	{
		case APP_MESSAGE_SVR_START:
			/* Need CallBack Function to Receive Packet */
			if ( appMsgSvrReadCb == NULL ) {
				if ( SUCCESS == appSocketMsgSvrInit(PORT_MESSAGE_SVR)) {
					if ( (void *)para1 != NULL  ) {
						appMsgSvrReadCb = (void *)para1;
					} else {
						appMsgSvrReadCb = (void *)&appMsgSvrReadCallBack ;
					}
					ret = SUCCESS;
				} 
			}

			break;
        case APP_MESSAGE_SESSION_TIMEOUT_CB:
            appMsgMessTimeOutCb = (void *)para1;
            ret = SUCCESS;
            break;
		case APP_MESSAGE_SVR_STOP:
			break;
		case APP_MESSAGE_SVR_SEND:
			ret =  appMsgSvrSessionSend((void *) para1 , (unsigned int) para2);
			break;

		default:
			ret = FAIL;
			break;
	}

	if(paraTot) va_end(args);

	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "set sel:%d 0x%x",sel ,ret);

	return ret;
}



