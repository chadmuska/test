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
#ifndef GPIO_CUSTOM_H
#define GPIO_CUSTOM_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

#include "customization/dev_init.h"
#include "gpio_def.h"
#include "app_btn_def.h"

#include GPIO_INC

/*-- SD Card */
#define IS_CARD_EXIST   IN_SD_CARD_DETECT
#define IS_CARD_LOCK    IN_SD_CARD_WP
#define IS_TV_IN        IN_TV_DETECT
#if !API_TEST_INIT
#define IS_USB_IN       IN_USB_DETECT /*(sp5kUsbDetectStateGet()) replaced by USB GPIO in V35.*/
#else
#define IS_USB_IN       0
#endif
#if (KIT_WITH_HDMI)
#define IS_HDMI_IN      (sp5kDispHdmiHotPlugDetectGet(0))
#define IS_CEC_ON		        0
#else
#define IS_HDMI_IN      0
#define IS_CEC_ON		0
#endif


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
extern UINT32 dummyioLcdBL;

/* Note : EVB does not use the following constants.
 *        Keep the definition here for format compatibility.
 */
#ifndef SPCA_GEN_GPIO_CFGIO
#define SPCA_GEN_GPIO_CFGIO
#define SPCA_GEN_GPIO_MASK
#define SPCA_TGL_GPIO_CFGIO
#define SPCA_TGL_GPIO_MASK
#define SPCA_TGH_GPIO_CFGIO
#define SPCA_TGH_GPIO_MASK
#define SPCA_DISP_GPIO_CFGIO
#define SPCA_DISP_GPIO_MASK
#endif

/* GPIO group initial H/L define*/
#ifndef SPCA_GEN_GPIO_INIT_VAL
#define SPCA_GEN_GPIO_INIT_VAL
#define SPCA_GEN_GPIO_INIT_MASK
#define SPCA_TGL_GPIO_INIT_VAL
#define SPCA_TGL_GPIO_INIT_MASK
#define SPCA_TGH_GPIO_INIT_VAL
#define SPCA_TGH_GPIO_INIT_MASK
#define SPCA_DISP_GPIO_INIT_VAL
#define SPCA_DISP_GPIO_INIT_MASK
#endif
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/
/*-------------------------------------------------------------------------
 * Device IO Controls
 */
#define _SP5K_IO_SET(grp, msk, val) sp5kGpioWrite(grp, msk, (UINT32)((!!val) ? msk : 0))
#define _SP5K_IO_CFG(grp, msk, dir) sp5kGpioCfgSet(grp, msk, (UINT32)((!!dir) ? msk : 0))

/* Lens Zoom Home Led power */
#define IO_LENS_LED_ZHM_PWR_SET(a)	_SP5K_IO_SET(SP5K_GPIO_GRP_LMI, GPIO_MASK(11), a)
#define IO_LENS_LED_ZHM_PWR_ON		IO_LENS_LED_ZHM_PWR_SET(1)
#define IO_LENS_LED_ZHM_PWR_OFF		IO_LENS_LED_ZHM_PWR_SET(0)

/* Lens Zoom Clk Led power */
#define IO_LENS_LED_ZCLK_PWR_SET(a)	_SP5K_IO_SET(SP5K_GPIO_GRP_TGL, GPIO_MASK(0), a)
#define IO_LENS_LED_ZCLK_PWR_ON		IO_LENS_LED_ZCLK_PWR_SET(1)
#define IO_LENS_LED_ZCLK_PWR_OFF	IO_LENS_LED_ZCLK_PWR_SET(0)

/* Lens Focus Home Led power */
#define IO_LENS_LED_FHM_PWR_SET(a)	_SP5K_IO_SET(SP5K_GPIO_GRP_LMI, GPIO_MASK(11), a)
#define IO_LENS_LED_FHM_PWR_ON		IO_LENS_LED_FHM_PWR_SET(1)
#define IO_LENS_LED_FHM_PWR_OFF		IO_LENS_LED_FHM_PWR_SET(0)

#define IO_STROBE_TRIG_SET(a)		_SP5K_IO_SET(SP5K_GPIO_GRP_TGL, GPIO_MASK(CFG_FLASH_TRIG), a)
#define IO_STROBE_TRIG_ON			IO_STROBE_TRIG_SET(1)
#define IO_STROBE_TRIG_OFF			IO_STROBE_TRIG_SET(0)
#define IO_STROBE_TRIG_GET			gpioPinLevelGet(SP5K_GPIO_GRP_TGL, GPIO_MASK(CFG_FLASH_TRIG))

