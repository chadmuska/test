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
#include "sp5k_global_api.h"
#include "sp5k_dcf_api.h"
#include "sp5k_rsvblk_api.h"

#include "app_playback_private.h"
#include "app_playback_def.h"
#include "app_icon_def.h"
#include "app_stringid.h"
#include "app_jpg_draw.h"
#include "app_osdfont.h"
#include "app_playback_osd.h"
#include "app_exif.h"
#include "app_view_param_def.h"
#include "app_error_msg.h"
#include "app_menu_frameosd.h"
#include "app_osd_api.h"
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
static pbDlgBoxInfo_t gpbDlgCb;
static UINT16 PBLastProgBarLength;	//For progress bar drawing in P/B mode.
pbFilePageCtrl_t gpbFilePageCrl;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_HighlightDraw
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_HighlightDraw(
	SINT32 iconX, SINT32 iconY,  UINT32 iconID,
	SINT32 textX, SINT32 textY,  UINT32 textID,
	COLOR_DEF textColor
)
{
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);

	appOsdLib_OsdLayIconDraw(iconX, iconY,  SP5K_GFX_ALIGN_TOP_LEFT, iconID);

	appOsd_ColorDrawTextColorSet(textColor, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(textX, textY,SP5K_GFX_ALIGN_TOP_LEFT, textID);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNoteDraw
 *------------------------------------------------------------------------*/
static
void
appPbOsd_AuxNoteDraw(
	SINT32 iconX, SINT32 iconY,  UINT32 iconID,
	SINT32 textX, SINT32 textY,  UINT32 textID
)
{
	appPbOsd_AuxNote_HighlightDraw(
		iconX, iconY, iconID,
		textX, textY, textID, PAL_WHITE);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_LeftDraw
 *------------------------------------------------------------------------*/
static
void
appPbOsd_AuxNote_LeftDraw(
	BOOL bShow,
	UINT32 iconId,
	UINT32 textId
)
{
	/* UINT32 textW; */
	if(bShow)
	{
		appPbOsd_AuxNoteDraw(
			/*-- Icon */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X,
			TKTYPE_SNAPSHAPE_MENU_OKSTART_Y,
			iconId,
			/*-- Text */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X+TKTYPE_MENUICON_OFFSET_X+2,
			TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10,
			textId);

	}
	else
	{
		UINT32 textW;
		textW = appOsdLib_FontGetStringWidthbyId(textId);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,
									TKTYPE_SSNAPSHAPE_MENU_OKSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y,
									PB_AUXNOTE_ICON_WIDTH+textW, PB_AUXNOTE_ICON_HEIGHT, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_RightDraw
 *------------------------------------------------------------------------*/
static
void
appPbOsd_AuxNote_RightDraw(
	BOOL bShow,
	UINT32 iconId,
	UINT32 textId
)
{
	/* UINT32 textW; */

	if (!bShow) {
		UINT32 textW;
		textW = appOsdLib_FontGetStringWidthbyId(textId);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,
									TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y,
									PB_AUXNOTE_ICON_WIDTH+textW+10, PB_AUXNOTE_ICON_HEIGHT, 0, 0);
		return;
	}
	appPbOsd_AuxNoteDraw(
		/*-- Icon */
		TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X,
		TKTYPE_SNAPSHAPE_MENU_OKSTART_Y,
		iconId,
		/*-- Text */
		TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X+TKTYPE_MENUICON_OFFSET_X+10,
		TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10,
		textId);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_CenterDraw
 *------------------------------------------------------------------------*/
static
void
appPbOsd_AuxNote_CenterDraw(
	BOOL bShow,
	UINT32 iconId,
	UINT32 textId
)
{
	UINT32 textW;
	SINT32 iconX, iconY;

	textW = appOsdLib_FontGetStringWidthbyId(textId);
	iconX = (320-textW-PB_AUXNOTE_ICON_WIDTH)>>1;
	iconY = TKTYPE_SNAPSHAPE_MENU_OKSTART_Y;
	if (!bShow) {
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,
									iconX, iconY,
									PB_AUXNOTE_ICON_WIDTH+textW, PB_AUXNOTE_ICON_HEIGHT+4, 0, 0);
		return;
	}

	appPbOsd_AuxNoteDraw(
		/*-- Icon */
		iconX,
		iconY,
		iconId,
		/*-- Text */
		iconX+TKTYPE_MENUICON_OFFSET_X,
		iconY+10,
		textId);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKPause
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKPause(
	BOOL bShow
)
{
	appPbOsd_AuxNote_CenterDraw(bShow, ID_ICON_BUTTON_OK, STRTK_PAUSE);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKPlay
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKPlay(
	BOOL bShow
)
{
	appPbOsd_AuxNote_CenterDraw(bShow, ID_ICON_BUTTON_OK, STRTK_PLAY);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKStop
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKStop(
	BOOL bShow
)
{
	appPbOsd_AuxNote_LeftDraw(bShow, ID_ICON_BUTTON_OK, STRTK_STOP);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKRec
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKRec(
	BOOL bShow
)
{
	appPbOsd_AuxNote_LeftDraw(bShow, ID_ICON_BUTTON_OK, STRTK_REC);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKSet
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKSet(
	BOOL bShow
)
{
	appPbOsd_AuxNote_LeftDraw(bShow, ID_ICON_BUTTON_OK, STRTK_SET);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKSelect
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKSelect(
	BOOL bShow
)
{
	appPbOsd_AuxNote_LeftDraw(bShow, ID_ICON_BUTTON_OK, STRTK_SEL);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_MenuCancel
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_MenuCancel(
	BOOL bShow
)
{
	appPbOsd_AuxNote_RightDraw(bShow, ID_ICON_BUTTON_MENU, STRTK_CANCEL);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_MenuBack
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_MenuBack(
	BOOL bShow
)
{
	appPbOsd_AuxNote_RightDraw(bShow, ID_ICON_BUTTON_MENU, STRTK_PB_BACK);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_OKDispPhoto
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_OKDispPhoto(
	BOOL bShow
)
{
	appPbOsd_AuxNote_LeftDraw(bShow, ID_ICON_BUTTON_OK, STRTK_DISPLAY_FILE);
}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_TrashCanDelet
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_TrashCanDelet(
	BOOL bShow,
	BOOL bActive
)
{
	/* UINT32 textW; */

	if (!bShow)
	{
		UINT32 textW;
		textW = appOsdLib_FontGetStringWidthbyId(STRTK_PB_DELETE);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, PB_AUXNOTE_ICON_WIDTH+textW, PB_AUXNOTE_ICON_HEIGHT, 0, 0);
		return;
	}

	if (bActive)
	{
		/*-- Set Green Color for TEXT */
		appPbOsd_AuxNote_HighlightDraw(
			/*-- Icon */
			TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, ID_ICON_DELETE,
			/*-- Text */
			TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X+TKTYPE_MENUICON_OFFSET_X+10, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10, STRTK_PB_DELETE,
			/*-- Highligh Color */
			PAL_YELLOW_1);
	}
	else
	{
		/*-- Set Normal (White) for TEXT */
		appPbOsd_AuxNoteDraw(
			/*-- Icon */
			TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, ID_ICON_DELETE,
			/*-- Text */
			TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X+TKTYPE_MENUICON_OFFSET_X+10, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10, STRTK_PB_DELETE);
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appPbOsd_AuxNote_LockProtect
 *------------------------------------------------------------------------*/
void
appPbOsd_AuxNote_LockProtect(
	BOOL bShow,
	BOOL bActive
)
{
	/* UINT32 textW; */

	if (!bShow) {
		UINT32 textW;
		textW = appOsdLib_FontGetStringWidthbyId(STRTK_PB_PROTECT);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, PB_AUXNOTE_ICON_WIDTH+textW, PB_AUXNOTE_ICON_HEIGHT, 0, 0);
		return;
	}

	if (bActive) {
		/*-- Set Green Color for TEXT */
		appPbOsd_AuxNote_HighlightDraw(
			/*-- Icon */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, ID_ICON_PROTECT,
			/*-- Text */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X+TKTYPE_MENUICON_OFFSET_X+2, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10, STRTK_PB_PROTECT,
			/*-- Highligh Color */
			PAL_YELLOW_1);
	} else {
		/*-- Set Normal (White) for TEXT */
		appPbOsd_AuxNoteDraw(
			/*-- Icon */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y, ID_ICON_PROTECT,
			/*-- Text */
			TKTYPE_SSNAPSHAPE_MENU_OKSTART_X+TKTYPE_MENUICON_OFFSET_X+2, TKTYPE_SNAPSHAPE_MENU_OKSTART_Y+10, STRTK_PB_PROTECT);
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxCBGet
 *------------------------------------------------------------------------*/
pbDlgBoxInfo_t
*appPlayOsd_DlgBoxCBGet(
	void
)
{
	return &gpbDlgCb;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxSelectSet
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxSelectSet(
	UINT32 selIndex
)
{
	gpbDlgCb.lastActIndex = gpbDlgCb.actIndex = selIndex;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxSelectGet
 *------------------------------------------------------------------------*/
UINT8
appPlayOsd_DlgBoxSelectGet(
	void
)
{
	return gpbDlgCb.actIndex;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxStatusGet
 *------------------------------------------------------------------------*/
UINT32 appPlayOsd_DlgBoxStatusGet(void)
{
	return gpbDlgCb.dlgStatus;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxStatusSet
 *------------------------------------------------------------------------*/
void appPlayOsd_DlgBoxStatusSet(UINT32 status)
{
	 gpbDlgCb.dlgStatus = status;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxClear
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxClear(
	void
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, gpbDlgCb.boxX, gpbDlgCb.boxY, gpbDlgCb.boxW, gpbDlgCb.boxH, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxTitleSet
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxTitleSet(
	UINT32 titleStrId
)
{
	gpbDlgCb.titleStrId = titleStrId;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxTitleUpdate
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxTitleUpdate(
	UINT32 titleStrId
)
{
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	appPlayOsd_DlgBoxTitleSet(titleStrId);
	appPlayOsd_DlgBoxShow();
	#if 0
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, gpbDlgCb.boxX, gpbDlgCb.boxY, SP5K_GFX_ALIGN_TOP_LEFT, ID_DLGBOX_TXT_TITLE_ICON, 0x0000);
	appOsdLib_TextIDDraw(gpbDlgCb.titleX, gpbDlgCb.titleY, SP5K_GFX_ALIGN_BOTTOM, gpbDlgCb.titleStrId);
	#endif
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxActiveBarChange
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxActiveBarChange(
	void
)
{

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsd_ColorIconColorSet(PAL_GRAY_1, PAL_GRAY_1);
	appPlayOsd_DlgBoxShow();
	#if 0
	/*-- Inactivate last option */
	index = gpbDlgCb.lastActIndex;
	//DBG_PRINT("\n==> lastActIndex (%d) ..\n\n", index);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, (320-DLGBOX_HL_BAR_W)>>1, gpbDlgCb.optY[index], SP5K_GFX_ALIGN_TOP_LEFT, RO_RES_PATH"UI\\ICON\\MU2CLBAR.SFN", 0x0000);
	appOsdLib_TextIDDraw(gpbDlgCb.optX[index], gpbDlgCb.optY[index], SP5K_GFX_ALIGN_TOP_LEFT, gpbDlgCb.optionStrID[index]);

	/*-- Activate current option */
	index = gpbDlgCb.actIndex;
	//DBG_PRINT("\n==> actIndex (%d) ..\n\n", index);
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, (320-DLGBOX_HL_BAR_W)>>1, gpbDlgCb.optY[index], SP5K_GFX_ALIGN_TOP_LEFT, RO_RES_PATH"UI\\ICON\\MU2NDBAR.SFN", 0x0000);
	appOsdLib_TextIDDraw(gpbDlgCb.optX[index], gpbDlgCb.optY[index], SP5K_GFX_ALIGN_TOP_LEFT, gpbDlgCb.optionStrID[index]);
	#endif
	appOsd_ColorIconColorSet(PAL_GRAY_1, PAL_NOCOLOR);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxScroll
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxScroll(
	BOOL isUp
)
{
	gpbDlgCb.lastActIndex = gpbDlgCb.actIndex;

	if (isUp)
	{
		PB_INDEX_DEC(gpbDlgCb.actIndex, 0, gpbDlgCb.numOptions-1);
	}
	else
	{
		PB_INDEX_INC(gpbDlgCb.actIndex, 0, gpbDlgCb.numOptions-1);
	}

	appPlayOsd_DlgBoxActiveBarChange();
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxShow
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxShow(
	void
)
{
	UINT8 *fileName;
	switch(gpbDlgCb.numOptions)
	{
		case 2:
    		#if (KIT_WITH_HDMI)
    		if(IS_HDMI_IN){
    			fileName=RES_ICON_MU2ND2_HDMI;
    		}else
    		#endif
    		{
    			fileName=RES_ICON_MU2ND2;
    		}
			break;
		case 3:
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				fileName=RES_ICON_MU2ND3_HDMI;
			}else
			#endif
			{
			fileName=RES_ICON_MU2ND3;
			}
			break;
		case 4:
			fileName=RES_ICON_MU2ND4;
			break;
		default:
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				fileName=RES_ICON_MU2ND2_HDMI;
			}else
			#endif
			{
			fileName=RES_ICON_MU2ND2;
			}
			break;
	}

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_GRAY_1);
	appOsd_ColorBlendingSet(PAL_GRAY_1, PAL_BLEND_75);

	/*-- Title ICON & TEXT */
	__FUNC_TRACK__;
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, gpbDlgCb.boxX, gpbDlgCb.boxY, SP5K_GFX_ALIGN_TOP_LEFT, fileName, 0x0000);
	}else
	#endif
	{
	appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, gpbDlgCb.boxX, gpbDlgCb.boxY, SP5K_GFX_ALIGN_TOP_LEFT, fileName, 0x0000);
	}

	if (gpbDlgCb.titleStrId)
	{
		appOsdLib_TextIDDraw(gpbDlgCb.titleX, gpbDlgCb.titleY, SP5K_GFX_ALIGN_BOTTOM, gpbDlgCb.titleStrId);
	}

	/*-- Options TEXT & ICON, except the last */
	int i;
	for (i=0; i<gpbDlgCb.numOptions; i++)
	{
		/*-- Show marker ICON */
		if (i==gpbDlgCb.actIndex)
		{
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (320-DLGBOX_HL_BAR_W)>>1, gpbDlgCb.optY[i]+5, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)RES_ICON_MUSUSBAR_HDMI, 0x0000);
			}else
			#endif
			{
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (320-DLGBOX_HL_BAR_W)>>1, gpbDlgCb.optY[i]-1, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)"A:\\RO_RES\\UI\\ICON\\MUDLGBAR.SFN", 0x0000);
		}
		}
		appOsdLib_TextIDDraw(gpbDlgCb.optX[i], gpbDlgCb.optY[i]+5, SP5K_GFX_ALIGN_TOP_LEFT, gpbDlgCb.optionStrID[i]);
	}

//	appOsd_ColorIconColorSet(PAL_GRAY_1, PAL_NOCOLOR);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxActiveBarChange
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxActiveBarChangeSet(
	UINT32 DlgStartX,
	UINT32 DlgStartY,
	UINT32 DlgFirstItemX,
	UINT32 DlgFirstItemY,
	UINT32 DlgItemGap
)
{
	UINT8 index;


	/*-- Inactivate last option */
	index = gpbDlgCb.lastActIndex;
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR,PAL_NOCOLOR,PAL_NOCOLOR,PAL_BLEND_100);
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,DlgStartX,DlgStartY+(index+1)*DlgItemGap,291,31,0,0);

	appOsd_ColorDrawTextColorSet(PAL_GRAY_1, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(DlgFirstItemX ,DlgFirstItemY+index*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,gpbDlgCb.optionStrID[index]);

	/*-- Activate current option */
	index = gpbDlgCb.actIndex;
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		#if 0
		appMenu_TKMenuIconFileDraw(TRUE,DlgStartX,DlgStartY+((index+1)*DlgItemGap),TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENUBAR_HDMI,0x0000);
		#endif
	}else
	#endif
	{
	#if 0
	appMenu_TKMenuPBSubMenuSelectFrameDraw(TRUE,DlgStartX,DlgStartY+((index+1)*DlgItemGap));
	#endif
	}

    appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
    appOsdLib_OsdLayIconDraw(DlgFirstItemX-24,DlgFirstItemY+index*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ITEM_SELECT);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(DlgFirstItemX ,DlgFirstItemY+index*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,gpbDlgCb.optionStrID[index]);
}


/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxScrollSet
 *------------------------------------------------------------------------*/

void
appPlayOsd_DlgBoxScrollSet(
	BOOL isUp,
	UINT32 DlgStartX,
	UINT32 DlgStartY,
	UINT32 DlgFirstItemX,
	UINT32 DlgFirstItemY,
	UINT32 DlgItemGap
)
{
	gpbDlgCb.lastActIndex = gpbDlgCb.actIndex;

	if (isUp) {
		PB_INDEX_DEC(gpbDlgCb.actIndex, 0, gpbDlgCb.numOptions-1);
	} else {
		PB_INDEX_INC(gpbDlgCb.actIndex, 0, gpbDlgCb.numOptions-1);
	}

	appPlayOsd_DlgBoxActiveBarChangeSet(DlgStartX,DlgStartY,DlgFirstItemX,DlgFirstItemY,DlgItemGap);
}


/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxSetandShow()
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxSetandShow(
	UINT32 DlgStartX,
	UINT32 DlgStartY,
	UINT32 DlgFirstItemX,
	UINT32 DlgFirstItemY,
	UINT32 DlgItemGap
)
{
	UINT8 i;
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(DlgStartX ,DlgStartY,SP5K_GFX_ALIGN_TOP_LEFT,gpbDlgCb.titleStrId);
	for(i=0;i<gpbDlgCb.numOptions;i++)
	{
		if(gpbDlgCb.actIndex==i)
		{
			#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
                #if 0
				appMenu_TKMenuIconFileDraw(TRUE,DlgStartX,DlgFirstItemY+i*DlgItemGap,TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_SUBMENUBAR_HDMI,0x0000);
                #endif
			}else
			#endif
			{
			#if 0
			appMenu_TKMenuPBSubMenuSelectFrameDraw(TRUE,DlgStartX,DlgFirstItemY+i*DlgItemGap);
			#endif
			}
			appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
			appOsdLib_OsdLayIconDraw(DlgFirstItemX-24,DlgFirstItemY+i*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_ITEM_SELECT);	 //Ethan
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_TextIDDraw(DlgFirstItemX ,DlgFirstItemY+i*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,gpbDlgCb.optionStrID[i]);
		}
		else
		{
			appOsd_ColorDrawTextColorSet(PAL_GRAY_1, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
			appOsdLib_TextIDDraw(DlgFirstItemX ,DlgFirstItemY+i*DlgItemGap,SP5K_GFX_ALIGN_TOP_LEFT,gpbDlgCb.optionStrID[i]);
		}
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgBoxConfig
 *------------------------------------------------------------------------*/
void
appPlayOsd_DlgBoxConfig(
	UINT32 titleStrId,
	UINT32 numOptions,
	UINT32 *poptionStrID,
	UINT32 actIndex,
	SINT32 yOffset
)
{
	/* int i; */

	memset(&gpbDlgCb, 0, sizeof(pbDlgBoxInfo_t));

	gpbDlgCb.titleStrId = titleStrId;
	gpbDlgCb.numOptions = (numOptions>DLGBOX_OPTION_NUM_MAX) ? DLGBOX_OPTION_NUM_MAX : numOptions;
	gpbDlgCb.actIndex = (actIndex>=DLGBOX_OPTION_NUM_MAX) ? DLGBOX_OPTION_NUM_MAX-1 : actIndex;
	gpbDlgCb.lastActIndex = gpbDlgCb.actIndex;
	gpbDlgCb.dlgStatus = DLGSTATUS_SHOW;
	/*-- Construct coordinates & dimension */

	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		int i;
		gpbDlgCb.boxW = DLGBOX_WIDTH;
		gpbDlgCb.boxH = ((gpbDlgCb.numOptions-1)*DLGBOX_DATA_CONT_H)+DLGBOX_DATA_LAST_H;
		gpbDlgCb.boxH += ((STRTK_NULL==titleStrId) ? DLGBOX_NULL_TITLE_H : DLGBOX_COLOR_TITLE_H);
		gpbDlgCb.boxX = (320-gpbDlgCb.boxW)>>1;
		gpbDlgCb.boxY = ((240-gpbDlgCb.boxH)>>1);
		gpbDlgCb.boxY += yOffset;
		gpbDlgCb.titleX = 150;
		gpbDlgCb.titleY = (STRTK_NULL==titleStrId) ? gpbDlgCb.boxY+DLGBOX_NULL_TITLE_H : gpbDlgCb.boxY+DLGBOX_COLOR_TITLE_H-8;
		UINT32 optX = gpbDlgCb.boxX+32;
		UINT32 optY = gpbDlgCb.titleY;
		for (i=0; i<numOptions; i++)
		{
			gpbDlgCb.optionStrID[i] = *(poptionStrID+i);
			gpbDlgCb.optX[i] = optX;
			gpbDlgCb.optY[i] = optY+(i*HDMI_BOX_DATA_CONT_H);
		}
	}else
	#endif
	{
		int i;
    	gpbDlgCb.boxW = DLGBOX_WIDTH;
    	gpbDlgCb.boxH = ((gpbDlgCb.numOptions-1)*DLGBOX_DATA_CONT_H)+DLGBOX_DATA_LAST_H;
    	gpbDlgCb.boxH += ((STRTK_NULL==titleStrId) ? DLGBOX_NULL_TITLE_H : DLGBOX_COLOR_TITLE_H);
    	gpbDlgCb.boxX = (320-gpbDlgCb.boxW)>>1;
    	gpbDlgCb.boxY = ((240-gpbDlgCb.boxH)>>1);
    	gpbDlgCb.boxY += yOffset;
    	gpbDlgCb.titleX = 160;	/*-- title text is BOTTOM-CENTER aligned */
    	gpbDlgCb.titleY = (STRTK_NULL==titleStrId) ? gpbDlgCb.boxY+DLGBOX_NULL_TITLE_H : gpbDlgCb.boxY+DLGBOX_COLOR_TITLE_H;
    	UINT32 optX = gpbDlgCb.boxX+32;
    	UINT32 optY = gpbDlgCb.titleY+4;
    	for (i=0; i<numOptions; i++)
    	{
    		gpbDlgCb.optionStrID[i] = *(poptionStrID+i);
    		gpbDlgCb.optX[i] = optX;
    		gpbDlgCb.optY[i] = optY+(i*DLGBOX_DATA_CONT_H);
    	}
    }
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DlgConfirmYesNoConfig
 *------------------------------------------------------------------------*/
static UINT32 strID_DlgConfirmYesNo[2] = {STRTK_YES, STRTK_NO};
void
appPlayOsd_DlgConfirmYesNoConfig(
	UINT32 strId,
	UINT32 actIdx
)
{
	appPlayOsd_DlgBoxConfig(strId, 2, strID_DlgConfirmYesNo, actIdx, 20);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_IconDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_IconDraw
(
	UINT8 bShow,
	SINT32 X,
	SINT32 Y,
	UINT32 IconID
)
{
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, X, Y, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
	if (bShow)
	{
		appOsdLib_OsdLayIconDraw(X, Y, SP5K_GFX_ALIGN_TOP_LEFT, IconID);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ProtectDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_ProtectDraw(
	UINT8 bShow
)
{
	appPlayOsd_IconDraw(bShow, PROTTECT_ICON_X, PROTTECT_ICON_Y, ID_ICON_PROTECT);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DPOFDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_DPOFDraw(
	UINT8 bShow
)
{
	appPlayOsd_IconDraw(bShow, DPOF_ICON_X, DPOF_ICON_Y, ID_ICON_MODE_PICTBRIDGE);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VoiceMemoDraw
 *------------------------------------------------------------------------*/
void appPlayOsd_VoiceMemoDraw(void)
{
	if(gpbImgCb.fileType==PB_FT_JPG_MEMO)
	{
		appPlayOsd_IconDraw(TRUE, VOICE_MEMO_ICON_X, VOICE_MEMO_ICON_Y, ID_ICON_MODE_VOICE);
	}
	else
	{
		appPlayOsd_IconDraw(FALSE, VOICE_MEMO_ICON_X, VOICE_MEMO_ICON_Y, ID_ICON_MODE_VOICE);
	}
}

/*-------------------------------------------------------------------------
*  File Name : appPlayOsd_VideoModeDraw
*------------------------------------------------------------------------*/
void
appPlayOsd_VideoModeDraw(
 UINT8 bShow
)
{
    appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_MODE_VIDEO);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOSD_JPGImageIconDraw
*------------------------------------------------------------------------*/
void
appPlayOSD_JPGImageIconDraw(
	UINT8 bShow,
	pbImgExifInfo_t *psPBCurImgExif
 )
 {
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
	if (bShow)
	{
		switch(psPBCurImgExif->sceneMode)
		{
			case SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_STANDARD:
				if (psPBCurImgExif->expProgMode== SP5K_EXIF_TAGDATA_EXPPROG_MANUAL)
				{
					appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_PASM_MANUAL);
				}
				else if (psPBCurImgExif->expProgMode == SP5K_EXIF_TAGDATA_EXPPROG_APERTURE)
				{
					appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_PASM_APERTURE);
				}
				else if (psPBCurImgExif->expProgMode == SP5K_EXIF_TAGDATA_EXPPROG_SHUTTER)
				{
					appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_PASM_TV);
				}
				else
				{
					appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_PASM_PROGRAM);
				}
				break;
			case SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_PORTRAIT:
				appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_SCENE_HISO_PORTRAIT);
				break;
			case SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_LANDSCANPE:
				appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_SCENE_LANDSCAPE);
				break;
			case SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_NIGHTSCENE:
				appPlayOsd_IconDraw(bShow, CAPTURED_MODE_ICON_X, CAPTURED_MODE_ICON_Y, ID_ICON_SCENE_NIGHTPORTRAIT);
				break;
			case SP5K_EXIF_TAGDATA_SCENECAPTURETYPE_OTHER:
			default:
				break;
		}
	}
}

 /*-------------------------------------------------------------------------
  *  File Name : appPlayOsd_ResolutionDraw
  *------------------------------------------------------------------------*/
void
appPlayOsd_ResolutionDraw(
	UINT8 bShow,
	UINT16 Resolution
)
{

	UINT16 ResolutionModeIcon[] =
	{
		[PB_IMG_SIZE_GRADE_0	] = ID_ICON_RESOLUTION_VGA,
		[PB_IMG_SIZE_GRADE_1	] = ID_ICON_RESOLUTION_1M,
		[PB_IMG_SIZE_GRADE_2	] = ID_ICON_RESOLUTION_2M,
		[PB_IMG_SIZE_GRADE_3	] = ID_ICON_RESOLUTION_3M,
		[PB_IMG_SIZE_GRADE_4   	] = ID_ICON_RESOLUTION_4M,
		[PB_IMG_SIZE_GRADE_5	] = ID_ICON_RESOLUTION_5M,
		[PB_IMG_SIZE_GRADE_6	] = ID_ICON_RESOLUTION_6M,
		[PB_IMG_SIZE_GRADE_7  	] = ID_ICON_RESOLUTION_7M,
		[PB_IMG_SIZE_GRADE_8   	] = ID_ICON_RESOLUTION_8M,
		[PB_IMG_SIZE_GRADE_9	] = ID_ICON_RESOLUTION_9M,
		[PB_IMG_SIZE_GRADE_10	] = ID_ICON_RESOLUTION_10M,
		[PB_IMG_SIZE_GRADE_12	] = ID_ICON_RESOLUTION_12M,
		[PB_IMG_SIZE_GRADE_14	] = ID_ICON_RESOLUTION_14M,
		[PB_IMG_SIZE_GRADE_16	] = ID_ICON_RESOLUTION_16M,
		[PB_IMG_SIZE_GRADE_20	] = ID_ICON_RESOLUTION_20M,
		[PB_IMG_SIZE_GRADE_TOTAL] = 0,
	};

	if (Resolution > PB_IMG_SIZE_GRADE_TOTAL)
	{
		bShow = FALSE;
	}
	appPlayOsd_IconDraw(bShow, RESOLUTION_ICON_X, RESOLUTION_ICON_Y, ResolutionModeIcon[Resolution]);
}

void appPlayOsd_VideoSizeDraw(UINT8 bShow,UINT32 width)
{
	switch(width)
	{
		case 320:
			appPlayOsd_IconDraw(bShow,RESOLUTION_ICON_X, RESOLUTION_ICON_Y, ID_ICON_MOVIE_SIZE_320);
			break;
		case 640:
			appPlayOsd_IconDraw(bShow,RESOLUTION_ICON_X, RESOLUTION_ICON_Y,ID_ICON_MOVIE_SIZE_640);
			break;
		case 1280:
			appPlayOsd_IconDraw(bShow, RESOLUTION_ICON_X, RESOLUTION_ICON_Y, ID_ICON_MOVIE_SIZE_720P);
			break;
		case 1920:
			appPlayOsd_IconDraw(bShow, RESOLUTION_ICON_X, RESOLUTION_ICON_Y, ID_ICON_MOVIE_SIZE_1080P);
			break;
		default:
			break;
	}

}

/*-------------------------------------------------------------------------
 *	File Name : appPlayOsd_WhiteBalanceDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_WhiteBalanceDraw(
	UINT8 bShow,
	UINT16 WhiteBalanceMode,
	unsigned int LightSource
)
{
	UINT16 WhiteBalanceModeIcon = 0;
	switch(WhiteBalanceMode)
	{
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_UNKNOWN:
			WhiteBalanceModeIcon = ID_ICON_WB_AUTO;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_MANUAL:
			WhiteBalanceModeIcon = ID_ICON_WB_CUSTOM;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT:
			WhiteBalanceModeIcon = ID_ICON_WB_DAYLIGHT;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_FLUORESCENT:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAYLIGHT_FLUORESCENT:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_WHITE_FLUORESCENT:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_STANDARD_LIGHT_A:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_STANDARD_LIGHT_B:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_STANDARD_LIGHT_C:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_D55:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_D65:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_D75:
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_D50:
			WhiteBalanceModeIcon = ID_ICON_WB_FLUORESCENT;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_CLOUDY_WEATHER:
			WhiteBalanceModeIcon = ID_ICON_WB_CLOUDY;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_ISO_STUDIO_TUNGSTEN:
			WhiteBalanceModeIcon = ID_ICON_WB_TUNGSTEN;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_DAY_WHITE_FLUORESCENT:
			WhiteBalanceModeIcon = ID_ICON_WB_FLUORESCENT_H;
			break;
		case SP5K_EXIF_TAGDATA_LIGHTSOURCE_COOL_WHITE_FLUORESCENT:
			WhiteBalanceModeIcon = ID_ICON_WB_FLUORESCENT_L;
			break;
		default:
			WhiteBalanceModeIcon = ID_ICON_WB_AUTO;
			break;
	}
	appPlayOsd_IconDraw(bShow, WHITE_BALANCE_ICON_X, WHITE_BALANCE_ICON_Y, WhiteBalanceModeIcon);
}

/*-------------------------------------------------------------------------
 *	File Name : appPlayOsd_ExposureDraw
 *------------------------------------------------------------------------*/
 void
appPlayOsd_ExposureDraw(
	UINT8 bShow,
	SINT32 ExpBias
)
{

	UINT16 ExplosureModeIcon[] =
	{
		[0 ] = ID_ICON_EV_N20,
		[1 ] = ID_ICON_EV_N20,
		[2 ] = ID_ICON_EV_N17,
		[3 ] = ID_ICON_EV_N13,
		[4 ] = ID_ICON_EV_N10,
		[5 ] = ID_ICON_EV_N07,
		[6 ] = ID_ICON_EV_N03,
		[7 ] = ID_ICON_EV_00,
		[8 ] = ID_ICON_EV_P03,
		[9 ] = ID_ICON_EV_P07,
		[10] = ID_ICON_EV_P10,
		[11] = ID_ICON_EV_P13,
		[12] = ID_ICON_EV_P17,
		[13] = ID_ICON_EV_P20,
		[14] = ID_ICON_EV_P20,
	};

	int ExpIconIdex = (int)(ExpBias / 33) + 7;

	if (ExpIconIdex > 14)
	{
		bShow = FALSE;
		ExpIconIdex = 14;
	}
	appPlayOsd_IconDraw(bShow, EXPLOSURE_ICON_X, EXPLOSURE_ICON_Y, ExplosureModeIcon[ExpIconIdex]);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ISODraw
 *------------------------------------------------------------------------ */
void
appPlayOsd_ISODraw(
	UINT8 bShow,
	UINT16 ISOMode
	)
{

	UINT16 ISOType = 0;
	switch (ISOMode)
		{
		case 50:
			ISOType = ID_ICON_ISO_50;
			break;
		case 100:
			ISOType = ID_ICON_ISO_100;
			break;
		case 200:
			ISOType = ID_ICON_ISO_200;
			break;
		case 400:
			ISOType = ID_ICON_ISO_400;
			break;
		case 800:
			ISOType = ID_ICON_ISO_800;
			break;
		case 1600:
			ISOType = ID_ICON_ISO_1600;
			break;
		default:
			bShow = FALSE;
			break;
		}
	appPlayOsd_IconDraw(bShow, ISO_ICON_X, ISO_ICON_Y, ISOType);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_StrobeModeDraw
 *------------------------------------------------------------------------*/

void
appPlayOsd_StrobeModeDraw(
	UINT8 bShow,
	UINT16 StrobeMode
	)
{

	UINT16 FlashType = 0;
	switch (StrobeMode)
	{
		case EXIF_VALUE_FLASH_RED_EYE:
			FlashType = ID_ICON_FLASH_REDEYE_ON;
			break;
		case EXIF_VALUE_FLASH_SLOW_SYNC:
			FlashType = ID_ICON_FLASH_SLOW_SYNC;
			break;
		case EXIF_VALUE_FLASH_AUTO_FIRED:
			FlashType = ID_ICON_FLASH_AUTO;
			break;
		case EXIF_VALUE_FLASH_FORCE_ON:
			FlashType = ID_ICON_FLASH_ON;
			break;
		case EXIF_VALUE_FLASH_FORCE_OFF:
		case EXIF_VALUE_FLASH_AUTO_NO_FIRED:
		case EXIF_VALUE_FLASH_OFF:
			FlashType = ID_ICON_FLASH_OFF;
			break;
		default:
			bShow = FALSE;
			break;
	}
	appPlayOsd_IconDraw(bShow,STROBE_MODE_ICON_X, STROBE_MODE_ICON_Y,FlashType);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_MeteringModeDraw
 *------------------------------------------------------------------------*/

void
appPlayOsd_MeteringModeDraw(
	UINT8 bShow,
	UINT16 MeteringMode
	)
{

	UINT16 MeteringModeIcon = 0;
	switch (MeteringMode)
	{
		case SP5K_EXIF_TAGDATA_METERMODE_CENTERWEIGHTED:
			MeteringModeIcon = ID_ICON_METERING_CENTER;
			break;
		case SP5K_EXIF_TAGDATA_METERMODE_SPOT:
			MeteringModeIcon = ID_ICON_METERING_SPOT;
			break;
		case SP5K_EXIF_TAGDATA_METERMODE_MULTISPOT:
			MeteringModeIcon = ID_ICON_METERING_MULTI;
			break;
		default:
			bShow = FALSE;
			break;
	}

	appPlayOsd_IconDraw(bShow, METERING_MODE_ICON_X, METERING_MODE_ICON_Y, MeteringModeIcon);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_DateTimeDraw
 *-----------------------------------------------------------------------*/
void
appPlayOsd_DateTimeDraw (
	UINT8 bShow,
	UINT32 DateTimeFormat,
	sp5kDcfTime_t *FileTime
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	if(pUiSetting->pbfileType ==PB_FILETYPE_IMAGE)
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 12,210,200,28, 0, 0);
	}
	else
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 12,185,200,28, 0, 0);
	}

	if (bShow)
	{
		UINT8 DateString[12], TimeString[10];
		memset(DateString, 0, 12);
		memset(TimeString, 0, 10);
		if (DateTimeFormat == DATE_TIME_YYMMDD)
		{
			sprintf((char*)DateString, "%04d.%02d.%02d", FileTime->year ,FileTime->mon, FileTime->day);
		}
		else if (DateTimeFormat == DATE_TIME_DDMMYY)
		{
			sprintf((char*)DateString, "%02d.%02d.%04d", FileTime->day, FileTime->mon, FileTime->year);
		}
		else
		{
			sprintf((char*)DateString, "%02d.%02d.%04d", FileTime->mon, FileTime->day, FileTime->year);
		}
		sprintf((char*)TimeString, "%02d:%02d", FileTime->hour, FileTime->min);
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

		if(pUiSetting->pbfileType ==PB_FILETYPE_IMAGE)
		{
			appOsdLib_TextDraw(12, 210, SP5K_GFX_ALIGN_TOP_LEFT, DateString);
			appOsdLib_TextDraw(130, 210, SP5K_GFX_ALIGN_TOP_LEFT, TimeString);
		}
		else
		{
			appOsdLib_TextDraw(12, 185, SP5K_GFX_ALIGN_TOP_LEFT, DateString);
			appOsdLib_TextDraw(130, 185, SP5K_GFX_ALIGN_TOP_LEFT, TimeString);
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_FileNumDraw			Brian Modified
 *------------------------------------------------------------------------*/
void
appPlayOsd_FileNumDraw(
	UINT8 bShow,
	UINT32	CurrntIndex,
	UINT32	Total
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, LCD_SIZE_X - ALPHABET_FONT_SIZE_X * 12, 0, ALPHABET_FONT_SIZE_X * 12, ALPHABET_FONT_SIZE_Y+5, 0, 0);
	if (bShow)
	{
		char FileNumString[10];
		memset(FileNumString, 0, 10);
		sprintf(FileNumString, "%u/%u ", CurrntIndex, Total);
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
        #if (KIT_WITH_HDMI)
        if (IS_HDMI_IN) {
            appOsdLib_TextDraw(LCD_SIZE_X-10, PLAYBACK_ICON_Y, SP5K_GFX_ALIGN_TOP_RIGHT, (UINT8*)FileNumString);
        }
        else
        #endif
        {
            appOsdLib_TextDraw(LCD_SIZE_X, PLAYBACK_ICON_Y, SP5K_GFX_ALIGN_TOP_RIGHT, (UINT8*)FileNumString);
        }
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_CurrntFileNameDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_CurrntFileNameDraw(
	BOOL bShow,
	UINT8* CurrntFileName
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);

    #if SP5K_CDFS_OPT
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_FILE_NAME_CDFS_X, PB_FILE_NAME_Y, PB_FILE_NAME_SIZE_X, ALPHABET_FONT_SIZE_Y, 0, 0);
    #else
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_FILE_NAME_X, PB_FILE_NAME_Y, PB_FILE_NAME_SIZE_X, ALPHABET_FONT_SIZE_Y, 0, 0);
    #endif

	if (bShow)
	{
	 	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
        #if SP5K_CDFS_OPT
		appOsdLib_TextDraw(PB_FILE_NAME_CDFS_X, PB_FILE_NAME_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)CurrntFileName);
        #else
		appOsdLib_TextDraw(PB_FILE_NAME_X, PB_FILE_NAME_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)CurrntFileName);
        #endif
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_CurrntFileSizeDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_CurrntFileSizeDraw(
	BOOL bShow,
	UINT32 FileSize
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	if(pUiSetting->pbfileType ==PB_FILETYPE_IMAGE)
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 210, 210, 110, ALPHABET_FONT_SIZE_Y, 0, 0);
	}
	else
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 200, 185, 120, ALPHABET_FONT_SIZE_Y, 0, 0);
	}

	if (bShow)
	{
		unsigned char  FileSizeString[15];
		FileSize = FileSize / 10000;
		sprintf((char*)FileSizeString, "%u.%02u MB", FileSize / 100, FileSize % 100);
	 	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		if(pUiSetting->pbfileType ==PB_FILETYPE_IMAGE)
		{
			appOsdLib_TextDraw(210, 210, SP5K_GFX_ALIGN_TOP_LEFT, FileSizeString);
		}
		else
		{
			appOsdLib_TextDraw(200, 185, SP5K_GFX_ALIGN_TOP_LEFT, FileSizeString);
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : ExposureStringGet
 *------------------------------------------------------------------------*/
void
ExposureStringGet(
	UINT32 Exp0,
	UINT32 Exp1,
	char *outstr)
{
	if (Exp0 > 999 || Exp1 > 9999 || Exp0 == 0 || Exp1 == 0) {
		strcpy(outstr, "0/0");
	}
	else if ((Exp0 == 1) && (Exp1 > 1)) {
		sprintf(outstr, "%u/%us", Exp0, Exp1);
	}
	else if ((Exp0 >= 1) && (Exp1 == 1)) {
		sprintf(outstr, "%us", Exp0);
	}
	else if ((Exp0 >= 1) && (Exp1 == 10)) {
		sprintf(outstr, "%u.%us", Exp0/10, Exp0%10);
	} else {
		sprintf(outstr, "%u/%u", Exp0, Exp1);

	}

}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_CurrntExpFNumDraw
 *  
 *  Redmine: 2522
 *  Jason: avoid overflow. 
 *
 *------------------------------------------------------------------------*/
#define MAXLEN_NUMSTR	35

void
appPlayOsd_CurrntExpFNumDraw(
	UINT8 bShow,
	pbImgExifInfo_t *psPBCurImgExif
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_F_NUM_X, PB_F_NUM_Y, PB_EXP_F_NUM_SIZE_X, ALPHABET_FONT_SIZE_Y, 0, 0);
	if (bShow)
	{
		char ExpString[9];
		char FNumString[MAXLEN_NUMSTR];
		unsigned int szFNum[2];
		memset(ExpString,  0, sizeof(ExpString));
		memset(FNumString, 0, sizeof(FNumString));
		memset(szFNum,     0, sizeof(szFNum));
		if(psPBCurImgExif->fNum[1])
		{
			szFNum[0] = (psPBCurImgExif->fNum[0] / psPBCurImgExif->fNum[1]);
			szFNum[1] = (psPBCurImgExif->fNum[0] % psPBCurImgExif->fNum[1]) * 10 / psPBCurImgExif->fNum[1];
		}
		sprintf(FNumString, "F%u.%u", szFNum[0], szFNum[1]);
	 	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw(PB_F_NUM_X, PB_F_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)FNumString);
		ExposureStringGet(psPBCurImgExif->expTime[0], psPBCurImgExif->expTime[1], (char*)ExpString);
		appOsdLib_TextDraw(PB_F_NUM_X + (6 * ALPHABET_FONT_SIZE_X) , PB_F_NUM_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)ExpString);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_HistogramDraw
 *------------------------------------------------------------------------*/
#define PB_RECT_leftupper_x	    175
#define PB_RECT_leftupper_y	    56
#define PB_HISTOGRAM_SHAPE_HEIGHT	118
void
appPlayOsd_HistogramDraw(
	UINT8 bShow,
	UINT32 *pLuma
)
{
	int i;
	int bar_width;
	/* int bar_Hight; */

	if (bShow==FALSE)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_HISTOGRAM_X, PB_HISTOGRAM_Y, PB_HISTOGRAM_SIZE_X, PB_HISTOGRAM_SIZE_Y, 0, 0);
		return;
	}

	 //bar_width = PB_HISTOGRAM_RECT_WIDTH / bBinNum;
	bar_width = (PB_HISTOGRAM_SIZE_X ) / PB_HISTOGRAM_BAR_NUM;
	UINT32 Luma[PB_HISTOGRAM_SIZE_X];
	if (pLuma==NULL)
	{
		pLuma = Luma;
		if (appPbHistogramGet(pLuma,PB_HISTOGRAM_SIZE_X,PB_HISTOGRAM_SIZE_Y)!=SUCCESS)
		{
			return;
		}
	}

	bar_width = (PB_HISTOGRAM_SIZE_X ) / PB_HISTOGRAM_BAR_NUM;
	appOsd_ColorDrawShapeColorSet(PAL_GRAY_1, PAL_BLEND_75, PAL_GRAY_2, PAL_BLEND_75);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_HISTOGRAM_X, PB_HISTOGRAM_Y, PB_HISTOGRAM_SIZE_X, PB_HISTOGRAM_SIZE_Y, 0, 0);

	appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);

	for(i = 0; i < PB_HISTOGRAM_BAR_NUM; i++)
	{
		int bar_Hight;
		if(*pLuma > PB_HISTOGRAM_Y)
		{
			*pLuma = PB_HISTOGRAM_Y;
		}
		bar_Hight = PB_HISTOGRAM_Y-(PB_HISTOGRAM_SIZE_Y - *pLuma);
		if (bar_Hight !=0)
		{
			//OSDLib_DrawShape(OSDLIB_SHAPE_RECT, x + i*bar_width,
			//	y + PB_HISTOGRAM_SHAPE_HEIGHT - bar_Hight,
			//	bar_width, bar_Hight, 0, 0);
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_HISTOGRAM_X  + (i * bar_width), PB_HISTOGRAM_Y + PB_HISTOGRAM_SIZE_Y  - *pLuma, bar_width, *pLuma, 0, 0);
		}
		pLuma++;
	}

}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VoiceMemoCntrlDraw
 *------------------------------------------------------------------------*/

void
appPlayOsd_MenuIconDraw(
	UINT8 bShow
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, MENU_ICON_X, MENU_ICON_Y - APPPB_ICON_SIZE_Y + 3, APPPB_ICON_SIZE_X + (OSD_FONT_CONST_WIDTH * 3), APPPB_ICON_SIZE_Y, 0, 0);
	if (bShow)
	{
		appOsdLib_OsdLayIconDraw(MENU_ICON_X, MENU_ICON_Y, SP5K_GFX_ALIGN_BOTTOM_LEFT, ID_ICON_BUTTON_MENU);
		appOsdLib_TextIDDraw(MENU_ICON_X + APPPB_ICON_SIZE_X + OSD_FONT_CONST_WIDTH/3, MENU_ICON_Y + 4, SP5K_GFX_ALIGN_BOTTOM_LEFT, STRTK_PB_BACK);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VoiceMemoCntrlDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_VoiceMemoCntrlDraw(
	UINT8 bShow,
	UINT8 isPlay
)
{
	appPlayOsd_IconDraw(bShow, INDICATE_ARROW_UP_ICON_X, INDICATE_ARROW_UP_ICON_Y+2, ID_ICON_BUTTON_UP);
	if (isPlay)
	{
		appPlayOsd_IconDraw(bShow, INDICATE_PLAY_ICON_X, INDICATE_PLAY_ICON_Y, ID_ICON_VIDEOPLAY_START);
	}
	else
	{
		appPlayOsd_IconDraw(bShow, INDICATE_PLAY_ICON_X, INDICATE_PLAY_ICON_Y, ID_ICON_VIDEOPLAY_PAUSE);
	}

}
void
appPlayOsd_VideoTrimCntrlDraw(
	UINT8 bShow,
	UINT8 item
)
{
	appPlayOsd_IconDraw(bShow, INDICATE_ARROW_UP_ICON_X, INDICATE_ARROW_UP_ICON_Y+2, ID_ICON_BUTTON_UP);
	if (item==0)
	{
		appPlayOsd_IconDraw(bShow, INDICATE_PLAY_ICON_X, INDICATE_PLAY_ICON_Y, ID_ICON_VIDEOPLAY_START);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VoiceMemoCntrlDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_VideoCntrlDraw(
	UINT8 bShow,
	UINT8 isPlay,
	UINT8 bShowRWD,
	UINT8 bShowFWD
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, 200, 280, 40, 0, 0);
	appPlayOsd_ShowFileTime(FALSE,gpbImgCb.mediaTotalTime);
	appPlayOsd_IconDraw(bShowRWD, INDICATE_ARROW_UP_ICON_X - APPPB_ICON_SIZE_X*2+3, INDICATE_ARROW_UP_ICON_Y+2, ID_ICON_BUTTON_LEFT);
	appPlayOsd_IconDraw(bShowRWD, INDICATE_ARROW_UP_ICON_X - APPPB_ICON_SIZE_X-5, INDICATE_ARROW_UP_ICON_Y, ID_ICON_VIDEOPLAY_REW);
	appPlayOsd_VoiceMemoCntrlDraw(bShow, isPlay);
	appPlayOsd_IconDraw(bShowFWD, INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X-4-5, INDICATE_ARROW_UP_ICON_Y+2, ID_ICON_BUTTON_RIGHT);
	appPlayOsd_IconDraw(bShowFWD, INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X*2-15, INDICATE_PLAY_ICON_Y, ID_ICON_VIDEOPLAY_FF);
	appPlayOsd_IconDraw(bShowFWD, INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X*3-10, INDICATE_ARROW_UP_ICON_Y-2, ID_ICON_BUTTON_MENU);
	appPlayOsd_IconDraw(bShowFWD, INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X*4-10, INDICATE_ARROW_UP_ICON_Y, ID_ICON_VIDEOPLAY_STOP);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_MediaPlayStatusDraw
 *------------------------------------------------------------------------*/

void
appPlayOsd_MediaPlayStatusDraw(
	UINT8 bShow,
	UINT16 MediaPlayStatus
)
{

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, PB_PLAY_MEDIA_STATUS_ICON_SIZE_X+16, PB_PLAY_MEDIA_STATUS_ICON_SIZE_Y, 0, 0);
	if (bShow)
	{
	#if 1
		switch (MediaPlayStatus)
		{
			case PLAYING:
				appPlayOsd_IconDraw(bShow, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, ID_ICON_PLAY_STS_PLAY);
				break;
			case PAUSING:
				appPlayOsd_IconDraw(bShow, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, ID_ICON_PLAY_STS_PAUSE);
				break;
			case FWDING:
				appPlayOsd_IconDraw(bShow, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, ID_ICON_PLAY_STS_FF);
				break;
			case RWDING:
				appPlayOsd_IconDraw(bShow, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, ID_ICON_PLAY_STS_REW);
				break;
			case SLOW :
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextDraw( PB_PLAY_MEDIA_STATUS_ICON_X , PB_PLAY_MEDIA_STATUS_ICON_Y+16 , SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)"SLOW");
				break;
		}
	#else
		switch (MediaPlayStatus)
		{
			case PLAYING:
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)RO_RES_PATH"UI\\ICON\\PB.SFN", 0x0001);
				break;
			case PAUSING:
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)RO_RES_PATH"UI\\ICON\\PB.SFN", 0x0002);
				break;
			case FWDING:
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)RO_RES_PATH"UI\\ICON\\PB.SFN", 0x0003);
				break;
			case RWDING:
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8 *)RO_RES_PATH"UI\\ICON\\PB.SFN", 0x0000);
				break;
			case SLOW :
				appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
				appOsdLib_TextDraw( PB_PLAY_MEDIA_STATUS_ICON_X , PB_PLAY_MEDIA_STATUS_ICON_Y+16 , SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)"SLOW");
				break;
		}
	#endif
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ProgressBarFrame
 *------------------------------------------------------------------------*/
