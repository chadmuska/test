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
 **************************************************************************/

#define HOST_DBG 0

#include "common.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "sp5k_cdsp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_ae_api.h"
#include "sp5k_os_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_sensor_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_cal_api.h"
#include "sp5k_msg_def.h"
#include "sp5k_modesw_api.h"
#if SP5K_ICAT_EIS
#include "sp5k_eis_rsc_algo_api.h"
#endif

#include "app_com_api.h"
#include "app_com_debug.h"
#include "app_dbg_api.h"
#include "app_exif.h"
#include "app_osd_api.h"
#include "app_lens_api.h"
#include "app_view_param_def.h"
#include "app_view_param.h"
#include "app_user_setting.h"
#include "app_calib_api.h"
#include "app_flash_api.h"
#include "app_aaa.h"
#include "app_ae_private.h"
#include "app_face_detect_ex.h"
#include "app_3aalg.h"
#include "app_aealg_api.h"
#include "app_awbalg_api.h"
#include "app_res_def.h"
#include "middleware/sensor_model.h"
#if SP5K_DIQ_FRMWRK
#include "api/sp5k_dq_api.h"
#endif
#define KIT_WITH_IRIS	    0
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define KIT_WITH_IRIS   0
#define AE_SUB_ACTION_FLOW  0


#define HDR_MaxExpIdx					215  /*control exp max idx*/
#define HDR_MinExpIdx					33
#define HDR_MaxAgcIdx					96
#define HDR_MinAgcIdx					0

#define AECONFIG            IQ_PATH"\\AECONFIG.BIN"

#define AEPCHART50HZ10F     IQ_PATH"AEPCHART50HZ10F.BIN"
#define AEPCHART50HZ15F     IQ_PATH"AEPCHART50HZ15F.BIN"
#define AEPCHART50HZ30F     IQ_PATH"AEPCHART50HZ30F.BIN"
#define AEPCHART50HZ60F     IQ_PATH"AEPCHART50HZ60F.BIN"
#define AEPCHART60HZ10F     IQ_PATH"AEPCHART60HZ10F.BIN"
#define AEPCHART60HZ15F     IQ_PATH"AEPCHART60HZ15F.BIN"
#define AEPCHART60HZ30F     IQ_PATH"AEPCHART60HZ30F.BIN"
#define AEPCHART60HZ60F     IQ_PATH"AEPCHART60HZ60F.BIN"
#define AEPCHART120F        IQ_PATH"AEPCHART120F.BIN"
#define AEPCHART240F        IQ_PATH"AEPCHART240F.BIN"

#define AEPCHART50HZCAP     IQ_PATH"AEPCHART50HZCAP.BIN"
#define AEPCHART50HZISO100  IQ_PATH"AEPCHART50HZISO100.BIN"
#define AEPCHART50HZCAPMD   IQ_PATH"AEPCHART50HZCAPMD.BIN"
#define AEPCHART60HZCAP     IQ_PATH"AEPCHART60HZCAP.BIN"
#define AEPCHART60HZISO100  IQ_PATH"AEPCHART60HZISO100.BIN"
#define AEPCHART60HZCAPMD   IQ_PATH"AEPCHART60HZCAPMD.BIN"

#define AEWINDOWLV          IQ_PATH"AEWINDOWLV.BIN"
#define AEWINDOWMULTI       IQ_PATH"AEWINDOWMULTI.BIN"
#define AEWINDOWCENTER      IQ_PATH"AEWINDOWCENTER.BIN"
#define AEWINDOWSPOT        IQ_PATH"AEWINDOWSPOT.BIN"

#define AETARGET            IQ_PATH"AETARGET.BIN"



/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static SINT32 aebiascmptbl[AE_BIAS_MAX] = {-2000, -1700, -1300, -1000, -700, -300, 0, 300, 700, 1000, 1300, 1700, 2000 };
static SINT32 aeIris[2] = {IRIS_STEP_1, IRIS_STEP_2};

appAeCb_t aeCb;
appAePara_t aePara;

static UINT32 aeFrameRateSel= 120;
static UINT32 aeFlickerSel=0;/*0:50Hz , 1:60HZ*/
static UINT16 aePVHighISO = 0xffff;
static UINT16 aeCAPHighISO = 0xffff;
static UINT8   aeinfoprint = 0;
static UINT8   aeinfoprintdual = 0;
static UINT32 aestatusprintvdcnt = 0;
static UINT8   aeMDEnable = 0;
static UINT8   aeMD = 0;
static SINT32 aeevset = 0;
static SINT32 aebiascmp=0;

static AEConfig_t    AEConfig;
static AEProgram_t AEProgramChart_PV[AE_PROGRAM_CHART_SIZE];
static AEProgram_t AEProgramChart_CAP[AE_PROGRAM_CHART_SIZE];
static UINT8 AEWindowLV[AE_WINDOW_H_SIZE*AE_WINDOW_V_SIZE];
static UINT8 AEWindow[AE_WINDOW_H_SIZE*AE_WINDOW_V_SIZE];
static UINT8 AEWindowTemp[AE_WINDOW_H_SIZE*AE_WINDOW_V_SIZE];

static UINT16 AETarget[AE_TARGET_LV_RANGE];

sp5kEvUnit_t evUnit;
sp5kExpLimit_t expLimitPv,expLimitCap;
sp5kAgcLimit_t agcLimitPv,agcLimitCap;
#if SP5K_DIQ_HDR
static SINT32 aeHDRev = 30;
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void appExif_MN_BufClear(UINT8 bufID);
extern UINT32  appExif_MN_Printf(UINT8 bufID, const char *szFmt, ...);

extern void sensor_HDR_ev_set(UINT8 ev);
extern void sensor_HDR_ev_diff_set(UINT16 diff1, UINT32 diff2);
extern void sensor_HDR_EXP_do(void);
extern void sensor_HDR_ISO_do(void);
extern UINT32 sensorModeGet(void);
extern UINT32  sp5kSensorInfoGet(UINT32 sensorMode,UINT32 selector,UINT32 *pval);
extern UINT32 LoadFromFile( UINT8 fname[], UINT8 * pFilledBuff, UINT32 nFillBuffSize, UINT32 mallocFlag, UINT32 nGetFileSizeOnly );

#if SP5K_ICAT_EIS
void appVideoRSCExpTimeGet(void);
#endif

/**************************************************************************
 *                 SPECIAL APPLICATION                 *
 **************************************************************************/
#if (AE_SUB_ACTION_FLOW==1)
#define EVT_AE_SUB_ALL				0xffffffff
#define EVT_AE_SUB_CLR(act_evt,evtstate) {act_evt &= ~(evtstate);}

#define AE_SUB_ACTION_LED_STATE_CONTROL(led_state_mode) (led_state_mode-AE_SUBACTION_LED_MANUAL_START)

#define AE_SUB_DELAY_VD                         3
#define AE_SUB_ACTION_START_LV      2000
#define AE_SUB_ACTION_CHANGE_LV   5000
#define AE_SUB_ACTION_LV_DELTA      2000
#define AE_SUB_SPEED                               5
#define AE_SUB_SLOPE                            192
#define AE_SUB_LIMIT                             256
#define AE_SUB_IRLED_RGAIN	                300
#define AE_SUB_IRLED_BGAIN	                300

static SP5K_THREAD *ptrdaesub = NULL;
static SP5K_EVENT_FLAGS_GROUP evtaesub = 0;

static UINT8 AeSubActionState=0;/*0:OFF , 1:IR_Low , 2:IR_Mid , 3:IR_High*/
static UINT8 AeSubActionDelayVD=0;
static UINT8 AeSubActionHandle=0;
static UINT8 AeSubActionMode=0;/*0:Auto , 1:OFF , 2:IR_Low , 3:IR_Mid , 4:IR_High*/
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 * Function : svidx_to_aeIso_tblidx
 *------------------------------------------------------------------------*/
UINT8 svidx_to_aeIso_tblidx(SINT32 svidx)
{
	SINT32 sv10;
	UINT8	idx;

	sv10 = (svidx*10)/evUnit.evUnit;

	idx = 0;
	if (sv10 < 50) { /*-- iso-50 */ return idx; }
	idx++;
	if (sv10 < 60) { /*-- iso-100 */ return idx; }
	idx++;
	if (sv10 < 70) { /*-- iso-200 */ return idx; }
	idx++;
	if (sv10 < 80) { /*-- iso-400 */ return idx; }
	idx++;
	if (sv10 < 90) { /*-- iso-800 */ return idx; }
	idx++;
	if (sv10 < 100) { /*-- iso-1600 */ return idx; }
	idx++;
	if (sv10 < 110) { /*-- iso-3200 */ return idx; }
    else
    {
    	/*-- out of aeIso[] */
        HOST_PROF_LOG_ADD(LEVEL_WARN,"ae: Out of scope of aeIso[]");
    	idx = 0;
    	return idx;
    }
}

/*-------------------------------------------------------------------------
 *  Function Name : _aesubthread
 *  Description :
 *------------------------------------------------------------------------*/
void _aesubthread(ULONG init_data)
{
#if (AE_SUB_ACTION_FLOW==1)
    UINT32 ret = SP5K_SUCCESS;
    UINT32 ulActualEvents;
	UINT32 mode;

	ret = sp5kOsEventFlagsCreate(&evtaesub, "evtaesub");
	HOST_ASSERT(ret == SUCCESS);

	while(1)
	{
		if(evtaesub)
		{
			sp5kOsEventFlagsGet(&evtaesub, EVT_AE_SUB_ALL, TX_OR_CLEAR, &ulActualEvents, TX_WAIT_FOREVER);

            if (ulActualEvents & (0x01<<AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_OFF)))
			{
                printf("\n~ IQ Color ~ \n");
                printf("~ IR OFF ~ \n");
				appVideoIRLedCtrl(APP_IR_FORWARD_MODE, APP_IR_LED_LEVEL_OFF);
				sp5kModeGet(&mode);
				sp5kIqModeSet(mode);
				appIqEffectSet(UI_EFFECT_NORMAL);
				sp5kIqModeSetDone(mode);
                EVT_AE_SUB_CLR(ulActualEvents, (0x01<<AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_OFF)));
				AeSubActionHandle=0;
			}
            if (ulActualEvents & (0x01<<AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_1)))
			{
                printf("\n~ IQ BW ~ \n");
                printf("~ IR LOW ~ \n");
				sp5kModeGet(&mode);
				sp5kIqModeSet(mode);
				appIqEffectSet(UI_EFFECT_BW);
				sp5kIqModeSetDone(mode);
				sp5kSensorVvalidWait();
				sp5kSensorVvalidWait();
				appVideoIRLedCtrl(APP_IR_REVERSE_MODE, APP_IR_LED_LEVEL_LOW);
                EVT_AE_SUB_CLR(ulActualEvents, (0x01<<AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_1)));
				AeSubActionHandle=0;
			}
            if (ulActualEvents & (0x01<<AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_2)))
			{
                printf("\n~ IQ BW ~ \n");
                printf("~ IR MID ~ \n");
                sp5kModeGet(&mode);
                sp5kIqModeSet(mode);
                appIqEffectSet(UI_EFFECT_BW);
                sp5kIqModeSetDone(mode);
                sp5kSensorVvalidWait();
                sp5kSensorVvalidWait();
				appVideoIRLedCtrl(APP_IR_REVERSE_MODE, APP_IR_LED_LEVEL_MID);
                EVT_AE_SUB_CLR(ulActualEvents, (0x01<<AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_2)));
				AeSubActionHandle=0;
			}
            if (ulActualEvents & (0x01<<AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_3)))
			{
                printf("\n~ IQ BW ~ \n");
                printf("~ IR HIGH ~ \n");
                sp5kModeGet(&mode);
                sp5kIqModeSet(mode);
                appIqEffectSet(UI_EFFECT_BW);
                sp5kIqModeSetDone(mode);
                sp5kSensorVvalidWait();
                sp5kSensorVvalidWait();
				appVideoIRLedCtrl(APP_IR_REVERSE_MODE, APP_IR_LED_LEVEL_HIGH);
                EVT_AE_SUB_CLR(ulActualEvents, (0x01<<AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_3)));
				AeSubActionHandle=0;
			}
		}
	}
#endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appAeSubThreadCreate
 *  Description :
 *------------------------------------------------------------------------*/
void appAeSubThreadCreate(void)
{
#if (AE_SUB_ACTION_FLOW==1)
	ptrdaesub = sp5kOsThreadCreate(
		"_aesubthread",
		_aesubthread,
		0,
		15,
		15,
		TX_NO_TIME_SLICE,
		TX_AUTO_START);

    if(ptrdaesub==NULL)
        HOST_ASSERT(ptrdaesub);
#endif
}

/*-------------------------------------------------------------------------
 *  Function Name : appAeSubAction
 *  Description :
 *------------------------------------------------------------------------*/
void appAeSubAction(void)
{
#if (AE_SUB_ACTION_FLOW==1)
    AEStatus_t AEStatus;
    static AEConfig_t    AEConfigOriginal;
    AEConfig_t    AEConfigFast;

    appAEALG_ParamGet(AE_PARA_STATUS,(SINT32*)&AEStatus);

    if(AEStatus.cbcnt>=3 && AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_NUM)>0 && AeSubActionHandle==0 && AeSubActionDelayVD==0)
    {
        if(AeSubActionMode==AE_SUBACTION_AUTO)
        {
            if(AeSubActionState==AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_OFF) && AEStatus.LVx1000<AE_SUB_ACTION_START_LV)
            {
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigOriginal);
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigFast);
                AEConfigFast.ConvergeSpeed = AE_SUB_SPEED;
                AEConfigFast.ConvergeSlope = AE_SUB_SLOPE;
                AEConfigFast.ConvergeLimit = AE_SUB_LIMIT;
                appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfigFast);

                AeSubActionState++;
                AeSubActionDelayVD=AE_SUB_DELAY_VD;
                AeSubActionHandle=1;
                sp5kOsEventFlagsSet(&evtaesub, (0x1<<AeSubActionState), TX_OR);
        }
            else if(AeSubActionState>AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_OFF) && AeSubActionState<AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_NUM) && AEStatus.LVx1000<AE_SUB_ACTION_CHANGE_LV)
            {
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigOriginal);
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigFast);
                AEConfigFast.ConvergeSpeed = AE_SUB_SPEED;
                AEConfigFast.ConvergeSlope = AE_SUB_SLOPE;
                AEConfigFast.ConvergeLimit = AE_SUB_LIMIT;
                appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfigFast);

                AeSubActionState++;
                AeSubActionDelayVD=AE_SUB_DELAY_VD;
                AeSubActionHandle=1;
                sp5kOsEventFlagsSet(&evtaesub, (0x1<<AeSubActionState), TX_OR);
        }
            else if(AeSubActionState>AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_OFF)  && AEStatus.LVx1000>AE_SUB_ACTION_START_LV+AE_SUB_ACTION_LV_DELTA)
        {
                WbAlgCalibGain_t gwgain;
                UINT16 rgain,bgain;

                appAWBALG_WBParamGet(AWB_PARA_GW,(UINT32*)&gwgain);

                rgain = gwgain.rg;
                bgain = gwgain.bg;

                if(rgain>AE_SUB_IRLED_RGAIN||bgain>AE_SUB_IRLED_BGAIN)
            {
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigOriginal);
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigFast);
                AEConfigFast.ConvergeSpeed = AE_SUB_SPEED;
                AEConfigFast.ConvergeSlope = AE_SUB_SLOPE;
                AEConfigFast.ConvergeLimit = AE_SUB_LIMIT;
                appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfigFast);

                    AeSubActionState=AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_OFF);
                AeSubActionDelayVD=AE_SUB_DELAY_VD;
                AeSubActionHandle=1;
                    sp5kOsEventFlagsSet(&evtaesub, (0x1<<AeSubActionState), TX_OR);
        }
                else if(AeSubActionState>AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_OFF)+1 && AEStatus.LVx1000>AE_SUB_ACTION_CHANGE_LV+AE_SUB_ACTION_LV_DELTA)
            {
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigOriginal);
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigFast);
                AEConfigFast.ConvergeSpeed = AE_SUB_SPEED;
                AEConfigFast.ConvergeSlope = AE_SUB_SLOPE;
                AEConfigFast.ConvergeLimit = AE_SUB_LIMIT;
                appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfigFast);

                    AeSubActionState--;
                AeSubActionDelayVD=AE_SUB_DELAY_VD;
                AeSubActionHandle=1;
                    sp5kOsEventFlagsSet(&evtaesub, (0x1<<AeSubActionState), TX_OR);
                }
            }
        }
        else if(AeSubActionMode==AE_SUBACTION_AUTO_LED)
        {
            if(AeSubActionState==AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_OFF))
            {
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigOriginal);
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigFast);
                AEConfigFast.ConvergeSpeed = AE_SUB_SPEED;
                AEConfigFast.ConvergeSlope = AE_SUB_SLOPE;
                AEConfigFast.ConvergeLimit = AE_SUB_LIMIT;
                appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfigFast);

                AeSubActionState++;
                AeSubActionDelayVD=AE_SUB_DELAY_VD;
                AeSubActionHandle=1;
                sp5kOsEventFlagsSet(&evtaesub, (0x1<<AeSubActionState), TX_OR);
        }
            else if(AeSubActionState<AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_NUM) && AEStatus.LVx1000<AE_SUB_ACTION_CHANGE_LV)
        {
                 appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigOriginal);
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigFast);
                AEConfigFast.ConvergeSpeed = AE_SUB_SPEED;
                AEConfigFast.ConvergeSlope = AE_SUB_SLOPE;
                AEConfigFast.ConvergeLimit = AE_SUB_LIMIT;
                appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfigFast);

                AeSubActionState++;
                AeSubActionDelayVD=AE_SUB_DELAY_VD;
                AeSubActionHandle=1;
                sp5kOsEventFlagsSet(&evtaesub, (0x1<<AeSubActionState), TX_OR);
            }
            else if(AeSubActionState>AE_SUB_ACTION_LED_STATE_CONTROL(AE_SUBACTION_LED_OFF)+1 && AEStatus.LVx1000>AE_SUB_ACTION_CHANGE_LV+AE_SUB_ACTION_LV_DELTA)
            {
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigOriginal);
                appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigFast);
                AEConfigFast.ConvergeSpeed = AE_SUB_SPEED;
                AEConfigFast.ConvergeSlope = AE_SUB_SLOPE;
                AEConfigFast.ConvergeLimit = AE_SUB_LIMIT;
                appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfigFast);

                AeSubActionState--;
                AeSubActionDelayVD=AE_SUB_DELAY_VD;
                AeSubActionHandle=1;
                sp5kOsEventFlagsSet(&evtaesub, (0x1<<AeSubActionState), TX_OR);
            }
            }
        else if(AeSubActionMode<=AE_SUBACTION_LED_NUM&& AeSubActionState!=AE_SUB_ACTION_LED_STATE_CONTROL(AeSubActionMode))
        {
            appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigOriginal);
            appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfigFast);
            AEConfigFast.ConvergeSpeed = AE_SUB_SPEED;
            AEConfigFast.ConvergeSlope = AE_SUB_SLOPE;
            AEConfigFast.ConvergeLimit = AE_SUB_LIMIT;
            appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfigFast);

            AeSubActionState=AE_SUB_ACTION_LED_STATE_CONTROL(AeSubActionMode);
            AeSubActionDelayVD=AE_SUB_DELAY_VD;
            AeSubActionHandle=1;
            sp5kOsEventFlagsSet(&evtaesub, (0x1<<AeSubActionState), TX_OR);
        }
    }
    else if(AeSubActionHandle==0 && AeSubActionDelayVD>0)
    {
        AeSubActionDelayVD--;

        if(AeSubActionDelayVD==0)
        {
            appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfigOriginal);
        }
    }
#endif
}

/*-------------------------------------------------------------------------
 * Function : appAeSubActionMode
 *------------------------------------------------------------------------*/
void appAeSubActionMode(UINT8 mode)
{
#if (AE_SUB_ACTION_FLOW==1)
    AeSubActionMode =  mode;
#endif
}
/*-------------------------------------------------------------------------
 * Function : appAeCbGet
 *------------------------------------------------------------------------*/
appAeCb_t *appAeCbGet(void)
{
	return &aeCb;
}

/*-------------------------------------------------------------------------
 * Function : appAeParaGet
 *------------------------------------------------------------------------*/
appAePara_t *appAeParaGet(void)
{
	return &aePara;
}

/*-------------------------------------------------------------------------
 * Function : appAeStableGet
 *------------------------------------------------------------------------*/
UINT8 appAeStableGet(void)
{
       UINT8 Stable;

	appAEALG_ParamGet(AE_PARA_STABLE,(SINT32*)&Stable);

	return Stable;
}

void appAeStableSet(void)
{
}

void appAeStableReset(void)
{
}

void
appAeStabilizerSet(
	UINT8 stabilizer
)
{
	if(stabilizer < UI_STABILIZER_MAX)
	{
		aeCb.aeset.stabilizer = stabilizer;
	}
}

SINT32 appPreviewAeBvIdxGet(void)
{
	AEStatus_t AEStatus;
	appAEALG_ParamGet(AE_PARA_STATUS,(SINT32*)&AEStatus);

	return AEStatus.LastBV;
}

/*-------------------------------------------------------------------------
 * Function : appAeLumaGet
 *------------------------------------------------------------------------*/
UINT16 appAeLumaGet(void)
{
	return aeCb.luma;
}

/*-------------------------------------------------------------------------
 * Function : appAeSetGet
 *------------------------------------------------------------------------*/
appAeSet_t *appAeSetGet(
	void
)
{
	return (appAeSet_t *)&aeCb.aeset;
}

SINT32 appAeMinCapGainGet(void)
{
#if 0
	return capSatAgc;
#else
    return 0;
#endif
}

