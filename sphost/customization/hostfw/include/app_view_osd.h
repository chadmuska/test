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

#ifndef _APP_STILL_OSD_H_
#define _APP_STILL_OSD_H_

/**************************************************************************
in view

	mode  flash  Af  drive  	   stabilizer  size  quality  image-number

						error message/focus
															  A-mode-f-num
															T-mode-shutter
	battery		histogram								ev  meter  wb  iso
---------------------------------------------------------------------------
in aaa

	hand				focus
															 f-number-value
														     shutter-value
													    		  iso-value

 **************************************************************************/
#define VIEW_OSD_PRESENT_VIEWSCENE_MODE			0x00000001
#define VIEW_OSD_PRESENT_FLASH					0x00000002
#define VIEW_OSD_PRESENT_DRIVE					0x00000004
#define VIEW_OSD_PRESENT_AF						0x00000008
#define VIEW_OSD_PRESENT_STABILIZER				0x00000010
#define VIEW_OSD_PRESENT_SIZE					0x00000020
#define VIEW_OSD_PRESENT_QUALITY				0x00000040
#define VIEW_OSD_PRESENT_REMAIN_IMAGE_NUMBER	0x00000080
#define VIEW_OSD_PRESENT_ERROR					0x00000100
#define VIEW_OSD_PRESENT_FOCUS_WINDOW			0x00000200
#define VIEW_OSD_PRESENT_AMODE_FNUM				0x00000400
#define VIEW_OSD_PRESENT_SMODE_SHUTTER			0x00000800
#define VIEW_OSD_PRESENT_BATTERY				0x00001000
#define VIEW_OSD_PRESENT_HISTOGRAM				0x00002000
#define VIEW_OSD_PRESENT_EV						0x00004000
#define VIEW_OSD_PRESENT_METER					0x00008000
#define VIEW_OSD_PRESENT_WB						0x00010000
#define VIEW_OSD_PRESENT_ISO					0x00020000
#define VIEW_OSD_PRESENT_HAND					0x00040000
#define VIEW_OSD_PRESENT_REAL_FNUM				0x00080000
#define VIEW_OSD_PRESENT_REAL_SHUTTER			0x00100000
#define VIEW_OSD_PRESENT_REAL_ISO				0x00200000
#define VIEW_OSD_PRESENT_SELFTIME			    0x00400000
#define VIEW_OSD_PRESENT_GUID_LINE				0x00800000
#define VIEW_OSD_PRESENT_VIDEO_SIZE				0x01000000
#define VIEW_OSD_PRESENT_VIDEO_REMAIN_TIME		0x02000000

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
enum {
	OSD_OBJ_DESTROYED,
	OSD_OBJ_CREATED,
	OSD_OBJ_RUNNING,
};

enum{
	VIDEO_REC_STOP,
	VIDEO_REC_RECORDING,
	VIDEO_REC_PAUSE,
};

#if !SP5K_PVQV_DISABLE
enum{
	PVQV_SHOW_OFF,
    PVQV_SHOW_ON,
    PVQV_SHOW_DISK_ON,
	PVQV_SHOW_FAIL,
    PVQV_SHOW_DISABLE,
};
#endif

#define VIDEO_RECPAUSE_STR_PROMPT_X 		140
#define VIDEO_RECPAUSE_STR_PROMPT_Y 		210
#define VIDEO_CLEAN_RECICON_X 				190
#define VIDEO_CLEAN_RECICON_WIDTH_X 		40
#define VIDEO_CLEAN_RECICON_HEIGHT_Y 		30
#define FONT_H                              30
#define FONT_W                              14
#define VIEW_LEFT_X	                        3
#define VIEW_UP_Y	                        6
#define APPVIEW_ICON_SIZEX					36
#define APPVIEW_ICON_SIZEY					32
#define APPVIEW_ICON_Y_GAP					8

#define APPVIEW_RIGHT_X						314
#define APPVIEW_LEFT_X						6
#define APPVIEW_UP_Y						6
#define APPVIEW_BOTTOM_Y					204

#define APPVIEW_ICON_SIZE_W					36
#define APPVIEW_ICON_SIZE_H					32

#define APPVIEW_MODE_X						4//APPVIEW_LEFT_X
#define APPVIEW_MODE_Y						APPVIEW_UP_Y

#define APPVIEW_FD_X						2
#define APPVIEW_FD_Y						APPVIEW_UP_Y + APPVIEW_ICON_SIZEY +1

#define APPVIEW_PICSIZE_X					APPVIEW_STABILIZER_X+ APPVIEW_ICON_SIZEX+1
#define APPVIEW_PICSIZE_Y					APPVIEW_UP_Y

