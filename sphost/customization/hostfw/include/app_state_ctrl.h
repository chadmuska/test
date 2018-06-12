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
#ifndef _APP_STATE_CTRL_H_
#define _APP_STATE_CTRL_H_
#include "app_key_def.h"
#include "app_ui_msg.h"

#define TEST_STATE_CHANG_MSG    1

typedef enum {

    /* sys */
    APP_STATE_NULL_IDX,
    APP_STATE_POWER_ON_IDX,
    APP_STATE_POWER_OFF_IDX,
    APP_STATE_POWER_ON_SET_LANGUAGE_IDX,
    APP_STATE_POWER_HIBERNATE_IDX,
#ifdef JET_FEATURE
    APP_STATE_JET_POWER_OFF_IDX,
    APP_STATE_JET_SDCERT_IDX,
#endif

    /* still  */
    APP_STATE_STILL_PREVIEW_IDX,
    APP_STATE_STILL_CAPTURE_IDX,
    APP_STATE_BURST_CAPTURE_IDX,
    APP_STATE_STILL_AAA_IDX,
    APP_STATE_STILL_BUSY_IDX,
    APP_STATE_MENU_IDX,

    /* video */
    APP_STATE_VIDEO_PREVIEW_IDX,
    APP_STATE_VIDEO_REC_IDX,
	APP_STATE_VIDEO_AAA_IDX,

    APP_STATE_VIDEO_MAINMENU_IDX,
    APP_STATE_VIDEO_SUBMENU_IDX,
    APP_STATE_VIDEOSETUP_MAINMENU_IDX,
    APP_STATE_VIDEOSETUP_SUBMENU_IDX,
    APP_STATE_VIDEOSETUP_2NDMENU_IDX,

    #if GPS_OPEN
    APP_STATE_GPS_GSVINFO_IDX,
    #endif

    /* playback */
    APP_STATE_PB_MAIN_IDX,
    APP_STATE_PB_VIDEO_IDX,
    APP_STATE_PB_IMG_PROT_IDX,
    APP_STATE_PB_IMG_DEL_IDX,
	APP_STATE_PLAY_MAINMENU_IDX,
	APP_STATE_PLAY_SUBMENU_IDX,
    APP_STATE_PB_SETUP_MAINMENU_IDX,
    APP_STATE_PB_SETUP_SUBMENU_IDX,
    APP_STATE_PB_SETUP_2NDMENU_IDX,
    APP_STATE_PB_FILETYPE_MENU_SET_IDX,

    /* usb */
    APP_STATE_USB_CREATE_IDX,
    APP_STATE_USB_MSDC_IDX,
    APP_STATE_USB_PCCAM_IDX,
    APP_STATE_USB_BATTERY_CHARGING_IDX,

    /* TV plug*/
    APP_STATE_TV_PLUG_IDX,

    /* HDMI plug*/
    #if (KIT_WITH_HDMI)
    APP_STATE_HDMI_PLUG_IDX,
    APP_STATE_CEC_CAPABILITY_CTRL_IDX,
    #endif

    /* card plug */
    APP_STATE_CARD_PLUG_IDX,

    /* Customer */
    APP_STATE_CUSTOMER_IDX,
    APP_STATE_CUSTOMER_OTHERS,
    APP_STATE_MAX_IDX
} appStateGroupIdx_e;

/* [bit 8 ~ bit 11] : use for state group (0x00-- to 0x0F--) */
typedef enum {
	APP_STATE_GROUP_SYS	        = 0x0000,
	APP_STATE_GROUP_VIEW	    = 0x0100,
	APP_STATE_GROUP_VIDEO	    = 0x0200,
	APP_STATE_GROUP_AUDIO	    = 0x0300,
	APP_STATE_GROUP_PLAYBACK	= 0x0400,
    /* dev plug */
	APP_STATE_GROUP_USB	        = 0x0500,
	APP_STATE_GROUP_TV_PLUG	    = 0x0600,
    APP_STATE_GROUP_HDMI_PLUG	= 0x0700,
	APP_STATE_GROUP_CARD_PLUG	= 0x0800,
    /* Calib */
	APP_STATE_GROUP_CALIB	    = 0x0900,
    /* Test */
	APP_STATE_GROUP_TEST	    = 0x0A00,
    /* Customer */
	APP_STATE_GROUP_CUSTOMER    = 0x0B00,
	/* EZ_menu*/
	APP_STATE_GROUP_MENU		= 0x0C00,
} appStateGroup_e;