#define IO_STROBE_PWR_SET(a)		dummyio_set
#define IO_STROBE_PWR_ON			IO_STROBE_PWR_SET(1)
#define IO_STROBE_PWR_OFF			IO_STROBE_PWR_SET(0)
#define IO_STROBE_PWR_GET			1

#define IO_STROBE_CHARGE_SET(a)		/*dummyio_set*/_SP5K_IO_SET(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_FLASH_CHARGE), a)
#define IO_STROBE_CHARGE_ON			IO_STROBE_CHARGE_SET(1)
#define IO_STROBE_CHARGE_OFF		IO_STROBE_CHARGE_SET(0)
#define IO_STROBE_CHARGE_GET		/*1*/gpioPinLevelGet(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_FLASH_CHARGE))

#define IO_STROBE_CHARGE_RDY_GET	gpioPinLevelGet(SP5K_GPIO_GRP_LMI, GPIO_MASK(CFG_FLASH_READY))
#define IS_STROBE_RDY				(!!IO_STROBE_CHARGE_RDY_GET)

#define IO_SPEAKER_SET(a)        dummyio_set
#define IO_SPEAKER_ON             IO_SPEAKER_SET(1)
#define IO_SPEAKER_OFF            IO_SPEAKER_SET(0)

/*-- Camera UI Keys*/
#define IO_KEY_UP_GET			adcBtnIoLevelGet(BTN_UP)
#define IO_KEY_DOWN_GET			adcBtnIoLevelGet(BTN_DOWN)
#define IO_KEY_LEFT_GET			adcBtnIoLevelGet(BTN_LEFT)
#define IO_KEY_RIGHT_GET		adcBtnIoLevelGet(BTN_RIGHT)
#define IO_KEY_OK_GET			adcBtnIoLevelGet(BTN_SET)
#define IO_KEY_MENU_GET			adcBtnIoLevelGet(BTN_MENU)
#define IO_KEY_DEL_GET			adcBtnIoLevelGet(BTN_DEL)
#define IO_KEY_MODE_GET			adcBtnIoLevelGet(BTN_MODE)


/*-- IRIS, SHUTTER */
#define CFG_IRIS_N 	    		PIN_4//PIN_27 	//lmi grp enable
#define CFG_SHUTTER_P 			PIN_3//PIN_24	//lmi grp position/direction

/*-- BATTERY */
#if defined(HW_JETDVR) || defined(HW_PDVR)|| defined(HW_17DVR)||defined(HW_19DVR)
/*Output*/
#define CFG_BATCHARGER_ENn          PIN_16//lmi grp #Battery Charger Enable
#else
#define CFG_BATCHARGER_ENn          PIN_12//lmi grp #Battery Charger Enable
#endif
/*Input*/
#define CFG_BATCHARGER_CHARGINGn	PIN_15//lmi grp #Battery Charging

/*-- LED  Isaac change dummy */
#define IO_LED_MAIN_SET(a) 		SET_LED_POWER(a)
#define LED_MAIN_ON			IO_LED_MAIN_SET(1)
#define LED_MAIN_OFF			IO_LED_MAIN_SET(0)

#define IO_LED_STIMER_SET(a) 	SET_LED_SELFTIMER(a)
#define LED_STIMER_ON			IO_LED_STIMER_SET(1)
#define LED_STIMER_OFF			IO_LED_STIMER_SET(0)

/*-- LCD */
#define IO_LCD_PWR_SET(a)       SET_PWR_LCD(a)  /* LCD Power IO Control */
#define LCD_PWR_ON              IO_LCD_PWR_SET(1)
#define LCD_PWR_OFF             IO_LCD_PWR_SET(0)

#define IO_LCD_BACKLIGHT_SET(a) SET_PWR_BACKLIGHT(a)  /* LCD Backlight IO Control */
#define LCD_BACKLIGHT_ON        IO_LCD_BACKLIGHT_SET(1)
#define LCD_BACKLIGHT_OFF       IO_LCD_BACKLIGHT_SET(0)
#define IS_BACKLIGHT_ON         OUT_PWR_BACKLIGHT/*(0)*/

