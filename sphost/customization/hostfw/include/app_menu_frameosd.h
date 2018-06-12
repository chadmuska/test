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

#ifndef _APP_MENUFRAMEOSD_H_
#define _APP_MENUFRAMEOSD_H_

#include "sp5k_gfx_api.h"

#include "app_osdfont.h"

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
 **************************************************************************/
#define TKTYPE_MENU_BLEND	PAL_BLEND_50

#define TKTYPE_MENUBODY_W		                320
#define TKTYPE_MENUBODY_H		                240
#define TKTYPE_MENUBODY_TRANSPARENT_OFFSET_X    13
#define TKTYPE_MENUBODY_TRANSPARENT_OFFSET_Y  	4

#define TKTYPE_MENUBODY_TRANSPARENT_W		    275
#define TKTYPE_MENUBODY_TRANSPARENT_H		    164

#define TKTYPE_STILLVIEW_MENUBAR_START_X  	    0
#define TKTYPE_STILLVIEW_MENUBAR_START_Y  	    42
#define TKTYPE_STILLVIEW_MENUTAB_START_X  	    5
#define TKTYPE_STILLVIEW_MENUTAB_START_Y  	    0
#define TKTYPE_STILLVIEW_SETUPTAB_START_X  	    69
#define TKTYPE_STILLVIEW_SETUPTAB_START_Y  	    0
#define TKTYPE_STILLVIEW_MENU_STILLSTART_Y      7
#define TKTYPE_STILLVIEW_MENU_STILLSTART_X      26
#define TKTYPE_STILLVIEW_MENU_SETUPSTART_X      90
#define TKTYPE_STILLVIEW_MENU_SETUPSTART_Y      7

#define TKTYPE_STILLVIEW_MENU_LEFTARROWSTART_X  24
#define TKTYPE_STILLVIEW_MENU_ARROWSTART_Y      18
#define TKTYPE_STILLVIEW_MENU_RIGHTARROWSTART_X 84

#define TKTYPE_STILLVIEW_MENU_MAINSTART_X       0
#define TKTYPE_STILLVIEW_MENU_MAINSTART_Y       0

#define TKTYPE_MENUICON_OKSTART_X               20
#define TKTYPE_MENUICON_OKSTART_Y               212

#define TKTYPE_MENUICON_MENUSTART_X             190

#define TKTYPE_MENUSTRING_OKSTART_X             78
#define TKTYPE_MENUSTRING_OKSTART_Y             212

#define TKTYPE_MENSTRING_MENUSTART_X            246

#define TKTYPE_STILLVIEW_MENU_SELBAR_W		    264
#define TKTYPE_STILLVIEW_MENU_SELBAR_H		    29

#define TKTYPE_STILLVIEW_ARROWCLEAR_W           7
#define TKTYPE_STILLVIEW_ARROWCLEAR_H           12
#define TKTYPE_STILLVIEW_MENU_SELBAR_CLEAR_W	257

#define TKTYPE_STILLVIEW_SUBMENU_4ITEM_W		270
#define TKTYPE_STILLVIEW_SUBMENU_4ITEM_H		143
#define TKTYPE_STILLVIEW_SUBMENUBAR_W		    237
#define TKTYPE_STILLVIEW_SUBMENUBAR_H		    29
#define TKTYPE_STILLVIEW_SUBMENU_3ITEM_W		270
#define TKTYPE_STILLVIEW_SUBMENU_3ITEM_H		109
#define TKTYPE_STILLVIEW_SUBMENU_2ITEM_W		270
#define TKTYPE_STILLVIEW_SUBMENU_2ITEM_H		80

#define TKTYPE_STILLVIEW_SUBMENU_OKSTART_X      78
#define TKTYPE_STILLVIEW_SUBMENU_OKSTART_Y      220
#define TKTYPE_STILLVIEW_SUBMENU_OKSTART_CLEAR_W  88
#define TKTYPE_STILLVIEW_SUBMENU_OKSTART_CLEAR_H  30