/* [bit 12]: set bit-12 to 1 if state is a menu state */
#define MENU_STATE_TAG	  0x1000

/* [bit 0 ~ bit 7] : state ID by group*/
/* state definition */
/* Please Note: after update the state definition, the appStateTbl[] also need to be updated */
typedef enum {
	/* sys group */
	APP_STATE_NULL    		        = (APP_STATE_GROUP_SYS |0x00),
	APP_STATE_POWER_ON	            = (APP_STATE_GROUP_SYS |0x01),
	APP_STATE_POWER_OFF	            = (APP_STATE_GROUP_SYS |0x02),
	APP_STATE_POWER_ON_SET_LANGUAGE	= (APP_STATE_GROUP_SYS |0x03),
	APP_STATE_POWER_HIBERNATE       = (APP_STATE_GROUP_SYS |0x04),
#ifdef JET_FEATURE
	APP_STATE_JET_POWER_OFF       = (APP_STATE_GROUP_SYS |0x05),
	APP_STATE_JET_SD_CERT       = (APP_STATE_GROUP_SYS |0x06),
#endif
	/* still view group */
	APP_STATE_STILL_PREVIEW	        = (APP_STATE_GROUP_VIEW |0x00),
	APP_STATE_STILL_CAPTURE	        = (APP_STATE_GROUP_VIEW |0x01),
	APP_STATE_BURST_CAPTURE	        = (APP_STATE_GROUP_VIEW |0x02),
	APP_STATE_STILL_AAA	            = (APP_STATE_GROUP_VIEW |0x03),
	APP_STATE_STILL_BUSY            = (APP_STATE_GROUP_VIEW |0x04),

	/* video group */
	APP_STATE_VIDEO_PREVIEW	        = (APP_STATE_GROUP_VIDEO |0x00),
	APP_STATE_VIDEO_REC		        = (APP_STATE_GROUP_VIDEO |0x01),
	APP_STATE_VIDEO_AAA 			= (APP_STATE_GROUP_VIDEO |0x02),

	/* audio group */
	APP_STATE_AUDIO_REC 	= (APP_STATE_GROUP_AUDIO |0x00),

	/*MENU GROUP*/
	APP_STATE_MENU		= (APP_STATE_GROUP_MENU |0x00),
	APP_STATE_MENU_SYSTEM = (APP_STATE_GROUP_MENU |0x01),

	#if GPS_OPEN
	APP_STATE_GPS_GSVINFO 			= (APP_STATE_GROUP_VIDEO |MENU_STATE_TAG|0x08),
	#endif

	/* playback group */
	APP_STATE_PB_MAIN		        = (APP_STATE_GROUP_PLAYBACK |0x00),
 	APP_STATE_PB_VIDEO	            = (APP_STATE_GROUP_PLAYBACK |0x01),
 	APP_STATE_PB_IMG_PROT	        = (APP_STATE_GROUP_PLAYBACK |0x02),
 	APP_STATE_PB_IMG_DEL	        = (APP_STATE_GROUP_PLAYBACK |0x03),

	/* USB */
	APP_STATE_USB_CREATE		    = (APP_STATE_GROUP_USB |0x00),
	APP_STATE_USB_MSDC		        = (APP_STATE_GROUP_USB |0x01),
	APP_STATE_USB_PCCAM		        = (APP_STATE_GROUP_USB |0x02),
	APP_STATE_USB_BATTERY_CHARGING  = (APP_STATE_GROUP_USB |0x03),

	/* TV PLUG */
	APP_STATE_TV_PLUG		        = (APP_STATE_GROUP_TV_PLUG |0x00),

	/* HDMI PLUG */
	#if (KIT_WITH_HDMI)
	APP_STATE_HDMI_PLUG		        = (APP_STATE_GROUP_HDMI_PLUG |0x00),
	#endif
	#if SPCA6330
	APP_STATE_CEC_CAPABILITY_CTRL	= (APP_STATE_GROUP_HDMI_PLUG |0x01),
	#endif

	/* CARD PLUG */
	APP_STATE_CARD_PLUG		        = (APP_STATE_GROUP_CARD_PLUG |0x00),

	/* CALIB */
	APP_STATE_CALIBRATION	        = (APP_STATE_GROUP_CALIB |0x00),
	APP_STATE_CALIB_ZOOM	        = (APP_STATE_GROUP_CALIB |0x01),
	APP_STATE_CALIB_AWB		        = (APP_STATE_GROUP_CALIB |0x02),

	/* TEST */
	APP_STATE_TEST_MAIN	            = (APP_STATE_GROUP_TEST |0x00),
	APP_STATE_TEST_CAMCFG	        = (APP_STATE_GROUP_TEST |0x01),
	APP_STATE_TEST_UNIT_TEST        = (APP_STATE_GROUP_TEST |0x02),
	APP_STATE_TEST_BAT_CURVE        = (APP_STATE_GROUP_TEST |0x03),

    /* Customer */
    APP_STATE_CUSTOMER              = (APP_STATE_GROUP_CUSTOMER |0x00),
    APP_STATE_CUSTOMER_VIDEO_PREVIEW    = (APP_STATE_GROUP_CUSTOMER |0x01),
    APP_STATE_CUSTOMER_VIDEO_REC        = (APP_STATE_GROUP_CUSTOMER |0x02)
} appState_e;


