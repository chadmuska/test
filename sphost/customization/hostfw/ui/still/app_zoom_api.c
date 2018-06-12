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
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "app_com_api.h"

#include "app_dbg_api.h"
#include "app_util.h"
#include "app_state_ctrl.h"
#include "app_zoom_api.h"
#include "app_ui_msg.h"
#include "app_audio.h"
#include "app_still.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define APP_DZOOM_RATIO_TOTAL_NUM	(sizeof(gdzoomRatioArray)/sizeof(UINT32))
#define APP_DZOOM_MAX_RATIO_INDEX	(APP_DZOOM_RATIO_TOTAL_NUM-1)
#define APP_DZOOM_MIN_RATIO_INDEX	(0)

/*-- to advance ratio index by 1 according current dzoom direction */
#define ADV_NEXT_DZOOM_RATIO_INDEX(a)	do { \
											if (a==APP_ZOOM_TELE) gdzoomCb.ratioIdx++; \
											else if (a==APP_ZOOM_WIDE) gdzoomCb.ratioIdx--; \
										} while(0)

#define ADV_NEXT_DZOOM_SEG_INDEX(a)		do { \
											if (a==APP_ZOOM_TELE) gdzoomCb.curSeg++; \
											else if (a==APP_ZOOM_WIDE) gdzoomCb.curSeg--; \
										} while(0)

#define IS_DZOOM_TELE_MAX_RATIO(a)	(a==APP_ZOOM_TELE && gdzoomCb.ratioIdx==APP_DZOOM_MAX_RATIO_INDEX)
#define IS_DZOOM_WIDE_MIN_RATIO(a)	(a==APP_ZOOM_WIDE && gdzoomCb.ratioIdx==APP_DZOOM_MIN_RATIO_INDEX)

#define EXTRACT_DZOOM_RATIO(a)		(a & (~DZOOM_SEG_MARKER))
#define IS_PRIMARY_DZOOM_STOP(a)	(a & DZOOM_SEG_MARKER)

#define SET_UI_DZOOM_RATIO(m, r)	do {sp5kZoomFactorSet(((appDZoomUiMode_t)m)&0xFF, r); } while(0)

#define IS_DZOOM_TELE_REPEATED(a)	(a==APP_ZOOM_TELE && IS_KEY_TELE_PRESSED)
#define IS_DZOOM_WIDE_REPEATED(a)	(a==APP_ZOOM_WIDE && IS_KEY_WIDE_PRESSED)

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static appDZoomCtrl_t gdzoomCb;

/*-- still & view DZOOM ratio table
 *   To mark the primary DZOOM stop, please add
 *   DZOOM_SEG_MARKER to the actual ratio.
 */
