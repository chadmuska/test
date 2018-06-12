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
#ifndef APP_VIDEO_UVC_API_H
#define APP_VIDEO_UVC_API_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "oss_usbhost.h"
#include "app_video.h"
#include "app_view_param_def.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define REDUCE_DRAM_BUF         0
#define PREFORMANCE_PROF        0
#define PROFID                  3
#define PLUG_LOG_DISABLE        0

#if SP5K_HW_DDR3_PARM_2G
#define UVC_RING_BUFS           12
#else
#define UVC_RING_BUFS           8 /*need use IPPP*/
#endif

#if REDUCE_DRAM_BUF
#define MAIN_PATH_W_SIZE        1888
#define MAIN_PATH_H_SIZE        1062
#endif

#define UVC_NO_WAIT_FLAG		1
#if UVC_NO_WAIT_FLAG
#define UVC_VIDEO_DELAY         0
#define UVC_RING_DELAY          0
#else
#define UVC_VIDEO_DELAY         (50+UVC_RING_BUFS)
#define UVC_RING_DELAY          (80)
#endif

#define UVC_PWR_RECORD_TMR      10 * 1000
#define CVR_HOR_LINE_COLOR		PAL_GREEN_1
#define CVR_SAFE_LINE_COLOR		PAL_YELLOW_1
#define CVR_WARN_LINE_COLOR		PAL_RED
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef struct HostCfg_s {
    UINT32 plug;
    UINT32 szidx;
    UINT32 width;
    UINT32 height;
    UINT32 frmintval;
} HostCfg_t;

typedef struct UvcCamCfg_s {
    UINT32 devno;
    UINT32 fmt;
    HostCfg_t host;
} UvcCamCfg_t;

 typedef struct UsbCtrl_s {
    void* handle;
    UINT8 flag;
}UsbCtrl_t;

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
enum {
    UVC_CAM_CFG_SET_DEV_OPEN,
    UVC_CAM_CFG_SET_DEV_CLOSE,
    UVC_CAM_CFG_SET_LIST_FMT_DEVNO,
    UVC_CAM_CFG_SET_CUR_DEVNO,
    UVC_CAM_CFG_SET_REQEST_BUF,
    UVC_CAM_CFG_SET_QUERY_BUF,
    UVC_CAM_CFG_SET_QUEUE_BUF,
    UVC_CAM_CFG_SET_DEQUEUE_BUF,
    UVC_CAM_CFG_SET_STREAM_ON,
    UVC_CAM_CFG_SET_STREAM_OFF,
    UVC_CAM_CFG_SET_CUR_CFG,
    UVC_CAM_CFG_SET_LIST_CFG,

    UVC_CAM_CFG_GET_CUR_DEVNO,
    UVC_CAM_CFG_GET_CUR_CFG,

    UVC_CAM_CFG_MAX
};

enum {
    UVC_HOST_CFG_PLUG_OUT,
    UVC_HOST_CFG_PLUG_IN,
    UVC_HOST_CFG_PLUG_COMPLETE,
    UVC_HOST_CFG_PLUG_FAIL,
    UVC_HOST_CFG_SIZE,
    UVC_HOST_CFG_FRM_INTVAL,
    UVC_HOST_CFG_SZ_IDX,
    UVC_HOST_CFG_MAX
};

enum {
	UVC_PLUG_OUT = 0,
	UVC_PLUG_IN,
	UVC_PLUG_COMPLETE,
    UVC_PLUG_FAIL = 0xff,
	UVC_PLUG_MAX = UVC_PLUG_FAIL,
};

enum {
	CAR_REVERSE_DIS     = 0x00,
	CAR_REVERSE_EN      = 0x01,
    CAR_REVERSE_DEF_CTRL= 0xff,
};

enum {
	CAR_REVERSE_LINE    = 0,
};

enum {
	FLIP_DIS            = 0x00,
	FLIP_EN             = 0x02,
};

enum {
	DISP_CB_MAIN_PATH   = 0x01,
	DISP_CB_2ND_TO_MAIN = 0x02,
};

enum {
	PWR_ON_REC_DIS      = 0x00,
	PWR_ON_REC_EN       = 0x01,
};

enum {
	EVENT_TRIG_DIS      = 0x00,
	EVENT_TRIG_EN       = 0x01,
	EVENT_TRIG_MAX      = 0x02,
	EVENT_TRIG_INIT     = EVENT_TRIG_MAX+1,
	EVENT_TRIG_PROC     = EVENT_TRIG_INIT+1,
};

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
/*app_video_uvc.c*/
void    appUvcCamInit(void);
void    appUvcCamVpvEnter(void);
void    appUvcModeSet(void);
UINT32  appUvcRingBufDone(void);

void    appUvcCapModeSet(void);
UINT8   appUvcCapModeStsGet(void);
UINT8   appUvcFlipModeGet(void);
void    appUvcFlipModeSet(void);
UINT8   appUvcCarModeGet(void);
void    appUvcCarModeSet(UINT8 en);
UINT32  appUvcPwrOnRecGet(void);
void    appUvcPwrOnRecSet(UINT8 sel);
UINT32  appUvcEventSet(UINT8 sel);

UINT32  appUvcImeCbUnReg(void);
UINT32  appUvcImeCbReg(void);

/*app_video_uvc_setting.c*/
UINT32  appUvcEnumExtensionUnit(void);
void    appUvcCamCfgInit(void);
void    appUvcModeCfgSet(UINT8 en, UINT8 pvsrc); /*? check appUvcModeSet*/
UvcCamCfg_t* appUvcCamParmGet(void);
SINT32  appUvcHostCfgSet(UINT32 sel,	...);
SINT32  appUvcHostCfgGet(UINT32 sel,	...);
#endif  /* APP_VIDEO_UVC_API_H */