/* state ctrl cmd */
typedef enum {
	STATE_INIT = 0x00,
	STATE_CLOSE,
	STATE_ABORT,
	STATE_ERROR = 0xFF,
	#if TEST_STATE_CHANG_MSG
	STATE_CHANG = 0xF00000
	#endif
} appStateCmd_e;

/* app state control msg */
typedef enum {
	APP_STATE_MSG_INIT          = (SP5K_MSGX_STATE|STATE_INIT),  /* 0xFA00-0000 */
	APP_STATE_MSG_CLOSE         = (SP5K_MSGX_STATE|STATE_CLOSE), /* 0xFA00-0001 */
	APP_STATE_MSG_ABORT         = (SP5K_MSGX_STATE|STATE_ABORT), /* 0xFA00-0002 */
	APP_STATE_MSG_ERR           = (SP5K_MSGX_STATE|STATE_ERROR), /* 0xFA00-00FF */
	#if TEST_STATE_CHANG_MSG
	APP_STATE_MSG_CHANG         = (SP5K_MSGX_STATE|STATE_CHANG), /* 0xFAF0-0000 */	/*test for state change msg*/
	#endif
} appStateCtrlMsg_e;

#if TEST_STATE_CHANG_MSG
#define IS_STATE_CHANG_MSG(state) ((state&APP_STATE_MSG_CHANG) == APP_STATE_MSG_CHANG)
#define MAX_STATE_TAG   (MENU_STATE_TAG|0xfff);
typedef enum {
	STATE_CHANG_REQUEST = 0x00,
	STATE_CHANG_MSG_FINISH,
} appStateChangeMsg_e;
#endif

/* state msg parameter */
typedef enum {
	STATE_PARAM_NORMAL_INIT     = 0x00,
	STATE_PARAM_LENS_ERROR      = APP_POWER_OFF_LENS_ERROR,
	STATE_PARAM_BATT_EMPTY      = APP_POWER_OFF_BATT_EMPTY,
	STATE_PARAM_CHARGE_ERROR    = APP_POWER_OFF_CHARGE_ERROR,
	STATE_PARAM_CARD_PLUG       = APP_POWER_OFF_CARD_PLUG,
	STATE_PARAM_USB_PLUG        = APP_POWER_OFF_USB_PLUG,
	STATE_PARAM_TV_PLUG         = APP_POWER_OFF_TV_PLUG,
	STATE_PARAM_HDMI_PLUG       = APP_POWER_OFF_HDMI_PLUG,
	STATE_PARAM_VIDEO_REC_ERROR = APP_POWER_OFF_VIDEO_REC_ERROR,
	STATE_PARAM_POWER_ON        = 0x1000,
	STATE_PARAM_FREE_CARRY      = 0xF000, /* after this, free for each state */
	/*-- SYS_GROUP Param : Started from 0xF000 (0xF000+APP_STATE_GROUP_SYS) */
	/*-- VIEW_GROUP Param : Started from 0xF100 (0xF000+APP_STATE_GROUP_VIEW) */
	/*-- VIDEO_GROUP Param : Started from 0xF200 (0xF000+APP_STATE_GROUP_VIDEO) */
	/*-- PB_GROUP Param : Started from 0xF400 (0xF000+APP_STATE_GROUP_PLAYBACK) */
	STATE_PARAM_PB_DPOF_CANCEL  = (STATE_PARAM_FREE_CARRY+APP_STATE_GROUP_PLAYBACK+0x00), /* 0xF400 */
	STATE_PARAM_PB_DPOF_SET     = (STATE_PARAM_FREE_CARRY+APP_STATE_GROUP_PLAYBACK+0x01), /* 0xF401 */
	/*for EZ_MENU*/
	STATE_PARAM_MENU_POS_SET = (STATE_PARAM_FREE_CARRY+APP_STATE_GROUP_MENU+0x00), /* 0xF500 */
} appStateMsgeParam_e;