void appAePreviewProgramChartChange(void)
{
      UINT32 uFileSize;

	if(aeFrameRateSel == 60)
	{
           if(aeFlickerSel==0)/*0:50Hz , 1:60HZ*/
           {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART50HZ60F, (UINT8 *)(AEProgramChart_PV), sizeof(AEProgramChart_PV), 0, 0 );
           }
	    else
	    {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART60HZ60F, (UINT8 *)(AEProgramChart_PV), sizeof(AEProgramChart_PV), 0, 0 );
	    }
	}
	else if(aeFrameRateSel == 10)
	{
           if(aeFlickerSel==0)/*0:50Hz , 1:60HZ*/
           {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART50HZ10F, (UINT8 *)(AEProgramChart_PV), sizeof(AEProgramChart_PV), 0, 0 );
           }
	    else
	    {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART60HZ10F, (UINT8 *)(AEProgramChart_PV), sizeof(AEProgramChart_PV), 0, 0 );
	    }
	}
	else if(aeFrameRateSel == 15)
	{
           if(aeFlickerSel==0)/*0:50Hz , 1:60HZ*/
           {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART50HZ15F, (UINT8 *)(AEProgramChart_PV), sizeof(AEProgramChart_PV), 0, 0 );
           }
	    else
	    {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART60HZ15F, (UINT8 *)(AEProgramChart_PV), sizeof(AEProgramChart_PV), 0, 0 );
	    }
	}
	else if(aeFrameRateSel == 30)
	{
           if(aeFlickerSel==0)/*0:50Hz , 1:60HZ*/
           {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART50HZ30F, (UINT8 *)(AEProgramChart_PV), sizeof(AEProgramChart_PV), 0, 0 );
           }
	    else
	    {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART60HZ30F, (UINT8 *)(AEProgramChart_PV), sizeof(AEProgramChart_PV), 0, 0 );
	    }
	}
	else if(aeFrameRateSel == 240)
	{
		    uFileSize = LoadFromFile((UINT8*)AEPCHART240F, (UINT8 *)(AEProgramChart_PV), sizeof(AEProgramChart_PV), 0, 0 );
	}
	else
	{
		    uFileSize = LoadFromFile((UINT8*)AEPCHART120F, (UINT8 *)(AEProgramChart_PV), sizeof(AEProgramChart_PV), 0, 0 );
	}

	    if( uFileSize != sizeof(AEProgramChart_PV) )
	    {
	        printf( "AE2.0 Err:Fail to load PV program chart!! %u != %zu\n",uFileSize, sizeof(AEProgramChart_PV) );
		return;
	    }

	appAEALG_ParamSet(AE_PARA_PROGRAMCHART_PV, (SINT32)AEProgramChart_PV);

}

void appAeCaptureProgramChartChange(void)
{
      UINT32 uFileSize;

	if(aeCb.aeset.iso == UI_ISO_AUTO)
	{
	       if(aeFlickerSel==0)/*0:50Hz , 1:60HZ*/
	       {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART50HZCAP, (UINT8 *)(AEProgramChart_CAP), sizeof(AEProgramChart_CAP), 0, 0 );
	       }
	    else
	    {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART60HZCAP, (UINT8 *)(AEProgramChart_CAP), sizeof(AEProgramChart_CAP), 0, 0 );
	    }
	}
	else
	{
	       if(aeFlickerSel==0)/*0:50Hz , 1:60HZ*/
	       {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART50HZISO100, (UINT8 *)(AEProgramChart_CAP), sizeof(AEProgramChart_CAP), 0, 0 );
	       }
	    else
	    {
		    uFileSize = LoadFromFile((UINT8*)AEPCHART60HZISO100, (UINT8 *)(AEProgramChart_CAP), sizeof(AEProgramChart_CAP), 0, 0 );
	    }

	    UINT32 SVDiff=0, i=0;

	    SVDiff = (aeCb.aeset.iso - UI_ISO_100)*16;

	   for(i=0;i<AE_PROGRAM_CHART_SIZE;i++)
	   {
	       if(AEProgramChart_CAP[i].AV!=0&&AEProgramChart_CAP[i].TV!=0&&AEProgramChart_CAP[i].SV!=0)
		   	AEProgramChart_CAP[i].SV+=SVDiff;
	   }
	}

    if( uFileSize != sizeof(AEProgramChart_CAP) )
    {
        printf( "AE2.0 Err:Fail to load CAP program chart!! %u != %zu\n",uFileSize, sizeof(AEProgramChart_CAP) );
	return;
    }

	appAEALG_ParamSet(AE_PARA_PROGRAMCHART_CAP, (SINT32)AEProgramChart_CAP);
}

void appAeMDCaptureProgramChartChange(void)
{
	if(aeCb.aeset.iso == UI_ISO_AUTO)
	{
		UINT32 uFileSize;

		if(aeFlickerSel==0)/*0:50Hz , 1:60HZ*/
		{
			uFileSize = LoadFromFile((UINT8*)AEPCHART50HZCAPMD, (UINT8 *)(AEProgramChart_CAP), sizeof(AEProgramChart_CAP), 0, 0 );
		}
		else
		{
			uFileSize = LoadFromFile((UINT8*)AEPCHART60HZCAPMD, (UINT8 *)(AEProgramChart_CAP), sizeof(AEProgramChart_CAP), 0, 0 );
		}

		if( uFileSize != sizeof(AEProgramChart_CAP) )
			printf( "AE2.0 Err:Fail to load CAP MD program chart!! %u != %zu\n",uFileSize, sizeof(AEProgramChart_CAP) );
		else
			appAEALG_ParamSet(AE_PARA_PROGRAMCHART_CAP, (SINT32)AEProgramChart_CAP);
	}
}

/*-------------------------------------------------------------------------
 * Function : appAeZoomLevelSet
 *------------------------------------------------------------------------*/
void
appAeZoomLevelSet(
	UINT32 zml
)
{
	if(zml < AE_ZOOM_LEVEL_MAX)
	{
		aeCb.zml = zml;
	}
}

/*-------------------------------------------------------------------------
 * Function : appAeCapShutterSet
 *------------------------------------------------------------------------*/
void
appAeCapShutterSet(
	UINT8 shutidx
)
{
	if(shutidx < SHUTTER_MAX)
	{
		aeCb.aeset.shutter = shutidx;
	}
}

/*-------------------------------------------------------------------------
 * Function : appAeCapIsoSet
 *------------------------------------------------------------------------*/
void
appAeCapIsoSet(
	UINT8 isoidx
)
{
    HOST_PROF_LOG_PRINT(LEVEL_INFO,"ae: aeCapIsoSet: %d",isoidx);

	if (isoidx >= UI_ISO_MAX)
	{
        HOST_PROF_LOG_ADD(LEVEL_ERROR,"ae: invalid ISO requested");
		return;
	}
	aeCb.aeset.iso = isoidx;

	appAeCaptureProgramChartChange();
}

/*-------------------------------------------------------------------------
 * Function : appAeIrisSet
 *------------------------------------------------------------------------*/
UINT32 appAeIrisSet(UINT8 irisStop)
{
#if KIT_WITH_IRIS
	UINT32 ret = SUCCESS, curStop;

	curStop = appLensQuery(LENS_QUERY_IRIS_SEG_POS);

	if (curStop != irisStop)
	{
		if(irisStop==0)
		{
			sp5kSensorVvalidWait();
			DBG_PRINT("ae: %s: Large IRIS\n", __FUNCTION__);
			ret = appIrisLargeSet();
		}
		else if (irisStop==1)
		{
			sp5kSensorVvalidWait();
			DBG_PRINT("ae: %s: Small IRIS\n", __FUNCTION__);
		   	ret = appIrisSmallSet();
		}
		else
		{
			DBG_PRINT("ae: %s: Illegal IRIS\n", __FUNCTION__);
			ret = FAIL;
		}
	}

	if (ret==SUCCESS)
	{
		DBG_PRINT("ae: %s: declare IRIS (%d)\n", __FUNCTION__, aeIris[irisStop]);
		sp5kAeApertureDeclare(aeIris[irisStop]);
		aePara.ui_iris=aeCb.iris_stop = irisStop;
	}

	return ret;
#else
	return SUCCESS;
#endif
}

/*-------------------------------------------------------------------------
 * Function : appAeCapIrisSet
 *------------------------------------------------------------------------*/
void
appAeCapIrisSet(
	UINT8 irisidx
)
{
	if( irisidx < APERTURE_MAX)
	{
		aeCb.aeset.iris = irisidx;
		if(irisidx == APERTURE_AUTO)
		{
			appAeIrisSet(APERTURE_2_8-1);
		}
		else
		{
			appAeIrisSet(irisidx-1);
		}
	}
}

/*-------------------------------------------------------------------------
 * Function : appAeMeterSet
 *------------------------------------------------------------------------*/
void
appAeMeterSet(
	UINT8 bMeter
)
{
	if(bMeter<AE_METERING_MAX)
	{
	    UINT32 uFileSize;
	    aeCb.aeset.meter = bMeter;

           if(bMeter==AE_METERING_SPOT)
           {
		    uFileSize = LoadFromFile( (UINT8*)AEWINDOWSPOT, (UINT8 *)(AEWindow), sizeof(AEWindow), 0, 0 );

		    if( uFileSize != sizeof(AEWindow) )
		    {
		        printf( "AE2.0 Err:Fail to load %s !! %u != %zu\n",(char*)AEWINDOWSPOT,uFileSize, sizeof(AEWindow) );
			return;
		    }
           }
	    else if(bMeter==AE_METERING_CENTER)
           {
		    uFileSize = LoadFromFile((UINT8*)AEWINDOWCENTER, (UINT8 *)(AEWindow), sizeof(AEWindow), 0, 0 );

		    if( uFileSize != sizeof(AEWindow) )
		    {
		        printf("AE2.0 Err:Fail to load %s !! %u != %zu\n",(char*)AEWINDOWCENTER,uFileSize, sizeof(AEWindow) );
			return;
		    }
           }
           else
           {
		    uFileSize = LoadFromFile((UINT8*)AEWINDOWMULTI, (UINT8 *)(AEWindow), sizeof(AEWindow), 0, 0 );

		    if( uFileSize != sizeof(AEWindow) )
		    {
		        printf( "AE2.0 Err:Fail to load %s !! %u != %zu\n",(char*)AEWINDOWMULTI ,uFileSize, sizeof(AEWindow) );
			return;
		    }
           }

           appAEALG_ParamSet(AE_PARA_WINDOW, (SINT32)AEWindow);

	}
}

/*-------------------------------------------------------------------------
 * Function : appAeEvBiasSet
 *------------------------------------------------------------------------*/
void
appAeEvBiasSet(
	UINT8 bEvBias
)
{
	if(bEvBias< AE_BIAS_MAX)
	{
	    aeCb.aeset.evbias = bEvBias;
	    aebiascmp = aebiascmptbl[bEvBias];
	    aeevset = aebiascmp;

	    appAEALG_ParamSet(AE_PARA_EVSET, (SINT32)aeevset);

	}
}

/*-------------------------------------------------------------------------
 * Function : appAeTargetFactorSet
 *------------------------------------------------------------------------*/
void
appAeTargetFactorSet(UINT8 FactorIdx)
{
    UINT32 uFileSize;
    SINT32 i;
    UINT32 TargetFactor1[AE_TARGET_LV_RANGE] = {800,800,800,800,800,800,800,800,912,1024,1024,1024,1024,1024,1024,1024,1024,1024};
    UINT32 TargetFactor2[AE_TARGET_LV_RANGE] = {600,600,600,600,600,600,600,600,812,1024,1024,1024,1024,1024,1024,1024,1024,1024};
    UINT32 TargetFactor3[AE_TARGET_LV_RANGE] = {400,400,400,400,400,400,400,400,712,1024,1024,1024,1024,1024,1024,1024,1024,1024};

    uFileSize = LoadFromFile((UINT8*)AETARGET, (UINT8 *)(AETarget), sizeof(AETarget), 0, 0 );

    if( uFileSize != sizeof(AETarget) )
    {
        printf( "AE2.0 Err:Fail to load %s !! %u != %zu\n",AETARGET, uFileSize, sizeof(AETarget) );
    }
    else if(FactorIdx==1)
    {
        for(i=0;i<AE_TARGET_LV_RANGE;i++)
        {
            AETarget[i] = AETarget[i]*TargetFactor1[i]/1024;
        }
    }
    else if(FactorIdx==2)
    {
        for(i=0;i<AE_TARGET_LV_RANGE;i++)
        {
            AETarget[i] = AETarget[i]*TargetFactor2[i]/1024;
        }
    }
    else if(FactorIdx==3)
    {
        for(i=0;i<AE_TARGET_LV_RANGE;i++)
        {
            AETarget[i] = AETarget[i]*TargetFactor3[i]/1024;
        }
    }
    appAEALG_ParamSet(AE_PARA_TARGET, (SINT32)AETarget);
}


