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
#ifndef _APP_KEY_DEF_H_
#define _APP_KEY_DEF_H_

#include "api/sp5k_msg_def.h"
#include "api/sp5k_disk_api.h"
#include "app_key_msg.h"
#include "app_btn_def.h"
#include "gpio_custom.h"
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

typedef enum {
	SP5K_MSGX_KEY		= SP5K_MSG_HEAD_CUSTOMER,	  		  /* 0xF000-0000 */
	SP5K_MSGX_STATE	    = SP5K_MSG_HEAD_CUSTOMER + 0xA000000, /* 0xFA00-0000 */
	SP5K_MSGX_UI		= SP5K_MSG_HEAD_CUSTOMER + 0xB000000, /* 0xFB00-0000 */
} appMsgGroup_e;

#define APP_KEY_MSG			(SP5K_MSGX_KEY|0x01000000)


/* app key msg define (including TV/USB/CARD) */
typedef enum {

	APP_KEY_PRESS_TELE      = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_PRESS<<4)	|BTN_ID_TELE), /* 0xF100-1000 */
	APP_KEY_RELEASE_TELE    = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_RELEASE<<4)|BTN_ID_TELE), /* 0xF100-1020 */

	APP_KEY_PRESS_WIDE      = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_PRESS<<4)	|BTN_ID_WIDE), /* 0xF100-1001 */
	APP_KEY_RELEASE_WIDE    = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_RELEASE<<4)|BTN_ID_WIDE), /* 0xF100-1021 */

	APP_KEY_PRESS_S1        = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_PRESS<<4)	|BTN_ID_S1),   /* 0xF100-1002 */
	APP_KEY_RELEASE_S1      = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_RELEASE<<4)|BTN_ID_S1),   /* 0xF100-1022 */
	APP_KEY_REPEAT_S1      = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_REPEAT<<4)|BTN_ID_S1),

	APP_KEY_PRESS_S2        = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_PRESS<<4)	|BTN_ID_S2),   /* 0xF100-1003 */
	APP_KEY_RELEASE_S2      = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_RELEASE<<4)|BTN_ID_S2),   /* 0xF100-1023 */
	APP_KEY_REPEAT_S2      = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_REPEAT<<4)|BTN_ID_S2),

	APP_KEY_TV_IN        	= (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_PRESS<<4)	|BTN_ID_TV),   /* 0xF100-1004 */
	APP_KEY_TV_OUT      	= (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_RELEASE<<4)|BTN_ID_TV),   /* 0xF100-1024 */

	APP_KEY_PRESS_PB        = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_PRESS<<4)	|BTN_ID_PB),   /* 0xF100-1005 */
	APP_KEY_RELEASE_PB      = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_RELEASE<<4)|BTN_ID_PB),   /* 0xF100-1025 */

	APP_KEY_PRESS_POWER     = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_PRESS<<4)	|BTN_ID_POWER),/* 0xF100-1006 */
	APP_KEY_RELEASE_POWER   = (APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_RELEASE<<4)|BTN_ID_POWER),/* 0xF100-1026 */

	APP_KEY_PRESS_UP        = (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4)	|BTN_ID_UP),   /* 0xF100-1030 */
	APP_KEY_REPEAT_UP		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_REPEAT<<4)	|BTN_ID_UP),   /* 0xF100-1040 */
	APP_KEY_RELEASE_UP      = (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4)	|BTN_ID_UP),   /* 0xF100-1050 */

	APP_KEY_PRESS_DOWN		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4)	|BTN_ID_DOWN), /* 0xF100-1031 */
	APP_KEY_REPEAT_DOWN		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_REPEAT<<4)	|BTN_ID_DOWN), /* 0xF100-1041 */
	APP_KEY_RELEASE_DOWN    = (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4)	|BTN_ID_DOWN), /* 0xF100-1051 */

	APP_KEY_PRESS_LEFT		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4)	|BTN_ID_LEFT), /* 0xF100-1032 */
	APP_KEY_RELEASE_LEFT    = (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4)	|BTN_ID_LEFT), /* 0xF100-1052 */

	APP_KEY_PRESS_RIGHT		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4)	|BTN_ID_RIGHT),/* 0xF100-1033 */
	APP_KEY_RELEASE_RIGHT   = (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4)	|BTN_ID_RIGHT),/* 0xF100-1053 */

	APP_KEY_PRESS_SET		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4)	|BTN_ID_SET),  /* 0xF100-1034 */
	APP_KEY_RELEASE_SET   	= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4)	|BTN_ID_SET),  /* 0xF100-1054 */

	APP_KEY_PRESS_DEL		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4)	|BTN_ID_DEL),  /* 0xF100-1035 */
	APP_KEY_RELEASE_DEL   	= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4)	|BTN_ID_DEL),  /* 0xF100-1055 */