#define TKTYPE_WARNING_X                        60
#define TKTYPE_WARNING_Y                        65
#define TKTYPE_WARNING_W                        200
#define TKTYPE_WARNING_H                        110
#define TKTYPE_WARNING_ICON_X                   (TKTYPE_WARNING_X+(TKTYPE_WARNING_W/2)-23)
#define TKTYPE_WARNING_ICON_Y                   TKTYPE_WARNING_Y+10
#define TKTYPE_WARNING_TEXT_CLEAR_W             (TKTYPE_WARNING_W-8)
#define TKTYPE_WARNING_TEXT_CLEAR_H             40
#define TKTYPE_VOICEMEMO_WARNING_Y              50

#define TKTYPE_WARNING_FRAME2_X                 40
#define TKTYPE_WARNING_FRAME2_Y                 65
#define TKTYPE_WARNING_FRAME2_W                 240
#define TKTYPE_WARNING_FRAME2_H                 110
#define TKTYPE_WARNING_FRAME2_ICON_X            (TKTYPE_WARNING_FRAME2_X+(TKTYPE_WARNING_FRAME2_W/2)-23)
#define TKTYPE_WARNING_FRAME2_ICON_Y            TKTYPE_WARNING_FRAME2_Y+10
#define TKTYPE_WARNING_FRAME2_TEXT_CLEAR_W      (TKTYPE_WARNING_FRAME2_W-8)
#define TKTYPE_WARNING_FRAME2_TEXT_CLEAR_H      40
#define TKTYPE_VOICEMEMO_FRAME2_WARNING_Y       50

#define TKTYPE_PLAY_MENU_STILLSTART_X           26
#define TKTYPE_PLAY_MENU_STILLSTART_Y           7
#define TKTYPE_PLAY_MENU_SETUPSTART_X           90
#define TKTYPE_PLAY_MENU_SETUPSTART_Y           7
#define TKTYPE_PLAY_MENU_LEFTARROWSTART_X       24
#define TKTYPE_PLAY_MENU_ARROWSTART_Y           18
#define TKTYPE_PLAY_MENU_RIGHTARROWSTART_X      84

#define TKTYPE_PLAY_MENU_MAINSTART_X            0
#define TKTYPE_PLAY_MENU_MAINSTART_Y            0

#define TKTYPE_PLAY_MENU_FILETYPE_START_X       0
#define TKTYPE_PLAY_MENU_FILETYPE_START_Y       0
#define TKTYPE_PLAY_MENU_FILETYPE_BLEND_X	    (TKTYPE_PLAY_MENU_FILETYPE_START_X+10)
#define TKTYPE_PLAY_MENU_FILETYPE_BLEND_Y	    (TKTYPE_PLAY_MENU_FILETYPE_START_Y+38)

#define TKTYPE_PLAY_MENU_SELBAR_W		        264
#define TKTYPE_PLAY_MENU_SELBAR_H		        29

#define TKTYPE_PLAY_ARROWCLEAR_W                7
#define TKTYPE_PLAY_ARROWCLEAR_H                12
#define TKTYPE_PLAY_MENU_SELBAR_CLEAR_W	        257
#define TKTYPE_PLAY_SUBMENU_4ITEM_W		        270
#define TKTYPE_PLAY_SUBMENU_4ITEM_H		        143
#define TKTYPE_PLAY_SUBMENUBAR_W		        237
#define TKTYPE_PLAY_SUBMENUBAR_H		        29
#define TKTYPE_PLAY_SUBMENU_3ITEM_W		        270
#define TKTYPE_PLAY_SUBMENU_3ITEM_H		        109
#define TKTYPE_PLAY_SUBMENU_2ITEM_W		        270
#define TKTYPE_PLAY_SUBMENU_2ITEM_H		        80

