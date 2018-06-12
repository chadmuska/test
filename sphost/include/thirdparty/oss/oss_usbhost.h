/**************************************************************************
 *
 *       Copyright (c) 2013-2013 by iCatch Technology, Inc.
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
 * @file usbhost_api.h
 * @brief usbhost middleware interface
 * @author LiJun
 *
 *
 */
#ifndef _USBHOST_API_H_
#define _USBHOST_API_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include <stdio.h>
#include "common.h"

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
/* -- Init Mode -- */
#define USBHOST_DRVMODE_EHCI_MSDC  0x00
#define USBHOST_DRVMODE_EHCI_NET   0x01
#define USBHOST_DRVMODE_EHCI_UVC   0x02
#define USBHOST_DRVMODE_EHCI_ALL   0x0f
#define USBHOST_DRVMODE_OHCI_MSDC  0x10
#define USBHOST_DRVMODE_OHCI_NET   0x11
#define USBHOST_DRVMODE_OHCI_UVC   0x12
#define USBHOST_DRVMODE_OHCI_ALL   0x1f
#define USBHOST_DRVMODE_XHCI_MSDC  0xf0
#define USBHOST_DRVMODE_XHCI_NET   0xf1
#define USBHOST_DRVMODE_XHCI_UVC   0xf2
#define USBHOST_DRVMODE_XHCI_ALL   0xff

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
typedef enum {
	/*===== Root Hub  Layer  Event =====*/
	USB_SYSEVT_DEVICE_PLG_IN,	/* for usb host detect infor at first */
	USB_SYSEVT_DEVICE_PLG_OUT, 

	/*===== Protocol  Layer  Event =====*/
	USB_SYSEVT_SET_ADDR_ERR,
	USB_SYSEVT_GET_DSCR_ERR,	/* get descriptor error */

	/*===== Driver  Layer  Event =====*/
	USB_SYSEVT_MSDC_PLG_IN,
	USB_SYSEVT_UVC_PLG_IN,
	USB_SYSEVT_UNKNOWN_PLG_IN,
	
	USB_SYSEVT_MSDC_PLG_OUT,
	USB_SYSEVT_UVC_PLG_OUT,
	USB_SYSEVT_UNKNOWN_PLG_OUT,
	
	USB_SYSEVT_MSDC_PROBE_ERR,
	USB_SYSEVT_UVC_PROBE_ERR,

	/*===== HW  Layer  Event =====*/
	/* FixMe: 
		to be added 
	*/ 

	USB_SYSEVT_NR
} USBSysEvt;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
/* register moudle functions */
extern void usbhostFuncUninstall(void);
extern void usbhostFuncInstall(void);

/* test */
extern void cmdOhciTestInit(void);
extern void cmdEhciTestInit(void);
extern void cmdUsbHostTestInit(void);
extern void cmdUsbHostInit(void);

typedef void (*USBSysEvtHandler)(USBSysEvt evt, unsigned long param, unsigned long udata);
void usb_sysevt_send(USBSysEvt evt, unsigned long param);

