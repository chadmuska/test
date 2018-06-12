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
#include "app_timer.h"
#include "gpio_custom.h"
#include "app_led.h"

 typedef enum {
	LED_STATUS_OFF  = 0,  
	LED_STATUS_ON, 
} ledStatus_e;

typedef struct ledFlickInfo_s {
	UINT8 enable;
	UINT8 ledStatus;
	UINT16 flickerMs; 
	UINT32 timeCount;	
} ledFlickInfo_t;

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
UINT32 GRInTurnTimer = 0;
UINT32 GRInTurnbackGroundTimer = 0;
UINT32 GRInTurnCount = 0;
BOOL GRinTurn = FALSE;
#endif
static ledFlickInfo_t ledFlicker[APP_LED_TOTAL] = { {FALSE, LED_STATUS_OFF, 0 , 0},{FALSE,LED_STATUS_OFF, 0, 0} };

void
_ledOn(
	appLedOption_e led
)
{
	if(led == APP_LED_POWER) {
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		LED_MAIN_ON;
#else
        SET_LED_AF(1);
#endif
	}
	else if(led == APP_LED_SELFTIMER) {
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		LED_STIMER_ON;
#else
	    SET_LED_SELFTIMER(1);
#endif
	}
}

void
_ledOff(
	appLedOption_e led
)
{
    if(led == APP_LED_POWER) {
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		LED_MAIN_OFF;
#else
        SET_LED_AF(0);
#endif
    }
    else if(led == APP_LED_SELFTIMER) {
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		LED_STIMER_OFF;
#else
        SET_LED_SELFTIMER(0);
#endif
    }

}

void
_ledFlickerReset(
	appLedOption_e led
)	
{	
	ledFlicker[led].enable = FALSE;
	ledFlicker[led].ledStatus = LED_STATUS_OFF;
	ledFlicker[led].timeCount = 0;
	ledFlicker[led].flickerMs = 0;
}


void
appLedOn(
	appLedOption_e led
)
{
	_ledOn(led);
	_ledFlickerReset(led);
	ledFlicker[led].ledStatus = LED_STATUS_ON;
}

void
appLedOff(
	appLedOption_e led
)
{
	_ledOff(led);
	_ledFlickerReset(led);	
}

void
appLedFlickerSet(
	appLedOption_e led,
	UINT32 flickerMs
)	
{
	if (flickerMs == 0) {
		_ledOff(led);
	}
	else {
		ledFlicker[led].enable = TRUE;	
		ledFlicker[led].ledStatus = LED_STATUS_OFF;
		ledFlicker[led].timeCount = 0;
	}
	ledFlicker[led].flickerMs = flickerMs;
}

void 
appLedFlickerServiceRoutine(
	void
)
{
	UINT32 i;

	for(i = 0; i < APP_LED_TOTAL; i++ ) {
		if (ledFlicker[i].enable && ledFlicker[i].flickerMs) {
			ledFlicker[i].timeCount = ledFlicker[i].timeCount + appBackgroundTimerTickIntervalGet();
			if (ledFlicker[i].timeCount  >= ledFlicker[i].flickerMs) {				
				if (ledFlicker[i].ledStatus == LED_STATUS_ON) {
					_ledOff(i);
					ledFlicker[i].ledStatus = LED_STATUS_OFF;
					
				}
				else {
					_ledOn(i);
					ledFlicker[i].ledStatus = LED_STATUS_ON;
				}
				ledFlicker[i].timeCount = 0;
			}
		}
	}
}

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
void appLedGRInTurnEnable( UINT32 ms  , UINT32 times)
{
	LED_MAIN_ON;
	LED_STIMER_OFF;
	GRinTurn = TRUE;
	GRInTurnbackGroundTimer = ms;
	GRInTurnTimer = ms;
	if(times < (0xffffffff/2))
		GRInTurnCount = times*2-1;
}

void appLedGRInTurnDisable(void)
{
	  LED_MAIN_OFF;
	  LED_STIMER_OFF;
	  GRinTurn = FALSE;
	 GRInTurnbackGroundTimer = GRInTurnTimer = 0;
	 GRInTurnCount = 0;
}
/*paul 2018.04.16 remove unused variable*/
void appLedGRInTurn( void )
{


	
	if(GRInTurnbackGroundTimer > 0)
	{
		GRInTurnbackGroundTimer = GRInTurnbackGroundTimer -appBackgroundTimerTickIntervalGet();
		
		if(GRInTurnbackGroundTimer  == 0 )
		{
			
			if(GRInTurnCount  > 0)
			{
				GRInTurnCount--;
				GRInTurnbackGroundTimer = GRInTurnTimer;
				if(GRinTurn)
				{
					GRinTurn = FALSE;
					LED_MAIN_OFF;
					LED_STIMER_ON;
					return;
				}

				if(GRinTurn == FALSE)
				{
					GRinTurn = TRUE;
					LED_STIMER_OFF;
					LED_MAIN_ON;
					return;
				}	
			}
			else
				appLedGRInTurnDisable();
		}
	}

}
#endif