#if defined(PRJ_PDVR)
	APP_KEY_PRESS_MENU		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4)	|BTN_ID_MENU), /* 0xF100-1037 */
	APP_KEY_RELEASE_MENU   	= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4)	|BTN_ID_MENU), /* 0xF100-1057 */

	APP_KEY_PRESS_MODE		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4)	|BTN_ID_MODE), /* 0xF100-1036 */
	APP_KEY_RELEASE_MODE   	= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4)	|BTN_ID_MODE), /* 0xF100-1056 */
#else
	APP_KEY_PRESS_MENU		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4)	|BTN_ID_MENU), /* 0xF100-1036 */
	APP_KEY_RELEASE_MENU   	= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4)	|BTN_ID_MENU), /* 0xF100-1056 */

	APP_KEY_PRESS_MODE		= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4)	|BTN_ID_MODE), /* 0xF100-1037 */
	APP_KEY_RELEASE_MODE   	= (APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4)	|BTN_ID_MODE), /* 0xF100-1057 */
#endif
	APP_KEY_CARD_IN	        = (APP_KEY_MSG|SP5K_MSG_DISK_INSERT |SP5K_DRIVE_SD),   /* 0xF100-0203 */
	APP_KEY_CARD_OUT        = (APP_KEY_MSG|SP5K_MSG_DISK_REMOVAL|SP5K_DRIVE_SD),   /* 0xF100-0202 */
#if defined(PRJ_PDVR)	
	APP_KEY_GSENSOR_TRIGGER	= 0xF1000300, /* SP5K_MSG_GSENSOR_EVENT 0xF100-0300  Isaac add 20161116*/
	APP_KEY_EVENT_FINISH		= 0xF1000301, /* SP5K_MSG_EVENT_FINISH 0xF100-0301  Isaac add 20161116*/


	APP_KEY_VERSION			 = 0xf1001044,/*Isaac add 20161212 for factory version key*/
#endif

	APP_KEY_SKIP		    = (APP_KEY_MSG|0xFFFE),   /* 0xF100-FFFE */
	APP_KEY_ERROR		    = (APP_KEY_MSG|0xFFFF),   /* 0xF100-FFFF */

} appKeyMsg_e;


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define KEY_STATE(key)		((key)&0x0000ffff)    /* key state and UI mode state */
#define KEY_GROUP(key)		((key)&0x0000fff0)
#define KEY_CODE(key)		((key)&0x0000000f)

#define APP_KEY_PRESS(key)			((key)&0xff00f0f0)
#define IS_APP_KEY_MSG(msg)         (((msg)&0xff000000)==APP_KEY_MSG)
#define APP_GPIO_KEY_PRESS_MSG		(APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_PRESS<<4))	/* 0xF100-1000 */
#define APP_GPIO_KEY_REPEAT_MSG		(APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_REPEAT<<4))	/* 0xF100-1010 */
#define APP_GPIO_KEY_RELEASE_MSG	(APP_KEY_MSG|(SP5K_MSG_GPIO_BUTTON_RELEASE<<4))	/* 0xF100-1020 */
#define APP_ADC_KEY_PRESS_MSG		(APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_PRESS<<4))	/* 0xF100-1030 */
#define APP_ADC_KEY_REPEAT_MSG		(APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_REPEAT<<4))	/* 0xF100-1040 */
#define APP_ADC_KEY_RELEASE_MSG		(APP_KEY_MSG|(SP5K_MSG_ADC_BUTTON_RELEASE<<4))	/* 0xF100-1050 */

