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

#ifndef __APP_WATCH_DOG_H__
#define __APP_WATCH_DOG_H__

#define WATCHDOG_TIMEOUT_ASSERT     (500)       /* unit: 10 ms */
                                                /* TIMEOUT_ASSERT must more than (INTERVAL_ISR - INTERVAL_THD) */
#define TICK_UNIT                   (10)        /* get tick unit is 10 ms */
#define DEBUG_PRINT_SOFT_WDG        0           /* DBG 0:disable , 1:enable */
#define WDG_DISABLE                 0
#define WDG_ENABLE                  1

extern UINT32 watchDog_enable;
extern UINT32 watchDog_timeout_setting;

void appWatchDogModify(UINT32 val);  			/*TICK_UNIT=10ms*/
void appWatchDogRollBack(void);
void appWatchDogInit_SW(void);
void appWatchDogRefresh(void);


#endif

