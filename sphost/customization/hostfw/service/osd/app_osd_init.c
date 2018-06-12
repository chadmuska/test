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
#include "gpio_custom.h"

#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_rsvblk_api.h"
#include "app_osd_api.h"
#include "app_osd.h"
#include "app_osdfont.h"
#include "app_setup_lib.h"
#include "app_video.h"
#include "app_ui_para.h"
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

extern SINT32 gbOsdFactorXNumerator ;
extern SINT32 gbOsdFactorXDenominator ;
extern SINT32 gbOsdFactorYNumerator ;
extern SINT32 gbOsdFactorYDenominator ;
extern appOsdColorSetting_t appOsdCtrl;
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

UINT32 osdwidth, osdheight;

static UINT8 blendTbl[16] __attribute__ ((unused)) ={4,4,4,4, 4,4,4,4,4,4,4,4,4,4,4,4};
static UINT16 pltTbl[]= {
	/* 0 - 7 */
	//RGB888_TO_RGB565(COLOR_TRANSPARENT),
	RGB888_TO_RGB565(COLOR_BLACK),
	RGB888_TO_RGB565(COLOR_GREEN_2),
	RGB888_TO_RGB565(COLOR_GREEN_3),
	RGB888_TO_RGB565(COLOR_GREEN_4),
	RGB888_TO_RGB565(COLOR_YELLOW_1),
	RGB888_TO_RGB565(COLOR_BROWN_1),
	RGB888_TO_RGB565(COLOR_RED),
	/* 8- */
	RGB888_TO_RGB565(COLOR_DARKBLUE_1),
	RGB888_TO_RGB565(COLOR_WHITE),
	RGB888_TO_RGB565(COLOR_GRAY_5),
	RGB888_TO_RGB565(COLOR_LIGHTBLUE_1),
	RGB888_TO_RGB565(COLOR_GRAY_2),
	RGB888_TO_RGB565(COLOR_WHITE),
	RGB888_TO_RGB565(COLOR_GRAY_1),
	RGB888_TO_RGB565(COLOR_GRAY_2),
	/* 16- */
	RGB888_TO_RGB565(COLOR_GRAY_3),  /* transparent */
	RGB888_TO_RGB565(COLOR_PURPLE),  /* transparent */
	RGB888_TO_RGB565(COLOR_GREEN),  /* transparent */
	RGB888_TO_RGB565(COLOR_ORANGE),
	RGB888_TO_RGB565(COLOR_WHITE_1),
	RGB888_TO_RGB565(COLOR_PURPLE_1),
	RGB888_TO_RGB565(COLOR_DARK_BLUE),
	RGB888_TO_RGB565(COLOR_IRON),
	/* 24- */
	RGB888_TO_RGB565(COLOR_GRAY_4),
	RGB888_TO_RGB565(COLOR_RED_1),
	RGB888_TO_RGB565(COLOR_GREEN_1),
	RGB888_TO_RGB565(COLOR_YELLOW),
	RGB888_TO_RGB565(COLOR_BLUE_2),
	RGB888_TO_RGB565(COLOR_PINK_4),
	RGB888_TO_RGB565(COLOR_LIGHT_BLUE),
	RGB888_TO_RGB565(COLOR_RED_2),
	/* 32- */
};

/*-------------------------------------------------------------------------
 *  File Name : appOsd_OSDColorBlendingInit
 *------------------------------------------------------------------------*/
void appOsd_OSDColorBlendingInit(void)
{
	UINT32 i;

	appOsdCtrl.blendTbl[0]= 0;
	for (i=1; i<16; i++)
	{
		appOsdCtrl.blendTbl[i]= 4;
	}
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_BLEND_TABLE, (SINT32)appOsdCtrl.blendTbl);
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_COLOR_KEY, SP5K_DISP_COLOR_KEY_RGB565, 0x000000, 0);
}