void
appPlayOsd_ProgressBarFrameDraw(
	UINT8 bShow
)
{
	PBLastProgBarLength = 0;

	if(bShow)
	{
		appOsd_FontIconFontSet(RES_ICON_WARNM);
		appOsdLib_OsdLayIconDraw(PB_VOICE_MEMO_PROG_FRAME_CENT_X,PB_VOICE_MEMO_PROG_FRAME_CENT_Y,SP5K_GFX_ALIGN_CENTER,0x0000);

		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsd_FontIconFontSet(ID_FONT_TK_BIGICON);
		}else
		#endif
		{
		    appOsd_FontIconFontSet(ID_FONT_TK_ICON);
	    }
	}
	else
	{
		appOsdLib_TransparentLayerShapeClear(APP_OSD_SHAPE_RECT, PB_VOICE_MEMO_PROG_FRAME_LEFT_X, PB_VOICE_MEMO_PROG_FRAME_UPPER_Y, PB_VOICE_MEMO_PROG_FRAME_SIZE_X, PB_VOICE_MEMO_PROG_FRAME_SIZE_Y, 0, 0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ProgressBarLastTimeReset
 *------------------------------------------------------------------------*/
void
appPlayOsd_ProgressBarLastTimeReset(
		void
)
{
	PBLastProgBarLength =0;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ProgressBarDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_ProgressBarDraw(
		UINT32 CurrentTime_msec,
		UINT32 TotalTime_msec
)
{
	UINT32 CurrentProgBarLength;

	if (CurrentTime_msec > TotalTime_msec)
	{
		CurrentTime_msec = TotalTime_msec;
	}

	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		CurrentProgBarLength = ((float)CurrentTime_msec / (float)TotalTime_msec) * (float)PBHDMI_VOICE_MEMO_BAR_SIZE_X;
	}else
	#endif
	{
		CurrentProgBarLength = ((float)CurrentTime_msec / (float)TotalTime_msec) * (float)PB_VOICE_MEMO_BAR_SIZE_X;
	}

	if(PBLastProgBarLength <= CurrentProgBarLength)
	{
		appOsd_ColorDrawShapeColorSet(PAL_YELLOW, PAL_BLEND_100, PAL_YELLOW, PAL_BLEND_100);
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 121 + PBLastProgBarLength, 141, CurrentProgBarLength - PBLastProgBarLength, 14, 0, 0);
		}else
		#endif
		{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_VOICE_MEMO_BAR_X + PBLastProgBarLength, PB_VOICE_MEMO_BAR_Y, CurrentProgBarLength - PBLastProgBarLength, PB_VOICE_MEMO_BAR_SIZE_Y, 0, 0);
	}
	}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_GRAY_2, PAL_BLEND_100, PAL_GRAY_2, PAL_BLEND_100);
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 121 + CurrentProgBarLength , 141, PBLastProgBarLength - CurrentProgBarLength, 14, 0, 0);
		}else
		#endif
		{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_VOICE_MEMO_BAR_X + CurrentProgBarLength , PB_VOICE_MEMO_BAR_Y, PBLastProgBarLength - CurrentProgBarLength, PB_VOICE_MEMO_BAR_SIZE_Y, 0, 0);
	}
	}
	PBLastProgBarLength = CurrentProgBarLength;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ProgressTimeDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_ProgressTimeDraw(
	UINT8 bShow,
	UINT32 ProgressTime_msec,
	UINT8 X,
	UINT8 Y
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, X, Y, ALPHABET_FONT_SIZE_X * 10, ALPHABET_FONT_SIZE_Y, 0, 0);

	if (bShow)
	{

		UINT32 ProgressTime = (ProgressTime_msec / 1000);

		/*if(!ProgressTime && ProgressTime_msec != 0)
			ProgressTime = 1;*/

		UINT32  SS =  (ProgressTime % 3600) % 60;
		UINT32  MM =  (ProgressTime / 60) % 60;
		UINT32  HH =  ProgressTime / 3600;
		char  TimeString[9];
		memset(TimeString, 0, 9);
		sprintf(TimeString, "%02u:%02u:%02u", HH, MM, SS);

		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextDraw( X , Y , SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)TimeString);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ShowFileTime
 *------------------------------------------------------------------------*/
