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
#ifndef APP_RES_DEF_H
#define APP_RES_DEF_H

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
extern unsigned char * appRootGet(char *input);
#define RootGet(x) appRootGet(x)

/*---------------------A: B:----------------------------------------*/
#define RO_RES_PATH				"A:\\RO_RES\\"
#define USER_RES_PATH           "B:\\USER_RES\\"
#define IQ_PATH                 RO_RES_PATH"IQ\\"
#if SPCA6350
#define RES_PLATFORM			RO_RES_PATH"6350\\"
#elif SPCA6330
#define RES_PLATFORM			RO_RES_PATH"6330\\"
#elif SPCA5330
#define RES_PLATFORM			RO_RES_PATH"5330\\"
#endif

/*---------------------A:\\RO_RES\\------------------------------*/
#define RES_RTC_CHECK_SIG	    RootGet(RO_RES_PATH"ENGSMALL.SFN")

/*lcd*/
#define RES_LCD_LCDTEST1	    (RO_RES_PATH"LCD\\LCDTEST1.JPG")

/*Facetrack*/
#define RES_FACETRACK		    RootGet(RO_RES_PATH"APP\\FACETRCK.BIN")

/*awb*/
#define RES_AWB_CTT             (IQ_PATH"AWBCTT.BIN")
#define RES_AWB_CFG             (IQ_PATH"AWBCFG.BIN")

#define RES_6350_BPVIEW			(IQ_PATH"BPVIEW.BIN")
#define RES_6350_BPSTILL		(IQ_PATH"BPSTILL.BIN")

/*USB*/
#define RES_USB_USBUVC          RootGet(RES_PLATFORM"USBUVC.BIN")
#define RES_USB_USBUVC_DUAL  RootGet(RES_PLATFORM"USBUVC_DUAL.BIN")
#define RES_USB_USBUVC_TS       RootGet(RO_RES_PATH"USBUVC_TS.BIN")
#define RES_USB_USBUVC_PB       RootGet(RO_RES_PATH"USBUVC_PB.BIN")
#define RES_USB_USBUVC_15       RootGet(RES_PLATFORM"USBUVC15.BIN")

/*---------------------------A-6330------------------------------*/
#define DIR6330                 RO_RES_PATH""
#define RES_6330_ADC            RootGet(DIR6330"BTNADC.BIN")
#define RES_6330_BPVIEW         RootGet(IQ_PATH"BPVIEW.BIN")
#define RES_6330_BPSTILL        RootGet(IQ_PATH"BPSTILL.BIN")
#define RES_5330_BPSTILL        RootGet(IQ_PATH"BPSTILL.BIN")

/*---------------------------A-Calib------------------------------*/
#define CALIB_DIR               RO_RES_PATH"CALIB\\"
#define FIF_COLD_DATA           RootGet(CALIB_DIR"COLD.BIN")
#define RES_CALIB_BATTERY_SET   RootGet(CALIB_DIR"BATTERY.BIN")
#define RES_CALIB_DATA          RootGet(CALIB_DIR"CALIB.BIN")
#define RES_CALIB_AF_DATA       RootGet(CALIB_DIR"CALIBAF.BIN")
#define RES_CALIB_AWB             RootGet(CALIB_DIR"AWBCALIB.BIN")

/*---------------------------A-Disp-------------------------------*/
#define DISP_DIR                RO_RES_PATH"DISP\\"
#define FIF_DISP_DATA           RootGet(RO_RES_PATH"DISP\\DISP.BIN")
#define RES_DISP_DISPGAMA       RootGet(DISP_DIR"DISPGAMA.BIN")
#define RES_DISP_DISPMAX        RootGet(DISP_DIR"DISPMAX.BIN")
#define RES_DISP_DISPGAM0       RootGet(DISP_DIR"DISPGAM0.BIN")
#define RES_DISP_DISPMAX0       RootGet(DISP_DIR"DISPMAX0.BIN")
#define RES_DISP_PALMAX         RootGet(DISP_DIR"PALMAX.BIN")

