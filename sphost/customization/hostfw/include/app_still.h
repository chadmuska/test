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
#ifndef _APP_STILL_H_
#define _APP_STILL_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "app_user_setting.h"
#include "app_aaa.h"
#include "app_btn_def.h"
#include "app_disk.h"
#include "api/sp5k_gfx_api.h"
#include "app_afalg_api.h"
#include "app_aaa.h"

#define S2STEST         0
#define S2SNOAF         1
#define S2S_IMPROVE  1
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

enum {
	OSD_VIEW_SIMPLE,
	OSD_VIEW_VERBOSE,
	OSD_VIEW_NONE,
	OSD_VIEW_GRID,
	OSD_VIEW_CLEAN_ALL,
	OSD_VIEW_S_ONE_HOLD,
	OSD_VIEW_MAX
};
#define OSD_VIEW_DEFAULT	OSD_VIEW_SIMPLE

enum {
	RAW_CB_SHUTTER_NORAML,
	RAW_CB_SHUTTER_LONG,
	RAW_CB_SHUTTER_MAX
};

typedef enum {
	PASM_MMODE_SEL_TV,
	PASM_MMODE_SEL_AV,
	PASM_MMODE_SEL_MAX
} tvAvSel_e;

typedef enum
{
	FOCUS_STATUS_VIEW,
	FOCUS_STATUS_OK,
	FOCUS_STATUS_NG,
	FOCUS_STATUS_MAX,
} focusStatus_e;

typedef enum {
	VIEW_ERR_NULL,
	VIEW_ERR_NO_ERROR,
	VIEW_ERR_CARD_ERROR,
	VIEW_ERR_CARD_LOCKED,
	VIEW_ERR_MEMORY_FULL,
	VIEW_ERR_TO_POWER_OFF,
	VIEW_ERR_INSUFFICIENT_DISK_SPACE,
	VIEW_ERR_DISK_SPEED_TOO_SLOW,
	VIEW_ERR_MAX
} viewErrorCode_t;

typedef enum {
	VIEW_FT_STATUS_DESTROY,
	VIEW_FT_STATUS_PAUSE,
	VIEW_FT_STATUS_RESETDATA_PAUSE,
	VIEW_FT_STATUS_RUN,
	VIEW_FT_STATUS_MAX,
} viewFtStatus_t;

#if SP5K_USB_UVC_SUPPORT
enum {
	DUALSEN_SINGLE_MAIN = 0,
    DUALSEN_PIP_MAIN,
	DUALSEN_PIP_SEC,
	DUALSEN_SINGLE_SEC,
	DUAL_SEN_MAX,
};
#elif SP5K_SENSOR_DUAL_MODE
typedef enum {
	DUALSEN_SINGLE_MAIN = 0,
	DUALSEN_SINGLE_SEC,
	DUALSEN_PIP_MAIN,
	DUALSEN_PIP_SEC,
	DUAL_SEN_MAX,
}dualModeStatus_e;
#endif
/*define still preview size*/
#define CFG_WIDTTH_QVGA         320
#define CFG_HEIGHT_QVGA         240
#define CFG_WIDTTH_VGA          640
#define CFG_HEIGHT_VGA          480
#define CFG_WIDTH_HD            1280
#define CFG_HEIGHT_HD           720

//define driver mode setting
#define DEF_COMPRESSION_RATIO           800     /* compression ratio is 1/8 */
#define BURST_COMPRESSION_RATIO         1200

#if API_TEST_INIT
#define DEF_RAW_BUFFER                  1
#define DEF_YUV_BUFFER                  2
#else
#if (SP5K_SENSOR_SONYIMX283)
/*To avoid the buffer memory allocation Assert*/
#define DEF_RAW_BUFFER                  1
#define DEF_YUV_BUFFER                  2
#else
#define DEF_RAW_BUFFER                  2
#define DEF_YUV_BUFFER                  4
#endif
#endif
#define DEF_VLC_BUFFER                  2
#define DEF_INTERVAL                    200

#define AEB_BURST_LENGTH                3
#define AEB_BURST_INTRVAL               DEF_INTERVAL
#define AEB_RAW_BUFFER                  DEF_RAW_BUFFER
#define AEB_YUV_BUFFER                  DEF_YUV_BUFFER
#define AEB_VLC_BUFFER                  DEF_VLC_BUFFER

#define MFNR_BURST_LENGTH               4
#define MFNR_BURST_INTRVAL              DEF_INTERVAL
#define MFNR_RAW_BUFFER                 MFNR_BURST_LENGTH
#define MFNR_VLC_BUFFER                 DEF_VLC_BUFFER

#define HDR_BURST_LENGTH                3
#define HDR_BURST_INTRVAL               DEF_INTERVAL
#define HDR_RAW_BUFFER                  HDR_BURST_LENGTH
#define HDR_VLC_BUFFER                  DEF_VLC_BUFFER
#define HDR_BURST_EV_ADJUST             24

#define BURST_H_LENGTH                  3
#define BURST_H_INTRVAL                 300 //1s 3.3frame
#define BURST_L_UHS_INTRVAL             100//1s 10frame
#define BURST_L_INTRVAL                 300//1s 3.3frame
#define BURST_L_LENGTH                  5

