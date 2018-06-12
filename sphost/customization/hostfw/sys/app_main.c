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
 *  File name	: 	App_main.c
 *  Function 	: 	Main for application		 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"

#include "gpio_custom.h"
#include "sp5k_global_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_capture_api.h"
#include "sp5k_disp_api.h"
#include "sp5k_disk_api.h"
#include "sp5k_usb_api.h"
#include "sp5k_rsvblk_api.h"

#include "app_dev_plug.h"
#include "app_tvlcd.h"
#include "app_script.h"
#include "app_calib_api.h"
#include "app_playback_def.h"
#include "app_rtc.h"
#include "app_battery.h"
#include "app_lyt_show.h"
#include "app_view_osd.h"
#include "app_still.h"
#include "app_video.h"
#include "customization/app_init.h"
#include "../service/misc/app_battery_ctrl.h"
#include "app_user_setting.h"
#include "app_calib_api.h"
#include "app_errorosd.h"

#if defined(PRJ_JETDVR) ||defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_uart2.h"
#include "app_infohandlerComm.h"
#include "sp5k_dbg_api.h"
#include "app_buzzer.h"
extern SP5K_EVENT_FLAGS_GROUP InfoHandler_sig;
extern SP5K_QUEUE infohandlerQueue;
extern PanaState appInfoPanaStateGet(void);
extern void appVideoStop(void);
/**************************************************************************
 *         app_InfoHandler  F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
extern void appInfoHandler(UINT8 *buf, UINT32 len);
extern BOOL jet_filelist_sending_status_get(void);
#endif

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define READ8(a) (*((volatile UINT8*)(a)))
#define WRITE8(a,b)   *(volatile UINT8*)(a) = (b)
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static BOOL isFirstDiskMount = FALSE;
static BOOL isGoing2PowerOff = FALSE;
BOOL bBatteryIn = FALSE;
BOOL bPccamConn = FALSE;
//extern void appExceptionHandle();


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
extern void appHostFastBootInit(void);
extern void appTimerGetPOR(void);

void appStateCtrlInitial(void);
void appStateOnProc(UINT32 msg,UINT32 param);
void appUiModeChange(void);
void appPsuedoPowerOff(void);
void appDiskMountMsgHandler(UINT32 msg, UINT32 param);
void appDevHotPlugHandler(UINT32 msg, UINT32 param);
void appPowerSaveTimeOutReset(void);
void appUsbMsdcModeSet(void);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern void appInfoRegularFB(PanaState state);
#endif
/**************************************************************************
 *  Function Name: appKeyMsgDebugPrint                                        *
 **************************************************************************/
/*void
appKeyMsgDebugPrint(
    UINT32 keyMsg
)
{

}*/

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOffUiMsgSend
 * Function	       	: UI message send for power off
 * Return value   	: void
 * Parameter1    	: param (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

/**************************************************************************
 *  Function Name: appPowerOffUiMsgSend                                        *
 **************************************************************************/
void
appPowerOffUiMsgSend(
    UINT32 param
)
{
    HOST_PROF_LOG_ADD(LEVEL_INFO, "main: appPowerOffUiMsgSend");
    sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, param);
    isGoing2PowerOff = TRUE;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appPowerOffWakeupSet
 * Function	       	: set power wakeup
 * Return value   	: void
 * Parameter1    	: param (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appPowerOffWakeupSet(
    UINT32 param
)
{
    appRTC_t tt;
    UINT32 wakeuptime =0;

    appCalibDataSave();

    wakeuptime = param; /*Unit:Sec*/
    appRtcGet(&tt);

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "main: WakeupGet : %d : %d : %d : %d : %d : %d"\
                        ,tt.year, tt.mon,tt.day,tt.hour,tt.min,tt.sec);
    tt.sec=tt.sec+wakeuptime;
#if !defined(PRJ_17DVR)&&!defined(PRJ_19DVR)
if (tt.sec>59)
    {
        tt.min =tt.min+1;
        tt.sec = tt.sec-60;
    }

    if (tt.min>59)
    {
        tt.hour =tt.hour+1;
        tt.min =tt.min-60;
    }
#endif
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "main: WakeupSet : %d : %d : %d : %d : %d : %d"\
                        ,tt.year, tt.mon,tt.day,tt.hour,tt.min,tt.sec);
    appRtcWakeUpSet(&tt);
    sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, APP_POWER_OFF_NORMAL);
    isGoing2PowerOff = TRUE;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appStateCommonProc
 * Function	       	: state common process
 * Return value   	: appState_msg_flow_e (APPSTATE_MSG_DEFAULT/APPSTATE_MSG_IGNORE)
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

