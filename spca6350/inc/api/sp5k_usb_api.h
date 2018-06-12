/**************************************************************************
 *
 *       Copyright (c) 2002-2016 by iCatch Technology, Inc.
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
*  Author:
*
*******************************************************************************/
#ifndef _SP5K_USB_API_H_
#define _SP5K_USB_API_H_

#include "common.h"
#include "middleware/pima_code.h"

/*******************************************************************************
*                          D A T A    T Y P E S
*******************************************************************************/
typedef UINT32 (*fpsp5kUsbCallback_t)(UINT16 idx, UINT16 val, UINT8 *pdata, UINT16 dataSize);
typedef void (*fpsp5kUsb3ACallback_t)(void);
typedef UINT32 (*fpsp5KUsbChargerCusDetectCB_t)(UINT32 dp, UINT32 dm, UINT32 *Pporttype);
/** pcbwcb : pointer to CBWCB block.
      dataDir : data direction, 1: data in(send to host) ; 0 : data out (send to device).
      dCBWDataTransferLength : USB host expected data transfer length.
      pdataBuf : pointer to transfer buffer.
      pretdataSize : actual data transfer length (only support data in).
*/
typedef UINT32 (*fpsp5kUsbMsdcCusUfiCB_t)(UINT8 *pcbwcb, UINT8 dataDir, UINT32 dCBWDataTransferLength, UINT8 *pdataBuf,UINT32 *pretdataSize);

/*same as usb_api.h USC_MEDIA_PARAM_t*/
typedef struct sp5kUsbUVCSCMediaParam_s {
	UINT16 width;
	UINT16 height;
	UINT32 frmRate;
	UINT32 bitRate;
	UINT32 vCodec;
} sp5kUsbUVCSCMediaParam_t;

typedef UINT32 (*fpsp5kUsbUVCSCStart_t)(sp5kUsbUVCSCMediaParam_t *param);
typedef UINT32 (*fpsp5kUsbUVCSCStop_t)(void);
typedef void (*fpsp5kUsbUVCStillStartCb_t)(UINT32 width, UINT32 height, UINT8 **ppdataBuf, UINT32 *pdataSize);
typedef void (*fpsp5kUsbUVCStillStopCb_t)(UINT8 *pdataBuf);
typedef UINT32 (*fpsp5kUsbUVCDISPStart_t)(UINT16 Width,UINT16 Height,UINT32 fps);
typedef UINT32 (*fpsp5kUsbUVCDISPStop_t)(void);

/* For Application mode defintion */
typedef enum {
	SP5K_USB_MODE_PCCAM   = 0x00000001,
	SP5K_USB_MODE_MSDC    = 0x00000002,
	SP5K_USB_MODE_SIDC    = 0x00000003,
	SP5K_USB_MODE_APITEST = 0x00000004,
	SP5K_USB_MODE_MULTI	  = 0x00000005,
	SP5K_USB_MODE_MAX_NUM = 0x00000006
} sp5k_Usb_Mode;

/* For Error code */
typedef enum{
	SP5K_ERR_USB_SUCCESS           = 0x00000000,
	SP5K_ERR_USB_FAIL              = 0x00000001,
	SP5K_ERR_USB_PARAM_ERR         = 0x00000004,
	SP5K_ERR_DPS_PRT_NOT_AVAILABLE = 0x00000005,
	SP5K_ERR_DPS_ADDIMAGE          = 0x0D09060D,
	SP5K_ERR_USB_MAX_NUM           = 0x0D090603
} sp5k_usb_error_code;

/* For SP5K_USB_DEV_DESC_CUSTOM_STR_CFG */
typedef enum {
	/*!> max size is 31 byte */
	SP5K_USB_STR_INDEX_MANUFACTURER		= 1,
	SP5K_USB_STR_INDEX_PRODUCT			= 2,
	SP5K_USB_STR_INDEX_SERIAL_NUMBER	= 3,
	SP5K_USB_STR_INDEX_DPS_VERSION      = 4,
	SP5K_USB_STR_INDEX_DPS_NAME         = 5,
	SP5K_USB_STR_INDEX_DPS_SPEC_VERSION = 6,
	SP5K_USB_STR_INDEX_DPS_PRODUCT_NAME = 7,
	SP5K_USB_STR_INDEX_DPS_SERIAL_NUMBER= 8,
	SP5K_SIDC_STR_INDEX_MANUFACTURER = 16,
	SP5K_SIDC_STR_INDEX_MODEL = 17,
	SP5K_SIDC_STR_INDEX_DEVICE_VERSION = 18,
	SP5K_SIDC_STR_INDEX_SERIAL_NUMBER = 19,
} sp5kUsbStrDesc_t;

