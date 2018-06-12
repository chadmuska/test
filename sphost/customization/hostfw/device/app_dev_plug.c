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
 *	File name    : App_dev_plug.c
 *	Function     : About device plug handle implementation.
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.06  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"

#include "sp5k_disk_api.h"

#include "app_dev_plug.h"
#include "app_tvlcd.h"
#include "app_disk.h"
#include "app_video.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)
#define READ8(a) (*((volatile UINT8*)(a)))

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef enum {
	DEVICE_NONE_CHANGED = 0x00,
	DEVICE_PLUG_IN,
	DEVICE_PLUG_OUT,
} appDeviceChangeStatus_e;

typedef struct appDeviceStatus_s {
	UINT32 isCardPlugChanged;
	UINT32 isTvPlugChanged;
	UINT32 isUsbPlugChanged;
	UINT32 isHDMIPlugChanged;
} appDeviceStatus_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static appDeviceStatus_t appDevPlugInfo = {DEVICE_NONE_CHANGED, DEVICE_NONE_CHANGED, DEVICE_NONE_CHANGED, DEVICE_NONE_CHANGED};
UINT16 HDMI_IN_DLY = 1000;
UINT16 HDMI_OUT_DLY = 500;
static UINT8 HDMI_PLUG_STATE = HDMI_OUT_DONE;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

extern UINT32 previousStateGet;
extern UINT32 _powerOnStateGet();

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

static void appHdmiDebounceConfirm(UINT32 msg);
static void appHdmiHwConfirm(BOOL IN);
static void appHdmiConfirmRdy(void);