#define TKTYPE_PLAY_SUBMENU_OKSTART_X           78
#define TKTYPE_PLAY_SUBMENU_OKSTART_Y           220
#define TKTYPE_PLAY_SUBMENU_OKSTART_CLEAR_W     88
#define TKTYPE_PLAY_SUBMENU_OKSTART_CLEAR_H     30

#define TKTYPE_PLAY_TK3NDMENU_FRAME_X           10
#define TKTYPE_PLAY_TK3NDMENU_FRAME_Y           50

#define TKTYPE_PLAY_CONFIRMMENU2_STR_X_OFFSET   32
#define TKTYPE_PLAY_CONFIRMMENU2_STR_Y_OFFSET   46

#define TKTYPE_PLAY_CONFIRMMENU2_FRAME_X_OFFSET 2
#define TKTYPE_PLAY_CONFIRMMENU2_FRAME_Y_OFFSET 40

#define TKTYPE_PLAY_CONFIRMMENU3_FRAME_GAP      40
#define TKTYPE_PLAY_CONFIRMMENU3_STR_GAP  	    42
#define HDMI_PLAY_CONFIRMMENU3_FRAME_GAP        32
#define HDMI_PLAY_CONFIRMMENU3_STR_GAP  	    32

#define TKTYPE_PLAY_DPOF_BAR_X_W                95
#define TKTYPE_PLAY_DPOF_BAR_X_OFFSET           110
#define TKTYPE_PLAY_DPOF_LEFTARROW_X_OFFSET     26
#define TKTYPE_PLAY_DPOF_LEFTARROW_Y_OFFSET     0
#define TKTYPE_PLAY_DPOF_RIGHTARROW_X_OFFSET    5
#define TKTYPE_PLAY_DPOF_CLEAR_BAR_H		    39

#define TKTYPE_PLAY_SUBMENU_STR_GAP  	        42

#define TKTYPE_MAINMENUSCROLL_STARTX  	        291
#define TKTYPE_MAINMENUSCROLL_STARTY  	        50
#define TKTYPE_MAINMENUSCROLL_WIDTH  		    4
#define TKTYPE_MAINMENUSCROLL_HEIGHT  	        147
#define TKTYPE_MAINMENUSCROLL_PAGEITEM  	    5
#define TKTYPE_MAINMENUSCROLL_STARTITEM         1
#define TKTYPE_MAINMENUSCROLL_COLOR  		    PAL_WHITE

#define TKTYPE_MENU_BAR_CLEAR_W		            257
#define TKTYPE_MENU_SETUPMENU_BAR_W             264

#define TKTYPE_MENUSUB2_W		                270
#define TKTYPE_MENUSUB2_H		                80
#define TKTYPE_MENUSUB3_W		                270
#define TKTYPE_MENUSUB3_H		                109
#define TKTYPE_MENUSUB4_W		                270
#define TKTYPE_MENUSUB4_H		                143
#define TKTYPE_MENUSUB4_W		                270
#define TKTYPE_MENUSUB4_H		                143

#define TKTYPE_SETUP_MENU_SETSTART_X            80
#define TKTYPE_SETUP_MENU_SETLSTART_Y           4
#define TKTYPE_SSNAPSHAPE_MENU_OKSTART_X        70
#define TKTYPE_SSNAPSHAPE_MENU_MENUSTART_X      200
#define TKTYPE_SNAPSHAPE_MENU_OKSTART_Y         210
#define TKTYPE_SETUP_MENU_MAINSTART_X           14
#define TKTYPE_SETUP_MENU_MAINSTART_Y           38
#define TKTYPE_SETUP_MENU_LEFTARROWSTART_X      72
#define TKTYPE_SETUP_MENU_ARROWSTART_Y          18
#define TKTYPE_SETUP_MENU_RIGHTARROWSTART_X     132
#define TKTYPE_MENUICON_OFFSET_X		        36
#define TKTYPE_MENU2NDBAR_W		                60
#define TKTYPE_MENU2NDBAR_H		                32

