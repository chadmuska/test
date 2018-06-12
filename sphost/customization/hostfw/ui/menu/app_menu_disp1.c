/**************************************************************************
 *         Copyright(c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch Tech-  *
 *  nology Co., Ltd. All rights are reserved by iCatch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of iCatch Technology Co., Ltd.                 *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/
#define HOST_DBG 0
#include "app_icon_def.h"
#include "app_menu_event.h"
#include "app_menu_core.h"
#include "app_menu_tab.h"
#include "app_menu_disp.h"
#include "app_ui_para.h"
#include "app_menu.h"
#include "app_com_api.h"
#include "app_osd_api.h"
#include "app_com_def.h"
#include "gpio_custom.h"
#include "app_rtc.h"
#include "app_menu_disp1.h"/**/
#include "app_playback_osd.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define WIDTH_ICON  40
#define HEIGHT_ICON    32

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
#if FUNC_HOST_TOUCH_PANEL
#endif
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
	extern UINT8 fileTypeSelect;
	extern UINT8 USBModeSet;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
#if FUNC_HOST_TOUCH_PANEL

void iconGuiReg(UINT32 iconID,UINT32 msg,UINT32 para	)
{
	UINT16 x, y;
	
	appOsdIconPosGet(iconID,&x,&y);
	appGuiObjNew_EvtMsgReg(x,y,\
			WIDTH_ICON*2,HEIGHT_ICON,\
			msg,para);
		
}
void iconGuiRepeateReg(UINT32 iconID,\
	UINT32 penUpMsg,UINT32 penUpPara,\
	UINT32 penDownMsg,UINT32 penDownPara,\
	UINT32 fcsKeepMsg,UINT32 fcsKeepPara\
	)
{
	UINT16 x, y;
	
	appOsdIconPosGet(iconID,&x,&y);
	appGuiObjNew_EvtMsgRegExt(x,y,\
			WIDTH_ICON,HEIGHT_ICON,\
			penUpMsg,penUpPara,\
			penDownMsg,penDownPara,0,0,0,0,\
		fcsKeepMsg, fcsKeepPara,0,0);
		
}
void appTPGuiKeyInit()
{
	//iconGuiReg(ID_ICON_BUTTON_MODE,APP_KEY_PRESS_TPMODE,0);
	iconGuiReg(ID_ICON_BUTTON_MENU,APP_KEY_PRESS_TPMENU,0);
}

/**************************************************************************
 *                                                                        *
 *  Function Name: menuProcButIconDisp                                         *
 *                                                                        *
 *  Purposes:                                                             *
 *                                                                        *
 *  Descriptions:                                                         *
 *                                                                        *
 *  Arguments: None                                                       *
 *                                                                        *
 *  Returns:                                                              *
 *                                                                        *
 *  See also:                                                             *
 *                                                                        *
 **************************************************************************/
void menuProcButIconDisp(UINT16 attr,UINT16 x0,UINT16 y0,UINT16 w,UINT16 h,UINT16 iconIdx,UINT32 msg,UINT32 para)
{	

	if (iconIdx !=ID_ICON_NONE && attr!=0xff)
	{
		appOsdColorSet(attr);		
		//menuIconDispExt((iconw>=w?x0-(iconw-w)/2:(x0+(w-iconw)/2)), y0, iconIdx,0);
		menuIconDispExt(x0, y0, iconIdx,0);
	}
	appGuiObjNew_EvtMsgReg(x0, y0, w, h,msg,para);
}

/**************************************************************************
 * @fn        UINT32 appMenuViewSelItemFuncGui(void *menuItem)
 * @brief     appMenuViewSelItemFuncGui
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author   jj
 * @since     2013-4-16
 * @todo      N/A
 * @bug       N/A                                                                 
 **************************************************************************/

void appMenuViewSelItemFuncGui(void *menuItem)
{
	UINT32 currItem = 0;
	UINT8 layer;
	layer =  menuProcLayerGet();	
	currItem=(*(UINT32*)menuItem);
	if (menuProcTPSet((UINT8)currItem, !layer) == SUCCESS)
	{	
		if(ENABLE == appTpKeyEnCheck(APP_KEY_PRESS_TPSET))
			sp5kHostMsgSend(APP_KEY_PRESS_TPSET,0);
	}
}
/**************************************************************************
 * @fn        UINT32 appMenuViewSelItemGetFocusFuncGui(void *menuItem)
 * @brief     appMenuViewSelItemGetFocusFuncGui
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author   jj
 * @since     2013-4-16
 * @todo      N/A
 * @bug       N/A                                                                 
 **************************************************************************/
