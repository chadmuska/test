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

#ifndef APP_PLAYBACK_OSD_H
#define APP_PLAYBACK_OSD_H

#include "app_playback_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

#define OSD_FONT_CONST_WIDTH	            18
#define OSD_FONT_CONST_HEIGHT	            24
#define LCD_SIZE_X							320
#define LCD_SIZE_Y							240
#define APPPB_ICON_SIZE_X					36
#define APPPB_ICON_SIZE_Y					32
#define PB_PLAY_MEDIA_STATUS_ICON_SIZE_X	48
#define PB_PLAY_MEDIA_STATUS_ICON_SIZE_Y	48

#define PB_AUXNOTE_ICON_HEIGHT	32
#define PB_AUXNOTE_ICON_WIDTH	36

#define APPPB_ICON_X_GAP						0
#define APPPB_ICON_Y_GAP						0
#define PLAYBACK_ICON_X							6
#define PLAYBACK_ICON_Y							6
#define PROTTECT_ICON_X							6
#define PROTTECT_ICON_Y							PLAYBACK_ICON_Y + APPPB_ICON_SIZE_Y + APPPB_ICON_Y_GAP + 7
#define DPOF_ICON_X								PROTTECT_ICON_X
#define DPOF_ICON_Y								PROTTECT_ICON_Y + APPPB_ICON_SIZE_Y + APPPB_ICON_Y_GAP
#define VOICE_MEMO_ICON_X						PROTTECT_ICON_X
#define VOICE_MEMO_ICON_Y						DPOF_ICON_Y + APPPB_ICON_SIZE_Y + APPPB_ICON_Y_GAP

#define EXPLOSURE_ICON_X						VOICE_MEMO_ICON_X
#define EXPLOSURE_ICON_Y						VOICE_MEMO_ICON_Y+APPPB_ICON_SIZE_Y + APPPB_ICON_Y_GAP
#define ISO_ICON_X								EXPLOSURE_ICON_X
#define ISO_ICON_Y								EXPLOSURE_ICON_Y+APPPB_ICON_SIZE_Y + APPPB_ICON_Y_GAP

#define CAPTURED_MODE_ICON_X					PROTTECT_ICON_X
#define CAPTURED_MODE_ICON_Y					LCD_SIZE_Y - APPPB_ICON_SIZE_Y -APPPB_ICON_Y_GAP
#define RESOLUTION_ICON_X						PROTTECT_ICON_X + APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP + 5
#define RESOLUTION_ICON_Y						CAPTURED_MODE_ICON_Y
#define WHITE_BALANCE_ICON_X					RESOLUTION_ICON_X + APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP
#define WHITE_BALANCE_ICON_Y					CAPTURED_MODE_ICON_Y
#define STROBE_MODE_ICON_X					    WHITE_BALANCE_ICON_X + 3*APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP
#define STROBE_MODE_ICON_Y					    CAPTURED_MODE_ICON_Y
#define METERING_MODE_ICON_X					STROBE_MODE_ICON_X + APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP
#define METERING_MODE_ICON_Y					CAPTURED_MODE_ICON_Y

#define INDICATE_ARROW_UP_ICON_X				WHITE_BALANCE_ICON_X+APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP
#define INDICATE_ARROW_UP_ICON_Y				CAPTURED_MODE_ICON_Y
#define INDICATE_PLAY_ICON_X					INDICATE_ARROW_UP_ICON_X+APPPB_ICON_SIZE_X + APPPB_ICON_X_GAP
#define INDICATE_PLAY_ICON_Y					INDICATE_ARROW_UP_ICON_Y
#define INDICATE_PAUSE_ICON_X					INDICATE_PLAY_ICON_X
#define INDICATE_PAUSE_ICON_Y					INDICATE_PLAY_ICON_Y