/*--------------------------------------------------------------------------*
* Function name    : appHotPlugNextStChange
* Function         : When hotplug occur, will back to default(init) group state.
                     This function only handle card plug now.
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appHotPlugNextStChange(void)
{
	UINT32 nextState;

	switch(STATE_GROUP(appPreviousStateGet()))
	{
		case APP_STATE_GROUP_VIEW:
			nextState = APP_STATE_STILL_PREVIEW;
			break;
		case APP_STATE_GROUP_VIDEO:
			nextState = APP_STATE_VIDEO_PREVIEW;
			break;
		case APP_STATE_GROUP_PLAYBACK:
			nextState = APP_STATE_PB_MAIN;
			break;
		case APP_STATE_GROUP_USB:
			nextState = APP_STATE_USB_CREATE;
			break;
		default:
			nextState = APP_STATE_VIDEO_PREVIEW;
			break;
	}
	if(appPreviousStateGet() == APP_STATE_USB_PCCAM){
		nextState = APP_STATE_USB_PCCAM;
	}
	appStateChange(nextState,STATE_PARAM_CARD_PLUG);
}

/*--------------------------------------------------------------------------*
* Function name    : _cardPlugHandler
* Function         : Card plug handle.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
_cardPlugHandler(
	void
)
{
	UINT32 cfgSet;

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "devplug: isCardPlugChanged=%d, IS_CARD_EXIST = %d",appDevPlugInfo.isCardPlugChanged,IS_CARD_EXIST);
	#if 0
	if (((appDevPlugInfo.isCardPlugChanged == DEVICE_PLUG_IN) && !IS_CARD_EXIST) ||((appDevPlugInfo.isCardPlugChanged == DEVICE_PLUG_OUT) && IS_CARD_EXIST)) {
		printf("[WARN]Card Plug change status is [%08x] but Card gpio status is [%d]\n",appDevPlugInfo.isCardPlugChanged,IS_CARD_EXIST);
		appDevPlugInfo.isCardPlugChanged = DEVICE_NONE_CHANGED;
		return;
	}
	#endif

	cfgSet = CARD_PLUG_CFG_GET(appActiveStateHotPlugCfgGet());

	switch(cfgSet) {
		case HOT_PLUG_IGNORE:
			HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: card HOT_PLUG_IGNORE");
			appDevPlugInfo.isCardPlugChanged = DEVICE_NONE_CHANGED;
			break;
		case HOT_PLUG_PENDING:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: card HOT_PLUG_PENDING");
			break;
		case HOT_PLUG_DO_PROC:
            HOST_PROF_LOG_ADD(LEVEL_INFO, "devplug: card HOT_PLUG_DO_PROC");
			appDevPlugInfo.isCardPlugChanged = DEVICE_NONE_CHANGED;
			#if POWER_OFF_AFTER_CARD_PLUG
			appPowerOffUiMsgSend(APP_POWER_OFF_CARD_PLUG);
			#else
			appStateChange(APP_STATE_CARD_PLUG,STATE_PARAM_CARD_PLUG);
			#endif
			break;
		case HOT_PLUG_DO_ABORT:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: card HOT_PLUG_DO_ABORT");
			sp5kHostMsgSend(APP_STATE_MSG_ABORT, STATE_PARAM_CARD_PLUG);
			break;
		case HOT_PLUG_DO_POWER_OFF:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: card HOT_PLUG_DO_POWER_OFF");
			appDevPlugInfo.isCardPlugChanged = DEVICE_NONE_CHANGED;
			appPowerOffUiMsgSend(APP_POWER_OFF_CARD_PLUG);
			break;
		default:
			break;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : _usbPlugHandler
* Function         : Usb plug handle.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
_usbPlugHandler(
	void
)
{
	UINT32 cfgSet;

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "devplug: isUsbPlugChanged = %d, IS_USB_IN = %d",appDevPlugInfo.isUsbPlugChanged,IS_USB_IN);

	#ifndef FPGA
	if (((appDevPlugInfo.isUsbPlugChanged == DEVICE_PLUG_IN) && !IS_USB_IN)
		||((appDevPlugInfo.isUsbPlugChanged == DEVICE_PLUG_OUT) && IS_USB_IN))
	{
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "devplug: USB Plug change status is [%08x] but USB gpio status is [%d]"\
            ,appDevPlugInfo.isUsbPlugChanged,IS_USB_IN);
		appDevPlugInfo.isUsbPlugChanged = DEVICE_NONE_CHANGED;
		return;
	}
	#endif
	cfgSet = USB_PLUG_CFG_GET(appActiveStateHotPlugCfgGet());

	switch(cfgSet)
	{
		case HOT_PLUG_IGNORE:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: usb HOT_PLUG_IGNORE");
			appDevPlugInfo.isUsbPlugChanged = DEVICE_NONE_CHANGED;
			break;
		case HOT_PLUG_PENDING:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: usb HOT_PLUG_PENDING");
			break;
		case HOT_PLUG_DO_PROC:
			HOST_PROF_LOG_ADD(LEVEL_INFO, "devplug: usb HOT_PLUG_DO_PROC");
			if (appDevPlugInfo.isUsbPlugChanged == DEVICE_PLUG_IN)
			{
				appDevPlugInfo.isUsbPlugChanged = DEVICE_NONE_CHANGED;
				appStateChange(APP_STATE_USB_CREATE,STATE_PARAM_USB_PLUG);
			}
			else
			{
				appDevPlugInfo.isUsbPlugChanged = DEVICE_NONE_CHANGED;
				appLcdTurnOn();
				#if POWER_OFF_AFTER_USB_OUT
				appPowerOffUiMsgSend(APP_POWER_OFF_USB_PLUG);
				#else
				sp5kHostMsgSend(APP_UI_MSG_USB_PLUG, 0x00);
				#endif
			}
			break;
		case HOT_PLUG_DO_ABORT:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: usb HOT_PLUG_DO_ABORT");
			sp5kHostMsgSend(APP_STATE_MSG_ABORT, STATE_PARAM_USB_PLUG);
			break;
		case HOT_PLUG_DO_POWER_OFF:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: usb HOT_PLUG_DO_POWER_OFF");
			appDevPlugInfo.isUsbPlugChanged = DEVICE_NONE_CHANGED;
			appPowerOffUiMsgSend(APP_POWER_OFF_USB_PLUG);
			break;
		default:
			break;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : _tvPlugHandler
* Function         : TV plug handle.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
_tvPlugHandler(
	void
)
{
	UINT32 cfgSet;
	static UINT8 cnt=1;
    HOST_PROF_LOG_PRINT(LEVEL_INFO, "devplug: isTvPlugChanged = %d, IS_TV_IN = %d",appDevPlugInfo.isTvPlugChanged,IS_TV_IN);

    if (((appDevPlugInfo.isTvPlugChanged == DEVICE_PLUG_IN) && !IS_TV_IN)
		||((appDevPlugInfo.isTvPlugChanged == DEVICE_PLUG_OUT) && IS_TV_IN))
	{
        HOST_PROF_LOG_PRINT(LEVEL_WARN, "devplug: TV Plug change status is [%08x] but TV gpio status is [%d]"\
            ,appDevPlugInfo.isTvPlugChanged,IS_TV_IN);
		if(cnt != 0)
		{
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 500);
			if(appDevPlugInfo.isTvPlugChanged == DEVICE_PLUG_IN)
				sp5kHostMsgSend(APP_KEY_TV_IN, 0);
			else
				sp5kHostMsgSend(APP_KEY_TV_OUT, 0);
			cnt--;
			return;
		}
		appDevPlugInfo.isTvPlugChanged = DEVICE_NONE_CHANGED;
		return;
	}
	cfgSet = TV_PLUG_CFG_GET(appActiveStateHotPlugCfgGet());
	cnt = 1;

	switch(cfgSet) {
		case HOT_PLUG_IGNORE:
			HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: tv HOT_PLUG_IGNORE");
			appDevPlugInfo.isTvPlugChanged = DEVICE_NONE_CHANGED;
			break;
		case HOT_PLUG_PENDING:
			HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: tv HOT_PLUG_PENDING");
			break;
		case HOT_PLUG_DO_PROC:
			HOST_PROF_LOG_ADD(LEVEL_INFO, "devplug: tv HOT_PLUG_DO_PROC");
			appDevPlugInfo.isTvPlugChanged = DEVICE_NONE_CHANGED;
			appStateChange(APP_STATE_TV_PLUG,STATE_PARAM_TV_PLUG);
			break;
		case HOT_PLUG_DO_ABORT:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: tv HOT_PLUG_DO_ABORT");
			sp5kHostMsgSend(APP_STATE_MSG_ABORT, STATE_PARAM_TV_PLUG);
			break;
		case HOT_PLUG_DO_POWER_OFF:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: tv HOT_PLUG_DO_POWER_OFF");
			appDevPlugInfo.isTvPlugChanged = DEVICE_NONE_CHANGED;
			appPowerOffUiMsgSend(STATE_PARAM_TV_PLUG);
			break;
		default:
			break;
	}

}

#if (KIT_WITH_HDMI)

/*--------------------------------------------------------------------------*
* Function name    : _hdmiPlugHandler
* Function         : HDMI plug handle.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
_hdmiPlugHandler(
	void
)
{
	UINT32 cfgSet;

    HOST_PROF_LOG_PRINT(LEVEL_INFO, "devplug: ishdmiPlugChanged = %d, IS_HDMI_IN = %d",appDevPlugInfo.isHDMIPlugChanged,IS_HDMI_IN);


	cfgSet = HDMI_PLUG_CFG_GET(appActiveStateHotPlugCfgGet());

	switch(cfgSet) {
		case HOT_PLUG_IGNORE:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: HDMI HOT_PLUG_IGNORE");
			appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
			break;
		case HOT_PLUG_PENDING:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: HDMI HOT_PLUG_PENDING");
			break;
		case HOT_PLUG_DO_PROC:
            HOST_PROF_LOG_ADD(LEVEL_INFO, "devplug: HDMI HOT_PLUG_DO_PROC");

			if(appActiveStateGet() == APP_STATE_VIDEO_REC){
				appVideoStop();
				appStateChange(APP_STATE_VIDEO_PREVIEW,STATE_PARAM_NORMAL_INIT);
                }else if (appActiveStateGet() == APP_STATE_POWER_ON || appActiveStateGet() == APP_STATE_NULL ||
                appActiveStateGet() == APP_STATE_HDMI_PLUG|| appActiveStateGet() == APP_STATE_CEC_CAPABILITY_CTRL)/*No ideato get pre pre state, so use to power on state*/
                {
                    previousStateGet = _powerOnStateGet();
                }else{
                    previousStateGet = appActiveStateGet();
			}
            if (IS_CEC_ON && appDevPlugInfo.isHDMIPlugChanged == DEVICE_PLUG_IN)
    			appStateChange(APP_STATE_CEC_CAPABILITY_CTRL,STATE_PARAM_HDMI_PLUG);
            else
    			appStateChange(APP_STATE_HDMI_PLUG, STATE_PARAM_NORMAL_INIT);
			appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
			break;
		case HOT_PLUG_DO_ABORT:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: HDMI APP_STATE_MSG_ABORT");
			sp5kHostMsgSend(APP_STATE_MSG_ABORT, STATE_PARAM_HDMI_PLUG);
			break;
		case HOT_PLUG_DO_POWER_OFF:
            HOST_PROF_LOG_ADD(LEVEL_WARN, "devplug: HDMI HOT_PLUG_DO_POWER_OFF");
			appDevPlugInfo.isHDMIPlugChanged = DEVICE_NONE_CHANGED;
			appPowerOffUiMsgSend(STATE_PARAM_HDMI_PLUG);
			break;
		default:
			break;
	}

}
#endif