void appMenuViewSelItemGetFocusFuncGui(void *menuItem)
{
	UINT8 currLine;
//	UINT8 scrollItem;
	UINT8 layer,line,col,i;
	UINT32 strIdx;
//	UINT16 icon;
	UINT8	nodeItem = 0;
//	UINT8	subItem=0;
	UINT8	selItem;
	UINT8	item = 0;	
	UINT8 strDrawX,strDrawY,iconDispX;
	UINT32 promptParm;
	currLine = (UINT8)(*(UINT32*)menuItem);
	layer =  menuProcLayerGet();
	if(layer == 0)layer=1;	
	/*printf("get %d\n",currLine);*/
	menuLayerSizeGet(layer, &line, &col);
	APP_OSD_REFRESH_OFF;
	menuBodyDisp(layer);
	menuHeaderDisp(0);
	nodeItem = menuProcNodeItemGet(layer, item);
	 /*modify bug ICOM-759*/
	if (nodeItem > line)
	{
		promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_ON;
		
	}else
	{
	
		promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_OFF;
		
	}
	nodeItem -= menuProcScrollGet(layer);
	if (nodeItem > line)
		nodeItem = line;
	if((layer == 1)||(menuProcCurrRootGet()== MENU_MARK_PLAYBACK))
	{
		strDrawX =0;
		iconDispX =col-2;
	}
	else if(layer == 3)
	{
		strDrawX =2;
		iconDispX =col-2;
		strDrawY = 0;
	}
	else
	{
		strDrawX =4;
		iconDispX =4;
	}		
	item = menuProcScrollGet(layer);
	for(i=0; i<nodeItem; i++,item++)
	{
		UINT8 subItem;
		UINT16 icon;
		menuProcStrGet(layer, item, &strIdx);
		icon = menuProcIconIDGet(layer, item);
		subItem = menuProcSubItemGet(layer, item);
		
		if (subItem > 0)
		{
			selItem = menuProcParaGet (layer, item);
			icon = menuProcChildIconIDGet(layer, item, selItem);
		}
		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(strDrawX, i + strDrawY, strIdx, 0, layer);
			menuIconDisp(iconDispX, i, icon, 0, layer);
		}
		else if (i == currLine)
		{
			menuBarDisp(i, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDraw(strDrawX, i + strDrawY, strIdx, 0, layer);
			appOsdColorSet(MENU_ICON_GET_FOCUS);
			menuIconDisp(iconDispX, i, icon, 1, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(strDrawX, i + strDrawY, strIdx, 0, layer);
			menuIconDisp(iconDispX, i, icon, 0, layer);
		}	
	}
	  /*modify bug ICOM-759*/
	menuPromptDisp(promptParm);
	//menuPromptDisp(0x07);
	APP_OSD_REFRESH_ON;
}

/**************************************************************************
 * @fn        UINT32 appMenuViewSelItemLostFocusFuncGui(void *menuItem)
 * @brief     appMenuViewSelItemLostFocusFuncGui
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author   jj
 * @since     2013-4-16
 * @todo      N/A
 * @bug       N/A                                                                 
 **************************************************************************/

void appMenuViewSelItemLostFocusFuncGui(void *menuItem)
{	
//	UINT8 scrollItem;
	UINT8 layer,line,col,i;
	UINT32 strIdx;
//	UINT16 icon;
	UINT8	nodeItem = 0;
//	UINT8	subItem=0;
	UINT8	selItem;
	UINT8	item = 0;
	UINT8 strDrawX,strDrawY,iconDispX;
	UINT32 promptParm;
	layer =  menuProcLayerGet();
	if(layer == 0)layer=1;
	menuLayerSizeGet(layer, &line, &col);
	APP_OSD_REFRESH_OFF;
	menuBodyDisp(layer);
	menuHeaderDisp(0);
	nodeItem = menuProcNodeItemGet(layer, item);
	 /*modify bug ICOM-759*/
	if (nodeItem > line)
	{
		promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_ON;
		
	}else
	{
	
		promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_ON|MENU_PROMPT_PAGEUPDN_OFF;
		
	}
	nodeItem -= menuProcScrollGet(layer);
	if (nodeItem > line)
		nodeItem = line;
	if((layer == 1)||(menuProcCurrRootGet()== MENU_MARK_PLAYBACK))
	{
		strDrawX =0;
		iconDispX =col-2;
		strDrawY = 0;
	}
	else if(layer == 3)
	{
		strDrawX =2;
		strDrawY =(line - nodeItem);
	}
	else
	{
		strDrawX =4;
		iconDispX =4;
		strDrawY = 0;
	}		
	item = menuProcScrollGet(layer);
	for(i=0; i<nodeItem; i++,item++)
	{
		UINT8 subItem;
		UINT16 icon;
		menuProcStrGet(layer, item, &strIdx);
		icon = menuProcIconIDGet(layer, item);
		subItem = menuProcSubItemGet(layer, item);
		
		if (subItem > 0)
		{
			selItem = menuProcParaGet (layer, item);
			icon = menuProcChildIconIDGet(layer, item, selItem);
		}
		if (menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(strDrawX, i + strDrawY, strIdx, 0, layer);
			menuIconDisp(iconDispX, i, icon, 0, layer);
		}
		else
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(strDrawX, i + strDrawY, strIdx, 0, layer);
			menuIconDisp(iconDispX, i, icon, 0, layer);
		}
	}
	
	/*modify bug ICOM-759*/
	menuPromptDisp(promptParm);
	//menuPromptDisp(0x07);
	APP_OSD_REFRESH_ON;
}
/**************************************************************************
 * @fn        UINT32 appMenuViewSelItemFuncGui(void *menuItem)
 * @brief     appMenuViewSelItemFuncGui
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author   jj
 * @since     2013-4-16
 * @todo      N/A
 * @bug       N/A                                                                 
 **************************************************************************/