typedef enum {
	SP5K_USB_DETECT_PIN = 0,
}sp5kUsbPinCfg_t;

/* USB Bus Max Power */
typedef enum {
	SP5K_USB_MAXPOWER_HIGH           = 0x00000001,
	SP5K_USB_MAXPOWER_NORMAL         = 0x00000002
} sp5kUsbMaxPower_t;

/* USB bus event */
typedef enum {
	SP5K_USB_BUS_EVENT_NOINSTANCE,
	SP5K_USB_BUS_EVENT_SUSPEND,
	SP5K_USB_BUS_EVENT_RESUME,
	SP5K_USB_BUS_EVENT_RESET,
} sp5kUsbBusEvent_t;

/* USB charge selector */
typedef enum {
	SP5K_USB_CHARGE_MODE,
	SP5K_USB_CHARGE_START,
	SP5K_USB_CHARGE_STOP,
} sp5kUsbChargeSel_t;

/* USB charger port type */
typedef enum {
	SP5K_USB_DEDICATED_CHARGING_PORT_OPEN,
	/*SP5K_USB_DEDICATED_CHARGING_PORT_50K,*/
	SP5K_USB_DEDICATED_CHARGING_PORT_SHORT,
	SP5K_USB_DEDICATED_CHARGING_PORT_DPGND,
	SP5K_USB_STANDARD_DOWNSTREAM_PORT,
	SP5K_USB_CHARGING_DOWNSTREAM_PORT,
	SP5K_USB_DEDICATED_CHARGING_PORT_CUST = 0x100, 
} sp5kUsbChargePortType_t;

/*paramId*/
typedef enum {
	SP5K_USB_DEV_DESC_BCD_DEVICE_CFG          = 0x100, 
	SP5K_USB_DEV_DESC_CUSTOM_STR_CFG          = 0x101,
	SP5K_USB_DEV_LINE_STABLE_TIME_CFG         = 0x102,
	SP5K_USB_DEV_LINE_STATE_ADDR_CFG          = 0x103,
	SP5K_USB_DEV_MAX_POWER_SET                = 0x104,
	SP5K_USB_DEV_TASK_PRIORITY_ENHANCE_CFG    = 0x105,

	SP5K_USB_PCCAM_UVC_RESOLUTION_CHANGE_CB   = 0x200,
	SP5K_USB_PCCAM_UVC_MJPG_BYTERATE_SET      = 0x201,
	SP5K_USB_PCCAM_UVC_VER_SET                = 0x202,
	SP5K_USB_PCCAM_UVC_STILL_IMG_SAVE         = 0x203,
	SP5K_USB_PCCAM_UVC_STREAM_CONTROL_SET	  = 0x204,
	SP5K_USB_PCCAM_UVC_STREAM_CONTROL_CB	  = 0x205,
	SP5K_USB_PCCAM_UVC_DRAM_CLIP_SRC_SET	  = 0x206, /* yuv source: 0 - main path, 1 - 2nd path */
	SP5K_USB_PCCAM_UVC_IF_EXCHANGE_SET	      = 0x207, /* interface 1 mapping to EP1: 1 ; interface 2 mapping to EP1: 0 */
	SP5K_USB_PCCAM_UVC_DISP_CB	              = 0x208, 
	SP5K_USB_PCCAM_UVC_DISP_YUV_BUF_NUM_SET	  = 0x209,
	SP5K_USB_PCCAM_UVC_STILL_CB				  = 0x20a,
	SP5K_USB_PCCAM_UVC_MJPG_BITRATE_SET       = 0x20b,
	SP5K_USB_PCCAM_UVC_H264_DYNAMIC_BITRATE   = 0x20C, /* for uvc dynamic bit rate */
	
	SP5K_USB_MSDC_LUN_SET                     = 0x300,
	SP5K_USB_MSDC_SCSI_CB				      = 0x301,
	SP5K_USB_MSDC_CUSTOM_UFI_CB               = 0x302,
	SP5K_USB_MSDC_CUSTOM_MOUNT_SET            = 0x303,
	SP5K_USB_MSDC_INTERFACE_CLASS_DESC		  = 0x304,
	SP5K_USB_MSDC_HIDE_DISK_STRING		  	  = 0x305,
	
	SP5K_USB_CHARGER_ENUM_TIMEOUT             = 0x400,
	SP5K_USB_CHARGER_DETECT_CUS_TYPE_CB	      = 0x401,

	SP5K_USB_VNDR_REQ_READ_CB                 = 0x500,
	SP5K_USB_VNDR_REQ_WRITE_CB                = 0x501,

	SP5K_USB_NORMAL_ISP_PRE_CB                = 0x600,	/* void (*pisbCb)(void) */
} sp5kUsbCfg_e;


