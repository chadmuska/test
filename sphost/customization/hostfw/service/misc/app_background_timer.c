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
 *	File name    : App_background_timer.c
 *	Function     : Provide flow structure which background timer service for user design.
                   Note:
                   Because background timer service register in ISR, 
                   it is not recommended to use too costly time flow EX:"printf","time delay".
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.04  Edward.Lu  "Add description and modify code"
 **************************************************************************/

#define HOST_DBG 1

#include "app_com_def.h"
#include "app_com_api.h"
#include "app_timer.h"
#include "app_battery.h"
#include "app_battery_ctrl.h"
#include "gpio_custom.h"
#include "sp5k_usb_api.h"
#include "sp5k_disk_api.h"
#include "app_disk.h"
#include "app_still.h"
#include "app_view_osd.h"
#include "app_battery_ctrl.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_rtc.h"
#include "api/sp5k_utility_api.h"
#include "jet_utility.h"
#include "app_infohandler_func.h"
#endif

#if SP5K_USB_UVC_SUPPORT
#include "sp5k_media_api.h"
#include "app_video_uvc.h"
#endif

#include "app_infohandlerComm.h"
#include "sp5k_dbg_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define BACKGROUND_TIMER_TICK       20 //20ms
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define POWER_TIMER_TICK            10 //10ms
#define AUTO_POWER_OFF_TIME_OUT     40*1000 // ms
#define APP_POWER_TIMER_THREAD_PRI  20
#else
#define AUTO_POWER_OFF_TIME_OUT     60*1000 // ms
#endif
#define BATTERY_CHECK_COUNT         50

#define USBHOST_IN                  1
#define USBHOST_OUT                 0
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define POWER_ACC_ON_FLAG           (1 << 0)
#define POWER_ACC_OFF_FLAG          (1 << 1)
#define POWER_ACC_MASK              (POWER_ACC_ON_FLAG | POWER_ACC_OFF_FLAG)
#endif
#define NO_STATE                    0xff
#define TIME_COUNT_DEFAULT          0xFFFFFFFF
#define TIME_2_MINUTE               2*60
#define TIME_100_MINUTE             100*60
#define LIMIT_BATTERY_ADC           100

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

static UINT32 isPowerSavingEnable = 1;
static UINT32 isAutoPowerOffCountDown = 0;
static UINT32 powerSave_t0 = 0;
static UINT32 count=0;
/*static UINT32 HWWDCountT = 0;*/

SP5K_EVENT_FLAGS_GROUP powerTimer_sig;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

extern BOOL bBatteryIn;

#if defined(PRJ_JETDVR) || defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern UINT32 PlayPos_TimerCount;
extern UINT32 PosReq_TimerCount;
extern UINT32 ConnectDev_TimerCount;
extern UINT32 Regular_TimerCount;
extern UINT32 SDCARD_TimerCount;

extern BOOL PlayPos_TimerCount_PAUSE ;
extern SP5K_EVENT_FLAGS_GROUP qEvent_sig;

extern PowerDetect appInfoPowerDETStateGet(void);
extern void appInfHWWDT(UINT32 ms);

UINT32 appInfoParkingByPassTimeGet(void);
#endif

extern void appBatteryLevelPolling(void);
extern void appLedFlickerServiceRoutine(void);
extern void appPsuedoPowerOff(UINT32 powerDown);
extern UINT32 appStillCheckReady(UINT32 question);

void _usbLowPwrChargeHandler(void);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern void appPowerOffUiMsgSend(UINT32 param);
extern void AppBuzzerFilckService(void);
extern void appLedGRInTurn( void );
extern void halPwrcEnable(UINT32 en);
extern void SetPowerBacklight(UINT32 en);
extern void appPsuedoPowerOff(UINT32 powerDown);

#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appUsbHostDetect
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

