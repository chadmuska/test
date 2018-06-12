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

#include "app_com_def.h"
#include "app_com_api.h"
#include "common.h"

#include "sp5k_gfx_api.h"

#include "app_osd.h"
#include "app_osd_api.h"
#include "app_menu_frameosd.h"
#include "app_pal.h"
#include "app_osdfont.h"
#include "app_iconfont1.h"
#include "app_icon_def.h"
#include "app_stringid.h"
#include "app_user_setting.h"
#include "app_menu.h"
#include "app_usb_dpslib.h"
#include "app_usb_dps_osd.h"
#include "app_menu_frameosd.h"
#include "app_user_setting.h"
#include "app_playback_osd.h"
#include "app_playback_def.h"

 /**************************************************************************
  * 						  C O N S T A N T S 						   *
 **************************************************************************/

 /**************************************************************************
  * 							 M A C R O S							   *
  **************************************************************************/

 /**************************************************************************
  * 						 D A T A	T Y P E S						   *
  **************************************************************************/

typedef struct DPSSubMenu{
 	unsigned short nMenuChange[2];
	unsigned short nPrint[DPS_PRINT_TOTAL];
	unsigned short nImage[DPS_SELECT_TOTAL];
	unsigned short nQuality[DPS_PAPER_QUALITY_TOTAL];
	unsigned short nLayout[DPS_LAYOUT_TOTAL];
	unsigned short nSize[DPS_PAPER_SIZE_TOTAL];
	unsigned short nType[DPS_PAPER_TYPE_TOTAL];
	unsigned short nDate[DPS_DATE_TOTAL];
	unsigned short nFile[DPS_FILE_TOTAL];
	unsigned short nDateType[DPS_DATETYPE_TOTAL];
	unsigned short nReset[DPS_RESET_TOTAL];
}appmenuDPSMenuItemSelect_t, *PappmenuDPSMenuItemSelect_s;

unsigned short DpsCustomMenuStr[DPS_MENU_TOTAL] =
{
	STRTK_NULL,
	STRTK_STU_PRINT,
	STRTK_STU_IMG,
	STRTK_QUAL,
	STRTK_LAYOUT,
	STRTK_PAPER_SIZE,
	STRTK_PAPER_TYPE,
	STRTK_DATE,
	STRTK_FILE_NO,
	STRTK_SPV_DATESTAMP,
	STRTK_FN_RESET
};

appmenuDPSMenuItemSelect_t sgCustomSubMenuStrId=
{
	{STRTK_NULL, STRTK_NULL},
	{STRTK_STU_STD, STRTK_STU_INDEX, STRTK_PB_DPOF},
	{STRTK_STU_SGL, STRTK_STU_AL},
	{STRTK_AUTO, STRTK_FINE, STRTK_NORMAL, STRTK_DRAFT},
	{STRTK_AUTO, STRTK_LAY_1, STRTK_LAY_2, STRTK_LAY_3,STRTK_LAY_4, STRTK_LAY_5,STRTK_LAY_6,STRTK_LAY_7,STRTK_LAY_8, STRTK_LAY_9, STRTK_LAY_16},//Layout
	{STRTK_AUTO, STRTK_PGS_L, STRTK_PGS_2L, STRTK_POSTCARD,STRTK_100X150, STRTK_4X6, STRTK_8X10,STRTK_LETTER, STRTK_A4,STRTK_A3},
	{STRTK_AUTO, STRTK_PT_PLAIN, STRTK_PHOTO, STRTK_FPHOTO},
	{STRTK_AUTO, STRTK_DATE_OFF, STRTK_DATE_ON},
	{STRTK_AUTO, STRTK_FN_OFF, STRTK_FN_ON},//File No.
	{STRTK_STU_YYMMDD, STRTK_STU_DDMMYY, STRTK_STU_MMDDYY},//File No.
	{STRTK_DPSRESET_YES, STRTK_DPSRESET_NO},
};

appmenuDPSMenuItemSelect_t sgCustomSubMenuIcon=
{
	{ID_ICON_NONE, ID_ICON_NONE},
	{ID_ICON_PRINT_STANDARD, ID_ICON_PRINT_INDEX, ID_ICON_PRINT},
	{ID_ICON_NONE,ID_ICON_NONE},
	{ID_ICON_MODE_PICTBRIDGE, ID_ICON_QUALITY_SUPER_FINE, ID_ICON_QUALITY_FINE, ID_ICON_QUALITY},
	{ID_ICON_MODE_PICTBRIDGE, ID_ICON_PRINT_INDEX,ID_ICON_PRINT_INDEX, ID_ICON_PRINT_INDEX, ID_ICON_PRINT_INDEX, ID_ICON_PRINT_INDEX,ID_ICON_PRINT_INDEX,ID_ICON_PRINT_INDEX,ID_ICON_PRINT_INDEX,ID_ICON_PRINT_INDEX,ID_ICON_PRINT_INDEX},//Layout
	{ID_ICON_MODE_PICTBRIDGE, ID_ICON_RESIZE,ID_ICON_RESIZE, ID_ICON_RESIZE,ID_ICON_RESIZE,ID_ICON_RESIZE,ID_ICON_RESIZE,ID_ICON_RESIZE,ID_ICON_RESIZE,ID_ICON_RESIZE},
	{ID_ICON_MODE_PICTBRIDGE, ID_ICON_QUALITY_SUPER_FINE, ID_ICON_QUALITY_FINE, ID_ICON_QUALITY},//PaperType
	{ID_ICON_MODE_PICTBRIDGE, ID_ICON_OFF, ID_ICON_ON},//Date
	{ID_ICON_MODE_PICTBRIDGE, ID_ICON_OFF, ID_ICON_ON},//File No.
	{ID_ICON_MODE_PICTBRIDGE, ID_ICON_PRINT_STANDARD, ID_ICON_PRINT_STANDARD},
	{ID_ICON_NONE,ID_ICON_NONE},
};

/**************************************************************************
* 						G L O B A L    D A T A						   *
**************************************************************************/
#define DPS_TKMENU_MAIN_STR_X           38
#define DPS_TKMENU_MAIN_STR_Y           48

#define DPS_TKMENU_MAIN_ICON_X          240
#define DPS_TKMENU_MAIN_ICON_Y          48

#define DPS_TKMENU_MAIN_STR_Y_GAP       31

#define DPS_TKMENU_MAIN_STR_BAR_X       23
#define DPS_TKMENU_MAIN_STR_BAR_Y       48

#define DPS_TKMENU_MAIN_BAR_Y_GAP       31

#define DPS_TKSUBMENU_STR_X_OFFSET      24
#define DPS_TKSUBMENU_STR_Y_OFFSET      10
#define DPS_TKSUBMENU_STR_GAP  	        31
#define DPS_TKSUBMENU_FRAM_X_OFFSET     9
#define DPS_TKSUBMENU_FRAM_Y_OFFSET     8
#define DPS_TKSUBMENU_FRAM_GAP          31
#define DPS_TKSUBMENU_STR_ICON_X_OFFSET 104

#define DPS_TKSUBMENU_ICON_X_OFFSET     64
#define DPS_TKSUBMENU_ICON_Y_OFFSET     10
#define DPS_TKSUBMENU_FRAM_2NUMGAP      33
#define DPS_TKSUBMENU_STR_Y_4NUM_OFFSET 13
#define DPS_TKSUBMENU_FRAM_Y_4NUM_OFFSET  11

#define DPS_MENU_PER_PAGE				5
#define DPS_SUBMENU_PER_PAGE	 	    4

