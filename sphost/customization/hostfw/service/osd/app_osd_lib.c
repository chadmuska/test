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

#include "sp5k_disp_api.h"
#include "sp5k_gfx_api.h"
#include "sp5k_rsvblk_api.h"

#include "app_osd.h"
#include "customization/app_init.h"
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define OSD_KEEPER_MAX	COUNT_OF(osdColorKeeper)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
appFontInfo_t gsappCurFont;
appOsdColorSetting_t appOsdCtrl;
struct {
	UINT8 key;
	UINT8 prev,cur;
} osdColorKeeper[]={
	{ SP5K_GFX_TRANS_COLOR,0,0 },
	{ SP5K_GFX_PEN_COLOR,0,0 },
 	{ SP5K_GFX_FILL_COLOR,0,0 },
};

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern const rsvSize_t OSDRsvSize[];
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*-------------------------------------------------------------------------
 *  File Name : appOsd_ColorTextColorSet
 *------------------------------------------------------------------------*/
void
appOsd_ColorTextColorSet(
	UINT8 TextColor,
	UINT8 ShadowColor,
	UINT8 BKColor
)
{

	appOsdCtrl.textcolorTbl[0] = BKColor;    /* background color */
	appOsdCtrl.textcolorTbl[1] = TextColor;     /* text color */
	appOsdCtrl.textcolorTbl[2] = ShadowColor;    /* outline color */
	appOsdCtrl.textcolorTbl[3] = 0;

	/* config osd text mapping table */
	sp5kGfxTextColorSet(appOsdCtrl.textcolorTbl, 0, 4);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_ColorIconColorSet
 *------------------------------------------------------------------------*/
void
appOsd_ColorIconColorSet(
	UINT8 ChangeColor,
	UINT8 DrawColor
)
{
	UINT8 tmp[1];
	tmp[0] = DrawColor;
	sp5kGfxIconColorSet(tmp, ChangeColor, 1);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_ColorBlendingSet
 *------------------------------------------------------------------------*/
void appOsd_ColorBlendingSet(
	UINT8 index,
	UINT8 blendTb
)
{
	//Blending Table with a total of 16 entiies
	//Only 5 levels of blending 0%,25%,50%,75%,100%

	if (index ==0){
		if (blendTb != PAL_BLEND_0){
			blendTb = PAL_BLEND_0;
		}
	}

	if (index<=15){
		if (appOsdCtrl.blendTbl[index] !=blendTb){
			appOsdCtrl.blendTbl[index]= blendTb;
			sp5kDispAttrSet(sp5kDispActChnlGet(0), SP5K_DISP_OSD_BLEND_TABLE, (SINT32)appOsdCtrl.blendTbl);
		}else{
			return;
		}
	}else{
		return;
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_ColorDrawShapeColorSet
 *------------------------------------------------------------------------*/
void
appOsd_ColorDrawShapeColorSet(
	COLOR_DEF BrushColor,
	BLEND_TYPE BrushBlend,
	COLOR_DEF PanColor,
	BLEND_TYPE PanBlend
)
{
	if (BrushColor<=15){
		appOsd_ColorBlendingSet(BrushColor,BrushBlend);
	}

	if (PanColor<=15){
		appOsd_ColorBlendingSet(PanColor,PanBlend);
	}

	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, BrushColor, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PanColor, 0, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_ColorDrawTextColorSet
 *------------------------------------------------------------------------*/
void
appOsd_ColorDrawTextColorSet(
	COLOR_DEF enColor,
	COLOR_DEF enShadowColor,
	COLOR_DEF enBkColor,
	BLEND_TYPE enBkBlend
)
{
	//Set Text Color
	appOsd_ColorTextColorSet(enColor,enShadowColor,enBkColor);

	//Set Background and shadow Blending
	appOsd_ColorBlendingSet(enBkColor,enBkBlend);
	appOsd_ColorBlendingSet(enShadowColor,enBkBlend);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_ColorTransparentShapeColorSet
 *------------------------------------------------------------------------*/
void
appOsd_ColorTransparentShapeColorSet(
	COLOR_DEF BrushColor,
	COLOR_DEF PanColor,
	BLEND_TYPE Blend
)
{
	UINT32 BlendSet = 8*Blend;
	sp5kGfxAttrSet(SP5K_GFX_FILL_COLOR, BrushColor, 0, 0, 0);
	sp5kGfxAttrSet(SP5K_GFX_PEN_COLOR, PanColor, 0, 0, 0);

	if ((BrushColor<=15)&&(PanColor<=15)){

		sp5kGfxAttrSet(SP5K_GFX_LAYER_ALPHA,SP5K_GFX_PAGE_OSD_0+1,BlendSet,0,0);
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_FontIconResSelect
 *------------------------------------------------------------------------*/
unsigned char*
appOsd_FontIconResSelect()
{
	char *pszIconFile;
	
	#if KIT_WITH_HDMI
	if(IS_HDMI_IN)
	{
		pszIconFile = "UI\\ICON\\ICON_48.SFN";
	}
	else
	#endif
	{
		pszIconFile = "UI\\ICON\\ICON_32.SFN";
	}

	return (unsigned char *)pszIconFile;
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_FontTextFontSet
 *------------------------------------------------------------------------*/
void
appOsd_FontTextFontSet(
	unsigned char *szFontFile
)
{
	if (strcmp((const char *)gsappCurFont.CurFontFile, (const char *)szFontFile))
	{
		if(sp5kResourceFileLoad(SP5K_RES_OSD_FONT,(const char*)szFontFile) == SP5K_ECODE_SUCCESS){
			strcpy((char *)gsappCurFont.CurFontFile, (const char *)szFontFile);
		}
		else{
			HOST_PROF_LOG_ADD(LEVEL_ERROR, "osd lib: load font fail");
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_FontTextStringSet
 *------------------------------------------------------------------------*/
void
appOsd_FontTextStringSet(
	unsigned char *szStrTblFile
)
{
	if (strcmp((const char *)gsappCurFont.CurStrTblFile, (const char *)szStrTblFile))
	{
		if(sp5kResourceFileLoad(SP5K_RES_OSD_STRING,(const char*)szStrTblFile) == SP5K_ECODE_SUCCESS){
			strcpy((char *)gsappCurFont.CurStrTblFile, (const char *)szStrTblFile);
		}
		else{
			HOST_PROF_LOG_ADD(LEVEL_ERROR, "osd lib: load string fail");
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_FontIconFontSet
 *------------------------------------------------------------------------*/
void appOsd_FontIconFontSet(unsigned char *szIconFile)
{
	if (strcmp((const char *)gsappCurFont.CurIconFile, (const char *)szIconFile))
	{
		if(sp5kResourceFileLoad(SP5K_RES_OSD_ICON,(const char*)szIconFile) == SP5K_ECODE_SUCCESS){
			strcpy((char *)gsappCurFont.CurIconFile, (const char *)szIconFile);
		}
		else{
			HOST_PROF_LOG_ADD(LEVEL_ERROR, "osd lib: load icon fail");
		}
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_FontIconPreLoadSet
 *------------------------------------------------------------------------*/
void
appOsd_FontIconPreLoadSet(
	UINT32 PreLoadID,
	unsigned char *szIconFile
)
{
	if(sp5kResourceFileLoad(PreLoadID,(const char*)szIconFile) == SP5K_ECODE_SUCCESS){
		//
	}
	else{
		 HOST_PROF_LOG_ADD(LEVEL_ERROR, "osd lib: pre load fail");
	}
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxStringDraw
 *------------------------------------------------------------------------*/
UINT32
appOsd_GfxStringDraw(
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT8 *str
)
{
	return sp5kGfxStringDraw(SP5K_GFX_PAGE_OSD_0, x, y, align, SP5K_GFX_ENCODE_ASCII, str);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxStringIDDraw
 *------------------------------------------------------------------------*/
UINT32
appOsd_GfxStringIDDraw(
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT32 StrIndex
)
{
	return sp5kGfxStringIdDraw(SP5K_GFX_PAGE_OSD_0, x, y, align,StrIndex);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxIconDraw
 *------------------------------------------------------------------------*/
UINT32
appOsd_GfxIconDraw(
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT32 IconIndex
)
{
	return sp5kGfxIconDraw(SP5K_GFX_PAGE_OSD_0, x, y,align, IconIndex);
}
/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxStringPIPDraw
 *------------------------------------------------------------------------*/
UINT32 
appOsd_GfxStringPIPDraw(
	SINT32 x, 
	SINT32 y, 
	UINT32 align, 
	UINT8 *str
)
{
	return sp5kGfxStringDraw(SP5K_GFX_PAGE_PIP_0, x, y, align, SP5K_GFX_ENCODE_ASCII, str);
}
/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxStringIDPIPDraw
 *------------------------------------------------------------------------*/
UINT32 
appOsd_GfxStringIDPIPDraw(
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT32 StrIndex
)
{
	return sp5kGfxStringIdDraw(SP5K_GFX_PAGE_PIP_0, x, y, align,StrIndex);
}
/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxIconDraw
 *------------------------------------------------------------------------*/
UINT32
appOsd_GfxPIPLayerIconDraw(
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT32 IconIndex
)
{
	return sp5kGfxIconDraw(SP5K_GFX_PAGE_PIP_0, x, y,align, IconIndex);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxIconFileDraw
 *------------------------------------------------------------------------*/
UINT32
appOsd_GfxIconFileDraw(
	UINT32 page,
	SINT32 x,
	SINT32 y,
	UINT32 align,
	UINT8 *fileName,
	UINT32 IconIndex
)
{
	return sp5kGfxIconFileDraw(page, x, y,align,fileName ,IconIndex);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxRectShapeDraw
 *------------------------------------------------------------------------*/
UINT32
appOsd_GfxRectShapeDraw(
	UINT32 page,
	SINT32 posX,
	SINT32 posY,
	SINT32 posX1,
	SINT32 posY1
)
{
	return sp5kGfxRectDraw(page,posX,posY,posX1,posY1);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxCircleShapeDraw
 *------------------------------------------------------------------------*/
UINT32
appOsd_GfxCircleShapeDraw(
	UINT32 page,
	SINT32 posX,
	SINT32 posY,
	SINT32 radiusX
)
{
	return sp5kGfxCircleDraw(page,posX,posY,radiusX);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxEllipseShapeDraw
 *------------------------------------------------------------------------*/
UINT32 appOsd_GfxEllipseShapeDraw(UINT32 page,SINT32 posX,SINT32 posY,SINT32 radiusX,SINT32 radiusY)
{
	return sp5kGfxEllipseDraw(page,posX,posY,radiusX,radiusY);

}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxLineShapeDraw
 *------------------------------------------------------------------------*/
UINT32
appOsd_GfxLineShapeDraw(
	UINT32 page,
	SINT32 posX,
	SINT32 posY,
	SINT32 posX1,
	SINT32 posY1
)
{
	return 	sp5kGfxLineDraw(page,posX,posY,posX1,posY1);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxPixelShapeDraw
 *------------------------------------------------------------------------*/
UINT32
appOsd_GfxPixelShapeDraw(
	UINT32 page,
	SINT32 posX,
	SINT32 posY
)
{
	return sp5kGfxPixelDraw(page,posX,posY);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxLineShapeWidthSet
 *------------------------------------------------------------------------*/
void
appOsd_GfxLineShapeWidthSet(
	UINT32 wThickness
)
{

	if (wThickness >3){
		wThickness =3 ;
	}
	sp5kGfxAttrSet(SP5K_GFX_PEN_WIDTH, wThickness, 0, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxOsdRefreshSet
 *------------------------------------------------------------------------*/
void
appOsd_GfxOsdRefreshSet(
	UINT32 nshow
)
{
	sp5kGfxAttrSet(SP5K_GFX_REFRESH_ACTIVE,nshow, 0, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxIconIDSet
 *------------------------------------------------------------------------*/
void
appOsd_GfxIconIDSet(
	UINT32 id
)
{
	sp5kGfxAttrSet(SP5K_GFX_ICON_ID, id, 0, 0, 0);
}

/*-------------------------------------------------------------------------
 *  File Name : appOsd_GfxFontIDSet
 *------------------------------------------------------------------------*/
void appOsd_GfxFontIDSet(UINT32 id)
{
	sp5kGfxAttrSet(SP5K_GFX_FONT_ID, id, 0, 0, 0);
}

UINT32 appOsdSFNFileLoad(UINT32 id,UINT8 *pfilename)
{
	#define fileHeaderSize  0x3F
	#define SnappyFlag 		0x50414e53
	#define LZOFlag			0xFA820BD0
	
	char ext[4];
	UINT8 fileHeader[fileHeaderSize];
	UINT32 fd;
	UINT32 fileFlag = 0;
	UINT32 fileSize=0;
	UINT32 ret = SUCCESS;
	char fileName[128];

	if ( strlen( (char *)pfilename ) < 4 ) {
		return FAIL;
	}
	if ( pfilename[ strlen( (char *)pfilename ) - 4 ] != '.' ) {
		return FAIL;
	}

	strncpy( ext, (char *)pfilename + strlen( (char *)pfilename ) - 3, 3 );
	ext[3] = 0;

	if ( 0 != strncmp( ext, "SFN",3) ) {
		printf("Only support SFN file\n");
		return FAIL;
	}
	
	sprintf(fileName,"A:\\RO_RES\\%s", (char *)pfilename); // add prefix
	fd = sp5kFsFileOpen((UINT8 *)fileName,FS_OPEN_RDONLY | FS_OPEN_BINARY);
	if(!fd){
		printf("%s open fail\n",fileName);
		return FAIL;
	}

	sp5kFsFileRead(fd,fileHeader,fileHeaderSize);	
	
	//check SFN header
	if((strncmp((char *)fileHeader + 0x08,"SPFNmark",8)!=0) || (strncmp((char *)fileHeader+0x14,"Sunplus ",8)!=0)){
		printf("Not support!\n");
		return FAIL;
	}

	sp5kFsFileSeek(fd,0x36,SP5K_FS_SEEK_SET);
	sp5kFsFileRead(fd,(UINT8 *)&fileFlag,sizeof(UINT32));

	if(fileFlag == SnappyFlag){
		printf("Snappy file:%s\n", pfilename);
		fileFlag = 1;
	}else if(fileFlag == LZOFlag){
		printf("LZO file:%s\n", pfilename);
		fileFlag = 2;
	}else{
		fileFlag = 0;
	}

	if(fileFlag == 1){
		sp5kFsFileSeek(fd,0x04,SP5K_FS_SEEK_SET);
		sp5kFsFileRead(fd,(UINT8 *)&fileSize,sizeof(UINT32));
		fileSize += 8;
	}else if(fileFlag == 2){
		sp5kFsFileSeek(fd,0x3A,SP5K_FS_SEEK_SET);
		sp5kFsFileRead(fd,(UINT8 *)&fileSize,sizeof(UINT32));
	}else{
		fileSize= sp5kFsFileSizeGet(fd);
	}

	sp5kFsFileClose(fd);

	if(fileSize > OSDRsvSize[id].size){
		printf("\033[41;30m[WARNING]:%s need %u KB space,but the buf size only %d KB\033[0m\n",(char *)fileName,fileSize/1024,OSDRsvSize[id].size/1024);
	}
	ret = sp5kResourceFileLoad(id,fileName);
	/*printf("Load %s\n",(ret==SUCCESS)?"SUCCESS":"FAIL");*/
	return ret;
}