#if USB_DISK_SUPPORT && !SP5K_USB_UVC_SUPPORT
void appUsbHostDetect(void)
{
    static UINT8 pre_state=NO_STATE;
    UINT8 cur_state;
    cur_state = IS_DISK_USB_HOST_MOUNT;

    if(pre_state==NO_STATE && cur_state==USBHOST_OUT)
        return;

    if(cur_state!=pre_state)
    {
        if(cur_state==USBHOST_IN)
        {
            appActiveDiskSet(SP5K_DRIVE_USB);

            if(IS_MENU_STATE(appActiveStateGet()) || IS_PLAYBACK_GROUP(appActiveStateGet()))
                appStateChange(appActiveStateGet(),appActiveStateGet());

            if(appStillCheckReady(CAN_I_START_SNAP)==STILL_STATE_NO)
                return;

            HOST_PROF_LOG_ADD(LEVEL_INFO, "bktime: appUsbHostDetect in");
            if(appActiveStateGet() == APP_STATE_STILL_PREVIEW)// && pre_state!=0xff)
            {
                appStillSettingInit(VIEW_SET_DISK);
                appViewOsdDiskShow();
                sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, DISABLE, DISABLE, DISABLE, DISABLE );
                #if !SP5K_PVQV_DISABLE
                appViewOsd_PvQV_DiskDraw();
                #endif
                sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, ENABLE, DISABLE, DISABLE, DISABLE );
            }
        }
        else
        {
             HOST_PROF_LOG_ADD(LEVEL_INFO, "bktime: appUsbHostDetect out");
            (IS_CARD_EXIST)?appActiveDiskSet(SP5K_DRIVE_SD):appActiveDiskSet(SP5K_DRIVE_NAND);

            if(IS_MENU_STATE(appActiveStateGet()) || IS_PLAYBACK_GROUP(appActiveStateGet()))
                appStateChange(appActiveStateGet(),appActiveStateGet());

            if(appActiveStateGet() == APP_STATE_STILL_PREVIEW)
            {
                appStillSettingInit(VIEW_SET_DISK);
                appViewOsdDiskShow();
                sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, DISABLE, DISABLE, DISABLE, DISABLE );
                #if !SP5K_PVQV_DISABLE
                appViewOsd_PvQV_DiskDraw();
                #endif
                sp5kGfxAttrSet( SP5K_GFX_REFRESH_ACTIVE, ENABLE, DISABLE, DISABLE, DISABLE );
            }
        }
        pre_state = cur_state;
    }
    else
    {
        return;
    }
}
#endif

/*--------------------------------------------------------------------------*
* Function name    : appUsbDetect
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

BOOL appUsbDetect(void)
{
	static UINT32 preValue=0;
	UINT32 curValue;
	static BOOL bsrc_PC = FALSE;
	BOOL EnterPCUSB = FALSE;

	curValue=IS_USB_IN;

	if((curValue == TRUE) && (preValue == FALSE))
	{
		{
			UINT32  bChargePortType;
		    bChargePortType = sp5kUsbChargePortTypeGet();
            bsrc_PC = BATTERY_DCSRC_ADAPTER ;
		    switch (bChargePortType){
                case SP5K_USB_DEDICATED_CHARGING_PORT_OPEN :
	                printf("USB_PORT_OPEN\n") ;
		            break ;
	    		case SP5K_USB_DEDICATED_CHARGING_PORT_SHORT :
		            printf("USB_PORT_SHORT\n");
		            break ;
			 	case SP5K_USB_DEDICATED_CHARGING_PORT_DPGND :
			        printf("USB_PORT_DPGND\n");
		            break ;
				case SP5K_USB_STANDARD_DOWNSTREAM_PORT :
			        printf("USB_STANDARD DOWNSTREAM\n");
					bsrc_PC = BATTERY_DCSRC_PC ;
		            break ;
				case SP5K_USB_CHARGING_DOWNSTREAM_PORT :
			        printf("USB_CHARGE DOWNSTREAM\n");
					bsrc_PC = BATTERY_DCSRC_PC ;
		            break ;
                default:
			        printf("Unkown USB type, be a adapter\n");
		    }

			if(bsrc_PC == BATTERY_DCSRC_PC){
				#if MSG_VBUS       /*Shen: Don't delete! host sholud be action by APP_MSG, flow not take down*/
					sp5kHostMsgSend(SP5K_MSG_USB_DEVICE_VBUS_IN,DISABLE);
       			#endif
			}

			bBatteryIn = (appBatteryAdcValueGet() > LIMIT_BATTERY_ADC) ? TRUE:FALSE;
			if(bBatteryIn){
				appBatteryCharge_SetStartCBF();
				RED_LED_ON;
			}
		}
	}
	else if((curValue == FALSE) && (preValue == TRUE))
	{
		if(bsrc_PC != BATTERY_DCSRC_ADAPTER) {
			#if MSG_VBUS /*Shen: Don't delete! host sholud be action by APP_MSG, flow not take down*/
				sp5kHostMsgSend(SP5K_MSG_USB_DEVICE_VBUS_OFF,DISABLE);
			#endif
        }

		if(bBatteryIn){
			appBatteryCharge_SetStopCBF();
			RED_LED_OFF;
		}
	}

	if(curValue == TRUE)
	{
		count++;
		if(count > BATTERY_CHECK_COUNT && bBatteryIn)
		{
		    if(appBatteryCharge_SetCheckStatusCBF() == BATTERY_STATUS_FULL)
			{
				if (appBatteryCharge_ChargerStatusGet()==TRUE){
		        appBatteryCharge_SetStopCBF();
					RED_LED_OFF;
				}
			}
	        count = 0;
	    }
	}
	preValue = curValue;

    return EnterPCUSB;
}

