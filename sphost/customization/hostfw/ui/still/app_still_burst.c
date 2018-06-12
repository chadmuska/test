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
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"
#include "app_video.h"
#include "app_still.h"
#include "app_view_param.h"
#include "app_view_osd.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_error_msg.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_sensor_api.h"
#include "app_stringid.h"
#include "app_errorosd.h"
#include "app_aaa.h"
#include "app_cdfs_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define BURST_STATUS_INIT   0
#define BURST_STATUS_ABOURT 1
#define BURST_STATUS_START  2
#define MAX_BURST_NUM 5
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct burst_s
{
	UINT32 aeb;
	UINT32 status;
	UINT32 imgId; 		/* message IMAGE_ID accumulate number */
}burst_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
burst_t gBtCB; 	/* burst control block */
UINT32 senMode;
sp5kAgcLimit_t agcLmt;
static tmx_t burstFileTime[MAX_BURST_NUM];
static UINT32 burstIdx = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appBurstSendObjToAPP()
{
	printf("wifi use\n");
}

#if 0
static void HdrCallback(UINT8 *yuv, UINT8 *qv, UINT32 *sz)
{
	SINT32 expIdx;
	UINT32 agcIdx;
	static SINT32 expIdx0 = 0;
	static SINT32 agcIdx0 = 0;
	static UINT32 capNum = 0;

	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bst:-HDR burst cb %d\n",capNum);

	if(capNum == 0){
		appAEStillCapExpAgcGet(&expIdx,&agcIdx);
		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bst:-HDR expIdx:%d/agcIdx:%d"\
		                ,expIdx,agcIdx);
		expIdx0 = expIdx;
		agcIdx0 = agcIdx;
	}

	capNum++;

	if(capNum == 1)  {
		agcIdx = agcIdx0 - HDR_BURST_EV_ADJUST;

		if(agcLmt.agcIdxMin>=agcIdx)
		{
			agcIdx = agcIdx0;
			expIdx -= HDR_BURST_EV_ADJUST;
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bst:-HDR lit cb:%d exp:%d agc%d"\
								,capNum,expIdx,agcIdx);
		}
	}
	else if(capNum == 2) {
		agcIdx = agcIdx0 + HDR_BURST_EV_ADJUST;

		if(agcLmt.agcIdxMax<=agcIdx)
		{
			agcIdx = agcIdx0;
			expIdx += HDR_BURST_EV_ADJUST;
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bst:-HDR lit cb:%d exp:%d agc%d"\
			                    ,capNum,expIdx,agcIdx);
		}
	}
	else {
		agcIdx = agcIdx0;
		capNum = 0;
	}
	expIdx = expIdx0;

	appAEStillCapExpAgcSet(expIdx,agcIdx,0);
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bst:-HDR set=> expIdx:%d/agcIdx:%d"\
	                    ,expIdx,agcIdx);
}
#endif

void
appStillBurstTimeget(tmx_t *getFileTime, UINT32 bidx)
{
	memcpy(getFileTime, &burstFileTime[bidx], sizeof(*getFileTime));
}

