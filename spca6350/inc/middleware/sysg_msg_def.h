/**************************************************************************
 *
 *       Copyright (c) 2005-2013 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/
/**
 * @file sysg_msg_def.h
 *
 * @author Matt Wang
 */
#ifndef _SP5K_MSG_DEF_H_
#define _SP5K_MSG_DEF_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"

/**************************************************************************
 *                          C O N S T A N T S
 **************************************************************************/
/* This must be same as the messages for every module, it is in temporal before updating source tree. */
typedef enum _sp5kHostMsgHead_t {
	SP5K_MSG_HEAD_HOST                       = 0x0010, /* Host */
	SP5K_MSG_HEAD_BUTTON                     = 0x0100, /* button driver */
	SP5K_MSG_HEAD_DISK                       = 0x0200  /* disk driver */
} sp5kHostMsgHead_t;

typedef enum _sp5kHostMsgList_e {
	/* host init */
	SP5K_MSG_HOST_TASK_INIT                  = SP5K_MSG_HEAD_HOST,
	SP5K_MSG_HOST_TASK_PRIVATE               ,
	SP5K_MSG_UART_READ_READY                 ,
	SP5K_MSG_UART_READ_TIMEOUT               ,

	/* power off */
	SP5K_MSG_POWER_OFF                       = SP5K_MSG_HEAD_HOST + 0x10,
	SP5K_MSG_POWER_OFF_KEY_RELEASE           ,

	/* button driver */
	SP5K_MSG_GPIO_BUTTON_PRESS               = SP5K_MSG_HEAD_BUTTON,
	SP5K_MSG_GPIO_BUTTON_REPEAT              ,
	SP5K_MSG_GPIO_BUTTON_RELEASE             ,
	SP5K_MSG_ADC_BUTTON_PRESS                ,
	SP5K_MSG_ADC_BUTTON_REPEAT               ,
	SP5K_MSG_ADC_BUTTON_RELEASE              ,
	SP5K_MSG_MATRIX_GPIO_BUTTON_PRESS        ,
	SP5K_MSG_MATRIX_GPIO_BUTTON_REPEAT       ,
	SP5K_MSG_MATRIX_GPIO_BUTTON_RELEASE      ,

	/* counter */
	SP5K_MSG_CNTR_DONE                       = 0x63510001,
	SP5K_MSG_RTC_ALARM_TIME_IS_UP            = 0x63510002,

	/* disk driver */
	SP5K_MSG_DISK_REMOVAL                    = SP5K_MSG_HEAD_DISK,
	SP5K_MSG_DISK_INSERT                     ,
	SP5K_MSG_DISK_MOUNT_START                ,
	SP5K_MSG_DISK_MOUNT_COMPLETE             ,
	SP5K_MSG_DISK_MOUNT_FAIL                 ,
	SP5K_MSG_DISK_ERROR                      ,

	/* disp */
	SP5K_MSG_HDMI_PLUG                       = 0x60510101,
	SP5K_MSG_HDMI_UNPLUG                     = 0x60510102,

	SP5K_MSG_CEC_PHYSICAL_ADDR_DISCOVER_DONE = 0x60510201,
	SP5K_MSG_CEC_PHYSICAL_ADDR_DISCOVER_FAIL = 0x60510202,

	SP5K_MSG_DISP_POWER_ON_READY             = 0x60510301,
	SP5K_MSG_DISP_POWER_OFF_READY            = 0x60510302,

	/* sensor messages */
	SP5K_MSG_AE_READY                        = 0x54510101, /*param:frameid*/
	SP5K_MSG_AWB_READY                       = 0x54510102, /*param:frameid*/
	SP5K_MSG_AEAWB_INVALID                   = 0x54510103, /*param:frameid*/
	SP5K_MSG_PREFLASH_READY                  = 0x54510104, /*param:frameid*/
	SP5K_MSG_PREFLASH_INVALID                = 0x54510105, /*param:frameid*/
	SP5K_MSG_PREFLASH_TIMEOUT                = 0x54510106, /*param:frameid*/
	SP5K_MSG_REDEYE_FLASH_FINISHED           = 0x54510107,
	SP5K_MSG_AF_READY                        = 0x54510108, /*param:frameid*/
	SP5K_MSG_HIS_ALL_READY                   = 0x54510109, /*only send at 1st time*/

	/* mode switch */
	SP5K_MSG_MODE_READY                      = 0x58510000,
	SP5K_MSG_DISP_SWITCH_READY               = 0x58510001,
	SP5K_MSG_SENSOR_SWITCH_READY             = 0x58510002,
	SP5K_MSG_AUTO_BANDING_RESULT             = 0x58510003,

	/* host */
	SP5K_MSG_TIMER                           = 0x51510000,

	/* still playback */
	SP5K_MSG_STILLPLAY_DONE                  = 0x5B510401,
	SP5K_MSG_MULTIPLAY_DONE                  = 0x5B510402,
	SP5K_MSG_MULTIPLAY_SHIFT                 = 0x5B510403,
	SP5K_MSG_IMGEDIT_DONE                    = 0x5B510404,
	SP5K_MSG_RESPLAY_DONE                    = 0x5B510405,
	SP5K_MSG_MULTIPLAY_IMG_RDY               = 0x5B510406,
	SP5K_MSG_STILLPLAY_ZOOM_DONE             = 0x5B510407,

	/* still capture */
	SP5K_MSG_STILL_VIEW_SETTING_NOTIFY       = 0x53510517,
	SP5K_MSG_STILL_QV_TIMEOUT                = 0x53510516,
	SP5K_MSG_STILL_STORE_COMPLETE            = 0x5251050f,
	SP5K_MSG_STILL_STORE_FAIL                = 0x52510507,
	SP5K_MSG_STILL_SAVE_PROGRESSIVE          = 0x57510518,
	SP5K_MSG_STILL_CAPTURE_IMAGE_ID          = 0x53510519,
	SP5K_MSG_STILL_READOUT_START             = 0x5351051f,
	SP5K_MSG_STILL_STORAGE_FULL              = 0x52510509,
	SP5K_MSG_STILL_CONFIRM_STORE             = 0x5251050a,
	SP5K_MSG_STILL_STORE_ALL_JOB_COMPLETE    = 0x5251051e,
	SP5K_MSG_CALI_COMPLETE                   = 0x5251051b,
	SP5K_MSG_STILL_BP_SWITCH                 = 0x5651051c,
	SP5K_MSG_PREVIEW_CALLBACK_STOP_COMPLETE  = 0x555105e2,

	/* media */                                /* 0x500 */
	SP5K_MSG_MEDIA_PLAY_START                = 0x61510500, /* param: 0:okay, 1:failed */
	SP5K_MSG_MEDIA_PLAY_STOP                 = 0x61510501,
	SP5K_MSG_MEDIA_PLAY_END                  = 0x61510502,
	SP5K_MSG_MEDIA_REC_START                 = 0x61510503, /* param: 0:okay, 1:failed */
	SP5K_MSG_MEDIA_REC_STOP                  = 0x61510504,
	SP5K_MSG_MEDIA_REC_DISK_FULL             = 0x61510505,
	SP5K_MSG_MEDIA_FF_TO_END                 = 0x61510506,
	SP5K_MSG_MEDIA_RW_TO_END                 = 0x61510507,
	SP5K_MSG_MEDIA_SOUND_FINISH              = 0x61510508,
	SP5K_MSG_MEDIA_ELAPSED_TIME_UPDATE       = 0x61510509,
	SP5K_MSG_MEDIA_PLAY_FILE_ERROR           = 0x6151050b,
	SP5K_MSG_MEDIA_REC_PAUSE_FINISH          = 0x6151050c,
	SP5K_MSG_MEDIA_FILE_SIZE_NOTIFY          = 0x6151050d,
	SP5K_MSG_MEDIA_REC_DROP_FRAME            = 0x6151050e, /* param: 0=video, 1=audio */
	SP5K_MSG_MEDIA_AVI_REC_FPS_LOW           = 0x6151050f,
	SP5K_MSG_MEDIA_REC_AUD_HW_STOP           = 0x61510510,
	SP5K_MSG_MEDIA_TRIM_END                  = 0x61510511,
	SP5K_MSG_MEDIA_SEMALESS_CNT_NOTIFY       = 0x61510512,
	SP5K_MSG_MEDIA_URGENT_CB_STOP            = 0x61510513,
	SP5K_MSG_MEDIA_REC_FRAME_CAPUTRE_END     = 0X61510515,
	SP5K_MSG_MEDIA_REC_RESUME_FINISH         = 0X61510516,
	SP5K_MSG_MEDIA_SEAMLESS_CHANGE_FILE      = 0x61510517,
	SP5K_MSG_MEDIA_REC_CDSP_SKIP_FRAME       = 0x61510518, /* param: [31:30] : 0 : cdsp1 skip, 1 : cdsp2 skip, 2 : video skip, [29:0] : skip count. */
	SP5K_MSG_MEDIA_CODEC_FPS_LOW             = 0x61510519, /* param: (a<<24)|(b<<16)|(c<<8)|(d),
																	  a: 0=dec, 1=enc,
																	  b: 0=h264, 1=mjpg,
																	  c: stream id,
																	  d: higher(0) or lower(1) */
	SP5K_MSG_MEDIA_PLAY_SEAMLESS_LOAD_NOTIFY = 0x61510520,
	SP5K_MSG_MEDIA_PLAY_SEAMLESS_LOAD_FINISH = 0x61510521,
	SP5K_MSG_MEDIA_PLAY_SEAMLESS_LOAD_FAILED = 0x61510522,
	SP5K_MSG_MEDIA_REC_FIRST_FRAME_READY     = 0x61510523,
	SP5K_MSG_MEDIA_AVPLAY_DONE               = 0x61510524,
	SP5K_MSG_MEDIA_PLAY_ZOOM_DONE            = 0x61510525,
	SP5K_MSG_MEDIA_DIQ_FLUSH_ERROR           = 0x61510526,
	SP5K_MSG_MEDIA_REC_TRIGGER_FINISH		 = 0x61510527,
	SP5K_MSG_MEDIA_REC_VLC_FULL				 = 0x61510528,
	SP5K_MSG_MEDIA_PRE_REC_TRIGGER_FINISH	 = 0x61510529,
	
	/* usb */
	SP5K_MSG_USB_MSDC_ACCESS_START           = 0x62510001,
	SP5K_MSG_USB_MSDC_ACCESS_STOP            = 0x62510002,
	SP5K_MSG_USB_DPS_NOTIFY_JOB_STATUS       = 0x62510003,
	SP5K_MSG_USB_DPS_NOTIFY_DEVICE_STATUS    = 0x62510004,
	SP5K_MSG_USB_DEVICE_VBUS_IN              = 0x62510005,
	SP5K_MSG_USB_DEVICE_VBUS_OFF             = 0x62510006,
	SP5K_MSG_USB_DPS_DISCOVER                = 0x62510007,
	SP5K_MSG_USB_POWER_CONFIGURED            = 0x62510008,
	SP5K_MSG_USB_CONFIGURED                  = 0x62510009,
	SP5K_MSG_USB_MSDC_ACCESS                 = 0x6251000a,
	SP5K_MSG_USB_DEVICE_SUSPEND              = 0x6251000b,
	SP5K_MSG_USB_SIDC_ACCESS                 = 0x6251000c,
	SP5K_MSG_USB_SIDC_NOTIFY_STATUS          = 0x6251000d,
	SP5K_MSG_USB_SIDC_ACCESS_START           = 0x6251000e,
	SP5K_MSG_USB_SIDC_ACCESS_STOP            = 0x6251000f,
	SP5K_MSG_USB_MSDC_ACCESS_FAIL            = 0x62510010,
	SP5K_MSG_USB_UI_TOOL_PARSE               = 0x62510011,
	SP5K_MSG_USB_PTP_SESSION_OPEN            = 0x62510018,
	SP5K_MSG_USB_PTP_SESSION_CLOSE           = 0x62510019,
	SP5K_MSG_USB_PTP_OP_START                = 0x6251001a,	/* param: PIMA operation code (usbPIMAOperationCode_e) */
	SP5K_MSG_USB_PTP_OP_END                  = 0x6251001b,	/* param: 0=success, 1=failure */
	SP5K_MSG_USB_MSDC_EJECT                  = 0x6251001c,
	SP5K_MSG_USB_DEVICE_ENUM_DONE            = 0x6251001d,

	/* dcf */
	SP5k_MSG_DCF_INIT_START                  = 0x64510001,
	SP5K_MSG_DCF_FILEOP_COMPLETE             = 0x64510002,
	SP5k_MSG_DCF_INIT_FAIL                   = 0x64510003,
	SP5K_MSG_DCF_FILEOP_FAIL                 = 0x64510004,

	/* slide */
	SP5K_MSG_SLIDE_READY                     = 0x65510000,

	/* dist */
	SP5K_MSG_DIST_STILL_DONE                 = 0x69510001,

	/* object tracking. */
	SP5K_MSG_OBJ_TRACK_READY                 = 0x71510001,
	SP5K_MSG_OBJ_TRACK_STOP                  = 0x71510002,
	SP5K_MSG_OBJ_TRACK_TIMEOUT               = 0x71510003,
	SP5K_MSG_OBJ_TRACK_INVALID               = 0x71510004,
	SP5K_MSG_OBJ_TRACK_START                 = 0x71510005,
	SP5K_MSG_OBJ_TRACK_START_FAIL            = 0x71510006,

	/* pattern tracking. */
	SP5K_MSG_PATTERN_TRACK_READY             = 0x70510001,
	SP5K_MSG_PATTERN_TRACK_STOP              = 0x70510002,
	SP5K_MSG_PATTERN_TRACK_TIMEOUT           = 0x70510003,
	SP5K_MSG_PATTERN_TRACK_INVALID           = 0x70510004,
	/* backward compatible */
	SP5K_MSG_FACE_TRACK_READY                = SP5K_MSG_PATTERN_TRACK_READY,
	SP5K_MSG_FACE_TRACK_STOP                 = SP5K_MSG_PATTERN_TRACK_STOP,
	SP5K_MSG_FACE_TRACK_TIMEOUT              = SP5K_MSG_PATTERN_TRACK_TIMEOUT,
	SP5K_MSG_FACE_TRACK_INVALID              = SP5K_MSG_PATTERN_TRACK_INVALID,

	/* das warning. */
	/* 1:left 2:right,3:both*/
	SP5K_MSG_LANE_SHIFT_WARNING              = 0x70511001,
	SP5K_MSG_HEAD_LIGHT_DETECT               = 0x70511002,
	SP5K_MSG_SPEED_DETECT                    = 0x70511003,

	/*rtmp*/
	SP5K_MSG_OMF_RTMP_SEND_DATA_FAIL               = 0x72510001,
	SP5K_MSG_OMF_RTMP_SEND_DATA_SUCCESS        = 0x72510002,
	SP5K_MSG_OMF_RTMP_CONNECT_FAIL                  = 0x72510003,
	SP5K_MSG_OMF_RTMP_CONNECT_SUCCESS           = 0x72510004,
	SP5K_MSG_OMF_RTMP_RECONNECT_FAIL              = 0x72510005,
	SP5K_MSG_OMF_RTMP_RECONNECT_SUCCESS       = 0x72510006,
	
	/* might be more ...*/

	/* customer */
	SP5K_MSG_HEAD_CUSTOMER                   = 0xF0000000,
} sp5kHostMsgList_e;