#if defined(PRJ_JETDVR) || defined(PRJ_17DVR)||defined(PRJ_19DVR)
/*
void JetHWWatchDog(void)
{
	UINT32 pwrVal;
	UINT32 timeOutInMs = 1000;
	UINT32 state = INFO_QUEUEMSG_HWWDT;
	UINT32 err;
	
	HWWDCountT = HWWDCountT+BACKGROUND_TIMER_TICK;
	if(HWWDCountT > timeOutInMs)
	{

	HWWDCountT = 0;
	appInfHWWDT(500);
		
	}
}

*/

/*--------------------------------------------------------------------------*
* Function name    : appInfoTimerPolling
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

/*paul 2018.04.16 remove unused variable  */
void appInfoTimerPolling(void)
{
	UINT32 err;
	UINT32 timeOutInMs =  1000;
#if INFO_QUEUE_TEST
	UINT32 state;
#endif

	if(PlayPos_TimerCount != TIME_COUNT_DEFAULT && PlayPos_TimerCount_PAUSE==FALSE)
	{

		PlayPos_TimerCount = PlayPos_TimerCount + BACKGROUND_TIMER_TICK;
		if(jet_avn_pb_para_get()!=NULL){
			if(jet_avn_pb_para_get()->speed == AVN_PB_PARA_SPEED_X4){
				timeOutInMs = 250;
			}
		}
		if (PlayPos_TimerCount >= timeOutInMs) {
			
			PlayPos_TimerCount = 0;
#if INFO_QUEUE_TEST
			state = INFO_QUEUEMSG_PLAYPOS;
			err = sp5kHostMsgSend(state, 0);
			HOST_ASSERT(err == SP5K_SUCCESS);
#endif
			err = sp5kOsEventFlagsSet(&qEvent_sig, INFO_QUEUEMSG_THREAD_PLAYPOS, TX_INT_ENABLE);
    			HOST_ASSERT(err == SP5K_SUCCESS);
		}
		timeOutInMs = 1000;
	}
	if(PosReq_TimerCount != TIME_COUNT_DEFAULT )
	{

		PosReq_TimerCount = PosReq_TimerCount + BACKGROUND_TIMER_TICK;
		if (PosReq_TimerCount >= timeOutInMs) {
			
			PosReq_TimerCount = 0;
		#if INFO_QUEUE_TEST
			state = INFO_QUEUEMSG_POSREQUEST;
			err = sp5kHostMsgSend(state, 0);
			HOST_ASSERT(err == SP5K_SUCCESS);
		#endif
			err = sp5kOsEventFlagsSet(&qEvent_sig, INFO_QUEUEMSG_THREAD_POSREQUEST, TX_INT_ENABLE);
    			HOST_ASSERT(err == SP5K_SUCCESS);
		}
	}
	if(ConnectDev_TimerCount != TIME_COUNT_DEFAULT )
	{

		ConnectDev_TimerCount = ConnectDev_TimerCount + BACKGROUND_TIMER_TICK;
		if (ConnectDev_TimerCount >= timeOutInMs) {
			
			ConnectDev_TimerCount = 0;
		#if INFO_QUEUE_TEST 
			state = INFO_QUEUEMSG_CONNECTDEV;
			err = sp5kHostMsgSend(state, 0);
			HOST_ASSERT(err == SP5K_SUCCESS);
		#endif
			err = sp5kOsEventFlagsSet(&qEvent_sig, INFO_QUEUEMSG_THREAD_CONNECTDEV, TX_INT_ENABLE);
      			HOST_ASSERT(err == SP5K_SUCCESS);
		}
	}
	if(Regular_TimerCount != TIME_COUNT_DEFAULT )
	{
		Regular_TimerCount = Regular_TimerCount + BACKGROUND_TIMER_TICK;
		if (Regular_TimerCount >= timeOutInMs) {
			
			Regular_TimerCount = 0;
		#if INFO_QUEUE_TEST 
			state = INFO_QUEUEMSG_REGULAR;
			err = sp5kHostMsgSend(state, 0);
			HOST_ASSERT(err == SP5K_SUCCESS);
		#endif
			err = sp5kOsEventFlagsSet(&qEvent_sig, INFO_QUEUEMSG_THREAD_REGULAR, TX_INT_ENABLE);
      			HOST_ASSERT(err == SP5K_SUCCESS);
		}
	}

	if(SDCARD_TimerCount != TIME_COUNT_DEFAULT)
	{

		SDCARD_TimerCount = SDCARD_TimerCount + BACKGROUND_TIMER_TICK;
		if (SDCARD_TimerCount >= timeOutInMs) {
			
			SDCARD_TimerCount = 0;
		#if INFO_QUEUE_TEST
			state = INFO_QUEUEMSG_SDPLUGTIMER;
			err = sp5kHostMsgSend(state, 0);
			HOST_ASSERT(err == SP5K_SUCCESS);
		#endif
			err = sp5kOsEventFlagsSet(&qEvent_sig, INFO_QUEUEMSG_THREAD_SDPLUGTIMER, TX_INT_ENABLE);
      			HOST_ASSERT(err == SP5K_SUCCESS);
		}
	}
}