#define ZOOMFAST    1
static UINT32 gdzoomRatioArray[] =
{
/*s Modify by Aries 10/02/04*/
#if 1 //(_CAM_A900_EVM)||(_CAM_M900_EVM)||(_CAM_A900_D3) ||(_CAM_CATHY_EVM)
	
	1000+DZOOM_SEG_MARKER,	  
	1022,					  
	1044+DZOOM_SEG_MARKER,	  
	1067,					  
	1091+DZOOM_SEG_MARKER,	  
	1114,					  
	1139+DZOOM_SEG_MARKER,	  
	1164,					  
	1189+DZOOM_SEG_MARKER,	  
	1215,					  
	1242+DZOOM_SEG_MARKER,	  
	1269,					  
	1297+DZOOM_SEG_MARKER,	  
	1325,					  
	1354+DZOOM_SEG_MARKER,	  
	1384,					  
	1414+DZOOM_SEG_MARKER,	  
	1445,					  
	1477+DZOOM_SEG_MARKER,	  
	1509,					  
	1542+DZOOM_SEG_MARKER,	  
	1576,					  
	1610+DZOOM_SEG_MARKER,	  
	1646,					  
	1682+DZOOM_SEG_MARKER,	  
	1719,					  
	1756+DZOOM_SEG_MARKER,	  
	1795,					  
	1834+DZOOM_SEG_MARKER,	  
	1874,					  
	1915+DZOOM_SEG_MARKER,	  
	1957,					  
	2000+DZOOM_SEG_MARKER,	  
	2044,					  
	2089+DZOOM_SEG_MARKER,	  
	2134,					  
	2181+DZOOM_SEG_MARKER,	  
	2229,					  
	2278+DZOOM_SEG_MARKER,	  
	2327,					  
	2378+DZOOM_SEG_MARKER,	  
	2430,					  
	2484+DZOOM_SEG_MARKER,
	2538,					  
	2594+DZOOM_SEG_MARKER,	  
	2650,					  
	2709+DZOOM_SEG_MARKER,	  
	2768,					  
	2828+DZOOM_SEG_MARKER,	  
	2890,					  
	2954+DZOOM_SEG_MARKER,	  
	3018,					  
	3084+DZOOM_SEG_MARKER,	  
	3152,					  
	3221+DZOOM_SEG_MARKER,	  
	3292,					  
	3364+DZOOM_SEG_MARKER,	  
	3437,					  
	3513+DZOOM_SEG_MARKER,	  
	3589,					  
	3668+DZOOM_SEG_MARKER,	  
	3748,					  
	3830+DZOOM_SEG_MARKER,	  
	3914,					  
	4000+DZOOM_SEG_MARKER,	  
	
	#if 0
	(1000+DZOOM_SEG_MARKER),
	1078,
	1161,
	1251,
	(1348+DZOOM_SEG_MARKER),
	1452,
	1565,
	1686,
	(1817+DZOOM_SEG_MARKER),
	1958,
	2110,
	2273,
	(2449+DZOOM_SEG_MARKER),
	2639,
	2844,
	3064,
	(3302+DZOOM_SEG_MARKER),
	3558,
	3834,
	(4000+DZOOM_SEG_MARKER),
	#endif
/* Eric 100527 Digital Zoom fix to 4.0x */	
/*	4131, 
	(4451+DZOOM_SEG_MARKER),
	4796,
	5168,
	5568,
	(6000+DZOOM_SEG_MARKER)
*/	
#else

	(1000+DZOOM_SEG_MARKER),
	1040,
	1080,
	(1100+DZOOM_SEG_MARKER),
	1140,
	1180,
	1220,
	1260,
	(1320+DZOOM_SEG_MARKER),
    1360,
    1400,
    1460,
    1500,
    (1560+DZOOM_SEG_MARKER),
    1600,
    1680,
    1760,
    (1800+DZOOM_SEG_MARKER),
    1900,
    2060,
    2120,
    (2200+DZOOM_SEG_MARKER),
    2300,
    2400,
    2500,
    (2600+DZOOM_SEG_MARKER),
    2700,
    2800,
    2900,
    (3000+DZOOM_SEG_MARKER),
    3280,
    (3400+DZOOM_SEG_MARKER),
    3580,
    3780,
    (4000+DZOOM_SEG_MARKER)	/* PRV_DZOOM_MAX */
 #endif
/*e Modify by Aries 10/02/04*/

};

#define APP_DZOOM_DISTANCE (EXTRACT_DZOOM_RATIO(gdzoomRatioArray[APP_DZOOM_MAX_RATIO_INDEX])-1000)

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : zoomUIKeyControl
 *------------------------------------------------------------------------*/
static
void
zoomUIKeyControl(
	UINT8 zoomSxn
)
{
	/* %TODO% */
	return;
}

/*-------------------------------------------------------------------------
 *  File Name : dzoomRatioSyncAssociatedMode
 *------------------------------------------------------------------------*/