/*-------------------------------------------------------------------------
 * Function : appAePreviewHighISOBoundarySet
 *------------------------------------------------------------------------*/
void
appAePreviewHighISOBoundarySet(UINT32 SV)
{
    if(SV==0xffff)
        return;

    appAEALG_ParamGet(AE_PARA_PROGRAMCHART_PV,(SINT32*)AEProgramChart_PV);

    printf("PV pchart[0] : AV = %d , TV = %d , SV= %d\n",AEProgramChart_PV[0].AV,AEProgramChart_PV[0].TV,AEProgramChart_PV[0].SV);
    printf("PV pchart[1] : AV = %d , TV = %d , SV= %d\n",AEProgramChart_PV[1].AV,AEProgramChart_PV[1].TV,AEProgramChart_PV[1].SV);
    printf("PV high ISO boundary set = %u\n",SV);

    if(AEProgramChart_PV[0].SV>=AEProgramChart_PV[1].SV && AEProgramChart_PV[0].AV==AEProgramChart_PV[1].AV &&
		AEProgramChart_PV[0].TV==AEProgramChart_PV[1].TV && SV>AEProgramChart_PV[1].SV)
    {
        aePVHighISO = SV;
        AEProgramChart_PV[0].SV = SV;

        printf("PV pchart[0] : AV = %d , TV = %d , SV= %d\n",AEProgramChart_PV[0].AV,AEProgramChart_PV[0].TV,AEProgramChart_PV[0].SV);
        printf("PV pchart[1] : AV = %d , TV = %d , SV= %d\n",AEProgramChart_PV[1].AV,AEProgramChart_PV[1].TV,AEProgramChart_PV[1].SV);

        appAEALG_ParamSet(AE_PARA_PROGRAMCHART_PV, (SINT32)AEProgramChart_PV);
    }
    else
    {
        printf("PV high ISO boundary set fail!\n");
    }
}

/*-------------------------------------------------------------------------
 * Function : appAeCaptureHighISOBoundarySet
 *------------------------------------------------------------------------*/
void
appAeCaptureHighISOBoundarySet(UINT32 SV)
{
    if(SV==0xffff||aeCb.aeset.iso != UI_ISO_AUTO)
        return;

    appAEALG_ParamGet(AE_PARA_PROGRAMCHART_CAP,(SINT32*)AEProgramChart_CAP);

    printf("CAP pchart[0] : AV = %d , TV = %d , SV= %d\n",AEProgramChart_CAP[0].AV,AEProgramChart_CAP[0].TV,AEProgramChart_CAP[0].SV);
    printf("CAP pchart[1] : AV = %d , TV = %d , SV= %d\n",AEProgramChart_CAP[1].AV,AEProgramChart_CAP[1].TV,AEProgramChart_CAP[1].SV);
    printf("CAP high ISO boundary set = %u\n",SV);

    if(AEProgramChart_CAP[0].SV>=AEProgramChart_CAP[1].SV && AEProgramChart_CAP[0].AV==AEProgramChart_CAP[1].AV &&
		AEProgramChart_CAP[0].TV==AEProgramChart_CAP[1].TV && SV>AEProgramChart_CAP[1].SV)
    {
        aeCAPHighISO = SV;
        AEProgramChart_CAP[0].SV = SV;

        printf("CAP pchart[0] : AV = %d , TV = %d , SV= %d\n",AEProgramChart_CAP[0].AV,AEProgramChart_CAP[0].TV,AEProgramChart_CAP[0].SV);
        printf("CAP pchart[1] : AV = %d , TV = %d , SV= %d\n",AEProgramChart_CAP[1].AV,AEProgramChart_CAP[1].TV,AEProgramChart_CAP[1].SV);

        appAEALG_ParamSet(AE_PARA_PROGRAMCHART_CAP, (SINT32)AEProgramChart_CAP);
    }
    else
    {
        printf("CAP high ISO boundary set fail!\n");
    }
}

/*-------------------------------------------------------------------------
 * Function : appAeAebSet
 *------------------------------------------------------------------------*/
void
appAeAebSet(
	UINT8 aeb
)
{
	DBG_PRINT("ae: appAeAebSet(%d)\n",aeb);
	aeCb.aeset.aeb = aeb;
}

/*-------------------------------------------------------------------------
 * Function : appAeFrameRateSet
 *------------------------------------------------------------------------*/
void appAeFrameRateSet(UINT32 fps)
{
	if(fps > 120){
		aeFrameRateSel = 240;
	}
	else if(fps > 60){
		aeFrameRateSel = 120;
	}
	else if(fps > 30){
		aeFrameRateSel = 60;
	}
	else if(fps > 15){
		aeFrameRateSel = 30;
	}
	else if(fps > 10){
		aeFrameRateSel = 15;
	}
	else{
		aeFrameRateSel = 10;
	}

       appAePreviewProgramChartChange();
	appAeCaptureProgramChartChange();

	appAePreviewHighISOBoundarySet(aePVHighISO);
	appAeCaptureHighISOBoundarySet(aeCAPHighISO);
}
/*-------------------------------------------------------------------------
 * Function : appAeExpFlickerModeSet
 *------------------------------------------------------------------------*/
void
appAeExpFlickerModeSet(
	UINT8 flickeridx
)
{
	aeFlickerSel = flickeridx;/*0:50Hz , 1:60HZ*/

       appAePreviewProgramChartChange();
	appAeCaptureProgramChartChange();

	appAePreviewHighISOBoundarySet(aePVHighISO);
	appAeCaptureHighISOBoundarySet(aeCAPHighISO);
}

void
appAeMDEnable(UINT8 bEnable)
{
	aeMDEnable = bEnable;
}


void
appAeMDSet(UINT8 bDetect)
{
	aeMD = bDetect;
}


/*-------------------------------------------------------------------------
 * Function : appAePvReset
 *------------------------------------------------------------------------*/
void
appAePvReset(
	void
)
{

}


/*-------------------------------------------------------------------------
 * Function : appAePreviewSet
 *------------------------------------------------------------------------*/
void appAePreviewSet(void)
{
#if 0
	SINT32 expidx;
	UINT32 agcidx;

	appAEPreviewExpAgcGet(&expidx,& agcidx);
	appAEPreviewExpAgcSet(expidx, agcidx,0);
#endif
}

void appAeEnable(BOOL fEnable)
{
	if(fEnable)
	    appAEALG_ParamSet(AE_PARA_LOCK, 0);
       else
	   appAEALG_ParamSet(AE_PARA_LOCK, 1);
}

/*-------------------------------------------------------------------------
 * Function : appAePreviewRestore
 *------------------------------------------------------------------------*/
void appAePreviewRestore(void)
{
       appAEALG_ParamSet(AE_PARA_LOCK, 0);
}

/*-------------------------------------------------------------------------
 * Function : appAeOsdDbgShow
 *------------------------------------------------------------------------*/
#if AE_OSD_DBG_MSG
void appAeOsdDbgShow(
    UINT32 row,
    const char *format, ...
)
{
    UINT32 textLen;
    unsigned char buf[16];
    va_list	ap;

    if (row >= 8)
    {
        HOST_PROF_LOG_ADD(LEVEL_ERROR,"ae: raw size overflow");
        return;
    }
    va_start(ap, format);
    vsprintf(buf,format,ap);
    va_end(ap);

 	textLen = strlen(buf);
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 30, (row*30), (textLen+1)*18, 32, 0, 0);

    appOsd_ColorDrawTextColorSet(PAL_YELLOW, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
    appOsdLib_TextDraw(30, row*30, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)buf);
}
#endif

/*-------------------------------------------------------------------------
 * Function : appAeOsdDbg
 *------------------------------------------------------------------------*/
void appAeOsdDbg(void)
{
    #if AE_OSD_DBG_MSG
    appAeOsdDbgShow(1, "Luma:%d", aeCb.luma);
    appAeOsdDbgShow(2, "lv:%d", aeCb.lvidx);
	appAeOsdDbgShow(3, "expidx:%d", expidx);
    appAeOsdDbgShow(4, "agcidx:%d", agcidx);
    appAeOsdDbgShow(5, "ResStep:%d", 0);
    appAeOsdDbgShow(6, "step:%d", nextstep);
    #endif
}

/*-------------------------------------------------------------------------
 * Function : appAeInfoPrint
 *------------------------------------------------------------------------*/
void appAeInfoPrint(const aaaProcInfo_t *pinfo)
{
	if(aeinfoprint==1)
	{
              UINT32    i, j;
              printf("\n");
	       for(i=0;i<pinfo->aewin_hgrid ;i++)
	       {
		       for(j=0;j<pinfo->aewin_vgrid ;j++)
		       {
		           printf("%5d,",pinfo->paewin_y[i*pinfo->aewin_vgrid+j] );
		       }
			printf("\n");
	       }
		   aeinfoprint=0;
	}
}

/*-------------------------------------------------------------------------
 * Function : appAeInfoPrintDual
 *------------------------------------------------------------------------*/
void appAeInfoPrintDual(const aaaProcInfo_t *pinfo,UINT8 sensorsel)
{
	if(aeinfoprintdual==1)
	{      
		static UINT8 bSenPrint=0;
		printf("\n");

		if(sensorsel == bSenPrint)
		{
			UINT32    i, j ;
			printf("Sen%d\n",sensorsel);
		       for(i=0;i<pinfo->aewin_hgrid ;i++)
		       {
			       for(j=0;j<pinfo->aewin_vgrid ;j++)
			       {
			           printf("%5d,",pinfo->paewin_y[i*pinfo->aewin_vgrid+j] );
			       }
				printf("\n");
		       }

			bSenPrint++;
		}

		   if(bSenPrint==2)
		   {
		   	bSenPrint=0;
		   	aeinfoprintdual=0;
		   }
	}
}

/*-------------------------------------------------------------------------
 * Function : appAeStatusPrint
 *------------------------------------------------------------------------*/
void appAeStatusPrint(UINT8 sensor_id)
{
    AEStatus_t AEStatus;
    UINT8 sensor_id_sel;

    appAEALG_ParamGet(AE_PARA_SENSOR_ID,(SINT32*)&sensor_id_sel);

    if(sensor_id_sel==AE_SENSOR_ID_BOTH)
        sensor_id_sel=AE_SENSOR_ID_PRI;

    if(aestatusprintvdcnt>0&&sensor_id_sel==sensor_id)
    {
        aestatusprintvdcnt--;

        appAEALG_ParamGet(AE_PARA_STATUS,(SINT32*)&AEStatus);

        printf("\nsen%d:LVx1000=%d,Luma=%d,Target=%d,EXP=%d,AGC=%d,ACCUR=%d\n",sensor_id,AEStatus.LVx1000,AEStatus.Luma,AEStatus.Target,AEStatus.expidx,AEStatus.agcidx,AEStatus.accuridx);
    }
}

/*-------------------------------------------------------------------------
 * Function : appAEObjectWindow
 *------------------------------------------------------------------------*/
void appAEObjectWindow(UINT8 x,UINT8 y,UINT8 h,UINT8 v,UINT8 weight)
{
    if(x==0&&y==0&&(h==0||v==0))
    {
        appAEALG_ParamSet(AE_PARA_WINDOW, (SINT32)AEWindow);
    }
    else
    {
        UINT32    i, j;
        for(j=0;j<AE_WINDOW_V_SIZE;j++)
        {
            for(i=0;i<AE_WINDOW_H_SIZE;i++)
            {
                if(i>=x&&i<x+h&&j>=y&&j<y+v)
                   	AEWindowTemp[j*AE_WINDOW_H_SIZE+i]=weight;
                else
                     AEWindowTemp[j*AE_WINDOW_H_SIZE+i]=1;
            }
        }

        appAEALG_ParamSet(AE_PARA_WINDOW, (SINT32)AEWindowTemp);
    }
}

