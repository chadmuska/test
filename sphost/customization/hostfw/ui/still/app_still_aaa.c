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
#define HOST_DBG 1
#include "app_com_def.h"
#include "app_com_api.h"

#include "sp5k_global_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_cdsp_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_media_api.h"

#include "app_still.h"
#include "app_view_param.h"
#include "app_sound.h"
#include "app_strobe.h"
#include "app_flash_api.h"
#include "app_aaa.h"
#include "app_osd_api.h"
#include "app_view_osd.h"

#if S2SNOAF
#include "app_calib_api.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	AE_FASTAE_OK_FLASH_OFF,
	AE_FASTAE_OK_FLASH_ON,
	AE_FASTAE_RUNNING=0xff
};

enum {
	AAA_STAGE_AE,
	AAA_STAGE_AF,
	AAA_STAGE_READY
};

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 appStillAaa_stage = AAA_STAGE_AE;
UINT32 appStillAaa_force_capture = 0; // enable this will capture without s2 press.
UINT32 appStillAaa_af_on = 0;
UINT32 appStillAaa_ae_on = 0;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

extern UINT32 sp5kAeModeGet(UINT32 *pmode);
SINT32 pv_expidx_to_tvidx(SINT32 expidx);
SINT32 pv_agcidx_to_svidx(SINT32 agcidx);
extern SINT16 afALG_FocusDo(UINT32 *uEdgeValue);
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
_doSnapTwoPressAction(
	UINT32 preState
)
{
	UINT32 next_state,next_param;

    /*HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "3a: s2SnapTicket=%d, forceCapture=%d",\
                        gStillCB.s2SnapTicket, appStillAaa_force_capture);*/

	/* flow control variables */
	gStillCB.s2SnapTicket=0; // We will consume this ticket when we execute snap job.
	gStillCB.remainImg = 1;

    /*HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "3a: %s, gStillCB.remainImg = %d",\
                        __FUNCTION__, gStillCB.remainImg);

    HOST_PROF_LOG_ADD(LEVEL_DEBUG, "3a: clr face s");*/

    #if !SP5K_PVQV_DISABLE
    appViewOsd_PvQVDraw(FALSE, gStillCB.cap.qvSec);
    #endif

    /*HOST_PROF_LOG_ADD(LEVEL_DEBUG, "3a: clr face e");*/

	switch (preState)
	{
		case APP_STATE_VIDEO_PREVIEW:
            /*HOST_PROF_LOG_ADD(LEVEL_INFO, "3a: goto video rec state");*/
			appStateChange(APP_STATE_VIDEO_REC,STATE_PARAM_NORMAL_INIT);
			break;
		case APP_STATE_VIDEO_REC:
            /*HOST_PROF_LOG_ADD(LEVEL_INFO, "3a: goto video pv state");*/
            appModeSet(SP5K_MODE_VIDEO_PREVIEW);
			appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
			break;
		default:
#if SP5K_SENSOR_DUAL_MODE
            if (appDualPipModeGet() >= DUALSEN_SINGLE_SEC)
            {
                appYUVCapture(1);
                if(pUiSetting->sound.shutter != SHUTTER_SOUND_OFF)
	                appPreloadSoundPlay(SOUND_ID_SHUTTER);
                if(appDualPipModeGet() == DUALSEN_SINGLE_SEC)
                {
                    appAePreviewRestore();
                    appStateChange(APP_STATE_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);
                }
                else
                {
        			appStillFlow_GetNextState(&next_state, &next_param);
        			appStateChange(next_state, next_param);
                }
            }
            else
#endif
            {
    			appStillFlow_GetNextState(&next_state, &next_param);
                /*HOST_PROF_LOG_ADD(LEVEL_INFO, "3a: goto cap state");*/
    			appStateChange(next_state, next_param);
            }
			break;
	}
}

void
_doSnapOneReleaseAction(
	UINT32 CurMode
)
{
	if(CurMode == VIDEO_MODE)
	{
		/*sp5kMediaRecCfgSet(SP5K_MEDIA_REC_DIS_LEVEL,MEDIA_REC_RSC_DIS_OFF);*/
        if(appNextStateGet()!=APP_STATE_VIDEO_REC)
        {
		appModeSet(SP5K_MODE_VIDEO_PREVIEW);
		appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
	}
	else
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "3a: enter video rec state and skip s1 key");
	}
	else
	{
		if(appStillAaa_stage != AAA_STAGE_READY)
			appStateChange(APP_STATE_STILL_PREVIEW,STATE_PARAM_NORMAL_INIT);
	}
}