#define BURST_H_RAW_BUFFER              9
#define BURST_H_YUV_BUFFER              1/*2*/
#define BURST_H_VLC_BUFFER              1

#define BURST_L_RAW_BUFFER              2
#define BURST_L_YUV_BUFFER              1/*2*/
#define BURST_L_VLC_BUFFER              2

#define SR_RAW_BUFFER                   DEF_RAW_BUFFER
#define SR_YUV_BUFFER                   (DEF_YUV_BUFFER+1)
#define SR_VLC_BUFFER                   DEF_VLC_BUFFER
#define SR_EDGE_ENHANCE                 512

#define ANRL_YUV_BUFFER                 2
#define ANR_YUV_BUFFER                  3

#define VIEW_SET_STATE_START		    (1<<1)
#define VIEW_SET_DISK				    (1<<2)
#define VIEW_SET_DATE_STAMP			    (1<<3)
#define VIEW_SET_ALL				    0xffffffff

#define CAN_I_START_SNAP	        0
#define CAN_I_LEAVE_STILL	        1
#define STILL_STATE_YES             0
#define STILL_STATE_NO 	            1
#define STILL_STATE_FLASH_CHARGIN 	2
#define STILL_STATE_MEMORY_FULL		3

#define IS_PASM_PMODE (gStillCB.cap.CurViewMode == PASM_PROGRAM)
#define IS_VIDEO_MODE (gStillCB.cap.CurViewMode == VIDEO_MODE)
#define IS_VIEW_MODE_CHANGE (gStillCB.cap.CurViewMode != gStillCB.lastcap.CurViewMode)


#define IS_DRIVE_DOUBLE (0)

#define PF_PATH		"A:\\RO_RES\\UI\\JPG\\"
#define FIELD_QV_ENABLE   	0
#define CAP_AUTO_ENABLE		1

/*#define AF_CURVE_SHOW*/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct appStillCB_s {
	void *viewParam;
	appCaptureSetting_t cap;
	appCaptureSetting_t lastcap;
	appAeStatus_t ae;
	appAwbStatus_t awb;
	UINT32 diskClusterSize;		/* In KB*/
	UINT32 estFileSize;			/* estimated file size	*/
	UINT32 estJobFilesSize;		/* estimated job files size, like AEB (file*3)*/
	UINT32 remainImg;			/* job files*/
	UINT32 s1press;				/* to record the s1 status*/
	UINT32 s2press;				/* to record the s2 status*/
	UINT32 s1AaaTicket;			/* record the s1 press message in Preview and Capture st.*/
	UINT32 s2SnapTicket;		/* record the s2 press message in Capture.*/
	UINT32 tvAvSel;				/* only for PASM_MMODE, Tv(shutter speed) and Av(Aperture value)*/
	UINT32 flashCharging;		/* to blink the led.*/
	UINT32 lensZoomSeg;			/* 0~6 (wide->tele)*/
	UINT32 amodeApertureIdx;	/* AMODE f number*/
	UINT32 smodeShutterIdx;	    /* SMODE shutter speed*/
	UINT32 needFlash;
	UINT32 focusStatus;
	UINT32 needDarkFrameSub;	/* This will enable while we have long shutter speed.*/
	UINT32 isDiskReady;
	UINT32 ftStatus;
	UINT32 isLensReady;
	UINT32 isXDenoiseOn;
	UINT32 curPhotoFrameIdx;
	UINT32 diskSpaceInImgId;
	UINT32 flashChargeMonitorTimer;	/* flash's power will decrease with time, we need to monitor it.*/
	UINT32 flashMode;				/* this is a copy of pViewParam->flash, except burst mode.*/
	UINT32 awbSpeed;
	UINT32 isReady2View;
	UINT32 err;
	UINT32 otStatus;
	UINT32 btnum;
	UINT8 osd;					/* osd disply mode*/
} appStillCB_t;

typedef struct {
	UINT16 rgain;
	UINT16 grgain;
	UINT16 bgain;
	UINT16 gbgain;
} appWbGain_t;


typedef struct photoframeFile_s{
	unsigned char* pf_big;
	unsigned char* pf_small;
}photoframeFile_t;
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
#define FLOW_EXTRCT_ST_PAR(x) 	(x>=STATE_PARAM_FREE_CARRY? (x&0x000000FF) : x)
#define FLOW_EXTRCT_CTRL_PAR(x) (x&0x00000F00)
#define TIME_LAPSE_H264_VIDEO_FILE		0x01

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/
extern appStillCB_t gStillCB;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appViewInit(void);
void appStill_ViewMenuSelFunctionCB(UINT8 SelPara,PappMenuInfo_t CurTab,PappSubMenuInfo_t CurItem);
UINT32 appStillEstFileSize(UINT32 imgSizeIdx, UINT32 qualityIdx, UINT32 clustersize);
UINT32 appStillWaitJobFinish(void);
void appStillPvStateInit(UINT32 preState);
void appBtnEnableDelay(void);

