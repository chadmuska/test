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
 /*--------------------------------------------------------------------------*
 *  File name	: 	App_buzzer.c
 *  Function 	: 	Buzzer process	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  			:2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/


#define HOST_DBG 0
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "sp5k_global_api.h"
#include "app_buzzer.h"
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#include "app_timer.h"
#endif
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define BUZZER_PWM_CTRL	0

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
UINT32 buzzerBeep =0;
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
UINT32 buzzerTimer =0;
SINT32 buzzerbackGroundTimer =0;
UINT32 buzzerCount = 0;
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

  /*--------------------------------------------------------------------------*
 * Function name	: AppBuzzerOn
 * Function	       	: Set buzzer to turn on sequence.
 * Return value   	: void
 * Parameter1    	: PWM frequence (UINT32)
 * Parameter2    	: PWM duty cycle (UINT32) 
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
void AppBuzzerOn(
	UINT32 freq,UINT32 duty
)
{
	if (buzzerBeep ==FALSE)
	{
		sp5kPwmCfgSet(BUZZER_PWM_CTRL,freq,duty);
		sp5kPwmTrig( 1 << BUZZER_PWM_CTRL, 0 );
		buzzerBeep =TRUE;
	}
}

  /*--------------------------------------------------------------------------*
 * Function name	: AppBuzzerOff
 * Function	       	: Set buzzer to turn off sequence.
 * Return value   	: void
 * Parameter1    	: void
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
void AppBuzzerOff(
	void
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	if(buzzerBeep  == TRUE)
		sp5kPwmTrig( 0, 1 << BUZZER_PWM_CTRL );
	buzzerBeep =FALSE;
#else
	sp5kPwmTrig( 0, 1 << BUZZER_PWM_CTRL );
#endif
}

  /*--------------------------------------------------------------------------*
 * Function name	: AppBuzzerCB
 * Function	       	: Function to set buzzer off and buzzerbeep flag to false
 * Return value   	: void
 * Parameter1    	: void
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
void AppBuzzerCB(
	void
)
{
	AppBuzzerOff();
	buzzerBeep =FALSE;
}

#if defined(PRJ_17DVR)||defined(PRJ_19DVR)

  /*--------------------------------------------------------------------------*
 * Function name	: AppBuzzerFlick
 * Function	       	: Set time period for buzzer on
 * Return value   	: void
 * Parameter1    	: buzzer beep time (UINT32 millisecond)
 * Parameter2    	: Number of executions (UINT32)
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
void AppBuzzerFlick( UINT32 ms  , UINT32 times)
{
	buzzerbackGroundTimer =buzzerTimer = ms;
	buzzerCount = times*2-1;
	AppBuzzerOn(180,80);
	
}

  /*--------------------------------------------------------------------------*
 * Function name	: AppBuzzerFlickOFF
 * Function	       	: Clean time period for buzzer off 
 * Return value   	: void
 * Parameter1    	: void
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
void AppBuzzerFlickOFF( void )
{
	buzzerTimer = 0;
	buzzerCount = 0;
}

  /*--------------------------------------------------------------------------*
 * Function name	: AppBuzzerFilckService
 * Function	       	: Buzzer control by timer count
 * Return value   	: void
 * Parameter1    	: void
 * History	   :  
 *                :  2018.04.12  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/
 
void AppBuzzerFilckService(void)
{
	if(buzzerbackGroundTimer > 0 )
	{
		buzzerbackGroundTimer = buzzerbackGroundTimer -appBackgroundTimerTickIntervalGet();
		

		if(buzzerbackGroundTimer <= 0 )
		{

			if( buzzerCount > 0  )
			{
				if(buzzerBeep == FALSE)
					AppBuzzerOn(180,80);
				else
					AppBuzzerOff();
				buzzerbackGroundTimer = buzzerTimer ;
				buzzerCount--;
			}
			else
			{

				if(buzzerBeep == TRUE)
					AppBuzzerOff();
									
				buzzerbackGroundTimer = 0;
				buzzerCount = 0;
				
			}
		}

	}
	
}


  /*--------------------------------------------------------------------------*
 * Function name	: appBuzzerReady
 * Function	       	: Buzzer ready yes or no
 * Return value   	: result (BOOL true/false)
 *			             TRUE : Buzzer is ready to start next event 
 				     FALSE : Buzzer is working right now
 * Parameter1    	: void
 * History	   :  2017.08.08  Isaac Fong	  "First edit"
 *                :  2018.04.12  Paul.Chang     "modify description"
 *--------------------------------------------------------------------------*/
 
 BOOL appBuzzerReady(void){
	if(buzzerCount == 0 )
		return TRUE;
	else
		return FALSE;
}

  /*--------------------------------------------------------------------------*
 * Function name	: appBuzzerReadyWaitTime
 * Function	       	: Buzzer need how much time to finish job
 * Return value   	: Buzeer need how much time(ms) ready (unsigned int)  
 * Parameter1    	: void
 * History	   :  2017.08.08  Isaac Fong	  "First edit"
 *                :  2018.04.12  Paul.Chang     "modify description"
 *--------------------------------------------------------------------------*/
 
 UINT32 appBuzzerReadyWaitTime(void){
	UINT32 tempMs = 0;

	tempMs = buzzerTimer*buzzerCount + buzzerbackGroundTimer;
	if(tempMs == 0xFFFFFFFF)
		tempMs = 0;

	return tempMs;
}
#endif