#if INFO_QUEUE_TEST
static int _queue_busy;

void JetQueueBusySet(int st){
	_queue_busy = st;
}
#endif
/* default disable off until system ready */
static int _can_power_off = 0;

/*--------------------------------------------------------------------------*
* Function name    : JetPowerCanOff
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void JetPowerCanOff(int st)
{
	printf("System %s off (%d)\n", st ? "can": "cannot", st);
	_can_power_off = st;
}

/*--------------------------------------------------------------------------*
* Function name    : JetPowerCanOffState
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

int JetPowerCanOffState(void)
{
	return _can_power_off;
}

static int accOnCount = 0;
static int accOffCount = 0;
static int timeOutInMs = -40000; /* from off signal to power need 5 sec */
static int parkrecord_time = 0;

BOOL reboot_flag = FALSE;

/*--------------------------------------------------------------------------*
* Function name    : JetPowerWatch
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void JetPowerWatch(void)
{
    UINT32 pwrVal;
    ULONG flag;
    static UINT32 count;
    static ULONG preFlag;

    if (timeOutInMs < (TIME_100_MINUTE*1000)) /* preven overflow of timer */
        timeOutInMs += POWER_TIMER_TICK;
    sp5kGpioRead(SP5K_GPIO_GRP_POCU, 0xffffffff, &pwrVal);
    if (pwrVal & BIT1) { /* ACC on */
        accOffCount = 0;
        accOnCount += POWER_TIMER_TICK;
        flag = POWER_ACC_ON_FLAG;
    } else { /* ACC off */
        accOnCount = 0;
#if POWER_TEST
        if (_can_power_off != 0) {
            accOffCount += POWER_TIMER_TICK;
        }
#endif
        accOffCount += POWER_TIMER_TICK;
        flag = POWER_ACC_OFF_FLAG;
        if (accOffCount >= (parkrecord_time + (TIME_2_MINUTE*1000))) {
            printf("Force power off over %d seconds\n",
                    (parkrecord_time + (TIME_2_MINUTE*1000)));
            appInfoSWWDTWakeup(0);// second
            halPwrcEnable(DISABLE);
            while (1)
                ;
        }
    }
    if (count == 0 || preFlag != flag) {
        sp5kOsEventFlagsSet(&powerTimer_sig, flag, TX_OR);
        if (preFlag != flag) {
            printf("ACC changed 0x%u\n", flag);
            preFlag = flag;
	}
    } else {
        count++;
        count %= 4;
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appPowerTimerThread
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

/*paul 2018.04.16 remove unused variable  */
void appPowerTimerThread(ULONG param)
{
    static int bypass_time = 0;
    UINT32 err;
    UINT32 flags;
    PowerDetect pd;

    while (1) {
        sp5kOsEventFlagsGet(&powerTimer_sig, POWER_ACC_MASK, TX_OR_CLEAR,
                &flags,
                TX_WAIT_FOREVER);

        #if DEBUG_ON
        debugcount++;
        if(debugcount == 50) {
            printf("%s  %d   %d\n",__FUNCTION__ , accOffCount , _queue_busy);
            debugcount = 0;
        }
        #endif

        pd = appInfoPowerDETStateGet();
		
        #if POWER_TEST
        if (pd == POWER_DET_ACC_OFF) {
            printf("%s(%d) POWER OFF\n", __func__, __LINE__);
            continue;
        }
        #endif
		
        if (flags & POWER_ACC_ON_FLAG) {
            if (accOnCount >= 10 * POWER_TIMER_TICK) {
                if (pd != POWER_DET_ACC_ON && pd !=POWER_DET_ACC_OFF) {
                    while (1) {
			            UINT32 bat;
                        /* use RTC wakeup to reboot system */
                        printf("reboot on (%d)\n", pd);
                        reboot_flag = TRUE;
                        bat = appInfoBatteryLvGet();
                        appInfoSWWDTReset();
                        if (bat <  5900 ) {
                            appInfoSWWDTWakeup(0);// second
                        } else if (bat < 9900) {
                            appInfoSWWDTWakeup(6);
                        } else
                            appInfoSWWDTWakeup(2);
                        err = sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, param);
                        HOST_ASSERT(err == SP5K_SUCCESS);
                        sp5kOsThreadSleep(3000);
                    }
                }
            }
            continue;
        } 
		else {
            if (accOffCount >= (55 * POWER_TIMER_TICK)
                    && pd == POWER_DET_ACC_ON && _can_power_off) {
#if POWER_TEST
                printf("%s(%d) ACC off\n", __func__, __LINE__);
                if (appActiveStateGet() == APP_STATE_VIDEO_REC) {
                    appTimeDelayMs(100); /*add for mantis bug 0050816*/
                    appVideoStop();
                } else if (appActiveStateGet() == APP_STATE_PB_MAIN) {
                    if (jet_pb_image_pip_is_in_pip_get() == TRUE)
                        jet_pb_image_pip_exit();
                }
#endif
                if (pUiSetting->ParkingModesetting) {
                    /* from off signal to power need 5 sec */
                    bypass_time = appInfoParkingByPassTimeGet();
                    if (timeOutInMs < 0) {
                        bypass_time += (timeOutInMs / 1000);
                        if (bypass_time < 2)
                            bypass_time = 2;
                    }
                    printf("PARKING MODE %d %d\n", bypass_time, timeOutInMs);
                }

                if (timeOutInMs < 0) {
                    printf("POWER ACC OFF SIGNAL\n");
                    err = sp5kHostMsgSend(INFO_QUEUEMSG_POWER,
                            POWER_DET_ACC_OFF_SIGNAL);
                    HOST_ASSERT(err == SP5K_SUCCESS);
                } else {
                    printf("POWER ACC OFF after OFF SIGNAL\n");
                    appInfoSWWDTWakeup(bypass_time);
                    err = sp5kHostMsgSend(INFO_QUEUEMSG_POWER,
                            POWER_DET_ACC_OFF);
                    HOST_ASSERT(err == SP5K_SUCCESS);
                }
            } 
			else if (timeOutInMs >= 0 && pd == POWER_DET_ACC_OFF_SIGNAL) {
                printf("POWER ACC OFF after OFF SIGNAL\n");
                appInfoSWWDTWakeup(bypass_time);
                err = sp5kHostMsgSend(INFO_QUEUEMSG_POWER, POWER_DET_ACC_OFF);
                HOST_ASSERT(err == SP5K_SUCCESS);
            }
            /*Acc off signal trigger over 2min , need to do power off  */
        }
    }
}