#define IO_LCD2_BACKLIGHT_SET(a) SET_PWR_BACKLIGHT2(a)  /* LCD2 Backlight IO Control */
#define LCD2_BACKLIGHT_ON        IO_LCD2_BACKLIGHT_SET(1)
#define LCD2_BACKLIGHT_OFF       IO_LCD2_BACKLIGHT_SET(0)

#define IO_LCD_RESET_SET(a)     SET_LCD_RESET(a)  /* LCD RESET IO Control */
#define LCD_RESET_ON            IO_LCD_RESET_SET(1)
#define LCD_RESET_OFF           IO_LCD_RESET_SET(0)

#if (SP5K_LCD_A025BN01_UPS051 || SP5K_LCD_A025BN01_UPS052 || SP5K_LCD_A024CN03_UPS052)
#define IO_LCD_STANDBY_SET(a)  {_dispSpiWrite(0x00, a?0x3f:0x39, 16, 0);dummyioLcdBL=a;}
#elif SP5K_LCD_TS027CAARD02_UPS052 || SP5K_LCD_TD025THEEA_UPS052
#define IO_LCD_STANDBY_SET(a)  {_dispSpiWrite(0x05, a?0x5e:0x5c, 16, 0);dummyioLcdBL=a;}
#elif SP5K_LCD_TXDT250C_UPS052 || SP5K_LCD_WDF9648W7FLWA_UPS052 || SP5K_LCD_WDF9648W7FLWA_UPS051 || SP5K_LCD_WDF9648XK7FLWA_UPS051 || SP5K_LCD_S6D04D2_DSI1_LCM
#define IO_LCD_STANDBY_SET(a)  {dummyioLcdBL=a;}
#elif SP5K_LCD_TXDT240TQ_LCM_H || SP5K_LCD_TXDT240TQ_LCM_V || SP5K_LCD_COGT350_LCM_H  || SP5K_LCD_T2024PD0_LCM_H
#define IO_LCD_STANDBY_SET(a)  {dummyioLcdBL=a;}
#elif SP5K_LCD_GPM1316B0_DSI_H || SP5K_LCD_GPM1316B0_DSI_H
#define IO_LCD_STANDBY_SET(a)  {dummyioLcdBL=a;}
#elif SP5K_LCD_DUMMY || SP5K_LCD_A025BN01_YUV_W640 ||SP5K_LCD_A025BN01_YUV_W720 || SP5K_LCD_GPM1303A0_UPS051 || SP5K_LCD_GPM1177B1_UPS052|| SP5K_LCD_Y83482V01_UPS051 ||SP5K_LCD_VS020C2A_UPS052 || SP5K_LCD_GM15000_UPS052
#define IO_LCD_STANDBY_SET(a)  {dummyioLcdBL=a;}
#endif
#define IO_LCD_STANDBY_GET      dummyioLcdBL
#define LCD_WAKEUP              IO_LCD_STANDBY_SET(1)
#define LCD_STANDBY             IO_LCD_STANDBY_SET(0)
#define IS_LCD_ACTIVE           (IO_LCD_STANDBY_GET)

#ifdef EVB_CFG_COUGAR
/* Lens Zoom Home Led Value */
#define IO_LENS_LED_ZHM_GET			gpioPinLevelGet(SP5K_GPIO_GRP_TGL, GPIO_MASK(2))

/* Lens Zoom Clk Led Value */
#define IO_LENS_LED_ZCLK_GET		gpioPinLevelGet(SP5K_GPIO_GRP_GEN, GPIO_MASK(13))

/* Lens Focus Home Led Value */
#define IO_LENS_LED_FHM_GET			gpioPinLevelGet(SP5K_GPIO_GRP_TGL, GPIO_MASK(3))

#endif /*-- EVB_CFG_COUGAR */
/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void gpioInitialCfgSet(void);
UINT32 gpioPinLevelGet(UINT32 ioGrp, UINT32 ioMask);


typedef enum _I2C_DEVICE_TYPE {
	I2C_DEVICE_PWR_IC  		= (unsigned char)(0x00),
	I2C_DEVICE_G_SENSOR  	= (unsigned char)(0x01),
} I2C_DEVICE_TYPE;

UINT32 appI2CInit(void);
void appGpioInit( void );
UINT32 appI2CRead(UINT32 subAddr,UINT32 *pdata);
UINT32 appI2CWrite(UINT32 par0, UINT32 mask, UINT32 par1);


#endif  /* GPIO_CUSTOM_H */