#define MENU_ICON_X								CAPTURED_MODE_ICON_X + 2
#define MENU_ICON_Y								LCD_SIZE_Y - APPPB_ICON_Y_GAP - 6
#define ALPHABET_FONT_SIZE_X					11
#define ALPHABET_FONT_SIZE_Y					32
#define PB_DATE_X								44
#define PB_DATE_Y								PLAYBACK_ICON_Y-3
#define PB_DATE_SIZE_X							(ALPHABET_FONT_SIZE_X * 10)
#define PB_TIME_X								164
#define PB_TIME_Y								PLAYBACK_ICON_Y
#define PB_TIME_SIZE_X							(ALPHABET_FONT_SIZE_X * 5)
#if SP5K_CDFS_OPT	 
#define PB_FILE_NAME_CDFS_X						PROTTECT_ICON_X 
#endif
#define PB_FILE_NAME_X							LCD_SIZE_X  - 14 * ALPHABET_FONT_SIZE_X
#define PB_FILE_NAME_Y							110
#define PB_FILE_NAME_SIZE_X						(ALPHABET_FONT_SIZE_X * 14)
#define PB_FILE_SIZE_X							PB_FILE_NAME_X
#define PB_FILE_SIZE_Y							PB_FILE_NAME_Y + ALPHABET_FONT_SIZE_Y
#define PB_FILE_SIZE_SIZE_X						PB_FILE_SIZE_X
#define PB_F_NUM_X								PB_FILE_NAME_X
#define PB_F_NUM_Y								PB_FILE_SIZE_Y + ALPHABET_FONT_SIZE_Y
#define PB_EXP_F_NUM_SIZE_X						PB_FILE_NAME_SIZE_X
#define PB_STARTING_SELECTSTR_X  				78
#define PB_STARTING_SELECTSTR_Y  				206
#define PB_STARTING_OKICON_X  					20
#define PB_STARTING_OKICON_Y  					214

#define PB_HISTOGRAM_X							PB_FILE_NAME_X
#define PB_HISTOGRAM_Y							PB_FILE_NAME_Y - PB_HISTOGRAM_SIZE_Y - 2
#define PB_HISTOGRAM_SIZE_X						128
#define PB_HISTOGRAM_SIZE_Y						60

#define PB_PLAY_MEDIA_STATUS_ICON_X  			136
#define PB_PLAY_MEDIA_STATUS_ICON_Y  			160
#define PB_VOICE_MEMO_PROG_FRAME_SIZE_X			240
#define PB_VOICE_MEMO_PROG_FRAME_SIZE_Y			150

#define PB_VOICE_MEMO_PROG_FRAME_CENT_X			LCD_SIZE_X / 2 + 3
#define PB_VOICE_MEMO_PROG_FRAME_CENT_Y			LCD_SIZE_Y / 2
#define PB_VOICE_MEMO_PROG_FRAME_LEFT_X			PB_VOICE_MEMO_PROG_FRAME_CENT_X  - (PB_VOICE_MEMO_PROG_FRAME_SIZE_X / 2)
#define PB_VOICE_MEMO_PROG_FRAME_UPPER_Y		PB_VOICE_MEMO_PROG_FRAME_CENT_Y - (PB_VOICE_MEMO_PROG_FRAME_SIZE_Y / 2)

#define PB_VOICE_MEMO_BAR_X						PB_VOICE_MEMO_PROG_FRAME_LEFT_X + 37
#define PB_VOICE_MEMO_BAR_Y						PB_VOICE_MEMO_PROG_FRAME_UPPER_Y + 107

#define PB_VOICE_MEMO_BAR_SIZE_X				165
#define PBHDMI_VOICE_MEMO_BAR_SIZE_X			85
#define PB_VOICE_MEMO_BAR_SIZE_Y				19

#define PB_VOICE_MEMO_TIME_X 					LCD_SIZE_X / 2 - (4 * ALPHABET_FONT_SIZE_X)
#define PB_VOICE_MEMO_TIME_Y					LCD_SIZE_Y / 2 - (ALPHABET_FONT_SIZE_Y / 2)

#define PB_VIDEO_TIME_X 						LCD_SIZE_X- (8 * ALPHABET_FONT_SIZE_X)
#define PB_VIDEO_TIME_Y 						6

#define PB_VIDEO_SEAMLESS_X 					PB_VIDEO_TIME_X - 30
#define PB_VIDEO_SEAMLESS_Y 					PB_VIDEO_TIME_Y

#define PB_AUDIO_STRING_X				        LCD_SIZE_X/2
#define PB_AUDIO_STRING_Y				        70
#define PB_AUDIO_STRING_W				        200

#define MPB_ICON_MARGINE_X			 3
#define MPB_ICON_MARGINE_Y			 3
#define MPB_PROTECT_ICON_X			 40- MPB_ICON_MARGINE_X
#define MPB_PROTECT_ICON_Y			 39- MPB_ICON_MARGINE_Y
#define MPB_DELETE_ICON_X 			 0 + MPB_ICON_MARGINE_X
#define MPB_DELETE_ICON_Y 			 0 + MPB_ICON_MARGINE_Y
#define MPB_VOICE_MEMO_ICON_X		 MPB_ICON_MARGINE_X
#define MPB_VOICE_MEMO_ICON_Y		 MPB_PROTECT_ICON_Y