/* state process prototype*/
typedef void (*pfpSATATE)(UINT32 msg, UINT32 param);

#define STATE_GROUP(state)	((state)&0x00000f00)

#define IS_SYS_GROUP(state)		    (STATE_GROUP(state) == APP_STATE_GROUP_SYS)
#define IS_VIEW_GROUP(state)	    (STATE_GROUP(state) == APP_STATE_GROUP_VIEW)
#define IS_VIDEO_GROUP(state)	    (STATE_GROUP(state) == APP_STATE_GROUP_VIDEO)
#define IS_AUDIO_GROUP(state)	    (STATE_GROUP(state) == APP_STATE_GROUP_AUDIO)
#define IS_PLAYBACK_GROUP(state)	(STATE_GROUP(state) == APP_STATE_GROUP_PLAYBACK)
#define IS_USB_GROUP(state)	        (STATE_GROUP(state) == APP_STATE_GROUP_USB)
#define IS_CALIB_GROUP(state)	    (STATE_GROUP(state) == APP_STATE_GROUP_CALIB)
#define IS_TV_PLUG_GROUP(state)	    (STATE_GROUP(state) == APP_STATE_GROUP_TV_PLUG)
#define IS_CARD_PLUG_GROUP(state)	(STATE_GROUP(state) == APP_STATE_GROUP_CARD_PLUG)
#define IS_MENU_STATE(state)		((state & MENU_STATE_TAG) == MENU_STATE_TAG)
#define IS_HDMI_PLUG_GROUP(state)	(STATE_GROUP(state) == APP_STATE_GROUP_HDMI_PLUG)

/* hot plug config */
#define CARD_PLUG_CFG_BIT       0  /* bit [0 : 3] */
#define USB_PLUG_CFG_BIT        4  /* bit [4 : 7] */
#define TV_PLUG_CFG_BIT         8  /* bit [8 : 11] */
#define HDMI_PLUG_CFG_BIT      12  /* bit [12 : 16}] */

typedef enum {
	HOT_PLUG_IGNORE         = 0x00, /* ignore hot plug msg */
	HOT_PLUG_PENDING        = 0x01, /* 1. pending hot plug;  2. process dev change after next sate init  */
	HOT_PLUG_DO_PROC        = 0x02, /* process hot plug msg */
	HOT_PLUG_DO_ABORT       = 0x03, /* 1. send abort msg to active state first; 2. process dev change after next sate init */
	HOT_PLUG_DO_POWER_OFF   = 0x04  /* send power off msg*/
} appStateHotCfg_e;

#define CARD_PLUG_IGNORE        (HOT_PLUG_IGNORE << CARD_PLUG_CFG_BIT)
#define CARD_PLUG_PENDING       (HOT_PLUG_PENDING << CARD_PLUG_CFG_BIT)
#define CARD_PLUG_DO_PROC       (HOT_PLUG_DO_PROC << CARD_PLUG_CFG_BIT)
#define CARD_PLUG_DO_ABORT      (HOT_PLUG_DO_ABORT << CARD_PLUG_CFG_BIT)
#define CARD_PLUG_DO_POWER_OFF  (HOT_PLUG_DO_POWER_OFF << CARD_PLUG_CFG_BIT)

