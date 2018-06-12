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
 
#ifndef _APP_OSD_H_
#define _APP_OSD_H_

#include "app_pal.h"
#include "app_icon_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define FONT_FILE_NAME_LEN		32
#define SP5K_ECODE_SUCCESS	    0

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
enum {
	GFX_SOFT_PAL_TYPE_TEXT,
	GFX_SOFT_PAL_TYPE_ICON,
	GFX_SOFT_PAL_TYPE_MAX
};

enum {
	GFX_SOFT_PAL_BACK,
	GFX_SOFT_PAL_FORE,
	GFX_SOFT_PAL_EDGE,
	GFX_SOFT_PAL_OTHER,
	GFX_SOFT_PAL_MAX
};

typedef struct appFontInfo_s{
	unsigned char CurFontFile[FONT_FILE_NAME_LEN];
	unsigned char CurIconFile[FONT_FILE_NAME_LEN];
	unsigned char CurStrTblFile[FONT_FILE_NAME_LEN];
}appFontInfo_t, *PappFontInfo_t;

typedef struct appOsdColorSetting_s{
	UINT8  textcolorTbl[4];
	UINT8  blendTbl[16];
} appOsdColorSetting_t;


/**************************************************************************
 *                           VARIABLE                          *
 **************************************************************************/
extern UINT32 osdwidth,osdheight;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appOsd_ColorTextColorSet(UINT8 TextColor, UINT8 ShadowColor, UINT8 BKColor);
void appOsd_ColorIconColorSet(UINT8 ChangeColor, UINT8 DrawColor);
void appOsd_ColorBlendingSet(UINT8 index,UINT8 blendTb);
void appOsd_ColorDrawShapeColorSet(COLOR_DEF BrushColor, BLEND_TYPE BrushBlend, COLOR_DEF PanColor, BLEND_TYPE PanBlend);
void appOsd_ColorDrawTextColorSet(COLOR_DEF enColor, COLOR_DEF enShadowColor, COLOR_DEF enBkColor, BLEND_TYPE enBkBlend);
void appOsd_ColorTransparentShapeColorSet(COLOR_DEF BrushColor,COLOR_DEF PanColor, BLEND_TYPE Blend);

void appOsd_FontTextFontSet(unsigned char *szFontFile);
void appOsd_FontTextStringSet(unsigned char *szStrTblFile);
void appOsd_FontIconFontSet(unsigned char *szIconFile);
void appOsd_FontIconPreLoadSet(UINT32 PreLoadID,unsigned char *szIconFile);
UINT32 appOsd_GfxPIPLayerIconDraw(SINT32 x, SINT32 y, UINT32 align, UINT32 IconIndex);

UINT32 appOsd_GfxStringDraw(SINT32 x, SINT32 y, UINT32 align,UINT8 *str);
UINT32 appOsd_GfxStringIDDraw(SINT32 x,SINT32 y,UINT32 align,UINT32 StrIndex);
UINT32 appOsd_GfxIconDraw(SINT32 x,SINT32 y,UINT32 align,UINT32 IconIndex);
UINT32 appOsd_GfxStringPIPDraw(SINT32 x, SINT32 y, UINT32 align, UINT8 *str);
UINT32 appOsd_GfxStringIDPIPDraw(SINT32 x, SINT32 y, UINT32 align, UINT32 StrIndex);
UINT32 appOsdSFNFileLoad(UINT32 id,UINT8 *pfilename);
UINT32 appOsd_GfxIconFileDraw(UINT32 page,SINT32 x,SINT32 y,UINT32 align,UINT8 *fileName,UINT32 IconIndex);
UINT32 appOsd_GfxJPGFileImgLayDraw(unsigned char *bkfilename,UINT32 diStartX,UINT32 diStartY,UINT32 diWidth,UINT32 diHeight);
UINT32 appOsd_GfxJPGFilePIPLayDraw(unsigned char *bkfilename,UINT32 diStartX,UINT32 diStartY,UINT32 diWidth,UINT32 diHeight);
UINT32 appOsd_GfxJPGFilePFLayDraw(unsigned char *bkfilename,UINT32 diStartX,UINT32 diStartY,UINT32 diWidth,UINT32 diHeight);
UINT32 appOsd_GfxRectShapeDraw(UINT32 page,SINT32 posX,SINT32 posY,SINT32 posX1,SINT32 posY1);
UINT32 appOsd_GfxCircleShapeDraw(UINT32 page,SINT32 posX,SINT32 posY,SINT32 radiusX);
UINT32 appOsd_GfxEllipseShapeDraw(UINT32 page,SINT32 posX,SINT32 posY,SINT32 radiusX,SINT32 radiusY);
UINT32 appOsd_GfxLineShapeDraw(UINT32 page,SINT32 posX,SINT32 posY,SINT32 posX1,SINT32 posY1);
UINT32 appOsd_GfxPixelShapeDraw(UINT32 page,SINT32 posX,SINT32 posY);
void appOsd_GfxLineShapeWidthSet(UINT32 wThickness);
void appOsd_GfxOsdRefreshSet(UINT32 nshow);
void appOsd_GfxIconIDSet(UINT32 id);
void appOsd_GfxFontIDSet(UINT32 id);
unsigned char* appOsd_FontIconResSelect();
#endif /* _APP_OSD_H_ */