#define IS_GPIO_PRESS_MSG(msg)		(msg & (SP5K_MSG_GPIO_BUTTON_PRESS<<4))			/* 0xF100-100x & 0x1000 */
#define IS_GPIO_RELEASE_MSG(msg)	(msg & (SP5K_MSG_GPIO_BUTTON_RELEASE<<4))		/* 0xF100-102x & 0x1020 */
#define IS_ADC_PRESS_MSG(msg)		(msg & (SP5K_MSG_ADC_BUTTON_PRESS<<4))			/* 0xF100-103x & 0x1030 */
#define IS_ADC_RELEASE_MSG(msg)		(msg & (SP5K_MSG_ADC_BUTTON_RELEASE<<4))		/* 0xF100-105x & 0x1050 */


#define IS_SP5K_KEY_PRESS_MSG(msg, param)	( (msg==SP5K_MSG_GPIO_BUTTON_PRESS) || \
											  (msg==SP5K_MSG_ADC_BUTTON_PRESS) || \
											 ((APP_KEY_PRESS(msg)== APP_GPIO_KEY_PRESS_MSG)||(APP_KEY_PRESS(msg)== APP_ADC_KEY_PRESS_MSG)))

#define IS_SP5K_BTN_MSG(msg)	( ((msg)&0xfffffff0)==SP5K_MSG_HEAD_BUTTON )


#define IS_STATE_CTRL_MSG(msg)      (((msg)&0xff000000)==SP5K_MSGX_STATE)
#define IS_PB_KEY_PRESS_MSG(msg)    (msg ==APP_KEY_PRESS_PB)

#define IS_KEY_S1_PRESSED		(IN_BUTTON_SNAP1 || IN_BUTTON_SNAP1)
#define IS_KEY_S2_PRESSED		(IN_BUTTON_SNAP2 || IN_BUTTON_SNAP2)
#define IS_KEY_TELE_PRESSED		(IN_BUTTON_TELE || IN_BUTTON_TELE)
#define IS_KEY_WIDE_PRESSED		(IN_BUTTON_WIDE || IN_BUTTON_WIDE)
#define IS_KEY_UP_PRESSED		(IO_KEY_UP_GET || IO_KEY_UP_GET)
#define IS_KEY_DOWN_PRESSED		(IO_KEY_DOWN_GET || IO_KEY_DOWN_GET)
#define IS_KEY_LEFT_PRESSED		(IO_KEY_LEFT_GET || IO_KEY_LEFT_GET)
#define IS_KEY_RIGHT_PRESSED	(IO_KEY_RIGHT_GET || IO_KEY_RIGHT_GET)
#define IS_KEY_OK_PRESSED		(IO_KEY_OK_GET || IO_KEY_OK_GET)

#define APP_KEY_PWR_SRC     (1<<CFG_BUTTON_POWER)
#define APP_KEY_PWR_GRP     GRP_BUTTON_POWER
#define APP_KEY_SKIP_SRC    (1<<CFG_BUTTON_SNAP2)
#define APP_KEY_SKIP_GRP    GRP_BUTTON_SNAP2

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appKeyLookup(char * keyname);
UINT32 appBtnMsgToKey(UINT32 msgId, UINT32 msgParam);
UINT32 appGpioMsg2Key(UINT32 msgId, UINT32 msgParam);
UINT32 appAdcMsg2Key(UINT32 msgId, UINT32 msgParam);
void appBtnControl(UINT32 btnID, UINT32 isEnable);
#endif  /* _APP_KEY_DEF_H_ */

