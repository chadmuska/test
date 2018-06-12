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
#ifndef APP_LED_H
#define APP_LED_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 typedef enum {
	APP_LED_POWER  = 0,  
	APP_LED_SELFTIMER,         
	APP_LED_TOTAL,
} appLedOption_e;


/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appLedOn(appLedOption_e led);
void appLedOff(appLedOption_e led);
void appLedFlickerSet(appLedOption_e led,UINT32 flickerMs);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
void appLedGRInTurnEnable( UINT32 ms  , UINT32 times);
void appLedGRInTurnDisable(void);
void appLedGRInTurn( void );
#endif
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#define GREEN_LED_ON                    appLedOn(APP_LED_POWER);printf("paul GREEN on\n");
#define GREEN_LED_OFF                   appLedOff(APP_LED_POWER);printf("paul GREEN off\n");
#define RED_LED_ON                      appLedOn(APP_LED_SELFTIMER);printf("paul RED on\n");
#define RED_LED_OFF                     appLedOff(APP_LED_SELFTIMER);printf("paul RED off\n");

#define GREEN_LED_FLICKER(a)            appLedFlickerSet(APP_LED_POWER,a);
#define RED_LED_FLICKER(a)              appLedFlickerSet(APP_LED_SELFTIMER,a);

#define ALL_LED_ON                      GREEN_LED_ON;RED_LED_ON;
#define ALL_LED_OFF                     GREEN_LED_OFF;RED_LED_OFF;

/*LED rules*/
#define LED_NORMAL_ON				    GREEN_LED_ON;RED_LED_OFF;printf("paul NORMAL_ON\n");
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
#define LED_FLICKER_POWER_ON 		    GREEN_LED_FLICKER(500)
#else
#define LED_FLICKER_POWER_ON 		    GREEN_LED_FLICKER(100)
#endif
#define LED_FLICKER_LCD_OFF		        GREEN_LED_FLICKER(200)
#define LED_FLICKER_FOMATE			    GREEN_LED_FLICKER(100)
#define LED_FLICKER_STROBE_CHARGE 	    GREEN_LED_FLICKER(100)
#define LED_FLICKER_AF_ERROR		    GREEN_LED_FLICKER(100)
#define LED_FLICKER_IMAGE_SAVING	    GREEN_LED_FLICKER(100)
#define LED_FLICKER_VIDEO_RECORDING	    GREEN_LED_FLICKER(200)
#define LED_FLICKER_AUDIO_RECORDING	    GREEN_LED_FLICKER(200)
#define LED_FLICKER_DELETING		    GREEN_LED_FLICKER(200)
#define LED_FLICKER_SLIDESHOW_READING	GREEN_LED_FLICKER(100)
#define LED_FLICKER_COPY2CARD_COPYING	GREEN_LED_FLICKER(100)
#define LED_FLICKER_USB_TRANSFERRING	GREEN_LED_FLICKER(40)
#define LED_FLICKER_VOICE_MEMO_RECORDING	GREEN_LED_FLICKER(200)
#define LED_FLICKER_POWER_OFF 		    GREEN_LED_FLICKER(100)

#endif  /* APP_LED_H*/

