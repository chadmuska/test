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
#include "string.h"

#include "sp5k_global_api.h"
#include "sp5k_rsvblk_api.h"
#include "sp5k_fs_api.h"
#include "sp5k_os_api.h"

#include "app_com_api.h"
#include "app_osd.h"
#include "app_lyt_structure.h"
#include "app_lyt_show.h"
#include "debug_mod/dbg_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define LYT_FILE_HEADER_SIZE    8
#define LYT_LAYER_FMT_IDX8      0
#define LYT_LAYER_FMT_RGB565    1
#define PALETTE_MAX	            32
#define DEFAULT_SCALEX          1000
#define DEFAULT_SCALEY          1000

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef lyt_layout_t *(*fpLayoutGet)(void);

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
lyt_res_sfn_t * CurTextFont = NULL;
lyt_res_sfn_t * CurIconFont = NULL;
lyt_layout_t * pCurLayout = NULL;
lyt_layer_t * pCurLayer = NULL;
lyt_item_t * pCurItem = NULL;
fpLayoutGet layout[] = {0};

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/

/**************************************************************************
 *				   E X T E R N A L	  R E F E R E N C E S				  *
 **************************************************************************/
extern SINT32 gbOsdFactorXNumerator ;		
extern SINT32 gbOsdFactorXDenominator ;
extern SINT32 gbOsdFactorYNumerator ;		
extern SINT32 gbOsdFactorYDenominator ;

/**************************************************************************
 *				 F U N C T I O N	D E C L A R A T I O N S 			  *
 **************************************************************************/
void app_lytItemShow(lyt_layer_t * player, lyt_item_t *pItem);

void app_lytZoomScaleSet(UINT32 fmt, UINT32 scaleX, UINT32 scaleY)
{
	static UINT32 tmpX = DEFAULT_SCALEX;
	static UINT32 tmpY = DEFAULT_SCALEY;
	if((scaleX != tmpX) || (scaleY != tmpY)){
        /*		
        switch(fmt){
			case LYT_LAYER_FMT_IDX8:
				sp5kGfxAttrSet(SP5K_GFX_IDX8_PIC_SCALE_FACTOR, scaleX, scaleY, 0, 0);
				break;
			case LYT_LAYER_FMT_RGB565:
				sp5kGfxAttrSet(SP5K_GFX_RGB565_PIC_SCALE_FACTOR, scaleX, scaleY, 0, 0);
				break;
			default:
				break;
		}*/
		sp5kGfxAttrSet(SP5K_GFX_PIC_SCALE_FACTOR, scaleX, scaleY, 0, 0);
		tmpX = scaleX;
		tmpY = scaleY;	
	}
}

void 
app_lytTextFontSet(
	UINT8 *pFont, UINT8 *pColorTbl
)
{
	appOsd_FontTextFontSet(pFont);
/*	sp5kResourceFileLoad(SP5K_RES_OSD_FONT,(char*)pFont);*/
	if(pColorTbl[0] != 0xff){
		sp5kGfxTextColorSet(pColorTbl, 0, 32);
	}
}

void app_lytIconFontSet(	
	UINT8 *pFont, UINT8 *pColorTbl
)
{
	appOsd_FontIconFontSet(pFont);
	if(pColorTbl[0] != 0xff){
		sp5kGfxIconColorSet(pColorTbl, 0, 32);
	}
}

void
app_lytCoordScaleSet(
	SINT32 *x,
	SINT32 *y
)
{
	*x = *x * gbOsdFactorXNumerator / gbOsdFactorXDenominator;
	*y = *y * gbOsdFactorYNumerator / gbOsdFactorYDenominator;
}

UINT32 app_lytIconDraw(lyt_layer_t *player, lyt_item_t * iconInfo){
	app_lytCoordScaleSet(&(iconInfo->roi.xStart), &(iconInfo->roi.yStart)); /*xian ??*/
	app_lytZoomScaleSet(player->fmt, iconInfo->scaleX, iconInfo->scaleY);
	if(CurIconFont != iconInfo->pItem){
		CurIconFont = (lyt_res_sfn_t *)iconInfo->pItem;
		app_lytIconFontSet(CurIconFont->sfnFile, CurIconFont->colorTbl);
	}

	return sp5kGfxIconDraw(player->pageId, iconInfo->roi.xStart, iconInfo->roi.yStart, SP5K_GFX_ALIGN_TOP_LEFT, iconInfo->id);
}

