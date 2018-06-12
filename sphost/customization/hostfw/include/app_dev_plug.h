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
#ifndef APP_DEV_PLUG_H
#define APP_DEV_PLUG_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define POWER_OFF_AFTER_USB_OUT      0
#define POWER_OFF_AFTER_CARD_PLUG    0

typedef enum {
	APP_DEVICE_CARD = 0x00,
	APP_DEVICE_USB,
	APP_DEVICE_TV,
} appDevicePlug_e;

typedef enum {
    HDMI_OUT = 0,   /*HDMI_OUT_DONE*/
    HDMI_IN  = 1    /*HDMI_IN_DONE*/
}appHdmiPlugStatus_e;

typedef enum {
    HDMI_IN_STAGE  = 0x10,
    HDMI_OUT_STAGE = 0x20,
    HDMI_STABLE    = 0x40
}appHdmiStage_e;

typedef enum {
	HDMI_IN_CONFIRM=HDMI_IN_STAGE|0x01,     /*Stage of HW ensure*/
	HDMI_IN_PROC   =HDMI_IN_STAGE|0x02,     /*Stage of start HDMI_IN process*/
	HDMI_IN_DONE   =HDMI_STABLE|0x01,       /*Stage of HDMI_IN process complete*/
	HDMI_OUT_CONFIRM=HDMI_OUT_STAGE|0x01,   /*Stage of HW ensure*/
	HDMI_OUT_PROC   =HDMI_OUT_STAGE|0x02,   /*Stage of start HDMI_OUT process*/
	HDMI_OUT_DONE   =HDMI_STABLE|0x02       /*Stage of HDMI_OUT process complete*/
}appHdmiPlugDetailStatus_e;

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appDevStatusInit(void);
void appHotPlugNextStChange(void);
void appDevPlugStatusChange(void);
void appHdmiStatusSet(BOOL IN);
UINT8 appHdmiStateGet(void);

#endif  /* APP_DEV_PLUG_H */
