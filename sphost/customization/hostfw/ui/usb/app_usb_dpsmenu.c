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

#define HOST_DBG 0
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"

#include "sp5k_usb_api.h"

#include "app_dev_plug.h"
#include "app_osdfont.h"
#include "app_osd.h"
#include "app_osd_api.h"
#include "app_menu_frameosd.h"
#include "app_usb_dpslib.h"
#include "app_playback_def.h"
#include "app_menu.h"
#include "app_usb_dps_osd.c"
#include "app_usb_dps_menu.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
extern dpsImage_t  gDpsImage;
appMenuInfo_t gDpsMainMenu;
appSubMenuInfo_t gDpsSubMenu;
extern dpsInfo_t gDpsInfo;
extern dpsMenuOption_t gDpsMenuOpt ;

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appUsbDpsMenuStateInit
 *------------------------------------------------------------------------*/
void
appUsbDpsMenuStateInit(
	void
)
{
	appOsd_GfxOsdRefreshSet(FALSE);
	CLEAR_OSD_SRC;
	if (FAIL==appPbImagePlay(gDpsImage.curImgIdx)) {
        HOST_PROF_LOG_ADD(LEVEL_ERROR,"usb dps menu: appUsbDpsMenuState :Play Image Error");
	}
	appUsbDpsMainMenuStateCreate(gDpsMenuOpt);
	appMenu_TKMenuDpsMenuFrameDraw(TRUE);
	appDPSMenu_DPSMainMenuTitleDraw();

	appOsd_GfxOsdRefreshSet(TRUE);
	gDpsInfo.dpsModePara =DPS_STANDBYMODE;
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsMenuStateOnKey
 *------------------------------------------------------------------------*/
void 
appUsbDps_DpsMenuStateOnKey(
	UINT32 msg,
	UINT32 param)
{
	switch (msg) {
	case APP_KEY_PRESS_S1:
	case APP_KEY_PRESS_TELE:
	case APP_KEY_PRESS_WIDE:
	case APP_KEY_RELEASE_WIDE:
	case APP_KEY_RELEASE_TELE:
	case APP_KEY_PRESS_MENU:
	case APP_KEY_PRESS_MODE:	
		break;	
	case APP_KEY_PRESS_SET:
		appStateChange(APP_STATE_USB_PRINTSUBMENU,STATE_PARAM_NORMAL_INIT);
		break;
	case APP_KEY_PRESS_LEFT:
	case APP_KEY_PRESS_RIGHT:
		break;
	case APP_KEY_PRESS_UP:
		gDpsMainMenu.recentlyTab = gDpsMainMenu.curTab;
		appMenu_MainMenuInfoDecreaseSet(&gDpsMainMenu);		
		if (gDpsMainMenu.curTab == DPS_MENU_CHANGE) 
		{
			gDpsMainMenu.curTab=gDpsMainMenu.totalTabs;
		}
		appUsbDpsSubMenuStateCreate(  gDpsMenuOpt );
		appOsd_GfxOsdRefreshSet(FALSE);
		appDPSMenu_DPSMainMenuTitleChangeDraw();
		appOsd_GfxOsdRefreshSet(TRUE);
		break;
	case APP_KEY_PRESS_DOWN:
		gDpsMainMenu.recentlyTab = gDpsMainMenu.curTab;
		appMenu_MainMenuInfoIncreaseSet(&gDpsMainMenu);
		if (gDpsMainMenu.curTab == DPS_MENU_CHANGE) 
		{
			gDpsMainMenu.curTab=gDpsMainMenu.curTab+1;
		}
		appUsbDpsSubMenuStateCreate(  gDpsMenuOpt );
		appOsd_GfxOsdRefreshSet(FALSE);
		appDPSMenu_DPSMainMenuTitleChangeDraw();
		appOsd_GfxOsdRefreshSet(TRUE);
		break;	
	default:
		break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsMenuStateOnMsg
 *------------------------------------------------------------------------*/
void 
appUsbDps_DpsMenuStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch ( msg) {
	case SP5K_MSG_USB_DPS_DISCOVER:	
	case SP5K_MSG_USB_DPS_NOTIFY_JOB_STATUS:	
	case SP5K_MSG_USB_DPS_NOTIFY_DEVICE_STATUS:	
	default:
		break;
	}	
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDpsMenuState
 *------------------------------------------------------------------------*/
void 
appUsbDpsMenuState(
	UINT32 msg,
	UINT32 param
)
{	
	DBG_PRINT("usb dps menu: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);

	switch (msg) {
	case APP_STATE_MSG_INIT:	
		RPrintf("appUsbDpsMenuState");
		appUsbDpsMenuStateInit();
		break;
	case APP_STATE_MSG_CLOSE:		
		appStateCloseDone();
		break;
	case APP_UI_MSG_USB_PLUG:	
		sp5kFree(gDpsInfo.PicBuf);
		appStateCloseDone();
		break;
	default:	
		if (IS_APP_KEY_MSG(msg)) {
			appUsbDps_DpsMenuStateOnKey(msg,param);
		}
		else{
			appUsbDps_DpsMenuStateOnMsg(msg,param);
		}
		break;
	}
} 

/*-------------------------------------------------------------------------
 *  File Name : appUsbDpsMainMenuStateCreate
 *------------------------------------------------------------------------*/
void appUsbDpsMainMenuStateCreate( dpsMenuOption_t  gDpsMenuOpt )
{

	unsigned short count;
	
	count = 0;
	gDpsMainMenu.tab[count++] = DPS_MENU_PRINT;
	gDpsMainMenu.tab[count++] = DPS_MENU_IMAGE;
	gDpsMainMenu.tab[count++] = DPS_MENU_QUALITY;
	gDpsMainMenu.tab[count++] = DPS_MENU_LAYOUT;
	gDpsMainMenu.tab[count++] = DPS_MENU_PAPER_SIZE;
	gDpsMainMenu.tab[count++] = DPS_MENU_PAPER_TYPE;
	gDpsMainMenu.tab[count++] = DPS_MENU_DATE;
	gDpsMainMenu.tab[count++] = DPS_MENU_RESET;	
	gDpsMainMenu.totalTabs = count;
	gDpsMainMenu.enableTab = 0xFFFF;

	appUsbDpsSubMenuStateCreate(gDpsMenuOpt);
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDpsSubMenuStateCreate
 *------------------------------------------------------------------------*/
void appUsbDpsSubMenuStateCreate( dpsMenuOption_t gDpsMenuOpt )
{

	switch( gDpsMainMenu.tab[gDpsMainMenu.curTab - 1] )
	{
	case DPS_MENU_PRINT:
		gDpsSubMenu.curItem=gDpsMenuOpt.Print.SelOption;
		gDpsSubMenu.totalItem= DPS_PRINT_TOTAL;
		gDpsSubMenu.firstItem= DPS_PRINT_STANDARD;
		gDpsSubMenu.lastItem= DPS_PRINT_DPOF;
		/*s modified by Leo @20100416*/
		//gDpsSubMenu.osdEnableItems= gDpsMenuOpt.Print.OsdEnOption;
		gDpsSubMenu.osdEnableItems=0xFFFFFFFF;
		/*e modified by Leo @20100416*/
		break;

	case DPS_MENU_IMAGE:
		gDpsSubMenu.curItem=gDpsMenuOpt.Image.SelOption;
		gDpsSubMenu.totalItem= DPS_SELECT_TOTAL;
		gDpsSubMenu.firstItem= DPS_SELECT_ONE;
		gDpsSubMenu.lastItem= DPS_SELECT_ALL;
		gDpsSubMenu.osdEnableItems= 0xFFFFFFFF;
		break;
		
	case DPS_MENU_QUALITY:
		gDpsSubMenu.curItem=gDpsMenuOpt.Quality.SelOption;
		gDpsSubMenu.totalItem= DPS_PAPER_QUALITY_TOTAL;
		gDpsSubMenu.firstItem= DPS_PAPER_QUALITY_AUTO;
		gDpsSubMenu.lastItem= DPS_PAPER_QUALITY_DRAFT;
		gDpsSubMenu.osdEnableItems= gDpsMenuOpt.Quality.OsdEnOption;
		break;
	case DPS_MENU_LAYOUT:
		gDpsSubMenu.curItem=gDpsMenuOpt.Layout.SelOption;
		gDpsSubMenu.totalItem= DPS_LAYOUT_TOTAL;
		gDpsSubMenu.firstItem= DPS_LAYOUT_AUTO;
		gDpsSubMenu.lastItem = DPS_LAYOUT_16;
		gDpsSubMenu.osdEnableItems= gDpsMenuOpt.Layout.OsdEnOption;
		break;
		
	case DPS_MENU_PAPER_SIZE:
		gDpsSubMenu.curItem=gDpsMenuOpt.PaperSize.SelOption;
		gDpsSubMenu.totalItem= DPS_PAPER_SIZE_TOTAL;
		gDpsSubMenu.firstItem= DPS_PAPER_SIZE_AUTO;
		gDpsSubMenu.lastItem= DPS_PAPER_SIZE_A3;
		/*s modified by Leo @20100416*/
		//gDpsSubMenu.osdEnableItems= gDpsMenuOpt.PaperSize.OsdEnOption;
		gDpsSubMenu.osdEnableItems=0xFFFFFFFF;
		/*e modified by Leo @20100416*/
		break;
		
	case DPS_MENU_PAPER_TYPE:
		gDpsSubMenu.curItem=gDpsMenuOpt.PaperType.SelOption;
		gDpsSubMenu.totalItem= DPS_PAPER_TYPE_TOTAL;
		gDpsSubMenu.firstItem= DPS_PAPER_TYPE_AUTO;
		gDpsSubMenu.lastItem= DPS_PAPER_TYPE_FAST_PHOTO;
		gDpsSubMenu.osdEnableItems= gDpsMenuOpt.PaperType.OsdEnOption;
		break;
		
	case DPS_MENU_DATE:
		gDpsSubMenu.curItem=gDpsMenuOpt.DateTime.SelOption;
		gDpsSubMenu.totalItem= DPS_DATE_TOTAL;
		gDpsSubMenu.firstItem= DPS_DATE_AUTO;
		gDpsSubMenu.lastItem= DPS_DATE_ON;
		gDpsSubMenu.osdEnableItems= gDpsMenuOpt.DateTime.OsdEnOption;
		break;
		
	case DPS_MENU_FILE_NO:
		gDpsSubMenu.curItem=gDpsMenuOpt.FileNo.SelOption;
		gDpsSubMenu.totalItem= DPS_FILE_TOTAL;
		gDpsSubMenu.firstItem= DPS_FILE_AUTO;
		gDpsSubMenu.lastItem= DPS_FILE_ON;
		gDpsSubMenu.osdEnableItems= gDpsMenuOpt.FileNo.OsdEnOption;
		break;
		
	case DPS_MENU_DATETYPE:
		gDpsSubMenu.curItem=gDpsMenuOpt.DateType.SelOption;
		gDpsSubMenu.totalItem= DPS_DATETYPE_TOTAL;
		gDpsSubMenu.firstItem= DPS_DATETYPE_YYMMDD;
		gDpsSubMenu.lastItem= DPS_DATETYPE_MMDDYY;
		gDpsSubMenu.osdEnableItems= 0xFFFFFFFF;
		break;
		
	case DPS_MENU_RESET:
		gDpsSubMenu.curItem=DPS_RESET_OFF;
		gDpsSubMenu.totalItem= DPS_RESET_TOTAL;
		gDpsSubMenu.firstItem= DPS_RESET_ON;
		gDpsSubMenu.lastItem= DPS_RESET_OFF;
		gDpsSubMenu.osdEnableItems= 0xFFFFFFFF;
		break;	

	default :
		break;
	}
}

