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

#ifndef _APP_LOAD_FD_H_
#define _APP_LOAD_FD_H_

#ifdef __cplusplus
extern "C" {
#endif
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define USE_APP_FACETRACK
#define APP_FACETRACK_FILE		    RO_RES_PATH"APP\\FACETRCK.BIN"

#define MAX_SUPPORT_FACE_NUM	    5
#define MAX_SUPPORT_OBJECT_NUM	    5		/* Add by sc.liu */
#define SIZE_FACE_DETECT_WIDTH	    160
#define SIZE_FACE_DETECT_HEIGHT	    120

#define SIZE_SMILE_DETECT_WIDTH	    320
#define SIZE_SMILE_DETECT_HEIGHT	240

#define VIEW_SCREEN_WIDTH		    320
#define VIEW_SCREEN_HEIGHT	        240

typedef enum {	
	FACE_DETECT_ONLY = 0x00,
	SMILE_DETECT,
	//BLINK_DETECT,
	OPTION_TOTAL
} appFdIniOption_e;

typedef struct {
	SINT16 nLeft;
	SINT16 nTop;
	SINT16 nRight;
	SINT16 nBottom;
	BOOL isSmile;
} appFaceInfo_t;

typedef struct appFdResult_s{
	UINT32 totalFaceNum;
	UINT32 mainFace;
	appFaceInfo_t rectFace[MAX_SUPPORT_FACE_NUM];
}appFdResult_t;

typedef struct {
	SINT16 nLeft;
	SINT16 nTop;
	SINT16 nRight;
	SINT16 nBottom;
	BOOL isSmile;
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
#define appFdCreate			        (*(gFdOutFunc.fn_FD_Create))
#define appFdDestroy			    (*(gFdOutFunc.fn_FD_Destroy))
#define appFdProcStart			    (*(gFdOutFunc.fn_FD_Start))
#define appFdProcPause			    (*(gFdOutFunc.fn_FD_Pause))
#define appFdProcStop			    (*(gFdOutFunc.fn_FD_Stop))
#define appFdCoreProcess			(*(gFdOutFunc.fn_FD_CoreProcess))
#define appFdFaceDataGet			(*(gFdOutFunc.fn_FD_FaceDataGet))
#define appSmileDetectCallbackSet	(*(gFdOutFunc.fn_FD_SmileDetectCallbackSet))

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct fdOutFunc_s {
	UINT32 (*fn_FD_Create)(appFdIniOption_e iniFlag);
	void (*fn_FD_Destroy)(void);
	void (*fn_FD_Start)(void);
	void (*fn_FD_Pause)(void);
	void (*fn_FD_Stop)(void);
	UINT32 (*fn_FD_CoreProcess)(UINT32 addr, UINT32 zoomratio);
	UINT32 (*fn_FD_FaceDataGet)(appFdResult_t *fdResult);
	void   (*fn_FD_SmileDetectCallbackSet)(appSmileDetectCBF_t *pfun);
} fdOutFunc_t;

typedef struct odOutFunc_s {
	UINT32 (*on_OD_Create)(appOdIniOption_e iniFlag);
	void (*on_OD_Destroy)(void);
	void (*on_OD_Start)(void);
	void (*on_OD_Pause)(void);
	void (*on_OD_Stop)(void);
	UINT32 (*on_OD_CoreProcess)(UINT32 addr, UINT32 zoomratio);
	UINT32 (*on_OD_ObjectDataGet)(appOdResult_t *odResult);
	void   (*on_OD_SmileDetectCallbackSet)(appSmileDetectCBF_t *pfun);
} odOutFunc_t;

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
extern fdOutFunc_t gFdOutFunc;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 fdAppInit(const char *fileName);
UINT32 odAppInit(const char *fileName);		/* sc.liu */
#ifdef __cplusplus
}
#endif

#endif  /* _APP_LOAD_FD_H_ */