void appMenuDisp2LayerSelItemFuncGui(void *menuItem)
{
	UINT32 currItem = 0;
	UINT8 layer;
	layer =  menuProcLayerGet();	
	currItem=(*(UINT32*)menuItem);
	if (menuProcTPSet((UINT8)currItem, !layer) == SUCCESS)
	{	
		menuProcDisp2Layer();
	}
}

/**************************************************************************
 * @fn        UINT32 appMenuDisp2LayerSelItemGetFocusFuncGui(void *menuItem)
 * @brief     appMenuDisp2LayerSelItemGetFocusFuncGui
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author   jj
 * @since     2013-4-22
 * @todo      N/A
 * @bug       N/A                                                                 
 **************************************************************************/
void appMenuDisp2LayerSelItemGetFocusFuncGui(void *menuItem)
{
	UINT32 currItem = 0;
	UINT8 layer,line,col,i;
	UINT32 strIdx;
	UINT8	nodeItem = 0;
//	UINT8	subItem=0;
	UINT8	selItem;
	UINT8	item = 0;
	UINT16 MENU_CHAR_WIDTH = appMenuDispParaGet(IDX_MENU_CHAR_WIDTH);
	UINT16 MENU_CHAR_HEIGHT = appMenuDispParaGet(IDX_MENU_CHAR_HEIGHT) ;
	UINT16 MENU_BAR_LAYER2_X_OFFSET = appMenuDispParaGet(IDX_MENU_BAR_LAYER2_X_OFFSET) ;
	UINT16 MENU_BAR_LAYER2_Y_OFFSET = appMenuDispParaGet(IDX_MENU_BAR_LAYER2_Y_OFFSET) ;
	UINT16 MENU_MU2BARSL_WIDTH = appMenuDispParaGet(IDX_MENU_MU2BARSL_WIDTH) ;
	layer =  menuProcLayerGet();	
	menuLayerSizeGet(layer, &line, &col);
	currItem=(UINT8)(*(UINT32*)menuItem);
	printf("focus get = %u\n",currItem);
	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);
	APP_OSD_REFRESH_OFF;
	menuBodyDisp(layer);

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);
	for(i=0; i<nodeItem; i++,item++)
	{
		UINT8 subItem;
		menuProcStrGet(layer,item, &strIdx);
		subItem = menuProcSubItemGet(layer, item);
		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(col-6, i, " ", 0, layer);
			appOsdColorSet(MENU_STR_DISABLE);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
				appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(54-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-2, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
			}
		}
		else if (currItem== item)
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
#if (KIT_WITH_HDMI)
			if(IS_HDMI_IN){
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,MENU_BAR_LAYER2_X_OFFSET*2+3,(i*(MENU_CHAR_HEIGHT-2)+MENU_BAR_LAYER2_Y_OFFSET+6)*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUSBAR_HDMI, 0x0000);
			}
			else