void appPlayOsd_ShowFileTime(UINT8 bShow,UINT32 CurrentTime_msec)
{
	/* UINT32 x=15,y=150; */
    #if SP5K_CDFS_OPT
    if(pUiSetting->pbfileType == PB_FILETYPE_VIDEO)
    #else
    if(pUiSetting->pbfileType != PB_FILETYPE_IMAGE)
    #endif
	    appPlayOsd_ProgressTimeDraw(bShow, CurrentTime_msec, 15, 150);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VideoTimeDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_VideoTimeDraw(
		UINT8 bShow,
		UINT32 CurrentTime_msec
)
{
	appPlayOsd_ProgressTimeDraw(bShow, CurrentTime_msec, PB_VIDEO_TIME_X, PB_VIDEO_TIME_Y);
}


/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ThumbnailIconDraw			Brian
 *------------------------------------------------------------------------*/

void
appPlayOsd_ThumbnailIconDraw(
	UINT8 bShow,
	UINT32 type,
	UINT32 X,
	UINT32 Y
)
{
	/* UINT16 MultiPlaybackIcon[] = {
		[MPLAYBACK_PROTECTED		] = ID_ICON_PROTECT,
		[MPLAYBACK_UNPROTECTED		] = ID_ICON_NONE,
		[MPLAYBACK_NEWPROTECTED 	] = ID_ICON_PROTECT,
		[MPLAYBACK_DELETE			] = ID_ICON_DELETE,
		[MPLAYBACK_VOICE_MEMO		] = ID_ICON_MODE_VOICE,
		[MPLAYBACK_VIDEO			] = ID_ICON_MODE_VIDEO
	}; */

	UINT32 x1, y1;

	switch(type)
	{
		case MPLAYBACK_PROTECTED:
		case MPLAYBACK_UNPROTECTED:
		case MPLAYBACK_NEWPROTECTED:
			x1 = X + MPB_PROTECT_ICON_X;
			y1 = Y + MPB_PROTECT_ICON_Y;
			break;
		case MPLAYBACK_DELETE:
			x1 = X + MPB_DELETE_ICON_X;
			y1 = Y + MPB_DELETE_ICON_Y;
			break;
		case MPLAYBACK_VOICE_MEMO:
		case MPLAYBACK_VIDEO:
			x1 = X + MPB_VOICE_MEMO_ICON_X;
			y1 = Y + MPB_VOICE_MEMO_ICON_Y;
			break;
		default:
			x1 = y1 = 0;
			break;
	}

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x1, y1, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y - 6, 0, 0);

	if (bShow)
	{
		UINT16 MultiPlaybackIcon[] = {
		[MPLAYBACK_PROTECTED		] = ID_ICON_PROTECT,
		[MPLAYBACK_UNPROTECTED		] = ID_ICON_NONE,
		[MPLAYBACK_NEWPROTECTED 	] = ID_ICON_PROTECT,
		[MPLAYBACK_DELETE			] = ID_ICON_DELETE,
		[MPLAYBACK_VOICE_MEMO		] = ID_ICON_MODE_VOICE,
		[MPLAYBACK_VIDEO			] = ID_ICON_MODE_VIDEO
		};
		appOsdLib_OsdLayIconDraw(x1, y1, SP5K_GFX_ALIGN_TOP_LEFT, MultiPlaybackIcon[type]);
	}

}
/*-------------------------------------------------------------------------
 *	File Name : appPlayOsd_DigitalZoomWinDraw
 *------------------------------------------------------------------------*/
