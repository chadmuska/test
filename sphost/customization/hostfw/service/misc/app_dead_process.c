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
 *	File name    : App_dead_process.c
 *	Function     : About dead process
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.05  Edward.Lu  "Add description and modify code"
 **************************************************************************/
#include "app_com_def.h"
#include "app_com_api.h"
#include "sp5k_fs_api.h"
#include "customization/dev_init.h"
#include "gpio_custom.h"
#include "sp5k_disk_api.h"
#include "sp5k_utility_api.h"
#include "app_dead_process.h"
#include "app_key_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define DEBUG_PRINT_POWER_KEY_NORMAL                    0           /* 0:disable , 1:enable */
/*#define APP_SYS_POWER_OFF_LOOP_COUNT                  1500*/     /* 1ms */
#define	APP_SYS_POWER_FORCE_OFF_LOOP_COUNT              1000    /* 1ms */
#define APP_SYS_POWER_FORCE_OFF_RESET_LOOP_COUNT        3000    /* 1ms */
#define	APP_SYS_HANG_POWER_FORCE_OFF_LOOP_COUNT         1000    /* 1ms */
#define	APP_SYS_HANG_POWER_FORCE_OFF_RESET_LOOP_COUNT   3000    /* 1ms */
#define	APP_SYS_HANG_TIME_OUT                           30       /* 1sec */

#define DEAD_GOTO_CMD_LINE		    0
#define DEAD_POWER_OFF			    1
#define DEAD_REBOOT				    2
#define HOST_DEAD_TIME_OUT_CTRL 	DEAD_GOTO_CMD_LINE  /*execute command after program dead timeout*/

#define POWER_KEY_PRESSED_INTERVAL_NORMAL       (300)       /* 300 ms */
#define POWER_KEY_PRESSED_INTERVAL_DEAD         (100)       /* 100 ms */

#define OPT_HOST_NORMAL     0
#define OPT_HOST_DEAD       -1

#define PROGRAM_DEAD_AUTOREBOOT 1
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

 /**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT32 PowerKeyStatusFirst = 1;  /* 0:release , 1: First PWR ON*/
static UINT32 powerKeyStatusLast = 0;   /* 0:release , 1: pressing*/
static UINT32 PowerKeyStatusNow = 0;    /* 0:release , 1: pressing*/
static UINT32 powerKeyPressCnt = 0;     /* ++ if power key press*/
static UINT32 deadKeyTimeOutCnt = 0;    /* ++ if power key no press, after program dead*/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appVideoLeave(void);
void appDeadDelayTime(UINT32 ms);
void appRtcReset(char opt);
void appForcePowerOffCb(void);
void appKeyPowerRelease(char opt, UINT32 nowCnt, UINT32 offCnt, UINT rstCnt);
void appKeyPowerPress(char opt, UINT32 nowCnt, UINT32 offCnt, UINT rstCnt);
BOOL keySkipDetect(void);    /*after program dead use, return 0:goto command line, 1:check Key continue*/
void keyPowerDetect(char opt);
void appPowerKeyNormalCb(void);
void appPowerKeyNormalInit(void);
void appHostDeadMunuCb(void);

/*--------------------------------------------------------------------------*
* Function name    : appVideoLeave
* Function         : Leave video record mode
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void appVideoLeave(void)
{
    UINT32 mode;

    sp5kModeGet(&mode);
    if(mode==SP5K_MODE_VIDEO_RECORD){
        sp5kModeSet(SP5K_MODE_VIDEO_PREVIEW);
        sp5kModeWait(SP5K_MODE_VIDEO_PREVIEW);
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appDeadDelayTime
* Function         : All os(thread) will delay ms time for dead use.
* Return value	   : void
* Parameter1	   : ms (UINT32)
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

void appDeadDelayTime(UINT32 ms)
{
    UINT32 delta=0,temp1=0;

    temp1 = appMsTimeGet();
    /*printf("%d %d %d\n",delta,temp1,temp2);*/

    while(delta<ms){
	UINT32 temp2=0;
        temp2 = appMsTimeGet();
        delta = temp2 - temp1;
    }
}


#if RTC_REBOOT

