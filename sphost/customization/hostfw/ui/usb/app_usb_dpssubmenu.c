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
#include "app_osd_api.h"
#include "app_menu_frameosd.h"
#include "app_usb_dpslib.h"
#include "app_menu.h"
#include "app_usb_dps_osd.h"
#include "app_usb_dps_menu.h"
#include "app_error_msg.h"
#include "app_errorosd.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static appMenuFunctionSetCBF_t *pDpsMenuKeyPressCBF = NULL;
UINT32 enPartial;

extern appSubMenuInfo_t gDpsSubMenu;
extern appMenuInfo_t gDpsMainMenu;
extern dpsMenuOption_t  gDpsMenuOpt ;
extern dpsInfo_t  gDpsInfo;
extern UINT8	DpsModePara ; 
extern UINT16 selPic;
extern dpsImage_t  gDpsImage;
extern UINT8 IsSetImage;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/


/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*-------------------------------------------------------------------------
*	File Name : appSetupMenu_SetKeyCBF
*------------------------------------------------------------------------*/
void 
appUsbDpsSubMenu_SetKeyCBFReg(
	appMenuFunctionSetCBF_t *setupKeyPressCbf
)
{
	if (setupKeyPressCbf) 
	{
		pDpsMenuKeyPressCBF = setupKeyPressCbf;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appSetupMenu_SetKeyCBF
 *------------------------------------------------------------------------*/
void 
appUsbDpsSubMenu_SetKeyCBF(
	appMenuAction_e action,
	appMenuInfo_t *menuTab,
	appSubMenuInfo_t *SubmenuItem
)
{

	if (pDpsMenuKeyPressCBF) 
	{
		(pDpsMenuKeyPressCBF)(action, menuTab, SubmenuItem);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDpsSubMenuStateInit
 *------------------------------------------------------------------------*/
void
appUsbDpsSubMenuStateInit(
	void
)
{
	appOsd_GfxOsdRefreshSet(FALSE);
	appDPSMenu_DPSSubMenuSelHiddenFrameDraw();
	appDPSMenu_DPSSubMenuFrameDraw();

	appOsd_GfxOsdRefreshSet(TRUE);
	appUsbDpsSubMenu_SetKeyCBFReg(appDPSMenuFunctionSetCB); 

}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsSubMenuStateOnKey
 *------------------------------------------------------------------------*/
void 
appUsbDps_DpsSubMenuStateOnKey(
	UINT32 msg,
	UINT32 param)
{
	switch (msg) 
	{
		case APP_KEY_PRESS_S1:
		case APP_KEY_PRESS_TELE:
		case APP_KEY_PRESS_WIDE:
		case APP_KEY_RELEASE_WIDE:
		case APP_KEY_RELEASE_TELE:
		case APP_KEY_PRESS_MODE:	
			break;
		case APP_KEY_PRESS_MENU:
		case APP_KEY_PRESS_LEFT:
			appUsbDpsSubMenu_SetKeyCBF(MENU_ACTION_RETURN,&gDpsMainMenu,&gDpsSubMenu);
			appStateChange(APP_STATE_USB_PRINTMENU,STATE_PARAM_NORMAL_INIT);
			break;
		case APP_KEY_PRESS_RIGHT:
		case APP_KEY_PRESS_SET:
			appDPSMenu_SubMenuSetKeyProcess();
			break;
		case APP_KEY_PRESS_UP:
		case APP_KEY_PRESS_DOWN:
			appDPSMenu_SubMenuChange((msg == APP_KEY_PRESS_DOWN ) ?TRUE :FALSE);
			appDPSMenu_DPSSubMenuFrameChangeDraw();
			appUsbDpsSubMenu_SetKeyCBF(MENU_ACTION_SELECT,&gDpsMainMenu,&gDpsSubMenu);

			break;	
		default:
			break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appUsbDps_DpsSubMenuStateOnMsg
 *------------------------------------------------------------------------*/
void 
appUsbDps_DpsSubMenuStateOnMsg(
	UINT32 msg,
	UINT32 param
)
{
	switch ( msg)
	{
	case SP5K_MSG_USB_DPS_DISCOVER:	
	case SP5K_MSG_USB_DPS_NOTIFY_JOB_STATUS:	
	case SP5K_MSG_USB_DPS_NOTIFY_DEVICE_STATUS:	
		break;

	default:
		break;
	}	
}


/*-------------------------------------------------------------------------
 *  File Name : appUsbDpsSubMenuState
 *------------------------------------------------------------------------*/
void 
appUsbDpsSubMenuState(
	UINT32 msg,
	UINT32 param
)
{	
	DBG_PRINT("usb dps sub menu: %s : [0x%x] [0x%x]\n",__FUNCTION__,msg,param);
	
	switch (msg) 
	{
	case APP_STATE_MSG_INIT:	
		RPrintf("appUsbDpsSubMenuState");
		appUsbDpsSubMenuStateInit();
		break;
	case APP_STATE_MSG_CLOSE:		
		appStateCloseDone();
		break;
	case APP_UI_MSG_USB_PLUG:	
		sp5kFree(gDpsInfo.PicBuf);
		appStateCloseDone();
		break;
	default:	
		if (IS_APP_KEY_MSG(msg))
		{
			appUsbDps_DpsSubMenuStateOnKey(msg,param);
		}
		else
		{
			appUsbDps_DpsSubMenuStateOnMsg(msg,param);
		}
		break;
	}

} 


/*-------------------------------------------------------------------------
 *  File Name : appDPSMenu_SubMenuChange
 *------------------------------------------------------------------------*/
void appDPSMenu_SubMenuChange( unsigned char bMenuDown)
{
	if (bMenuDown)
	{
		appMenu_SubMenuInfoIncreaseSet(&gDpsSubMenu);
	}
	else
	{
		appMenu_SubMenuInfoDecreaseSet(&gDpsSubMenu);
	}

}
/*-------------------------------------------------------------------------
*  File Name : appDPSMenuFunctionSetCB
*------------------------------------------------------------------------*/
void 
appDPSMenuFunctionSetCB(
	appMenuAction_e action,
	appMenuInfo_t *pmenuCurTab,
	appSubMenuInfo_t *psubMenuItem
)
{
	DBG_PRINT("usb dps sub menu: %s(%d)\n",__FUNCTION__,action);	
	DBG_PRINT("usb dps sub menu: pmenuCurTab->curTab  = %d\n",pmenuCurTab->curTab);
	DBG_PRINT("usb dps sub menu: pmenuCurTab->tab[%d] = %d\n",pmenuCurTab->curTab,pmenuCurTab->tab[pmenuCurTab->curTab]);
	DBG_PRINT("usb dps sub menu: psubMenuItem->curItem  = %d\n",psubMenuItem->curItem);

	if (action == MENU_ACTION_SET)
	{
		_DpsMenuKeyPressSettingChange(pmenuCurTab->curTab,psubMenuItem->curItem);
		_DpsSettingMaintain();
	}
	else if (action == MENU_ACTION_RETURN) 
	{
	}
	else 
	{
	}
}
/*-------------------------------------------------------------------------
*  File Name : _DpsMenuKeyPressSettingChange
*------------------------------------------------------------------------*/

void
_DpsMenuKeyPressSettingChange(
	appMenuSetupMenu_e menuCurTab,
	UINT32 subMenuItem
)
{
	DBG_PRINT("usb dps sub menu: %s (%d,%d)\n",__FUNCTION__, menuCurTab,subMenuItem);

	switch(menuCurTab)
	{
		case DPS_MENU_PRINT:
			gDpsMenuOpt.Print.SelOption= subMenuItem;
			break;
		case DPS_MENU_IMAGE:
			gDpsMenuOpt.Image.SelOption= subMenuItem;
			break;
		case DPS_MENU_QUALITY:
			gDpsMenuOpt.Quality.SelOption= subMenuItem;
			break;
		case DPS_MENU_LAYOUT:
			gDpsMenuOpt.Layout.SelOption= subMenuItem;
			break;
		case DPS_MENU_PAPER_SIZE:
			gDpsMenuOpt.PaperSize.SelOption= subMenuItem;
			break;
		case DPS_MENU_PAPER_TYPE:
			gDpsMenuOpt.PaperType.SelOption= subMenuItem;
			break;
		case DPS_MENU_DATE:
			gDpsMenuOpt.DateTime.SelOption= subMenuItem;
			break;
		case DPS_MENU_FILE_NO:
			gDpsMenuOpt.FileNo.SelOption= subMenuItem;
			break;
		case DPS_MENU_DATETYPE:
			gDpsMenuOpt.DateType.SelOption= subMenuItem;
			break;		
		case DPS_MENU_RESET:
			gDpsMenuOpt.Reset.SelOption= subMenuItem;
			break;
		default :
			break;
	}
}

/*-------------------------------------------------------------------------
*  File Name : _DpsSettingMaintain
*------------------------------------------------------------------------*/
void _DpsSettingMaintain()
{
	switch(gDpsMainMenu.curTab)
	{
		case DPS_MENU_RESET:
			if(gDpsSubMenu.curItem==DPS_RESET_ON)
			{
				gDpsMenuOpt.Print.SelOption= DPS_PRINT_STANDARD;
				gDpsMenuOpt.Image.SelOption= DPS_IMAGE_SIGNAL;
				gDpsMenuOpt.Quality.SelOption= DPS_PAPER_QUALITY_AUTO;
				gDpsMenuOpt.Layout.SelOption= DPS_LAYOUT_AUTO;
				gDpsMenuOpt.PaperSize.SelOption= DPS_PAPER_SIZE_AUTO;
				gDpsMenuOpt.PaperType.SelOption= DPS_PAPER_TYPE_AUTO;
				gDpsMenuOpt.DateTime.SelOption= DPS_DATE_AUTO;
				gDpsMenuOpt.FileNo.SelOption= DPS_FILE_AUTO;
				gDpsMenuOpt.DateType.SelOption= DPS_DATETYPE_YYMMDD;
				gDpsMenuOpt.Reset.SelOption= DPS_RESET_OFF;
				memset(gDpsInfo.PicBuf,0,gDpsImage.totalImg);

			}
			break;	
		case DPS_MENU_PRINT:
			if(gDpsSubMenu.curItem==DPS_PRINT_STANDARD)
			{
				appUsbDps_StandardPrintingConfigSet();
			}
			else if(gDpsSubMenu.curItem==DPS_PRINT_INDEX)
			{
				appUsbDps_IndexPrintingConfigSet();
			}
			else if(gDpsSubMenu.curItem==DPS_PRINT_DPOF)
			{
				enPartial = 0;
				appUsbDps_DPOFPrintingConfigSet();
			}	
			break;	

		case DPS_MENU_IMAGE:
			gDpsInfo.SelPic = selPic; 
			if(gDpsSubMenu.curItem==DPS_IMAGE_SIGNAL)
			{
				if(DpsModePara==DPS_MENU_2ND_SELECT0)
				{
					gDpsInfo.PicBuf[gDpsImage.curImgIdx-1]= gDpsInfo.SelPic ;
				}
			}
			else
			{
 				if(DpsModePara==DPS_MENU_2ND_SELECT0)
 				{
					UINT8 count = 0;
					for (count=0; count<gDpsImage.totalImg; count++)
					{
						gDpsInfo.PicBuf[count]= gDpsInfo.SelPic ;
  					}
				}
 			}
			gDpsInfo.TotalSelPics=appUsbDps_TotalPrintSelImg(&gDpsInfo,gDpsImage.totalImg);
			IsSetImage = 1;
			break;	
			
		case DPS_MENU_QUALITY:
			appUsbDps_PaperQualityConfigSet(&gDpsMenuOpt);
			break;	
		
		case DPS_MENU_LAYOUT:
			appUsbDps_PaperLayoutConfigSet(&gDpsMenuOpt);
			break;	
			
		case DPS_MENU_PAPER_SIZE:
			appUsbDps_PaperSizeConfigSet(&gDpsMenuOpt);
			break;	

		case DPS_MENU_PAPER_TYPE:
			appUsbDps_PaperTypeConfigSet(&gDpsMenuOpt);
			break;	

		case DPS_MENU_DATE:
			appUsbDps_DateTimeConfigSet(&gDpsMenuOpt);
			break;	

		case DPS_MENU_FILE_NO:
			appUsbDps_FileNameConfigSet(&gDpsMenuOpt);
			break;	

		case DPS_MENU_DATETYPE:
			appUsbDps_PaperDataTypeConfigSet(&gDpsMenuOpt);
			break;	

		default:
			break;
	}
}

/*-------------------------------------------------------------------------
*  File Name : _DpsSettingMaintain
*------------------------------------------------------------------------*/
void _DpsAddImageProcess()
{
	if (gDpsInfo.TotalSelPics<= DPS_PARTIAL_PIC_NUM)
	{
		appUsbDps_AddImageSet(0,&gDpsInfo,gDpsImage.totalImg) ;
	}
	else
	{
		UINT32 Tol;
		enPartial = 1;
		Tol = ((gDpsInfo.PatialPics+DPS_PARTIAL_PIC_NUM)<=gDpsImage.totalImg)?DPS_PARTIAL_PIC_NUM:(gDpsImage.totalImg%DPS_PARTIAL_PIC_NUM);
		appUsbDps_AddImageSet(gDpsInfo.PatialPics,&gDpsInfo,Tol) ;
	}
}
/*-------------------------------------------------------------------------
*  File Name : _DpsSettingMaintain
*------------------------------------------------------------------------*/
void appDPSMenu_SubMenuSetKeyProcess()
{
	appUsbDpsSubMenu_SetKeyCBF(MENU_ACTION_SET,&gDpsMainMenu,&gDpsSubMenu);
	if(gDpsMainMenu.curTab==DPS_MENU_IMAGE)
	{
		if(gDpsSubMenu.curItem==DPS_IMAGE_SIGNAL)
		{
			appStateChange(APP_STATE_USB_PRINTIMAGESEL,STATE_PARAM_NORMAL_INIT);
		}
		else
		{
			appStateChange(APP_STATE_USB_PRINT2NDMENU,STATE_PARAM_NORMAL_INIT);
		}
	}
	else if(gDpsMainMenu.curTab==DPS_MENU_PRINT)
	{
		appStateChange(APP_STATE_USB_PRINTING,STATE_PARAM_NORMAL_INIT);
	}
	else
	{
		if(gDpsMainMenu.tab[gDpsMainMenu.curTab - 1]==DPS_MENU_RESET)
		{
			CLEAR_OSD_SRC;
			_errorMsgDraw(ERR_MSG_BUSY_WAIT,TRUE);
			appTimeDelayMs(2000);
			appOsdMessageBoxShow(TRUE,STRTK_COMPLETED);
			appTimeDelayMs(1000);
		}
		appStateChange(APP_STATE_USB_PRINTMENU,STATE_PARAM_NORMAL_INIT);
	}
}