BOOL doFirstTime=TRUE;		/* Modify by sc.liu */

void appPlayOsd_DigitalZoomWinDraw
(
	SINT32 CropImageCentX,
	SINT32 CropImageCentY,
	UINT32 ImageWidth,
	UINT32 ImageHeight,
	UINT32 ZoomFactor,
	UINT8 ImageAspRatio
)
{
	UINT32 DZoom_Pan_W;
	UINT32 DZoom_Pan_H;
	char  AMPString[5];
	static UINT32 DZoomCenX;
	static UINT32 DZoomCenY;
	static SINT32 DZoom_Crop_X;
	static SINT32 DZoom_Crop_Y;

	switch(ImageAspRatio)
	{
		case PB_IMG_ORIENT_16X9:
			DZoom_Pan_W = 67;
			DZoom_Pan_H	= 39;
			break;
		case PB_IMG_ORIENT_3X2:
			DZoom_Pan_W = 67;
			DZoom_Pan_H	= 45;
			break;
		case PB_IMG_ORIENT_4X3:
			DZoom_Pan_W = 67;
			DZoom_Pan_H	= 51;
			break;
		case PB_IMG_ORIENT_9X16:
			DZoom_Pan_W = 39;
			DZoom_Pan_H	= 67;
			break;
		case PB_IMG_ORIENT_2X3:
			DZoom_Pan_W = 45;
			DZoom_Pan_H	= 67;
			break;
		case PB_IMG_ORIENT_3X4:
			DZoom_Pan_W = 51;
			DZoom_Pan_H	= 67;
			break;
		case PB_IMG_ORIENT_UNKNOWN:
		default:
			if((ImageWidth / MAX_ZOOM_PAN_W_H) >= (ImageHeight / MAX_ZOOM_PAN_W_H))
			{
				DZoom_Pan_W = ImageWidth  / (ImageWidth /MAX_ZOOM_PAN_W_H);
				DZoom_Pan_H = ImageHeight / (ImageWidth /MAX_ZOOM_PAN_W_H);
			}
			else
			{
				DZoom_Pan_W = ImageWidth  / (ImageHeight /MAX_ZOOM_PAN_W_H);
				DZoom_Pan_H = ImageHeight / (ImageHeight /MAX_ZOOM_PAN_W_H);
			}

			if(DZoom_Pan_W % 2 == 0)
			{
				DZoom_Pan_W = DZoom_Pan_W + 1;
			}

			if(DZoom_Pan_H % 2 == 0)
			{
				DZoom_Pan_H = DZoom_Pan_H + 1;
			}

			break;
	}


	int Ratio = (ImageWidth /DZoom_Pan_W);

	SINT32 DZoom_Pan_X = CENT_DZOOM_PAN_X - (DZoom_Pan_W)/2.0;
	SINT32 DZoom_Pan_Y = CENT_DZOOM_PAN_Y - (DZoom_Pan_H)/2.0;

	SINT32 AMP_X = DZoom_Pan_X-10;
	SINT32 AMP_Y = DZoom_Pan_Y - 30;

	int DZoom_Crop_W = (float)(DZoom_Pan_W * 1000) / ZoomFactor;
	int DZoom_Crop_H = (float)(DZoom_Pan_H * 1000) / ZoomFactor;

/* Modify by sc.liu */
	if(DZoom_Crop_W % 2 != 0)
	{
		DZoom_Crop_W = DZoom_Crop_W - 1;
	}

	if(DZoom_Crop_H % 2 != 0)
	{
		DZoom_Crop_H = DZoom_Crop_H - 1;
	}

	if(doFirstTime)
	{
		DZoom_Crop_X = ((float)((float)CropImageCentX * ZoomFactor - (float)(ImageWidth/2.0*1000)))/Ratio/(float)ZoomFactor  ;
		DZoom_Crop_X = DZoom_Crop_X + DZoom_Pan_X + ((DZoom_Crop_X%2)? 0:1);
		DZoomCenX = DZoom_Crop_X + DZoom_Crop_W/2;

		DZoom_Crop_Y = ((float)((float)CropImageCentY * ZoomFactor - (float)(ImageHeight/2.0*1000)))/Ratio/(float)ZoomFactor;
		DZoom_Crop_Y = DZoom_Crop_Y + DZoom_Pan_Y + ((DZoom_Crop_Y%2)? 0:1);
		DZoomCenY = DZoom_Crop_Y + DZoom_Crop_H/2;
		doFirstTime = FALSE;
	}
	//Detecting whether reach the border or not after zooming

	DZoom_Crop_X = DZoomCenX - DZoom_Crop_W/2;
	DZoom_Crop_Y = DZoomCenY - DZoom_Crop_H/2;
/* Modify by sc.liu */
	if(DZoom_Crop_Y < DZoom_Pan_Y)
	{
		DZoom_Crop_Y = DZoom_Pan_Y;
	}

	if(DZoom_Crop_Y + DZoom_Crop_H > DZoom_Pan_Y + DZoom_Pan_H )
	{
		DZoom_Crop_Y = DZoom_Pan_Y + DZoom_Pan_H - DZoom_Crop_H;
	}

	if(DZoom_Crop_X < DZoom_Pan_X)
	{
		DZoom_Crop_X = DZoom_Pan_X;
	}

	if(DZoom_Crop_X + DZoom_Crop_W > DZoom_Pan_X + DZoom_Pan_W)
	{
		DZoom_Crop_X = DZoom_Pan_X + DZoom_Pan_W - DZoom_Crop_W;
	}

	//Detecting whether reach the border or not after moving

	UINT32 UpBound = ((UINT32)(ImageHeight/(((float)ZoomFactor)/1000)))>>1;
	UINT32 LowBound = ImageHeight - ((UINT32)(ImageHeight/(((float)ZoomFactor)/1000))>>1) - 1;
	UINT32 LeftBound = ((UINT32)(ImageWidth/(((float)ZoomFactor)/1000)))>>1;
	UINT32 RightBound = ImageWidth - ((UINT32)(ImageWidth/(((float)ZoomFactor)/1000))>>1) - 1;

	if(CropImageCentY > UpBound)
	{
		appOsdLib_OsdLayIconDraw(LCD_SIZE_X/2 - 10, 0, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MEDIA_UP_ARROW);
	}
	else
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, LCD_SIZE_X/2 - 10, 0, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
	}

	if(CropImageCentY <LowBound)
	{
		appOsdLib_OsdLayIconDraw(LCD_SIZE_X/2 - 10, LCD_SIZE_Y, SP5K_GFX_ALIGN_BOTTOM_LEFT, ID_ICON_MEDIA_DOWN_ARROW);
	}
	else
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, LCD_SIZE_X/2 - 10, LCD_SIZE_Y - APPPB_ICON_SIZE_Y, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
	}

	if(CropImageCentX > LeftBound)
	{
		appOsdLib_OsdLayIconDraw(0, LCD_SIZE_Y/2 - 10, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_MEDIA_LEFT_ARROW);
	}
	else
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 0, LCD_SIZE_Y/2 - 10, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
	}

	if(CropImageCentX < RightBound)
	{
		appOsdLib_OsdLayIconDraw(LCD_SIZE_X, LCD_SIZE_Y/2 - 10, SP5K_GFX_ALIGN_TOP_RIGHT, ID_ICON_MEDIA_RIGHT_ARROW);
	}
	else
	{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, LCD_SIZE_X - APPPB_ICON_SIZE_X, LCD_SIZE_Y/2 - 10, APPPB_ICON_SIZE_X, APPPB_ICON_SIZE_Y, 0, 0);
	}


	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_75, PAL_GRAY_1, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DZoom_Pan_X, DZoom_Pan_Y, DZoom_Pan_W, DZoom_Pan_H, 0, 0);
	appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, DZoom_Crop_X, DZoom_Crop_Y, DZoom_Crop_W, DZoom_Crop_H, 0, 0);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, AMP_X, AMP_Y, ALPHABET_FONT_SIZE_X * 6+10, ALPHABET_FONT_SIZE_Y, 0, 0);
	memset(AMPString, 0, sizeof(AMPString));
	sprintf(AMPString,"%u.%02ux",ZoomFactor / 1000, ZoomFactor /10  - (ZoomFactor/1000)*100);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(AMP_X, AMP_Y, SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)AMPString);

}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_ModeIconDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_ModeIconDraw
(
	BOOL bShow
)
{
	appPlayOsd_IconDraw(bShow, PLAYBACK_ICON_X, PLAYBACK_ICON_Y, ID_ICON_MODE_PLAYBACK);
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_AudioBGDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_AudioBGDraw(
	BOOL bShow
)
{

    if(bShow)
    {
        #if (KIT_WITH_HDMI)
    	if(IS_HDMI_IN){
    		appJpgDraw_PfLayer(RES_JPG_AUDIO_BG, 0, 0, 640, 360);
    	}else
    	#endif
    	{
        	appModeSet(SP5K_MODE_STANDBY);
    		appJpgDraw_PfLayer(RES_JPG_AUDIO_BG, 0, 0, 320, 240);
    	}
    }
    else
    {
		appOsdLib_PFLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, 320, 240, 0, 0);
    }
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_MainInfoDraw
 *------------------------------------------------------------------------*/
void
appPlayOsd_MainInfoDraw(
	UINT32 osdDrawType
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)/*weikai modify 180426*/
	return;
#endif
	DBG_PRINT("pbosd: *************************");
	DBG_PRINT("pbosd: MAIN_INFO_DRAW: osdType(%d) ...\n", gpbMainCb.osdType);
	DBG_PRINT("pbosd: *************************\n");

	if (gpbMainCb.isNoImage || gpbDiskCb.isDiskErr || gpbImgCb.isFileErr)
	{
		APP_OSD_REFRESH_ON;
		return;
	}

	APP_OSD_REFRESH_OFF;
	appPlayOsd_OSDInfoDraw(&gpbMainCb,&gpbExifInfo,&gpbImgCb);
	APP_OSD_REFRESH_ON;
	DBG_PRINT("pbosd: MainInfoDraw done \n");
}
/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_OSDInfoDraw
 *------------------------------------------------------------------------*/