/*---------------------------A-FACETRACK---------------------------*/
#define FTRACK_DIR              RO_RES_PATH"FTRACK\\"

/*---------------------------A-Wav---------------------------------*/
#define WAV_DIR                 RO_RES_PATH"WAV\\"
#define FIF_AUDIO_DATA          RootGet(WAV_DIR"AUDIO.BIN")
#define RES_WAV_SHUTTER         RootGet(WAV_DIR"SHUTTER.WAV")
#define RES_WAV_BURST           RootGet(WAV_DIR"BURST.WAV")
#define RES_WAV_BEEP            RootGet(WAV_DIR"BEEP.WAV")
#define RES_WAV_WARNING         RootGet(WAV_DIR"WARNING.WAV")
#define RES_WAV_ALC_TXT         RootGet(WAV_DIR"ALC.TXT")
#define RES_WAV_POWER_ON1       (WAV_DIR"STARTUP1.WAV")
#define RES_WAV_POWER_ON2       (WAV_DIR"STARTUP2.WAV")
#define RES_WAV_POWER_ON3       (WAV_DIR"STARTUP3.WAV")
#define RES_WAV_SLIDE1          (WAV_DIR"SLIDE001.WAV")
#define RES_WAV_SLIDE2          (WAV_DIR"SLIDE002.WAV")
#define RES_WAV_SLIDE3          (WAV_DIR"SLIDE003.WAV")

/*---------------------------A-IQ-----------------------------------*/
#define IQ_DIR                  RO_RES_PATH"IQ\\"

/*IQ*/
#define RES_IQ_LSC              RootGet(IQ_DIR"LSC.BIN")
#define RES_IQ_WTBL             RootGet(IQ_DIR"WTBL.BIN")
#define RES_IQ_DIVTBL           RootGet(IQ_DIR"DIVTBL.BIN")
#define RES_IQ_HUECORR          RootGet(IQ_DIR"HUECORR.BIN")
#define RES_IQ_FACEBEAUTY       RootGet(IQ_DIR"FBHUE.BIN")

/*-------------------------A-UI-----------------------------------*/
/*FONT*/
#define UI_FONT_DIR             RO_RES_PATH"UI\\FONT\\"
#define RES_FONT_DATSTAMP_24    (UI_FONT_DIR"DATESTAMP_24.SFN")
#define RES_FONT_DATSTAMP       (UI_FONT_DIR"DATSTAMP.SFN")
#define RES_FONT_FONTENG	    (UI_FONT_DIR"ENGLISH_32.SFN")
#define RES_FONT_TCHFONT	    (UI_FONT_DIR"TCHINESE_32.SFN")
#define RES_FONT_SCHFONT	    (UI_FONT_DIR"SCHINESE_32.SFN")
#define RES_FONT_BENG	    	(UI_FONT_DIR"ENGLISH_42.SFN")
#define RES_FONT_BTCH	    	(UI_FONT_DIR"TCHINESE_42.SFN")
#define RES_FONT_BSCH	    	(UI_FONT_DIR"SCHINESE_42.SFN")
#define RES_FONT_ARAFONT		(UI_FONT_DIR"ARAFONT.SFN")
#define RES_FONT_FONTTAI		(UI_FONT_DIR"FONTTAI.SFN")
#define RES_FONT_JAPFONT		(UI_FONT_DIR"JAPFONT.SFN")
#define RES_FONT_KORFONT	    (UI_FONT_DIR"KORFONT.SFN")

/*ICON*/
#define UI_ICON_DIR             RO_RES_PATH"UI\\ICON\\"