UINT32 app_lytStringDraw(lyt_layer_t *player, lyt_item_t * stringInfo){
    lyt_res_str_t *pstrRes;
	app_lytCoordScaleSet(&(stringInfo->roi.xStart), &(stringInfo->roi.yStart));
	app_lytZoomScaleSet(player->fmt, stringInfo->scaleX, stringInfo->scaleY);
    pstrRes = (lyt_res_str_t *)stringInfo->pItem;
	if( CurTextFont != &pstrRes->sfnRes){
        #if 0
		CurTextFont = pstrRes->psfn;
        #else
        CurTextFont = &pstrRes->sfnRes;
        #endif
		app_lytTextFontSet(CurTextFont->sfnFile, CurTextFont->colorTbl);
	}
    appOsd_FontTextStringSet(pstrRes->pSstFilename);	
	return sp5kGfxStringIdDraw(player->pageId, stringInfo->roi.xStart, stringInfo->roi.yStart, SP5K_GFX_ALIGN_TOP_LEFT, stringInfo->id);
}

void app_lytMenuItemDraw(lyt_layer_t *player, lyt_item_t * menuItem)
{
    lyt_res_menu_t *pmenuRes;
    lyt_item_t subItem;

    pmenuRes = (lyt_res_menu_t *)menuItem->pItem;
    if(menuItem->id < pmenuRes->menuItemTotal) {
	UINT8 i;
		for(i = 0; i < pmenuRes->atomsPerItem; i ++) {
			lyt_item_t  *pItem;
			pItem = pmenuRes->ppMenuList[pmenuRes->atomsPerItem * menuItem->id + i];
            if(pItem->showMode == 1) {
                memcpy(&subItem, pItem, sizeof(lyt_item_t));
				subItem.roi.xStart += menuItem->roi.xStart;
                subItem.roi.yStart += menuItem->roi.yStart;
                app_lytItemShow(player, &subItem); 
            }
        }
    }
}

void app_lytAlphaSet(lyt_layer_t *player)
{
	if(player->isWithAlpha){
	/*	sp5kGfxAttrSet(SP5K_GFX_LAYER_ALPHA, player->id, player->alpha, 0, 0);*/
		switch(player->fmt){
		case LYT_LAYER_FMT_IDX8:
			sp5kGfxAttrSet(SP5K_GFX_IDX8_BLEND_FACTOR, player->alpha, 0, 0, 0);
			break;
		case LYT_LAYER_FMT_RGB565:
			sp5kGfxAttrSet(SP5K_GFX_RGB565_BLEND_FACTOR, player->alpha, 0, 0, 0);
			break;
		default:
			break;
		}
	}
}

void app_lytLalyerAlphaSet()
{
/*	if()*/;
}

/*add for now app osd others
UINT32 Idx8Fmt, Rgb565Fmt;*/
void app_lytEnter(void)
{
/*	sp5kGfxIconColorGet()*/
/*
	sp5kGfxAttr_t *psp5kAttr;
	sp5kGfxAttrGet();*/
	CurTextFont = NULL;
	CurIconFont = NULL; 
}

void app_lytRuit(void)
{
	app_lytZoomScaleSet(LYT_LAYER_FMT_IDX8, DEFAULT_SCALEX, DEFAULT_SCALEX);
	
	sp5kGfxAttrSet(SP5K_GFX_BLEND_FACTOR, 128, 0, 0, 0);/*come back with no transparence*/
}

void app_lytItemShow(lyt_layer_t * player, lyt_item_t *pItem)
{
	if ( NULL == pItem ){
/*		DBG_FUNC_RETURN(FAIL, "Layout error: item");*/
		return;
	}
	if(pItem->showMode == 1){
		if(pItem->itemType == LYT_ITEM_TYPE_ICON){
			app_lytIconDraw(player, pItem);
		}
        else if(pItem->itemType == LYT_ITEM_TYPE_MENU) { /* menu type*/
            app_lytMenuItemDraw(player, pItem);
        }
		else if(pItem->itemType == LYT_ITEM_TYPE_STR){
			app_lytStringDraw(player, pItem);
		}
	}
}