unsigned char DPSMenuStart_Index;
unsigned char DPSSubMenuStart_Index;
unsigned char DPSSubMenuLastTab;
extern appMenuInfo_t gDpsMainMenu;
extern appSubMenuInfo_t gDpsSubMenu;

/**************************************************************************
* 				E X T E R N A L    R E F E R E N C E S				   *
**************************************************************************/
extern dpsImage_t gDpsImage;
extern dpsMenuOption_t gDpsMenuOpt ;

/**************************************************************************
* 			  F U N C T I O N	 D E C L A R A T I O N S			   *
**************************************************************************/

/*-------------------------------------------------------------------------
*  File Name : appDPSMenu_DPSMainMenuTitleDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSMainMenuTitleDraw()
{
	UINT8 i,MaxCnt,start_idx=0;
	UINT8 ncuridx = gDpsMainMenu.curTab;
	unsigned short *pIconId=NULL;
	unsigned short *pIconIdColor=NULL;
	unsigned short *pStrId=NULL;
	if (gDpsMainMenu.totalTabs >(DPS_MENU_PER_PAGE+1))
	{
		MaxCnt = DPS_MENU_PER_PAGE;
	}
	else
	{
		MaxCnt = gDpsMainMenu.totalTabs;
	}

	if (ncuridx>0)
	{
		DPSMenuStart_Index = (ncuridx-1)/DPS_MENU_PER_PAGE;
	}
	else
	{
	 	DPSMenuStart_Index = 0;
	}
	start_idx = DPSMenuStart_Index*DPS_MENU_PER_PAGE;

	if (ncuridx == DPS_MENU_CHANGE)
	{
		appOsd_ColorDrawTextColorSet( PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

		for(i=0; i<MaxCnt; i++)
		{
			appDPSMenu_DPSMainMenuSubItemDetailStrSet(gDpsMainMenu.tab[(start_idx+i)], &pStrId, &pIconId, &pIconIdColor);
			if(start_idx+i < gDpsMainMenu.totalTabs)
			{
				appOsdLib_TextIDDraw(DPS_TKMENU_MAIN_STR_X,DPS_TKMENU_MAIN_STR_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,DpsCustomMenuStr[ gDpsMainMenu.tab[(start_idx+i)] ]);
				appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
				if (!(*pIconId==ID_ICON_NONE)&&gDpsMainMenu.tab[(start_idx+i)]!=DPS_MENU_LAYOUT&&gDpsMainMenu.tab[(start_idx+i)]!=DPS_MENU_PAPER_SIZE)
				{
					appOsdLib_OsdLayIconDraw(DPS_TKMENU_MAIN_ICON_X,DPS_TKMENU_MAIN_ICON_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,*(pIconId+appDPSMenu_CurrentSettingGet(gDpsMainMenu.tab[(start_idx+i)])));
				}
			}
		}
	}
	else
	{

		for(i=0; i<MaxCnt; i++)
		{
			if(start_idx+i < gDpsMainMenu.totalTabs)
			{
				appDPSMenu_DPSMainMenuSubItemDetailStrSet(gDpsMainMenu.tab[(start_idx+i)] , &pStrId, &pIconId, &pIconIdColor);

				// selected item
				if(start_idx+i == (ncuridx-1))
				{
					#if (KIT_WITH_HDMI)
					if(IS_HDMI_IN){
						appMenu_TKMenuIconFileDraw(TRUE,DPS_TKMENU_MAIN_STR_BAR_X,DPS_TKMENU_MAIN_STR_BAR_Y+(i*DPS_TKMENU_MAIN_BAR_Y_GAP),TKTYPE_STILLVIEW_MENU_SELBAR_W,TKTYPE_STILLVIEW_MENU_SELBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_MENUSELBAR_HDMI,0x0000);
					}else
					#endif
					{
					appMenu_TKMenuDPSMenuSelectBarFrameDraw(TRUE,DPS_TKMENU_MAIN_STR_BAR_X,DPS_TKMENU_MAIN_STR_BAR_Y+(i*DPS_TKMENU_MAIN_BAR_Y_GAP));
					}
					appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
					appOsdLib_TextIDDraw(DPS_TKMENU_MAIN_STR_X,DPS_TKMENU_MAIN_STR_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,DpsCustomMenuStr[ gDpsMainMenu.tab[(start_idx+i)] ]);
					appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
					if (!(*pIconId==ID_ICON_NONE)&&gDpsMainMenu.tab[(start_idx+i)]!=DPS_MENU_LAYOUT&&gDpsMainMenu.tab[(start_idx+i)]!=DPS_MENU_PAPER_SIZE)
						appOsdLib_OsdLayIconDraw(DPS_TKMENU_MAIN_ICON_X,DPS_TKMENU_MAIN_ICON_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,*(pIconId+appDPSMenu_CurrentSettingGet(gDpsMainMenu.tab[(start_idx+i)])));
				}
				else
				{
					appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
					appOsdLib_TextIDDraw(DPS_TKMENU_MAIN_STR_X,DPS_TKMENU_MAIN_STR_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,DpsCustomMenuStr[ gDpsMainMenu.tab[(start_idx+i)] ]);
					appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
					if (!(*pIconId==ID_ICON_NONE)&&gDpsMainMenu.tab[(start_idx+i)]!=DPS_MENU_LAYOUT&&gDpsMainMenu.tab[(start_idx+i)]!=DPS_MENU_PAPER_SIZE)
						appOsdLib_OsdLayIconDraw(DPS_TKMENU_MAIN_ICON_X,DPS_TKMENU_MAIN_ICON_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,*(pIconId+appDPSMenu_CurrentSettingGet(gDpsMainMenu.tab[(start_idx+i)])));

				}
			}
		}
	}
}

/*-------------------------------------------------------------------------
*  File Name : appDPSMenu_DPSMainMenuTitleDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSMainMenuSubItemDetailStrSet(unsigned char nCurTab, unsigned short **pStrId,unsigned short **pIconId, unsigned short **pIconIdColor)
{
	switch(nCurTab)
	{
		case DPS_MENU_CHANGE:
			*pStrId = sgCustomSubMenuStrId.nMenuChange;
			*pIconId = sgCustomSubMenuIcon.nMenuChange;
			break;

		case DPS_MENU_PRINT:
			*pStrId = sgCustomSubMenuStrId.nPrint;
			*pIconId = sgCustomSubMenuIcon.nPrint;
			break;

		case DPS_MENU_IMAGE:
			*pStrId = sgCustomSubMenuStrId.nImage;
			*pIconId = sgCustomSubMenuIcon.nImage;
			break;

		case DPS_MENU_QUALITY:
			*pStrId = sgCustomSubMenuStrId.nQuality;
			*pIconId = sgCustomSubMenuIcon.nQuality;
			break;

		case DPS_MENU_LAYOUT:
			*pStrId = sgCustomSubMenuStrId.nLayout;
			*pIconId = sgCustomSubMenuIcon.nLayout;
			break;

		case DPS_MENU_PAPER_SIZE:
			*pStrId = sgCustomSubMenuStrId.nSize;
			*pIconId = sgCustomSubMenuIcon.nSize;
			break;

		case DPS_MENU_PAPER_TYPE:
			*pStrId = sgCustomSubMenuStrId.nType;
			*pIconId = sgCustomSubMenuIcon.nType;
			break;

		case DPS_MENU_DATE:
			*pStrId = sgCustomSubMenuStrId.nDate;
			*pIconId = sgCustomSubMenuIcon.nDate;
			break;

		case DPS_MENU_FILE_NO:
			*pStrId = sgCustomSubMenuStrId.nFile;
			*pIconId = sgCustomSubMenuIcon.nFile;
			break;
		case DPS_MENU_DATETYPE:
			*pStrId = sgCustomSubMenuStrId.nDateType;
			*pIconId = sgCustomSubMenuIcon.nDateType;
			break;
		case DPS_MENU_RESET:
			*pStrId = sgCustomSubMenuStrId.nReset;
			*pIconId = sgCustomSubMenuIcon.nReset;
			break;

		default:
			break;
	}
}

/*-------------------------------------------------------------------------
*  File Name : appDPSMenu_CurrentSettingGet
*------------------------------------------------------------------------*/
UINT8 	appDPSMenu_CurrentSettingGet(
unsigned short GetSettingCurTab
)
{
	UINT8 CurSetting=0;
	switch (GetSettingCurTab)
	{
		case DPS_MENU_CHANGE:
			break;
		case DPS_MENU_PRINT:
			CurSetting= gDpsMenuOpt.Print.SelOption;
			break;
		case DPS_MENU_IMAGE:
			CurSetting= gDpsMenuOpt.Image.SelOption;
			break;
		case DPS_MENU_QUALITY:
			CurSetting= gDpsMenuOpt.Quality.SelOption;
			break;
		case DPS_MENU_LAYOUT:
			CurSetting= gDpsMenuOpt.Layout.SelOption;
			break;
		case DPS_MENU_PAPER_SIZE:
			CurSetting= gDpsMenuOpt.PaperSize.SelOption;
			break;
		case DPS_MENU_PAPER_TYPE:
			CurSetting= gDpsMenuOpt.PaperType.SelOption;
			break;
		case DPS_MENU_DATE:
			CurSetting= gDpsMenuOpt.DateTime.SelOption;
			break;
		case DPS_MENU_FILE_NO:
			CurSetting= gDpsMenuOpt.FileNo.SelOption;
			break;
		case DPS_MENU_DATETYPE:
			CurSetting= gDpsMenuOpt.DateType.SelOption;
			break;
		case DPS_MENU_RESET:
			CurSetting= gDpsMenuOpt.Reset.SelOption;
			break;
		default:
			break;
	}
	return CurSetting;
}