#define SCENEMODE_MENU_START_X                  14
#define SCENEMODE_MENU_START_Y                  4
#define SCENEMODE_MENU_BLEND_OFFSET_X	        (SCENEMODE_MENU_START_X+10)
#define SCENEMODE_MENU_BLEND_OFFSET_Y	        (SCENEMODE_MENU_START_Y+38)
#define IconSize36_X                            36
#define IconSize32_Y                            32

#define TK_LANGUAGEMENUSCROLL_STARTX            300
#define TK_LANGUAGEMENUSCROLL_STARTY            73
#define TK_LANGUAGEMENUSCROLL_WIDTH	            4
#define TK_LANGUAGEMENUSCROLL_HEIGHT            118
#define TK_LANGUAGEMENUSCROLL_TOTALITEM         LANG_MAX
#define TK_LANGUAGEMENUSCROLL_PAGEITEM	        4
#define TK_LANGUAGEMENUSCROLL_STARTITEM	        0
#define TK_LANGUAGEMENUSCROLL_COLOR		        PAL_WHITE

#define TK_SUBMENU_SCROLLBAR_X		            299
#define TK_SUBMENU_SCROLLBAR_Y		            60
#define TK_SUBMENU_SCROLLBAR_W		            5
#define TK_SUBMENU_SCROLLBAR_H		            121
#define TK_SUBMENU_SCROLLBAR_PAGEITEM	        4
#define TK_SUBMENU_SCROLLBAR_STARTITEM	        0

/*  DPS  */
#define DPS_SUBMENU_SCROLLBAR_X		            299
#define DPS_SUBMENU_SCROLLBAR_Y		            60

#define TKTYPE_DPS_MENU_MAINSTART_X             14
#define TKTYPE_DPS_MENU_MAINSTART_Y             38

#define appMenu_TKSubMenuScrollBarCursorShow(bShow, nCurItem,nTotalItem) 		appMenu_TKMenuScrollBarCursorDraw(bShow,TK_SUBMENU_SCROLLBAR_X,DPS_SUBMENU_SCROLLBAR_Y,TK_SUBMENU_SCROLLBAR_W,TK_SUBMENU_SCROLLBAR_H, nCurItem,nTotalItem, TK_SUBMENU_SCROLLBAR_PAGEITEM, TK_SUBMENU_SCROLLBAR_STARTITEM, TKTYPE_MAINMENUSCROLL_COLOR);
#define UIMenu_DrawLanguageMenuScrollBarCursor(bShow, nCurItem,nTotalItem)      appMenu_TKMenuScrollBarCursorDraw(bShow, TK_LANGUAGEMENUSCROLL_STARTX, TK_LANGUAGEMENUSCROLL_STARTY, TK_LANGUAGEMENUSCROLL_WIDTH, TK_LANGUAGEMENUSCROLL_HEIGHT, nCurItem,nTotalItem,  TK_LANGUAGEMENUSCROLL_PAGEITEM, TK_LANGUAGEMENUSCROLL_STARTITEM, TK_LANGUAGEMENUSCROLL_COLOR);