void appPalInit(void)
{
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_PALETTE, (SINT32)pltTbl, 1, COUNT_OF(pltTbl));
	appOsd_OSDColorBlendingInit();
}

void
appOsdResPreload(
	void
)
{
#if	!SP5K_ULTRA_FAST_BOOT
profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appOsdResPreload s");

	uiPara_t* puiPara = appUiParaGet();

	profLogAdd(0, "osd preload :s");
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON, ID_FONT_ICON);
	appOsdSFNFileLoad(SP5K_RES_DATE_STAMP_OSD, (UINT8 *)ID_FONT_DATESTAMP);
	appSetupMenu_SetCurLanguage(puiPara->Language);
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_3,(UINT8 *)ID_ICON_WARNMEDIASTANDBY);
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_4, (UINT8 *)ID_FONT_LOGO);
	profLogAdd(0, "osd preload :e");


profLogAdd(LEVEL_TRACE,"ULTRA_FAST_BOOT:appOsdResPreload e");
#endif
}
void
appMenuResPreload(
	void
)
{
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_2, (UINT8 *)ID_ICONFILE_MUBODY);
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_3, (UINT8 *)ID_ICONFILE_MUSUSBAR);
	appOsdSFNFileLoad(SP5K_RES_OSD_ICON_4, (UINT8 *)ID_ICONFILE_MUSUBSC);
}

void
appOsdInit(
	void
)
{
	HOST_PROF_LOG_ADD(LEVEL_INFO, "osdinit: init :s");
	profLogPrintf(LEVEL_TRACE,"%s s",__FUNCTION__);
	appPalInit();
	appOsdLib_OsdLayerClearDraw();
	sp5kGfxAttrSet(SP5K_GFX_TRANS_COLOR, 0, 0, 0, 0);
	appOsd_ColorTextColorSet(PAL_WHITE,PAL_NOCOLOR,PAL_NOCOLOR);
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_BG_COLOR, 0, 0, 0);

	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_ACTIVE, 1);

	sp5kGfxAttrSet(SP5K_GFX_FILL_ENABLE, 1, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PEN_ENABLE, 1, 0, 0, 0);

	appOsd_ColorIconColorSet(PAL_GRAY_1,PAL_NOCOLOR);

	HOST_PROF_LOG_ADD(LEVEL_INFO, "osdinit: init :e");
	profLogPrintf(LEVEL_TRACE,"%s e",__FUNCTION__);
	/*appOsdResPreload();*/
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdInit_TVModeSet
 *------------------------------------------------------------------------*/