typedef enum sp5kHostMsgParamMask_e {
	/*disk drivers*/
	SP5K_MSG_PARM_DISK_UNKNOWN_ERROR         = 0x00000000,
	SP5K_MSG_PARM_DISK_OPEN_ERROR            = 0x00010000,
	SP5K_MSG_PARM_DISK_CLOSE_ERROR           = 0x00020000,
	SP5K_MSG_PARM_DISK_WRITE_ERROR           = 0x00040000,
	SP5K_MSG_PARM_DISK_READ_ERROR            = 0x00080000,
	SP5K_MSG_PARM_DISK_DELETE_ERROR          = 0x00100000,

	/*media*/
	SP5K_MSG_PARM_MEDIA_AUD_PTS_ERROR        = 0x90000000,
	SP5K_MSG_PARM_MEDIA_AUD_FIFO_OVER        = 0xA0000000,
	SP5K_MSG_PARM_MEDIA_AUD_QUE_OVER         = 0xC0000000,
	SP5K_MSG_PARM_MEDIA_VID_PTS_ERROR        = 0x10000000,
	SP5K_MSG_PARM_MEDIA_VID_FIFO_OVER        = 0x20000000,
	SP5K_MSG_PARM_MEDIA_VID_QUE_OVER         = 0x40000000,
} sp5kHostMsgParamMask_e;

#define SYSG_HOST_MSG_CALLBACK_RET_SKIP      ( 1 << 0 )
#define SYSG_HOST_MSG_CALLBACK_RET_MODIFIED  ( 1 << 1 )

#define SP5K_HOST_MSG_CALLBACK_RET_SKIP      SYSG_HOST_MSG_CALLBACK_RET_SKIP
#define SP5K_HOST_MSG_CALLBACK_RET_MODIFIED  SYSG_HOST_MSG_CALLBACK_RET_MODIFIED

#endif  /* _SP5K_MSG_DEF_H_ */