appState_msg_flow_e
appStateCommonProc(
    UINT32 msg,
    UINT32 param
)
{
    UINT32 ret = APPSTATE_MSG_IGNORE;
    static UINT32 countNum=0;
    static UINT32 detectResult=0;
   // UINT32 bandingset=0;

    switch( msg )
    {

    case SP5K_MSG_HOST_TASK_INIT:       /* the 1'st msg after boot*/
        appHostFastBootInit();
        appStateCtrlInitial();
        break;

    case SP5K_MSG_POWER_OFF:
    case APP_KEY_PRESS_POWER:
        HOST_PROF_LOG_ADD(LEVEL_INFO, "main: SP5K_MSG_POWER_OFF");
        /* Here user do pre-power down preparation
           The times = 0 for no need to polling */

        if(param == APP_POWER_OFF_BATT_EMPTY)
        {
            printf("[INFO] Battery Empty!!! system is going to power off now!\n");
        }
        break;

    case SP5K_MSG_POWER_OFF_KEY_RELEASE:
        HOST_PROF_LOG_ADD(LEVEL_INFO, "main: SP5K_MSG_POWER_OFF_KEY_RELEASE");
        //do nothing
        break;

    case SP5K_MSG_AUTO_BANDING_RESULT:
        if(detectResult == param)
        {
            countNum++;
            if(countNum > 5)
            {
				UINT32 bandingset;
                countNum=0;
                sp5kModeCfgSet(SP5K_MODE_CFG_BAND_DETECT_ENABLE, 0);
                if (detectResult ==50){ /*50 Hz*/
                    bandingset = 0;
                }else{
                    bandingset = 1;
                }
                appStill_SetExpFlickerMode(bandingset);
                appAePvReset();
            }
        }
        else
        {
            countNum=0;
            detectResult = param;

        }
        break;

    case SP5K_MSG_DISK_INSERT:
        profLogPrintf(0, "[host]main: disk in(%d)",param);
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "main: SP5K_MSG_DISK_INSERT(%d)",param);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	appInfoRegularFB(appInfoPanaStateGet());
#endif
        if (param == SP5K_DRIVE_NAND)
        {
			#if (SP5K_DISK_NAND || SP5K_DISK_EMMC || SP5K_DISK_ESD)
            if(!IS_CARD_EXIST) {
	            isFirstDiskMount = TRUE;
	            appDiskMount(param);
            }
			#endif
        }
        else
        {
            if (isFirstDiskMount)
            { // send card in msg after power on
                printf("send msg :APP_KEY_CARD_IN(%u) ",param);
                sp5kHostMsgSend(APP_KEY_CARD_IN, param);
            }
            else
            {


                if(appActiveStateGet() != APP_STATE_POWER_ON)
                {
                    HOST_PROF_LOG_PRINT(LEVEL_INFO, "main: send msg APP_KEY_CARD_IN(%d)",param);
                    isFirstDiskMount = TRUE;
                    sp5kHostMsgSend(APP_KEY_CARD_IN, param);
                }
                else
                {
                    HOST_PROF_LOG_ADD(LEVEL_INFO, "main: SD card mount.");
                    isFirstDiskMount = TRUE;
                    appDiskMount(param);
                }
            }
        }
        ret = APPSTATE_MSG_DEFAULT;
        break;

    case SP5K_MSG_DISK_REMOVAL:
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "main: SP5K_MSG_DISK_REMOVAL(%d)",param);

        /*rec and plug card error handle*/
        if(appActiveStateGet() == APP_STATE_VIDEO_REC)
        {
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "main: recording file but card plug(%d)",param);
            appVideoStop();
            sp5kHostMsgSend(APP_STATE_MSG_ABORT, param);
        }

        appDiskUnMount(param);

        if(param==SP5K_DRIVE_SD)
        {
            isFirstDiskMount = FALSE;
            sp5kHostMsgSend(APP_KEY_CARD_OUT, param);
        }
        break;

    case SP5K_MSG_DISK_MOUNT_COMPLETE:
        profLogPrintf(0, "[host]main: mount rdy %d",param);
    case SP5K_MSG_DISK_MOUNT_START:
    case SP5K_MSG_DISK_MOUNT_FAIL:
    case SP5K_MSG_DISK_ERROR:
        if (IS_DISK_USB_HOST_MOUNT && param==SP5K_DRIVE_USB)
        {
            sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, 0, 0, 0, 0 );
            #if !SP5K_PVQV_DISABLE
            appViewOsd_PvQV_DiskDraw();
            #endif
            sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, 1, 0, 0, 0 );
            HOST_PROF_LOG_PRINT(LEVEL_INFO, "main: USB Host mount %x",msg);
        }
        else
           appDiskMountMsgHandler(msg,param);

        if(!IS_CARD_EXIST && appActiveStateGet() != APP_STATE_MENU)
        {
            appErrMsgWithSound(ERR_MSG_NO_MEMORY_CARD, ERR_MSG_ONLY, ERR_MSG_SHOW_FOREVER);
            /*appBtnDisable(BTN_ALL);*/
        }

        ret = APPSTATE_MSG_DEFAULT;
        break;

    case SP5K_MSG_USB_POWER_CONFIGURED:
        if(param)
        {
            bBatteryIn = appBatteryAdcValueGet() > 100 ? TRUE:FALSE;

            if(appBatteryCharge_SetCheckStatusCBF() != BATTERY_STATUS_FULL && bBatteryIn){
                appBatteryCharge_SetStartCBF();
                RED_LED_ON;
            }
        }
        break;

    //----UI message ----------------------------------------
    case APP_UI_MSG_LENS_ERR:
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "main: APP_UI_MSG_LENS_ERR(%d)",param);
        appPowerOffUiMsgSend(APP_POWER_OFF_LENS_ERROR);
        break;

    case APP_UI_MSG_STROBE_CHARGE_ERR:
        HOST_PROF_LOG_ADD(LEVEL_ERROR, "main: APP_UI_MSG_STROBE_CHARGE_ERR");
        appPowerOffUiMsgSend(APP_POWER_OFF_CHARGE_ERROR);
        break;

    case APP_UI_MSG_POWER_OFF:
        HOST_PROF_LOG_PRINT(LEVEL_INFO, "main: APP_UI_MSG_POWER_OFF(%d)",param);
        if(param == APP_POWER_OFF_APO)
        {
            if(IS_USB_GROUP(appActiveStateGet())
                || IS_CALIB_GROUP(appActiveStateGet()))
            {
                HOST_PROF_LOG_ADD(LEVEL_WARN, "main: Ignore auto power off message!!");
                return ret;
            }
        }
        else if(param == APP_POWER_OFF_BATT_EMPTY)
        {
            HOST_PROF_LOG_ADD(LEVEL_WARN, "main: Battery Empty - system is going to power off now");
        }
        appStateChange(APP_STATE_POWER_OFF,param);
        break;
    case APP_KEY_TV_IN:
    case APP_KEY_TV_OUT:
        appDevHotPlugHandler(msg,param);
        break;
    case SP5K_MSG_USB_DEVICE_VBUS_IN:
    case SP5K_MSG_USB_DEVICE_VBUS_OFF:
        if(IS_CALIB_GROUP(appActiveStateGet()) || appCalibAPITestSettingGet())
        {
            #if USB_SUPPORT
            if (msg == APP_KEY_USB_IN)
            {
                if(appCalibAPITestSettingGet())
                {
                    sp5kUsbSwCon( 0 );
                    sp5kUsbModeSet( SP5K_USB_MODE_APITEST );
                }
                else
                {
                    appUsbMsdcModeSet();
                }
                sp5kUsbCfgSet(SP5K_USB_DEV_MAX_POWER_SET,SP5K_USB_MAXPOWER_HIGH);
                sp5kUsbSwCon(1);
            }
            else
            {
                sp5kUsbSwCon(0);
            }
            #endif
            return APPSTATE_MSG_DEFAULT;
        }
    case APP_KEY_CARD_IN:
    case APP_KEY_CARD_OUT:
        if(IS_CALIB_GROUP(appActiveStateGet()))
        {
            return APPSTATE_MSG_DEFAULT;
        }