#if defined(PRJ_PDVR) //Isaac change 20161209
#define APPVIEW_FRAMERATE_X					APPVIEW_STABILIZER_X+ APPVIEW_ICON_SIZEX+1 - 90
#else
#define APPVIEW_FRAMERATE_X					APPVIEW_STABILIZER_X+ APPVIEW_ICON_SIZEX+1 - 70
#endif
#define APPVIEW_FRAMERATE_Y					APPVIEW_UP_Y


#define APPVIEW_FLASH_X						APPVIEW_LEFT_X+ APPVIEW_ICON_SIZEX
#define APPVIEW_FLASH_Y						APPVIEW_UP_Y

#define APPVIEW_WB_X						APPVIEW_ISO_X-APPVIEW_ICON_SIZEX
#define APPVIEW_WB_Y						APPVIEW_BOTTOM_Y

#define APPVIEW_FOCUSMODE_X					APPVIEW_FLASH_X+ APPVIEW_ICON_SIZEX
#define APPVIEW_FOCUSMODE_Y					APPVIEW_UP_Y

#define APPVIEW_SHAKE_X						APPVIEW_LEFT_X
#define APPVIEW_SHAKE_Y						100

#define APPVIEW_ISO_X						APPVIEW_RIGHT_X-APPVIEW_ICON_SIZEX+1
#define APPVIEW_ISO_Y						APPVIEW_BOTTOM_Y

#define APPVIEW_DRIVERMODE_X				APPVIEW_FOCUSMODE_X+ APPVIEW_ICON_SIZEX
#define APPVIEW_DRIVERMODE_Y				APPVIEW_UP_Y

#define APPVIEW_PICQTY_X					APPVIEW_PICSIZE_X + APPVIEW_ICON_SIZEX+1
#define APPVIEW_PICQTY_Y					VIEW_UP_Y

#define APPVIEW_REMAINNUM_RIGHT_X			APPVIEW_PICQTY_X + APPVIEW_CHR_WIDTH*6
#define APPVIEW_REMAINNUM_RIGHT_Y			VIEW_LEFT_X

#define APPVIEW_CHR_WIDTH					16
#define APPVIEW_CHR_HIEGHT					28

#define APPVIEW_REAL_ISO_ICON_X				310
#define APPVIEW_REAL_ISO_ICON_Y				210

#define APPVIEW_REAL_TV_X					APPVIEW_REAL_ISO_ICON_X
#define APPVIEW_REAL_TV_Y					APPVIEW_REAL_ISO_ICON_Y - FONT_H

#define APPVIEW_REAL_AV_X					APPVIEW_REAL_ISO_ICON_X
#define APPVIEW_REAL_AV_Y					APPVIEW_REAL_TV_Y-FONT_H

#define APPVIEW_STABILIZER_X				APPVIEW_DRIVERMODE_X+ APPVIEW_ICON_SIZEX
#define APPVIEW_STABILIZER_Y				VIEW_UP_Y

#define APPVIEW_SELFTIME_X					APPVIEW_DRIVERMODE_X+ APPVIEW_ICON_SIZEX
#define APPVIEW_SELFTIME_Y					APPVIEW_DRIVERMODE_Y

#define APPVIEW_METER_X						APPVIEW_WB_X - APPVIEW_ICON_SIZEX
#define APPVIEW_METER_Y						APPVIEW_BOTTOM_Y

#define APPVIEW_EV_X						APPVIEW_METER_X - APPVIEW_ICON_SIZEX-1
#define APPVIEW_EV_Y						APPVIEW_BOTTOM_Y

#define APPVIEW_BATTERY_X					2
#define APPVIEW_BATTERY_Y					APPVIEW_BOTTOM_Y

#define APPVIEW_SENSOR_X					40
#define APPVIEW_SENSOR_Y					APPVIEW_BOTTOM_Y

#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
#define APPVIEW_DUAL_X				        30
#define APPVIEW_DUAL_Y				        35
#endif

#define APPVIEW_FOCUS_WIN_X					0
#define APPVIEW_FOCUS_WIN_Y					0

#define APPVIEW_HISTOGRAM_X					38
#define APPVIEW_HISTOGRAM_Y					170
#define APPVIEW_HISTOGRAM_W					128
#define APPVIEW_HISTOGRAM_H					60
#define APPVIEW_HISTOGRAM_BG_X				APPVIEW_HISTOGRAM_X -2
#define APPVIEW_HISTOGRAM_BG_Y				APPVIEW_HISTOGRAM_Y -2
#define APPVIEW_HISTOGRAM_BG_W				APPVIEW_HISTOGRAM_W +4
#define APPVIEW_HISTOGRAM_BG_H				APPVIEW_HISTOGRAM_H +4