void
_processAfterAf(
	UINT32 af_result
)
{
		switch(af_result)
		{
		default:
		case AF_BUSY:
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "3a: %s, %d, AF_BUSY",\
                       __FUNCTION__, __LINE__);
			// the af is running.
		break;
		case AF_SUCCESS:
		case AF_FAIL:
			// OSD and Sound
			if(af_result == AF_SUCCESS)
			{
				gStillCB.focusStatus = FOCUS_STATUS_OK;
			}
			else
			{
				gStillCB.focusStatus = FOCUS_STATUS_NG;
				LED_FLICKER_AF_ERROR
			}

			if(!appStillAaa_force_capture)
			{
			}

			_processAfterAf(AF_FINI);
			break;
		case AF_FINI :
            /*HOST_PROF_LOG_ADD(LEVEL_INFO, "3a: af finish");*/

			// the af finish
			// TODO: the af should return ok or ng here.
			// In 10-sec self case, we will don't re-draw the
			// osd and directly capture after af.
			if(appStillAaa_force_capture)
			{
				_doSnapTwoPressAction(appPreviousStateGet());
			}
			else
			{
				if(gStillCB.focusStatus == FOCUS_STATUS_VIEW)
				{
					// We miss the AF_SUCCESS/AF_FAIL af_result.
					_processAfterAf(AF_SUCCESS);
				}
				// S1 & S2 job
				/*if(gStillCB.s2SnapTicket)*/
				{
					_doSnapTwoPressAction(appPreviousStateGet());
				}
			}
			appStillAaa_stage = AAA_STAGE_READY;
			break;
		}
}

void
_processAfterFastAe(
	UINT32 ae_result
)
{
    #if SP5K_ALGORITHM_AF
    #if S2SNOAF
    appCalibData_t *pcalib= appCalibDataGet();
    #endif
    #endif

	switch(ae_result) {
	case AE_FASTAE_RUNNING:
        /*HOST_PROF_LOG_ADD(LEVEL_DEBUG, "3a: AE_FASTAE_RUNNING...");*/
		break;
	case AE_FASTAE_OK_FLASH_ON:
	case AE_FASTAE_OK_FLASH_OFF:
		{
			appAeSet_t* pAe;
			pAe = appAeSetGet();
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "3a: pasm=%d, scene=%d, meter=%d, ev=%d, iris=%d, shutter=%d, iso=%d\n",\
                       pAe->pasm, pAe->scene,pAe->meter,pAe->evbias,pAe->iris,pAe->shutter,pAe->iso);
		}
		if(appStillAaa_ae_on) {
			gStillCB.needFlash = (gStillCB.flashMode == UI_FLASH_ON)? 1 :
				(gStillCB.flashMode == UI_FLASH_OFF)? 0: ae_result;
		}
		else {
			// preflash will use ae message. When it is off, we should stop flash.
			gStillCB.needFlash = 0;
		}

		if(gStillCB.needFlash)
		{
			LED_FLICKER_STROBE_CHARGE
		}

		appAeCapParaGet(&gStillCB.ae.iris, &gStillCB.ae.shutter, &gStillCB.ae.iso);

		// check the value
		if(gStillCB.ae.iris <= APERTURE_AUTO || gStillCB.ae.iris >= APERTURE_MAX) {
			gStillCB.ae.iris = APERTURE_2_8;
		}

		gStillCB.ae.iso = (gStillCB.ae.iso <= UI_ISO_50)?  UI_ISO_50:
			(gStillCB.ae.iso >= UI_ISO_MAX)?  UI_ISO_MAX-1: gStillCB.ae.iso;

#if 0
		if(gStillCB.ae.shutter <= SHUTTER_1S)
		{
			#if KIT_WITH_MSHUT
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "3a: %s: do dark sub",\
                       __FUNCTION__);
			gStillCB.needDarkFrameSub = 1;
			sp5kStillCapCfgSet( SP5K_CAPTURE_DARK_FRAME, 1 );
			sp5kImgDarkFrameSubCfgSet(0xffffffff);
			#else


			#endif
		}
		else
#endif           
		{
            HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "3a: %s: no dark sub",\
                       __FUNCTION__);
			gStillCB.needDarkFrameSub = 0;
			sp5kStillCapCfgSet( SP5K_CAPTURE_DARK_FRAME, 0);
		}
		appStillRawCbSet();
		appStillAaa_stage = AAA_STAGE_AF;
        /*HOST_PROF_LOG_ADD(LEVEL_INFO, "3a: ae finish");*/

        #if SP5K_ALGORITHM_AF
        #if S2SNOAF                 /*don't do af flow*/
        if (pcalib->CIPASign ==1)   /*mtbf flow for QA test AF*/
        {
            /*HOST_PROF_LOG_ADD(LEVEL_INFO, "3a: mtbf do af flow");*/
	    }
        else
        {
            appStillAaa_stage = AAA_STAGE_READY;
            _doSnapTwoPressAction(appPreviousStateGet());
        }
        #endif
        #endif

	default:
		break;
	}
}