#if 0
        if(!IS_CARD_EXIST)
        {
            appDiskMount(SP5K_DRIVE_NAND);
        }
#endif
		if(!IS_CARD_EXIST)
		{
			SET_SD_CARD_POWER(0);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)/*Isaac add at 20180517, if we get the 19DVR PCB with ETV this define(PRJ_19DVR) need remove*/
    			SET_SD_CARD_POWER_N(0);
#endif
			sp5kGpioWrite(SP5K_SD_CLKCMDDAT_GRP, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID, 0);
			sp5kGpioCfgSet(SP5K_SD_CLKCMDDAT_GRP, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID, SP5K_SD_CLKCMD_ID|SP5K_SD_DAT_ID);
			WRITE8(0xb00000ec,READ8(0xb00000ec)&0xFB);
		}
		appDevHotPlugHandler(msg,param);
		break;

    case SP5K_MSG_HDMI_PLUG:
    case SP5K_MSG_HDMI_UNPLUG:
        appDevHotPlugHandler(msg,param);
        if(IS_CALIB_GROUP(appActiveStateGet()))
        {
            return APPSTATE_MSG_DEFAULT;
        }
        break;

    case APP_UI_MSG_RTC_WAKEUP:
        appPowerOffWakeupSet(param);
        break;