#define APPVIEW_FOCUS_X 					124
#define APPVIEW_FOCUS_Y 					93
#define APPVIEW_FOCUS_W 					72
#define APPVIEW_FOCUS_H						54

/* Audio Osd Position */
#define APPVIEW_AUDIO_UP					6
#define APPVIEW_AUDIO_LEFT  				6
#define APPVIEW_AUDIO_RIGHT  				306
#define APPVIEW_AUDIO_DOWN  				228
#define APPVIEW_AUDIO_FREETIME_X			(LCD_SIZE_X-180-LCD_SCREEN_OFFSET_X)
#define APPVIEW_AUDIO_FREETIME_Y			APPVIEW_AUDIO_UP
#define APPVIEW_AUDIO_FREETIME_OFFSET       99

#define LCD_SCREEN_OFFSET_X					15
#define LCD_SCREEN_OFFSET_Y					20
#define LCD_SIZE_X							320
#define LCD_SIZE_Y							240

#define VIEW_ZOOM_FRAME_X					136
#define VIEW_ZOOM_FRAME_Y					VIEW_UP_Y + APPVIEW_ICON_SIZEY+2

#define OPTICAL_DIGITAL_ZOOM_ICON_SIZEX		180
#define OPTICAL_DIGITAL_ZOOM_ICON_SIZEY		14

#define OPTICAL_ZOOM_BAR_X					VIEW_ZOOM_FRAME_X + 26
#define OPTICAL_ZOOM_BAR_Y					VIEW_ZOOM_FRAME_Y + 4

#define OPTICAL_ZOOM_BAR_SIZEX				88
#define OPTICAL_ZOOM_BAR_SIZEY				5

#define DIGITAL_ZOOM_BAR_X					VIEW_ZOOM_FRAME_X + 120
#define DIGITAL_ZOOM_BAR_Y					OPTICAL_ZOOM_BAR_Y
#define DIGITAL_ZOOM_BAR_SIZEX				39
#define DIGITAL_ZOOM_BAR_SIZEY				OPTICAL_ZOOM_BAR_SIZEY

/* audio status*/
#define APPVIEW_AUDIO_STRING_X				LCD_SIZE_X/2
#define APPVIEW_AUDIO_STRING_Y				70
#define APPVIEW_AUDIO_RECORDTIME_X			LCD_SIZE_X/2
#define APPVIEW_AUDIO_RECORDTIME_Y			LCD_SIZE_Y/3+20
#define APPVIEW_AUDIO_STRING_W				180
#define VOICE_MEMO_PROG_FRAME_CENT_X		LCD_SIZE_X / 2+ 3
#define VOICE_MEMO_PROG_FRAME_CENT_Y		LCD_SIZE_Y / 2
#define VOICE_MEMO_PROG_FRAME_SIZE_X		240
#define VOICE_MEMO_PROG_FRAME_SIZE_Y		150
#define VIDEO_REC_CONTROL_Y 216

/*s added by Leo Li @20091126*/
/*confirm dialog */
#define APPVIEW_BLINK_DLG_START_X           14
#define APPVIEW_BLINK_DLG_START_Y           142
#define APPVIEW_BLINK_DLG_1STITEM_X         120 /*Leo @20090827*/
#define APPVIEW_BLINK_DLG_1STITEM_Y         173
#define APPVIEW_BLINK_DLG_ITEM_GAP          31

/* wifi status */
#define APPVIEW_WIFI_STRING_X               130
#define APPVIEW_WIFI_STRING_Y               5