void
appStillAaaInit(
	UINT32 param
)
{
	/*HOST_PROF_LOG_ADD(LEVEL_INFO, "3a: init s");*/
	appPowerSavingEnableSet(FALSE);
	gStillCB.s1AaaTicket=0; // We will consume this ticket when we execute Aaa job.
	appStillAaa_force_capture = param;
	appStillAaa_stage = AAA_STAGE_AE;

	appStillAaa_af_on = 0;

    /*HOST_PROF_LOG_ADD(LEVEL_DEBUG, "3a: aeget s");*/
	sp5kAeModeGet(&appStillAaa_ae_on);
    /*HOST_PROF_LOG_ADD(LEVEL_DEBUG, "3a: aeget e");*/

    /*HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "3a: %s, %d, Aaa_ae_on(%d)",\
                        __FUNCTION__, __LINE__, appStillAaa_ae_on);*/

	if(appStillAaa_ae_on == SP5K_AE_MODE_OFF)
	{
        /*HOST_PROF_LOG_ADD(LEVEL_DEBUG, "3a: ae msg s");*/
		sp5kHostMsgSend(SP5K_MSG_AE_READY, 0);
		/*HOST_PROF_LOG_ADD(LEVEL_DEBUG, "3a: ae msg e");*/
	}

	//if(appStillAaa_af_on)
	//{
	//}
    /*HOST_PROF_LOG_ADD(LEVEL_DEBUG, "3a: init e");*/
}

/*-------------------------------------------------------------------------
 *  Function Name : appStillAaaStateOnMsg
 *  Description :   Here we will apply AE then AF. So in this state, we have
 *			        3 stage, AE, AF and Ready. Capture only allowed in Ready
 *					stage. If we don't support lens, the af will be bypssed.
 *------------------------------------------------------------------------*/
void
appStillAaaStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
	case SP5K_MSG_AE_READY:
		/* 1) do fast AE, next is AF */
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "3a: Aaa_stage for AE: %d", appStillAaa_stage);
		if(appStillAaa_stage != AAA_STAGE_AE)
		{
			break;
		}

        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "3a: Aaa_ae_on: %d", appStillAaa_ae_on);
		if(appStillAaa_ae_on)
		{
		    UINT32 currMode;
			_processAfterFastAe(appAeFastAdjust(param));
			sp5kModeGet(&currMode);
			if(currMode == SP5K_MODE_VIDEO_PREVIEW)
				appStillAaa_stage = AAA_STAGE_AF ;
		}
		else
		{
			_processAfterFastAe(AE_FASTAE_OK_FLASH_OFF);
		}

		/* after ae, check if we need to do af */
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "3a: Aaa_stage for AF: %d, Aaa_af_on: %d", \
                            appStillAaa_stage, appStillAaa_af_on);

		if(appStillAaa_stage == AAA_STAGE_AF)
		{
			if(appStillAaa_af_on == 1)
			{
                sp5kHostMsgSend(SP5K_MSG_AF_READY, AF_SUCCESS);
			}
			else
			{
				_processAfterAf(AF_SUCCESS);
			}
		}
		break;
	case SP5K_MSG_AWB_READY:
		break;
	case SP5K_MSG_AF_READY:
		if(appStillAaa_stage != AAA_STAGE_AF)
		{
			break;
		}
        _processAfterAf(AF_SUCCESS);

		break;
	case APP_STATE_MSG_ABORT:
		gStillCB.s1AaaTicket=0;
		gStillCB.s2SnapTicket=0;
		_doSnapOneReleaseAction(gStillCB.cap.CurViewMode);
		break;
	default:
		break;
	}
}

void
appStillAaaStateOnKey(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
	    #if 0
		case APP_KEY_RELEASE_S1:
			if(gStillCB.s2SnapTicket == 1)	break;
            HOST_PROF_LOG_ADD(LEVEL_INFO, "3a: s1 release");
			appAePreviewRestore();
			_doSnapOneReleaseAction(gStillCB.cap.CurViewMode);

			break;
        #endif
		case APP_KEY_PRESS_S2:
            /*HOST_PROF_LOG_ADD(LEVEL_INFO, "3a: s2 press");
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "3a: Aaa_stage for 3A: %d %x", appStillAaa_stage, appPreviousStateGet());*/
			gStillCB.s2SnapTicket=1;

			#ifndef SP5K_SENSOR_FAKE  /* for fake sensor, the image content is fixed pattern and no need to check 3A state*/
			if(appStillAaa_stage == AAA_STAGE_READY)
			#endif
			{
				#if !SP5K_CVR_MODE
				_doSnapTwoPressAction(appPreviousStateGet());
				#endif
			}
			break;
		default:
			break;
	}
}

void
appStillAaaState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg)
	{
		case APP_STATE_MSG_INIT:
			appStillAaaInit(FLOW_EXTRCT_ST_PAR(param));
			break;
		case APP_STATE_MSG_CLOSE:
            #if SP5K_SENSOR_DUAL_MODE
            appSetYUVSaveStatus(0);
            #endif
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg))
			{
				appStillAaaStateOnKey(msg,param);
			}
			else
			{
				appStillAaaStateOnMsg(msg,param);
			}
			break;
	}
}