#if API_TEST_INIT
    case APP_UI_MSG_EMULATION:
        appEmulationStateChange();
        break;
#endif
    case SP5K_MSG_USB_UI_TOOL_PARSE:
        break;
    default:
        ret = APPSTATE_MSG_DEFAULT;
        break;
    }

    if ( (ret == APPSTATE_MSG_IGNORE) &&
        (msg != SP5K_MSG_USB_PTP_OP_START) &&
        (msg != SP5K_MSG_USB_PTP_OP_END) )
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "main: Msg[0x%x] is ignored after %s",msg,__FUNCTION__);

    return ret;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appStateGroupBaseProc
 * Function	       	: state process for group base
 * Return value   	: appState_msg_flow_e (APPSTATE_MSG_DEFAULT/APPSTATE_MSG_IGNORE)
 * Parameter1    	: msg (UINT32)
 * Parameter2    	: param (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

appState_msg_flow_e
appStateGroupBaseProc(
    UINT32 msg,
    UINT32 param
)
{
    UINT32 ret = APPSTATE_MSG_DEFAULT;
    switch( STATE_GROUP(appActiveStateGet()) )
    {
        case APP_STATE_GROUP_SYS:
            break;
        case APP_STATE_GROUP_VIEW:
            ret = appGroupViewProc(msg,param);
            break;
        case APP_STATE_GROUP_VIDEO:
            ret = appGroupVideoProc(msg,param);
            break;
        case APP_STATE_GROUP_AUDIO:
            ret = appGroupAudioProc(msg,param);
            break;
        case APP_STATE_GROUP_PLAYBACK:
            ret = appGroupPlaybackProc(msg,param);
            break;
        case APP_STATE_GROUP_USB:
            ret = appGroupUsbProc(msg,param);
            break;
        case APP_STATE_GROUP_CALIB:
            ret = appGroupCalibrationProc(msg,param);
            break;
        default:
            break;
    }

    if (ret == APPSTATE_MSG_IGNORE)
    {
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "main: Msg[0x%x] is ignored after %s",msg,__FUNCTION__);
    }

    return ret;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appMain
 * Function	       	: Main application
 * Return value   	: void
 * Parameter1    	: sp5kmsg (UINT32)
 * Parameter2    	: sp5kparam (UINT32)
 * History	   :
 *                :  2018.06.01  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

void
appMain(
    UINT32 sp5kmsg,
    UINT32 sp5kparam
)
{
    UINT32 msg = sp5kmsg;
    UINT32 param = sp5kparam;
    //UINT32 key;
    #if HOST_DBG
    UINT32 inTime=0, outTime=0;
    #endif


    #if HOST_DBG
    if ((msg&0xffffff00) != (SP5K_MSG_AE_READY&0xffffff00))
    {
        profLogPrintf(0, "[host]main: ..[0x%08x, 0x%08x]",sp5kmsg, sp5kparam);
        HLPrintf2("main: Msg(0x%08x),Param( 0x%08x)",sp5kmsg, sp5kparam);
        inTime = appMsTimeGet();
    }
    #endif

    #if TEST_STATE_CHANG_MSG
    if (IS_STATE_CHANG_MSG(sp5kmsg)) /*test for state change msg*/
    {
        /*HOST_PROF_LOG_ADD(LEVEL_INFO, "main: APP_STATE_MSG_CHANG");*/
        appMsgStateChange(STATE_CHANG_MSG_FINISH, sp5kmsg, sp5kparam);
        return;
    }
    #endif


    /* decode sp5k gpio/adc button msg to app key msg*/
    if (IS_SP5K_BTN_MSG(sp5kmsg))
    {
		UINT32 key;
        key = appBtnMsgToKey(sp5kmsg,sp5kparam);
        if (key == APP_KEY_ERROR || key == APP_KEY_SKIP)
        {
            #if HOST_DBG
            if ((msg&0xffffff00) != (SP5K_MSG_AE_READY&0xffffff00))
            {
                DBG_PRINT("main: %d : msg=%08x param=%08x done\n\n", appMsTimeGet(),key, 0);
            }
            #endif

            return;
        }
        msg = key; param = 0;
    }

    /* power saving time out reset */
    if(IS_APP_KEY_MSG(msg))
    {
    /* Paul 2018.04.16  "Remove unused variable "(need check)
#if defined(PRJ_17DVR)
	pbMainCB_t *ppbmainCb = appPb_MainCbPtrGet();
#endif */
        if (isGoing2PowerOff)
        {
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "main: Ignore KEY MSG because system power off (0x%x)",msg);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	    if(appActiveStateGet() == APP_STATE_VIDEO_REC)
			appVideoStop();
#endif
        }
		#if 0
        pbMainCB_t *ppbmainCb = appPb_MainCbPtrGet();
        if (ppbmainCb->isIniting==TRUE)
        {
            HOST_PROF_LOG_PRINT(LEVEL_WARN, "main: Ignore KEY MSG because system playback mode (0x%x)",msg);
            return;
        }
		#endif
        if(IS_APO_COUNT_DOWN)
        {
            if(IS_LCD_OUT && !IS_BACKLIGHT_ON)
            {
                LCD_BACKLIGHT_ON;
                LED_NORMAL_ON;
                appPowerSaveTimeOutReset();
                HOST_PROF_LOG_ADD(LEVEL_ERROR, "main: LCD wake up... and drop key msg");
                return;
            }
        }
        appPowerSaveTimeOutReset();

        /*#if HOST_DBG
        appKeyMsgDebugPrint(msg);
        #endif*/
    }

    if (appStateCommonProc(msg,param) == APPSTATE_MSG_DEFAULT)
    {
        if (appStateGroupBaseProc(msg,param) == APPSTATE_MSG_DEFAULT)
        {
            appStateOnProc(msg,param);
        }
    }

    #if HOST_DBG
    if ((msg&0xffffff00) != (SP5K_MSG_AE_READY&0xffffff00))
    {
        outTime = appMsTimeGet();
        /*printf("### %d : msg=%08x param=%08x done (out-in=%d)\n\n", outTime, msg, param, outTime-inTime);*/
    }
    #endif
}