#define ID_ICONFILE_MUSUSBAR		"UI\\ICON\\MUSUSBAR.SFN"//RootGet(UI_ICON_DIR"MUSUSBAR.SFN")
#define ID_ICONFILE_MUSUBSC		    "UI\\ICON\\MUSUBSC.SFN"//RootGet(UI_ICON_DIR"MUSUBSC.SFN")
#define ID_ICONFILE_MUBODY			"UI\\ICON\\MUBODY.SFN"//RootGet(UI_ICON_DIR"MUBODY.SFN")
#define ID_ICONFILE_MU2LYBAR		RootGet(UI_ICON_DIR"MU2LYBAR.SFN")
#define ID_ICONFILE_MUSBAR			RootGet(UI_ICON_DIR"MUSBAR.SFN")
#define ID_ICONFILE_MUDLGBAR		RootGet(UI_ICON_DIR"MUDLGBAR.SFN")
#define ID_ICONFILE_MUBAR			RootGet(UI_ICON_DIR"MUBAR.SFN")
#define ID_ICONFILE_MUDLG			RootGet(UI_ICON_DIR"MU2ND2.SFN")
#define ID_ICONFILE_MUSCENE		    RootGet(UI_ICON_DIR"MUSCENE.SFN")
#define ID_ICONFILE_BUTTON			RootGet(UI_ICON_DIR"BTNMENUOK.SFN")
#define ID_ICONFILE_MODE			RootGet(UI_ICON_DIR"MODEICON.SFN")
#define ID_ICONFILE_MENUTAB		    RootGet(UI_ICON_DIR"MENUTAB.SFN")

#define ID_ICONFILE_MU2LYBAR_HDMI	RootGet(UI_ICON_DIR"MU2LYBAR_HDMI.SFN")
#define ID_ICONFILE_MUSUSBAR_HDMI	RootGet(UI_ICON_DIR"MUSUSBAR_HDMI.SFN")
#define ID_ICONFILE_MUBAR_HDMI	RootGet(UI_ICON_DIR"MUBAR_HDMI.SFN")
#define ID_ICONFILE_MUSUBSC_HDMI	RootGet(UI_ICON_DIR"MUSUBSC_HDMI.SFN")
#define ID_ICONFILE_MUDLG_HDMI	RootGet(UI_ICON_DIR"MU2ND2_HDMI.SFN")
#define ID_ICONFILE_MUBODY_HDMI	RootGet(UI_ICON_DIR"MUBODY_HDMI.SFN")
#define ID_ICONFILE_BUTTON_HDMI	RootGet(UI_ICON_DIR"BTNMENUOK_HDMI.SFN")
#define ID_ICONFILE_MODE_HDMI		RootGet(UI_ICON_DIR"MODEICON_HDMI.SFN")
#define ID_ICONFILE_MENUTAB_HDMI	RootGet(UI_ICON_DIR"MENUTAB_HDMI.SFN")


