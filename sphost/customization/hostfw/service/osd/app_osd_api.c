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

#include "sp5k_gfx_api.h"

#include "app_osd_api.h"
#include "app_osd.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
/*  
Make sure those coordinates,  (StartX, StartY) and (EndX, EndY)  are 
not overrun.  
*/
#define APPOSD_GFX_RECT_SHAPE_REVISE(pageId, StartX, StartY, EndX, EndY) \
    { \
        sp5kGfxPageCapabDesc_t p; \
        sp5kGfxPageCapabilityGet(pageId, &p); \
        if(EndX > p.frmW) \
        { \
            StartX -= (EndX - p.frmW)+1; \
            EndX -= (EndX - p.frmW)+1; \
        } \
        if(EndY > p.frmH) \
        { \
            StartY -= (EndY - p.frmH) + 1; \
            EndY -= (EndY - p.frmH) + 1; \
        } \
    } \
    //printf("(%d, %d) & (%d, %d)\n", StartX, EndX, StartY, EndY)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
SINT32 gbOsdFactorXNumerator ;
SINT32 gbOsdFactorXDenominator =1;
SINT32 gbOsdFactorYNumerator ;
SINT32 gbOsdFactorYDenominator =1;

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appOsdLibCoordScaleSet
 *------------------------------------------------------------------------*/