/*--------------------------------------------------------------------------*
* Function name    : _devPlugStatusUpdate
* Function         : Update device plug status.
* Return value	   : void
* Parameter1	   : msg (UINT32 appKeyMsg_e)
* Parameter2	   : param (UINT32 no use)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
_devPlugStatusUpdate(
	UINT32 msg,
	UINT32 param
)
{
	switch(msg) {
	case APP_KEY_CARD_IN:
		appDevPlugInfo.isCardPlugChanged = DEVICE_PLUG_IN;
		break;
	case APP_KEY_CARD_OUT:
		appDevPlugInfo.isCardPlugChanged = DEVICE_PLUG_OUT;
		break;
	case SP5K_MSG_USB_DEVICE_VBUS_IN:
		appDevPlugInfo.isUsbPlugChanged = DEVICE_PLUG_IN;
		break;
	case SP5K_MSG_USB_DEVICE_VBUS_OFF:
		appDevPlugInfo.isUsbPlugChanged = DEVICE_PLUG_OUT;
		break;
	case APP_KEY_TV_IN:
		appDevPlugInfo.isTvPlugChanged = DEVICE_PLUG_IN;
		break;
	case APP_KEY_TV_OUT:
		appDevPlugInfo.isTvPlugChanged = DEVICE_PLUG_OUT;
		break;
	case SP5K_MSG_HDMI_PLUG:
		appDevPlugInfo.isHDMIPlugChanged = DEVICE_PLUG_IN;
		break;
	case SP5K_MSG_HDMI_UNPLUG:
		appDevPlugInfo.isHDMIPlugChanged = DEVICE_PLUG_OUT;
		break;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appDevPlugStatusChange
* Function         : Check is device plug status change,
                     and do card, usb, tv, hdmi plug handle.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appDevPlugStatusChange(
	void
)
{
	if (appDevPlugInfo.isCardPlugChanged != DEVICE_NONE_CHANGED)
	{

        _cardPlugHandler();

	}
	if(appDevPlugInfo.isUsbPlugChanged != DEVICE_NONE_CHANGED)
	{
		_usbPlugHandler();
	}
	if(appDevPlugInfo.isTvPlugChanged != DEVICE_NONE_CHANGED)
	{
		_tvPlugHandler();
	}
#if (KIT_WITH_HDMI)
	if(appDevPlugInfo.isHDMIPlugChanged != DEVICE_NONE_CHANGED)
	{
		_hdmiPlugHandler();
	}
#endif
}

/*--------------------------------------------------------------------------*
* Function name    : appDevHotPlugHandler
* Function         : Uninterrupted power, plug the device to work properly concept.
                     It's related processing.
* Return value	   : void
* Parameter1	   : msg (UINT32 appKeyMsg_e)
* Parameter1	   : param (UINT32 no use)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appDevHotPlugHandler(
	UINT32 msg,
	UINT32 param
)
{
#if (KIT_WITH_HDMI)
    if (msg == SP5K_MSG_HDMI_PLUG || msg == SP5K_MSG_HDMI_UNPLUG)
    {
        printf("appDevHotPlugHandler msg= 0x%X, HDMI=%d state=%X\n", msg, IS_HDMI_IN, HDMI_PLUG_STATE);
        appHdmiDebounceConfirm(msg);
    }
    else
#endif
    {
    	_devPlugStatusUpdate(msg,param); // update dev plug status
    	appDevPlugStatusChange(); // change dev plug status
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appHdmiDebounceConfirm
* Function         : Confirm is Hdmi debounce.
* Return value	   : void
* Parameter1	   : msg (UINT32 SP5K_MSG_HDMI_PLUG/SP5K_MSG_HDMI_UNPLUG)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

#define  MAX_CONFIRM_NUM  10
BOOL ConfirmQeue[MAX_CONFIRM_NUM];
UINT8 ConfirmSet=0, ConfirmGet=0;


void appHdmiDebounceConfirm(UINT32 msg)
{
      /*Maybe message send, but HDMI already out, so double confirm by IS_HDMI_IN*/
        if (msg==SP5K_MSG_HDMI_PLUG && IS_HDMI_IN)
        {
            if ((HDMI_PLUG_STATE&HDMI_IN_STAGE) == HDMI_IN_STAGE || HDMI_PLUG_STATE == HDMI_IN_DONE)
            {
                ;/*alreay IN, so skip new IN message*/
            }
            else if ((HDMI_PLUG_STATE&HDMI_OUT_STAGE) == HDMI_OUT_STAGE)
            {
                if (HDMI_PLUG_STATE == HDMI_OUT_CONFIRM)
                {
                    HDMI_PLUG_STATE = HDMI_IN_DONE;
                }
                else if (HDMI_PLUG_STATE == HDMI_OUT_PROC)
                {
                    HDMI_PLUG_STATE = HDMI_IN_CONFIRM;
        		    appHdmiHwConfirm(HDMI_IN);
                }
            }
            else if (HDMI_PLUG_STATE == HDMI_OUT_DONE)
            {
                HDMI_PLUG_STATE = HDMI_IN_CONFIRM;
        		appHdmiHwConfirm(HDMI_IN);
            }
        }
        else if (msg==SP5K_MSG_HDMI_UNPLUG && !IS_HDMI_IN)
        {
            if ((HDMI_PLUG_STATE&HDMI_OUT_STAGE) == HDMI_OUT_STAGE || (HDMI_PLUG_STATE == HDMI_OUT_DONE))
            {
            printf("%d\n",__LINE__);
                ;/*alreay IN, so skip new IN stage*/
            }
            else if ((HDMI_PLUG_STATE&HDMI_IN_STAGE) == HDMI_IN_STAGE)
            {
                        printf("%d\n",__LINE__);
                if (HDMI_PLUG_STATE == HDMI_IN_CONFIRM)
                {
                    HDMI_PLUG_STATE = HDMI_OUT_DONE;
                }
                else if (HDMI_PLUG_STATE == HDMI_IN_PROC)
                {
                    HDMI_PLUG_STATE = HDMI_OUT_CONFIRM;
            		appHdmiHwConfirm(HDMI_OUT);
                }
            }
            else if (HDMI_PLUG_STATE == HDMI_IN_DONE)
            {
                        printf("%d\n",__LINE__);
                HDMI_PLUG_STATE = HDMI_OUT_CONFIRM;
        		appHdmiHwConfirm(HDMI_OUT);
            }
            else
            {
                printf("%d\n",__LINE__);
            }
        }
        else
        {
            //Skip for IN<->OUT during very short time.
            return;
        }
}