void appPlayOsd_OSDInfoDraw(pbMainCB_t *psPBImage, pbImgExifInfo_t *psPBCurImgExif,	pbImgCb_t *pimgCb)
{

	switch(psPBImage->osdType)
	{
		case PB_DISP_FULL:
		case PB_DISP_BASIC:
			switch(pimgCb->fileType)
			{
				case PB_FT_JPG_MEMO:
				case PB_FT_JPG:
                    #if 0
					appPlayOsd_DateTimeDraw(TRUE,pUiSetting->dateTime,&pimgCb->dcfAttr.time);
					appPlayOsd_ResolutionDraw(TRUE,pimgCb->sizeGrade);
					appPlayOsd_PicExifInfoDraw(TRUE,psPBCurImgExif,pimgCb);
					appPlayOsd_HistogramDraw(TRUE, pimgCb->pHistBuf	);
                    #endif
                    break;
				case PB_FT_MOV:
				case PB_FT_AVI:
				case PB_FT_MP4:
				case PB_FT_WAV:
                    appPlayOsd_ShowFileTime(TRUE,gpbImgCb.mediaTotalTime);
					appPlayOsd_VideoFileIconDrow(TRUE);
					break;
				default:
					break;
			}
            appPlayOsd_ModeIconDraw(TRUE);
			appPlayOsd_FileNumDraw(TRUE,psPBImage->curImgIdx,psPBImage->totalImg);
            appPlayOsd_DateTimeDraw(TRUE,pUiSetting->dateTime,&pimgCb->dcfAttr.time);
            appPlayOsd_CurrntFileSizeDraw(TRUE,pimgCb->dcfAttr.filesize);
			appPlayOsd_ProtectDraw(pimgCb->isProtected);
		case PB_DISP_NONE:
		default:
			break;
	}
}