#define appMenu_TKMenuScrollBarCursorShow(bShow, nCurItem,nTotalItem) 			appMenu_TKMenuScrollBarCursorDraw(bShow,TKTYPE_MAINMENUSCROLL_STARTX,TKTYPE_MAINMENUSCROLL_STARTY,TKTYPE_MAINMENUSCROLL_WIDTH,TKTYPE_MAINMENUSCROLL_HEIGHT, nCurItem,nTotalItem, TKTYPE_MAINMENUSCROLL_PAGEITEM, TKTYPE_MAINMENUSCROLL_STARTITEM, TKTYPE_MAINMENUSCROLL_COLOR);
#define appMenu_TKMenuViewMenuSelectBarFrameDraw(bShow,StarX,StarY) 			appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_MENU_SELBAR_W,TKTYPE_STILLVIEW_MENU_SELBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENUSELBAR,0x0000);
#define appMenu_TKMenuViewMenuSelectBarFrameHiddenDraw(bShow,StarX,StarY)		appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_MENU_SELBAR_W,TKTYPE_STILLVIEW_MENU_SELBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_MENUSUBUBAR,0x0000);
#define appMenu_TKMenuViewSubMenu4ScrollFrameDraw(bShow,StarX,StarY)			appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_4ITEM_W,TKTYPE_STILLVIEW_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_4ITEMSC,0x0000);
#define appMenu_TKMenuViewSubMenuSelectFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENUBAR,0x0000);
#define appMenu_TKMenuViewSubMenu3ScrollFrameDraw(bShow,StarX,StarY)			appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_3ITEM_W,TKTYPE_STILLVIEW_SUBMENU_3ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_3ITEM,0x0000);
#define appMenu_TKMenuViewSubMenu2ScrollFrameDraw(bShow,StarX,StarY)			appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_2ITEM_W,TKTYPE_STILLVIEW_SUBMENU_2ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_2ITEM,0x0000);
#define appMenu_TKMenuViewSubMenuClearMenuBarFrameDraw(bShow,StarX,StarY)		appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_CLEARBAR,0x0000);
#define appMenu_TKMenuSetupMenuMainBarFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TK_SUMENUUBAR,0x0000);
#define appMenu_TKMenuSetupMenuSISubBarFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TK_MENUSISUBUBAR,0x0000);
#define appMenu_TKMenuSetupMenuSubBarFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TK_SUMENUSUBUBAR,0x0000);
#define appMenu_TKMenuSetupMenuSub2RowFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_MENUSUB2_W,TKTYPE_MENUSUB2_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TK_SUMENUSUB2,0x0000);
#define appMenu_TKMenuSetupMenuSub3RowFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_MENUSUB3_W,TKTYPE_MENUSUB3_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TK_SUMENUSUB3,0x0000);
#define appMenu_TKMenuSetupMenuSub4RowFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_4ITEM_W,TKTYPE_STILLVIEW_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TK_SUMENUSUB4,0x0000);
#define appMenu_TKMenuSetupMenuSub4RowWithScrollFrameDraw(bShow,StarX,StarY)	appMenu_TKMenuIconFileDraw(bShow,StarX,StarY-3,TKTYPE_MENUSUB4_W,TKTYPE_MENUSUB4_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TK_SUMENUSUB4SCROLL,0x0000);
#define appMenu_TKMenuPBMenuSelectBarFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_MENU_SELBAR_W,TKTYPE_PLAY_MENU_SELBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENUSELBAR,0x0000);
#define appMenu_TKMenuPBMenuSelectBarFrameHiddenDraw(bShow,StarX,StarY)			appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_MENU_SELBAR_W,TKTYPE_PLAY_MENU_SELBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_MENUSUBUBAR,0x0000);
#define appMenu_TKMenuPBSubMenu4ScrollFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENU_4ITEM_W,TKTYPE_PLAY_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_4ITEMSC,0x0000);
#define appMenu_TKMenuPBSubMenu4FrameDraw(bShow,StarX,StarY)					appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENU_4ITEM_W,TKTYPE_PLAY_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_4ITEM,0x0000);
#define appMenu_TKMenuPBSubMenuSelectFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENUBAR,0x0000);
#define appMenu_TKMenuPBSubMenu3FrameDraw(bShow,StarX,StarY)					appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENU_3ITEM_W,TKTYPE_PLAY_SUBMENU_3ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_3ITEM,0x0000);
#define appMenu_TKMenuPBSubMenu2FrameDraw(bShow,StarX,StarY)					appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENU_2ITEM_W,TKTYPE_PLAY_SUBMENU_2ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_2ITEM,0x0000);
#define appMenu_TKMenuPBSubMenuClearMenuBarFrameDraw(bShow,StarX,StarY)			appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_CLEARBAR,0x0000);
#define appMenu_TKMenuSetupMenuSubBarFrameClear(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TK_CLEAR_SUMENUSUBUBAR,0x0000);
#define appMenu_TKMenuSetupMenuSISubBarFrameClear(bShow,StarX,StarY)			appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TK_CLEAR_SUMENUSISUBUBAR,0x0000);
#define appMenu_TKMenuSetupMenu2ndSoundSlBarFrame(bShow,StarX,StarY)			appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_MENU2NDBAR_W,TKTYPE_MENU2NDBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_ICON_TK_SMU2BAR,0x0000);
#define appMenu_TKMenuPBMenuSubMenuSlideArrowBarFrame(bShow,StarX,StarY)		appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TK_MENUSUBSETBAR,0x0000);
#define appMenu_TKMenuPBMenuSubMenuSlideSelBar(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TK_SLSHOWBAR,0x0000);
#define appMenu_TKMenuSetupMenu2ndDateTimeMenuFrame(bShow,StarX,StarY)			appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_MENUSUB4_W,TKTYPE_MENUSUB4_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TK_MENUDT,0x0000);

