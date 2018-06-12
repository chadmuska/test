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
 *	File name    : App_errorosd.c
 *	Function     : The implementation which Show error information on the osd
 *	First editor :   
 *	First date   :   
 *	History      :
 *               : 2018.06.06  Edward.Lu  "Add description and modify code"
 **************************************************************************/

#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "app_com_def.h"
#include "app_osd.h"
#include "app_osd_api.h"
#include "app_pal.h"
#include "app_osdfont.h"
#include "app_iconfont1.h"
#include "app_icon_def.h"
#include "app_stringid.h"
#include "app_errorosd.h"
#include "app_menu_frameosd.h"
#include "gpio_custom.h"

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

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name    : appOsdMessageBoxShow
* Function         : The implementation which Show error information on the osd.
* Return value	   : void
* Parameter1	   : bShow (BOOL TRUE/FALSE)
* Parameter2	   : usStrId (unsigned short / app_stringid.h)
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void  appOsdMessageBoxShow(BOOL bShow, unsigned short usStrId)
{

	UINT32 Warn_X,Warn_Y,FRAME_X,FRAME_W;
	
	if(appOsdLib_FontGetStringWidthbyId(usStrId)<190 /*The width of the string in unit of pixel.*/)
	{
    	Warn_X = TK_WARNING_X+(TK_WARNING_W/2);
     	Warn_Y = TK_WARNING_Y+(TK_WARNING_H-50);
		FRAME_X = TK_WARNING_X;
		FRAME_W = TKTYPE_WARNING_TEXT_CLEAR_W;
		appMenu_TKTypeWarningMsgFrameDraw(bShow);
	}
	else
	{
		Warn_X = TK_WARNING_FRAME2_X+(TK_WARNING_FRAME2_W/2);
		Warn_Y = TK_WARNING_FRAME2_Y+(TK_WARNING_FRAME2_H-50);
		FRAME_X = TK_WARNING_FRAME2_X;
		FRAME_W = TKTYPE_WARNING_FRAME2_TEXT_CLEAR_W;
		appMenu_TKTypeWarningMsgBigFrame2Draw(bShow);
	
	}

	if(bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		#if (KIT_WITH_HDMI)
		if(IS_HDMI_IN){
			appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, 112, Warn_Y, 96, TKTYPE_WARNING_TEXT_CLEAR_H, 0, 0);
		}else
		#endif
		{
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, FRAME_X+2, Warn_Y,FRAME_W, TKTYPE_WARNING_TEXT_CLEAR_H, 0, 0);
		}
		appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_TextIDDraw(Warn_X,Warn_Y,SP5K_GFX_ALIGN_TOP,usStrId);		
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appErrorOsd_VoiceMemoWarnScreenDraw
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void appErrorOsd_VoiceMemoWarnScreenDraw(BOOL bShow, unsigned short usStrId)
{

	unsigned int Warn_X,Warn_Y;
	
	Warn_X = TK_WARNING_X+(TK_WARNING_W/2);
	Warn_Y = TKTYPE_WARNING_VOICEMEMO_Y;


	if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X+2, Warn_Y,TKTYPE_WARNING_W-8,32, 0, 0);
		return ;
	}

	if(usStrId==STRTK_RDY_TO_PLAY||usStrId==STRTK_PLAYING_AUDIO||usStrId==STRTK_PAUSE)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X-6, Warn_Y,TKTYPE_WARNING_W+12,32, 0, 0);
	}
	else
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X+2, Warn_Y,TKTYPE_WARNING_W-8,32, 0, 0);
	}
	appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(Warn_X, Warn_Y, SP5K_GFX_ALIGN_CENTER, usStrId);
}