/*-------------------------------------------------------------------------
*  File Name : appDPSMenu_CurrentSettingGet
*------------------------------------------------------------------------*/
void appDPSMenu_DPSMainMenuTitleChangeDraw()
{


		UINT8 i,MaxCnt,start_idx=0;
		UINT8 ncuridx = gDpsMainMenu.curTab;
		UINT8 nLastTab= gDpsMainMenu.recentlyTab;
		UINT8 nClearShapeIndex=0;
		unsigned char DPSMenuLast_Index;

		unsigned short *pIconId=NULL;
		unsigned short *pIconIdColor=NULL;
		unsigned short *pStrId=NULL;
		if (gDpsMainMenu.totalTabs >(DPS_MENU_PER_PAGE+1)){
			MaxCnt = DPS_MENU_PER_PAGE;
		}else{
			MaxCnt = gDpsMainMenu.totalTabs;
		}

		// Look for 1st item in detail menu
		DPSMenuLast_Index= DPSMenuStart_Index;

		if (ncuridx>0){
			DPSMenuStart_Index = (ncuridx-1)/DPS_MENU_PER_PAGE;
		}
		start_idx = DPSMenuStart_Index*DPS_MENU_PER_PAGE;

		//Look for Clear shape index
		if (DPSMenuStart_Index!=DPSMenuLast_Index){
			nClearShapeIndex = 0xff;
		}else{
			nClearShapeIndex = nLastTab-(start_idx+1);
		}

		if (nClearShapeIndex ==0xff){
			appMenu_TKMenuDpsMenuFrameDraw(TRUE);
			appMenu_TKMenuScrollBarCursorShow(TRUE,gDpsMainMenu.curTab,gDpsMainMenu.totalTabs);
		}else{
			appMenu_TKMenuViewMenuClearMenuBarFrameDraw(TRUE,DPS_TKMENU_MAIN_STR_BAR_X,DPS_TKMENU_MAIN_STR_BAR_Y+(nClearShapeIndex*DPS_TKMENU_MAIN_BAR_Y_GAP));
		}

	for(i=0; i<MaxCnt; i++){
		if(start_idx+i < gDpsMainMenu.totalTabs){
			appDPSMenu_DPSMainMenuSubItemDetailStrSet(gDpsMainMenu.tab[(start_idx+i)], &pStrId, &pIconId, &pIconIdColor);
			// selected item
			if(start_idx+i == (ncuridx-1)){
				#if (KIT_WITH_HDMI)
				if(IS_HDMI_IN){
					appMenu_TKMenuIconFileDraw(TRUE,DPS_TKMENU_MAIN_STR_BAR_X,DPS_TKMENU_MAIN_STR_BAR_Y+(i*DPS_TKMENU_MAIN_BAR_Y_GAP),TKTYPE_STILLVIEW_MENU_SELBAR_W,TKTYPE_STILLVIEW_MENU_SELBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_MENUSELBAR_HDMI,0x0000);
				}else
				#endif
				{
				appMenu_TKMenuDPSMenuSelectBarFrameDraw(TRUE,DPS_TKMENU_MAIN_STR_BAR_X,DPS_TKMENU_MAIN_STR_BAR_Y+(i*DPS_TKMENU_MAIN_BAR_Y_GAP));
				}
				appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextIDDraw(DPS_TKMENU_MAIN_STR_X,DPS_TKMENU_MAIN_STR_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,DpsCustomMenuStr[ gDpsMainMenu.tab[(start_idx+i)] ]);
				appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
					if (!(*pIconId==ID_ICON_NONE)&&gDpsMainMenu.tab[(start_idx+i)]!=DPS_MENU_LAYOUT&&gDpsMainMenu.tab[(start_idx+i)]!=DPS_MENU_PAPER_SIZE)
						appOsdLib_OsdLayIconDraw(DPS_TKMENU_MAIN_ICON_X,DPS_TKMENU_MAIN_ICON_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,*(pIconId+appDPSMenu_CurrentSettingGet(gDpsMainMenu.tab[(start_idx+i)])));
			}else{
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextIDDraw(DPS_TKMENU_MAIN_STR_X,DPS_TKMENU_MAIN_STR_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,DpsCustomMenuStr[ gDpsMainMenu.tab[(start_idx+i)] ]);
				appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
					if (!(*pIconId==ID_ICON_NONE)&&gDpsMainMenu.tab[(start_idx+i)]!=DPS_MENU_LAYOUT&&gDpsMainMenu.tab[(start_idx+i)]!=DPS_MENU_PAPER_SIZE)
						appOsdLib_OsdLayIconDraw(DPS_TKMENU_MAIN_ICON_X,DPS_TKMENU_MAIN_ICON_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,*(pIconId+appDPSMenu_CurrentSettingGet(gDpsMainMenu.tab[(start_idx+i)])));
			}
		}
	}

}