void
appStillBurstStateInit(void)
{
	UINT32 cfg;
	UINT32 sel;
	appDiskCapability_t *sdCap;

	appStill_SetSp5kStillSize(pViewParam->stillSize);
	appAeCaptureSet(BURST_STATUS_INIT);
	appAeAebSet(BURST_STATUS_INIT);
	gBtCB.status = gBtCB.imgId = gBtCB.aeb = BURST_STATUS_INIT;
	HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bst: %s BURST:%d", __FUNCTION__, pViewParam->stillDriverMode);

	sel = pViewParam->stillDriverMode;
	if(sel == UI_STILL_DRIVERMODE_BURST_H)
		cfg = BURST_H_LENGTH;
	else
		cfg = BURST_L_LENGTH;

	/* we miss the s2 release message */
	if(gStillCB.s2press == 0)
	{
        HOST_PROF_LOG_ADD(LEVEL_INFO, "bst: send s2 release message");
		sp5kHostMsgSend(APP_KEY_RELEASE_S2, 0);
	}
    gStillCB.s2press=1;
	gStillCB.isReady2View = 0;

	/* no callback */
	sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_YUV_WITH_CDSP, NULL);
	sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_RAW, NULL);
	sp5kStillCapCfgSet(SP5K_CAPTURE_IMG_MERGE_MODE, SP5K_CAPTURE_MERGE_MODE_OFF);

	/* compression ratio is 1/8 */
	/*sp5kStillCapCfgSet(SP5K_CAPTURE_MAX_COMPRESSION_RATIO, 800);*/

	sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_LENGTH, cfg);
	sp5kStillCapCfgSet(SP5K_CAPTURE_FAST_BK_PREVIEW, 0);

	sp5kStillCapCallbackSet(SP5K_CAPTURE_CALLBACK_BURST, NULL);

	sdCap = appSdInfoGet();
	/* fast burst mode */
	switch(sel)
	{
	case UI_STILL_DRIVERMODE_AEB:
		sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_INTERVAL, AEB_BURST_INTRVAL);
		sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_FAST_SENSOR_TRIG, 0);
		break;

	case UI_STILL_DRIVERMODE_BURST_H:
		sp5kStillCapCfgSet(SP5K_CAPTURE_BRC, 0);
		sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_INTERVAL, BURST_H_INTRVAL);
		sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_FAST_SENSOR_TRIG, 1);
		break;

	case UI_STILL_DRIVERMODE_BURST_L:
		if(sdCap->sdMode >= SD_18V_SDR50){
			sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_INTERVAL, BURST_L_UHS_INTRVAL);
		}else{
			sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_INTERVAL, BURST_L_INTRVAL);
		}
        sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_INTERVAL, BURST_L_INTRVAL);
		sp5kStillCapCfgSet(SP5K_CAPTURE_BURST_FAST_SENSOR_TRIG, 1);
		break;

	}

	#if !SP5K_PVQV_DISABLE
	appStillQVCbSet(TRUE);
	#endif

	sp5kStillCapCfgSet(SP5K_CAPTURE_MODE, SP5K_CAPTURE_MODE_BURST);
	sp5kStillCapCallbackSet( SP5K_CAPTURE_CALLBACK_STORE_DONE,  appBurstSendObjToAPP );
	HOST_PROF_LOG_ADD(LEVEL_DEBUG, "bst: mode set s");
	appStillUtil_CaptureSet();
	gBtCB.status = BURST_STATUS_START;

	appBurstTimeCbReg(appStillBurstTimeget, cfg);
}