void app_lytItemIsShow(lyt_item_t * pItem, UINT8 isShow){
	if ( NULL == pItem){
		return;
	}
	pItem->showMode = isShow;
}

void app_lytItemClean(lyt_layer_t * player, lyt_item_t *pItem)
{
	if ( NULL == pItem ){
		return;
	}
	appOsd_ColorDrawShapeColorSet( 0, PAL_BLEND_100, 0, PAL_BLEND_100);
	appOsd_GfxRectShapeDraw(player->pageId, pItem->roi.xStart, pItem->roi.yStart, 
		pItem->roi.xStart + pItem->roi.width, pItem->roi.yStart + pItem->roi.height);
}

void app_lytLayerClean(lyt_layer_t * player)
{
	if ( NULL == player){
		return;
	}
	appOsd_ColorDrawShapeColorSet( 0, PAL_BLEND_100, 0, PAL_BLEND_100);
	appOsd_GfxRectShapeDraw(player->pageId, player->roi.xStart, player->roi.yStart, 
		player->roi.xStart + player->roi.width, player->roi.yStart + player->roi.height);
}

void app_lytItemMove(lyt_layer_t * player, lyt_item_t *pItem, SINT32 xMove, SINT32 yMove)
{
	if ( NULL == pItem){
		return;
	}
	app_lytItemClean(player, pItem);
	pItem->roi.xStart = pItem->roi.xStart + xMove;
	pItem->roi.yStart = pItem->roi.yStart + yMove;
}
void app_lytItemChange(lyt_layer_t * player, lyt_item_t *pItem, UINT32 changeId)
{
	if ( NULL == pItem ){
		return;
	}
	pItem->id = changeId;
}

void app_lytShow(lyt_layout_t *playout)
{
	UINT8 i, j;
	if(playout == NULL){
	/*	DBG_FUNC_RETURN(FAIL, "Read Layout error");*/
		return;
	}
	app_lytEnter();
	for(i = 0; i < playout->layerTotal; i ++){
		if(playout->ppLayer[i] == NULL){
	/*		DBG_FUNC_RETURN(FAIL, "Layout error: page");*/
			return;
		}
		app_lytAlphaSet(playout->ppLayer[i]); /*set the alpha of the layer*/
		for(j = 0; j < playout->ppLayer[i]->itemTotal; j ++){
			app_lytItemShow(playout->ppLayer[i], playout->ppLayer[i]->ppItem[j]);
		}
	}
	app_lytRuit();
}

UINT32 app_lytCommand(
	app_lytCfg_e paramId,
	UINT32 param1,
	UINT32 param2,
	UINT32 param3,
	UINT32 param4
)
{
	UINT32 ret = SUCCESS;
	switch ( paramId ) {
	case APP_LYT_LAYOUT_SEL:
		pCurLayout = (lyt_layout_t *)param1; /*app_lytLayoutGet(param1);*/
		break;
	case APP_LYT_LAYER_SEL:
		if ( NULL == pCurLayout ){
	/*		DBG_FUNC_RETURN(FAIL, "Have no ");*/
			return 0;
		}
		pCurLayer = pCurLayout->ppLayer[param1];
		break;
/*	case APP_LYT_ITEM_SEC:
		if ( NULL == pCurLayer){
			DBG_FUNC_RETURN(FAIL, "Have no ");
			return 0;
		}
		pCurItem = pCurLayer->item[param1];
		break;*/
	case APP_LYT_LAYOUT_SHOW:
		if(param1){
			app_lytShow(pCurLayout);
		}
		else{
			/**** If support multilayer, it must be correction. ****/
			app_lytLayerClean(pCurLayer);/*temp*/
		}
		break;
	case APP_LYT_LAYER_SHOW:
		if(param1){
			/**** If support multilayer, it must be correction. ****/
			app_lytShow(pCurLayout);/*temp*/
		}
		else{
			app_lytLayerClean(pCurLayer);
		}
		break;
	case APP_LYT_ITEM_SHOW:
		if(param2){
			app_lytItemShow(pCurLayer, pCurLayer->ppItem[param1]);
		}
		else{
			app_lytItemClean(pCurLayer, pCurLayer->ppItem[param1]);
		}
		break;
	case APP_LYT_ITEM_MOVE:
		app_lytItemMove(pCurLayer, pCurLayer->ppItem[param1], (SINT32)param2, (SINT32)param3);
		break;
	case APP_LYT_ITEM_CHANGE:
		app_lytItemChange(pCurLayer, pCurLayer->ppItem[param1], param2);
		break;
	case APP_LYT_ITEM_ISSHOW:
		app_lytItemIsShow(pCurLayer->ppItem[param1], param2);
	default:
		break;
	}
	return ret;
}