/*-------------------------------------------------------------------------
*  File Name : appDPSMenu_DPSSubMenuSelHiddenFrameDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuSelHiddenFrameDraw()
{

	appMenu_TKMenuDpsMenuFrameHiddenDraw(TRUE);
	appDPSMenu_DPSSubMenuSelHiddenDraw();
}

/*-------------------------------------------------------------------------
*  File Name : appDPSMenu_DPSSubMenuSelHiddenFrameDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuSelHiddenDraw(
	void
)
{
UINT8 i,MaxCnt,start_idx=0;
UINT8 ncuridx = gDpsMainMenu.curTab;


	if (gDpsMainMenu.totalTabs >(DPS_MENU_PER_PAGE+1)){
		MaxCnt = DPS_MENU_PER_PAGE;
	}else{
		MaxCnt = gDpsMainMenu.totalTabs;
	}


	if (ncuridx>0){
		DPSMenuStart_Index = (ncuridx-1)/DPS_MENU_PER_PAGE;
	} else{
	 	DPSMenuStart_Index = 0;
	}
	start_idx = DPSMenuStart_Index*DPS_MENU_PER_PAGE;


	for(i=0; i<MaxCnt; i++){
		if(start_idx+i < gDpsMainMenu.totalTabs){
			// selected item
			if(start_idx+i == (ncuridx-1)){
				appMenu_TKMenuDPSMenuSelectBarFrameHiddenDraw(TRUE,DPS_TKMENU_MAIN_STR_BAR_X,DPS_TKMENU_MAIN_STR_BAR_Y+(i*DPS_TKMENU_MAIN_BAR_Y_GAP));
				appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextIDDraw(DPS_TKMENU_MAIN_STR_X,DPS_TKMENU_MAIN_STR_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,DpsCustomMenuStr[ gDpsMainMenu.tab[(start_idx+i)]]);
			}else{
				appOsd_ColorDrawTextColorSet(PAL_GRAY_1, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextIDDraw(DPS_TKMENU_MAIN_STR_X,DPS_TKMENU_MAIN_STR_Y+(i*DPS_TKMENU_MAIN_STR_Y_GAP),SP5K_GFX_ALIGN_TOP_LEFT,DpsCustomMenuStr[ gDpsMainMenu.tab[(start_idx+i)]]);
			}
		}
	}
}

/*-------------------------------------------------------------------------
*  File Name : appDPSMenu_DPSSubMenuSelHiddenFrameDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuFrameDraw()
{
	unsigned int StarX=0;
	unsigned int StarY=0;
	switch (gDpsMainMenu.tab[gDpsMainMenu.curTab - 1])
	{
		case DPS_MENU_CHANGE:
			break;

		case DPS_MENU_PRINT:
		case DPS_MENU_DATE:
			StarX = DPS_TKSUBMENU_1ST_FRAME_X;
			StarY = DPS_TKSUBMENU_1ST_FRAME_Y;
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				appMenu_TKMenuIconFileDraw(TRUE,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_3ITEM_W,TKTYPE_STILLVIEW_SUBMENU_3ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENU_3ITEM_HDMI,0x0000);
			}else
			#endif
			{
			appMenu_TKMenuDPSMenuSub3RowFrameDraw(TRUE,StarX,StarY);
			}
			appDPSMenu_DPSSubMenuDetailItemSetandDraw(gDpsMainMenu.tab[gDpsMainMenu.curTab - 1],StarX,StarY);
			break;

		case DPS_MENU_QUALITY:
		case DPS_MENU_PAPER_TYPE:
			StarX = DPS_TKSUBMENU_1ST_FRAME_X;
			StarY = DPS_TKSUBMENU_1ST_FRAME_Y;
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				appMenu_TKMenuIconFileDraw(TRUE,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_4ITEM_W,TKTYPE_STILLVIEW_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENU_4ITEMSC_HDMI,0x0000);
			}else
			#endif
			{
			appMenu_TKMenuDPSMenuSub4RowFrameDraw(TRUE,StarX,StarY);
			}
			appDPSMenu_DPSSubMenuDetailItemSetandDraw(gDpsMainMenu.tab[gDpsMainMenu.curTab - 1],StarX,StarY);
			break;

		case DPS_MENU_LAYOUT:
			StarX = DPS_TKSUBMENU_1ST_FRAME_X;
			StarY = DPS_TKSUBMENU_1ST_FRAME_Y;
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				appMenu_TKMenuIconFileDraw(TRUE,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_4ITEM_W,TKTYPE_STILLVIEW_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENU_4ITEMSC_HDMI,0x0000);
			}else
			#endif
			{
			appMenu_TKMenuDPSpMenuSub4RowWithScrollFrameDraw(TRUE,StarX,StarY);
			}
			appDPSMenu_DPSSubMenuDetailItemSetandDraw(gDpsMainMenu.tab[gDpsMainMenu.curTab - 1],StarX,StarY);
			break;
		case DPS_MENU_PAPER_SIZE:
			StarX = DPS_TKSUBMENU_1ST_FRAME_X;
			StarY = DPS_TKSUBMENU_1ST_FRAME_Y;
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				appMenu_TKMenuIconFileDraw(TRUE,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_4ITEM_W,TKTYPE_STILLVIEW_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENU_4ITEMSC_HDMI,0x0000);
			}else
			#endif
			{
			appMenu_TKMenuDPSpMenuSub4RowWithScrollFrameDraw(TRUE,StarX,StarY);
			}
			appDPSMenu_DPSSubMenuDetailItemSetandDraw(gDpsMainMenu.tab[gDpsMainMenu.curTab - 1],StarX,StarY);
			break;

		case DPS_MENU_IMAGE:
			StarX = DPS_TKSUBMENU_3ST_FRAME_X;
			StarY = DPS_TKSUBMENU_3ST_FRAME_Y;
			appMenu_TKMenuDPSMenuSub2RowFrameDraw(TRUE,StarX,StarY);
			appDPSMenu_DPSSubMenuDetailItemSetandDraw(gDpsMainMenu.tab[gDpsMainMenu.curTab - 1],StarX,StarY);
			break;

		case DPS_MENU_FILE_NO:
			StarX = DPS_TKSUBMENU_5ST_FRAME_X;
			StarY = DPS_TKSUBMENU_5ST_FRAME_Y;
			appMenu_TKMenuDPSMenuSub3RowFrameDraw(TRUE,StarX,StarY);
			appDPSMenu_DPSSubMenuDetailItemSetandDraw(gDpsMainMenu.tab[gDpsMainMenu.curTab - 1],StarX,StarY);
			break;

		case DPS_MENU_DATETYPE:
			StarX = DPS_TKSUBMENU_5ST_FRAME_X;
			StarY = DPS_TKSUBMENU_5ST_FRAME_Y;
			appMenu_TKMenuDPSMenuSub3RowFrameDraw(TRUE,StarX,StarY);
			appDPSMenu_DPSSubMenuDetailItemSetandDraw(gDpsMainMenu.tab[gDpsMainMenu.curTab - 1],StarX,StarY);
			break;

		case DPS_MENU_RESET:
			StarX = DPS_TKSUBMENU_6ST_FRAME_X;
			StarY = DPS_TKSUBMENU_6ST_FRAME_Y;
			appMenu_TKMenuDPSMenuSub2RowFrameDraw(TRUE,StarX,StarY);
			appDPSMenu_DPSSubMenuDetailItemSetandDraw(gDpsMainMenu.tab[gDpsMainMenu.curTab - 1],StarX,StarY);
			break;

		default:
			break;
	}
	appMenu_TKMenuViewSubMenuHelpFrame(TRUE);
}

/*-------------------------------------------------------------------------
*	File Name : appDPSMenu_DPSSubMenuDetailItemSetandDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuDetailItemSetandDraw (unsigned short wTitle,unsigned int StrStarX,unsigned int StrStarY)
{
	unsigned short *pIconId=NULL;
	unsigned short *pIconIdColor=NULL;
	unsigned short *pStrId=NULL;
	appDPSMenu_DPSMainMenuSubItemDetailStrSet(wTitle, &pStrId, &pIconId, &pIconIdColor);
	appDPSMenu_DPSSubMenuDetailInfoDraw(pIconId,pStrId,wTitle,StrStarX,StrStarY);

}

/*-------------------------------------------------------------------------
*	File Name : appDPSMenu_DPSSubMenuDetailInfoDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuDetailInfoDraw(unsigned short *pwIconId, unsigned short *pwStrId,unsigned short wTitle,unsigned int StarX,unsigned int StarY)
{
	switch(wTitle)
	{
		case DPS_MENU_CHANGE:
			break;
		case DPS_MENU_PRINT:
		case DPS_MENU_IMAGE:
		case DPS_MENU_QUALITY:
		case DPS_MENU_LAYOUT:
		case DPS_MENU_PAPER_SIZE:
		case DPS_MENU_PAPER_TYPE:
		case DPS_MENU_DATE:
		case DPS_MENU_FILE_NO:
		case DPS_MENU_DATETYPE:
		case DPS_MENU_RESET:
			appDPSMenu_DPSSubMenuTitleDraw(wTitle,pwStrId,pwIconId,StarX,StarY);
			break;

		default:
			break;
	}
}

/*-------------------------------------------------------------------------
*	File Name : appDPSMenu_DPSSubMenuTitleDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuTitleDraw(unsigned short wTitle,unsigned short *pwStrId,unsigned short *pwIconId,unsigned int StarX,unsigned int StarY)
{
	UINT8 i,MaxCnt,start_idx=0;
	UINT8 ncuridx = gDpsSubMenu.curItem;
	UINT8 curItemSetting=appDPSMenu_CurrentSettingGet(wTitle);
	UINT32 curEnableItem=gDpsSubMenu.osdEnableItems;
	BOOL bIsItemEnable=TRUE;

	unsigned int Str_X,Str_Y;
	unsigned int Icon_X,Icon_Y;
	unsigned int Fram_X,Fram_Y;
	unsigned int HLGap;


	Str_X = StarX+DPS_TKSUBMENU_STR_X_OFFSET;
	Str_Y = StarY+DPS_TKSUBMENU_STR_Y_OFFSET;

	Fram_X = StarX+DPS_TKSUBMENU_FRAM_X_OFFSET;
	Fram_Y = StarY+DPS_TKSUBMENU_FRAM_Y_OFFSET;

	if (!(*pwIconId==ID_ICON_NONE))
		Str_X = StarX+DPS_TKSUBMENU_STR_ICON_X_OFFSET;

	Icon_X = StarX+DPS_TKSUBMENU_ICON_X_OFFSET;
	Icon_Y = StarY+DPS_TKSUBMENU_ICON_Y_OFFSET;
	if (gDpsSubMenu.totalItem== 2){
		HLGap=DPS_TKSUBMENU_FRAM_2NUMGAP;
	}else if (gDpsSubMenu.totalItem>= 4){
		Str_Y = StarY+DPS_TKSUBMENU_STR_Y_4NUM_OFFSET;
		Fram_Y = StarY+DPS_TKSUBMENU_FRAM_Y_4NUM_OFFSET;
		HLGap=DPS_TKSUBMENU_FRAM_GAP;
		Icon_Y = StarY+DPS_TKSUBMENU_STR_Y_4NUM_OFFSET;

	}else{
		HLGap=DPS_TKSUBMENU_FRAM_GAP;
	}


	if (gDpsSubMenu.totalItem>(DPS_SUBMENU_PER_PAGE)){
		MaxCnt = DPS_SUBMENU_PER_PAGE;
	}else{
		MaxCnt = gDpsSubMenu.totalItem;
	}

	if (ncuridx>0){
		DPSSubMenuStart_Index = ncuridx/DPS_SUBMENU_PER_PAGE;
	} else{
	 	DPSSubMenuStart_Index = 0;
	}
	start_idx = DPSSubMenuStart_Index*DPS_SUBMENU_PER_PAGE;

	DPSSubMenuLastTab=gDpsSubMenu.curItem;
	if (wTitle ==DPS_MENU_PAPER_SIZE){
		appMenu_TKSubMenuScrollBarCursorShow(TRUE,gDpsSubMenu.curItem,gDpsSubMenu.totalItem);
	}
	for(i=0; i<MaxCnt; i++){
		if(start_idx+i < gDpsSubMenu.totalItem){
			// selected item
			if(start_idx+i == ncuridx){
				if(wTitle ==DPS_MENU_IMAGE){
					#if (KIT_WITH_HDMI)
					if(IS_HDMI_IN){
						appMenu_TKMenuIconFileDraw(TRUE,Fram_X,Fram_Y+(i*HLGap),TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENUBAR_HDMI,0x0000);
					}else
					#endif
					{
					appMenu_TKMenuDPSMenuSubMenuSlideArrowBarFrame(TRUE,Fram_X,Fram_Y+(i*HLGap));
					}
				}else{
					#if (KIT_WITH_HDMI)
					if(IS_HDMI_IN){
						appMenu_TKMenuIconFileDraw(TRUE,Fram_X,Fram_Y+(i*HLGap),TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENUBAR_HDMI,0x0000);
					}else
					#endif
					{
					appMenu_TKMenuDPSSubMenuSelectFrameDraw(TRUE,Fram_X,Fram_Y+(i*HLGap));
					}
				}
				appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextIDDraw(Str_X,Str_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,pwStrId[start_idx+i]);
				appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
				if (!(*pwIconId==ID_ICON_NONE))
				{
					appOsdLib_OsdLayIconDraw(Icon_X,Icon_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,*(pwIconId+(start_idx+i)));

					if (start_idx+i==curItemSetting)
					{
						appOsdLib_OsdLayIconDraw(StarX+DPS_TKSUBMENU_STR_X_OFFSET,Icon_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ITEM_SELECT);
					}
				}

			}else{
				bIsItemEnable=((curEnableItem>>(start_idx+i))&0x1);
				appOsd_ColorDrawTextColorSet((bIsItemEnable)?PAL_WHITE:PAL_GRAY_1, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextIDDraw(Str_X,Str_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,pwStrId[start_idx+i]);
				appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
				if (!(*pwIconId==ID_ICON_NONE))
				{
					appOsdLib_OsdLayIconDraw(Icon_X,Icon_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,*(pwIconId+(start_idx+i)));
					if (start_idx+i==curItemSetting)
					{
						appOsdLib_OsdLayIconDraw(StarX+DPS_TKSUBMENU_STR_X_OFFSET,Icon_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ITEM_SELECT);
					}
				}
			}
		}
	}
}

/*-------------------------------------------------------------------------
*  File Name : appDPSMenu_DPSSubMenuFrameChangeDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuFrameChangeDraw()
{
	unsigned int StarX=0;
	unsigned int StarY=0;
	switch (gDpsMainMenu.tab[gDpsMainMenu.curTab - 1] )
	{
		case DPS_MENU_CHANGE:
			break;
		case DPS_MENU_PRINT:
		case DPS_MENU_QUALITY:
		case DPS_MENU_LAYOUT:
		case DPS_MENU_PAPER_SIZE:
		case DPS_MENU_PAPER_TYPE:
		case DPS_MENU_DATE:
			StarX = DPS_TKSUBMENU_1ST_FRAME_X;
			StarY = DPS_TKSUBMENU_1ST_FRAME_Y;
			break;
		case DPS_MENU_IMAGE:
			StarX = DPS_TKSUBMENU_3ST_FRAME_X;
			StarY = DPS_TKSUBMENU_3ST_FRAME_Y;
			break;
		case DPS_MENU_DATETYPE:
		case DPS_MENU_FILE_NO:
			StarX = DPS_TKSUBMENU_5ST_FRAME_X;
			StarY = DPS_TKSUBMENU_5ST_FRAME_Y;
			break;
		case DPS_MENU_RESET:
			StarX = DPS_TKSUBMENU_6ST_FRAME_X;
			StarY = DPS_TKSUBMENU_6ST_FRAME_Y;
			break;
		default:
			break;
	}
	appDPSMenu_DPSSubMenuDetailItemChangeSetandDraw(gDpsMainMenu.tab[gDpsMainMenu.curTab - 1],StarX,StarY);
}

/*-------------------------------------------------------------------------
*	File Name : appDPSMenu_DPSSubMenuDetailItemChangeSetandDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuDetailItemChangeSetandDraw(unsigned short wTitle,unsigned int StrStarX,unsigned int StrStarY)
{
	unsigned short *pIconId=NULL;
	unsigned short *pIconIdColor=NULL;
	unsigned short *pStrId=NULL;


	appDPSMenu_DPSMainMenuSubItemDetailStrSet(wTitle, &pStrId, &pIconId, &pIconIdColor);
	appDPSMenu_DPSSubMenuDetailInfoChangeDraw(pIconId,pStrId,wTitle,StrStarX,StrStarY);
}

/*-------------------------------------------------------------------------
*	File Name : appDPSMenu_DPSSubMenuDetailItemChangeSetandDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuDetailInfoChangeDraw(unsigned short *pwIconId, unsigned short *pwStrId,unsigned short wTitle,unsigned int StarX,unsigned int StarY)
{
	switch(wTitle)
	{
		case DPS_MENU_CHANGE:
			break;

		case DPS_MENU_PRINT:
		case DPS_MENU_IMAGE:
		case DPS_MENU_QUALITY:
		case DPS_MENU_LAYOUT:
		case DPS_MENU_PAPER_SIZE:
		case DPS_MENU_PAPER_TYPE:
		case DPS_MENU_DATE:
		case DPS_MENU_FILE_NO:
		case DPS_MENU_DATETYPE:
		case DPS_MENU_RESET:
			appDPSMenu_DPSSubMenuTitleChange(wTitle,pwStrId,pwIconId,StarX,StarY);
			break;

		default:
			break;
	}
}

/*-------------------------------------------------------------------------
*	File Name : appDPSMenu_DPSSubMenuTitleChange
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuTitleChange(unsigned short wTitle,unsigned short *pwStrId,unsigned short *pwIconId, unsigned int StarX,unsigned int StarY)
{
	UINT8 i,MaxCnt,start_idx=0;
	UINT8 ncuridx = gDpsSubMenu.curItem;
	UINT8 nClearShapeIndex=0;
	unsigned char DpsSubMenuLast_Index=0;

	unsigned int Str_X,Str_Y;
	unsigned int Icon_X,Icon_Y;

	unsigned int Fram_X,Fram_Y;
	unsigned int HLGap;
	UINT8 curItemSetting=appDPSMenu_CurrentSettingGet(wTitle);
	UINT32 curEnableItem=gDpsSubMenu.osdEnableItems;

	Str_X = StarX+DPS_TKSUBMENU_STR_X_OFFSET;
	Str_Y = StarY+DPS_TKSUBMENU_STR_Y_OFFSET;

	Fram_X = StarX+DPS_TKSUBMENU_FRAM_X_OFFSET;
	Fram_Y = StarY+DPS_TKSUBMENU_FRAM_Y_OFFSET;

	if (!(*pwIconId==ID_ICON_NONE))
		Str_X = StarX+DPS_TKSUBMENU_STR_ICON_X_OFFSET;

	Icon_X = StarX+DPS_TKSUBMENU_ICON_X_OFFSET;
	Icon_Y = StarY+DPS_TKSUBMENU_ICON_Y_OFFSET;

	if (gDpsSubMenu.totalItem== 2){
		HLGap=DPS_TKSUBMENU_FRAM_2NUMGAP;
	}else if (gDpsSubMenu.totalItem>= 4){
		Str_Y = StarY+DPS_TKSUBMENU_STR_Y_4NUM_OFFSET;
		Fram_Y = StarY+DPS_TKSUBMENU_FRAM_Y_4NUM_OFFSET;
		HLGap=DPS_TKSUBMENU_FRAM_GAP;
		Icon_Y = StarY+DPS_TKSUBMENU_STR_Y_4NUM_OFFSET;

	}else{
		HLGap=DPS_TKSUBMENU_FRAM_GAP;
	}

	if (gDpsSubMenu.totalItem>(DPS_SUBMENU_PER_PAGE)){
		MaxCnt = DPS_SUBMENU_PER_PAGE;
	}else{
		MaxCnt = gDpsSubMenu.totalItem;
	}

	DpsSubMenuLast_Index= DPSSubMenuStart_Index;

	if (ncuridx>0){
		DPSSubMenuStart_Index = ncuridx/DPS_SUBMENU_PER_PAGE;
	} else{
		DPSSubMenuStart_Index = 0;
	}
	start_idx = DPSSubMenuStart_Index*DPS_SUBMENU_PER_PAGE;

	if (DPSSubMenuStart_Index!=DpsSubMenuLast_Index){
		nClearShapeIndex = 0xff;
	}else{
		nClearShapeIndex = DPSSubMenuLastTab-start_idx;
	}

	DPSSubMenuLastTab=gDpsSubMenu.curItem;

	if (nClearShapeIndex ==0xff){
		appDPSMenu_DPSSubMenuFrameReDraw(TRUE,wTitle);
		if (gDpsSubMenu.totalItem>8){
			appMenu_TKMenuDpsSubMenuScrollBarDraw(TRUE,DPSSubMenuStart_Index);
		}else{
			if(DPSSubMenuStart_Index==0){
				appMenu_TKMenuSenceMenuScrollBarDrawType1(TRUE,DPS_SUBMENU_SCROLLBAR_X,DPS_SUBMENU_SCROLLBAR_Y);
			}else if(DPSSubMenuStart_Index==1){
				appMenu_TKMenuSenceMenuScrollBarDrawType2(TRUE,DPS_SUBMENU_SCROLLBAR_X,DPS_SUBMENU_SCROLLBAR_Y);
			}
		}
	}else{
		appMenu_TKMenuDPSSubMenuClearMenuBarFrameDraw(TRUE,Fram_X,Fram_Y+(nClearShapeIndex*HLGap));
	}

	for(i=0; i<MaxCnt; i++){
		if(start_idx+i < gDpsSubMenu.totalItem){
			// selected item
			if(start_idx+i == ncuridx){
				if(wTitle ==DPS_MENU_IMAGE){
					#if (KIT_WITH_HDMI)
					if(IS_HDMI_IN){
						appMenu_TKMenuIconFileDraw(TRUE,Fram_X,Fram_Y+(i*HLGap),TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBSETBAR_HDMI,0x0000);
					}else
					#endif
					{
					appMenu_TKMenuDPSMenuSubMenuSlideArrowBarFrame(TRUE,Fram_X,Fram_Y+(i*HLGap));
					}
				}else{
					#if (KIT_WITH_HDMI)
					if(IS_HDMI_IN){
						appMenu_TKMenuIconFileDraw(TRUE,Fram_X,Fram_Y+(i*HLGap),TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENUBAR_HDMI,0x0000);
					}else
					#endif
					{
					appMenu_TKMenuDPSSubMenuSelectFrameDraw(TRUE,Fram_X,Fram_Y+(i*HLGap));
				}
				}
				appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextIDDraw(Str_X,Str_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,pwStrId[start_idx+i]);
				appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);

				if (!(*pwIconId==ID_ICON_NONE))
				{
					appOsdLib_OsdLayIconDraw(Icon_X,Icon_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,*(pwIconId+(start_idx+i)));

					if (start_idx+i==curItemSetting)
					{
						appOsdLib_OsdLayIconDraw(StarX+DPS_TKSUBMENU_STR_X_OFFSET,Icon_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ITEM_SELECT);
					}
				}
			}else{
				BOOL bIsItemEnable;
				bIsItemEnable=((curEnableItem>>(start_idx+i))&0x1);
				//bIsItemEnable=((curEnableItem>>(start_idx+i))&0x1);
				appOsd_ColorDrawTextColorSet((bIsItemEnable)?PAL_WHITE:PAL_GRAY_1, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextIDDraw(Str_X,Str_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,pwStrId[start_idx+i]);
				appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
				if (!(*pwIconId==ID_ICON_NONE))
				{
					appOsdLib_OsdLayIconDraw(Icon_X,Icon_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,*(pwIconId+(start_idx+i)));
					if (start_idx+i==curItemSetting)
					{
						appOsdLib_OsdLayIconDraw(StarX+DPS_TKSUBMENU_STR_X_OFFSET,Icon_Y+(i*DPS_TKSUBMENU_STR_GAP),SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ITEM_SELECT);
					}
				}
			}
		}
	}
}

/*-------------------------------------------------------------------------
*	File Name : appDPSMenu_DPSSubMenuFrameReDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPSSubMenuFrameReDraw()
{
	unsigned int StarX=0;
	unsigned int StarY=0;
	switch (gDpsMainMenu.tab[gDpsMainMenu.curTab - 1])
	{
		case DPS_MENU_CHANGE:
			break;

		case DPS_MENU_PRINT:
		case DPS_MENU_DATE:
			StarX = DPS_TKSUBMENU_1ST_FRAME_X;
			StarY = DPS_TKSUBMENU_1ST_FRAME_Y;
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				appMenu_TKMenuIconFileDraw(TRUE,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_3ITEM_W,TKTYPE_STILLVIEW_SUBMENU_3ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENU_3ITEM_HDMI,0x0000);
			}else
			#endif
			{
			appMenu_TKMenuDPSMenuSub3RowFrameDraw(TRUE,StarX,StarY);
			}
			break;

		case DPS_MENU_QUALITY:
		case DPS_MENU_PAPER_TYPE:
			StarX = DPS_TKSUBMENU_1ST_FRAME_X;
			StarY = DPS_TKSUBMENU_1ST_FRAME_Y;
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				appMenu_TKMenuIconFileDraw(TRUE,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_4ITEM_W,TKTYPE_STILLVIEW_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENU_4ITEMSC_HDMI,0x0000);
			}else
			#endif
			{
			appMenu_TKMenuDPSMenuSub4RowFrameDraw(TRUE,StarX,StarY);
			}
			break;

		case DPS_MENU_LAYOUT:
			StarX = DPS_TKSUBMENU_1ST_FRAME_X;
			StarY = DPS_TKSUBMENU_1ST_FRAME_Y;
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				appMenu_TKMenuIconFileDraw(TRUE,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_4ITEM_W,TKTYPE_STILLVIEW_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENU_4ITEMSC_HDMI,0x0000);
			}else
			#endif
			{
			appMenu_TKMenuDPSpMenuSub4RowWithScrollFrameDraw(TRUE,StarX,StarY);
			}
			break;
		case DPS_MENU_PAPER_SIZE:
			StarX = DPS_TKSUBMENU_1ST_FRAME_X;
			StarY = DPS_TKSUBMENU_1ST_FRAME_Y;
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				appMenu_TKMenuIconFileDraw(TRUE,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_4ITEM_W,TKTYPE_STILLVIEW_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENU_4ITEMSC_HDMI,0x0000);
			}else
			#endif
			{
			appMenu_TKMenuDPSpMenuSub4RowWithScrollFrameDraw(TRUE,StarX,StarY);
			}
			break;

		case DPS_MENU_IMAGE:
			StarX = DPS_TKSUBMENU_3ST_FRAME_X;
			StarY = DPS_TKSUBMENU_3ST_FRAME_Y;
			appMenu_TKMenuDPSMenuSub2RowFrameDraw(TRUE,StarX,StarY);
			break;

		case DPS_MENU_FILE_NO:
			StarX = DPS_TKSUBMENU_5ST_FRAME_X;
			StarY = DPS_TKSUBMENU_5ST_FRAME_Y;
			appMenu_TKMenuDPSMenuSub3RowFrameDraw(TRUE,StarX,StarY);
			break;

		case DPS_MENU_DATETYPE:
			StarX = DPS_TKSUBMENU_5ST_FRAME_X;
			StarY = DPS_TKSUBMENU_5ST_FRAME_Y;
			appMenu_TKMenuDPSMenuSub3RowFrameDraw(TRUE,StarX,StarY);
			break;

		case DPS_MENU_RESET:
			StarX = DPS_TKSUBMENU_6ST_FRAME_X;
			StarY = DPS_TKSUBMENU_6ST_FRAME_Y;
			appMenu_TKMenuDPSMenuSub2RowFrameDraw(TRUE,StarX,StarY);
			break;

		default:
			break;
	}
}

/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuItenSetandDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPS2ndMenuItenSetandDraw( unsigned short wTitle,  unsigned short SelectIndex)
{
	switch (wTitle)
	{
		default:
		case DPS_MENU_CHANGE:
		case DPS_MENU_DATE:
		case DPS_MENU_QUALITY:
		case DPS_MENU_LAYOUT:
		case DPS_MENU_PAPER_SIZE:
		case DPS_MENU_PAPER_TYPE:
		case DPS_MENU_PRINT:
		case DPS_MENU_FILE_NO:
		case DPS_MENU_DATETYPE:
		case DPS_MENU_RESET:
			break;
		case DPS_MENU_IMAGE:
			appPlayOsd_DlgConfirmYesNoConfig(STRTK_SET_THIS_PHO,SelectIndex);
			appPlayOsd_DlgBoxShow();
			appDPSMenu_DPS2ndMenuHelpItemDraw(TRUE);
		break;
	}
}

/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuHelpItemDraw
*------------------------------------------------------------------------*/
void
appDPSMenu_DPS2ndMenuHelpItemDraw(BOOL bShow)
{
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);

	appOsdLib_OsdLayIconDraw(TKTYPE_SSNAPSHAPE_MENU_OKSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_OK);
	appOsdLib_TextIDDraw(TKTYPE_SSNAPSHAPE_MENU_OKSTART_X+TKTYPE_MENUICON_OFFSET_X+2, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10,SP5K_GFX_ALIGN_TOP_LEFT, STRTK_SET);
	appPlayOsd_ModeIconDraw(bShow);
	pbMainCB_t *pmainCb;
	pmainCb = appPb_MainCbPtrGet();
	if(pmainCb->totalImg>0){
		appPLayOsd_LeftRightArrow(bShow);
	}
	appPlayOsd_FileNumDraw(bShow,gDpsImage.curImgIdx,gDpsImage.totalImg);
}