#define FILE_LEFTRIGHT_LEFT_ARROW_X	    5
#define FILE_LEFTRIGHT_LEFT_ARROW_Y	    210
#define FILE_LEFTRIGHT_RIGHT_ARROW_X	298
#define FILE_LEFTRIGHT_RIGHT_CLEAR_X	306
#define FILE_LEFTRIGHT_ARROW_CLEAR_W	14
#define FILE_LEFTRIGHT_ARROW_CLEAR_H	20

#define MAX_ZOOM_PAN_W_H 67
#define CENT_DZOOM_PAN_X LCD_SIZE_X - 45
#define CENT_DZOOM_PAN_Y LCD_SIZE_Y - 45

#define DLGBOX_OPTION_NUM_MAX	4
#define DLGBOX_COLOR_TITLE_H	38
#define DLGBOX_NULL_TITLE_H		42
#define DLGBOX_DATA_CONT_H		39
#define HDMI_BOX_DATA_CONT_H	32
#define DLGBOX_DATA_LAST_H		39
#define DLGBOX_WIDTH			300
#define DLGBOX_HL_BAR_W			296
#define DLGBOX_HL_BAR_H			37

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/*playback file page ctrol */
typedef struct {
	UINT32 totalPage;
	UINT32 currPage;
	UINT32 currPageFirstFileIdx;
	UINT32 currPageLastFileIdx;
	UINT32 currFileIdx;
	BOOL pbSts;/*pbSts = TRUE: now is in pb file list page; pbSts = FASE: now is not in pb file list page; */
} pbFilePageCtrl_t;

typedef struct {
	/*-- Dialog Box config */
	UINT32 titleStrId;
	UINT32 numOptions;
	UINT32 optionStrID[DLGBOX_OPTION_NUM_MAX];
	UINT32 lastActIndex;
	UINT32 actIndex;

	/*-- Control data */
	UINT32 boxX;
	UINT32 boxY;
	UINT32 boxW;
	UINT32 boxH;
	SINT32 boxY_Offset;

	UINT32 markerIconW;
	UINT32 optionH;

	UINT32 titleX;
	UINT32 titleY;

	UINT32 optX[DLGBOX_OPTION_NUM_MAX];
	UINT32 optY[DLGBOX_OPTION_NUM_MAX];
	UINT32 dlgStatus;

} pbDlgBoxInfo_t;

enum MediaPlayStaus_S
{
    PAUSING	 =	 (unsigned char) 0x00,
    PLAYING	 =	 (unsigned char) 0x01,
    RWDING 	 =	 (unsigned char) 0x02,
    FWDING 	 =	 (unsigned char) 0x03,
    SLOW     =	 (unsigned char) 0x04,
};
enum MultiPlaybackInfo_S
{
	MPLAYBACK_PROTECTED,
	MPLAYBACK_UNPROTECTED,
	MPLAYBACK_NEWPROTECTED,
	MPLAYBACK_DELETE,
	MPLAYBACK_VOICE_MEMO,
	MPLAYBACK_VIDEO
};
enum ConfirmDlgType_S
{
	 DLGTYPE_YES_NO	 	=	 (unsigned char) 0x00,
	 DLGTYPE_ROTATE	 	=	 (unsigned char) 0x01,
	 DLGTYPE_MPB_LOCK 	=	 (unsigned char) 0x02,
	 DLGTYPE_MPB_DELE 	=	 (unsigned char) 0x03,
};
enum ConfirmDlgStatus_S
{
	 DLGSTATUS_IDLE	=	 (unsigned char) 0x00,
	 DLGSTATUS_SHOW	=	 (unsigned char) 0x01,
	 DLGSTATUS_SET	=	 (unsigned char) 0x02,
};

typedef enum
{
	PB_OSD_DRAW_NORMAL,
} pbOsdDrawControl_t;