/*--------------------------------------------------------------------------*
* Function name    : appRTCWakeUp
* Function         : Set RTC wake up.
* Return value	   : void
* Parameter1	   : void
* History          :             chris      "author create function"
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

static void
appRTCWakeUp(
   void
)
{
#define RESET_SET_SEC 3
 	tmx_t rtc;
 	UINT32 r,reliableCode;
 	r = sp5kRtcReliableGet(&reliableCode);
 	if(r != SUCCESS)	return;
    sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION | SP5K_DATE_TIME_BLOCK_WAIT, &rtc);
    rtc.tmx_sec += RESET_SET_SEC;
 	if (rtc.tmx_sec >= 60) {
    		rtc.tmx_sec -= 60;
 		rtc.tmx_min++;
 		if (rtc.tmx_min >= 59) {
    			rtc.tmx_min -= 59;
 			rtc.tmx_hour++;
    		}
 	}
    sp5kRtcDateTimeSet(SP5K_WAKEUP_OPTION | SP5K_DATE_TIME_BLOCK_WAIT, &rtc);
}

/*--------------------------------------------------------------------------*
* Function name    : appHostOsDeadCallback
* Function         : Callback for Os dead happend.
* Return value	   : void
* Parameter1	   : void
* History          :             chris      "author create function"
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

static UINT32 bKeyASSERTPowerOff = 1;   /* 0:Reboot , 1:PowerOff */

static void
appHostOsDeadCallback(
   void
)
{
    if ( !bKeyASSERTPowerOff ) {
        /* use RTC wakeup to reboot */
        #if RTC_WAKEUP_SUPPORT
            appRTCWakeUp();
        #endif
    }
    /* Powser off */
    sp5kPowerCtrl(SP5K_POWER_OFF, (UINT32)-1);

}
#endif

/*--------------------------------------------------------------------------*
* Function name    : appRtcReset
* Function         : Use RTC reset. Set RTC wakeup time and power off.
* Return value	   : void
* Parameter1	   : void
* History          :             chris      "author create function"
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/	

/* Ref. from hal_rtc_init.c
Maybe printf:
1. [ERR rtc]timer read time out
2. [ERR rtc] reg write time out
3. [ERR rtc] reg read time out
*/
void appRtcReset(char opt)
{
    UINT32 retSet = FAIL, retGet = FAIL, appRtcTimeout=0;
    tmx_t rtc;

    if(opt==OPT_HOST_NORMAL){
        appVideoLeave();
    }

    while((retSet!=SUCCESS)||(retGet!=SUCCESS))
    {
        retGet = sp5kRtcDateTimeGet(SP5K_DATE_TIME_OPTION, &rtc); // get RTC wakeup time
        if(retGet!=SUCCESS){
            printf("[warning]%s %d %u %u %d\n",__FUNCTION__,__LINE__,appRtcTimeout,retGet,rtc.tmx_sec);
            profLogPrintf(4,"[warning]%s %d %d %d %d \n",__FUNCTION__,__LINE__,appRtcTimeout,retGet,rtc.tmx_sec);
        }

        rtc.tmx_sec+= 1;		/* no need to worry about overflow */

        retSet = sp5kRtcDateTimeSet(SP5K_WAKEUP_OPTION, &rtc); // RTC wakeup time set
        if(retSet!=SUCCESS){
            printf("[warning]%s %d %u %u %d \n",__FUNCTION__,__LINE__,appRtcTimeout,retSet,rtc.tmx_sec);
            profLogPrintf(4,"[warning]%s %d %d %d %d \n",__FUNCTION__,__LINE__,appRtcTimeout,retSet,rtc.tmx_sec);
        }

        if(appRtcTimeout>10) break;
        else    appRtcTimeout++;
    }
    appDeadDelayTime(100);

    if(opt==OPT_HOST_NORMAL){
        #if PWR_OFF_NOR
        SYS_PWR_OFF_NOR;
        #else
        SYS_PWR_OFF_DEAD;
        #endif
    }
    else{
        SYS_PWR_OFF_DEAD;
    }
    while(1);

}