#endif

/*--------------------------------------------------------------------------*
* Function name    : _powerSavePolling
* Function         : Power saving polling flow
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void
_powerSavePolling(
	void
)
{
	UINT32 time[POWER_SAVE_MAX] = { 0, 1 };
	UINT32 timeOutInMs = (time[pUiSetting->powerSave]/*minute*/ * 60 * 1000);/*Ms*/
	#if AUTO_POWER_OFF_DISABLE
	return;
	#endif

    #if SP5K_USB_UVC_SUPPORT
    UINT32 elsTime;
    if(appUvcPwrOnRecGet())
    {
        timeOutInMs = UVC_PWR_RECORD_TMR;
        sp5kMediaRecAttrGet(SP5K_MEDIA_ATTR_ELAPSED_TIME, &elsTime);
    	if (elsTime >= timeOutInMs) {
    		HOST_PROF_LOG_PRINT(LEVEL_INFO, "uvc: rec pwr off");
            appUvcPwrOnRecSet(PWR_ON_REC_DIS);
    		appPowerOffUiMsgSend(APP_POWER_OFF_APO);
    	}
        return;
    }
    #endif

	if (!isPowerSavingEnable)
		return;

	if (pUiSetting->powerSave == POWER_SAVE_OFF)
		return;

	powerSave_t0 = powerSave_t0 + BACKGROUND_TIMER_TICK;
	if (powerSave_t0 >= timeOutInMs) {
		printf("#########powerSaveTimeReach#########\n");
		
        #if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		SetPowerBacklight(SP5K_GPIO_ACTIVE_LOW);
		appPowerSavingEnableSet(FALSE);
        #else
		appPowerOffUiMsgSend(APP_POWER_OFF_APO);
        #endif
		
		powerSave_t0 = 0;
	}
}