/*uvc*/
enum icatchtek_uvc_cmd {
	ICATCHTEK_UVC_QUERYCAP,
	ICATCHTEK_UVC_QUERYCTRL,
	ICATCHTEK_UVC_G_CTRL,
	ICATCHTEK_UVC_S_CTRL,
	ICATCHTEK_UVC_QUERYMENU,
	ICATCHTEK_UVC_G_EXT_CTRLS,
	ICATCHTEK_UVC_S_EXT_CTRLS,
	ICATCHTEK_UVC_TRY_EXT_CTRLS,
	ICATCHTEK_UVC_ENUMINPUT,
	ICATCHTEK_UVC_G_INPUT,
	ICATCHTEK_UVC_S_INPUT,
	ICATCHTEK_UVC_ENUM_FMT,
	ICATCHTEK_UVC_TRY_FMT,
	ICATCHTEK_UVC_S_FMT,
	ICATCHTEK_UVC_G_FMT,
	ICATCHTEK_UVC_ENUM_FRAMESIZES,
	ICATCHTEK_UVC_ENUM_FRAMEINTERVALS,
	ICATCHTEK_UVC_S_PARM,
	ICATCHTEK_UVC_G_PARM,
	ICATCHTEK_UVC_CROPCAP,
	ICATCHTEK_UVC_G_CROP,
	ICATCHTEK_UVC_S_CROP,
	ICATCHTEK_UVC_REQBUFS,
	ICATCHTEK_UVC_QUERYBUF,
	ICATCHTEK_UVC_QBUF,
	ICATCHTEK_UVC_DQBUF,
	ICATCHTEK_UVC_STREAMON,
	ICATCHTEK_UVC_STREAMOFF,
	ICATCHTEK_UVC_ENUMSTD,
	ICATCHTEK_UVC_QUERYSTD,
	ICATCHTEK_UVC_G_STD,
	ICATCHTEK_UVC_S_STD,
	ICATCHTEK_UVC_OVERLAY,
	ICATCHTEK_UVC_ENUMAUDIO,
	ICATCHTEK_UVC_ENUMAUDOUT,
	ICATCHTEK_UVC_ENUMOUTPUT,
	ICATCHTEK_UVC_CTRL_MAP,
	ICATCHTEK_UVC_CTRL_QUERY,
};

UINT32 icatchtek_uvc_ops(void * uh, UINT32 cmd, void* arg);
UINT32 uvc_get_devno(void* uh);

void* uvc_dev_open(UINT32 devno);
UINT32 uvc_dev_close(UINT32 devno);
UINT32 uvc_list_support_fmt_devno(UINT32 fmt);
UINT32 uvc_get_current_devno(void);
void uvc_set_current_devno(UINT32 devno);
UINT32 uvc_request_buf(void* uh, UINT32 num);
UINT32 uvc_query_buf(void* uh, UINT32 index, void **pbuf, UINT32 *len);
UINT32 uvc_queue_buf(void* uh, UINT32 index);
UINT32 uvc_dequeue_buf(void* uh, UINT32 *index, UINT32 *len);
UINT32 uvc_streamon(void* uh);
UINT32 uvc_streamoff(void* uh);
UINT32 uvc_set_current_configure(void* uh, UINT32 fmt, UINT32 width, UINT32 height, UINT32 interval);
UINT32 uvc_get_current_configure(void* uh, UINT32 *fmt, UINT32 *width, UINT32 *height, UINT32 *interval);
UINT32 uvc_list_support_configure(void* uh);
UINT32 uvc_get_ctrl(void* uh, UINT32 id, UINT32 *value);
UINT32 uvc_set_ctrl(void* uh, UINT32 id, UINT32 value);
UINT32 uvc_get_extension_unit_desc(void* uh, UINT32 id, UINT8 *guid, UINT8 *bNumControls, UINT8 *bControlSize, UINT8 *bmControls);
UINT32 uvc_extension_unit_get_info(void* uh, UINT32 id, UINT32 selector, UINT8 *info);
UINT32 uvc_extension_unit_get_len(void* uh, UINT32 id, UINT32 selector, UINT16 *len);
UINT32 uvc_extension_unit_get_min(void* uh, UINT32 id, UINT32 selector, UINT32 len, UINT8 *min);
UINT32 uvc_extension_unit_get_max(void* uh, UINT32 id, UINT32 selector, UINT32 len, UINT8 *max);
UINT32 uvc_extension_unit_get_def(void* uh, UINT32 id, UINT32 selector, UINT32 len, UINT8 *def);
UINT32 uvc_extension_unit_get_res(void* uh, UINT32 id, UINT32 selector, UINT32 len, UINT8 *res);
UINT32 uvc_extension_unit_get_cur(void* uh, UINT32 id, UINT32 selector, UINT32 len, UINT8 *cur);
UINT32 uvc_extension_unit_set_cur(void* uh, UINT32 id, UINT32 selector, UINT32 len, UINT8 *cur);
int usb_get_str_desc(void* uh,UINT32 index, char* buf);

#ifdef __cplusplus
}
#endif

#endif /*_USBHOST_API_H_*/