/*--------------------------------------------------------------------------*
* Function name    : appForcePowerOffCb
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appForcePowerOffCb(
   void
)
{
#if DEAD_LOG
    profLogPrintf(0,"[PWRKEY.Cb]%s %d",__FUNCTION__,__LINE__);
    printf("[PWRKEY.Cb]%s %d\n",__FUNCTION__,__LINE__);
#endif
    SYS_PWR_OFF_NOR;
}

/*--------------------------------------------------------------------------*
* Function name    : appKeyPowerRelease
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appKeyPowerRelease(char opt, UINT32 nowCnt, UINT32 offCnt, UINT rstCnt)
{
	if(offCnt<rstCnt){

		if(nowCnt<offCnt){
#if DEAD_LOG		
			printf("do nothing...\n");
#endif			
		}
		else if((nowCnt>=offCnt)&&(nowCnt<rstCnt)){
#if DEAD_LOG
            profLogPrintf(0,"[key.PwrOff]%s %d",__FUNCTION__,__LINE__);
            printf("[key.PwrOff]%s %d\n",__FUNCTION__,__LINE__);
            printf("[key.PwrOff]0x%x %d %d %d\n",opt,nowCnt,offCnt,rstCnt);
#endif
            if(opt==OPT_HOST_NORMAL){
                /*force power off*/
                #if FORCE_POWER_OFF
                sp5kTimerIsrOneShotReg(0,3000,appForcePowerOffCb);
                #endif
				/* We just go normal power off flow, watch dog will call assert when wores case. */
                sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, APP_POWER_OFF_NORMAL);
            }
            else{
                SYS_PWR_OFF_DEAD;
            }
		}
		else{
#if DEAD_LOG		
            profLogPrintf(0,"[key.reboot]%s %d",__FUNCTION__,__LINE__);
            printf("[key.reboot]%s %d\n",__FUNCTION__,__LINE__);
            printf("[key.PwrOff]0x%x %d %d %d\n",opt,nowCnt,offCnt,rstCnt);
#endif			
			appRtcReset(opt);
		}
	}
    else
    {
    	if(nowCnt<rstCnt){
			printf("do nothing!\n");
		}
		else if((nowCnt>=rstCnt)&&(nowCnt<offCnt)){
#if DEAD_LOG	
            profLogPrintf(0,"[key.reboot]%s %d",__FUNCTION__,__LINE__);
            printf("[key.reboot]%s %d\n",__FUNCTION__,__LINE__);
            printf("[key.PwrOff]0x%x %d %d %d\n",opt,nowCnt,offCnt,rstCnt);
#endif
            appRtcReset(opt);
		}
		else{
#if DEAD_LOG		
            profLogPrintf(0,"[key.PwrOff]%s %d",__FUNCTION__,__LINE__);
            printf("[key.PwrOff]%s %d\n",__FUNCTION__,__LINE__);
            printf("[key.PwrOff]0x%x %d %d %d\n",opt,nowCnt,offCnt,rstCnt);
#endif
			/*force power off*/
            if(opt==OPT_HOST_NORMAL){
                sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, APP_POWER_OFF_NORMAL);
            }
            else{
                SYS_PWR_OFF_DEAD;
            }
		}
    }
}

/*--------------------------------------------------------------------------*
* Function name    : appKeyPowerPress
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appKeyPowerPress(char opt, UINT32 nowCnt, UINT32 offCnt, UINT rstCnt)
{
	if(offCnt<rstCnt){
        if(nowCnt>=rstCnt){
#if DEAD_LOG		
            profLogPrintf(0,"[key.press]%s %d",__FUNCTION__,__LINE__);
            printf("[key.press]%s %d\n",__FUNCTION__,__LINE__);
            printf("[key.press]0x%x %d %d %d\n",opt,nowCnt,offCnt,rstCnt);
#endif
			appRtcReset(opt);
		}
	}
    else
    {
        if(nowCnt>=offCnt){
#if DEAD_LOG
            profLogPrintf(0,"[key.press]%s %d",__FUNCTION__,__LINE__);
            printf("[key.press]%s %d\n",__FUNCTION__,__LINE__);
            printf("[key.press]0x%x %d %d %d\n",opt,nowCnt,offCnt,rstCnt);
#endif
			/*force power off*/
            if(opt==OPT_HOST_NORMAL){
                sp5kHostMsgSend(APP_UI_MSG_POWER_OFF, APP_POWER_OFF_NORMAL);
            }
            else{
                SYS_PWR_OFF_DEAD;
            }
		}
    }
}