/*--------------------------------------------------------------------------*
* Function name    : appErrorOsd_BlackWarnScreen
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void  appErrorOsd_BlackWarnScreen(BOOL bShow, unsigned short usStrId)
{

	unsigned int Warn_X,Warn_Y;

	Warn_X = TK_WARNING_X+(TK_WARNING_W/2);
	Warn_Y = TK_WARNING_Y+(TK_WARNING_H-50);
	
	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X+2, Warn_Y,TKTYPE_WARNING_W-8,TKTYPE_WARNING_H-50, 0, 0);

	if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X+2, Warn_Y,TKTYPE_WARNING_W-8,TKTYPE_WARNING_H-50, 0, 0);
		return ;
	}
	appOsd_ColorDrawShapeColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextIDDraw(Warn_X, Warn_Y, SP5K_GFX_ALIGN_CENTER, usStrId);

}

/*--------------------------------------------------------------------------*
* Function name    : appMenu_TKTypeWarningMsgFrameDraw
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appMenu_TKTypeWarningMsgFrameDraw(
	unsigned char bShow
)
{
	if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,TKTYPE_WARNING_X, TKTYPE_WARNING_Y,TKTYPE_WARNING_W,TKTYPE_WARNING_H,0,0);
		appOsdLib_TransparentLayerShapeClear(APP_OSD_SHAPE_RECT,TKTYPE_WARNING_X+2,TKTYPE_WARNING_Y+2,TKTYPE_WARNING_W-4,TKTYPE_WARNING_H-4,0,0);
		return ;
	}

	appOsd_ColorBlendingSet(PAL_GRAY_1,TKTYPE_MENU_BLEND);
	appOsd_ColorTransparentShapeColorSet(PAL_GRAY_1,PAL_GRAY_1,TKTYPE_MENU_BLEND);
	appOsdLib_TransparentLayerShapeDraw(APP_OSD_SHAPE_RECT,TKTYPE_WARNING_X+2,TKTYPE_WARNING_Y+2,TKTYPE_WARNING_W-4,TKTYPE_WARNING_H-4,0,0);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_X, TKTYPE_WARNING_Y,TKTYPE_WARNING_W, TKTYPE_WARNING_H, 0, 0);

	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,160,130,SP5K_GFX_ALIGN_CENTER,(UINT8 *)ID_WARNFRAME,0x0000);
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,160,110,SP5K_GFX_ALIGN_CENTER,(UINT8 *)ID_WARNICON,0x0000);
	}else
	#endif
	{
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,TKTYPE_WARNING_X,TKTYPE_WARNING_Y,SP5K_GFX_ALIGN_TOP_LEFT,(UINT8 *)ID_WARNFRAME,0x0000);
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,TKTYPE_WARNING_ICON_X,TKTYPE_WARNING_ICON_Y,SP5K_GFX_ALIGN_TOP_LEFT,(UINT8 *)ID_WARNICON,0x0000);
	}
}

/*--------------------------------------------------------------------------*
* Function name    : appMenu_TKTypeWarningMsgBigFrame2Draw
* Function         : 
* Return value	   : 
* Parameter1	   : 
* History          :
*                  : 2018.06.06  Edward.Lu  "Add description and modify code"
*--------------------------------------------------------------------------*/

void
appMenu_TKTypeWarningMsgBigFrame2Draw(
	unsigned char bShow
)
{
	if (!bShow)
	{
		appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
		appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT,TKTYPE_WARNING_FRAME2_X, TKTYPE_WARNING_FRAME2_Y,TKTYPE_WARNING_FRAME2_W,TKTYPE_WARNING_FRAME2_H,0,0);
		appOsdLib_TransparentLayerShapeClear(APP_OSD_SHAPE_RECT,TKTYPE_WARNING_FRAME2_X+2,TKTYPE_WARNING_FRAME2_Y+2,TKTYPE_WARNING_FRAME2_W-4,TKTYPE_WARNING_FRAME2_H-4,0,0);
		return ;
	}

	appOsd_ColorBlendingSet(PAL_GRAY_1,TKTYPE_MENU_BLEND);
	appOsd_ColorTransparentShapeColorSet(PAL_GRAY_1,PAL_GRAY_1,TKTYPE_MENU_BLEND);
	appOsdLib_TransparentLayerShapeDraw(APP_OSD_SHAPE_RECT,TKTYPE_WARNING_FRAME2_X+2,TKTYPE_WARNING_FRAME2_Y+2,TKTYPE_WARNING_FRAME2_W-4,TKTYPE_WARNING_FRAME2_H-4,0,0);

	appOsd_ColorDrawShapeColorSet(PAL_NOCOLOR, PAL_BLEND_100, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_OsdLayerShapeDraw(APP_OSD_SHAPE_RECT, TKTYPE_WARNING_FRAME2_X, TKTYPE_WARNING_FRAME2_Y,TKTYPE_WARNING_FRAME2_W, TKTYPE_WARNING_FRAME2_H, 0, 0);

	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN){
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,160,130,SP5K_GFX_ALIGN_CENTER,(UINT8 *)ID_WARNFRAME,0x0000);
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,160,110,SP5K_GFX_ALIGN_CENTER,(UINT8 *)ID_WARNICON,0x0000);
	}else
	#endif
	{
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,TKTYPE_WARNING_FRAME2_X,TKTYPE_WARNING_FRAME2_Y,SP5K_GFX_ALIGN_TOP_LEFT,(UINT8 *)ID_WARNFRAME2,0x0000);
		appOsdLib_IconFileDraw(SP5K_GFX_PAGE_OSD_0,TKTYPE_WARNING_FRAME2_ICON_X,TKTYPE_WARNING_FRAME2_ICON_Y,SP5K_GFX_ALIGN_TOP_LEFT,(UINT8 *)ID_WARNICON,0x0000);
	}
}

