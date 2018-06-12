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
 *	File name    : App_facedetect_ex.c
 *	Function     : About face detect(FD) implementation.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.06  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#define HOST_DBG 0

#include "app_com_api.h"
#include "app_com_def.h"
#include "sp5k_ftrack_api.h"
#include "sp5k_fbeauty_api.h"
#include "api/sp5k_capture_api.h"
#include "api/sp5k_utility_api.h"

#include "app_mmface_detect.h"
#include "app_still.h"
#include "app_video.h"
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

static UINT32 ftrackFaceData;
static appFdResult_t faceInfo;
static UINT32 otrackObjectData;
static appOdResult_t objectInfo;
static UINT32 BlinkDetectSem;
UINT8 *BlKChkyuvBuf;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 ros_semaphore_create(char *name, UINT32 init_val);
UINT32 ros_semaphore_put(UINT32 semaphore_id);
UINT32 ros_semaphore_get(UINT32 semaphore_id, UINT32 timeout);
UINT32 View_CoordVisible(UINT32 x, UINT32 y);

/*-------------------------------------------------------------------------
 *  File Name : View_CoordVisible
 *------------------------------------------------------------------------*/
UINT32 View_CoordVisible(UINT32 x, UINT32 y)
{
	UINT32 w,h,fr,y0;
	extern UINT8 gVideoSizeTag ;

	if(IS_VIDEO_MODE)
	{
		appVideoSizeGet(gVideoSizeTag,&w,&h,&fr);
	}
	else
	{
		appStill_GetCurSize( &w, &h);
	}
	y0 = (240 - 240 * (4 * h) / (3 * w)) / 2;
	return y0<=y && y<=(240-y0);
}

/* face beauty function */
/*-------------------------------------------------------------------------
 *  File Name : appFbFaceGet
 *------------------------------------------------------------------------*/
BOOL appFbFaceGet(void)
{
    return (faceInfo.totalFaceNum)?TRUE:FALSE;
}

/* blink detect function */
/*-------------------------------------------------------------------------
 *  File Name : _fd_BlinkDetectSet
 *------------------------------------------------------------------------*/
void
_fd_BlinkDetectSet(
	UINT32 set
)
{
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_BLINK_DETECT,set);
	/*1:one eyes blink as blink,2:two eyes blink as blink*/
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_BLINK_NUM,1);

}

/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkingDetectCheck
 *------------------------------------------------------------------------*/
void appFdBlinkDetectSemInit(void)
{
	if ( BlinkDetectSem != (UINT32)NULL )
	{
		return;
	}
	BlinkDetectSem = ros_semaphore_create( (char *)"app_BlinkingDetect", 0 );
	if ( !BlinkDetectSem )
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "fd: %s %d", __FUNCTION__, __LINE__);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkDetectSemPut
 *------------------------------------------------------------------------*/