/*-------------------------------------------------------------------------
 * Function : appAeCustCaseAdjust
 *------------------------------------------------------------------------*/
void appAeCustCaseAdjust(void)
{

    #if (SP5K_SENSOR_MI5100p || SP5K_SENSOR_MI5100)
    UINT32 mode;
    sp5kAeCustomSts_t senCb;
    mode = appActiveStateGet();
    if(IS_VIEW_GROUP(mode) || IS_VIDEO_GROUP(mode))
    {
        SINT32 exp;
	    UINT32 agc;
        appAEPreviewExpAgcGet(&exp,&agc);
        senCb.agcIdx = agc;
        senCb.expIdx = exp;
        senCb.irise  = 48;

        switch(mode)
        {
            case APP_STATE_STILL_PREVIEW:
                mode = SP5K_MODE_STILL_PREVIEW;
                senCb.isovalue = _IqGetCurrISO(OPSTATE_PV);
            break;

            case APP_STATE_STILL_CAPTURE:
                mode = SP5K_MODE_STILL_SNAP;
                senCb.isovalue = _IqGetCurrISO(OPSTATE_SNAP);
            break;

            case APP_STATE_VIDEO_PREVIEW:
                mode = SP5K_MODE_VIDEO_PREVIEW;
                senCb.isovalue = _IqGetCurrISO(OPSTATE_PV);
            break;

            case APP_STATE_VIDEO_REC:
                mode = SP5K_MODE_VIDEO_RECORD;
                senCb.isovalue = _IqGetCurrISO(OPSTATE_VIDEO);
            break;
        }

        senCb.lv = aeCb.lvidx* 1000 /evUnit.evUnit;//aeCb.lvidx <=0  ? 0 : (aeCb.lvidx * 1000 /evUnit.evUnit);
        sp5kAeStsSet(mode ,senCb);

        #if 0
        printf_t("ae: lv=%d %d, iris=%d,exp=%d agc=%d iso=%d\n",aeCb.lvidx, senCb.lv,senCb.irise,\
               senCb.expIdx, senCb.agcIdx, senCb.isovalue);
        #endif
    }
    #endif
}

#if SP5K_DIQ_HDR
/*-------------------------------------------------------------------------
 * Function : appAeHDRCal
 *------------------------------------------------------------------------*/
UINT8
appappAeHDRCal(
	SINT32 expIdx,SINT32 agcIdx, SINT8 ev
)
{
	SINT32 exp_idx;
	/*SINT32 agc_idx;*/
	UINT16 ev_entry1=0, ev_entry2=0;
	UINT16 entry_diff1=0, entry_diff2=0;


	if(ev>30 || ev<0){
		printf("Error!! Check EV range in 0~2.0 (ex:EV1.5 ->15, EV2.0 ->20)\n");
		return FAIL;
	}
	if(expIdx<HDR_MinExpIdx || expIdx>HDR_MaxExpIdx)
	{
		printf("Error!! Check exp range  AE_Expidx:%d~%d exp:%d agc:%d\n",HDR_MinExpIdx,HDR_MaxExpIdx,expIdx,agcIdx);
		return FAIL;
	}
	if(agcIdx<HDR_MinAgcIdx || agcIdx>HDR_MaxAgcIdx)
	{
		printf("Error!! Check agc range  AE_Agcidx:%d~%d  exp:%d agc:%d\n",HDR_MinAgcIdx,HDR_MaxAgcIdx,expIdx,agcIdx);
		return FAIL;
	}

	exp_idx = expIdx;
	/*agc_idx = agcIdx;*/

	sensor_HDR_ev_set(ev);

	ev_entry1 = (ev*16+5)/10;   /*rounding (ev*16)/10*/
	if(exp_idx+ev_entry1 > HDR_MaxExpIdx)
	{
		entry_diff1 = (exp_idx+ev_entry1)-HDR_MaxExpIdx;
	}

	/*check HDR exp & agc index*/
	#if 0
	if(exp_idx+ev_entry1 > HDR_MaxExpIdx)
		printf("Normal:%d %d -ev:%d %d\n",expIdx,agcIdx,HDR_MaxExpIdx,agcIdx+entry_diff1);
	else
		printf("Normal:%d %d -ev:%d %d\n",expIdx,agcIdx,exp_idx+ev_entry1,agcIdx+entry_diff1);
	#endif

	ev_entry2 = (2*ev*16+5)/10; /*rounding ((2*ev*16)/10*/
	if(exp_idx+ev_entry2 > HDR_MaxExpIdx)
	{
		entry_diff2 = (exp_idx+ev_entry2)-HDR_MaxExpIdx;
	}
	if(entry_diff1 > 0 || entry_diff2>0)
		sensor_HDR_ev_diff_set(entry_diff1,entry_diff2);
	/*appAEStillCapExpAgcSet((SINT32)exp_idx, agc_idx,0);*/

	return SUCCESS;
}

void appAeHDREvSet(SINT32 HDRev)
{
	aeHDRev = HDRev;
}

void appAeHDRALGEnable(UINT8 bEnable)
{
	appAEALG_ParamSet(AE_PARA_HDR, (SINT32)bEnable);
}
#endif

void appAeHighPriority(UINT8 bEnable)
{
    static UINT8 bAATaskPriorityChanged=FALSE;
    AEStatus_t AEStatus;
    SINT32 ret=0;

    if(bEnable==TRUE && bAATaskPriorityChanged==FALSE)
    {
        ret=sp5kAeCfgSet(SP5K_AE_AA_TASK_PRIORITY_CHANGE,16);

        if(ret==SUCCESS)
            bAATaskPriorityChanged=TRUE;
    }
    else if(bEnable==FALSE && bAATaskPriorityChanged==TRUE)
    {
        appAEALG_ParamGet(AE_PARA_STATUS,(SINT32*)&AEStatus);

        if(AEStatus.cbcnt>3)
        {
            ret=sp5kAeCfgSet(SP5K_AE_AA_TASK_PRIORITY_CHANGE,20);

            if(ret==SUCCESS)
                bAATaskPriorityChanged=FALSE;
        }
    }
}

/*-------------------------------------------------------------------------
 * Function : appAePreAdjustCB
 *------------------------------------------------------------------------*/
void
appAePreAdjustCB(const aaaProcInfo_t *pinfo, aaaProcResult_t *presult,UINT8 sensorsel ,UINT32 mode )
{
}

/*-------------------------------------------------------------------------
 * Function : appAeAdjustCB
 *------------------------------------------------------------------------*/
void
appAeAdjustCB(const aaaProcInfo_t *pinfo, aaaProcResult_t *presult,UINT8 sensor_id ,UINT32 mode )
{
    AEStatus_t AEStatus;

    appAeInfoPrint(pinfo);
    appAeInfoPrintDual(pinfo,sensor_id);
    appAeStatusPrint(sensor_id);

    appAEALG_ParamGet(AE_PARA_STATUS,(SINT32*)&AEStatus);

    aeCb.luma = AEStatus.Luma;
    aeCb.lvidx = AEStatus.LVx1000*16/1000;
    aeCb.expidx = AEStatus.expidx;
    aeCb.agcidx= AEStatus.agcidx;
    aeCb.avidx= AEStatus.aperidx;
    aeCb.capexp= AEStatus.expidxCAP;
    aeCb.capagc= AEStatus.agcidxCAP;
    aeCb.capav= AEStatus.aperidxCAP;

    appAeSubAction();

#if SP5K_DIQ_FRMWRK
    sp5kDynIqCfgSet(SP5K_DYN_IQ_CFG_AE_INFO_SET,pinfo,AEStatus.LVx1000);
#endif

    appAeCustCaseAdjust();

#if SP5K_DIQ_HDR
    if(sensorModeGet()==0x41 || sensorModeGet()==0x35 || sensorModeGet()==0x39 || sensorModeGet()==0x3A)
    appappAeHDRCal(AEStatus.expidx,AEStatus.agcidx,aeHDRev);
#endif

#if SP5K_ICAT_EIS
    appVideoRSCExpTimeGet();
#endif
}

/*-------------------------------------------------------------------------
 * Function : appAeReady
 *------------------------------------------------------------------------*/
void appAeReady(void)
{
    appAeHighPriority(FALSE);/*Set AA task priority default from high priority by appVideoSizeSet()*/
}

/*-------------------------------------------------------------------------
 * Function : appAeInit
 *------------------------------------------------------------------------*/
void appAeInit(void)
{
    UINT32 uFileSize;


	sp5kAeEvUnitGet(&evUnit);
	sp5kAeExpLimitGet(SP5K_SENSOR_MODE_PREVIEW, &expLimitPv);
	sp5kAeAgcLimitGet(SP5K_SENSOR_MODE_PREVIEW, &agcLimitPv);
	sp5kAeExpLimitGet(SP5K_SENSOR_MODE_SNAP, &expLimitCap);
	sp5kAeAgcLimitGet(SP5K_SENSOR_MODE_SNAP, &agcLimitCap);

	memset(&aeCb, 0, sizeof(appAeCb_t));
	memset(&aePara, 0, sizeof(appAePara_t));

    uFileSize = LoadFromFile((UINT8*)AECONFIG, (UINT8*)&AEConfig, sizeof(AEConfig_t), 0, 0 );
    if( uFileSize != sizeof(AEConfig_t) )
        printf( "AE2.0 Err:Fail to load %s !! %u != %zu\n",AECONFIG,uFileSize, sizeof(AEConfig_t) );
    else
	appAEALG_ParamSet(AE_PARA_CONFIG, (SINT32)&AEConfig);

    uFileSize = LoadFromFile((UINT8*)AEWINDOWLV, (UINT8 *)(AEWindowLV), sizeof(AEWindowLV), 0, 0 );
    if( uFileSize != sizeof(AEWindowLV) )
        printf( "AE2.0 Err:Fail to load %s !! %u != %zu\n",AEWINDOWLV ,uFileSize, sizeof(AEWindowLV) );
    else
	appAEALG_ParamSet(AE_PARA_WINDOW_LV, (SINT32)AEWindowLV);

    uFileSize = LoadFromFile((UINT8*)AETARGET, (UINT8 *)(AETarget), sizeof(AETarget), 0, 0 );
    if( uFileSize != sizeof(AETarget) )
        printf( "AE2.0 Err:Fail to load %s !! %u != %zu\n",AETARGET, uFileSize, sizeof(AETarget) );
    else
	appAEALG_ParamSet(AE_PARA_TARGET, (SINT32)AETarget);

     appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
	 aeCb.pviso100agc = AEConfig.ISO100AgcIdx;
	 aeCb.capiso100agc = AEConfig.ISO100AgcIdxCAP;

       appAEALG_ParamSet(AE_PARA_PRE_CB_REGISTER, (SINT32)appAePreAdjustCB);
       appAEALG_ParamSet(AE_PARA_CB_REGISTER, (SINT32)appAeAdjustCB);

#if (AE_SUB_ACTION_FLOW==1)
	appAeSubThreadCreate();
#endif
}

/*-------------------------------------------------------------------------
 * Function : cap_tvidx_to_ui_shutter_idx
 *------------------------------------------------------------------------*/
static SINT16 cap_expidx_to_ui_shutter_idx(SINT32 expidx)
{
	return expidx+47;
}