/*--------------------------------------------------------------------------*
* Function name    : keySkipDetect
* Function         : after program dead use, return 
* Return value	   : result (BOOL , )
                     FALSE : Goto command line, you can get program dead information.
                     TRUE : Wait Key instruction.
* Parameter1	   : void
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

BOOL keySkipDetect(void)    /*after program dead use, return FALSE:goto command line, TRUE:check Key continue*/
{
    UINT32 isSkipKeyPress = 0;
    sp5kGpioRead(APP_KEY_SKIP_GRP,APP_KEY_SKIP_SRC,&isSkipKeyPress);
    /*printf("[skip]SNAP %d \n",isSkipKeyPress);*/
#if	defined(HW_SBC) || defined(HW_JETDVR) || defined(HW_PDVR)|| defined(HW_17DVR)||defined(HW_19DVR)
    if(isSkipKeyPress){
        printf("[skip]SNAP: goto command line! \n\n");   appDeadDelayTime(10);
        return FALSE;
    }
#else
    if(!isSkipKeyPress){
        printf("[skip]SNAP: goto command line! \n\n");   appDeadDelayTime(10);
        return FALSE;
    }
#endif
    return TRUE;
}

/*--------------------------------------------------------------------------*
* Function name    : osDeadTimeOut
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

/*
osDeadTimeOut() after program dead use,
return 0:   goto command line,
return 1:   continue to polling power key,
else :      execute command
*/
BOOL osDeadTimeOut(char deadFlowFlag)
{

#if DEBUG_PRINT_POWER_KEY_NORMAL
    /*profLogPrintf(0,"[key]%s cnt=%d",__FUNCTION__,deadKeyTimeOutCnt);*/
    printf("[key]%s cnt=%d\n",__FUNCTION__,deadKeyTimeOutCnt);
#endif

    if(deadKeyTimeOutCnt<APP_SYS_HANG_TIME_OUT)
        deadKeyTimeOutCnt++;
    else{
        switch(deadFlowFlag){
            case DEAD_GOTO_CMD_LINE:
                printf("[dead]time out %u sec & goto command line!\n\n",deadKeyTimeOutCnt);   appDeadDelayTime(10);
                return FALSE;
                break;
            case DEAD_POWER_OFF:
                printf("[dead]time out %u sec & force power off!\n\n",deadKeyTimeOutCnt);     appDeadDelayTime(10);
                SYS_PWR_OFF_DEAD;
                break;
            case DEAD_REBOOT:
                printf("[dead]time out %u sec & force reboot!\n\n",deadKeyTimeOutCnt);        appDeadDelayTime(10);
                SYS_REBOOT_DEAD;
                break;
			default:
				return FALSE;
        }

    }

    return TRUE;
}

