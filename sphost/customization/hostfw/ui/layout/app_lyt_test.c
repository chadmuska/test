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
#define HOST_DBG 1

#include "app_com_api.h"
#include "sp5k_global_api.h"
#include "sp5k_rsvblk_api.h"
#include "app_com_def.h"
#include "app_osd.h"
#include "app_lyt_show.h"

#include "Demo.c"

typedef lyt_layout_t *(*fpLayoutGet_t)(void);

fpLayoutGet_t layouts[] = {
	Demo_layoutGet,
};

void appLytTest(UINT8 * param)
{
	fpLayoutGet_t layoutGet;
	UINT8 layoutCnt;

	printf("appLytTest\n");
	layoutCnt = sizeof(layouts) / sizeof(fpLayoutGet_t);
	
	if(param!=NULL && *param >= '0' && *param <= '9' && *param-'0' < layoutCnt ){		
		printf("%c/%d\n", *param, layoutCnt);
		
		layoutGet = layouts[*param-'0'];
		
		APP_OSD_REFRESH_OFF;
		app_lytCommand(APP_LYT_LAYOUT_SEL, (UINT32)layoutGet(), 0, 0, 0);
		app_lytCommand(APP_LYT_LAYER_SEL, 0, 0, 0, 0);
		app_lytCommand(APP_LYT_LAYER_SHOW, 0, 0, 0, 0);
		app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
		APP_OSD_REFRESH_ON;
	}
	else{
		layoutGet = layouts[0];	
		#if 0 /*open if enable alpha*/
		sp5kGfxInitCfgSet(SP5K_GFX_OSD_WITH_ALPHA_PLANE, 1, 0, 0, 0);
		sp5kGfxInit();
		#endif
		
		APP_OSD_REFRESH_OFF;
		appOsd_ColorDrawShapeColorSet( RGB888_TO_RGB565(COLOR_PINK), PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
		appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_0, 0, 0, 320, 240);

		app_lytCommand(APP_LYT_LAYOUT_SEL, (UINT32)layoutGet(), 0, 0, 0); /*xian modify*/
		app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
		app_lytCommand(APP_LYT_LAYER_SEL, 0, 0, 0, 0);
		APP_OSD_REFRESH_ON;
	}

}

#if 0 /*xian 20120720*/

UINT32 tmp3 = 0;
extern int gVFInited;

void
appTestStateInit(
	void
)
{
#if 0 
	sp5kGfxInitCfgSet(SP5K_GFX_USR_PAGE_ID , 1, 0, 0, 0);/*SP5K_GFX_USR_PAGE_ID指定设置user层，1代表初始化SP5K_GFX_PAGE_USR_0+1层，其他参数填0*/
	sp5kGfxInitCfgSet(SP5K_GFX_USR_FMT , SP5K_GFX_FMT_IDX8, 0, 0, 0);/*设置图层格式*/
	sp5kGfxInitCfgSet(SP5K_GFX_USR_FRAME_SIZE, 320, 240, 0, 0);/*设置frame size*/
	sp5kGfxInitCfgSet(SP5K_GFX_USR_WINDOW, 0,0 , 320, 240);/*设置显示window大小*/
	sp5kGfxInit();/*必须要初始化usr层，否则不能够在上面绘图*/	
#endif

	sp5kGfxInitCfgSet(SP5K_GFX_OSD_WITH_ALPHA_PLANE, 1, 0, 0, 0);
	sp5kGfxInit();
	appJpgDraw_ImgLayer("A:\\RO_RES\\UI\\JPG\\SETUPBG.JPG", 0,0,320,240);
	appOsd_ColorDrawShapeColorSet( RGB888_TO_RGB565(COLOR_PINK), PAL_BLEND_100, PAL_BLACK, PAL_BLEND_100);
	appOsd_GfxRectShapeDraw(SP5K_GFX_PAGE_PIP_0, 0, 0, 320, 240);
	tmp3 = 0;
	app_lytCommand(APP_LYT_LAYOUT_SEL, 0/*PVView*/, 0, 0, 0);
	app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
	app_lytCommand(APP_LYT_LAYER_SEL, 0, 0, 0, 0);
}

#define BARMOVE 2