void
appOsdLib_CoordScaleSet(
	SINT32 *x,
	SINT32 *y
)
{
	*x = *x * gbOsdFactorXNumerator / gbOsdFactorXDenominator;
	*y = *y * gbOsdFactorYNumerator / gbOsdFactorYDenominator;
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_TextDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_TextDraw(
	SINT32 x,
	SINT32 y,
	UINT32 align,
    UINT8 *szStr
)
{
	appOsdLib_CoordScaleSet(&x, &y);
	appOsd_GfxStringDraw(x,y,align,(UINT8 *)szStr);
}
/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_TextPIPDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_TextPIPDraw(
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT8 *szStr
)
{
	appOsdLib_CoordScaleSet(&x, &y);
	appOsd_GfxStringPIPDraw(x,y,align,szStr);
}
/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_TextIDPIPDraw
 *------------------------------------------------------------------------*/
 void
 appOsdLib_TextIDPIPDraw(
 	SINT32 x, 
	SINT32 y,
	UINT32 align,
	UINT32 StrId
)
{
	appOsdLib_CoordScaleSet(&x, &y);
	appOsd_GfxStringIDPIPDraw(x,y,align,StrId);
}
/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_TextIDDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_TextIDDraw(
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT32 StrId
)
{
	appOsdLib_CoordScaleSet(&x, &y);
	appOsd_GfxStringIDDraw(x,y,align,StrId);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_OsdLayIconDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_OsdLayIconDraw(
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT32 IconId
)
{
#if defined(PRJ_17DVR)||defined(PRJ_19DVR)
	return;
#endif
	appOsdLib_CoordScaleSet(&x, &y);
	appOsd_GfxIconDraw(x,y,align,IconId);

}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_IconFileDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_IconFileDraw(
	UINT32 page,
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT8 *fileName,
	UINT32 IconIndex
)
{
	appOsdLib_CoordScaleSet(&x, &y);
	appOsd_GfxIconFileDraw(page,x,y,align,fileName,IconIndex);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_OsdLayerShapeDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_OsdLayerShapeDraw(
	UINT8 ShapeType,
	SINT32 StartX,
	SINT32 StartY,
	SINT32 Width,
	SINT32 High,
	SINT32 radiusX,
	SINT32 radiusY
)
{
	SINT32 EndX,EndY;
	appOsdLib_CoordScaleSet(&StartX, &StartY);
	appOsdLib_CoordScaleSet(&Width, &High);
	appOsdLib_CoordScaleSet(&radiusX, &radiusY);

	EndX = StartX+Width - 1;
	EndY = StartY+High - 1;

	switch(ShapeType)
	{
		case APP_OSD_SHAPE_RECT:
			APPOSD_GFX_RECT_SHAPE_REVISE(SP5K_GFX_PAGE_OSD_0, 
			StartX,StartY,EndX,EndY);
			appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0,StartX,StartY,EndX,EndY);
			
            break;
		case APP_OSD_SHAPE_CIRCLE:
			appOsd_GfxCircleShapeDraw(SP5K_GFX_PAGE_OSD_0,StartX,StartY,radiusX);
			break;
		case APP_OSD_SHAPE_ELLIPSE:
			appOsd_GfxEllipseShapeDraw(SP5K_GFX_PAGE_OSD_0,StartX,StartY,radiusX,radiusY);
			break;
		case APP_OSD_SHAPE_PIXEL:
			appOsd_GfxPixelShapeDraw(SP5K_GFX_PAGE_OSD_0,StartX,StartY);

		default:
			break;
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_OsdLayerLineDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_OsdLayerLineDraw(
	SINT32 x0,
	SINT32 y0,
	SINT32 x1,
	SINT32 y1
)
{
	appOsdLib_CoordScaleSet(&x0, &y0);
	appOsdLib_CoordScaleSet(&x1, &y1);

	appOsd_GfxLineShapeDraw(SP5K_GFX_PAGE_OSD_0,x0,y0,x1,y1);
}
/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_OsdLayerFrameDraw
 *------------------------------------------------------------------------*/
void 
appOsdLib_OsdLayerFrameDraw(
	SINT32 x, 
	SINT32 y,
	UINT32 w, 
	UINT32 h,
	UINT32 depth
)
{
	appOsdLib_CoordScaleSet(&x, &y);
	appOsdLib_CoordScaleSet((SINT32 *)&w, (SINT32 *)&h);

	if (depth > w) depth = w;
	if (depth > h) depth = h;

	appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0, x, y, x+w, y+depth);
	appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0, x+w-depth, y, x+w, y+h);
	appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0, x, y+h-depth, x+w, y+h);
	appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0, x, y, x+depth, y+h);

}
/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_PIPLayerShapeDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_PIPLayerShapeDraw(
	UINT8 ShapeType,
	SINT32 StartX,
	SINT32 StartY,
	SINT32 Width,
	SINT32 High,
	SINT32 radiusX,
	SINT32 radiusY
)
{
	SINT32 EndX,EndY;
	appOsdLib_CoordScaleSet(&StartX, &StartY);
	appOsdLib_CoordScaleSet(&Width, &High);
	appOsdLib_CoordScaleSet(&radiusX, &radiusY);

	EndX = StartX+Width - 1;
	EndY = StartY+High - 1;

	switch(ShapeType)
	{
		case APP_OSD_SHAPE_RECT:
		    APPOSD_GFX_RECT_SHAPE_REVISE(SP5K_GFX_PAGE_PIP_0, 
		    StartX,StartY,EndX,EndY);
			appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_0,StartX,StartY,EndX,EndY);
			break;
		case APP_OSD_SHAPE_CIRCLE:
			appOsd_GfxCircleShapeDraw(SP5K_GFX_PAGE_PIP_0,StartX,StartY,radiusX);
			break;
		case APP_OSD_SHAPE_ELLIPSE:
			appOsd_GfxEllipseShapeDraw(SP5K_GFX_PAGE_PIP_0,StartX,StartY,radiusX,radiusY);
			break;
		case APP_OSD_SHAPE_PIXEL:
			appOsd_GfxPixelShapeDraw(SP5K_GFX_PAGE_PIP_0,StartX,StartY);

		default:
			break;
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_PIPLayerLineDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_PIPLayerLineDraw(
	SINT32 x0,
	SINT32 y0,
	SINT32 x1,
	SINT32 y1
)
{
	appOsdLib_CoordScaleSet(&x0, &y0);
	appOsdLib_CoordScaleSet(&x1, &y1);

	appOsd_GfxLineShapeDraw(SP5K_GFX_PAGE_PIP_0,x0,y0,x1,y1);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_Osd1LayerShapeDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_TransparentLayerShapeDraw(
	UINT8 ShapeType,
	SINT32 StartX,
	SINT32 StartY,
	SINT32 Width,
	SINT32 High,
	SINT32 radiusX,
	SINT32 radiusY
)
{
	SINT32 EndX,EndY;
	appOsdLib_CoordScaleSet(&StartX, &StartY);
	appOsdLib_CoordScaleSet(&Width, &High);
	appOsdLib_CoordScaleSet(&radiusX, &radiusY);

	EndX = StartX+Width;
	EndY = StartY+High;

	switch(ShapeType)
	{
		case APP_OSD_SHAPE_RECT:
		    APPOSD_GFX_RECT_SHAPE_REVISE(SP5K_GFX_PAGE_OSD_0+1, 
		    StartX,StartY,EndX,EndY);
			appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_OSD_0+1,StartX,StartY,EndX,EndY);
			break;
		case APP_OSD_SHAPE_CIRCLE:
			appOsd_GfxCircleShapeDraw(SP5K_GFX_PAGE_OSD_0+1,StartX,StartY,radiusX);
			break;
		case APP_OSD_SHAPE_ELLIPSE:
			appOsd_GfxEllipseShapeDraw(SP5K_GFX_PAGE_OSD_0+1,StartX,StartY,radiusX,radiusY);
			break;
		case APP_OSD_SHAPE_PIXEL:
			appOsd_GfxPixelShapeDraw(SP5K_GFX_PAGE_OSD_0+1,StartX,StartY);

		default:
			break;
	}

}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_TransparentLayerShapeClear
 *------------------------------------------------------------------------*/
void
appOsdLib_TransparentLayerShapeClear(
	UINT8 ShapeType,
	SINT32 StartX,
	SINT32 StartY,
	SINT32 Width,
	SINT32 High,
	SINT32 radiusX,
	SINT32 radiusY
)
{

	appOsd_ColorTransparentShapeColorSet(PAL_NOCOLOR,PAL_NOCOLOR,PAL_BLEND_100);
	appOsdLib_TransparentLayerShapeDraw(ShapeType,StartX,StartY,Width,High,radiusX,radiusY);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_OsdLayerShapeClear
 *------------------------------------------------------------------------*/
void
appOsdLib_OsdLayerShapeClear(
	UINT8 ShapeType,
	SINT32 StartX,
	SINT32 StartY,
	SINT32 Width,
	SINT32 High,
	SINT32 radiusX,
	SINT32 radiusY
)
{
	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
	appOsdLib_OsdLayerShapeDraw(ShapeType,StartX,StartY,Width,High,radiusX,radiusY);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_OsdLayerLineClear
 *------------------------------------------------------------------------*/
void
appOsdLib_OsdLayerLineClear(
	SINT32 x0,
	SINT32 yo,
	SINT32 x1,
	SINT32 y1
)
{

	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
	appOsdLib_OsdLayerLineDraw(x0,yo,x1,y1);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_PIPLayerIconDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_PIPLayerIconDraw(
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT32 IconId
)
{
	appOsdLib_CoordScaleSet(&x, &y);
	appOsd_GfxPIPLayerIconDraw(x,y,align,IconId);

}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_PIPLayerShapeClear
 *------------------------------------------------------------------------*/
void
appOsdLib_PIPLayerShapeClear(
	UINT8 ShapeType,
	SINT32 StartX,
	SINT32 StartY,
	SINT32 Width,
	SINT32 High,
	SINT32 radiusX,
	SINT32 radiusY
)
{
	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
	appOsdLib_PIPLayerShapeDraw(ShapeType,StartX,StartY,Width,High,radiusX,radiusY);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_PIPLayerLineClear
 *------------------------------------------------------------------------*/
void
appOsdLib_PIPLayerLineClear(
	SINT32 x0,
	SINT32 yo,
	SINT32 x1,
	SINT32 y1
)
{
	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
	appOsdLib_PIPLayerLineDraw(x0,yo,x1,y1);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_PFLayerShapeDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_PFLayerShapeDraw(
	UINT8 ShapeType,
	SINT32 StartX,
	SINT32 StartY,
	SINT32 Width,
	SINT32 High,
	SINT32 radiusX,
	SINT32 radiusY
)
{
	SINT32 EndX,EndY;
	appOsdLib_CoordScaleSet(&StartX, &StartY);
	appOsdLib_CoordScaleSet(&Width, &High);
	appOsdLib_CoordScaleSet(&radiusX, &radiusY);

	EndX = StartX+Width - 1;
	EndY = StartY+High - 1;

	switch(ShapeType)
	{
		case APP_OSD_SHAPE_RECT:
		    APPOSD_GFX_RECT_SHAPE_REVISE(SP5K_GFX_PAGE_PF_0, 
		    StartX,StartY,EndX,EndY);
			appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PF_0,StartX,StartY,EndX,EndY);
			break;
		case APP_OSD_SHAPE_CIRCLE:
			appOsd_GfxCircleShapeDraw(SP5K_GFX_PAGE_PF_0,StartX,StartY,radiusX);
			break;
		case APP_OSD_SHAPE_ELLIPSE:
			appOsd_GfxEllipseShapeDraw(SP5K_GFX_PAGE_PF_0,StartX,StartY,radiusX,radiusY);
			break;
		case APP_OSD_SHAPE_PIXEL:
			appOsd_GfxPixelShapeDraw(SP5K_GFX_PAGE_PF_0,StartX,StartY);

		default:
			break;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_PFLayerLineDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_PFLayerLineDraw(
	SINT32 x0,
	SINT32 y0,
	SINT32 x1,
	SINT32 y1
)
{
	appOsdLib_CoordScaleSet(&x0, &y0);
	appOsdLib_CoordScaleSet(&x1, &y1);

	appOsd_GfxLineShapeDraw(SP5K_GFX_PAGE_PF_0,x0,y0,x1,y1);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_PFLayerShapeClear
 *------------------------------------------------------------------------*/
void
appOsdLib_PFLayerShapeClear(
	UINT8 ShapeType,
	SINT32 StartX,
	SINT32 StartY,
	SINT32 Width,
	SINT32 High,
	SINT32 radiusX,
	SINT32 radiusY
)
{
	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
	appOsdLib_PFLayerShapeDraw(ShapeType,StartX,StartY,Width,High,radiusX,radiusY);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_PFLayerLineClear
 *------------------------------------------------------------------------*/
void
appOsdLib_PFLayerLineClear(
	SINT32 x0,
	SINT32 yo,
	SINT32 x1,
	SINT32 y1
)
{

	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, 0x000000, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, 0x000000, 0, 0, 0);
	appOsdLib_PFLayerLineDraw(x0,yo,x1,y1);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_OsdLayerClearDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_OsdLayerClearDraw(
	void
)
{
	sp5kGfxPageClear(SP5K_GFX_PAGE_OSD_0,0);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_PIPLayerClearDraw
 *------------------------------------------------------------------------*/
void
appOsdLib_PIPLayerClearDraw(
	void
)
{
	sp5kGfxPageClear(SP5K_GFX_PAGE_PIP_0,0);

}

/*-------------------------------------------------------------------------
 *  File Name : appOsdLib_FontGetStringWidthbyId
 *------------------------------------------------------------------------*/

UINT32 appOsdLib_FontGetStringWidthbyId(UINT32 nStrId)
{
	UINT32 nWidth = 0;
	UINT32 nHeight = 0;

	sp5kGfxStringIdSizeGet(nStrId,&nWidth,&nHeight);

	return nWidth;

}

