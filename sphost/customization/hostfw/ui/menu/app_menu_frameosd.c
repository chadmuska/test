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
#include "sp5k_rsvblk_api.h"
#include "app_com_def.h"
#include "app_osd.h"
#include "app_osd_api.h"
#include "app_menu_frameosd.h"
#include "app_pal.h"
#include "app_iconfont1.h"
#include "app_stringid.h"
#include "app_calib.h"
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


/*-------------------------------------------------------------------------
 *  File Name : appMenu_TKCalibMenuFrameDraw
 *------------------------------------------------------------------------*/
void appMenu_TKCalibMenuFrameDraw(unsigned char bShow)
{

	if (!bShow)
	{
		return ;
	}

	appOsd_ColorDrawShapeColorSet(PAL_BLACK, PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0, 0, 0, 320, 240);
	appOsd_ColorDrawTextColorSet(PAL_WHITE, PAL_BLACK, PAL_NOCOLOR, PAL_BLEND_100);
	appOsdLib_TextDraw(TKTYPE_CALIBMENU_MAIN_STR_X,TKTYPE_MENUSTRING_OKSTART_Y,SP5K_GFX_ALIGN_TOP_LEFT,(UINT8 *)"OK: Enter");
}