static SINT16 cap_agcidx_to_ui_shutter_idx(SINT32 agcidx)
{
       SINT16 uishutteridx;
	appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
       uishutteridx=(STD_ISO_100+(agcidx-AEConfig.ISO100AgcIdxCAP)-STD_ISO_50)/evUnit.evUnit;

	if(uishutteridx<0)
		uishutteridx=0;

	return (uishutteridx+UI_ISO_50);//base on exif string table
}

/*-------------------------------------------------------------------------
 * Function : appAeCapParaGet
 *------------------------------------------------------------------------*/
void
appAeCapParaGet(
	SINT16 *piris,
	SINT16 *pshutter,
	SINT16 *piso
)
{
       AEStatus_t AEStatus;
       appAEALG_ParamGet(AE_PARA_STATUS,(SINT32*)&AEStatus);


	*piris = aePara.ui_iris+APERTURE_2_8;
	*pshutter = cap_expidx_to_ui_shutter_idx(AEStatus.expidxCAP);
	*piso = cap_agcidx_to_ui_shutter_idx(AEStatus.agcidxCAP);

	DBG_PRINT("ae: appAeCapParaGet\n");
	DBG_PRINT("ae: iris = %d shutter = %d iso = %d\n",*piris,*pshutter,*piso);
}

/*-------------------------------------------------------------------------
 *  Function Name : appAeCaptureSet
 *------------------------------------------------------------------------*/
void appAeCaptureSet(UINT8 imgId)
{

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "ae: cap exp/agc set s");

	if(aeCb.aeset.aeb && imgId < 3)
	{
#if 0//AEB Flow
        HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "ae: *** AEB %d***",imgId);
		aePara.cap_tv = aeCb.capaebexp[imgId];
		aePara.cap_sv = aeCb.capaebagc[imgId];
		aeCb.capexp = aePara.cap_tv;
		aeCb.capagc = cap_svidx_to_agcidx(aePara.cap_sv);
		if(imgId == 0)
		{
			aeCb.capav = aePara.cap_av;
			appAeIrisSet(aePara.ui_iris);

			#if LENS_MOTOR_DRV_MD3_IN4
			APP_SLEEP_MS(40);
			#endif
		}
#endif
	}
	else
	{
              HOST_PROF_LOG_ADD(LEVEL_DEBUG, "ae: *** appAeCaptureSet ***");

		appAeIrisSet(aePara.ui_iris);

		#if LENS_MOTOR_DRV_MD3_IN4
		APP_SLEEP_MS(40);
		#endif
	}


       AEALGCurrentBVtoPreview();

    HOST_PROF_LOG_PRINT(LEVEL_DEBUG, "ae: cap exp/agc set e");
}

void appAeCaptureFlashAeOverrideSet(void)
{
}

/*-------------------------------------------------------------------------
 * Function : appAeGetCurrentExpIsoUiIdx
 *------------------------------------------------------------------------*/
void appAeGetCurrentExpIsoUiIdx(UINT16* ui_exp_idx, UINT16* ui_iso_idx)
{
    SINT32 EXP;
    UINT32 AGC;

    appAEStillCapExpAgcGet(&EXP,&AGC);

    *ui_exp_idx = cap_expidx_to_ui_shutter_idx(EXP);
    *ui_iso_idx = cap_agcidx_to_ui_shutter_idx(AGC);
}

/*-------------------------------------------------------------------------
 * Function : appAeFastAdjust
 *------------------------------------------------------------------------*/
UINT8 appAeFastAdjust(UINT32 param)
{
    UINT8 ret = FALSE;

    if(AEALGFastStableCheck()==0)
    {
        return 0xff;
    }

    appAEALG_ParamSet(AE_PARA_LOCK, 1);

#if SP5K_MOTION_DETECT
    if(aeMDEnable&&aeMD)
    {
        appAeMDCaptureProgramChartChange();
        appAeCaptureHighISOBoundarySet(aeCAPHighISO);
        AEALGCurrentBVtoCapture();
        appAeCaptureProgramChartChange();
        appAeCaptureHighISOBoundarySet(aeCAPHighISO);
    }
#endif
    /**********************************************************
    * Check up safeshut to decide if Flash compensation needed
    **********************************************************/

    if(AEALGFlashTriggerCheck())
    {
        ret = aePara.cap_flash = TRUE; /*-- AE_FASTAE_OK_FLASH_ON */
    }
    else
    {
        ret = aePara.cap_flash = FALSE; /*-- AE_FASTAE_OK_FLASH_OFF */
    }

    return ret;
}

void appAEYGet(void)
{
    frameBufInfo_t frameBufVideoPvSec;
    UINT32 y, u, v;
    sp5kGfxObj_t objYuv;

    #define ALIGN16(x)  (((x)+15)&(UINT32)(~0xf))
    #define ALIGN4(x)	(((x)+3)&(UINT32)(~0x3))

#if 1
    sp5kDispUrgentCallbackYuvGet(&frameBufVideoPvSec, 0, 0);
#else
    memcpy(&frameBufVideoPvSec,pframe,sizeof(frameBufInfo_t));
#endif

    //fsSimpleWrite( "D:\\AE.YUV", frameBufVideoPvSec.pbuf, frameBufVideoPvSec.width * frameBufVideoPvSec.height * 2 );

    memset(&objYuv, 0, sizeof(sp5kGfxObj_t));

    objYuv.pbuf = frameBufVideoPvSec.pbuf;
    objYuv.bufW = ALIGN16(frameBufVideoPvSec.width);
    objYuv.bufH = ALIGN16(frameBufVideoPvSec.height);
    objYuv.roiX = frameBufVideoPvSec.roiX;
    objYuv.roiY = frameBufVideoPvSec.roiY;
    objYuv.roiW = frameBufVideoPvSec.roiW;
    objYuv.roiH = frameBufVideoPvSec.roiH;
    objYuv.fmt = SP5K_GFX_FMT_YUV422;

    sp5kGfxObjectYuvAverageGet(&objYuv, &y, &u, &v);

    printf("Y =%u\n",y);
}
/*-------------------------------------------------------------------------
 *  Function Name : aeExif_DbgLogInit
 *------------------------------------------------------------------------*/
void aeExif_DbgLogInit(void)
{
	appExif_MN_BufClear(EXIF_MN_AE);
}

/*-------------------------------------------------------------------------
 *  Function Name : aeExif_DbgLogSave
 *------------------------------------------------------------------------*/
void aeExif_DbgLogSave(void)
{
    SINT32 expIdx,snapexpIdx;
    UINT32 agcIdx,snapagcIdx;
    AEStatus_t AEStatus;

    appAEPreviewExpAgcGet(&expIdx, &agcIdx);
    appAEStillCapExpAgcGet(&snapexpIdx, &snapagcIdx);
    appAEALG_ParamGet(AE_PARA_STATUS,(SINT32*)&AEStatus);

	appExif_MN_Printf(EXIF_MN_AE, (const char *)"LVx1000=%d ", AEStatus.LVx1000);
	appExif_MN_Printf(EXIF_MN_AE, (const char *)"Luma=%d ", AEStatus.Luma);
	appExif_MN_Printf(EXIF_MN_AE, (const char *)"Target=%d ", AEStatus.Target);
	appExif_MN_Printf(EXIF_MN_AE, (const char *)"Stable=%d ", AEStatus.Stable);
	appExif_MN_Printf(EXIF_MN_AE, (const char *)"Step=%d ", AEStatus.TargetStep);
	appExif_MN_Printf(EXIF_MN_AE, (const char *)"BLStatus=%d ", AEStatus.BLStatus);
	appExif_MN_Printf(EXIF_MN_AE, (const char *)"pvexp=%d  ",expIdx);
	appExif_MN_Printf(EXIF_MN_AE, (const char *)"pvagc=%d ", agcIdx);
	appExif_MN_Printf(EXIF_MN_AE, (const char *)"capExp=%d ",snapexpIdx);
	appExif_MN_Printf(EXIF_MN_AE, (const char *)"capAgc=%d ", snapagcIdx);
}

void appAeResourcePrint(void)
{
    SINT32 i;

       appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
	appAEALG_ParamGet(AE_PARA_PROGRAMCHART_PV,(SINT32*)AEProgramChart_PV);
	appAEALG_ParamGet(AE_PARA_PROGRAMCHART_CAP,(SINT32*)AEProgramChart_CAP);
	appAEALG_ParamGet(AE_PARA_WINDOW_LV,(SINT32*)AEWindowLV);
	appAEALG_ParamGet(AE_PARA_WINDOW,(SINT32*)AEWindowTemp);
	appAEALG_ParamGet(AE_PARA_TARGET,(SINT32*)AETarget);


    printf("\n\n========AE Resource Print========\n");

    printf("AE Config : \n");
    printf("Ver=%d,ISO100AgcIdx=%d,ISO100AgcIdxCAP=%d,LVErrx1000=%d,DefaultBV=%d\n",
		AEConfig.Ver,AEConfig.ISO100AgcIdx,AEConfig.ISO100AgcIdxCAP,AEConfig.LVErrx1000,AEConfig.DefaultBV);
    printf("ConvergeSpeed=%d,ConvergeLimit=%d,ConvergeSpeedFast=%d,ConvergeLimitFast=%d,ConvergeRange=%d\n",
		AEConfig.ConvergeSpeed,AEConfig.ConvergeLimit,AEConfig.ConvergeSpeedFast,AEConfig.ConvergeLimitFast,AEConfig.ConvergeRange);
    printf("StableRange=%d,StableCNT=%d,OBError=%d\n",
              AEConfig.StableRange,AEConfig.StableCNT,AEConfig.OBError);
    printf("BLCompEnable=%d,BLUpHighRatio=%d,BLDownLowRatio=%d,BLUpLVHighBound=%d,BLUpLVLowBound=%d\n",
              AEConfig.BLCompEnable,AEConfig.BLUpHighRatio,AEConfig.BLDownLowRatio,AEConfig.BLUpLVHighBound,AEConfig.BLUpLVLowBound);
    printf("BLEVx1000HighBound=%d,BLEVx1000LowBound=%d,BLDownAETarget=%d\n",
              AEConfig.BLEVx1000HighBound,AEConfig.BLEVx1000LowBound,AEConfig.BLDownAETarget);

    printf("\nPV AE Program Chart : \n");
    for(i=0;i<AE_PROGRAM_CHART_SIZE;i++)
    {
        if(AEProgramChart_PV[i].AV!=0&&AEProgramChart_PV[i].TV!=0&&AEProgramChart_PV[i].SV!=0)
             printf("%d,%d,%d\n",AEProgramChart_PV[i].AV,AEProgramChart_PV[i].TV,AEProgramChart_PV[i].SV);
	else
		break;
    }

    printf("\nCAP AE Program Chart : \n");
    for(i=0;i<AE_PROGRAM_CHART_SIZE;i++)
    {
        if(AEProgramChart_CAP[i].AV!=0&&AEProgramChart_CAP[i].TV!=0&&AEProgramChart_CAP[i].SV!=0)
             printf("%d,%d,%d\n",AEProgramChart_CAP[i].AV,AEProgramChart_CAP[i].TV,AEProgramChart_CAP[i].SV);
	else
		break;
    }


        printf("\nAE Window LV : \n");
        for(i=0;i<AE_WINDOW_V_SIZE;i++)
        {
            printf("%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,\n"
				,AEWindowLV[i*AE_WINDOW_H_SIZE+0],AEWindowLV[i*AE_WINDOW_H_SIZE+1],AEWindowLV[i*AE_WINDOW_H_SIZE+2],AEWindowLV[i*AE_WINDOW_H_SIZE+3]
				,AEWindowLV[i*AE_WINDOW_H_SIZE+4],AEWindowLV[i*AE_WINDOW_H_SIZE+5],AEWindowLV[i*AE_WINDOW_H_SIZE+6],AEWindowLV[i*AE_WINDOW_H_SIZE+7]
				,AEWindowLV[i*AE_WINDOW_H_SIZE+8],AEWindowLV[i*AE_WINDOW_H_SIZE+9],AEWindowLV[i*AE_WINDOW_H_SIZE+10],AEWindowLV[i*AE_WINDOW_H_SIZE+11]
				,AEWindowLV[i*AE_WINDOW_H_SIZE+12],AEWindowLV[i*AE_WINDOW_H_SIZE+13],AEWindowLV[i*AE_WINDOW_H_SIZE+14],AEWindowLV[i*AE_WINDOW_H_SIZE+15]);
        }

        printf("\nAE Window : \n");
        for(i=0;i<AE_WINDOW_V_SIZE;i++)
        {
            printf("%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,%3d,\n"
				,AEWindowTemp[i*AE_WINDOW_H_SIZE+0],AEWindowTemp[i*AE_WINDOW_H_SIZE+1],AEWindowTemp[i*AE_WINDOW_H_SIZE+2],AEWindowTemp[i*AE_WINDOW_H_SIZE+3]
				,AEWindowTemp[i*AE_WINDOW_H_SIZE+4],AEWindowTemp[i*AE_WINDOW_H_SIZE+5],AEWindowTemp[i*AE_WINDOW_H_SIZE+6],AEWindowTemp[i*AE_WINDOW_H_SIZE+7]
				,AEWindowTemp[i*AE_WINDOW_H_SIZE+8],AEWindowTemp[i*AE_WINDOW_H_SIZE+9],AEWindowTemp[i*AE_WINDOW_H_SIZE+10],AEWindowTemp[i*AE_WINDOW_H_SIZE+11]
				,AEWindowTemp[i*AE_WINDOW_H_SIZE+12],AEWindowTemp[i*AE_WINDOW_H_SIZE+13],AEWindowTemp[i*AE_WINDOW_H_SIZE+14],AEWindowTemp[i*AE_WINDOW_H_SIZE+15]);
        }

        printf("\nAE Target: \n");
        for(i=0;i<AE_TARGET_LV_RANGE;i++)
        {
            printf("%d\n",AETarget[i]);
        }

}