/*-------------------------------------------------------------------------
*	File Name : appPlayOsd_PicExifInfoDraw
*------------------------------------------------------------------------*/
void appPlayOsd_PicExifInfoDraw(BOOL bShow, PpbImgExigInfo_t pCurImgExif,pbImgCb_t *pimgCb)
{
	pbImgExifInfo_t tmpExifInfo;

	if (pCurImgExif == NULL)
	{
		pCurImgExif = &tmpExifInfo;
	}
	appPlayOsd_WhiteBalanceDraw(bShow, pCurImgExif->wbMode,pCurImgExif->lightSource);
	appPlayOsd_ExposureDraw(bShow, pCurImgExif->expBias[0]);
	appPlayOsd_ISODraw(bShow, pCurImgExif->iso);
	appPlayOsd_StrobeModeDraw(bShow, pCurImgExif->flash);
	appPlayOsd_MeteringModeDraw(bShow, pCurImgExif->meteringMode);
	appPlayOSD_JPGImageIconDraw(bShow,pCurImgExif);
	appPlayOsd_CurrntFileNameDraw(bShow,pimgCb->dcfAttr.filename);
	appPlayOsd_CurrntFileSizeDraw(bShow,pimgCb->dcfAttr.filesize);
    appPlayOsd_CurrntExpFNumDraw(bShow,pCurImgExif);
}
/*-------------------------------------------------------------------------
*	File Name : appPLayOsd_MemoRecOsdDraw
*------------------------------------------------------------------------*/
void appPLayOsd_MemoRecOsdDraw(BOOL bShow)
{
	UINT32 mediaStat;

	if(!bShow)
	{
		appPlayOsd_ShowFileTime(bShow,gpbImgCb.mediaTotalTime);
		appPlayOsd_ProgressBarFrameDraw(bShow);
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_AUDIO_STRING_X-(PB_AUDIO_STRING_W/2), PB_AUDIO_STRING_Y, PB_AUDIO_STRING_W, 30, 0, 0);
		return;
	}
	appPlayOsd_ProgressBarFrameDraw(bShow);
	appPlayOsd_ShowFileTime(bShow,gpbImgCb.mediaTotalTime);

	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 104, 75, 117, 30, 0, 0);
	}else
	#endif
	{
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_AUDIO_STRING_X-(PB_AUDIO_STRING_W/2), PB_AUDIO_STRING_Y, PB_AUDIO_STRING_W, 30, 0, 0);
	}
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	mediaStat = appPbMediaUIStateGet();

	if(mediaStat==PB_MEDIA_UI_STAT_RECORDING){
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_TextIDDraw(162,75,SP5K_GFX_ALIGN_TOP, STRTK_RECORDING_MEMO);
		}else
		#endif
		{
		appOsdLib_TextIDDraw(PB_AUDIO_STRING_X,PB_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP, STRTK_RECORDING_MEMO);
		}
	}  else if (mediaStat==PB_MEDIA_UI_STAT_STANDBY){
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_TextIDDraw(162,75,SP5K_GFX_ALIGN_TOP, STRTK_RDY_TO_REC);
		}else
		#endif
		{
		appOsdLib_TextIDDraw(PB_AUDIO_STRING_X,PB_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP, STRTK_RDY_TO_REC);
		}
	}
}


