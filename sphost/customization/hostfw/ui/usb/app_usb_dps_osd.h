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
#ifndef _APP_MENUTKDPSOSD_H_
#define _APP_MENUTKDPSOSD_H_

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                           VARIABLE                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 *************************************************************************/

#define DPS_TKSUBMENU_1ST_FRAME_X  45
#define DPS_TKSUBMENU_1ST_FRAME_Y  50

#define DPS_TKSUBMENU_2ST_FRAME_X  45
#define DPS_TKSUBMENU_2ST_FRAME_Y  60

#define DPS_TKSUBMENU_3ST_FRAME_X  45
#define DPS_TKSUBMENU_3ST_FRAME_Y  75

#define DPS_TKSUBMENU_4ST_FRAME_X  45
#define DPS_TKSUBMENU_4ST_FRAME_Y  82

#define DPS_TKSUBMENU_5ST_FRAME_X  45
#define DPS_TKSUBMENU_5ST_FRAME_Y  100

#define DPS_TKSUBMENU_6ST_FRAME_X  45
#define DPS_TKSUBMENU_6ST_FRAME_Y  128

#define DPS_TK2NDMENU_TITLE_FRAME_W     292
#define DPS_TK2NDMENU_TITLE_FRAME_H     41
#define DPS_TK2NDMENU_TITLE_FRAME_X     (320-292)/2
#define DPS_TK2NDMENU_TITLE_FRAME_Y     100
#define DPS_TK2NDMENU_TITLE_STR_X       160 /*  align center*/
#define DPS_TK2NDMENU_TITLE_STR_Y       100+DPS_TK2NDMENU_TITLE_FRAME_H/2
#define DPS_TK2NDMENU_TITLE_CENTER_FRAME_H  31
#define DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y  DPS_TK2NDMENU_TITLE_FRAME_Y+DPS_TK2NDMENU_TITLE_FRAME_H
#define DPS_TK2NDMENU_PRINT_ICON_X      (320-292)/2+40
#define DPS_TK2NDMENU_PRINT_ICON_Y      DPS_TK2NDMENU_TITLE_FRAME_Y+DPS_TK2NDMENU_TITLE_FRAME_H+5
#define DPS_TK2NDMENU_STR_BACK_X        (320-292)/2+40
#define DPS_TK2NDMENU_STR_BACK_Y        DPS_TK2NDMENU_TITLE_CENTER_FRAME_Y+DPS_TK2NDMENU_TITLE_CENTER_FRAME_H+5
#define DPS_TK2NDMENU_SELBAR_X          170
#define DPS_TK2NDMENU_SELBAR_Y          DPS_TK2NDMENU_TITLE_FRAME_Y+DPS_TK2NDMENU_TITLE_FRAME_H+3
#define DPS_TK2NDMENU_SELBAR_W          78
#define DPS_TK2NDMENU_RIGHT_ARROW_X     170+DPS_TK2NDMENU_SELBAR_W+5
#define DPS_TK2NDMENU_RIGHT_ARROW_Y     DPS_TK2NDMENU_TITLE_FRAME_Y+DPS_TK2NDMENU_TITLE_FRAME_H+8
#define DPS_TK2NDMENU_LEFT_ARROW_X      170-15
#define DPS_TK2NDMENU_LEFT_ARROW_Y      DPS_TK2NDMENU_TITLE_FRAME_Y+DPS_TK2NDMENU_TITLE_FRAME_H+8
#define DPS_TK2NDMENU_NUM_X             DPS_TK2NDMENU_SELBAR_X+DPS_TK2NDMENU_SELBAR_W/2-10
#define DPS_TK2NDMENU_NUM_Y             DPS_TK2NDMENU_TITLE_FRAME_Y+DPS_TK2NDMENU_TITLE_FRAME_H+5

typedef enum DPS_MENU {
	DPS_MENU_CHANGE		=0x00,
	DPS_MENU_PRINT 		=0x01,
	DPS_MENU_IMAGE 		=0x02,
	DPS_MENU_QUALITY	=0x03,
	DPS_MENU_LAYOUT		=0x04,
	DPS_MENU_PAPER_SIZE	=0x05,
	DPS_MENU_PAPER_TYPE	=0x06,
	DPS_MENU_DATE		=0x07,
	DPS_MENU_FILE_NO	=0x08,
	DPS_MENU_DATETYPE 	=0x09,
	DPS_MENU_RESET 		=0X0A,
	DPS_MENU_TOTAL 		=0X0B
}DPS_MENU;

enum {
	DPS_MENU_2ND_SELECT0,
	DPS_MENU_2ND_SELECT1
};
/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

void appDPSMenu_DPSMainMenuSubItemDetailStrSet(unsigned char nCurTab, unsigned short **pStrId,unsigned short **pIconId, unsigned short **pIconIdColor);
void appDPSMenu_DPSMainMenuTitleDraw();
UINT8 appDPSMenu_CurrentSettingGet(unsigned short GetSettingCurTab);
void appDPSMenu_DPSMainMenuTitleChangeDraw();
void appDPSMenu_DPSSubMenuSelHiddenFrameDraw();
void appDPSMenu_DPSSubMenuSelHiddenDraw(void);
void appDPSMenu_DPSSubMenuFrameDraw();
void appDPSMenu_DPSSubMenuDetailItemSetandDraw (unsigned short wTitle,unsigned int StrStarX,unsigned int StrStarY);
void appDPSMenu_DPSSubMenuDetailInfoDraw(unsigned short *pwIconId, unsigned short *pwStrId,unsigned short wTitle,unsigned int StarX,unsigned int StarY);
void appDPSMenu_DPSSubMenuTitleDraw(unsigned short wTitle,unsigned short *pwStrId,unsigned short *pwIconId,unsigned int StarX,unsigned int StarY);
void appDPSMenu_DPSSubMenuFrameChangeDraw();
void appDPSMenu_DPSSubMenuDetailItemChangeSetandDraw(unsigned short wTitle,unsigned int StrStarX,unsigned int StrStarY);
void appDPSMenu_DPSSubMenuDetailInfoChangeDraw(unsigned short *pwIconId, unsigned short *pwStrId,unsigned short wTitle,unsigned int StarX,unsigned int StarY);
void appDPSMenu_DPSSubMenuTitleChange(unsigned short wTitle,unsigned short *pwStrId,unsigned short *pwIconId, unsigned int StarX,unsigned int StarY );
void appDPSMenu_DPSSubMenuFrameReDraw();
void appDPSMenu_DPS2ndMenuItenSetandDraw( unsigned short wTitle, unsigned short SelectIndex);
void appDPSMenu_DPS2ndMenuHelpItemDraw(BOOL bShow);
void appDPSMenu_DPS2ndMenuImageFrameDraw(unsigned short SelectIndex,UINT16 Num);
void appDPSMenu_DPS2ndMenuImageSelChangeDraw(unsigned short SelectIndex,UINT16 Num);
void appDPSMenu_DPS2ndMenuImageUpDownChangeDraw(unsigned short SelectIndex,UINT16 Num);
void appDPSMenu_DpsOSDIconDraw(UINT8 bShow);

#endif