#if 0
/*-------------------------------------------------------------------------
*	  File Name : apDPSMenu_DPS2ndMenuHelpItemDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPS3ndMenuImageItemDraw(unsigned char Enable,unsigned short SelectIndex, UINT16 pics)
{

	apDPSMenu_DPS3ndMenuImageFrameDraw();
	DpsOSD_BQMenu3ndDrawImageTitle();
	DpsOSD_BQMenu3ndDrawImageDetail(SelectIndex,pics);
}
#endif
/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuImageFrameDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPS2ndMenuImageFrameDraw(unsigned short SelectIndex,UINT16 Num)
{
	char szStr[8];

	/*   BACKGROUND*/
	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_GRAY_1);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, DPS_TK2NDMENU_TITLE_FRAME_X, DPS_TK2NDMENU_TITLE_FRAME_Y, SP5K_GFX_ALIGN_TOP_LEFT,ID_DLGBOX_TXT_TITLE_ICON, 0x0000);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, DPS_TK2NDMENU_TITLE_FRAME_X, DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_OPTION_TXT_ICON, 0x0000);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, DPS_TK2NDMENU_TITLE_FRAME_X, DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y+DPS_TK2NDMENU_TITLE_CENTER_FRAME_H, SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_OPTION_BOTTOM_ICON, 0x0000);
	/*	TITLE  */
	appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(DPS_TK2NDMENU_TITLE_STR_X,DPS_TK2NDMENU_TITLE_STR_Y, SP5K_GFX_ALIGN_CENTER, STRTK_STU_PICTBRIDGE);

	/* BOTTOM HELP BAR */
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayIconDraw(TKTYPE_SSNAPSHAPE_MENU_OKSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_OK);
	appOsdLib_TextIDDraw(TKTYPE_SSNAPSHAPE_MENU_OKSTART_X+TKTYPE_MENUICON_OFFSET_X+2, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10,SP5K_GFX_ALIGN_TOP_LEFT, STRTK_SET);

	/* Icon */
	appOsdLib_OsdLayIconDraw(DPS_TK2NDMENU_PRINT_ICON_X, DPS_TK2NDMENU_PRINT_ICON_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_PRINT);


	if(SelectIndex==DPS_MENU_2ND_SELECT0){
		/* ARROW*/
		appOsdLib_OsdLayIconDraw(DPS_TK2NDMENU_LEFT_ARROW_X, DPS_TK2NDMENU_LEFT_ARROW_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_LEFT);
		appOsdLib_OsdLayIconDraw(DPS_TK2NDMENU_RIGHT_ARROW_X, DPS_TK2NDMENU_RIGHT_ARROW_Y,	SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_RIGHT);

		appOsd_ColorDrawShapeColorSet(PAL_YELLOW_1, PAL_BLEND_100, PAL_YELLOW_1, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_SELBAR_X,DPS_TK2NDMENU_SELBAR_Y,DPS_TK2NDMENU_SELBAR_W,25, 0, 0);
		sprintf(szStr, "%02d", Num);
		appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(DPS_TK2NDMENU_NUM_X, DPS_TK2NDMENU_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szStr);
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, DPS_TK2NDMENU_TITLE_FRAME_X, DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y+DPS_TK2NDMENU_TITLE_CENTER_FRAME_H, SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_OPTION_BOTTOM_ICON, 0x0000);
		/*	 back string */
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextIDDraw(DPS_TK2NDMENU_STR_BACK_X,DPS_TK2NDMENU_STR_BACK_Y, SP5K_GFX_ALIGN_TOP_LEFT, STRTK_PB_BACK);
	}else{
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_LEFT_ARROW_X,DPS_TK2NDMENU_SELBAR_Y,100,25, 0, 0);
		sprintf(szStr, "%02d", Num);
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(DPS_TK2NDMENU_NUM_X, DPS_TK2NDMENU_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szStr);
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, DPS_TK2NDMENU_STR_BACK_X-15, DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y+DPS_TK2NDMENU_TITLE_CENTER_FRAME_H+5, SP5K_GFX_ALIGN_TOP_LEFT, "A:\\RO_RES\\UI\\ICON\\MU2NDBAR.SFN", 0x0000);
		/*	 back string */
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextIDDraw(DPS_TK2NDMENU_STR_BACK_X,DPS_TK2NDMENU_STR_BACK_Y, SP5K_GFX_ALIGN_TOP_LEFT, STRTK_PB_BACK);
	}
}