/*-------------------------------------------------------------------------
*	File Name : appPLayOsd_AudioPauseResumeDraw
*------------------------------------------------------------------------*/
void appPLayOsd_AudioPauseResumeDraw(BOOL isPause)
{
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 104, 75, 117, 30, 0, 0);
	}else
	#endif
	{
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_AUDIO_STRING_X-(PB_AUDIO_STRING_W/2), PB_AUDIO_STRING_Y, PB_AUDIO_STRING_W, 30, 0, 0);
	}
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	appPlayOsd_ShowFileTime(TRUE, gpbImgCb.mediaElapTime);

	if (isPause)
	{
		/*-- PAUSE */
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_TextIDDraw(162,75,SP5K_GFX_ALIGN_TOP,STRTK_PAUSE);
		}else
		#endif
		{
		appOsdLib_TextIDDraw(PB_AUDIO_STRING_X,PB_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP,STRTK_PAUSE);
		}
		appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
		appPlayOsd_MenuIconDraw(TRUE);
		//appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, "A:\\RO_RES\\UI\\ICON\\PB.SFN", 0x0002);
	}
	else
	{
		/*-- RESUME */
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_TextIDDraw(162,75,SP5K_GFX_ALIGN_TOP,STRTK_PLAYING_AUDIO);
		}else
		#endif
		{
			appOsdLib_TextIDDraw(PB_AUDIO_STRING_X,PB_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP,STRTK_PLAYING_AUDIO);
		}
		appPlayOsd_VoiceMemoCntrlDraw(TRUE,FALSE);
		appPlayOsd_MenuIconDraw(TRUE);
	}

}

/*-------------------------------------------------------------------------
*	File Name : appPLayOsd_AudioPlayDraw
*------------------------------------------------------------------------*/
void appPLayOsd_AudioPlayDraw(BOOL bShow)
{

	appPlayOsd_ProgressBarFrameDraw(bShow);

	appPlayOsd_ShowFileTime(bShow,gpbImgCb.mediaTotalTime);

	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 104, 75, 117, 30, 0, 0);
	}else
	#endif
	{
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, PB_AUDIO_STRING_X-(PB_AUDIO_STRING_W/2), PB_AUDIO_STRING_Y, PB_AUDIO_STRING_W, 30, 0, 0);
	}
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);

	if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_PLAYING)
	{
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_TextIDDraw(162,75,SP5K_GFX_ALIGN_TOP,STRTK_PLAYING_AUDIO);
		}else
		#endif
		{
			appOsdLib_TextIDDraw(PB_AUDIO_STRING_X,PB_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP,STRTK_PLAYING_AUDIO);
		}
		appPlayOsd_VoiceMemoCntrlDraw(TRUE,FALSE);
		appPlayOsd_MenuIconDraw(TRUE);
	}
	else if(appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_PAUSE)
	{
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_TextIDDraw(162,75,SP5K_GFX_ALIGN_TOP,STRTK_PAUSE);
		}else
		#endif
		{
		appOsdLib_TextIDDraw(PB_AUDIO_STRING_X,PB_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP,STRTK_PAUSE);
		}
		appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
		appPlayOsd_MenuIconDraw(TRUE);
		appPlayOsd_IconDraw(1, PB_PLAY_MEDIA_STATUS_ICON_X, PB_PLAY_MEDIA_STATUS_ICON_Y, ID_ICON_PLAY_STS_PAUSE);
	}
	else if (appPbMediaUIStateGet()==PB_MEDIA_UI_STAT_STANDBY)
	{
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_TextIDDraw(162,75,SP5K_GFX_ALIGN_TOP,STRTK_RDY_TO_PLAY);
		}else
		#endif
		{
			appOsdLib_TextIDDraw(PB_AUDIO_STRING_X,PB_AUDIO_STRING_Y,SP5K_GFX_ALIGN_TOP,STRTK_RDY_TO_PLAY);
		}
		appPlayOsd_VoiceMemoCntrlDraw(TRUE,TRUE);
		appPlayOsd_MenuIconDraw(FALSE);
	}
	else
	{
		appPlayOsd_VoiceMemoCntrlDraw(bShow,TRUE);
	}

}

/*-------------------------------------------------------------------------
*	 File Name : appPLayOsd_LeftRightArrow
*------------------------------------------------------------------------*/
void appPLayOsd_LeftRightArrow(BOOL bShow)
{
	if(bShow)
	{
		appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_1);
		appOsdLib_OsdLayIconDraw(FILE_LEFTRIGHT_LEFT_ARROW_X,FILE_LEFTRIGHT_LEFT_ARROW_Y, SP5K_GFX_ALIGN_TOP_LEFT,  ID_ICON_MEDIA_LEFT_ARROW);
		appOsdLib_OsdLayIconDraw(FILE_LEFTRIGHT_RIGHT_ARROW_X,FILE_LEFTRIGHT_LEFT_ARROW_Y,SP5K_GFX_ALIGN_TOP_LEFT ,  ID_ICON_MEDIA_RIGHT_ARROW);
	}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, FILE_LEFTRIGHT_LEFT_ARROW_X, FILE_LEFTRIGHT_LEFT_ARROW_Y,FILE_LEFTRIGHT_ARROW_CLEAR_W, FILE_LEFTRIGHT_ARROW_CLEAR_H, 0, 0);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, FILE_LEFTRIGHT_RIGHT_CLEAR_X, FILE_LEFTRIGHT_LEFT_ARROW_Y,FILE_LEFTRIGHT_ARROW_CLEAR_W, FILE_LEFTRIGHT_ARROW_CLEAR_H, 0, 0);
	}
}

/*-------------------------------------------------------------------------
*	 File Name : appPlayOsd_DlgMessageShow
*------------------------------------------------------------------------*/
void appPlayOsd_DlgMessageShow(UINT32 titleStrId,UINT32 dlgType)
{
	UINT32 dlgMsg_YesNo[2] = {STRTK_YES, STRTK_NO};
	UINT32 dlgMsg_Rotate[3] = {STRTK_PB_ROTATE, STRTK_PB_SAVE, STRTK_PB_BACK};
	UINT32 dlgMsg_MPbLock[4] = {STRTK_SEL, STRTK_PB_PROTECT_ALL, STRTK_PB_UNPROTECT_ALL, STRTK_PB_BACK};
	UINT32 dlgMsg_MpbDele[3] = {STRTK_SEL, STRTK_ALL, STRTK_PB_BACK};
	UINT32 *pItem;
	UINT32 numOption;
	UINT32 actIndex,yOffset;
	actIndex = yOffset =0;
	switch(dlgType)
	{
		case DLGTYPE_ROTATE:
			pItem = dlgMsg_Rotate;
			numOption = 3;
			break;
		case DLGTYPE_MPB_DELE:
			pItem = dlgMsg_MpbDele;
			numOption = 3;
			break;
		case DLGTYPE_MPB_LOCK:
			pItem = dlgMsg_MPbLock;
			numOption = 4;
			break;
		case DLGTYPE_YES_NO:
		default:
			pItem = dlgMsg_YesNo;
			numOption = 2;
			break;

	}
	APP_OSD_REFRESH_OFF;
	CLEAR_OSD_SRC;
	appPlayOsd_DlgBoxConfig(titleStrId, numOption, pItem, actIndex, yOffset);
	appPlayOsd_DlgBoxShow();
	appOsd_GfxIconIDSet(SP5K_GFX_ICON_ID_3);
	appOsdLib_PIPLayerIconDraw(PB_STARTING_OKICON_X, PB_STARTING_OKICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, 0x0001);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(PB_STARTING_SELECTSTR_X,PB_STARTING_SELECTSTR_Y,SP5K_GFX_ALIGN_TOP_LEFT,STRTK_SEL);
	APP_OSD_REFRESH_ON;
}

/*-------------------------------------------------------------------------
*	 File Name : appPlayOsd_DlgMessageProcess
*------------------------------------------------------------------------*/
UINT8 appPlayOsd_DlgMessageProcess(UINT32 msg)
{
	UINT32 ret ;


	if(gpbDlgCb.dlgStatus == DLGSTATUS_SHOW)
	{
		ret = APPSTATE_MSG_IGNORE;
		switch(msg)
		{
			case APP_KEY_PRESS_UP:
				appPlayOsd_DlgBoxScroll(TRUE);
				break;
			case APP_KEY_PRESS_DOWN:
				appPlayOsd_DlgBoxScroll(FALSE);
				break;
			case APP_KEY_PRESS_SET:
				gpbDlgCb.dlgStatus = DLGSTATUS_SET;
				ret = APPSTATE_MSG_DEFAULT;
				break;
			default:
				break;
		}
	}
	else
	{
		ret = APPSTATE_MSG_DEFAULT;
	}

	return ret;
}

/*-------------------------------------------------------------------------
 *  File Name : appPlayOsd_VideoClrOsd
 *------------------------------------------------------------------------*/
void appPlayOsd_VideoClrOsd(void)
{
	/*-- UI OSD... */
    /*profLogAdd(0,"appPlayOsd_VideoClrOsd s");*/
#if 0
    UINT32 Str_x = 148, Str_y=41;
    UINT32 W = 190, gapY=33, gap_clr=5, H = (gapY<<2)+gap_clr;
	APP_OSD_REFRESH_OFF;
    /*AppPbDestroyshowJPGInPIP();*/
    appPlayOsd_DateTimeDraw(FALSE, 0, 0);
    appPlayOsd_CurrntFileSizeDraw(FALSE,0);

	if(IS_TV_IN){
		Str_x  = 148;
		Str_y  = 54;
		gapY   = 26;
        gap_clr= 5;
		H      = (gapY<<2)+gap_clr;
		W      = 190;
	}

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		Str_x  = Str_x<<1;
		Str_y  = 66;
		gapY   = 48;
        gap_clr= 15;
		H      = (gapY<<1)+(gap_clr<<1);
		W      = 190;
	}
	#endif

	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,Str_x-gap_clr, Str_y-gap_clr, W, H, 0, 0);
	appPlayOsd_VideoCntrlDraw(TRUE,FALSE,TRUE,TRUE);
	appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);
	APP_OSD_REFRESH_ON;
#else
    APP_OSD_REFRESH_OFF;