static
void
dzoomRatioSyncAssociatedMode(
	UINT32	uiMode,
	UINT32	ratio
)
{
	if (uiMode==APP_DZOOM_UI_MODE_STILL_VIEW) {
		SET_UI_DZOOM_RATIO(SP5K_MODE_STILL_SNAP, ratio);
	} else if (uiMode==APP_DZOOM_UI_MODE_VIDEO_VIEW) {
		SET_UI_DZOOM_RATIO(APP_DZOOM_UI_MODE_VIDEO_REC, ratio);
	} else if (uiMode==APP_DZOOM_UI_MODE_VIDEO_REC) {
		SET_UI_DZOOM_RATIO(APP_DZOOM_UI_MODE_VIDEO_VIEW, ratio);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appDZoomTeleWide
 *------------------------------------------------------------------------*/
static
UINT32
appDZoomTeleWide(
	UINT8	zoomDir,
	UINT32	uiMode
)
{
	UINT32 comboRatio, realRatio;
	UINT32 ratioDist;

	/*-- Param validatiy check
	 */
	if (gdzoomCb.isEnabled==FALSE) 
	{
		DBG_PRINT("zoom: %s: DZOOM Disabled !!\n\n", __FUNCTION__);
		return SUCCESS;
	}

	if (zoomDir!=APP_ZOOM_TELE && zoomDir!=APP_ZOOM_WIDE) 
	{
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "zoom: %s: illegal zoomDir(%d)", __FUNCTION__, zoomDir);
		return FAIL;
	}

	if (IS_DZOOM_TELE_MAX_RATIO(zoomDir)) 
	{
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "zoom: Skipped, Max DZOOM ratio(%d)", zoomDir);
		return SUCCESS;
	} 
	else if (IS_DZOOM_WIDE_MIN_RATIO(zoomDir)) 
	{
        HOST_PROF_LOG_ADD(LEVEL_INFO, "zoom: Skipped, Min DZOOM ratio");
		return SUCCESS;
	}

	/*-- Disable keys
	 */
	zoomUIKeyControl(APP_ZOOM_SESSION_START);

	comboRatio = gdzoomRatioArray[gdzoomCb.ratioIdx];
	realRatio = EXTRACT_DZOOM_RATIO(comboRatio);

	do 
	{		
		ADV_NEXT_DZOOM_RATIO_INDEX(zoomDir);		
		/* to advance DZOOM ratio one step according to
		 * current DZOOM direction. E.g., for DZOOM_TELE --> +1,
		 * for DZOOM_WIDE --> -1
		 */
		comboRatio = gdzoomRatioArray[gdzoomCb.ratioIdx];
		realRatio = EXTRACT_DZOOM_RATIO(comboRatio);

		SET_UI_DZOOM_RATIO(uiMode, realRatio);
	} while (!IS_PRIMARY_DZOOM_STOP(comboRatio));

	/*-- Advance current DZOOM segment index by 1 stop according to zoomDir
	 */
	ADV_NEXT_DZOOM_SEG_INDEX(zoomDir);

	/*-- Synchronize associated mode DZOOM ratio */
	dzoomRatioSyncAssociatedMode(uiMode, realRatio);
		
	/*-- Post MSG for DZOOM OSD Bar update */
	ratioDist = realRatio - 1000;
	ratioDist = (ratioDist*100) / APP_DZOOM_DISTANCE;
	sp5kHostMsgSend(APP_UI_MSG_DZOOM_POS_UPDATE, ratioDist);
	/*-- check if Key repeated
	 */
	if (IS_DZOOM_TELE_REPEATED(zoomDir)) 
	{
		/*DBG_PRINT("DZOOM (T) Sending MSG \n"); */
		sp5kHostMsgSend(APP_UI_MSG_LENS_DZOOM_TELE, 0);
	} 
	else if (IS_DZOOM_WIDE_REPEATED(zoomDir)) 
	{
		/*DBG_PRINT("DZOOM (W) Sending MSG \n"); */
		sp5kHostMsgSend(APP_UI_MSG_LENS_DZOOM_WIDE, 0);
	}
	/*-- Enable keys */
	zoomUIKeyControl(APP_ZOOM_SESSION_END);  /*0x%08x*/

	DBG_PRINT("zoom: seg(%d), ratio(0x%08x) @ %u \n\n", gdzoomCb.curSeg, comboRatio, appMsTimeGet());

	return SUCCESS;
}

/*-------------------------------------------------------------------------
 *  File Name : appZoomTeleWide
 *------------------------------------------------------------------------*/
static
void
appZoomTeleWide(
	UINT8	zoomDir,
	UINT32	uiMode
)
{
	/* Param Validity check */
	if (uiMode!=APP_DZOOM_UI_MODE_STILL_VIEW &&
		uiMode!=APP_DZOOM_UI_MODE_STILL_SNAP &&
		uiMode!=APP_DZOOM_UI_MODE_VIDEO_VIEW &&
		uiMode!=APP_DZOOM_UI_MODE_VIDEO_REC)
	{
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "zoom: %s: illegal uiMode(%d)",__FUNCTION__, uiMode );
		return;
	}

	if (zoomDir!=APP_ZOOM_TELE && zoomDir!=APP_ZOOM_WIDE) 
	{
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "zoom: %s: illegal zoomDir(%d)",__FUNCTION__, zoomDir );
		return;
	}

	/* Disable keys */
	zoomUIKeyControl(APP_ZOOM_SESSION_START);

	/* Do DZoom */
	appDZoomTeleWide(zoomDir, uiMode);

	/*-- Enable keys */
	zoomUIKeyControl(APP_ZOOM_SESSION_END);
}



/*-------------------------------------------------------------------------
 *  File Name : appDZoomReset
 *------------------------------------------------------------------------*/