#endif
			{
				appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0,MENU_BAR_LAYER2_X_OFFSET,(i*MENU_CHAR_HEIGHT+MENU_BAR_LAYER2_Y_OFFSET), SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MUSUSBAR, 0x0000);
			}
			menuStrDraw(0, i, strIdx, 1, layer);
			if (subItem)
			{
				appOsdColorSet(MENU_TAB_GET_FOCUS);
#if (KIT_WITH_HDMI)
				if(IS_HDMI_IN){
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, ((col-2)*MENU_CHAR_WIDTH -9)<<1, ((i+2)*(MENU_CHAR_HEIGHT-2)+5)*3/2, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MU2LYBAR_HDMI, 0x0000);
				}
				else
#endif
				{
					appOsdLib_IconFileDraw(SP5K_GFX_PAGE_PIP_0, (col-2)*MENU_CHAR_WIDTH - 5, (i+2)*MENU_CHAR_HEIGHT-4, SP5K_GFX_ALIGN_TOP_LEFT, ID_ICONFILE_MU2LYBAR, 0x0000);
					printf("MENU_CHAR_WIDTH =%d ,MENU_CHAR_HEIGHT =%d ,xpos = %d,ypos = %d\n",MENU_CHAR_WIDTH,MENU_CHAR_HEIGHT, (col-2)*MENU_CHAR_WIDTH - 5, (i+2)*MENU_CHAR_HEIGHT-4);
				}
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
#if (KIT_WITH_HDMI)
				if(IS_HDMI_IN){
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH*2+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, ((i+2)*(MENU_CHAR_HEIGHT-2))*3/2, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				else
#endif				
				{
					printf("MENU_MU2BARSL_WIDTH =%d ,strWidth =%d ,xpos = %d,ypos = %d\n",MENU_MU2BARSL_WIDTH,appOsdLib_FontGetStringWidthbyId(strIdx),(col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-9);
				    appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-9, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
			}
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuStrDisp(col-6, i, " ", 0, layer);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
#if (KIT_WITH_HDMI)
				if(IS_HDMI_IN){
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH*2+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, ((i+2)*(MENU_CHAR_HEIGHT-2))*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				else
#endif
				{
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-9, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
			}
		}
	}
	APP_OSD_REFRESH_ON;	
}
/**************************************************************************
 * @fn        UINT32 appMenuDisp2LayerSelItemLostFocusFuncGui(void *menuItem)
 * @brief     appMenuDisp2LayerSelItemLostFocusFuncGui
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author   jj
 * @since     2013-4-22
 * @todo      N/A
 * @bug       N/A                                                                 
 **************************************************************************/
void appMenuDisp2LayerSelItemLostFocusFuncGui(void *menuItem)
{
	UINT32 currItem = 0;
	UINT8 layer,line,col,i;
	UINT32 strIdx;
	UINT8	nodeItem = 0;
	//UINT8	subItem=0;
	UINT8	selItem;
	UINT8	item = 0;
	UINT16 MENU_CHAR_WIDTH = appMenuDispParaGet(IDX_MENU_CHAR_WIDTH);
	UINT16 MENU_CHAR_HEIGHT = appMenuDispParaGet(IDX_MENU_CHAR_HEIGHT) ;
//	UINT16 MENU_BAR_LAYER2_X_OFFSET = appMenuDispParaGet(IDX_MENU_BAR_LAYER2_X_OFFSET) ;
//	UINT16 MENU_BAR_LAYER2_Y_OFFSET = appMenuDispParaGet(IDX_MENU_BAR_LAYER2_Y_OFFSET) ;
	UINT16 MENU_MU2BARSL_WIDTH = appMenuDispParaGet(IDX_MENU_MU2BARSL_WIDTH) ;
	layer =  menuProcLayerGet();
	menuLayerSizeGet(layer, &line, &col);
	currItem=(*(UINT32*)menuItem);
	printf("focus lost = %u\n",currItem);
	nodeItem = menuProcNodeItemGet(layer, item);
	nodeItem -= menuProcScrollGet(layer);
	APP_OSD_REFRESH_OFF;
	menuBodyDisp(layer);

	if (nodeItem > line)
		nodeItem = line;
	item = menuProcScrollGet(layer);
	for(i=0; i<nodeItem; i++,item++)
	{
		UINT8 subItem;
		menuProcStrGet(layer,item, &strIdx);
		subItem = menuProcSubItemGet(layer, item);
		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(0, i, strIdx, 0, layer);
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDisp(col-6, i, " ", 0, layer);
			appOsdColorSet(MENU_STR_DISABLE);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
			    appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(54-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-2, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
			}
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(0, i, strIdx, 0, layer);
			menuStrDisp(col-6, i, " ", 0, layer);
			if (subItem)
			{
				selItem = menuProcParaGet (layer, item);
				menuProcChildStrGet(layer, item, selItem, &strIdx);
#if (KIT_WITH_HDMI)
				if(IS_HDMI_IN){
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH*2+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, ((i+2)*(MENU_CHAR_HEIGHT-2))*3>>1, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
				else
#endif
				{
					appOsd_GfxStringIDDraw((col-2)*MENU_CHAR_WIDTH+(MENU_MU2BARSL_WIDTH-appOsdLib_FontGetStringWidthbyId(strIdx))/2, (i+2)*MENU_CHAR_HEIGHT-9, SP5K_GFX_ALIGN_TOP_LEFT, strIdx);
				}
			}
		}
	}	
	APP_OSD_REFRESH_ON;
}
/**************************************************************************
 * @fn        UINT32 appDlgBoxSelItemFuncGui(void *menuItem)
 * @brief     appDlgBoxSelItemFuncGui
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author   jj
 * @since     2013-4-22
 * @todo      N/A
 * @bug       N/A                                                                 
 **************************************************************************/

