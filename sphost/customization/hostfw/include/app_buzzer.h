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
#ifndef APP_BUZZER_H
#define APP_BUZZER_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "app_com_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
extern UINT32 buzzerTimer ;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define BEEP_KEY_PRESS(ms)     {AppBuzzerOn(180,80);sp5kTimerIsrReg( 0, ms, AppBuzzerCB);}
#else
#define BEEP_KEY_PRESS     {AppBuzzerOn(180,80);sp5kTimerIsrReg( 0, 180, AppBuzzerCB);}
#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

void AppBuzzerOn(UINT32 freq,UINT32 duty);
void AppBuzzerOff(void);
void AppBuzzerCB(void);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
void AppBuzzerFlickOFF( void );
void AppBuzzerFlick( UINT32 ms  , UINT32 times);
#endif
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#endif  /* APP_BUZZER_H*/