void appPlayOsd_IconDraw(UINT8 bShow, SINT32 X, SINT32 Y, UINT32 IconID);
void appPlayOsd_DigitalZoomWinDraw(	SINT32 CropImageCentX,	SINT32 CropImageCentY,	UINT32 ImageWidth,	UINT32 ImageHeight,	UINT32 ZoomFactor,	UINT8 ImageAspRatio);
void appPbOsd_AuxNote_OKPause(BOOL bShow);
void appPbOsd_AuxNote_OKPlay(BOOL bShow);
void appPbOsd_AuxNote_OKStop(BOOL bShow);
void appPbOsd_AuxNote_OKRec(BOOL bShow);
void appPbOsd_AuxNote_OKSet(BOOL bShow);
void appPbOsd_AuxNote_OKSelect(BOOL bShow);
void appPbOsd_AuxNote_MenuCancel(BOOL bShow);
void appPbOsd_AuxNote_MenuBack(BOOL bShow);
void appPbOsd_AuxNote_OKDispPhoto(BOOL bShow);
void appPbOsd_AuxNote_TrashCanDelet(BOOL bShow,	BOOL bActive);
void appPbOsd_AuxNote_LockProtect(BOOL bShow,	BOOL bActive);
pbDlgBoxInfo_t *appPlayOsd_DlgBoxCBGet(void);
UINT32 appPlayOsd_DlgBoxStatusGet(void);
void appPlayOsd_DlgBoxSelectSet(UINT32 selIndex);
void appPlayOsd_DlgBoxStatusSet(UINT32 status);
UINT8 appPlayOsd_DlgBoxSelectGet(void);
void appPlayOsd_DlgBoxClear(void);
void appPlayOsd_DlgBoxTitleSet(UINT32 titleStrId);
void appPlayOsd_DlgBoxTitleUpdate(UINT32 titleStrId);
void appPlayOsd_DlgBoxActiveBarChange(void);
void appPlayOsd_DlgBoxScroll(BOOL isUp);
void appPlayOsd_DlgBoxConfig(UINT32 titleStrId, UINT32 numOptions, UINT32 *poptionStrID, UINT32 actIndex, SINT32 yOffset);
void appPlayOsd_DlgBoxShow(void);
void appPlayOsd_DlgMessageShow(UINT32 titleStrId,UINT32 dlgType);
UINT8 appPlayOsd_DlgMessageProcess(UINT32 msg);
void appPlayOsd_DlgBoxSetandShow(UINT32 DlgStartX,UINT32 DlgStartY,	UINT32 DlgFirstItemX,	UINT32 DlgFirstItemY,UINT32 DlgItemGap);
void appPlayOsd_DlgBoxScrollSet(BOOL isUp,UINT32 DlgStartX,UINT32 DlgStartY,UINT32 DlgFirstItemX,	UINT32 DlgFirstItemY,UINT32 DlgItemGap);
void appPlayOsd_DlgBoxActiveBarChangeSet(UINT32 DlgStartX,UINT32 DlgStartY,UINT32 DlgFirstItemX,UINT32 DlgFirstItemY,UINT32 DlgItemGap);
void appPlayOsd_DlgConfirmYesNoConfig(UINT32 strId, UINT32 actIdx);
void appPlayOsd_ModeIconDraw(BOOL bShow);
void appPlayOsd_FileNumDraw(UINT8 bShow, UINT32 currIdx, UINT32 total);
void appPlayOsd_MainInfoDraw(UINT32 osdDrawType);
void appPlayOsd_AudioBGDraw(BOOL bShow);
void playOsd_TextCenterPrint(UINT32 x, UINT32 y, UINT8 *pszText);
void appPlayOsd_PicExifInfoDraw(BOOL bShow, PpbImgExigInfo_t pCurImgExif,pbImgCb_t *pimgCb);
void appPlayOsd_OSDInfoDraw(pbMainCB_t *psPBImage, pbImgExifInfo_t *psPBCurImgExif,	pbImgCb_t *pimgCb);
void appPlayOsd_ProtectDraw(UINT8 bShow);
void appPLayOsd_AudioPlayDraw(BOOL bShow);
void appPLayOsd_AudioPauseResumeDraw(BOOL isPause);
void appPlayOsd_ProgressBarLastTimeReset(void);
void appPlayOsd_ProgressBarDraw(	UINT32 CurrentTime_msec, UINT32 TotalTime_msec);
void appPlayOsd_VideoTimeDraw(UINT8 bShow,UINT32 CurrentTime_msec);
void appPlayOsd_VideoCntrlDraw(	UINT8 bShow,UINT8 isPlay,UINT8 bShowRWD,UINT8 bShowFWD);
void appPlayOsd_MediaPlayStatusDraw(UINT8 bShow,UINT16 MediaPlayStatus);
void appPlayOsd_ShowFileTime(UINT8 bShow,UINT32 CurrentTime_msec);
void appPlayOsd_ThumbnailIconDraw(UINT8 bShow,UINT32 type,UINT32 X,UINT32 Y);
void appPLayOsd_LeftRightArrow(BOOL bShow);
void appPLayOsd_MemoRecOsdDraw(BOOL bShow);
void appPlayOsd_DateTimeDraw(UINT8 bShow, UINT32 DateTimeFormat, sp5kDcfTime_t *FileTime);
void appPlayOsd_VideoClrOsd(void);
void appPbShowBG(void);
void appPbFilePageCtrlParaInit(void);
void appPbFilePageCtrlParaSync(void);
void appPlayOsd_VideoInfoDraw(void);
void appPlayOsd_VideoFileIconDrow(unsigned char bShow);
void appPlayOsd_VideoFileNameInfoDraw(void);
#endif /* APP_PLAYBACK_OSD_H */