/*--------------------------------------------------------------------------*
* Function name    : keyPowerDetect
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void keyPowerDetect(char opt)
{
#if DEAD_LOG
        profLogPrintf(0,"[key]%s %d s",__FUNCTION__,__LINE__);
        printf("[key]%s %d s\n",__FUNCTION__,__LINE__);
#endif

    sp5kGpioRead(APP_KEY_PWR_GRP,APP_KEY_PWR_SRC,&PowerKeyStatusNow);

    if (PowerKeyStatusNow){                                     /* key pressing */

        powerKeyPressCnt++;
        #if DEAD_LOG
        profLogPrintf(0,"[key.press]");
        printf("[key.press]\n");
        printf("[key.press]0x%x %d\n",opt,powerKeyPressCnt);
        #endif

        switch(opt){
            case OPT_HOST_NORMAL:
                appKeyPowerPress( opt,
                                    powerKeyPressCnt,
                                    APP_SYS_POWER_FORCE_OFF_LOOP_COUNT/POWER_KEY_PRESSED_INTERVAL_NORMAL,
                                    APP_SYS_POWER_FORCE_OFF_RESET_LOOP_COUNT/POWER_KEY_PRESSED_INTERVAL_NORMAL);
                break;
            case OPT_HOST_DEAD:
                appKeyPowerPress( opt,
                                    powerKeyPressCnt,
                                    APP_SYS_HANG_POWER_FORCE_OFF_LOOP_COUNT/POWER_KEY_PRESSED_INTERVAL_DEAD,
                                    APP_SYS_HANG_POWER_FORCE_OFF_RESET_LOOP_COUNT/POWER_KEY_PRESSED_INTERVAL_DEAD);
            	break;
			default:
				break;
        }

        if(opt==OPT_HOST_DEAD)
            deadKeyTimeOutCnt = 0;
    }
    else if((!PowerKeyStatusNow)&&(powerKeyStatusLast)){        /* key release */

        #if DEAD_LOG
        profLogPrintf(0,"[key.release]");
        printf("[key.release]\n");
        printf("[key.release]0x%x %d\n",opt,powerKeyPressCnt);
        #endif

        switch(opt){
            case OPT_HOST_NORMAL:
                    profLogPrintf(0,"[key.normal]pwr key cnt %dms",powerKeyPressCnt*POWER_KEY_PRESSED_INTERVAL_NORMAL);
                    printf("[key.normal]pwr key cnt %ums\n",powerKeyPressCnt*POWER_KEY_PRESSED_INTERVAL_NORMAL);

                    appKeyPowerRelease( opt,
                                    powerKeyPressCnt,
                                    APP_SYS_POWER_FORCE_OFF_LOOP_COUNT/POWER_KEY_PRESSED_INTERVAL_NORMAL,
                                    APP_SYS_POWER_FORCE_OFF_RESET_LOOP_COUNT/POWER_KEY_PRESSED_INTERVAL_NORMAL);
                break;

            case OPT_HOST_DEAD:
                    profLogPrintf(0,"[key.dead]pwr key cnt %dms",powerKeyPressCnt*POWER_KEY_PRESSED_INTERVAL_DEAD);
                    printf("[key.dead]pwr key cnt %ums\n",powerKeyPressCnt*POWER_KEY_PRESSED_INTERVAL_DEAD);

                    appKeyPowerRelease( opt,
                                    powerKeyPressCnt,
                                    APP_SYS_HANG_POWER_FORCE_OFF_LOOP_COUNT/POWER_KEY_PRESSED_INTERVAL_DEAD,
                                    APP_SYS_HANG_POWER_FORCE_OFF_RESET_LOOP_COUNT/POWER_KEY_PRESSED_INTERVAL_DEAD);
                break;

            default:
                powerKeyPressCnt    = 0;
                PowerKeyStatusNow   = 0;
                break;

        }
    }
    else{                                                       /* key idle */
        powerKeyPressCnt    = 0;
        PowerKeyStatusNow   = 0;
    }
    powerKeyStatusLast = PowerKeyStatusNow;

#if DEAD_LOG
    profLogPrintf(0,"[key]%s %d,cnt=%d",__FUNCTION__,__LINE__,powerKeyPressCnt);
    printf("[key]%s %d,cnt=%d\n",__FUNCTION__,__LINE__,powerKeyPressCnt);
#endif
}

/*--------------------------------------------------------------------------*
* Function name    : appPowerKeyNormalCb
* Function         : After POWER_KEY_PRESSED_INTERVAL_NORMAL, callback itself.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appPowerKeyNormalCb(
   void
)
{
#if DEAD_LOG
    profLogPrintf(0,"[PWRKEY.Cb]%s %d",__FUNCTION__,__LINE__);
    /*printf("[PWRKEY.Cb]%s %d\n",__FUNCTION__,__LINE__);*/
#endif

    if(PowerKeyStatusFirst)
	    sp5kGpioRead(APP_KEY_PWR_GRP,APP_KEY_PWR_SRC,&PowerKeyStatusFirst);
    else
	    keyPowerDetect(OPT_HOST_NORMAL);
	// Isr level timer callbackfunction register for once.
    sp5kTimerIsrOneShotReg(DISABLE,POWER_KEY_PRESSED_INTERVAL_NORMAL,appPowerKeyNormalCb);
}

