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
#include "app_res_def.h"

#ifndef _APP_OSDFONT_H_
#define _APP_OSDFONT_H_

/*********FONT SIZE************************************************/
#define FontEngNumSize_X	16
#define FontEngNumSize_Y	28

/*EZ_GUI menu*/
#define ID_FONT_ICON	appOsd_FontIconResSelect()
#define ID_FONT_DATESTAMP "UI\\FONT\\DATSTAMP.SFN"
#define ID_FONT_LOGO    "UI\\FONT\\LOGO.SFN"
#define ID_ICON_WARNMEDIASTANDBY	"UI\\ICON\\WARNM.SFN"
/*Cathylite menu*/
/*FONT*/
#define ID_FONT_ENG22X28			        RES_FONT_FONTENG
#define ID_FONT_TCH28X28			        RES_FONT_TCHFONT
#define ID_FONT_SCH28X28			        RES_FONT_SCHFONT
#define ID_FONT_ENG42			        	RES_FONT_BENG
#define ID_FONT_TCH42			        	RES_FONT_BTCH
#define ID_FONT_SCH42			        	RES_FONT_BSCH
#define ID_FONT_ARABIC28X28			        RES_FONT_ARAFONT
#define ID_FONT_THAI28X28			        RES_FONT_FONTTAI
#define ID_FONT_JAPAN28x28			        RES_FONT_JAPFONT
#define ID_FONT_KOREAN28X28			        RES_FONT_KORFONT

/*ICON*/
#define ID_MENU_TKMENUBODY					RES_ICON_MUBODY			
#define ID_MENU_MENUBODY_HDMI				RES_ICON_MUBODY_HDMI
#define ID_MENU_TKMENUSELBAR			    RES_ICON_MUBAR      
#define ID_MENU_MENUSELBAR_HDMI			    RES_ICON_MUBAR_HDMI
#define ID_MENU_TYMENUMAINVIEWCLEARBAR		RES_ICON_MUBARCL    
#define ID_MENU_TKMENU_MENUBODYHL			RES_ICON_MUBODYHL   
#define ID_MENU_TKMENU_MENUSUBUBAR			RES_ICON_MUBARHD    
#define ID_MENU_TKMENU_SUBMENU_4ITEMSC		RES_ICON_MUSUBSC    
#define ID_MENU_SUBMENU_4ITEMSC_HDMI		RES_ICON_MUSUBSC_HDMI
#define ID_MENU_TKMENU_SUBMENUBAR			RES_ICON_MUSUSBAR     
#define ID_MENU_TK_MENUSUBSETBAR	        RES_ICON_MUSUSBAR
#define ID_MENU_SUBSETBAR_HDMI	      		RES_ICON_MUSUSBAR_HDMI
#define ID_MENU_SUBMENUBAR_HDMI				RES_ICON_MUSUSBAR_HDMI
#define ID_MENU_TKMENU_SUBMENU_3ITEM		RES_ICON_MU2ND3     
#define ID_MENU_SUBMENU_3ITEM_HDMI			RES_ICON_MU2ND3_HDMI
#define ID_MENU_TKMENU_SUBMENU_2ITEM		RES_ICON_MUSUB2     
#define ID_MENU_TKMENU_SUBMENU_CLEARBAR		RES_ICON_MUSBARCL   
#define ID_MENU_TKMENU_SUBMENU_4ITEM		RES_ICON_MUSUB4    
#define ID_MENU_TK_MENUSCENE				RES_ICON_MUSCENE     
#define ID_MENU_TK_MENUDT					RES_ICON_MUDT 
#define ID_MENU_TK_SUBMENUSCROLL_TYPE   	RES_ICON_SSCR 
#define ID_MENU_TK_2NDMENU2					RES_ICON_MU2ND2     
#define ID_MENU_2NDMENU2_HDMI				RES_ICON_MU2ND2_HDMI
#define ID_MENU_TK_SLSHOWBAR				RES_ICON_MUSUSBAR 
#define ID_MENU_SLSHOWBAR_HDMI				RES_ICON_MUSUSBAR_HDMI
#define ID_ICON_TK_MENUBODYHL				RES_ICON_MUBODYHL   
#define ID_ICON_TK_MENUSISUBUBAR			RES_ICON_MUSIBAR  
#define ID_ICON_TK_HIDDEN_MENUSUBUBAR		RES_ICON_MUBARHD 
#define ID_ICON_TK_SUMENUSUB2				RES_ICON_SUMUSUB2   
#define ID_ICON_TK_SUMENUSUB3				RES_ICON_SUMUSUB3   
#define ID_ICON_TK_SUMENUSUB4				RES_ICON_MUSUBSC   
#define ID_ICON_SUMENUSUB4_HDMI				RES_ICON_MUSUBSC_HDMI
#define ID_ICON_TK_SUMENUUBAR				RES_ICON_SUMUBAR 
#define ID_ICON_TK_SMU2BAR 					RES_ICON_SMU2BAR 
#define ID_ICON_TK_SUMENUSUBUBAR			RES_ICON_MUSUSBAR 
#define ID_ICON_SUMENUSUBUBAR_HDMI			RES_ICON_MUSUSBAR_HDMI
#define ID_ICON_TK_SUMENUSUB4SCROLL			RES_ICON_SUMUSUBS
#define ID_ICON_TK_CLEAR_SUMENUUBAR			RES_ICON_SUMUBARC   
#define ID_ICON_TK_CLEAR_SUMENUSUBUBAR		RES_ICON_SUMUSBCL   
#define ID_ICON_TK_CLEAR_SUMENUSISUBUBAR	RES_ICON_MSIBARCL
#define ID_ICON_TK_VOLUME_BAR_HL			RES_ICON_VOLBARHL          
#define ID_FONT_TK_TIME						RES_ICON_TIMEBQ 
#define ID_FONT_TIME_HDMI					RES_ICON_TIMEBQ_HDMI
#define ID_FONT_TK_ICON						RES_ICON_ICON32  
#define ID_FONT_TK_BIGICON					RES_ICON_ICON48
#define ID_FONT_DATSTAMP                    RES_FONT_DATSTAMP
#define ID_ICON_ZOOM						RES_ICON_ZOOM    
#define ID_ICON_FOCUS						RES_ICON_FOCUS 
#define ID_WARNFRAME						RES_ICON_WARN   
#define ID_WARNFRAME2						RES_ICON_WARNBG2 
#define ID_WARNICON							RES_ICON_WARNICON            
#define ID_DLGBOX_TXT_TITLE_ICON			RES_ICON_DLGTITLL   
#define ID_DLGBOX_NULL_TITLE_ICON			RES_ICON_DLGOPTTP   
#define ID_DLGBOX_OPTION_TXT_ICON			RES_ICON_DLGOPTCT   
#define ID_DLGBOX_OPTION_BOTTOM_ICON		RES_ICON_DLGOPTBT   
     