void
appLayoutTestOnKey(
	UINT32 msg,
	UINT32 param
)
{
	static UINT32 tmp1 = 0, tmp2 = 0;
		switch ( msg ) {
			case APP_KEY_PRESS_MENU: /*Start*/		
				tmp1 ^= 0x01;
				if(tmp1 & 0x01){
					tmp3 = 0;
					app_lytCommand(APP_LYT_LAYOUT_SEL, MENUTEST1, 0, 0, 0);
					app_lytCommand(APP_LYT_LAYER_SEL, 0, 0, 0, 0);
					app_lytCommand(APP_LYT_LAYER_SHOW, 0, 0, 0, 0);
					app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
				}
				else{
					app_lytCommand(APP_LYT_LAYOUT_SEL, PVView, 0, 0, 0);
					app_lytCommand(APP_LYT_LAYER_SEL, 0, 0, 0, 0);
					app_lytCommand(APP_LYT_LAYER_SHOW, 0, 0, 0, 0);
					app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
				}
				break;
			case APP_KEY_PRESS_SET:	
				if(tmp3 == 0){
					if ( 0 == (tmp1 & 0x01) ){
						tmp1 ^= 0x02;
						APP_OSD_REFRESH_OFF;
						if ( tmp1 & 0x02){
						app_lytCommand(APP_LYT_ITEM_ISSHOW, 7, 0, 0, 0);
							app_lytCommand(APP_LYT_ITEM_ISSHOW, 8, 0, 0, 0);
							app_lytCommand(APP_LYT_ITEM_ISSHOW, 9, 0, 0, 0);
							app_lytCommand(APP_LYT_ITEM_ISSHOW, 6, 0, 0, 0);
							app_lytCommand(APP_LYT_LAYER_SHOW, 0, 0, 0, 0);
						}	
						else{
							app_lytCommand(APP_LYT_ITEM_ISSHOW, 9, 1, 0, 0);
							app_lytCommand(APP_LYT_ITEM_ISSHOW, 6, 1, 0, 0);
							app_lytCommand(APP_LYT_ITEM_ISSHOW, 7, 1, 0, 0);
							app_lytCommand(APP_LYT_ITEM_ISSHOW, 8, 1, 0, 0);
						}
						app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
						APP_OSD_REFRESH_ON;
					}
				
				else {
					if ( 0 == tmp2 ){	
						tmp1 ^= 0x04;
						APP_OSD_REFRESH_OFF;
						if ( tmp1 & 0x04){
							app_lytCommand(APP_LYT_LAYOUT_SEL, MENUTEST2, 0, 0, 0);
							app_lytCommand(APP_LYT_LAYER_SEL, 0, 0, 0, 0);
							app_lytCommand(APP_LYT_LAYER_SHOW, 0, 0, 0, 0);
							app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
							app_lytCommand(APP_LYT_LAYOUT_SEL, RESOMUSUB4, 0, 0, 0);
							app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);								
							}
						else {
							app_lytCommand(APP_LYT_LAYOUT_SEL, MENUTEST1, 0, 0, 0);
							app_lytCommand(APP_LYT_LAYER_SEL, 0, 0, 0, 0);
							app_lytCommand(APP_LYT_LAYER_SHOW, 0, 0, 0, 0);
							app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
						}
						APP_OSD_REFRESH_ON;
					}
				}
					}
				break;
			case APP_KEY_PRESS_MODE:
				appStateChange(APP_STATE_STILL_PREVIEW, STATE_PARAM_NORMAL_INIT);
				break;
			case APP_KEY_PRESS_UP:
				if(tmp3 == 0){
					if((0x01 & tmp1) && ( (tmp1 & 0x04) == 0)){
						APP_OSD_REFRESH_OFF;
						if(0 == tmp2){
							app_lytCommand(APP_LYT_ITEM_MOVE, BARMOVE, 0, 31 * 4, 0);
							tmp2 = 4;
						}
						else{
							app_lytCommand(APP_LYT_ITEM_MOVE, BARMOVE, 0, -31, 0);
							tmp2 --;
						}
						app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
						APP_OSD_REFRESH_ON;
					}
				}
				break;
			case APP_KEY_PRESS_DOWN:
				if (tmp3 == 0){
					if((0x01 & tmp1) && ( (tmp1 & 0x04) == 0)){
						APP_OSD_REFRESH_OFF;
						if(4 == tmp2){
						app_lytCommand(APP_LYT_ITEM_MOVE, BARMOVE, 0, -(31 * 4), 0);
							tmp2 = 0;
						}
						else{
							app_lytCommand(APP_LYT_ITEM_MOVE, BARMOVE, 0, 31, 0);
							tmp2 ++;
						}
						app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
						APP_OSD_REFRESH_ON;
					}
				}
				break;
			case APP_KEY_PRESS_LEFT:
				APP_OSD_REFRESH_OFF;
				tmp3 ++;
				if ( SHOWEND == tmp3 ){
					tmp3 = 0;
				}
				app_lytCommand(APP_LYT_LAYOUT_SEL, tmp3, 0, 0, 0);
				app_lytCommand(APP_LYT_LAYER_SEL, 0, 0, 0, 0);
				app_lytCommand(APP_LYT_LAYER_SHOW, 0, 0, 0, 0);
				app_lytCommand(APP_LYT_LAYOUT_SHOW, 1, 0, 0, 0);
				APP_OSD_REFRESH_ON;
				break;
			case APP_KEY_PRESS_RIGHT:
				break;
			default:
				break;
		}

}

void
appLayoutTestOnMsg(
	UINT32 msg,
	UINT32 param
)
{	
	switch ( msg ){
		case SP5K_MSG_TIMER:
			break;
		default:
			break;
	}
}

void
appLayoutPlayState(
	void
)
{
	;
}

void
appLayoutTestState(
	UINT32 msg,
	UINT32 param
)
{
	switch (msg) {
		case APP_STATE_MSG_INIT:
			RPrintf("appGamePvState");
			appTestStateInit();
			break;
        case APP_STATE_MSG_CLOSE:
            if( gVFInited == 1 ) {
              appOsd_FontTextStringSet("UI\\SST\\STR5KTK.FGS");
            }            
			appStateCloseDone();
			break;
		default:
			if (IS_APP_KEY_MSG(msg)) 
			{
				appLayoutTestOnKey(msg,param);
			}
			else
			{
				appLayoutTestOnMsg(msg,param);
			}
			break;
	}
}

#endif