void
appOsdInit_TVModeSet(
	unsigned char dispMode
)
{
	if(IS_TV_IN){
		sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);

#if TEST_ADAS
		sp5kGfxInitCfgSet(SP5K_GFX_OSD_PAGE_TOT, 2, 0, 0, 0);
#else
		sp5kGfxInitCfgSet(SP5K_GFX_OSD_PAGE_TOT, 1, 0, 0, 0);
#endif
		sp5kGfxInitCfgSet(SP5K_GFX_OSD_FMT, SP5K_GFX_FMT_IDX8, 0, 0, 0);
		sp5kGfxInitCfgSet(SP5K_GFX_OSD_FRAME_SIZE, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);

#if TEST_ADAS
		sp5kGfxInitCfgSet(SP5K_GFX_PIP_PAGE_TOT, 2, 0, 0, 0);
#else
		sp5kGfxInitCfgSet(SP5K_GFX_PIP_PAGE_TOT, 1, 0, 0, 0);
#endif
		sp5kGfxInitCfgSet(SP5K_GFX_PIP_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
		sp5kGfxInitCfgSet(SP5K_GFX_PIP_FRAME_SIZE,PIP_DATESTAMP_FRAMSIZE_W, PIP_DATESTAMP_FRAMSIZE_H, 0, 0);
		sp5kGfxInitCfgSet(SP5K_GFX_PIP_WINDOW, 0, 0, PIP_DATESTAMP_FRAMSIZE_W, PIP_DATESTAMP_FRAMSIZE_H);
#else
		sp5kGfxInitCfgSet(SP5K_GFX_PIP_FRAME_SIZE,OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);
#endif
		sp5kGfxInitCfgSet(SP5K_GFX_PF_PAGE_TOT, 1, 0, 0, 0);
		sp5kGfxInitCfgSet(SP5K_GFX_PF_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
		sp5kGfxInitCfgSet(SP5K_GFX_PF_FRAME_SIZE,OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);

		sp5kGfxInitCfgSet(SP5K_GFX_OSD_WITH_ALPHA_PLANE, 1, 0, 0, 0);
		sp5kGfxInit();

		sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0, 1, 0, 0);
		sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_BG_COLOR, 0, 0, 0);
	}

	gbOsdFactorXNumerator = OSD_FACTOR_X_NUMERATOR;
	gbOsdFactorXDenominator = OSD_FACTOR_X_DENOMINATOR;
	gbOsdFactorYNumerator = OSD_FACTOR_Y_NUMERATOR;
	gbOsdFactorYDenominator = OSD_FACTOR_Y_DENOMINATOR;

	#if (KIT_WITH_HDMI)
	if(IS_HDMI_IN)
	#endif
	{
		gbOsdFactorXNumerator = HDMI_OSD_FACTOR_X_NUMERATOR;
		gbOsdFactorYNumerator = HDMI_OSD_FACTOR_Y_NUMERATOR;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdInit_LCDModeSet
 *------------------------------------------------------------------------*/
void appOsdInit_LCDModeSet(
	UINT32 wSize,
	UINT32 hSize
)
{
	sp5kGfxInitCfgSet(SP5K_GFX_AB_FRAME_SIZE, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);

#if TEST_ADAS
    sp5kGfxInitCfgSet(SP5K_GFX_OSD_PAGE_TOT, 2, 0, 0, 0);
#else
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_PAGE_TOT, 1, 0, 0, 0);
#endif
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FMT, SP5K_GFX_FMT_IDX8, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_FRAME_SIZE, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_OSD_WINDOW, 0, 0, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H);

#if TEST_ADAS
    sp5kGfxInitCfgSet(SP5K_GFX_PIP_PAGE_TOT, 2, 0, 0, 0);
#else
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_PAGE_TOT, 1, 0, 0, 0);
#endif

	sp5kGfxInitCfgSet(SP5K_GFX_PIP_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_FRAME_SIZE,wSize, hSize, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PIP_WINDOW, 0, 0, wSize, hSize);

	sp5kGfxInitCfgSet(SP5K_GFX_PF_PAGE_TOT, 1, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_FMT, SP5K_GFX_FMT_RGB565, 0, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_FRAME_SIZE,OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H, 0, 0);
	sp5kGfxInitCfgSet(SP5K_GFX_PF_WINDOW, 0, 0, OSD_DEFAULT_FRAMSIZE_W, OSD_DEFAULT_FRAMSIZE_H);

	sp5kGfxInitCfgSet(SP5K_GFX_OSD_WITH_ALPHA_PLANE, 1, 0, 0, 0);

	sp5kGfxInit();

	sp5kGfxAttrSet(SP5K_GFX_LAYER_ACTIVE, SP5K_GFX_PAGE_PF_0, 1, 0, 0);
	sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_BG_COLOR, 0, 0, 0);
	/* Avoid black color is set to transparency */
	sp5kGfxAttrSet(SP5K_GFX_YUV422_TO_RGB565_DITHER_ENABLE, 1, 0, 0, 0);

	gbOsdFactorXNumerator = OSD_FACTOR_X_NUMERATOR;
	gbOsdFactorXDenominator = OSD_FACTOR_X_DENOMINATOR;
	gbOsdFactorYNumerator = OSD_FACTOR_Y_NUMERATOR;
	gbOsdFactorYDenominator = OSD_FACTOR_Y_DENOMINATOR;
}