void
cmdAE(
	UINT32 argc,
	UINT8 *arg[],
	UINT32 v[]
)
{
#if 1
	UINT32 i;
	if(argc==1)
	{
		printf("ae usage:\n"
			" ae ver : get version\n"
			" ae senid [sen_id] : select dual ae sensor index\n"
			" ae senmode [sen_mode] : select dual ae sensor mode\n"
			" ae algres : current resource print\n"
			" ae target [value] : set current ae target value\n"
			" ae speed [speed] [slope] [limit] : set ae converge speed\n"
			" ae speedfast [speed] [slope] [limit] : set ae converge fast speed\n"
			" ae range [converge] [stable] : set ae converge & stable range\n"
			" ae lverrx1000 [value] : set lverrx1000 value\n"
			" ae oberror [value] : set oberror value\n"
			" ae stillequalpv [enable] : set still equal pv on/off\n"
			" ae blcomp [enable] : set ae backlight compensation on/off\n"
			" ae bltarget [value] : set ae backlight compensation target\n"
			" ae status [vd_count]: current status print by vd count\n"
			" ae info : ae window info print\n"
			" ae infodual : dual ae window info print\n"
			" ae pvpchart [idx] [item] [value] : set current preview pchart\n"
			" ae cappchart [idx] [item] [value] : set current capture pchart\n"
			" ae pv [exp] [agc] : set preview exp & agc\n"
			" ae cap [exp] [agc] : set capture exp & agc\n"
			" ae pvdual [sen_id] [exp] [agc] : set dual sensor preview exp & agc\n"
			" ae cal [mode]: cal 1:iso100 , 2:saturation , 3:table accurate\n"
			" ae period [value]: set ae cb period\n"
			" ae frf : get frame rate & flick mode\n"
			" ae framerate [fps]: set frame rate\n"
			" ae flick [idx]: set flick idx 0:50HZ , 1:60HZ\n"
			" ae meter [idx]: set meter idx\n"
			" ae bias [idx] : ev compensation\n"
			" ae capiso [idx] : set menu ISO\n"
			" ae binning : get current sensor mode binning status\n"
			" ae exptime : get current accurate exposure time\n"
			" ae targetfactor [idx] : set ae target by factor table\n"
			" ae pvhighiso [sv] : set preview ae high iso boundary\n"
			" ae caphighiso [sv] : set capture ae high iso boundary\n"
			" ae objectwindow [x] [y] [h] [v] [weight] : set object ae window weighting\n"
			" ae gety [enable]: get brightness y \n"
			" ae lock [enable] : lock ae \n"
		);
	}
	else if (strcmp((const char *)arg[1], "ver")==0)
	{
		SINT32 aeLibVerStr ;
		appAEALG_ParamGet(AE_PARA_LIB_VERSION, &aeLibVerStr);

		printf("AE LIB VER: %s\n",(char*)aeLibVerStr);
	}
	else if (strcmp((const char *)arg[1], "senid")==0)
	{
		SINT32 sen_id ;
	       appAEALG_ParamGet(AE_PARA_SENSOR_ID,&sen_id);
		if (argc==2)
		{
			printf("sen_id=%d\n",sen_id);
		}
		else
		{
			sen_id = v[2];
			appAEALG_ParamSet(AE_PARA_SENSOR_ID,sen_id);
			printf("Set sen_id=%d\n",sen_id);
		}
	}
	else if (strcmp((const char *)arg[1], "senmode")==0)
	{
		SINT32 sen_mode ;
	       appAEALG_ParamGet(AE_PARA_SENSORSEL,&sen_mode);
		if (argc==2)
		{
			printf("sen_mode=%d\n",sen_mode);
		}
		else
		{
			sen_mode = v[2];
			appAEALG_ParamSet(AE_PARA_SENSORSEL,sen_mode);
			printf("Set sen_mode=%d\n",sen_mode);
		}
	}
	else if (strcmp((const char *)arg[1], "algres")==0)
	{
		appAeResourcePrint();
	}
	else if (strcmp((const char *)arg[1], "target")==0)
	{
		for(i=0;i<AE_TARGET_LV_RANGE;i++)
			AETarget[i] =  v[2];

		appAEALG_ParamSet(AE_PARA_TARGET, (SINT32)AETarget);
		printf("AE target = %u\n",v[2]);
	}
	else if (strcmp((const char *)arg[1], "speed")==0)
	{
	       appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
		if (argc==2)
		{
			printf("ConvergeSpeed=%d , ConvergeSlope=%d , ConvergeLimit=%d\n",AEConfig.ConvergeSpeed,AEConfig.ConvergeSlope,AEConfig.ConvergeLimit);
		}
		else
		{
			AEConfig.ConvergeSpeed = v[2];
			AEConfig.ConvergeSlope = v[3];
			AEConfig.ConvergeLimit = v[4];
			appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfig);
			printf("Set ConvergeSpeed=%d , ConvergeSlope=%d , ConvergeLimit=%d\n",AEConfig.ConvergeSpeed,AEConfig.ConvergeSlope,AEConfig.ConvergeLimit);
		}
	}
	else if (strcmp((const char *)arg[1], "speedfast")==0)
	{
	       appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
		if (argc==2)
		{
			printf("ConvergeSpeedFast=%d , ConvergeSlopeFast=%d , ConvergeLimitFast=%d\n",AEConfig.ConvergeSpeedFast,AEConfig.ConvergeSlopeFast,AEConfig.ConvergeLimitFast);
		}
		else
		{
			AEConfig.ConvergeSpeedFast = v[2];
			AEConfig.ConvergeSlopeFast = v[3];
			AEConfig.ConvergeLimitFast = v[4];
			appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfig);
			printf("Set ConvergeSpeedFast=%d , ConvergeSlopeFast=%d , ConvergeLimitFast=%d\n",AEConfig.ConvergeSpeedFast,AEConfig.ConvergeSlopeFast,AEConfig.ConvergeLimitFast);
		}
	}
	else if (strcmp((const char *)arg[1], "range")==0)
	{
	       appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
		if (argc==2)
		{
			printf("ConvergeRange=%d , StableRange=%d\n",AEConfig.ConvergeRange,AEConfig.StableRange);
		}
		else
		{
			AEConfig.ConvergeRange = v[2];
			AEConfig.StableRange= v[3];
			appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfig);
			printf("Set ConvergeRange=%d , StableRange=%d\n",AEConfig.ConvergeRange,AEConfig.StableRange);
		}
	}
	else if (strcmp((const char *)arg[1], "lverrx1000")==0)
	{
		appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
		if (argc==2)
		{
			printf("LVErrx1000=%d\n",AEConfig.LVErrx1000);
		}
		else
		{
			AEConfig.LVErrx1000 = v[2];
			appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfig);
			printf("Set LVErrx1000=%d\n",AEConfig.LVErrx1000);
		}
	}
	else if (strcmp((const char *)arg[1], "oberror")==0)
	{
		appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
		if (argc==2)
		{
			printf("OBError=%d\n",AEConfig.OBError);
		}
		else
		{
			AEConfig.OBError = v[2];
			appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfig);
			printf("OBError=%d\n",AEConfig.OBError);
		}
	}
	else if (strcmp((const char *)arg[1], "stillequalpv")==0)
	{
		appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
		if (argc==2)
		{
			printf("StillEqualPV=%d\n",AEConfig.StillEqualPV);
		}
		else
		{
			AEConfig.StillEqualPV = v[2];
			appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfig);
			printf("StillEqualPV=%d\n",AEConfig.StillEqualPV);
		}
	}
	else if (strcmp((const char *)arg[1], "blcomp")==0)
	{
		appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
		if (argc==2)
		{
			printf("BLCompEnable=%d\n",AEConfig.BLCompEnable);
		}
		else
		{
			AEConfig.BLCompEnable = v[2];
			appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfig);
			printf("Set BLCompEnable=%d\n",AEConfig.BLCompEnable);
		}
	}
	else if (strcmp((const char *)arg[1], "bltarget")==0)
	{
		appAEALG_ParamGet(AE_PARA_CONFIG,(SINT32*)&AEConfig);
		if (argc==2)
		{
			printf("BLDownAETarget=%d\n",AEConfig.BLDownAETarget);
		}
		else
		{
			AEConfig.BLDownAETarget = v[2];
			appAEALG_ParamSet(AE_PARA_CONFIG,(SINT32)&AEConfig);
			printf("Set BLDownAETarget=%d\n",AEConfig.BLDownAETarget);
		}
	}
	else if (strcmp((const char *)arg[1], "status")==0)
	{
		if (argc==2)
		{
			AEStatus_t AEStatus;

			appAEALG_ParamGet(AE_PARA_STATUS,(SINT32*)&AEStatus);

			printf("LVx1000=%d,Luma=%d,Target=%d,Stable=%d,TargetStep=%d\n",AEStatus.LVx1000,AEStatus.Luma,AEStatus.Target,AEStatus.Stable,AEStatus.TargetStep);
			printf("Preview aperidx=%d,expidx=%d,expidxset=%d,agcidx=%d,accuridx=%d\n",AEStatus.aperidx,AEStatus.expidx,AEStatus.expidxset,AEStatus.agcidx,AEStatus.accuridx);
			printf("Capture aperidx=%d,expidx=%d,agcidx=%d,accuridx=%d\n",AEStatus.aperidxCAP,AEStatus.expidxCAP,AEStatus.agcidxCAP,AEStatus.accuridxCAP);
			printf("BLStatus=%d,BLEVx1000Diff=%d,DeltaEVx1000=%d,UpLVx1000=%d,UpY=%d,DownY=%d\n",AEStatus.BLStatus,AEStatus.BLEVx1000Diff,AEStatus.DeltaEVx1000,AEStatus.UpLVx1000,AEStatus.UpY,AEStatus.DownY);
		}
		else
		{
			aestatusprintvdcnt=v[2];
		}
	}
	else if (strcmp((const char *)arg[1], "lock")==0)
	{
		appAEALG_ParamSet(AE_PARA_LOCK, v[2]);

		printf("Lock AE : %u\n",v[2]);
	}
	else if (strcmp((const char *)arg[1], "pv")==0)
	{
		appAEPreviewExpAgcSet(v[2], v[3],0);
		printf("exp/agc: %u, %u\n", v[2], v[3]);
	}
	else if (strcmp((const char *)arg[1], "cap")==0)
	{
		appAEStillCapExpAgcSet(v[2], v[3],0);
		printf("exp/agc: %u, %u\n", v[2], v[3]);
	}
	else if (strcmp((const char *)arg[1], "pvdual")==0)
	{
		appAEPreviewExpAgcNSet(v[2], v[3],v[4],0);
		printf("Sen%u  exp/agc: %u, %u\n",v[2], v[3], v[4]);
	}
	else if (strcmp((const char *)arg[1], "pvpchart")==0)
	{
		appAEALG_ParamGet(AE_PARA_PROGRAMCHART_PV,(SINT32*)AEProgramChart_PV);

		if (argc==2) {
		    printf("\nPV AE Program Chart : \n");
		    for(i=0;i<AE_PROGRAM_CHART_SIZE;i++)
		    {
		        if(AEProgramChart_PV[i].AV!=0&&AEProgramChart_PV[i].TV!=0&&AEProgramChart_PV[i].SV!=0)
		             printf("%d,%d,%d\n",AEProgramChart_PV[i].AV,AEProgramChart_PV[i].TV,AEProgramChart_PV[i].SV);
			else
				break;
		    }
		}
              else
              {
              	if(v[3]==0)
              		AEProgramChart_PV[v[2]].AV=v[4];
			else if(v[3]==1)
				AEProgramChart_PV[v[2]].TV=v[4];
			else if(v[3]==2)
				AEProgramChart_PV[v[2]].SV=v[4];

			appAEALG_ParamSet(AE_PARA_PROGRAMCHART_PV, (SINT32)AEProgramChart_PV);
              }

	}
	else if (strcmp((const char *)arg[1], "cappchart")==0)
	{
		appAEALG_ParamGet(AE_PARA_PROGRAMCHART_CAP,(SINT32*)AEProgramChart_CAP);

		if (argc==2) {
		    printf("\nCAP AE Program Chart : \n");
		    for(i=0;i<AE_PROGRAM_CHART_SIZE;i++)
		    {
		        if(AEProgramChart_CAP[i].AV!=0&&AEProgramChart_CAP[i].TV!=0&&AEProgramChart_CAP[i].SV!=0)
		             printf("%d,%d,%d\n",AEProgramChart_CAP[i].AV,AEProgramChart_CAP[i].TV,AEProgramChart_CAP[i].SV);
			else
				break;
		    }
		}
              else
              {
              	if(v[3]==0)
              		AEProgramChart_CAP[v[2]].AV=v[4];
			else if(v[3]==1)
				AEProgramChart_CAP[v[2]].TV=v[4];
			else if(v[3]==2)
				AEProgramChart_CAP[v[2]].SV=v[4];

			appAEALG_ParamSet(AE_PARA_PROGRAMCHART_CAP, (SINT32)AEProgramChart_CAP);
              }
	}
	else if (strcmp((const char *)arg[1], "cal")==0)
	{
	       appAEALG_ParamSet(AE_PARA_CAL,v[2]);
	}
	else if (strcmp((const char *)arg[1], "period")==0)
	{
		if (argc==2)
		{
			SINT32 period;
			appAEALG_ParamGet(AE_PARA_PERIOD,&period);
			printf("ae period = %d\n",period);
		}
		else
		{
			appAEALG_ParamSet(AE_PARA_PERIOD,(SINT32)v[2]);
			printf("Set ae period = %u\n",v[2]);
		}
	}
	else if (strcmp((const char *)arg[1], "frf")==0)
	{
		printf("aeFrameRateSel = %u , aeFlickerSel=%u(%dHZ)\n",aeFrameRateSel,aeFlickerSel,aeFlickerSel==0?50:60);
	}
	else if (strcmp((const char *)arg[1], "info")==0)
	{
		aeinfoprint = 1;
	}
	else if (strcmp((const char *)arg[1], "infodual")==0)
	{
		aeinfoprintdual = 1;
	}
	else if (strcmp((const char *)arg[1], "iris")==0)
	{
		appAeIrisSet(v[2]);
		printf("iris: %u avidx: %d\n", v[2],aeIris[v[2]]);
	}
	else if (strcmp((const char *)arg[1], "bias")==0)
	{
		appAeEvBiasSet(v[2]);
		printf("biasidx = %u biascmp = %d evset = %d \n", v[2],aebiascmp,aeevset);
	}
	else if (strcmp((const char *)arg[1], "pasminfo")==0)
	{
		printf("pasm = %d iris = %d shutter = %d iso = %d\n",aeCb.aeset.pasm,aeCb.aeset.iris,aeCb.aeset.shutter,aeCb.aeset.iso);
	}
	else if (strcmp((const char *)arg[1], "framerate")==0)
	{
		printf("framerate = %u\n",v[2]);
		appAeFrameRateSet(v[2]);
	}
	else if (strcmp((const char *)arg[1], "flick")==0)
	{
		printf("flick mode = %u\n",v[2]);
		appAeExpFlickerModeSet(v[2]);
	}
	else if (strcmp((const char *)arg[1], "meter")==0)
	{
		printf("meter mode = %u\n",v[2]);
		appAeMeterSet(v[2]);
	}
	else if (strcmp((const char *)arg[1], "capiso")==0)
	{
		printf("cap iso idx = %u\n",v[2]);
		appAeCapIsoSet(v[2]);
	}
	else if (strcmp((const char *)arg[1], "binning")==0)
	{
	       UINT32 biningstatus;
		sp5kSensorInfoGet(sensorModeGet(),SENSOR_CUST_INFO_01,&biningstatus);
		printf("current sensor mode binningstatus = %u\n",biningstatus);
	}
	else if (strcmp((const char *)arg[1], "exptime")==0)
	{
	       SINT32 exp;
	       UINT32 agc,exptime;
		appAEPreviewExpAgcGet(&exp,&agc);
		exptime = appAEPreviewExpIdxtoAccurateUs(exp);
		printf("current exposure index = %d , accurate exposure time = %u us\n",exp,exptime);
	}
	else if (strcmp((const char *)arg[1], "targetfactor")==0)
	{
		appAeTargetFactorSet(v[2]);
		printf("set ae target factor index= %u\n",v[2]);
	}
	else if (strcmp((const char *)arg[1], "pvhighiso")==0)
	{
		appAePreviewHighISOBoundarySet(v[2]);
		printf("set preview ae high iso boundary= %u\n",v[2]);
	}
	else if (strcmp((const char *)arg[1], "caphighiso")==0)
	{
		appAeCaptureHighISOBoundarySet(v[2]);
		printf("set capture ae high iso boundary= %u\n",v[2]);
	}
	else if (strcmp((const char *)arg[1], "objectwindow")==0)
	{
		appAEObjectWindow(v[2],v[3],v[4],v[5],v[6]);
	}
	else if (strcmp((const char *)arg[1], "gety")==0)
	{
		if(v[2]==0)
		{
		}
		else
		{
			appAEYGet();
		}
	}