void
appStillBurstStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	UINT32 next_state,next_param;
	BOOL is_yuv_cb_on=FALSE;

	switch (msg) {

#if 0
	case SP5K_MSG_STILL_CAPTURE_IMAGE_ID:
		if(pViewParam->stillDriverMode == UI_STILL_DRIVERMODE_BURST_H)
			break;
		if(pViewParam->stillDriverMode == UI_STILL_DRIVERMODE_BURST_L && gBtCB.status == BURST_STATUS_ABOURT)
		{
			HOST_PROF_LOG_ADD(LEVEL_INFO, "bst: UI_DRIVE_BURST_L msg: abort");
			sp5kStillCapAbort();
			gStillCB.isReady2View = 1;
			break;
		}

		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bst: IMAGE_ID %d", gBtCB.imgId);

		gBtCB.imgId++;
		if (gBtCB.status == BURST_STATUS_ABOURT) {
			gStillCB.isReady2View = 1;
			/*sp5kStillCapAbort();*/
			HOST_PROF_LOG_ADD(LEVEL_DEBUG, "bst: abort");
		}
		if(gBtCB.aeb == UI_STILL_DRIVERMODE_AEB && gBtCB.imgId == 3) {
			gStillCB.isReady2View = 1;
			/*sp5kStillCapAbort();*/
			gBtCB.status = BURST_STATUS_ABOURT;
			HOST_PROF_LOG_ADD(LEVEL_DEBUG, "bst: AEB mode finish");
		}
		// for AEB, ae relative data
		appAeCaptureSet(gBtCB.imgId);
		appAeCapParaGet(&gStillCB.ae.iris, &gStillCB.ae.shutter, &gStillCB.ae.iso);

		// check the value
		if(gStillCB.ae.iris <= APERTURE_AUTO || gStillCB.ae.iris >= APERTURE_MAX) {
			gStillCB.ae.iris = APERTURE_2_8;
		}
		if(gStillCB.ae.shutter <= SHUTTER_AUTO || gStillCB.ae.shutter >= SHUTTER_MAX) {
			gStillCB.ae.shutter = SHUTTER_1_15S;
		}
		gStillCB.ae.iso = (gStillCB.ae.iso <= UI_ISO_50)?  UI_ISO_50:
			(gStillCB.ae.iso >= UI_ISO_MAX)?  UI_ISO_MAX-1: gStillCB.ae.iso;
		if(gStillCB.needDarkFrameSub || gStillCB.isXDenoiseOn) {
			appErrMsgWithSound(ERR_MSG_BUSY_WAIT, ERR_MSG_ONLY, ERR_MSG_SHOW_FOREVER);
		}
#else
    case SP5K_MSG_STILL_CAPTURE_IMAGE_ID:
    if((pViewParam->stillDriverMode == UI_STILL_DRIVERMODE_BURST_H)||(pViewParam->stillDriverMode == UI_STILL_DRIVERMODE_BURST_L))
        break;
#endif

	case SP5K_MSG_STILL_QV_TIMEOUT:
		HOST_PROF_LOG_ADD(LEVEL_WARN, "bst: QV_TIMEOUT");
		break;

	case SP5K_MSG_STILL_VIEW_SETTING_NOTIFY:
		HOST_PROF_LOG_ADD(LEVEL_INFO, "bst: VIEW_SETTING_NOTIFY");

		if(gBtCB.status == BURST_STATUS_ABOURT) {
			HOST_PROF_LOG_ADD(LEVEL_WARN, "bst: VIEW_SETTING_NOTIFY ABOURT");
			appAePreviewSet();
			appAePreviewRestore();
		}
		break;

	case SP5K_MSG_MODE_READY:
		HOST_PROF_LOG_ADD(LEVEL_DEBUG, "bst: msg MODE_READY");

		if (param == SP5K_MODE_STILL_PREVIEW) {
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bst: msg MODE_READY-STILL_PREVIEW = %d"\
				, is_yuv_cb_on);
			burstIdx = 0;
			appStillYuvCbSet(is_yuv_cb_on);
			appStillFlow_GetNextState(&next_state, &next_param);
			appStateChange(next_state, next_param);
		}

		if (param == SP5K_MODE_STILL_SNAP) {
			appViewOsd_AllClean();
			if(gStillCB.needDarkFrameSub || gStillCB.isXDenoiseOn) {
				is_yuv_cb_on = TRUE;
			}
			HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bst: msg MODE_READY-STILL_SNAP = %d", is_yuv_cb_on);
			appStillYuvCbSet(is_yuv_cb_on);
			appOsdMessageBoxShow(TRUE,STRTK_WAITING_FOR_CAPTURE);
			sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, 1);
			}
		break;
	case APP_UI_MSG_VIEW_YUV_CALLBACK:
		if(gStillCB.needDarkFrameSub || gStillCB.isXDenoiseOn) {
			appErrMsgWithSound(ERR_MSG_CLEAR, 0, 0);
			#if (FIELD_QV_ENABLE	== 1)
			if (pViewParam->stillStamp == UI_DATESTAMP_OFF) {
			}
			#endif
		}
		break;
	case SP5K_MSG_STILL_READOUT_START:
		sp5kRtcDateTimeGet( SP5K_DATE_TIME_OPTION, &burstFileTime[burstIdx]);
		burstIdx ++;
		if(pUiSetting->sound.shutter != SHUTTER_SOUND_OFF) {
			appPreloadSoundPlay(SOUND_ID_SHUTTER);
		}
		break;
	default:
		break;
	}
}

void
appStillBurstStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
    case APP_KEY_RELEASE_S1:
	case APP_KEY_RELEASE_S2:
        if(gBtCB.status == BURST_STATUS_START)
        {
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "bst: key release msg:%x imgid:%d abort", msg, gBtCB.imgId);
            gBtCB.status = BURST_STATUS_ABOURT;
        }
        else
        {
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "bst: key release msg:%x imgid:%d %d", msg, gBtCB.imgId, gBtCB.status);
        }
		break;
	default:
		break;
	}
}

void
appStillBurstState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
	case APP_STATE_MSG_INIT:
		HOST_PROF_LOG_PRINT(LEVEL_INFO, "bst: %s init", __FUNCTION__);
		appStillBurstStateInit();
		HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "bst: %s init e", __FUNCTION__);
		break;
	case APP_STATE_MSG_CLOSE:
		sp5kStillCapAbort();
        sp5kStillCapCallbackSet( SP5K_CAPTURE_CALLBACK_STORE_DONE,  NULL);
		appStateCloseDone();
		break;
	default:
		if (IS_APP_KEY_MSG(msg))
		{
			appStillBurstStateOnKey(msg,param);
		}
		else
		{
			appStillBurstStateOnMsg(msg,param);
		}
		break;
	}
}