void
appDZoomReset(
	UINT32 uiMode
)
{
	memset(&gdzoomCb, 0, sizeof(appDZoomCtrl_t));

	switch ((uiMode&0xFF)) {
	default :
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "zoom: %s: illegal uiMode(%d)",__FUNCTION__, uiMode );
		break;
	case SP5K_MODE_STILL_PREVIEW :
	case SP5K_MODE_STILL_SNAP:
		sp5kZoomFactorSet(SP5K_MODE_STILL_PREVIEW, 1000);
		sp5kZoomFactorSet(SP5K_MODE_STILL_SNAP, 1000);
		break;
	case SP5K_MODE_VIDEO_PREVIEW:
	case SP5K_MODE_VIDEO_RECORD:
		sp5kZoomFactorSet(SP5K_MODE_VIDEO_PREVIEW, 1000);
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appDZoomEnableCtrl
 *------------------------------------------------------------------------*/
void
appDZoomEnableCtrl(
	BOOL isEnable
)
{
	gdzoomCb.isEnabled = (isEnable) ? TRUE : FALSE;
}

/*-------------------------------------------------------------------------
 *  File Name : appZoomStop
 *------------------------------------------------------------------------*/
void
appZoomStop(
	void
)
{

}

/*-------------------------------------------------------------------------
 *  File Name : appZoomNextModeQuery
 *------------------------------------------------------------------------*/
UINT32
appZoomNextModeQuery(
	UINT8	zoomDir
)
{
	/* Param Validity check */

	if (zoomDir!=APP_ZOOM_TELE && zoomDir!=APP_ZOOM_WIDE) {
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "zoom: %s: illegal zoomDir(%d)",__FUNCTION__, zoomDir );
		return APP_ZOOM_NEXT_UNKNOWN;
	}

	/* Check whether go Optical ZOOM or DigiZOOM */
	if (zoomDir==APP_ZOOM_TELE) {
		/* ZOOM-IN !! */
		return ((gdzoomCb.isEnabled) ? APP_ZOOM_NEXT_DZOOM : APP_ZOOM_NEXT_UNKNOWN);
	} else {
		/* ZOOM-OUT !! */
		return ((gdzoomCb.isEnabled) ? APP_ZOOM_NEXT_DZOOM : APP_ZOOM_NEXT_UNKNOWN);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appStillDZoomTele
 *------------------------------------------------------------------------*/
void
appStillDZoomTele(
	void
)
{
	appDZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_STILL_VIEW);
}

/*-------------------------------------------------------------------------
 *  File Name : appStillDZoomWide
 *------------------------------------------------------------------------*/
void
appStillDZoomWide(
	void
)
{
	appDZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_STILL_VIEW);
}

/*-------------------------------------------------------------------------
 *  File Name : appStillZoomTele
 *------------------------------------------------------------------------*/
void
appStillZoomTele(
	void
)
{
	appZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_STILL_VIEW);
}

/*-------------------------------------------------------------------------
 *  File Name : appStillZoomWide
 *------------------------------------------------------------------------*/
void
appStillZoomWide(
	void
)
{
	appZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_STILL_VIEW);
}

/*-------------------------------------------------------------------------
 *  File Name : appVideZoomTele
 *------------------------------------------------------------------------*/
 void appVideoZoomTele(void)
{
	UINT32 State;
	State = appActiveStateGet();
	if (State == APP_STATE_VIDEO_REC)
	{
		appZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_VIDEO_REC);
	}
	else
	{
		appZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_VIDEO_VIEW);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appVideZoomWide
 *------------------------------------------------------------------------*/
 void appVideoZoomWide(void)
{
	UINT32 State;
	State = appActiveStateGet();
	if (State == APP_STATE_VIDEO_REC)
	{
		appZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_VIDEO_REC);
	}
	else
	{
		appZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_VIDEO_VIEW);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appStillDZoomTele
 *------------------------------------------------------------------------*/
void appVideoDZoomTele(void)
{
	UINT32 State;
	State = appActiveStateGet();

	if (State == APP_STATE_VIDEO_REC)
		appDZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_VIDEO_REC);
	else
		appDZoomTeleWide(APP_ZOOM_TELE, APP_DZOOM_UI_MODE_VIDEO_VIEW);
}


/*-------------------------------------------------------------------------
 *  File Name : appStillDZoomWide
 *------------------------------------------------------------------------*/
 void appVideoDZoomWide(void)
{
	UINT32 State;
	State = appActiveStateGet();

	if (State == APP_STATE_VIDEO_REC)
		appDZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_VIDEO_REC);
	else
		appDZoomTeleWide(APP_ZOOM_WIDE, APP_DZOOM_UI_MODE_VIDEO_VIEW);
}

/*-------------------------------------------------------------------------
 *  File Name : appStillDZoomWide
 *------------------------------------------------------------------------*/
UINT32 
appDZoomGetRatio(
	void
)
{
	return EXTRACT_DZOOM_RATIO(gdzoomRatioArray[gdzoomCb.ratioIdx]);
}

/*-------------------------------------------------------------------------
 *  File Name : appMaxZoomGetRatio
 *------------------------------------------------------------------------*/
UINT32 
appMaxZoomGetRation(
	void
)
{
	UINT8 maxdzratioIdx = APP_DZOOM_MAX_RATIO_INDEX;
	return EXTRACT_DZOOM_RATIO(gdzoomRatioArray[maxdzratioIdx]);
}