#if 0
	else if (strcmp((const char *)arg[1], "cappara")==0)
	{
		printf("fast cappara\n");
		SINT16 iris,shutter,iso;
		appAeCapParaGet(&iris,&shutter,&iso);
	}
	else if (strcmp((const char *)arg[1], "win")==0)
	{
		printf("ae win %d\n", v[2]);
		sp5kAeCfgSet(SP5K_AE_WIN_POS_TEST, !!v[2]);
	}
	else if (strcmp((const char *)arg[1], "report")==0)
	{
		printf("ae report \n");
		_aeInitReport();
	}
	else if (strcmp((const char *)arg[1], "osd")==0)
	{
		appAeInfo();
	}
	else if (strcmp((const char *)arg[1], "hdrev")==0)
	{
		if (argc<3) {
			printf("plz enter: hdr ev");
			return;
		}
		if(v[2]>20 || v[4]<0){
			printf("plz enter EV range in 0~2.0 (ex:EV1.5 ->15, EV2.0 ->20)\n");
			return;
		}
		aeHDRev = v[2];
	}
	else if (strcmp((const char *)arg[1], "hdr")==0)
	{
		UINT32 exp_idx, agc_idx;
		UINT16 ev_entry1=0, ev_entry2=0;
		UINT16 entry_diff1=0, entry_diff2=0;
        extern void sensor_HDR_ev_diff_set(UINT16 diff1, UINT32 diff2);

		if (argc<5) {
			printf("plz enter: exp agc ev");
			return;
		}
		if(v[4]>20 || v[4]<0){
			printf("plz enter EV range in 0~2.0 (ex:EV1.5 ->15, EV2.0 ->20)\n");
			return;
		}
		if(v[2]<HDR_MinExpIdx || v[2]>HDR_MaxExpIdx)
		{
			printf("exp range error  AE_Expidx:%d~%d\n",HDR_MinExpIdx,HDR_MaxExpIdx);
			return;
		}
		if(v[3]<HDR_MinAgcIdx || v[3]>HDR_MaxAgcIdx)
		{
			printf("agc range error  AE_Agcidx:%d~%d\n",HDR_MinAgcIdx,HDR_MaxAgcIdx);
			return;
		}
		exp_idx = v[2];
		agc_idx = v[3];
		sensor_HDR_ev_set(v[4]);

		ev_entry1 = (v[4]*16+5)/10;   /*rounding (v[4]*16)/10*/
		if(exp_idx+ev_entry1 > HDR_MaxExpIdx)
		{
			entry_diff1 = (exp_idx+ev_entry1)-HDR_MaxExpIdx;
		}

		ev_entry2 = (2*v[4]*16+5)/10; /*rounding ((2*v[4]*16)/10*/
		if(exp_idx+ev_entry2 > HDR_MaxExpIdx)
		{
			entry_diff2 = (exp_idx+ev_entry2)-HDR_MaxExpIdx;
		}
		if(entry_diff1 > 0 || entry_diff2>0)
			sensor_HDR_ev_diff_set(entry_diff1,entry_diff2);
		/*appAEStillCapExpAgcSet((SINT32)exp_idx, agc_idx,0);*/
		appAEPreviewExpAgcSet((SINT32)exp_idx, agc_idx,0);
		sensor_HDR_EXP_do();
		sensor_HDR_ISO_do();
	}
#endif
	else
	{
		cmdAE(1,0,0);
	}
#endif
}


void appShowCurExpAgc(void)
{
	SINT32 expIdx;
	UINT32 agcIdx;

	appAEPreviewExpAgcGet(&expIdx, &agcIdx);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "cap: sp5k view (exp=%d, agc=%d)", expIdx, agcIdx);

	appAEStillCapExpAgcGet(&expIdx, &agcIdx);
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "cap: sp5k cap (exp=%d, agc=%d)", expIdx, agcIdx);
}