#define RES_ICON_MUBODY			RootGet(UI_ICON_DIR"MUBODY.SFN")
#define RES_ICON_MUBODY_HDMI	RootGet(UI_ICON_DIR"MUBODY_HDMI.SFN")
#define RES_ICON_MUBAR          RootGet(UI_ICON_DIR"MUBAR.SFN")
#define RES_ICON_MUBAR_HDMI     RootGet(UI_ICON_DIR"MUBAR_HDMI.SFN")
#define RES_ICON_MUTAB          RootGet(UI_ICON_DIR"MENUTAB.SFN")
#define RES_ICON_MUTAB_HDMI     RootGet(UI_ICON_DIR"MENUTAB_HDMI.SFN")
#define RES_ICON_MUOK           RootGet(UI_ICON_DIR"BTNMENUOK.SFN")
#define RES_ICON_MUOK_HDMI      RootGet(UI_ICON_DIR"BTNMENUOK_HDMI.SFN")
#define RES_ICON_MUMODE         RootGet(UI_ICON_DIR"ICON_40.SFN")
#define RES_ICON_MUMODE_HDMI    RootGet(UI_ICON_DIR"MODEICON_HDMI.SFN")
#define RES_ICON_MUSUBSC        RootGet(UI_ICON_DIR"MUSUBSC.SFN")
#define RES_ICON_MUSUBSC_HDMI   RootGet(UI_ICON_DIR"MUSUBSC_HDMI.SFN")
#define RES_ICON_TIMEBQ         RootGet(UI_ICON_DIR"TIMEBQ.SFN")
#define RES_ICON_TIMEBQ_HDMI    RootGet(UI_ICON_DIR"TIMEBQ_HDMI.SFN")
#define RES_ICON_MU2ND2         RootGet(UI_ICON_DIR"MU2ND2.SFN")
#define RES_ICON_MU2ND2_HDMI    RootGet(UI_ICON_DIR"MU2ND2_HDMI.SFN")
#define RES_ICON_MU2ND3         RootGet(UI_ICON_DIR"MU2ND3.SFN")
#define RES_ICON_MU2ND3_HDMI    RootGet(UI_ICON_DIR"MU2ND3_HDMI.SFN")
#define RES_ICON_MUSUSBAR       RootGet(UI_ICON_DIR"MUSUSBAR.SFN")
#define RES_ICON_MUSUSBAR_HDMI  RootGet(UI_ICON_DIR"MUSUSBAR_HDMI.SFN")
#define RES_ICON_ICON32         RootGet(UI_ICON_DIR"ICON_32.SFN")
#define RES_ICON_ICON48         RootGet(UI_ICON_DIR"ICON_48.SFN")
#define RES_ICON_ICON56         RootGet(UI_ICON_DIR"ICON56.SFN")
#define RES_ICON_ICONTK         RootGet(UI_ICON_DIR"ICONTK.SFN")
#define RES_ICON_MUBARCL        RootGet(UI_ICON_DIR"MUBARCL.SFN")
#define RES_ICON_MUBODYHL       RootGet(UI_ICON_DIR"MUBODYHL.SFN")
#define RES_ICON_MUBARHD        RootGet(UI_ICON_DIR"MUBARHD.SFN")
#define RES_ICON_MUSBARCL       RootGet(UI_ICON_DIR"MUSBARCL.SFN")
#define RES_ICON_MUSBAR         RootGet(UI_ICON_DIR"MUSBAR.SFN")
#define RES_ICON_MUSUB3         RootGet(UI_ICON_DIR"MUSUB3.SFN")
#define RES_ICON_MUSUB2         RootGet(UI_ICON_DIR"MUSUB2.SFN")
#define RES_ICON_MUSUB4         RootGet(UI_ICON_DIR"MUSUB4.SFN")
#define RES_ICON_MUBODYHL       RootGet(UI_ICON_DIR"MUBODYHL.SFN")
#define RES_ICON_MUBARHD        RootGet(UI_ICON_DIR"MUBARHD.SFN")
#define RES_ICON_MUSIBAR        RootGet(UI_ICON_DIR"MUSIBAR.SFN")
#define RES_ICON_WARN           RootGet(UI_ICON_DIR"WARN.SFN")
#define RES_ICON_WARNM          RootGet(UI_ICON_DIR"WARNM.SFN")
#define RES_ICON_WARNICON       RootGet(UI_ICON_DIR"WARNICON.SFN")
#define RES_ICON_WARNBG2        RootGet(UI_ICON_DIR"WARNBG2.SFN")
#define RES_ICON_ZOOM           RootGet(UI_ICON_DIR"ZOOM.SFN")
#define RES_ICON_FOCUS          RootGet(UI_ICON_DIR"FOCUS.SFN")
#define RES_ICON_SEBODY         RootGet(UI_ICON_DIR"SEBODY.SFN")
#define RES_ICON_SUMUBAR        RootGet(UI_ICON_DIR"SUMUBAR.SFN")
#define RES_ICON_SUMUBARC       RootGet(UI_ICON_DIR"SUMUBARC.SFN")
#define RES_ICON_SUMUSBAR       RootGet(UI_ICON_DIR"SUMUSBAR.SFN")
#define RES_ICON_SUMUSUB2       RootGet(UI_ICON_DIR"SUMUSUB2.SFN")
#define RES_ICON_SUMUSUB3       RootGet(UI_ICON_DIR"SUMUSUB3.SFN")
#define RES_ICON_SUMUSUB4       RootGet(UI_ICON_DIR"SUMUSUB4.SFN")
#define RES_ICON_SUMUSUBS       RootGet(UI_ICON_DIR"SUMUSUBS.SFN")
#define RES_ICON_SUMUSBCL       RootGet(UI_ICON_DIR"SUMUSBCL.SFN")
#define RES_ICON_MSIBARCL       RootGet(UI_ICON_DIR"MSIBARCL.SFN")
#define RES_ICON_VOLBAR         RootGet(UI_ICON_DIR"VOLBAR.SFN")
#define RES_ICON_VOLBARHL       RootGet(UI_ICON_DIR"VOLBARHL.SFN")
#define RES_ICON_SMU2BAR        RootGet(UI_ICON_DIR"SMU2BAR.SFN")
#define RES_ICON_MU2ND4         RootGet(UI_ICON_DIR"MU2ND4.SFN")
#define RES_ICON_MUSLBAR        RootGet(UI_ICON_DIR"MUSLBAR.SFN")
#define RES_ICON_MUDT           RootGet(UI_ICON_DIR"MUDT.SFN")
#define RES_ICON_DLGTITLL       RootGet(UI_ICON_DIR"DLGTITLL.SFN")
#define RES_ICON_DLGOPTTP       RootGet(UI_ICON_DIR"DLGOPTTP.SFN")
#define RES_ICON_DLGOPTCT       RootGet(UI_ICON_DIR"DLGOPTCT.SFN")
#define RES_ICON_DLGOPTBT       RootGet(UI_ICON_DIR"DLGOPTBT.SFN")
#define RES_ICON_MUSCENE        RootGet(UI_ICON_DIR"MUSCENE.SFN")
#define RES_ICON_MU2NDBAR       RootGet(UI_ICON_DIR"MU2NDBAR.SFN")
#define RES_ICON_MU2CLBAR       RootGet(UI_ICON_DIR"MU2CLBAR.SFN")
#define RES_ICON_SSCR           RootGet(UI_ICON_DIR"SSCR.SFN")