typedef enum {
	SP5K_USB_DEV_DESCRIPTOR_CFG_BCD_DEVICE,
	SP5K_USB_DEV_DESCRIPTOR_CFG_MAX
} sp5kUsbDeviceDescriptorCfg_t;


/* USB info */
typedef enum {
	SP5K_USB_INFO_BUS_SPEED, /* return value : 1:high speed ; 0:full speed. */
	SP5K_USB_INFO_DATA_SIGNAL, /* return value : bit0:DP ; bit1:DM */
	SP5K_USB_INFO_MSDC_INACTIVE_10MS,
	SP5K_USB_INFO_UVC_FPS,
	SP5K_USB_INFO_UVC_FORMAT,
	SP5K_USB_DISP_YUV_BUF_NUM_GET,
	SP5K_USB_INFO_MAX
} sp5kUsbInfo_t;

typedef enum {
	SP5K_USB_LOGIC_EP_EP0_IN = 0,
	SP5K_USB_LOGIC_EP_EP0_OUT,
	SP5K_USB_LOGIC_EP_BULK_IN,
	SP5K_USB_LOGIC_EP_BULK_OUT,
	SP5K_USB_LOGIC_EP_INTR_IN
} sp5kUsbLogicEP_t;

/* UVC control */
typedef enum {
	SP5K_USB_UVC_START,
	SP5K_USB_UVC_STOP
} sp5kUsbUVCCtl_t;

/*******************************************************************************
*               F U N C T I O N    D E C L A R A T I O N S
*******************************************************************************/
/* general */
UINT32 sp5kUsbFuncCreate(void);
UINT32 sp5kUsbModeSet(UINT32 mode);
#ifndef ISP_BIN_CREATE
UINT32 sp5kUsbLineStateGet(void);
UINT32 sp5kUsbLineStateTrig(void);/*must be after sp5kUsbCfgSet(SP5K_USB_DEV_LINE_STATE_ADDR_CFG,addr)*/

UINT32 sp5kUsbMaxPowerCheck(UINT32 powerMode);

#endif
UINT32 sp5kUsbSwCon(UINT32 enable);

UINT32 sp5kUsbBusEventGet(void);

#ifndef ISP_BIN_CREATE
UINT32 sp5kUsbChargePortTypeGet(void);
#endif

UINT32 sp5kUsbCfgSet(UINT32 paramId,...);
UINT32 sp5kUsbInfoGet(UINT32 infoId);

void sp5kUsbEPTransfer(UINT8 logicEP, UINT8 *pdataStart, UINT32 dataSize);

void sp5kUsbHwEventTrig(UINT8 bStatusType, UINT8 bOriginator, ...);
UINT32 sp5kUsbDispYuvSend(UINT32 yuvAddr);
UINT32 sp5kUsbUVCControl(UINT32 option);

UINT32 sp5kUsbCmdProcessorReg( UINT8 nVndrReqNo, void (* funpCmdProcess)( char * ) );

#endif /* _SP5K_USB_API_H_ */