void appDlgBoxSelItemFuncGui(void *menuItem)
{
	UINT32 currItem = 0;
	/* UINT8 layer;
	layer =  menuProcLayerGet(); */	
	currItem=(*(UINT32*)menuItem);
		
	if(ENABLE == appTpKeyEnCheck(APP_KEY_PRESS_TPSET))
	{
		appPlayOsd_DlgBoxSelectSet(currItem);
		sp5kHostMsgSend(APP_KEY_PRESS_TPSET,0);/**/
	}
}
/**************************************************************************
 * @fn        UINT32 appDlgBoxSelItemGetFocusFuncGui(void *menuItem)
 * @brief     appDlgBoxSelItemGetFocusFuncGui
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author   jj
 * @since     2013-4-22
 * @todo      N/A
 * @bug       N/A                                                                 
 **************************************************************************/
void appDlgBoxSelItemGetFocusFuncGui(void *menuItem)
	{
		UINT32 currItem = 0;
/* 		UINT8 layer;
		layer =  menuProcLayerGet();	 */
		currItem=(*(UINT32*)menuItem);
		printf("focus get = %u\n",currItem);
		if(appPlayOsd_DlgBoxSelectGet() != currItem)
		{/**/
			appPlayOsd_DlgBoxSelectSet(currItem);
			APP_OSD_REFRESH_OFF;
			appPlayOsd_DlgBoxDraw();
			APP_OSD_REFRESH_ON;
		}/**/
		
	}
/**************************************************************************
 * @fn        UINT32 appDlgBoxSelItemLostFocusFuncGui(void *menuItem)
 * @brief     appDlgBoxSelItemLostFocusFuncGui
 * @param     [in] w
 * @param     [in] h
 * @retval    NONE
 * @see       NULL
 * @author   jj
 * @since     2013-4-22
 * @todo      N/A
 * @bug       N/A                                                                 
 **************************************************************************/