/*JPG*/
#define UI_JPG_DIR              RO_RES_PATH"UI\\JPG\\"
#define RES_JPG_PB_CVR_BGHDMI	RootGet(UI_JPG_DIR"BGFACEHDMI.JPG")
#define RES_JPG_PB_CVR_BG		RootGet(UI_JPG_DIR"BGFACE.JPG")
#define RES_JPG_LOGO_SYS        RootGet(UI_JPG_DIR"STARTUP.JPG")
#define RES_JPG_LOGO_CUSTOM     RootGet(UI_JPG_DIR"STARTUP_CUSTOM.JPG")
#define RES_JPG_PB_BG           RootGet(UI_JPG_DIR"PBBG.JPG")
#define RES_JPG_PB_BG_FRAME		RootGet(UI_JPG_DIR"BGFACE.JPG")
#define RES_JPG_AUDIO_THUMB     RootGet(UI_JPG_DIR"AUDTHUMB.JPG")
#define RES_JPG_ERR_THUMB       RootGet(UI_JPG_DIR"ERRTHUMB.JPG")
#define RES_JPG_BLACK           RootGet(UI_JPG_DIR"SCRBLACK.JPG")
#define RES_JPG_AUDIO_BG 		RootGet(UI_JPG_DIR"AUDIOBK.JPG")
#define RES_JPG_GOODBYE 		RootGet(UI_JPG_DIR"GOODBYE.JPG")
#define RES_JPG_PHOTOFRAM1		(UI_JPG_DIR"FRAME001.JPG")
#define RES_JPG_PHOTOFRAM2		(UI_JPG_DIR"FRAME002.JPG")
#define RES_JPG_PHOTOFRAM3		(UI_JPG_DIR"FRAME003.JPG")
#define RES_JPG_PHOTOFRAM4		(UI_JPG_DIR"FRAME004.JPG")
#define RES_JPG_PHOTOFRAM5		(UI_JPG_DIR"FRAME005.JPG")
#define RES_JPG_ICONPHOTOFRAM1	(UI_JPG_DIR"FRAMEDP1.JPG")
#define RES_JPG_ICONPHOTOFRAM2	(UI_JPG_DIR"FRAMEDP2.JPG")
#define RES_JPG_ICONPHOTOFRAM3	(UI_JPG_DIR"FRAMEDP3.JPG")
#define RES_JPG_ICONPHOTOFRAM4	(UI_JPG_DIR"FRAMEDP4.JPG")
#define RES_JPG_ICONPHOTOFRAM5	(UI_JPG_DIR"FRAMEDP5.JPG")