#define USB_PLUG_IGNORE         (HOT_PLUG_IGNORE << USB_PLUG_CFG_BIT)
#define USB_PLUG_PENDING        (HOT_PLUG_PENDING << USB_PLUG_CFG_BIT)
#define USB_PLUG_DO_PROC        (HOT_PLUG_DO_PROC << USB_PLUG_CFG_BIT)
#define USB_PLUG_DO_ABORT       (HOT_PLUG_DO_ABORT << USB_PLUG_CFG_BIT)
#define USB_PLUG_DO_POWER_OFF   (HOT_PLUG_DO_POWER_OFF << USB_PLUG_CFG_BIT)

#define TV_PLUG_IGNORE          (HOT_PLUG_IGNORE << TV_PLUG_CFG_BIT)
#define TV_PLUG_PENDING         (HOT_PLUG_PENDING << TV_PLUG_CFG_BIT)
#define TV_PLUG_DO_PROC         (HOT_PLUG_DO_PROC << TV_PLUG_CFG_BIT)
#define TV_PLUG_DO_ABORT        (HOT_PLUG_DO_ABORT << TV_PLUG_CFG_BIT)
#define TV_PLUG_DO_POWER_OFF    (HOT_PLUG_DO_POWER_OFF << TV_PLUG_CFG_BIT)

#define HDMI_PLUG_IGNORE        (HOT_PLUG_IGNORE << HDMI_PLUG_CFG_BIT)
#define HDMI_PLUG_PENDING       (HOT_PLUG_PENDING << HDMI_PLUG_CFG_BIT)
#define HDMI_PLUG_DO_PROC       (HOT_PLUG_DO_PROC << HDMI_PLUG_CFG_BIT)
#define HDMI_PLUG_DO_ABORT      (HOT_PLUG_DO_ABORT << HDMI_PLUG_CFG_BIT)
#define HDMI_PLUG_DO_POWER_OFF  (HOT_PLUG_DO_POWER_OFF << HDMI_PLUG_CFG_BIT)


#define ALL_PLUG_IGNORE         (CARD_PLUG_IGNORE|USB_PLUG_IGNORE|TV_PLUG_IGNORE|HDMI_PLUG_IGNORE)
#define ALL_PLUG_PENDING        (CARD_PLUG_PENDING|USB_PLUG_PENDING|TV_PLUG_PENDING|HDMI_PLUG_PENDING)
#define ALL_PLUG_DO_PROC        (CARD_PLUG_DO_PROC|USB_PLUG_DO_PROC|TV_PLUG_DO_PROC|HDMI_PLUG_DO_PROC)
#define ALL_PLUG_DO_ABORT       (CARD_PLUG_DO_ABORT|USB_PLUG_DO_ABORT|TV_PLUG_DO_ABORT|HDMI_PLUG_DO_ABORT)

#define CARD_PLUG_CFG_GET(a)    ((a & (0x000F << CARD_PLUG_CFG_BIT )) >> CARD_PLUG_CFG_BIT)
#define USB_PLUG_CFG_GET(a)     ((a & (0x000F << USB_PLUG_CFG_BIT )) >> USB_PLUG_CFG_BIT)
#define TV_PLUG_CFG_GET(a)      ((a & (0x000F << TV_PLUG_CFG_BIT )) >> TV_PLUG_CFG_BIT)
#define HDMI_PLUG_CFG_GET(a)    ((a & (0x000F << HDMI_PLUG_CFG_BIT )) >> HDMI_PLUG_CFG_BIT)


/* APP_STATE_POWER_ON*/
#define PWR_ON_PLUG_CFG         (CARD_PLUG_DO_PROC|USB_PLUG_PENDING|TV_PLUG_PENDING|HDMI_PLUG_PENDING)

/* APP_STATE_POWER_OFF*/
#define PWR_OFF_PLUG_CFG        ALL_PLUG_IGNORE

/* APP_STATE_POWER_HIBERNATE*/
#define PWR_HIBERNATE_PLUG_CFG  ALL_PLUG_IGNORE


/* APP_STATE_STILL_PREVIEW*/
#define STILL_PV_PLUG_CFG       ALL_PLUG_DO_PROC

/* APP_STATE_STILL_CAPTURE*/
#define STILL_CAP_PLUG_CFG      (CARD_PLUG_DO_POWER_OFF|USB_PLUG_PENDING|TV_PLUG_PENDING|HDMI_PLUG_PENDING)