void appFdBlinkDetectSemPut(void)
{
	if (BlinkDetectSem)
	{
		ros_semaphore_put( BlinkDetectSem );
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkDetectSemGet
 *------------------------------------------------------------------------*/
void appFdBlinkDetectSemGet(void)
{
	if (BlinkDetectSem==0)
	{
		appFdBlinkDetectSemInit();
	}
	ros_semaphore_get( BlinkDetectSem, -1 );
}

/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkDetectSet
 *------------------------------------------------------------------------*/
void
appFdBlinkDetectSet(
	UINT32 set
)
{
	_fd_BlinkDetectSet(set);
}

/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkingDetectCheck
 *------------------------------------------------------------------------*/
UINT32
appFdBlinkingDetectCheck(
	UINT32 yuvAddr,
	UINT32 qvgaAddr,
	void * psSizeInfo
)
{
	#define W 640
	#define H 480

	sp5kYuvCbImgSize_t *pYuvSize=(sp5kYuvCbImgSize_t *)psSizeInfo;
	UINT32 srcAddr,width,height, qvw, qvh;;
	sp5kFaceTrackYuvBuf_t bufInfo;

	srcAddr = yuvAddr;
	width = pYuvSize->roiWidth;
	height = pYuvSize->roiHeight;

	appFdBlinkDetectSet(TRUE);

	/*
		assume only width is always large than height.
		blink need multiple of 8 pixle in w and h.
	*/
 	qvw = (UINT32)(W+4)&0xfffffff0;
 	qvh = (UINT32)(W*height/width+4)&0xfffffff0;
	BlKChkyuvBuf = sp5kYuvBufferAlloc(qvw,qvh);
	if (BlKChkyuvBuf == NULL)
	{
		appFdBlinkDetectSet(FALSE);
        HOST_PROF_LOG_ADD(LEVEL_ERROR,"fd: blink memory allocate fail");
		return FAIL;
	}

	bufInfo.yuvaddr = BlKChkyuvBuf;
	bufInfo.yuvw = qvw;
	bufInfo.yuvh = qvh;
	bufInfo.dispx = 0;
	bufInfo.dispy = 0;
	bufInfo.dispw =0;
	bufInfo.disph = 0;

	appyuvScale((UINT8 *)srcAddr, width, height, BlKChkyuvBuf, qvw, qvh);
	if (sp5kFaceTrackYuvBufStart(0, &bufInfo, 0, 0) != SUCCESS)
	{
        HOST_PROF_LOG_ADD(LEVEL_ERROR,"fd: blink sp5kFaceTrackYuvBufStart fail");
		return FAIL;
	}

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appFdBlinkDetectCheckEnd
 *------------------------------------------------------------------------*/
void
appFdBlinkDetectCheckEnd(
	void
)
{
	if (BlKChkyuvBuf!=NULL)
	{
		sp5kYuvBufferFree(BlKChkyuvBuf);
		BlKChkyuvBuf = NULL;
	}
}

/* Smile detect function */
 /*-------------------------------------------------------------------------
 *  File Name : _fd_SmileDetectSet
 *------------------------------------------------------------------------*/
void
_fd_SmileDetectSet(
	UINT32 set
)
{
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_SMILE_DETECT,set);
}

/* Object detect function */
 /*-------------------------------------------------------------------------
 *  File Name : _od_OdSemInit
 *------------------------------------------------------------------------*/
void
_od_OdSemInit(
	void
)
{
	if ( otrackObjectData != (UINT32)NULL ) {
		return;
	}
	otrackObjectData = ros_semaphore_create( (char *)"app_mmOd", 1 );
	if ( !otrackObjectData ) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "fd: %s %d", __FUNCTION__, __LINE__);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : _od_OdSemDataAlloc
 *------------------------------------------------------------------------*/
void
_od_OdSemDataAlloc(
	void
)
{
	if (otrackObjectData==0){
		_od_OdSemInit();
	}
	ros_semaphore_get( otrackObjectData, -1 );
}

/*-------------------------------------------------------------------------
 *  File Name : _od_OdSemDataFree
 *------------------------------------------------------------------------*/
void
_od_OdSemDataFree(
	void
)
{
	if (otrackObjectData){
		ros_semaphore_put( otrackObjectData );
	}
}

 /*-------------------------------------------------------------------------
 *  File Name : _odBinDynamicLoad
 *------------------------------------------------------------------------*/
UINT32
_odBinDynamicLoad(
	void
)
{
#ifdef HW_FACETRACK

#else
#ifdef USE_APP_FACETRACK
	if (odAppInit(APP_FACETRACK_FILE) == FAIL) {
		HOST_PROF_LOG_PRINT(LEVEL_ERROR,"fd: [%s] line %d: ftAppInit failed\n", __FUNCTION__, __LINE__);
		return FAIL;
	}
#endif
#endif
	return SUCCESS;
}

 /*-------------------------------------------------------------------------
  *  File Name : _od_OdObjectDataGet
  *------------------------------------------------------------------------*/
 UINT32
 _od_OdObjectDataGet(
	 appOdResult_t *odResult
 )
 {
	 _od_OdSemDataAlloc();
	 memcpy(odResult, &objectInfo, sizeof(appOdResult_t));
	 _od_OdSemDataFree();
	 return SUCCESS;
 }
#if FD_TEST
 /*-------------------------------------------------------------------------
  *  File Name : appOdObjectDataGetEx
  *------------------------------------------------------------------------*/
 UINT32
 appOdObjectDataGetEx(

	 appObjectInfo_t *odResult
 )
 {
	#ifdef HW_FACETRACK
	 return(_od_OdObjectDataGet(odResult));
	#else
	 if(_odBinDynamicLoad() != SUCCESS) return FAIL;
	 return(appOdObjectDataGet(odResult));
	#endif
 }

/*-------------------------------------------------------------------------
 *  File Name : appObdResultGet
 *------------------------------------------------------------------------*/
SINT32
appObdResultGet(
	UINT32 param, UINT32 *pnobj, sp5kObjTrackResult_t *presult
)
{
	return sp5kObjTrackResultGet(param, pnobj, presult);
}
#endif

/*-------------------------------------------------------------------------
 *  File Name : appOdDataUpdate
 *------------------------------------------------------------------------*/
#define OD_UPDATE_TEST 1
void
appOdDataUpdate(
	UINT32 obj, sp5kObjTrackResult_t objlist[]
)
{
	UINT32 i,j;
	_od_OdSemDataAlloc();
	for (i=j=0 ; i<obj ; i++) {
		#if OD_UPDATE_TEST
 		objectInfo.rectObj[j].nStartX= objlist[i].pos.elem.x * 320 / 1024;
    	objectInfo.rectObj[j].nStartY = objlist[i].pos.elem.y * 240 / 1024;
    	objectInfo.rectObj[j].nSize= objlist[i].pos.elem.sz * 320 / 1024;

	if (!View_CoordVisible(0, objectInfo.rectObj[j].nStartY) || !View_CoordVisible(0, objectInfo.rectObj[j].nStartY+objectInfo.rectObj[j].nSize))
			continue;
		#endif
		j++;
	}
	objectInfo.totalObjectNum =j;

	_od_OdSemDataFree();
}

/*-------------------------------------------------------------------------
 *  File Name : appOd_ObjectRectUpdateMsgSend
 *------------------------------------------------------------------------*/
void
appOd_ObjectRectUpdateMsgSend(
	void
)
{
	sp5kHostMsgSend(APP_UI_MSG_OBJECT_DETECT_OSD_UPDATE,0x00);
}

/* face detect function */
/*-------------------------------------------------------------------------
 *  File Name : _fd_MaxFaceSet
 *------------------------------------------------------------------------*/
void
_fd_MaxFaceSet(
	UINT32 val
)
{
	// set max count of resulted faces
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_PROC_MAX_FACE,val);
}

/*-------------------------------------------------------------------------
 *  File Name : _fd_EnableDetectSet
 *------------------------------------------------------------------------*/
void
_fd_EnableDetectSet(
	UINT32 enable
)
{
	sp5kFaceTrackModeSet(enable);
}

/*-------------------------------------------------------------------------
 *  File Name : _fd_FdSemInit
 *------------------------------------------------------------------------*/
void
_fd_FdSemInit(
	void
)
{

	if ( ftrackFaceData != (UINT32)NULL )
	{
		return;
	}
	ftrackFaceData = ros_semaphore_create( (char *)"app_mmFd", 1 );
	if ( !ftrackFaceData )
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "fd: %s %d", __FUNCTION__, __LINE__);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : _fd_FdSemDataAlloc
 *------------------------------------------------------------------------*/
void
_fd_FdSemDataAlloc(
	void
)
{
	if (ftrackFaceData==0)
	{
		_fd_FdSemInit();
	}
	ros_semaphore_get( ftrackFaceData, -1 );
}

/*-------------------------------------------------------------------------
 *  File Name : _fd_FdSemDataFree
 *------------------------------------------------------------------------*/
void
_fd_FdSemDataFree(
	void
)
{
	if (ftrackFaceData)
	{
		ros_semaphore_put( ftrackFaceData );
	}
}

/*-------------------------------------------------------------------------
 *  File Name : _fd_FdFaceDataGet
 *------------------------------------------------------------------------*/
UINT32
_fd_FdFaceDataGet(
	appFdResult_t *fdResult
)
{
	_fd_FdSemDataAlloc();
	memcpy(fdResult, &faceInfo, sizeof(appFdResult_t));
	_fd_FdSemDataFree();
	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name    : appFdCreateEx
* Function         : face detect init
* Return value	   : SUCCESS
* Parameter1	   : iniFlag (appFdIniOption_e)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT32
appFdCreateEx(
	appFdIniOption_e iniFlag
)
{
	appFdCfgSet(iniFlag);
    return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appFdDestroyEx
 *------------------------------------------------------------------------*/
void
appFdDestroyEx(
	void
)
{
	_fd_EnableDetectSet(FALSE);
}

/*-------------------------------------------------------------------------
 *  File Name : appFdProcStartEx
 *------------------------------------------------------------------------*/
void
appFdProcStartEx(
	void
)
{
	_fd_EnableDetectSet(TRUE);
}

/*-------------------------------------------------------------------------
 *  File Name : appFdProcPauseEx
 *------------------------------------------------------------------------*/
void
appFdProcPauseEx(
	void
)
{
	_fd_EnableDetectSet(FALSE);
}

/*-------------------------------------------------------------------------
 *  File Name : appFdProcStopEx
 *------------------------------------------------------------------------*/
void
appFdProcStopEx(
	void
)
{
	_fd_EnableDetectSet(FALSE);
}

/*-------------------------------------------------------------------------
 *  File Name : appFdResultGet
 *------------------------------------------------------------------------*/
SINT32
appFdResultGet(
	UINT32 param,UINT32 *pnface,sp5kFaceTrackFaceRes_t *presult
)
{
	return  sp5kFaceTrackResultGet(param,pnface,presult);
}

/*-------------------------------------------------------------------------
 *  File Name : appFdFaceDataGetEx
 *------------------------------------------------------------------------*/
UINT32
appFdFaceDataGetEx(

	appFdResult_t *fdResult
)
{
	return(_fd_FdFaceDataGet(fdResult));
}

/*-------------------------------------------------------------------------
 *  File Name : appFdDataUpdate
 *------------------------------------------------------------------------*/
void
appFdDataUpdate(
	UINT32 nface, sp5kFaceTrackFaceRes_t facelist[]
)
{
	UINT32 i,j;

	_fd_FdSemDataAlloc();
	for (i=j=0 ; i<nface ; i++)
	{
		faceInfo.rectFace[j].nStartX = facelist[i].x * 320 / 1024;
		faceInfo.rectFace[j].nStartY = facelist[i].y * 240 / 1024;
		faceInfo.rectFace[j].nSize = facelist[i].size * 320 / 1024;
		faceInfo.rectFace[j].attr = (facelist[i].attr & 0xff000000) >> 24;
#if 0
        //lester test foe face beauty
        fbInfo[i].x     = facelist[i].x;
       	fbInfo[i].y     = facelist[i].y;
       	fbInfo[i].size  = facelist[i].size;
#endif
		//faceInfo.rectFace[j].angle = (facelist[i].attr & 0x3f);
		if (!View_CoordVisible(0, faceInfo.rectFace[j].nStartY)
			|| !View_CoordVisible(0, faceInfo.rectFace[j].nStartY+faceInfo.rectFace[j].nSize))
		{
			continue;
		}
		if ((faceInfo.rectFace[j].attr & 0x10)==FD_TYPE_MAINFACE)
		{
			faceInfo.mainFace = j;
            DBG_PRINT("fd: main face=%d ,%d\n",i,faceInfo.rectFace[j].attr);
		}
		if ((faceInfo.rectFace[j].attr & 0x80)==FD_TYPE_SMILE)
		{
			faceInfo.rectFace[j].isSmile=1;
		}
		else
		{
			faceInfo.rectFace[j].isSmile=0;
		}
		j++;

        #if 0
        DBG_PRINT("fd: attr = %x,idx:%d\n", faceInfo.rectFace[i].attr,i);
        DBG_PRINT("fd: nStartX = %x,j:%d\n", faceInfo.rectFace[j].nStartX,j);
        DBG_PRINT("fd: nStartY = %x,j:%d\n", faceInfo.rectFace[j].nStartY,j);
        DBG_PRINT("fd: nSize = %x,j:%d\n", faceInfo.rectFace[j].nSize,j);
        #endif

	}

	faceInfo.totalFaceNum =j;
	_fd_FdSemDataFree();
}

/*-------------------------------------------------------------------------
 *  File Name : appFd_FaceRectUpdateMsgSend
 *------------------------------------------------------------------------*/
void
appFd_FaceRectUpdateMsgSend(
	void
)
{
	sp5kHostMsgSend(APP_UI_MSG_FACE_DETECT_OSD_UPDATE,0x00);
}

UINT32 FaceTrackResultCallback(UINT32 param, UINT32 rsvd, void *rsvd2)
{	
	sp5kFaceTrackFaceRes_t facelist[MAX_SUPPORT_FACE_NUM] = {{0}};
	UINT32 ret = 0, nface = 0;
	ret = sp5kFaceTrackResultGet(param, &nface, facelist);

	if (ret == SUCCESS && nface) {
		UINT32 i;
		printf("nface = %u:\n", nface);
		for (i = 0 ; i < nface ; i++) {
			printf("(%d,%d) size=%d attr=%x\n", facelist[i].x, facelist[i].y, facelist[i].size, facelist[i].attr);
		}
	}

	return ret;
}


/*-------------------------------------------------------------------------
 *  File Name : appFdCfgInit
 *------------------------------------------------------------------------*/
void
appFdCfgInit(
	void
)
{
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_FD_DONE_CB, (UINT32)FaceTrackResultCallback);
	
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_KEY_FACE, ENABLE);
	/* please confirm the face color is normal, no image effect */
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_CHECK_COLOR, ENABLE);
	/* Set scan interval (in preview frame).If not init, the default period value is 2. */
	sp5kFaceTrackCfgSet(SP5K_FACE_TRACK_CFG_ACCUM_PERIOD, 2);
	/* configuration should be ready before PV */
}

/*-------------------------------------------------------------------------
 *  File Name : appFdCfgInit
 *------------------------------------------------------------------------*/
void
appFdCfgSet(
	appFdIniOption_e option
)
{
	appFdCfgInit();

    switch(option)
    {
        case FACE_DETECT_ONLY:
            _fd_MaxFaceSet(MAX_SUPPORT_FACE_NUM); // max face number
            _fd_SmileDetectSet(FALSE);
            _fd_BlinkDetectSet(FALSE);
        	break;

        case SMILE_DETECT:
            _fd_MaxFaceSet(1); // max face number
            _fd_SmileDetectSet(TRUE);
            _fd_BlinkDetectSet(FALSE);
            break;
        case BLINK_DETECT:
            _fd_MaxFaceSet(1); // max face number
            _fd_SmileDetectSet(FALSE);
            _fd_BlinkDetectSet(TRUE);
            appFdBlinkDetectSemInit();
        	break;
		
		default:
	  		break;
    }
}