/*--------------------------------------------------------------------------*
* Function name    : _backgroundTimerServiceRoutine
* Function         : Use background timer to do your service routine.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void
_backgroundTimerServiceRoutine(
	void
)
{
    #if COMPEL_POWEROFF_CHARGE
	if(appGetBoot4ChargingFlag())
		_usbLowPwrChargeHandler();
	else
    #endif
	{
    #if !defined(HW_PDVR) && !defined(HW_17DVR)&& !defined(HW_19DVR)
    	appBatteryLevelPolling();
    #endif
    	_powerSavePolling();
    	appLedFlickerServiceRoutine();

#if defined(PRJ_JETDVR)||defined(PRJ_17DVR)||defined(PRJ_19DVR)
        appInfoTimerPolling();
        AppBuzzerFilckService();
        appLedGRInTurn();
#endif

        #if USB_SUPPORT && !SP5K_USB_UVC_SUPPORT
    	appUsbDetect();
        #endif

        #if SP5K_USB_UVC_SUPPORT
        appUvcEnumExtensionUnit();
        #endif

        #if USB_DISK_SUPPORT
        appUsbHostDetect();
        #endif
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appBackgroundTimerInit
* Function         : Init Background Timer Service.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void
appBackgroundTimerInit(
	void
)
{
	UINT32 timrHdlr;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	static UINT32 powerHdlr = TIMER_NULL;
    UINT32 err;
    SP5K_THREAD *ptrdInfoHandler;
#endif
	timrHdlr = appTimerIsrReg(_backgroundTimerServiceRoutine, (BACKGROUND_TIMER_TICK/appTimerIsrTickIntervalGet()));
	if (timrHdlr == TIMER_NULL)
	{
        HOST_PROF_LOG_ADD(LEVEL_ERROR, "bktime: background timer register failed");
	}
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

    err = sp5kOsEventFlagsCreate(&powerTimer_sig, "GsensorEvent");
    HOST_ASSERT(err == SP5K_SUCCESS);

    ptrdInfoHandler = sp5kOsThreadCreate("Power Timer Thread",
            appPowerTimerThread, 0,
            APP_POWER_TIMER_THREAD_PRI,
            APP_POWER_TIMER_THREAD_PRI, 0,
            TX_AUTO_START);
    HOST_ASSERT(ptrdInfoHandler);
    if (powerHdlr == TIMER_NULL)
        powerHdlr = appTimerIsrReg(JetPowerWatch,
                (POWER_TIMER_TICK / appTimerIsrTickIntervalGet()));
    if (powerHdlr == TIMER_NULL) {
        HOST_PROF_LOG_ADD(LEVEL_ERROR,
                "bktime: power handler timer register failed");
    }
    /*if Parking mode finish and over recording time 2 min to do power off */
    if (appInfoPowerDETStateGet() == POWER_DET_PARKING_WAKEUP) {
        parkrecord_time = appInfoParkingReCordingTimeGet() * 1000;
        printf("%s - parking mode timeout %d\n", __func__, parkrecord_time);
    }
