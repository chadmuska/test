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
 *  File name	: 	app_load_facedetect.c
 *  Function 	: 	load face detect	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  			:2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#define HOST_DBG 0
#include <stdio.h>
#include <math.h>
#include "app_com_def.h"
#include "app_load_facedetect.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
/* typedef struct odInFunc_s {
	UINT32 (*on_ros_semaphore_create)(UINT8 *name, UINT32 init_val);
	UINT32 (*on_ros_semaphore_get)(UINT32 semaphore_id, UINT32 timeout);
	UINT32 (*on_ros_semaphore_put)(UINT32 semaphore_id);
	UINT32 (*on_ros_semaphore_destroy)(UINT32 semaphore_id);
	void   (*on_osAllInfoDisplay)(UINT32 mode);
	void * (*on_sp5kMallocCache)(UINT32 size);
	void * (*on_sp5kMalloc)(UINT32 size);
	void   (*on_sp5kFree)(void *pbuf);
	void * (*on_snapMemRawBufferAlloc)(UINT32 size);
	void   (*on_snapMemRawBufferFree)(void *addr);
	UINT32 (*on_tmrMsTimeGet)(void);		
	UINT32 (*on_sp5kStillCapJobInfo)(UINT32 *pwaitJob, UINT32 *pavailableJob);
	UINT32 (*on_sp5kTimeDelay)(UINT32 unit, UINT32 count);
	UINT32 (*on_sp5kHostMsgSend)(UINT32 msg, UINT32 param);
	UINT32 (*on_fsFileOpen)(const UINT8 *fileName, SINT32 flag);
	UINT32 (*on_fsFileDelete)(const UINT8 *fileName);
	UINT32 (*on_fsFileRead)(UINT32 od, UINT8 *buf, UINT32 size);
	UINT32 (*on_fsFileClose)(UINT32 od);
	UINT32 (*on_fsFileWrite)(UINT32 od, UINT8 *buf, UINT32 size);
	UINT32 (*on_fsFileTell)(UINT32 od);
	UINT32 (*on_fsFileSeek)(UINT32 od, SINT32 offset, UINT32 mode);
	UINT32 (*on_fsFileSizeGet)(UINT32 od);	
	void  (*on_appStill_GetCurSize)(UINT32* w, UINT32* h);
	double (*on_exp)(double x);
	double (*on_pow)(double x, double y);
	double (*on_sqrt)(double x);
} odInFunc_t; */

/* Add by sc.liu */


/* typedef struct fdInFunc_s {
	UINT32 (*fn_ros_semaphore_create)(UINT8 *name, UINT32 init_val);
	UINT32 (*fn_ros_semaphore_get)(UINT32 semaphore_id, UINT32 timeout);
	UINT32 (*fn_ros_semaphore_put)(UINT32 semaphore_id);
	UINT32 (*fn_ros_semaphore_destroy)(UINT32 semaphore_id);
	void   (*fn_osAllInfoDisplay)(UINT32 mode);
	void * (*fn_sp5kMallocCache)(UINT32 size);
	void * (*fn_sp5kMalloc)(UINT32 size);
	void   (*fn_sp5kFree)(void *pbuf);
	void * (*fn_snapMemRawBufferAlloc)(UINT32 size);
	void   (*fn_snapMemRawBufferFree)(void *addr);
	UINT32 (*fn_tmrMsTimeGet)(void);		
	UINT32 (*fn_sp5kStillCapJobInfo)(UINT32 *pwaitJob, UINT32 *pavailableJob);
	UINT32 (*fn_sp5kTimeDelay)(UINT32 unit, UINT32 count);
	UINT32 (*fn_sp5kHostMsgSend)(UINT32 msg, UINT32 param);
	UINT32 (*fn_fsFileOpen)(const UINT8 *fileName, SINT32 flag);
	UINT32 (*fn_fsFileDelete)(const UINT8 *fileName);
	UINT32 (*fn_fsFileRead)(UINT32 fd, UINT8 *buf, UINT32 size);
	UINT32 (*fn_fsFileClose)(UINT32 fd);
	UINT32 (*fn_fsFileWrite)(UINT32 fd, UINT8 *buf, UINT32 size);
	UINT32 (*fn_fsFileTell)(UINT32 fd);
	UINT32 (*fn_fsFileSeek)(UINT32 fd, SINT32 offset, UINT32 mode);
	UINT32 (*fn_fsFileSizeGet)(UINT32 fd);	
	void  (*fn_appStill_GetCurSize)(UINT32* w, UINT32* h);
	double (*fn_exp)(double x);
	double (*fn_pow)(double x, double y);
	double (*fn_sqrt)(double x);
} fdInFunc_t; */

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT32 ros_semaphore_create(UINT8 *name, UINT32 init_val);
extern UINT32 ros_semaphore_get(UINT32 semaphore_id, UINT32 timeout);
extern UINT32 ros_semaphore_put(UINT32 semaphore_id);
extern UINT32 ros_semaphore_destroy(UINT32 semaphore_id);
extern void   osAllInfoDisplay(UINT32 mode);
extern void * snapMemRawBufferAlloc(UINT32 size);
extern void   snapMemRawBufferFree(void *addr);
extern UINT32 tmrMsTimeGet(void);
extern UINT32 sp5kStillCapJobInfo(UINT32 *pwaitJob, UINT32 *pavailableJob);
extern UINT32 sp5kTimeDelay(UINT32 unit, UINT32 count);
extern void * sp5kMallocCache(UINT32 size);
extern void * sp5kMalloc(UINT32 size);
extern void   sp5kFree(void *pbuf);
extern UINT32 sp5kHostMsgSend(UINT32 msgid,UINT32 param);
extern UINT32 fsFileOpen(const UINT8 *fileName, SINT32 flag);
extern UINT32 fsFileDelete(const UINT8 *fileName);
extern UINT32 fsFileRead(UINT32 fd, UINT8 *buf, UINT32 size);
extern UINT32 fsFileClose(UINT32 fd);
extern UINT32 fsFileWrite(UINT32 fd, UINT8 *buf, UINT32 size);
extern UINT32 fsFileTell(UINT32 fd);
extern UINT32 fsFileSeek(UINT32 fd, SINT32 offset, UINT32 mode);
extern UINT32 fsFileSizeGet(UINT32 fd);
extern void appStill_GetCurSize(UINT32* w, UINT32* h);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
odOutFunc_t gOdOutFunc;
static const odInFunc_t gOdInFunc = {	
	ros_semaphore_create,
	ros_semaphore_get,
	ros_semaphore_put,
	ros_semaphore_destroy,
	osAllInfoDisplay,
	sp5kMallocCache,
	sp5kMalloc,
	sp5kFree,
	snapMemRawBufferAlloc,
	snapMemRawBufferFree,	
	tmrMsTimeGet,
	sp5kStillCapJobInfo,
	sp5kTimeDelay,
	sp5kHostMsgSend,
	fsFileOpen,
	fsFileDelete,
	fsFileRead,
	fsFileClose,
	fsFileWrite,
	fsFileTell,
	fsFileSeek,
	fsFileSizeGet,	
	appStill_GetCurSize,	
	exp,
	pow,
	sqrt
};