/* state flow control*/
void appStillFlow_SetDriveFlow(UINT32 driveMode, UINT32 viewMode, UINT32 scene);
void appStillFlow_GetNextState(UINT32* pState, UINT32* pParam);

/* Still Setting funtion */
UINT32 appStill_GetViewParam(UINT8 view_idx, UINT8 scene_idx);
void appStill_SetPasmSceneParam(UINT8 view_idx, UINT8 scene_idx);
void appStillSettingInit(UINT32 flg);
void appStill_SetResolution(UINT8 idx);
void appStill_SetQuality(UINT8 idx);
void appStill_SetMetering(UINT8 idx);
void appStill_SetWB(UINT8 idx);
void appStill_SetISO(UINT8 idx);
void appStill_SetExposure(UINT8 idx);
void appStill_SetDriveMode(UINT8 idx);
void appStill_SetSharpness(UINT8 idx);
void appStill_SetHighlight(UINT8 idx);
void appStill_SetDist(UINT8 idx);
void appStill_SetStabilizer(UINT8 idx);
void appStill_SetDZoom(UINT8 idx);
void appStill_SetFlash(UINT8 idx);
void appStill_SetShutterSpeed(UINT8 idx);
void appStill_SetApertureValue(UINT8 idx);
void appStill_SetQV(UINT32 qv);
void appStill_SetConfirmTimeOut(UINT32 time);
void appStill_SetCaptureConfirmStore(UINT32 storeflag);
void appStill_CfgCaptureConfirmStore(UINT32 enable);
void appStill_SetExpFlickerMode(UINT8 idx);
void appStill_ChangeFlash(void);
void appStill_ChangeAf(void);
void appStill_ChangeShutterSpeed(UINT32 updownkey);
void appStill_ChangeApertureValue(UINT32 updownkey);
void appStill_ResetAllParam(void);
void appStill_GetCurSize(UINT32* w, UINT32* h);
void appStill_GetCurSizeIcon(UINT32* icon);
void appStill_SetSp5kStillSize(UINT8 idx);
void appStill_SetDateStamp(UINT32 stamp_in,UINT32 dateformat_in);
UINT32 appStill_CustumStamp(UINT32 addr, UINT32 qvAddr, void* size);

UINT32 appStillCapFlashStatusGet(void);
UINT32 appStillCapExpTimeGet(void);
UINT32 appStillCapShutCloseTimeGet(void);
UINT32 appStillRawCbExecute(UINT32 addr, UINT32 rawfmt, sp5kYuvCbImgSize_t *  pImgSize);
void appStillRawCbSet(void);
void appStillYuvCbSet(BOOL isEnable);
#if !SP5K_PVQV_DISABLE
void appStillQVCbSet(BOOL isEnable);
UINT8 appViewOsd_PvQVInfoGet(sp5kGfxPicture_t** pic);
void appViewOsd_PvQVInfoFree(void);
#endif
void appStillPreviewCbSet(BOOL isEnable);
void appStillAFBktCbSet(BOOL isEnable);

/* Still IQ function */
void appAAAOff(void);
void appAaaAfWinSet(UINT32* isFaceAF);
UINT32 appStillCwbGet(appWbGain_t* pWb);
#ifdef AF_CURVE_SHOW
void appAfValueCurveDraw(UINT8 focusStatus);
#endif
/* OSD function (pleae refere to app_still_osd.h) */

/* Background functions */
void appStillBackground(UINT32 msg,	UINT32 param);
void appStillBackgroundInit(UINT32 cluSzInByte,	UINT32 dskFree,	UINT32 estFileSize);
void appStillBackgroundSet(UINT32 estFileSize);
UINT32 appStillBackgroundGetFree(UINT32* dskFree);
UINT32 appStillRemainNumGet(void);
UINT32 _StillCheckDiskReady(void);

void appStillUtil_CaptureSet(void);
void appStillUtil_CalibCaptureSet(BOOL enable, BOOL saveRaw);
void appStillUtil_CaptureWithFaceCheck(void);
/* Periodical Capture */
void appPeriodicalCaptureS2Check(void);
BOOL appPeriodicalCapturePowerOffMsgSend(void);
void appPeriodicalCaptureTimerProc(UINT32 param);
void appPeriodicalCaptureTimerCheck(UINT8 videoFormat);
void appPeriodicalCaptureTimerSet(void);

/* Dual Capture Function */
UINT32 appGetYUVSaveStatus(void);
void appSetYUVSaveStatus(UINT32 val);
void appYUVCapture(UINT32 path);
UINT8 appDualPipModeGet(void);

#if SP5K_SENSOR_DUAL_MODE
void appStillDualModePIP(dualModeStatus_e DualMode);
#elif SP5K_USB_UVC_SUPPORT
void appUvcPipMode(void);
void appDualPipModeSet(UINT8 val);
#endif
UINT32 appView_ItemDecrease(UINT32  total,UINT32 curIndex);
UINT32 appView_ItemIncrease(UINT32  total,UINT32 curIndex);
#endif  /* __APP_VIEW_API_H__ */