UINT32
app_lytFileParse
(
	void
)
{
	UINT8 i;
	UINT32 len,ret = SUCCESS;
	UINT32 fileHndl;	 
	UINT32 headerBuf[LYT_FILE_HEADER_SIZE];	
	UINT32 *pLayerDataStart = NULL;
	UINT32 *pItemDataStart  = NULL;
	UINT32 *pResInfoStart   = NULL;
	UINT32 *pClrIdxStart    = NULL;
	UINT32 *pPaletteStart   = NULL;
	UINT32 *pBuf            = NULL;
	UINT8  *pSfnFilePath    = NULL;		 
	const UINT8 filePath[] = "D:\\PARSE.LYT";
	const UINT8 ILYTCheck[4] = {'I','L','Y','T'};
	const UINT8 FileCheck[8] = {'S','U','N','M','E','D','I','A'};
	lyt_layer_t layerInfo;
	lyt_item_t	itemInfo;
	lyt_res_sfn_t *resFontInfo = NULL;
	lyt_res_str_t resStrInfo; 

	len = strlen(filePath);
	if ( 64 <= len ) {
		DBG_LINE_PRINT("file path name too long\n");
		ret = FAIL; 
		return ret;
	}

	fileHndl = sp5kFsFileOpen(filePath, SP5K_FS_OPEN_RDONLY);
	if ( (UINT32)NULL == fileHndl ) {
		DBG_LINE_PRINT("open File err!\n");
		ret = FAIL; 
		return ret;
	}
	 
	for(i = 0;i < LYT_FILE_HEADER_SIZE;i++){
		headerBuf[i] = 0;
	}
	sp5kFsFileRead(fileHndl, headerBuf, 4*LYT_FILE_HEADER_SIZE);
		 
	if( (0 != memcmp(&ILYTCheck,(UINT8*)(headerBuf + 2),4))||
		(0 != memcmp(&FileCheck, (UINT8*)(headerBuf+4),8))){
		ret = FAIL;
		DBG_LINE_PRINT("No SunMedia font file!\n");
		sp5kFsFileClose(fileHndl);
	}
	else{
		UINT32 dataSize;
		UINT8 layerTot,ResFileTot;
		UINT8 layerCnt,ResFileCnt;
//		UINT16  paletteTbl[32];
		dataSize = *(headerBuf + 3); /*ILYT Chunk Size*/
		pBuf = (UINT32 *)sp5kMalloc(dataSize);
		
		if(NULL == pBuf){
		DBG_LINE_PRINT("Memory allocation failed!\n");
		return FAIL;
		}
		
		memset(pBuf,0,dataSize);
		sp5kFsFileRead(fileHndl, pBuf, dataSize);
		/* Read sfn file path and color index table*/
		pResInfoStart = pBuf;
		ResFileTot = (UINT8)*(pResInfoStart + 2); 
		resFontInfo = (lyt_res_sfn_t*)sp5kMalloc(ResFileTot*sizeof(lyt_res_sfn_t));
			 					 
		pSfnFilePath = (UINT8*)(pResInfoStart + 4);
		pClrIdxStart = (pResInfoStart + 20);
		
		for(ResFileCnt = 0;ResFileCnt < ResFileTot;ResFileCnt++){
			UINT8 ClrIdxCnt;
			resFontInfo[ResFileCnt].sfnFile = (pSfnFilePath + ResFileCnt*1092); 
			for(ClrIdxCnt = 0;ClrIdxCnt < COLOR_INDEX_MAX;ClrIdxCnt++){
				resFontInfo[ResFileCnt].colorTbl[ClrIdxCnt] = (UINT8)pClrIdxStart[ClrIdxCnt];
			}
			pClrIdxStart = (pClrIdxStart + 273);
		}
			 /**********read palette table**********/
/* 		for(paletteCnt = 0; paletteCnt < PALETTE_MAX; paletteCnt++){		
			paletteTbl[paletteCnt] = 0;
		} */
		pPaletteStart = (pBuf + 3 + ResFileTot*273 + 2);
/* 		for(paletteCnt = 0; paletteCnt < PALETTE_MAX; paletteCnt++){
			paletteTbl[paletteCnt] = (UINT16)*(pPaletteStart + paletteCnt);
		} */
					 
		/********************parse layer********************/
		pLayerDataStart = pBuf + 3 + ResFileTot*273 + 258;
		layerTot = *(pLayerDataStart + 2);
		for(layerCnt = 0;layerCnt < layerTot; layerCnt++){
			UINT8 itemTot;
			UINT8 itemCnt;
			UINT32 eachSizeOfItem;
			layerInfo.pageId = *(pLayerDataStart + 5);
			layerInfo.fmt = *(pLayerDataStart + 6);
			layerInfo.alpha = *(pLayerDataStart + 7);
			/*Removed : Variable 'layerInfo.isWithAlpha' is reassigned a value*/
			/*layerInfo.isWithAlpha = (UINT8)*(pLayerDataStart + 8);*/
			layerInfo.isWithAlpha = 1;
			itemTot = *(pLayerDataStart + 9);							 
			layerInfo.itemTotal = itemTot;
			eachSizeOfItem = *(pLayerDataStart + 10);
			pItemDataStart = pLayerDataStart + 11;
			app_lytAlphaSet(&layerInfo);
			/********************parse item*******************/
			for(itemCnt = 0;itemCnt < itemTot;itemCnt++){
				UINT8 itemType;
				UINT8 ResIndex;
				itemType = *(pItemDataStart);
				itemInfo.id = *(pItemDataStart + 1); 
				itemInfo.roi.xStart = *(pItemDataStart + 2); 
				itemInfo.roi.yStart = *(pItemDataStart + 3);
				itemInfo.roi.width = *(pItemDataStart + 4);
				itemInfo.roi.height = *(pItemDataStart + 5);
				itemInfo.scaleX = *(pItemDataStart + 6); 
				itemInfo.scaleY = *(pItemDataStart + 7);
				itemInfo.showMode = 1;					 
				ResIndex = *(pItemDataStart + 8);
				if(0 == itemType) {
					itemInfo.itemType = LYT_ITEM_TYPE_ICON;
					itemInfo.pItem = (void *)&resFontInfo[ResIndex];
				}
				else{
					itemInfo.itemType = LYT_ITEM_TYPE_STR;
					itemInfo.pItem = (void *)&resStrInfo;
					memcpy(&resStrInfo.sfnRes, &resFontInfo[ResIndex], sizeof(lyt_res_sfn_t));
					resStrInfo.pSstFilename = (UINT8 *)(pItemDataStart + 9);
				}
				pItemDataStart += (eachSizeOfItem/4); /*next item*/
									 
				app_lytItemShow(&layerInfo, &itemInfo);
			}				 
			/************next	layer start************/
			pLayerDataStart = pItemDataStart + 25;
		}
						 
		sp5kGfxAttrSet(SP5K_GFX_BLEND_FACTOR, 128, 0, 0, 0);	 
			 
		 if(pBuf != NULL){
			 sp5kFree(pBuf);
			 pBuf = NULL;
		 }
	 
		 if(resFontInfo!= NULL){
			 sp5kFree(resFontInfo);
			 resFontInfo= NULL;
		 }
		 if ( (UINT32)NULL != fileHndl ) {
			 DBG_LINE_PRINT("close file\n");
			 sp5kFsFileClose(fileHndl);
		 }
		 
	}
	return ret;
}

UINT32
app_lytUSBView(
	void
)
{
	//appOsd_ColorDrawShapeColorSet(13, 4, 14, 4);
	//appOsd_GfxRectShapeDraw(8, 0,0,320,240);

	sp5kGfxPageClear(8, 0);
	
	return app_lytFileParse();
}

void 
app_LayoutSceneShow(
	UINT8 showScene
)
{
	app_lytCommand(APP_LYT_LAYOUT_SEL, (UINT32)layout[showScene](), 0, 0, 0);
	app_lytCommand(APP_LYT_LAYER_SEL, 0, 0, 0, 0);
	app_lytCommand(APP_LYT_LAYER_SHOW, 0, 0, 0, 0);
	app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);

	return;
}