/*SST*/
#define UI_SST_DIR              RO_RES_PATH"UI\\SST\\"
#define RES_SST_ENG			    (UI_SST_DIR"ENGLISH.SST")
#define RES_SST_TCH			    (UI_SST_DIR"TCHINESE.SST")
#define RES_SST_SCH			    (UI_SST_DIR"SCHINESE.SST")
#define RES_SST_BENG			(UI_SST_DIR"ENGLISH.SST")
#define RES_SST_BTCH		    (UI_SST_DIR"TCHINESE.SST")
#define RES_SST_BSCH		    (UI_SST_DIR"SCHINESE.SST")
#define RES_SST_GERMAN			(UI_SST_DIR"TGERSTR.SST")
#define RES_SST_CZECH			(UI_SST_DIR"TCZESTR.SST")
#define RES_SST_DUTCH			(UI_SST_DIR"TDUTSTR.SST")
#define RES_SST_FRENCH			(UI_SST_DIR"TFRESTR.SST")
#define RES_SST_GREEK			(UI_SST_DIR"TGREESTR.SST")
#define RES_SST_HUNGARIAN		(UI_SST_DIR"THUNSTR.SST")
#define RES_SST_ITALIAN		    (UI_SST_DIR"TITALSTR.SST")
#define RES_SST_INDONESIAN		(UI_SST_DIR"TINDSTR.SST")
#define RES_SST_MALAY			(UI_SST_DIR"TMALYSTR.SST")
#define RES_SST_POLISH			(UI_SST_DIR"TPOLSTR.SST")
#define RES_SST_PORTUGUESE		(UI_SST_DIR"TPORTSTR.SST")
#define RES_SST_RUSSIAN		    (UI_SST_DIR"TRUSSTR.SST")
#define RES_SST_ROMANIAN		(UI_SST_DIR"TROMSTR.SST")
#define RES_SST_SPANISH		    (UI_SST_DIR"TSPANSTR.SST")
#define RES_SST_SWEDISH		    (UI_SST_DIR"TSWEDSTR.SST")
#define RES_SST_TURKISH		    (UI_SST_DIR"TTURKSTR.SST")
#define RES_SST_CROATION		(UI_SST_DIR"TCROASTR.SST")
#define RES_SST_ARABIC			(UI_SST_DIR"TARASTR.SST")
#define RES_SST_THAI			(UI_SST_DIR"TTHAISTR.SST")
#define RES_SST_JAPAN			(UI_SST_DIR"TJAPSTR.SST")
#define RES_SST_KOREAN			(UI_SST_DIR"TKORSTR.SST")

/*---------------------B:----------------------------------------*/
#define RESB_PRM_PATH           USER_RES_PATH
#define FIFB_UI_DATA            RootGet(USER_RES_PATH"UIDATA.BIN")
#define RESB_USER_SET           RootGet(USER_RES_PATH"USER_SET.BIN")
#endif /*APP_RES_DEF_H*/