fdOutFunc_t gFdOutFunc;

static const fdInFunc_t gFdInFunc = {	
	ros_semaphore_create,
	ros_semaphore_get,
	ros_semaphore_put,
	ros_semaphore_destroy,
	osAllInfoDisplay,
	sp5kMallocCache,
	sp5kMalloc,
	sp5kFree,
	snapMemRawBufferAlloc,
	snapMemRawBufferFree,	
	tmrMsTimeGet,
	sp5kStillCapJobInfo,
	sp5kTimeDelay,
	sp5kHostMsgSend,
	fsFileOpen,
	fsFileDelete,
	fsFileRead,
	fsFileClose,
	fsFileWrite,
	fsFileTell,
	fsFileSeek,
	fsFileSizeGet,	
	appStill_GetCurSize,	
	exp,
	pow,
	sqrt
};

extern UINT32 appBufAddress;
extern UINT32 appBufSize;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*--------------------------------------------------------------------------*
* Function name : checkData
* Function		: function for Check uart2 buffer data
* Return value	: result (UINT32 SUCCESS/FAIL)
* Parameter1	: fileName buffer pointer (--) 
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
UINT32
fdAppInit(
	const char *fileName
)
{
	DBG_PRINT("[%s] fileName:%s\n", __FUNCTION__, fileName);

	if (appLoadIsInit(fileName) == FAIL) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "loadfd: [%s] line %d: appBufAddress=0x%x, appBufSize=0x%x",
			__FUNCTION__, __LINE__, appBufAddress, appBufSize);
		appLoadBinBufSet(appBufAddress, appBufSize);

		if (appLoadExecutableBin(fileName) == FAIL) {
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "loadfd: [%s] line %d: appLoadExecutableBin failed", 
				__FUNCTION__, __LINE__);
			return FAIL;
		}

		if (appLoadMainInit((void *)&gFdInFunc, (void *)&gFdOutFunc) == FAIL) {
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "loadfd: [%s] line %d: appLoadMainInit failed", 
				__FUNCTION__, __LINE__);
			return FAIL;
		}
	}

	profLogAdd(0, "[host] load fd: fdAppInit fin");

	return SUCCESS;
}

UINT32
odAppInit(
	const char *fileName
)
{
	DBG_PRINT("[%s] fileName:%s\n", __FUNCTION__, fileName);

	if (appLoadIsInit(fileName) == FAIL) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR, "load fd: [%s] line %d: appBufAddress=0x%x, appBufSize=0x%x\n",
			__FUNCTION__, __LINE__, appBufAddress, appBufSize);
		appLoadBinBufSet(appBufAddress, appBufSize);

		if (appLoadExecutableBin(fileName) == FAIL) {
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "load fd: [%s] line %d: appLoadExecutableBin failed\n", 
				__FUNCTION__, __LINE__);
			return FAIL;
		}

		if (appLoadMainInit((void *)&gOdInFunc, (void *)&gOdOutFunc) == FAIL) {
			HOST_PROF_LOG_PRINT(LEVEL_ERROR, "load fd: [%s] line %d: appLoadMainInit failed\n", 
				__FUNCTION__, __LINE__);
			return FAIL;
		}
	}

	profLogAdd(0, "[host] load fd: fdAppInit fin");

	return SUCCESS;
}