#if !SP5K_PVQV_DISABLE
#define APPVIEW_QV_BOUNDARY                 4
#define APPVIEW_QV_W                        (320-APPVIEW_QV_BOUNDARY)
#define APPVIEW_QV_H                        (APPVIEW_ICON_SIZEY+APPVIEW_QV_BOUNDARY)
#define APPVIEW_QV_RATION                   50
#endif

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
extern UINT32 ViewOsdPresentStatus;
/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appViewOsdValidAreaAutoFitSet(UINT8 bEnable);
UINT8 appViewOsdValidAreaAutoFitGet(void);
void appViewOsdValidAreaShow(UINT16 imgw, UINT16 imgh);
UINT32 appViewDrawPhotoFrame(UINT32 idx);
void appViewOsdDiskShow(void);
void appViewOsd_PicSizeDraw(UINT8 bShow, UINT8 nPicSize);
void appViewOsd_FlashModeDraw(UINT8 bShow, UINT8 nFlashMode);
void appViewOsd_WBModeDraw(UINT8 bShow, UINT8 nWBMode);
void appViewOsd_FocusModeDraw(UINT8 bShow, UINT8 nFocusMode);
void appViewOsd_ShakeWarnDraw(UINT8 bShow);
void appViewOsd_ISOModeDraw(UINT8 bShow,UINT8 enISO);
void appViewOsd_DriverModeDraw(UINT8 bShow, UINT8 nDriverMode);
void appViewOsd_ViewModeDraw(UINT8 bShow, UINT8 Pasm, UINT8 Scene);
void appViewOsd_MeteringDraw(UINT8 bShow, UINT8 idx);
void appViewOsd_ExposureBiaseDraw(UINT8 bShow, UINT8 idx);
void appViewOsd_BatteryDraw(UINT8 bShow, UINT8 idx);
void appViewOsd_HistogramDraw(BOOL bShow, UINT32 *pLuma);
void appViewOsd_RealFnumberDraw( UINT8 bShow, UINT8* f_num_str);
void appViewOsd_RealShutterSpeedDraw(UINT8 bShow, UINT8* str);
void appViewOsd_FocusWindowDraw(UINT8 bShow, UINT8 idx);
void appViewOsd_AmodeAvDraw(BOOL bShow, UINT8 Select, UINT8 Index, UINT32 ZoomPoint);
void appViewOsd_SmodeTvDraw(BOOL bShow, UINT8 Select, UINT8 Index);
void appViewOsd_GuideLineDraw(BOOL bShow);
void appViewOsdShow(UINT8 clean, UINT8 outInOsdFreeze, UINT8 showType);
void appViewOsd_PicSizeVideoDraw(BOOL bShow, UINT8 nPicSize);
void appViewOsd_PicQtyDraw(BOOL bShow, UINT8 nPicQty);
void appViewOsd_VideoQtyDraw(BOOL bShow, UINT8 nPicQty);
void appViewOsd_RemainNumDraw(BOOL bShow, unsigned char bHighLight, unsigned int nRemainNum);
void appViewOsd_TotalFreeTimeDraw(BOOL bShow, unsigned char bHighLight, UINT32 nSec);
void appViewOsd_RealIsoValueDraw(BOOL bShow, UINT8*  str);
void appViewOsd_FDModeDraw(BOOL bShow);
void appViewOsd_StabilizerModeDraw(BOOL bShow);
void appViewOsd_SelfTimeDraw(UINT8 bShow);
void appViewOsd_ZoomFrameDraw(UINT8 bShow);
void appViewOsd_DigZoomFrameDraw(UINT8 bShow);
void appViewOsd_OptZoomFrameDraw(	UINT8 bShow);
void appViewOsd_OptZoomPosDraw(UINT8 Position);
void appViewOsd_DigZoomPosDraw(UINT8 Position);
void appViewOsd_ASMChangeDraw();
void appViewOsd_WarmingMsgDraw(BOOL bShow, UINT32 viewError, BOOL forceSound);
void appViewOsd_HistogramCreat(void);
void appViewOsd_HistogramDestroy(void);
void appViewOsd_HistogramUpdateStop(void);
void appViewOsd_HistogramUpdateStart(void);
void appViewOsd_HistogramProcess(UINT32 param);
void appViewOsd_VideoTotalFreeTimeDraw(BOOL bShow, UINT32 nSec);
void appViewOsd_VideoRecIconDraw(BOOL bShow);
void appViewOsd_VideoRecControlDraw(UINT8 Recstatus );
void appStillOsd_MainFaceDraw(BOOL isAfOK);
UINT32 appStillOsd_UpdateFaceOsd(BOOL isClrOnly);
UINT32 appStillOsd_UpdateObjectOsd(BOOL isClrOnly);		/* Add by sc.liu */
void appViewOsd_ZoomBarProcess(UINT32 param);
void appViewOsd_ZoomBarProcessStart();
void appViewOsd_ZoomBarDestroy(void);
void appStillOsd_UpdateSmileOsd(UINT32* nface,UINT32* smile, BOOL bShow);
void appViewOsd_AllClean(void);
void appViewOsd_VideoPauseIconDraw(BOOL bShow);
void appViewOsd_DriveModeCountDownDraw(UINT8 Second);
void appVideoOsdElsTimeShow(void);
void appViewOsd_BlackOsdLayerDraw(void);
void appViewOsdDiskShow(void);
void appViewOsd_SensorDraw(UINT8 bShow,	UINT8 isResume);
void appViewOsd_ModeIconTest(void);
#if !SP5K_PVQV_DISABLE
UINT32 appViewOsd_PvQVDraw(BOOL en,UINT8 qvSec);
void appViewOsd_PvQV_DiskDraw(void);
void appViewOsd_PvQV_En(void);
#endif
#if SP5K_USB_UVC_SUPPORT || SP5K_SENSOR_DUAL_MODE
void appViewOsd_DualModeDraw(void);
#endif
#endif /* _APP_STILL_OSD_H_ */