#endif
}

/*--------------------------------------------------------------------------*
* Function name    : appBackgroundTimerTickIntervalGet
* Function         : Get interval which is backgroundtimer Tick.
* Return value	   : BACKGROUND_TIMER_TICK (UINT32)
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

UINT32
appBackgroundTimerTickIntervalGet(
	void
)
{
	return BACKGROUND_TIMER_TICK;
}

/*--------------------------------------------------------------------------*
* Function name    : appPowerSaveTimeOutReset
* Function         : Reset power saving time out.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void
appPowerSaveTimeOutReset(
	void
)
{
	powerSave_t0 = 0;
	isAutoPowerOffCountDown = 0;
}

/*--------------------------------------------------------------------------*
* Function name    : appPowerSavingEnableSet
* Function         : Set power saving.
* Return value	   : void
* Parameter1	   : enable(BOOL ENABLE/DISABLE)
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void
appPowerSavingEnableSet(
	BOOL enable
)
{
	isPowerSavingEnable = enable;
	if(enable)
	{
		appPowerSaveTimeOutReset();
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appAutoPowerOffPhaseCheck
* Function         : Phase check for auto power off.
* Return value	   : isAutoPowerOffCountDown (UINT32)
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

UINT32
appAutoPowerOffPhaseCheck(
	void
)
{
	return isAutoPowerOffCountDown;
}

/*--------------------------------------------------------------------------*
* Function name    : _usbLowPwrChargeHandler
* Function         : Handle usb that low power charge.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.05.24  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void
_usbLowPwrChargeHandler(
	void
)
{
	static UINT8 fBatChargeInit = 0;
	UINT32 pwrVal;

	if(!fBatChargeInit){
		appBatteryCharge_Init();
		fBatChargeInit = 1;
	}
	if(!IS_USB_IN)/* replace IS_USB_IN with sp5kUsbDetectStateGet() temporarily */
		appPsuedoPowerOff(TRUE);

	if(!appBatteryCharge_ChargerStatusGet()){
		if(appBatteryAdcValueGet() > LIMIT_BATTERY_ADC){ /* have battery ? */
			appBatteryCharge_SetStartCBF();
			RED_LED_ON;
		}
		else
			RED_LED_OFF;
	}
	else{
		count++;

		if(count > BATTERY_CHECK_COUNT)
		{
		    if(appBatteryCharge_SetCheckStatusCBF() == BATTERY_STATUS_FULL)
			{
                HOST_PROF_LOG_ADD(LEVEL_INFO, "bktime: BATTERY_STATUS_FULL");
	        	appBatteryCharge_SetStopCBF();
				RED_LED_OFF;
				appPsuedoPowerOff(TRUE);
			}
	        count = 0;
	    }
	}

	sp5kGpioRead(SP5K_GPIO_GRP_POCU,BIT0,&pwrVal);

	if(pwrVal & BIT0){
		do{
			sp5kGpioRead(SP5K_GPIO_GRP_POCU,BIT0,&pwrVal);
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 20);
		}while(pwrVal & BIT0);
		appPsuedoPowerOff(TRUE);
	}

}