/*--------------------------------------------------------------------------*
* Function name    : appPowerKeyNormalInit
* Function         : Init detect power key feature.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appPowerKeyNormalInit(
   void
)
{
    profLogPrintf(0,"[PWRKEY.int] init");
    printf("[PWRKEY.int] init\n");
    keyPowerDetect(OPT_HOST_NORMAL);
	// Isr level timer callbackfunction register for once.
	// the Host not to do too much thing inside the callback function, 
	// otherwise the performance of the system will be unexpected.
    sp5kTimerIsrOneShotReg(DISABLE,POWER_KEY_PRESSED_INTERVAL_NORMAL,appPowerKeyNormalCb);
}

#if APP_DEAD_MENU_ENABLE

/*--------------------------------------------------------------------------*
* Function name    : appHostDeadMunuCb
* Function         : If host occur crash(program dead),
                     this callbackfunction(cbfunc) will show crash(program dead) log.
                     This flow is for engineering, you can set reboot flow for common user.                     
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appHostDeadMunuCb(
   void
)
{
	    printf("\nPROGRAM DEAD:\n");
#if PROGRAM_DEAD_AUTOREBOOT

SYS_REBOOT_DEAD;

#else
    UINT32 appSampleLoop = 0;
    keyPowerDetect(OPT_HOST_DEAD);
    appDeadDelayTime(POWER_KEY_PRESSED_INTERVAL_DEAD);
    printf("1) Press <SNAP>: >1sec goto command line.\n");

    if(APP_SYS_HANG_POWER_FORCE_OFF_RESET_LOOP_COUNT>APP_SYS_HANG_POWER_FORCE_OFF_LOOP_COUNT)
    {
        printf("2) Press <POWER>: %d~%dms power off.\n3) Press <POWER>: >%dms reboot.\n",
            APP_SYS_HANG_POWER_FORCE_OFF_LOOP_COUNT,
            APP_SYS_HANG_POWER_FORCE_OFF_RESET_LOOP_COUNT,
            APP_SYS_HANG_POWER_FORCE_OFF_RESET_LOOP_COUNT
            );
    }
    else
    {
        printf("2) Press <POWER>: %d~%dms reboot.\n3) Press <POWER>: >%dms power off.\n",
            APP_SYS_HANG_POWER_FORCE_OFF_RESET_LOOP_COUNT,
            APP_SYS_HANG_POWER_FORCE_OFF_LOOP_COUNT,
            APP_SYS_HANG_POWER_FORCE_OFF_LOOP_COUNT
            );
    }

    switch(HOST_DEAD_TIME_OUT_CTRL){
        case DEAD_GOTO_CMD_LINE:
            printf("4) Goto command line, if idle >%dsec.\n\n",APP_SYS_HANG_TIME_OUT);
            break;
        case DEAD_POWER_OFF:
            printf("4) Force power off, if idle >%dsec.\n\n",APP_SYS_HANG_TIME_OUT);
            break;
        case DEAD_REBOOT:
            printf("4) Force system reboot, if idle(osDeadTimeOut) >%d sec.\n\n",APP_SYS_HANG_TIME_OUT);
            break;
        default:
            break;
        }

    while(keySkipDetect() && osDeadTimeOut(HOST_DEAD_TIME_OUT_CTRL))
    {
        for(appSampleLoop=0; appSampleLoop<(1000/POWER_KEY_PRESSED_INTERVAL_DEAD); appSampleLoop++)
        {
            keyPowerDetect(OPT_HOST_DEAD);
            appDeadDelayTime(POWER_KEY_PRESSED_INTERVAL_DEAD);
        }
    }
#endif
}

#else

/*--------------------------------------------------------------------------*
* Function name    : appHostDeadDump
* Function         : Call videostate log to understand video situation.
* Return value	   : void
* Parameter1	   : void
* History          :
*                  : 2018.06.05  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appHostDeadDump(
	void
)
{
	// Run UART command in caller thread instead of CMD thread.
	sp5kCmdExec("videostate");
}
#endif

