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

#ifndef _APP_MMFACE_DETECT_H_
#define _APP_MMFACE_DETECT_H_

#include "sp5k_ftrack_api.h"
#include "sp5k_otrack_api.h"

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define MAX_SUPPORT_FACE_NUM	10
#define MAX_SUPPORT_OBJECT_NUM	5		

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {	
	FACE_DETECT_ONLY    = 0x00,
	SMILE_DETECT,
	BLINK_DETECT,
	OPTION_TOTAL
} appFdIniOption_e;

typedef enum {
	FD_TYPE_FACE 		= 0x00,
	FD_TYPE_LEFT  		= 0x01,
	FD_TYPE_RIGHT		= 0x02,
	FD_TYPE_SLANT		= 0x04,
	FD_TYPE_MAINFACE	= 0x10,
	FD_TYPE_BLINK		= 0x40,
	FD_TYPE_SMILE		= 0x80,
} faceDectType_t;


typedef struct {
	UINT32 nStartX;
	UINT32 nStartY;
	UINT32 nSize;
	UINT8 attr;
	UINT8 isSmile;
	UINT16 angle;
} appFaceInfo_t;


typedef struct appFdResult_s{
	UINT32 totalFaceNum;
	UINT32 mainFace;
	appFaceInfo_t rectFace[MAX_SUPPORT_FACE_NUM];
}appFdResult_t;

typedef enum {
	OD_TYPE_FACE 		= 0x00,
	OD_TYPE_LEFT  		= 0x01,
	OD_TYPE_RIGHT		= 0x02,
	OD_TYPE_SLANT		= 0x04,
	OD_TYPE_MAIN		= 0x10,
	OD_TYPE_BLINK		= 0x40,
	OD_TYPE_SMILE		= 0x80,
} objectDectType_t;

typedef struct {
	UINT32 nStartX;
	UINT32 nStartY;
	UINT32 nSize;
	UINT8 attr;
	UINT8 isSmile;
	UINT16 angle;
} appObjectInfo_t;


typedef struct appOdResult_s{
	UINT32 totalObjectNum;
	UINT32 mainObject;
	appObjectInfo_t rectObj[MAX_SUPPORT_OBJECT_NUM];
}appOdResult_t;

typedef UINT32 appSmileDetectCBF_t(UINT32 newfacenum,  appFaceInfo_t *newface);

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
 
/*Face detect*/
void appFdCfgInit(void);
void appFdCfgSet(appFdIniOption_e option);
SINT32 appFdResultGet(UINT32 param,UINT32 *pnface,sp5kFaceTrackFaceRes_t *presult);
void appFdDataUpdate(UINT32 nface, sp5kFaceTrackFaceRes_t facelist[]);
void appFd_FaceRectUpdateMsgSend(void);

/*Object detect*/
void appOdDataUpdate(UINT32 obj, sp5kObjTrackResult_t objlist[]);
void appOd_ObjectRectUpdateMsgSend(void);

/*Face Beauty*/
BOOL appFbFaceGet(void);

#endif  /* _APP_MMFACE_DETECT_H_ */