/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuImageFrameDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPS2ndMenuImageSelChangeDraw(unsigned short SelectIndex,UINT16 Num)
{
	if(SelectIndex==DPS_MENU_2ND_SELECT0){
		char szStr[8];
		appOsd_ColorDrawShapeColorSet(PAL_YELLOW_1, PAL_BLEND_100, PAL_YELLOW_1, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_SELBAR_X,DPS_TK2NDMENU_SELBAR_Y,DPS_TK2NDMENU_SELBAR_W,25, 0, 0);
		sprintf(szStr, "%02d", Num);
		appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(DPS_TK2NDMENU_NUM_X, DPS_TK2NDMENU_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szStr);
	}

}

/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuImageFrameDraw
*------------------------------------------------------------------------*/
void appDPSMenu_DPS2ndMenuImageUpDownChangeDraw(unsigned short SelectIndex,UINT16 Num)
{
	char szStr[8];


	if(SelectIndex==DPS_MENU_2ND_SELECT0){
		appOsdLib_OsdLayIconDraw(DPS_TK2NDMENU_LEFT_ARROW_X, DPS_TK2NDMENU_LEFT_ARROW_Y,  SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_LEFT);
		appOsdLib_OsdLayIconDraw(DPS_TK2NDMENU_RIGHT_ARROW_X, DPS_TK2NDMENU_RIGHT_ARROW_Y,	SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_RIGHT);

		appOsd_ColorDrawShapeColorSet(PAL_YELLOW_1, PAL_BLEND_100, PAL_YELLOW_1, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_SELBAR_X,DPS_TK2NDMENU_SELBAR_Y,DPS_TK2NDMENU_SELBAR_W,25, 0, 0);
		sprintf(szStr, "%02d", Num);
		appOsd_ColorDrawTextColorSet(PAL_BLACK, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(DPS_TK2NDMENU_NUM_X, DPS_TK2NDMENU_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szStr);
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_STR_BACK_X-15,DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y+DPS_TK2NDMENU_TITLE_CENTER_FRAME_H+5,264,26, 0, 0);

	}else{
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_50, PAL_GRAY_1, PAL_BLEND_50);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_TK2NDMENU_LEFT_ARROW_X,DPS_TK2NDMENU_SELBAR_Y,130,25, 0, 0);
		sprintf(szStr, "%02d", Num);
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(DPS_TK2NDMENU_NUM_X, DPS_TK2NDMENU_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)szStr);
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, DPS_TK2NDMENU_STR_BACK_X-15, DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y+DPS_TK2NDMENU_TITLE_CENTER_FRAME_H+5, SP5K_GFX_ALIGN_TOP_LEFT, "A:\\RO_RES\\UI\\ICON\\MU2NDBAR.SFN", 0x0000);
	}

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(DPS_TK2NDMENU_STR_BACK_X,DPS_TK2NDMENU_STR_BACK_Y, SP5K_GFX_ALIGN_TOP_LEFT, STRTK_PB_BACK);
}

/*-------------------------------------------------------------------------
*	 File Name : appDPSMenu_DPS2ndMenuImageFrameDraw
*------------------------------------------------------------------------*/
#define DPS_DPOF_ICON_X								6
#define DPS_DPOF_ICON_Y								45
void
appDPSMenu_DpsOSDIconDraw
(
	UINT8 bShow
)
{
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DPS_DPOF_ICON_X, DPS_DPOF_ICON_Y, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
	if (bShow)
		appOsdLib_OsdLayIconDraw(DPS_DPOF_ICON_X, DPS_DPOF_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MODE_PICTBRIDGE);
}