void appDlgBoxSelItemLostFocusFuncGui(void *menuItem)
{
//	UINT32 currItem = 0;
/* 	UINT8 layer;
	layer =  menuProcLayerGet();	 */
//	currItem=(*(UINT32*)menuItem);
	printf("focus lost\n");
	if(appPlayOsd_DlgBoxlastActSelectGet()!=appPlayOsd_DlgBoxSelectGet())
	{/**/
		appPlayOsd_DlgBoxSelectSet(0xff);
		APP_OSD_REFRESH_OFF;
		appPlayOsd_DlgBoxDraw();
		APP_OSD_REFRESH_ON;
	}/**/
	
}
void appMenuSelItemGetFocusFuncGui(void *menuItem)
{
	UINT8 currLine;
//	UINT8 scrollItem;
	UINT8 layer,line,col,i;
	UINT32 strIdx;
	//UINT16 icon;
	UINT8	nodeItem = 0;
//	UINT8	subItem=0;
//	UINT8	selItem;
	UINT8	item = 0;	
//	UINT8 strDrawX,strDrawY,barDispX,barDispY;
	UINT8 strDrawX,strDrawY,barDispY;
	UINT32 promptParm;
	currLine = (UINT8)(*(UINT32*)menuItem);
	layer =  menuProcLayerGet();
	if(layer == 0)layer=1;	
	/*printf("get %d\n",currLine);*/
	menuLayerSizeGet(layer, &line, &col);
	APP_OSD_REFRESH_OFF;
	menuBodyDisp(layer);
	nodeItem = menuProcNodeItemGet(layer, item);
	 /*modify bug ICOM-759*/
	if (nodeItem > line)
	{
		promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_OFF|MENU_PROMPT_PAGEUPDN_ON;
		
	}else
	{
	
		promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_OFF|MENU_PROMPT_PAGEUPDN_OFF;
		
	}
	nodeItem -= menuProcScrollGet(layer);
	if (nodeItem > line)
		nodeItem = line;
	

	if((fileTypeSelect == 1) || USBModeSet)
	{
		strDrawX =1;
		barDispY =1;
		strDrawY = 1;
	}
	else
	{
		strDrawX =2;
		barDispY =(line - nodeItem);
		strDrawY = (line - nodeItem);
	}
		
	item = menuProcScrollGet(layer);
	for(i=0; i<nodeItem; i++,item++)
	{
//		UINT16 icon;
		menuProcStrGet(layer,item, &strIdx);
//		icon = menuProcIconIDGet(layer, item);

		if (!menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(strDrawX, i + strDrawY, strIdx, 0, layer);
		
		}
		else if (i == currLine)
		{
			menuBarDisp(i+ barDispY, layer);
			appOsdColorSet(MENU_STR_GET_FOCUS);
			menuStrDraw(strDrawX, i+ strDrawY, strIdx, 1, layer);	
		}
		else
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			menuStrDraw(strDrawX, i+ strDrawY, strIdx, 0, layer);

		}
	}
	 /*modify bug ICOM-759*/
	menuPromptDisp(promptParm);
	//menuPromptDisp(0x07);
	APP_OSD_REFRESH_ON;
}

void appMenuSelItemLostFocusFuncGui(void *menuItem)
{	
//	UINT8 currLine;
//	UINT8 scrollItem;
	UINT8 layer,line,col,i;
	UINT32 strIdx;
	//UINT16 icon;
	UINT8	nodeItem = 0;
	//UINT8	subItem=0;
//	UINT8	selItem;
	UINT8	item = 0;	
//	UINT8 strDrawX,strDrawY,barDispX,barDispY;
	UINT8 strDrawX,strDrawY;
	UINT32 promptParm;
//	currLine = (UINT8)(*(UINT32*)menuItem);
	layer =  menuProcLayerGet();
	if(layer == 0)layer=1;	
	/*printf("get %d\n",currLine);*/
	menuLayerSizeGet(layer, &line, &col);
	APP_OSD_REFRESH_OFF;
	menuBodyDisp(layer);
	nodeItem = menuProcNodeItemGet(layer, item);
	  /*modify bug ICOM-759*/
	if (nodeItem > line)
	{
		promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_OFF|MENU_PROMPT_PAGEUPDN_ON;
		
	}else
	{
	
		promptParm = MENU_PROMPT_OK_ON| MENU_PROMPT_MENU_OFF|MENU_PROMPT_PAGEUPDN_OFF;
		
	}
	nodeItem -= menuProcScrollGet(layer);
	if (nodeItem > line)
		nodeItem = line;
	

	if((fileTypeSelect == 1) || USBModeSet)
	{
		strDrawX =1;
//		barDispY =1;
		strDrawY = 1;
	}
	else
	{
		strDrawX =2;
//		barDispY =(line - nodeItem);
		strDrawY = (line - nodeItem);
	}
		
	item = menuProcScrollGet(layer);
	for(i=0; i<nodeItem; i++,item++)
	{
		UINT8 subItem;
//		UINT16 icon;
		menuProcStrGet(layer, item, &strIdx);
//		icon = menuProcIconIDGet(layer, item);
		subItem = menuProcSubItemGet(layer, item);
		
		if (subItem > 0)
		{
//			selItem = menuProcParaGet (layer, item);
//			icon = menuProcChildIconIDGet(layer, item, selItem);
		}
		if (menuProcIsActive(layer, item))
		{
			appOsdColorSet(MENU_STR_LOST_FOCUS);
			
			menuStrDraw(strDrawX, i+ strDrawY, strIdx, 1, layer);	
			
		}
		else
		{
			appOsdColorSet(MENU_STR_DISABLE);
			menuStrDraw(strDrawX, i+ strDrawY, strIdx, 1, layer);	
			
		}
	}
	
	  /*modify bug ICOM-759*/
	menuPromptDisp(promptParm);
	//menuPromptDisp(0x07);
	APP_OSD_REFRESH_ON;
}

#endif
