/**************************************************************************
 *
 *       Copyright (c) 2002-2018 by iCatch Technology, Inc.
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
 *  File Description:
 *
 *  Author: Chris Wang
 *
 ******************************************************************************/
/**
 * @file app_sd_log.c
 * @brief Logging error message to storage
 * @author chris
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "common.h"
#include "sp5k_msg_def.h"
#include "sp5k_disk_api.h"
#include "sp5k_os_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_global_api.h"
#include "sp5k_utility_api.h"
#include "app_timer.h"
#include "cmd.h"
#include "debug_mod/dbg_api.h"
#include "middleware/proflog.h"
#include "gpio_custom.h"
#include "../include/app_watch_dog.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define WATCHDOG_INTERVAL_ISR       (1000)      /* unit: 1 ms */
#define WATCHDOG_INTERVAL_THD       (500)       /* unit: ms */
#define WDG_ISR_ONESHOT				1

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static SP5K_THREAD *thdWatchDogStatus = 0;     /* 0:never create or FAIL, 1: create success*/
static UINT32 thdWatchDogTick = 0 ;          /* WDT thread get timer tick, unit is 10 ms */
UINT32 watchDog_enable = WDG_ENABLE;
UINT32 watchDog_timeout_setting = WATCHDOG_TIMEOUT_ASSERT;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern void ros_thread_sleep(UINT32 ms);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
extern void appInfHWWDT(UINT32 ms);
#endif

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appWatchDogRefresh(void){
    thdWatchDogTick = sp5kOsTimeGet();
}
void appWatchDogModify(UINT32 val)  /*TICK_UNIT=10ms*/
{
	appWatchDogRefresh();

    if(val==0){
        watchDog_enable = WDG_DISABLE;
    }
    else{
        watchDog_enable = WDG_ENABLE;
        watchDog_timeout_setting = val;
    }
}

void appWatchDogRollBack(void)
{
    watchDog_enable = WDG_ENABLE;
    watchDog_timeout_setting = WATCHDOG_TIMEOUT_ASSERT;
}

static void
appWatchDogISRCb(
   void
)
{
    UINT32 ret;

#if DEBUG_PRINT_SOFT_WDG
    UINT32 aaa,bbb;
    aaa = sp5kOsTimeGet();
    bbb = thdWatchDogTick;
    printf("[WDT.ISR] ISR:%d\tThd:%d\tDelta:%d ms\n",aaa*TICK_UNIT,bbb*TICK_UNIT,(aaa-bbb)*TICK_UNIT);
    if (( aaa - bbb ) > watchDog_timeout_setting ) {
#else
    if (( sp5kOsTimeGet() - thdWatchDogTick ) > watchDog_timeout_setting ) {
#endif

        if(watchDog_enable == WDG_ENABLE){
            printf("\n!!!!!!!!!!!! host hang up > %d , soft watchdog timeout !!!!!!!!!!!!\n",watchDog_timeout_setting*TICK_UNIT);
            ASSERT(0);
    		/* Never executed here below */
        }
        else
        {
            printf("\n!!!!!!!!!!!! host hang up > %d , soft watchdog timeout !!!!!!!!!!!!\n",watchDog_timeout_setting*TICK_UNIT);
            appWatchDogRefresh();
        }
	}

    #if WDG_ISR_ONESHOT
    ret = sp5kTimerIsrOneShotReg(0,WATCHDOG_INTERVAL_ISR,appWatchDogISRCb);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
    appInfHWWDT(500);
#endif
    #else
    ret = sp5kTimerUsIsrReg(SP5K_TIMER_US_ISR_ID_1 , 1, WATCHDOG_INTERVAL_ISR*1000, appWatchDogISRCb);
    #endif

    if ( ret == FAIL ) {
        printf("[WDT.ISR] sp5kTimerIsrReg FAIL\n");
        profLogAdd(0,"[WDT.ISR] sp5kTimerIsrReg FAIL\n");
        return;
    }

    #if DEBUG_PRINT_SOFT_WDG
    printf("[WDT.ISR] sp5kTimerIsrReg SUCCESS\n");
    #endif
}

/**
 * @brief host app watchdog thread.
 * @param[in] timeval thread sleep time interval
 * @author chris
 */
static void
appSoftWatchDogThread(
   UINT32 timeval
)
{
	UINT32 t = timeval / 1 ;

	while (1) {
#if 0
        /*printf("[WDT.Thd] %s %d,call thread\n",__FUNCTION__,__LINE__);*/
        printf("[WDT.Thd] call thread\n");
#endif

		#if 0
			static SP5K_MUTEX muxWDGTest;
            /* unit test */
		    sp5kOsMutexGet(&muxWDGTest, TX_WAIT_FOREVER);
		#endif

        thdWatchDogTick = sp5kOsTimeGet();

		#if 0
            /* unit test */
		    sp5kOsMutexPut(&muxWDGTest);
		#endif

        ros_thread_sleep(t);
	}
}

void appWatchDogInit_HW(void){
#if 0
sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_IMMEDIATE_POWEROFF, 1); /*power off immediately*/
#endif
#if 0
sp5kSystemCfgSet(SP5K_SYS_HALT_OPERAT_CFG, SP5K_SYS_HALT_IMMEDIATE_REBOOT, 1); /*reboot immediately*/
#endif
}

/*
Software watch dog use a ISR(highest priority) and a thread (lowest priority) to minitor host status.
It will cause ASSERT(0) if host hang up.
*/
void appWatchDogInit_SW(void){
    if(watchDog_enable == WDG_ENABLE){

        printf("[WDT.int] init\n");
        #if DEBUG_PRINT_SOFT_WDG
            profLogPrintf(0,"[WDT.int] init");
        #endif

        //UINT32 ret;
        appWatchDogRefresh();

        /* Create timer thread for software watchwod */
        if (thdWatchDogStatus == 0){
			UINT32 ret;

            /* use one shot callback */
            /* must use one shot callback to avoide cause multi fault , after RTC reboot when program dead */
#if WDG_ISR_ONESHOT
    		ret = sp5kTimerIsrOneShotReg(0,WATCHDOG_INTERVAL_ISR,appWatchDogISRCb);
#endif
        	if (ret == FAIL){
        		printf("[WDT.int] Watch dog ISR Reg. FAIL\n");
                profLogAdd(0,"[WDT.int] Watch dog ISR Reg. FAIL\n");
        		return;
        	}

            #if DEBUG_PRINT_SOFT_WDG
            printf("[WDT.int] Watch dog ISR Reg. SUCCESS\n");
            #endif

            thdWatchDogStatus = sp5kOsThreadCreate("appWatchDogThread", appSoftWatchDogThread,
                                                     WATCHDOG_INTERVAL_THD , TX_PRIORITY_31, TX_PRIORITY_31,
                                                     0, TX_AUTO_START );

            DBG_ASSERT(thdWatchDogStatus);
            if (!thdWatchDogStatus){
                printf("[WDT.int] Watch dog thread create FAIL\n");
                profLogAdd(0,"[WDT.int] Watch dog thread create FAIL\n");
                return;
            }
            else
            {
                #if DEBUG_PRINT_SOFT_WDG
                printf("[WDT.int] Watch dog thread create SUCCESS\n");
                #endif
            }
        }
    }
    return;
}