#if defined(PRJ_PDVR)
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,0, 0, 320, 240, 0, 0);
	appOsdLib_OsdLayIconDraw(INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X*3-10, INDICATE_ARROW_UP_ICON_Y-2, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_MENU);

#else
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,0, 0, 320, 240, 0, 0);
	appOsdLib_OsdLayIconDraw(INDICATE_ARROW_UP_ICON_X - APPPB_ICON_SIZE_X*2+3, INDICATE_ARROW_UP_ICON_Y+2, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_LEFT);
	appOsdLib_OsdLayIconDraw(INDICATE_ARROW_UP_ICON_X - APPPB_ICON_SIZE_X-5, INDICATE_ARROW_UP_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_VIDEOPLAY_REW);
    appOsdLib_OsdLayIconDraw(INDICATE_ARROW_UP_ICON_X, INDICATE_ARROW_UP_ICON_Y+2, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_UP);
    appOsdLib_OsdLayIconDraw(INDICATE_PLAY_ICON_X, INDICATE_PLAY_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_VIDEOPLAY_PAUSE);
	appOsdLib_OsdLayIconDraw(INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X-4-5, INDICATE_ARROW_UP_ICON_Y+2, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_RIGHT);
	appOsdLib_OsdLayIconDraw(INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X*2-15, INDICATE_PLAY_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_VIDEOPLAY_FF);
	appOsdLib_OsdLayIconDraw(INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X*3-10, INDICATE_ARROW_UP_ICON_Y-2, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_MENU);
	appOsdLib_OsdLayIconDraw(INDICATE_PLAY_ICON_X + APPPB_ICON_SIZE_X*4-10, INDICATE_ARROW_UP_ICON_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_VIDEOPLAY_STOP);
#endif
	appPlayOsd_VideoTimeDraw(TRUE,gpbImgCb.mediaElapTime);
#endif
 /*profLogAdd(0,"appPlayOsd_VideoClrOsd e");*/
}

 /*-------------------------------------------------------------------------
  *  File Name : appPbFilePageCtrlParaInit
  *------------------------------------------------------------------------*/
 void
 appPbFilePageCtrlParaInit(void)
 {
	 gtotalImgCount = appPbTotalFileCountGet();
     if (gpbMainCb.lastImgIdx != 0)
        gcurrDcfIdx = gpbMainCb.lastImgIdx;
    else
        gcurrDcfIdx = gtotalImgCount;
	 gpbFilePageCrl.totalPage = (gtotalImgCount-1)/4;
	 gpbFilePageCrl.currPage = 0;
	 gpbFilePageCrl.currPageFirstFileIdx = gtotalImgCount;
	 gpbFilePageCrl.currFileIdx = gtotalImgCount;
	 gpbFilePageCrl.pbSts = FALSE;
	 printf("\033[36m PBsts:%s \033[0m\n",gpbFilePageCrl.pbSts?"TRUE":"FALSE");

 }

 /*-------------------------------------------------------------------------
  *  File Name : appPbFilePageCtrlSync
  *------------------------------------------------------------------------*/
  void appPbFilePageCtrlParaSync(void)
 {
	  gpbFilePageCrl.currFileIdx = gcurrDcfIdx;
	  gpbFilePageCrl.currPage = (gtotalImgCount - gcurrDcfIdx)/4;
	  gpbFilePageCrl.totalPage = (gtotalImgCount-1)/4;
 }

 UINT32
 appDcfFileNameGet(
	 UINT32 dcfIdx
 )
 {
	 gpbImgCb.dcfIdx = dcfIdx;
	 if(FAIL==playImgDcfAttrGet(&gpbImgCb))
	 {
		 return FAIL;
	 }
	 return SUCCESS;
 }

 void
 appPlayOsd_VideoInfoDraw(
	 void
 )
 {
	 appPlayOsd_VideoFileIconDrow(TRUE);
	 appPlayOsd_VideoFileNameInfoDraw();
 }

 void
 appPlayOsd_VideoFileIconDrow(
 	unsigned char bShow
 )
{
	UINT32 coordinateIcon_Y, coordinateString_Y;
	/*UINT32 TPButtonUP_X,TPButtonUP_Y,TPButtonDown_X,TPButtonDown_Y;*/
	UINT32 /*coordinateMenuIcon_X, */coordinateOKIcon_X, /*coordinateMenuString_X,*/coordinateOKString_X;
#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		coordinateIcon_Y=302;
		coordinateString_Y=302;

		/*coordinateMenuIcon_X=110;*/
		coordinateOKIcon_X=350;
		/*coordinateMenuString_X=155;*/
		coordinateOKString_X=395;

		/*TPButtonUP_X=310;*/
		/*TPButtonUP_Y=15;*/
		/*TPButtonDown_X=410;*/
		/*TPButtonDown_Y=15;*/
	}
	else
#endif
	{
		coordinateIcon_Y=208;
		coordinateString_Y=208;

		/*coordinateMenuIcon_X=40;*/
		coordinateOKIcon_X=10;
		/*coordinateMenuString_X=78;*/
		coordinateOKString_X=45;

		/*TPButtonUP_X=110;*/
		/*TPButtonUP_Y=9;*/
		/*TPButtonDown_X=180;*/
		/*TPButtonDown_Y=9;*/
	}

	if (!bShow)
	{
		return ;
	}

	appOsd_FontIconFontSet(ID_FONT_ICON);
	//Draw OK , MENU ICON and String
	//appOsdLib_OsdLayIconDraw(coordinateMenuIcon_X, coordinateIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_MENU );
	appOsdLib_OsdLayIconDraw(coordinateOKIcon_X, coordinateIcon_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_BUTTON_OK);

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	//appOsdLib_TextIDDraw(coordinateMenuString_X,coordinateString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_MENU);
	appOsdLib_TextIDDraw(coordinateOKString_X,coordinateString_Y,SP5K_GFX_ALIGN_TOP_LEFT,ID_STR_ENTER);
	#if 0//FUNC_HOST_TOUCH_PANEL
	//appGuiObjClr(-1);
	iconGuiReg(ID_ICON_MODE_PLAYBACK, APP_KEY_PRESS_TPMODE, 0);
	appGuiObjNew_EvtMsgReg(coordinateMenuIcon_X, coordinateIcon_Y, 40, 32,APP_KEY_PRESS_TPMENU, 0);
	appGuiObjNew_EvtMsgReg(coordinateOKIcon_X, coordinateIcon_Y, 40, 32,APP_KEY_PRESS_TPSET, 0);

	appOsdLib_OsdLayIconDraw(TPButtonUP_X,TPButtonUP_Y, SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TBUTTON_UP);/*SHOW UP BUTTON*/
	appOsdLib_OsdLayIconDraw(TPButtonDown_X,TPButtonDown_Y, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICON_TBUTTON_DOWN);/*SHOW DOWN BUTTON*/

	appGuiObjNew_EvtMsgRegExt(TPButtonUP_X ,TPButtonUP_Y,40,30,APP_KEY_RELEASE_TPUP,0,APP_KEY_PRESS_TPUP,0,\
		0,0,0,0,APP_KEY_REPEAT_TPUP,0,0,0);
	appGuiObjNew_EvtMsgRegExt(TPButtonDown_X,TPButtonDown_Y,40,30,APP_KEY_RELEASE_TPDOWN,0,APP_KEY_PRESS_TPDOWN,0,\
		0,0,0,0,APP_KEY_REPEAT_TPDOWN,0,0,0);
	#endif

}

 /**************************************************************************
 *  Function Name: appPlayOsd_VideoFileNameInfoDraw                                           *
 *  Descriptions:                                                         *
 *  Arguments: None                                                       *
 *  Returns:                                                              *
 *  See also:                                                             *
 **************************************************************************/
void
appPlayOsd_VideoFileNameInfoDraw(void)
{

	UINT32 dcfIdx,osdFileNameItem=0;
	char dst[64];
	UINT32 X = 140,Y=41;
	UINT32 gapY=34;
	UINT32 nextPageFirstFileIdx;
	/*UINT32 iconIdx;*/

	#if KIT_WITH_HDMI
	if(IS_HDMI_IN){
		X = 260;
		Y = 66;
		gapY=48;
	}
	#endif

	//APP_OSD_REFRESH_OFF;   /*fix bug ICOM-2388*/
	gtotalImgCount = appPbTotalFileCountGet();

	//appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT,StartX,StartY,W,H,0,0);  /*fix bug ICOM-3451*/
	if(!gtotalImgCount)
	{
		printf("\033[35m This Folder Has No File\033[0m\n");
		appErrMsgShow(ERR_MSG_NO_IMG, PB_ERR_SHOW_FOREVER);
		//APP_OSD_REFRESH_ON;
		return;
	}
	if(gpbMainCb.err == PB_ERR_FILE_ERROR)
	{
		appPbErrorProcess(PB_ERR_FILE_ERROR);
		//APP_OSD_REFRESH_ON;
		return;
	}
	gpbMainCb.err = PB_ERR_NONE;
	gpbMainCb.isNoImage = FALSE;
	appPbFilePageCtrlParaSync();
	gpbFilePageCrl.currPageFirstFileIdx = gtotalImgCount - gpbFilePageCrl.currPage*4;
	if(gpbFilePageCrl.currPageFirstFileIdx<=4)
	{
		gpbFilePageCrl.currPageLastFileIdx = 1;
		nextPageFirstFileIdx = 0;
	}
	else
	{
		gpbFilePageCrl.currPageLastFileIdx = gpbFilePageCrl.currPageFirstFileIdx - 3;
		nextPageFirstFileIdx = gpbFilePageCrl.currPageFirstFileIdx - 4;
	}
	printf("\033[35m gpbFilePageCrl.totalPage = %d,gpbFilePageCrl.currPage = %d,gpbFilePageCrl.currPageFirstFileIdx = %d,gpbFilePageCrl.currFileIdx = %d\033[0m\n",gpbFilePageCrl.totalPage,gpbFilePageCrl.currPage,gpbFilePageCrl.currPageFirstFileIdx,gpbFilePageCrl.currFileIdx);
	for(dcfIdx=gpbFilePageCrl.currPageFirstFileIdx; dcfIdx>nextPageFirstFileIdx; dcfIdx--){
		printf("idx %u  :",dcfIdx);
		if(gpbFilePageCrl.currFileIdx==dcfIdx)
		{
			appOsd_ColorDrawTextColorSet(PAL_YELLOW, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
			/*iconIdx = 2;*/
		}
		else
		{
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
			/*iconIdx = 0;*/
		}
		/*appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, X,Y+(osdFileNameItem*gapY), SP5K_GFX_ALIGN_TOP_LEFT, RES_ICON_PBLISTFRAME, iconIdx);*/
		if(FAIL == appDcfFileNameGet(dcfIdx))
		{
			sprintf(dst,"%s","file error");
		}
		else
		{
			#if 0//FUNC_HOST_TOUCH_PANEL
				/*reg*/
			appGuiObjNew_EvtFcsCallBackReg(\
				X,Y+(osdFileNameItem*gapY) ,\
				220, 40,\
				(pFunc)appPbFileSelectPenUpFuncGui, osdFileNameItem,\
				(pFunc)appPbFileSelectFocusFuncGui, (0x01<<4)|osdFileNameItem, \
				(pFunc)appPbFileSelectFocusFuncGui, osdFileNameItem);
			#endif
			strncpy(dst,(char *)&gpbImgCb.dcfAttr.filename[4],(strlen((char *)&gpbImgCb.dcfAttr.filename[4])-4));
			dst[strlen((char *)&gpbImgCb.dcfAttr.filename[4])-4]=0;
		}
	/* DRAW STRING */
		printf("\033[35m filename show %s \033[0m\n",dst);
		appOsd_GfxStringDraw(X,Y+2+(osdFileNameItem*gapY),SP5K_GFX_ALIGN_TOP_LEFT,(UINT8 *)dst);
		osdFileNameItem++;
	}

	/*add for bug ICOM-292*/
	appDcfFileNameGet(gpbFilePageCrl.currFileIdx);

	//APP_OSD_REFRESH_ON;
}