/* APP_STATE_BURST_CAPTURE*/
#define STILL_BURST_PLUG_CFG    (CARD_PLUG_DO_POWER_OFF|USB_PLUG_PENDING|TV_PLUG_PENDING|HDMI_PLUG_PENDING)

/* APP_STATE_STILL_AAA*/
#define STILL_AAA_PLUG_CFG      ALL_PLUG_DO_ABORT

/* APP_STATE_STILL_BUSY */
#define STILL_BUSY_PLUG_CFG     ALL_PLUG_PENDING

/* APP_STATE_MENU*/
#define MENU_PLUG_CFG   ALL_PLUG_DO_PROC

/* APP_STATE_VIDEO_PREVIEW*/
#define VIDEO_PV_PLUG_CFG       ALL_PLUG_DO_PROC

/* APP_STATE_VIDEO_REC*/
#define VIDEO_REC_PLUG_CFG   	ALL_PLUG_DO_PROC

/* APP_STATE_PB_MAIN*/
#define PB_MAIN_PLUG_CFG        ALL_PLUG_DO_PROC

/* APP_STATE_PB_VIDEO*/
#define VIDEO_PLAYING_PLUG_CFG  ALL_PLUG_DO_ABORT

/* APP_STATE_PB_IMG_PROT*/
#define PB_PROT_PLUG_CFG        ALL_PLUG_DO_ABORT

/* APP_STATE_PB_IMG_DEL*/
#define PB_DEL_PLUG_CFG         ALL_PLUG_DO_ABORT

/* APP_STATE_USB_MSDC*/
#define USB_MSDC_PLUG_CFG       (CARD_PLUG_DO_PROC|USB_PLUG_DO_PROC|TV_PLUG_PENDING|HDMI_PLUG_PENDING)

/* APP_STATE_TV_PLUG*/
#define TVPLUG_STATE_PLUG_CFG   (CARD_PLUG_PENDING|USB_PLUG_PENDING|TV_PLUG_PENDING|HDMI_PLUG_PENDING)

/* APP_STATE_HDMI_PLUG*/
#define HDMIPLUG_STATE_PLUG_CFG (CARD_PLUG_PENDING|USB_PLUG_PENDING|TV_PLUG_PENDING|HDMI_PLUG_DO_PROC)

/* APP_STATE_CEC_CAPABILITY_CTRL*/
#define CEC_CAPABILITY_CTRL_STATE_PLUG_CFG   (CARD_PLUG_PENDING|USB_PLUG_PENDING|TV_PLUG_PENDING|HDMI_PLUG_DO_PROC)

/* APP_STATE_CARD_PLUG*/
#define CARDPLUG_STATE_PLUG_CFG   (CARD_PLUG_DO_PROC|USB_PLUG_PENDING|TV_PLUG_PENDING|HDMI_PLUG_PENDING)

/* APP_STATE_CALIBRATION*/
#define CALIBRATION_STATE_PLUG_CFG   ALL_PLUG_IGNORE

void appStateChange(	UINT32 newState,UINT32 param);
void appStateCloseDone(void);
UINT32 appPreviousStateGet(void);
UINT32 appActiveStateGet (void);
UINT32 appNextStateGet(void);
UINT32 appNextDialStateGet (void);
UINT32 appActiveStateHotPlugCfgGet (void);
/* for debug*/
void appStateCtrlInfoPrint(char *title);
void appStateCustomerReg(pfpSATATE RegState);
void appStateReg(appStateGroupIdx_e eStateGroupIdx, pfpSATATE pCallBackFunc);


void appUiModeSel(void);
void appUiModeChange(void);
appState_msg_flow_e appGroupViewProc(UINT32 msg,UINT32 param);
appState_msg_flow_e appGroupVideoProc(UINT32 msg,UINT32 param);
appState_msg_flow_e appGroupAudioProc(UINT32 msg,UINT32 param);
appState_msg_flow_e appGroupPlaybackProc(UINT32 msg,UINT32 param);
appState_msg_flow_e appGroupUsbProc(UINT32 msg,UINT32 param);
appState_msg_flow_e appGroupCalibrationProc(UINT32 msg,UINT32 param);

void appStateGroupCloseProc(void);
#if TEST_STATE_CHANG_MSG
void appMsgStateChange(UINT32 sel, UINT32 newState,	UINT32 param);
#endif
#endif  /* _APP_STATE_CTRL_H_ */

