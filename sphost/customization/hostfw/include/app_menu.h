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

#ifndef _APP_MENU_H_
#define _APP_MENU_H_
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define	MENU_TAB_GET_FOCUS		0
#define	MENU_TAB_LOST_TFOCUS	1
#define	MENU_TAB_DISABLE		2
	
#define	MENU_BODY_GET_FOCUS		3
#define	MENU_BODY_LOST_FOCUS	4
#define	MENU_BODY_DISABLE		5
	
#define	MENU_STR_GET_FOCUS		6
#define	MENU_STR_LOST_FOCUS		7
#define	MENU_STR_DISABLE		8
	
#define	MENU_ICON_GET_FOCUS		9
#define	MENU_ICON_LOST_FOCUS	10
#define	MENU_ICON_DISABLE		11
	
#define	MENU_PROMPT_GET_FOCUS	12
#define	MENU_PROMPT_LOST_FOCUS	13
#define	MENU_PROMPT_DISABLE		14
	
#define	MENU_BAR_GET_FOCUS		15
#define	MENU_BAR_LOST_TFOCUS	16
#define	MENU_BAR_DISABLE		17
	
#define	MENU_DLG_GET_FOCUS		18

#define	SUB_MENU_STR_GET_FOCUS		19
#define	SUB_MENU_STR_LOST_FOCUS		20

enum viewMenuItem_e
{
	VIEWMENU_CHANGE					= (unsigned char) 0x00,
	/* still menu */
	VIEWMENU_STILL_RESOLUTION		,
	VIEWMENU_STILL_QUALITY			,
	VIEWMENU_STILL_DRIVERMODE		,
	VIEWMENU_DATESTAMP				,
	/* video menu */
	VIEWMENU_VIDEO_SIZE				,
	VIEWMENU_VIDEO_QUALITY			,
	VIEWMENU_VIDEO_SEAMLESS			,
	VIEWMENU_VIDEO_DATESTAMP		,
	VIEWMENU_TOTAL					,
};

typedef enum {
	SETUPMENU_CHANGE					= (unsigned char) 0x00,
	SETUPMENU_SETUP_SOUNDS				,
	SETUPMENU_SETUP_LIGHT_FREQ			,
	SETUPMENU_SETUP_POWER_SAVE			,
	SETUPMENU_SETUP_POWER_ON_RECORD		,
	SETUPMENU_SETUP_PERIODICAL_CAPTURE	,
	SETUPMENU_SETUP_DATE_TIME			,
	SETUPMENU_SETUP_LANGUAGE			,
	SETUPMENU_SETUP_TV_OUT				,
	SETUPMENU_SETUP_USB					,
	SETUPMENU_SETUP_FORMAT				,
	SETUPMENU_SETUP_RESET_ALL			,
	SETUPMENU_TOTAL						,
} appMenuSetupMenu_e;

typedef enum {
	MENU_ACTION_SELECT	= (unsigned char) 0x00,
	MENU_ACTION_SET,
	MENU_ACTION_RETURN,
} appMenuAction_e;

typedef enum _SET_RTC_ITEM
{
	SET_RTC_YEAR			= (unsigned char) 0x00,
	SET_RTC_MONTH			= (unsigned char) 0x01,
	SET_RTC_DAY 	        = (unsigned char) 0x02,
	SET_RTC_HOUR			= (unsigned char) 0x03,
	SET_RTC_MINUTE			= (unsigned char) 0x04,
	SET_DATE_STYLE			= (unsigned char) 0x05,
	SAVE_RTC_TIME			= (unsigned char) 0x06,
	SET_RTC_TOTAL_ITEMS     = (unsigned char) 0x07,
} set_RTC_Item_e;

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct appMenuInfo_s {
	UINT8 totalTabs;
	UINT8 curTab;
	UINT8 lastTab;
	UINT8 recentlyTab;
	UINT8 tab[32];
	UINT32 enableTab;
} appMenuInfo_t,*PappMenuInfo_t;

typedef struct appMenuVideoInfo_s {
	UINT32 totalVTabs;
    UINT32 curVTab;
	UINT32 lastVTab;
    UINT32 menuHLTab;
	UINT32 recentlyVTab;
} appMenuVideoInfo_t,*PappMenuVideoInfo_t;

typedef struct appSubMenuInfo_s {
	UINT8 totalItem;
	UINT8 firstItem;
	UINT8 lastItem;
	UINT8 prevItem;
	UINT8 osdFirstItem;
	UINT8 osdLastItem;
	UINT16 curItem;
	UINT32 osdEnableItems;
} appSubMenuInfo_t,*PappSubMenuInfo_t;

typedef void appMenuFunctionSetCBF_t(appMenuAction_e selPara,appMenuInfo_t *pmenuCurTab,appSubMenuInfo_t *psubMenuCurItem);

#endif /* _APP_MENU_H_ */

