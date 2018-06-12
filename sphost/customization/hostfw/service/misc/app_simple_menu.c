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
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "app_com_def.h"
#include "app_com_api.h"
#include "gpio_custom.h"

#include "sp5k_global_api.h"
#include "sp5k_sensor_api.h"

#include "app_menu_frameosd.h"
#include "app_simple_menu.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define STEXT_OSD_FONT_CONST_WIDTH	18
#define STEXT_OSD_FONT_CONST_HEIGHT 32

#define SMENU_COLOR_TITLE_H		5
#define SMENU_NULL_TITLE_H		8
#define SMENU_DATA_CONT_H		34
#define SMENU_DATA_LAST_H		38
#define SMENU_WIDTH				292

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static simpleMenuInfo_t gsmenuCb;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern UINT8 CalibFlashMenuSelect;
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : stext_Clear
 *------------------------------------------------------------------------*/
 void stext_Clear(UINT32 x, UINT32 y, UINT8 *pszText)
 {
 	UINT32 textLen;

 	textLen = strlen((const char*)pszText);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, x-(((textLen+1)>>1)*STEXT_OSD_FONT_CONST_WIDTH), y-(STEXT_OSD_FONT_CONST_HEIGHT>>1), (textLen+1)*STEXT_OSD_FONT_CONST_WIDTH, STEXT_OSD_FONT_CONST_HEIGHT, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : stext_Print
 *------------------------------------------------------------------------*/
 void stext_Print(UINT32 x, UINT32 y, 	const char *format, ...)
 {
 	char buf[256];
	va_list	ap;

	va_start(ap, format);
	vsprintf(buf,format,ap);
	va_end(ap);

	stext_Clear(x, y, (UINT8*)buf);
	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
 	appOsdLib_TextDraw(x, y, SP5K_GFX_ALIGN_CENTER, (UINT8*)buf);
 }

/*-------------------------------------------------------------------------
 *  File Name : dbgOsd_Print
 *------------------------------------------------------------------------*/
 void dbgOsd_Print(UINT32 row, const char *format, ...)
 {
 	char buf[64];
	va_list	ap;

	if (row >= 8) {
        HOST_PROF_LOG_PRINT(LEVEL_ERROR, "smp menu: %s illegal row(%d)", __FUNCTION__, row);
		return;
	}

	va_start(ap, format);
	vsprintf(buf,format,ap);
	va_end(ap);

	stext_Clear(160, row*30, (UINT8*)buf);
	appOsd_ColorDrawTextColorSet(PAL_RED, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
 	appOsdLib_TextDraw(160, row*30, SP5K_GFX_ALIGN_CENTER, (UINT8*)buf);
 }

/*-------------------------------------------------------------------------
 *  File Name : SMENU_SelectGet
 *------------------------------------------------------------------------*/
UINT8
SMENU_SelectGet(
	void
)
{
	return gsmenuCb.actIndex;
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_Clear
 *------------------------------------------------------------------------*/
void
SMENU_Clear(
	void
)
{
	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, 0, 0, 320, 240, 0, 0);
	//appOsdLib_OsdLayerShapeClear(APP_OSD_SHAPE_RECT, gsmenuCb.boxX, gsmenuCb.boxY, gsmenuCb.boxW, gsmenuCb.boxH, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_ActiveBarChange
 *------------------------------------------------------------------------*/
void
SMENU_ActiveBarChange(
	void
)
{
	UINT8 index;

	//appMenu_TKMenuBodyFrameDraw(TRUE, 0, 0);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	//appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_GRAY_1);

	/*-- Inactivate last option */
	index = gsmenuCb.lastActIndex;
	appOsdLib_TextDraw(gsmenuCb.itemX[index], gsmenuCb.itemY[index],
						SP5K_GFX_ALIGN_TOP_LEFT,
						(UINT8*)gsmenuCb.pszItemText[index]);

	/*-- Activate current option */
	index = gsmenuCb.actIndex;
	//appMenu_TKMenuIconFileDraw(TRUE,20, gsmenuCb.itemY[index],TKTYPE_STILLVIEW_MENU_SELBAR_W,TKTYPE_STILLVIEW_MENU_SELBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENUBAR,0x0000);
	appOsd_ColorDrawTextColorSet(PAL_YELLOW_1, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(gsmenuCb.itemX[index], gsmenuCb.itemY[index], SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)gsmenuCb.pszItemText[index]);

}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_Scroll
 *------------------------------------------------------------------------*/
void
SMENU_Scroll(
	BOOL isUp
)
{
	gsmenuCb.lastActIndex = gsmenuCb.actIndex;

	if (isUp) {
		gsmenuCb.actIndex = (gsmenuCb.actIndex==0) ? (gsmenuCb.numItems-1) : gsmenuCb.actIndex-1;
	} else {
		gsmenuCb.actIndex = (gsmenuCb.actIndex>=(gsmenuCb.numItems-1)) ? 0 : gsmenuCb.actIndex+1;
	}

	SMENU_ActiveBarChange();
	appBeep(TRUE, FALSE);
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_Show
 *------------------------------------------------------------------------*/
void
SMENU_Show(
	void
)
{
	int i;
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_GRAY_1);
	appOsd_ColorBlendingSet(PAL_GRAY_1, PAL_BLEND_75);

	/*-- Title TEXT */
	if (gsmenuCb.pszTitleText) {
		appOsdLib_TextDraw(gsmenuCb.titleX, gsmenuCb.titleY,
							SP5K_GFX_ALIGN_BOTTOM, (UINT8*)gsmenuCb.pszTitleText);
	}

	/*-- Options TEXT & ICON, except the last */
	for (i=0; i<gsmenuCb.numItems-1; i++) {
		if(gsmenuCb.actIndex == i){
			appOsd_ColorDrawTextColorSet(PAL_YELLOW_1, PAL_NOCOLOR, PAL_NOCOLOR, PAL_BLEND_100);
		}else{
			appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		}
		appOsdLib_TextDraw(gsmenuCb.itemX[i], gsmenuCb.itemY[i],
							SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)gsmenuCb.pszItemText[i]);
	}

	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(gsmenuCb.itemX[i], gsmenuCb.itemY[i],
						SP5K_GFX_ALIGN_TOP_LEFT, (UINT8*)gsmenuCb.pszItemText[i]);

	appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLEND_100, PAL_WHITE, PAL_BLEND_100);
	for (i=0; i<gsmenuCb.numItems+1; i++) {
		appOsdLib_OsdLayerLineDraw(25, 40+(i*34) , 295 ,40+(i*34));
	}
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_Config
 *------------------------------------------------------------------------*/
void
SMENU_Config(
	UINT8 *pszTitle,
	UINT32 numItems,
	UINT32 *pszOptText,
	UINT32 actIndex,
	SINT32 yOffset
)
{
	int i;

	memset(&gsmenuCb, 0, sizeof(simpleMenuInfo_t));

	gsmenuCb.pszTitleText= pszTitle;
	gsmenuCb.numItems = (numItems>SMENU_ITEM_NUM_TOTAL) ? SMENU_ITEM_NUM_TOTAL : numItems;
	gsmenuCb.actIndex = (actIndex>=SMENU_ITEM_NUM_TOTAL) ? SMENU_ITEM_NUM_TOTAL-1 : actIndex;
	gsmenuCb.lastActIndex = gsmenuCb.actIndex;

	/*-- Construct coordinates & dimension */
	gsmenuCb.boxW = SMENU_WIDTH;
	gsmenuCb.boxH = ((gsmenuCb.numItems-1)*SMENU_DATA_CONT_H)+SMENU_DATA_LAST_H;
	gsmenuCb.boxH += ((NULL==pszTitle) ? SMENU_NULL_TITLE_H : SMENU_COLOR_TITLE_H);

	gsmenuCb.boxX = (320-gsmenuCb.boxW)>>1;
	gsmenuCb.boxY = ((240-gsmenuCb.boxH)>>1);
	gsmenuCb.boxY += yOffset;

	gsmenuCb.titleX = 160;	/*-- title text is BOTTOM-CENTER aligned */
	gsmenuCb.titleY = (NULL==pszTitle) ? gsmenuCb.boxY+SMENU_NULL_TITLE_H : gsmenuCb.boxY+SMENU_COLOR_TITLE_H;

	UINT32 itemX = gsmenuCb.boxX+32;
	UINT32 itemY = gsmenuCb.titleY+6;
	UINT8 *pszTmp;
	for (i=0; i<numItems; i++) {
		pszTmp = (UINT8 *)(*(pszOptText+i));
		gsmenuCb.pszItemText[i] = pszTmp;
		gsmenuCb.itemX[i] = itemX;
		gsmenuCb.itemY[i] = itemY+(i*SMENU_DATA_CONT_H);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_YesNoConfig
 *------------------------------------------------------------------------*/
static UINT8 smenuYesNo_TitleText[24]="Yes or No ?";
static UINT32 smenuYesNo_SelList[] =
		{(UINT32)"Yes",
		 (UINT32)"No",
		};
void SMENU_YesNoConfig(UINT8 *pszTitle)
{
	if (pszTitle) {
		memset(smenuYesNo_TitleText, 0, 24);
		strcpy((char*)smenuYesNo_TitleText, (const char*)pszTitle);
	}
	SMENU_Config(smenuYesNo_TitleText, 2, smenuYesNo_SelList, 0, -16);
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_YesNoShow
 *------------------------------------------------------------------------*/
void SMENU_YesNoShow(void)
{
	SMENU_Show();
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_YesNoClear
 *------------------------------------------------------------------------*/
void SMENU_YesNoClear(void)
{
	SMENU_Clear();
}

/*-------------------------------------------------------------------------
 *  File Name : SMENU_SelScan
 *------------------------------------------------------------------------*/
UINT32 SMENU_SelScan(void)
{
	UINT32 msg, param;
	UINT32 key;

	while(1) {
		sp5kHostMsgReceive(&msg, &param);

		if (msg==SP5K_MSG_AE_READY) {
			continue;
		}

		if (!IS_SP5K_BTN_MSG(msg)) {
			continue;
		}
		key = appBtnMsgToKey(msg, param);

		if (key==APP_KEY_PRESS_UP) {
			SMENU_Scroll(TRUE);
			continue;
		} else if (key==APP_KEY_PRESS_DOWN) {
			SMENU_Scroll(FALSE);
			continue;
		} else if (key==APP_KEY_PRESS_SET) {
			appBeep(TRUE, FALSE);
			return SMENU_SelectGet();
		}
	}
}