/*--------------------------------------------------------------------------*
* Function name    : appHdmiHwConfirm
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appHdmiHwConfirm(BOOL IN)
{
    printf("start confirm HDMI=%d (time=%d)\n", IS_HDMI_IN, sp5kOsTimeGet());
    if (IN)
    {
        HDMI_PLUG_STATE = HDMI_IN_CONFIRM;
        ConfirmQeue[ConfirmSet] = HDMI_IN;
        sp5kTimerIsrOneShotReg( 0, HDMI_IN_DLY, appHdmiConfirmRdy);
    }
    else
    {
        HDMI_PLUG_STATE = HDMI_OUT_CONFIRM;
        ConfirmQeue[ConfirmSet] = HDMI_OUT;
        sp5kTimerIsrOneShotReg( 0, HDMI_OUT_DLY, appHdmiConfirmRdy);
    }
    ConfirmSet = (ConfirmSet+1) % MAX_CONFIRM_NUM;
}

/*--------------------------------------------------------------------------*
* Function name    : appHdmiConfirmRdy
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appHdmiConfirmRdy(void)
{
printf("confirm ready HDMI=%d time=%d, ptr=(%d,%d)\n", IS_HDMI_IN, sp5kOsTimeGet(), ConfirmSet, ConfirmGet);
    if (ConfirmQeue[ConfirmGet]==HDMI_IN && IS_HDMI_IN)
    {
        printf("After HW confirm (%d)ms for HDMI PLUG\n", HDMI_IN_DLY);
        HDMI_PLUG_STATE = HDMI_IN_PROC;
        _devPlugStatusUpdate(SP5K_MSG_HDMI_PLUG,0);
       	appDevPlugStatusChange();
    }
    else if (ConfirmQeue[ConfirmGet]==HDMI_OUT && !IS_HDMI_IN)
    {
        printf("After HW confirm (%d)ms for HDMI UNPLUG\n", HDMI_OUT_DLY);
        HDMI_PLUG_STATE = HDMI_OUT_PROC;
        _devPlugStatusUpdate(SP5K_MSG_HDMI_UNPLUG,0);
      	appDevPlugStatusChange();
    }
    else
    {
        printf("!!After HW confirm, ignore this HDMI!! %d\n", IS_HDMI_IN);
    }
    ConfirmGet = (ConfirmGet+1) % MAX_CONFIRM_NUM;
printf("appHdmiConfirmRdy() E HDMI_PLUG_STATE  0x%X\n", HDMI_PLUG_STATE);

}

/*--------------------------------------------------------------------------*
* Function name    : appHdmiStatusSet
* Function         : Set Hdmi status
* Return value	   : void
* Parameter1	   : IN (BOOL appHdmiPlugStatus_e, IS_HDMI_IN)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appHdmiStatusSet(BOOL IN)
{
printf("appHdmiStatusSet IN=%d  state=0x%X\n", IN, HDMI_PLUG_STATE);
    /*Make respect*/
    if (IN && HDMI_PLUG_STATE==HDMI_IN_PROC)
        HDMI_PLUG_STATE = HDMI_IN_DONE;
    else if (!IN && HDMI_PLUG_STATE==HDMI_OUT_PROC)
        HDMI_PLUG_STATE = HDMI_OUT_DONE;
}

/*--------------------------------------------------------------------------*
* Function name    : appHdmiStateGet
* Function         : Get Hdmi status
* Return value	   : HDMI_PLUG_STATE (UINT8 appHdmiPlugDetailStatus_e)
* Parameter1	   : void
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

UINT8 appHdmiStateGet(void)
{
    return HDMI_PLUG_STATE;
}