/*JPG*/
#define ID_JPG_RES_USB_MODE					RES_JPG_BLACK

/*SST*/
#define ID_STRTK_TBL_ENG			        RES_SST_ENG			  
#define ID_STRTK_TBL_TCH			        RES_SST_TCH			  
#define ID_STRTK_TBL_SCH			        RES_SST_SCH			  
#define ID_STRTK_TBL_BENG			        RES_SST_BENG
#define ID_STRTK_TBL_BTCH			        RES_SST_BTCH
#define ID_STRTK_TBL_BSCH			        RES_SST_BSCH
#define ID_STRTK_TBL_GERMAN			        RES_SST_GERMAN		
#define ID_STRTK_TBL_CZECH			        RES_SST_CZECH			
#define ID_STRTK_TBL_DUTCH			        RES_SST_DUTCH			
#define ID_STRTK_TBL_FRENCH			        RES_SST_FRENCH		
#define ID_STRTK_TBL_GREEK			        RES_SST_GREEK			
#define ID_STRTK_TBL_HUNGARIAN		        RES_SST_HUNGARIAN	
#define ID_STRTK_TBL_ITALIAN		        RES_SST_ITALIAN		
#define ID_STRTK_TBL_INDONESIAN		        RES_SST_INDONESIAN
#define ID_STRTK_TBL_MALAY			        RES_SST_MALAY			
#define ID_STRTK_TBL_POLISH			        RES_SST_POLISH		
#define ID_STRTK_TBL_PORTUGUESE		        RES_SST_PORTUGUESE
#define ID_STRTK_TBL_RUSSIAN		        RES_SST_RUSSIAN		
#define ID_STRTK_TBL_ROMANIAN		        RES_SST_ROMANIAN	
#define ID_STRTK_TBL_SPANISH		        RES_SST_SPANISH		
#define ID_STRTK_TBL_SWEDISH		        RES_SST_SWEDISH		
#define ID_STRTK_TBL_TURKISH		        RES_SST_TURKISH		
#define ID_STRTK_TBL_CROATION		        RES_SST_CROATION	
#define ID_STRTK_TBL_ARABIC			        RES_SST_ARABIC		
#define ID_STRTK_TBL_THAI			        RES_SST_THAI			
#define ID_STRTK_TBL_JAPAN			        RES_SST_JAPAN			
#define ID_STRTK_TBL_KOREAN			        RES_SST_KOREAN		
#endif /* _APP_OSDFONT_H_ */