#define appMenu_TKMenuSenceMenuScrollBarDrawType1(bShow,StarX,StarY)	        appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TK_SUBMENUSCROLL_TYPE,0x0003);
#define appMenu_TKMenuSenceMenuScrollBarDrawType2(bShow,StarX,StarY)	        appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TK_SUBMENUSCROLL_TYPE,0x0004);

#define appMenu_TKMenuDPSMenuSelectBarFrameDraw(bShow,StarX,StarY) 				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_MENU_SELBAR_W,TKTYPE_STILLVIEW_MENU_SELBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENUSELBAR,0x0000);
#define appMenu_TKMenuDPSMenuSelectBarFrameHiddenDraw(bShow,StarX,StarY)		appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_MENU_SELBAR_W,TKTYPE_STILLVIEW_MENU_SELBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_MENUSUBUBAR,0x0000);
#define appMenu_TKMenuDPSMenuSub2RowFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_2ITEM_W,TKTYPE_STILLVIEW_SUBMENU_2ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_2ITEM,0x0000);
#define appMenu_TKMenuDPSMenuSub3RowFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_3ITEM_W,TKTYPE_STILLVIEW_SUBMENU_3ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_3ITEM,0x0000);
#define appMenu_TKMenuDPSMenuSub4RowFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_4ITEM_W,TKTYPE_STILLVIEW_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_4ITEMSC,0x0000);
#define appMenu_TKMenuDPSpMenuSub4RowWithScrollFrameDraw(bShow,StarX,StarY)	    appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENU_4ITEM_W,TKTYPE_STILLVIEW_SUBMENU_4ITEM_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_4ITEMSC,0x0000);
#define appMenu_TKMenuDPSSubMenuSelectFrameDraw(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENUBAR,0x0000);
#define appMenu_TKMenuDPSMenuSubMenuSlideArrowBarFrame(bShow,StarX,StarY)		appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TK_MENUSUBSETBAR,0x0000);
#define appMenu_TKMenuDPSSubMenuClearMenuBarFrameDraw(bShow,StarX,StarY)		appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_STILLVIEW_SUBMENUBAR_W,TKTYPE_STILLVIEW_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TKMENU_SUBMENU_CLEARBAR,0x0000);
#define appMenu_TKMenuPBMenu3ndMenuDpofSelBar(bShow,StarX,StarY)				appMenu_TKMenuIconFileDraw(bShow,StarX,StarY,TKTYPE_PLAY_SUBMENUBAR_W,TKTYPE_PLAY_SUBMENUBAR_H,SP5K_GFX_ALIGN_TOP_LEFT,ID_MENU_TK_SLSHOWBAR,0x0000);

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appMenu_TKMenuBodyFrameDraw(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKStillViewMenuFrameDraw(unsigned char bShow);
void appMenu_TKStillViewMenuSelectFrameDraw(unsigned char bShow);
void appMenu_TKMenuScrollBarCursorDraw(BOOL bShow, unsigned int nScrollStarX, unsigned int nScrollStarY,  unsigned int nScrollWidth, unsigned int nScrollHeight, unsigned short nCurItem, unsigned short nTotalNum, unsigned short nPageItem, unsigned short nStartItem, unsigned int CursorColor);
void appMenu_TKMenuIconFileDraw(unsigned char bShow,unsigned int StarX,unsigned int StarY,unsigned int width,unsigned int high,UINT32 align,UINT8 *fileName,UINT32 IconIndex);
void appMenu_TKMenuViewMenuSelectArrow (unsigned char bShow);
void appMenu_TKMenuViewMenuClearMenuBarFrameDraw(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKMenuBodyHiddenDraw(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKMenuViewMenuFrameHiddenDraw(unsigned char bShow);
void appMenu_TKMenuViewSubMenuHelpFrame(unsigned char bShow);
void appMenu_TKMenuVideoMenuFrameDraw(unsigned char bShow);
void appMenu_TKMenuVideoMenuSelectFrameDraw(unsigned char bShow);
void appMenu_TKCalibMenuFrameDraw(unsigned char bShow);
void appMenu_TKTypeWarningMsgFrameDraw(unsigned char bShow);
void appMenu_TKMenuSetupMenuBodyFrameDraw(unsigned char bShow,	unsigned int StarX, unsigned int StarY);
void appMenu_TKMenuSetUpClearSetupMenuBarFrame(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKMenuSetupMenuDraw( unsigned char bShow);
void appMenu_TKMenuSetUpMainMenuItemArrowDraw(unsigned char bShow);
void appMenu_TKMenuSetupMenuSelectDraw(unsigned char bShow);
void appMenu_TKMenuSetupMenuHiddenDraw(unsigned char bShow);
void appMenu_TKMenuSetupMenuBodyFrameHiddenDraw(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKMenuDpsMenuFrameDraw(unsigned char bShow);
void appMenu_TKMenuBodyFrameDraw(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKMenuPBMenuFrameDraw(unsigned char bShow);
void appMenu_TKMenuPBMenuSelectFrameDraw(unsigned char bShow);
void appMenu_TKMenuScrollBarCursorDraw(BOOL bShow, unsigned int nScrollStarX, unsigned int nScrollStarY,  unsigned int nScrollWidth, unsigned int nScrollHeight, unsigned short nCurItem, unsigned short nTotalNum, unsigned short nPageItem, unsigned short nStartItem, unsigned int CursorColor);
void appMenu_TKMenuPBMenuSelectArrow (unsigned char bShow);
void appMenu_TKMenuPBMenuClearMenuBarFrameDraw(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKMenuBodyHiddenDraw(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKMenuPBMenuFrameHiddenDraw(unsigned char bShow);
void appMenu_TKMenuPlaySubMenuHelpFrame(unsigned char bShow);
void appMenu_TKMenuPBMenu2ndConfirm2Frame(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKMenuSceneModeMenuFrameDraw(unsigned char bShow);
void appMenu_TKMenuSceneModeMenuBodyFrameDraw(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKTypeWarningMsgBigFrame2Draw(unsigned char bShow);
void appMenu_TKMenuDpsMenuFrameHiddenDraw(unsigned char bShow);
void appMenu_TKMenuDpsSubMenuScrollBarDraw(unsigned char bShow,unsigned char Index);
void appMenu_TKMenuFileModeMenuBodyFrameDraw(unsigned char bShow,unsigned int StarX,unsigned int StarY);
void appMenu_TKMenuPBVideoMenuFrameDraw(unsigned char bShow);
void appMenu_TKMenuPBVideoMenuSelectFrameDraw(unsigned char bShow);
#endif /* _APP_MENUFRAMEOSD_H_ */

