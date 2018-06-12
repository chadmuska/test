/**************************************************************************
 *
 *       Copyright (c) 2005-2016 by iCatch Technology, Inc.
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
/**
 * \file   gpio_def.h
 * \brief  opened macros for GPIO port and pin direct access
 * \author Anmin Deng
 * \since  2006-6-27
 *   macros to direct access TGGPIO port and pin for opened sensor model
 * \date   2006-10-20
 *   porting 5000 reg to 5110
 * \date   2007-10-5 
 *   macros to direct access all GPIO port and pin
 * \date   2008-01-27
 *   treating POCU pwronint pins as GPIO
 *
 * coding style for generating more efficient MIPS instructions
 * than direct address coding
 * to direct access GPIO port and pin
 */
#ifndef _GPIO_DEF_H_
#define _GPIO_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

#ifndef LOGI_ADDR_REG
#if defined(__linux__) && !defined(__KERNEL__)
extern UINT32 direct_io_buffer;
#define LOGI_ADDR_REG  (direct_io_buffer)
#else
#define LOGI_ADDR_REG  (0xb0000000)
#endif
#endif

#define _CAT(t,n) t##n


/**
 * \typedef  _gpioPin##_t
 * \brief  GPIO pin bit field type definitions----*/

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1;
} _gpioPin8_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1;
} _gpioPin16_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	_pin_20: 1,_pin_21: 1,_pin_22: 1,_pin_23: 1,
	_pin_24: 1,_pin_25: 1,_pin_26: 1,_pin_27: 1,
	_pin_28: 1,_pin_29: 1,_pin_30: 1,_pin_31: 1;
} _gpioPin32_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	_pin_20: 1,_pin_21: 1,_pin_22: 1,_pin_23: 1,
	_pin_24: 1,_pin_25: 1,_pin_26: 1,_pin_27: 1,
	_pin_28: 1,_pin_29: 1,_pin_30: 1,_pin_31: 1,
	_pin_32: 1,_pin_33: 1,_pin_34: 1,_pin_35: 1,
	_pin_36: 1,_pin_37: 1,_pin_38: 1,_pin_39: 1,
	_pin_40: 1,_pin_41: 1,_pin_42: 1,_pin_43: 1,
	_pin_44: 1,_pin_45: 1,_pin_46: 1,_pin_47: 1;
} _gpioPin48_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	_pin_20: 1,_pin_21: 1,_pin_22: 1,_pin_23: 1,
	_pin_24: 1,_pin_25: 1,_pin_26: 1,_pin_27: 1,
	_pin_28: 1,_pin_29: 1,_pin_30: 1,_pin_31: 1,
	_pin_32: 1,_pin_33: 1,_pin_34: 1,_pin_35: 1,
	_pin_36: 1,_pin_37: 1,_pin_38: 1,_pin_39: 1,
	_pin_40: 1,_pin_41: 1,_pin_42: 1,_pin_43: 1,
	_pin_44: 1,_pin_45: 1,_pin_46: 1,_pin_47: 1,
	_pin_48: 1,_pin_49: 1,_pin_50: 1,_pin_51: 1;
} _gpioPin52_t, _gpioPin_t;

typedef struct {
	UINT8
	:8, :8, :8, :8,
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1;
} _gpioPinH_t;

typedef struct { UINT8
	rsv :4,
	_pin_0 :1, _pin_1 :1, _pin_2 :1, _pin_3 :1;
} _gpioPinMsb4_t;

typedef struct { UINT8
	_pin_4 :1, _pin_5 :1, _pin_6 :1, _pin_7 :1,
	rsv :4;
} _gpioPinLsb4_t;

typedef struct { UINT8
	_pin_0:  2,_pin_1:  2,_pin_2:  2,_pin_3:  2,
	_pin_4:  2,_pin_5:  2,_pin_6:  2,_pin_7:  2,
	_pin_8:  2,_pin_9:  2,_pin_10: 2,_pin_11: 2,
	_pin_12: 2,_pin_13: 2,_pin_14: 2,_pin_15: 2;
} _gpioDpin16_t;

typedef struct { UINT8
	_pin_0:  2,_pin_1:  2,_pin_2:  2,_pin_3:  2,
	_pin_4:  2,_pin_5:  2,_pin_6:  2,_pin_7:  2,
	_pin_8:  2,_pin_9:  2,_pin_10: 2,_pin_11: 2,
	_pin_12: 2,_pin_13: 2,_pin_14: 2,_pin_15: 2,
	_pin_16: 2,_pin_17: 2,_pin_18: 2,_pin_19: 2,
	_pin_20: 2,_pin_21: 2,_pin_22: 2,_pin_23: 2,
	_pin_24: 2,_pin_25: 2,_pin_26: 2,_pin_27: 2,
	_pin_28: 2,_pin_29: 2,_pin_30: 2,_pin_31: 2;
} _gpioDpin32_t;

typedef struct { UINT8
	_pin_0:  2,_pin_1:  2,_pin_2:  2,_pin_3:  2,
	_pin_4:  2,_pin_5:  2,_pin_6:  2,_pin_7:  2,
	_pin_8:  2,_pin_9:  2,_pin_10: 2,_pin_11: 2,
	_pin_12: 2,_pin_13: 2,_pin_14: 2,_pin_15: 2,
	_pin_16: 2,_pin_17: 2,_pin_18: 2,_pin_19: 2,
	_pin_20: 2,_pin_21: 2,_pin_22: 2,_pin_23: 2,
	_pin_24: 2,_pin_25: 2,_pin_26: 2,_pin_27: 2,
	_pin_28: 2,_pin_29: 2,_pin_30: 2,_pin_31: 2,
	_pin_32: 2,_pin_33: 2,_pin_34: 2,_pin_35: 2,
	_pin_36: 2,_pin_37: 2,_pin_38: 2,_pin_39: 2,
	_pin_40: 2,_pin_41: 2,_pin_42: 2,_pin_43: 2,
	_pin_44: 2,_pin_45: 2,_pin_46: 2,_pin_47: 2,
	_pin_48: 2,_pin_49: 2,_pin_50: 2,_pin_51: 2;
} _gpioDpin52_t, _gpioDpin_t;

typedef struct {
	UINT8
	:8, :8, :8, :8, :8, :8, :8, :8,
	_pin_0:  2,_pin_1:  2,_pin_2:  2,_pin_3:  2,
	_pin_4:  2,_pin_5:  2,_pin_6:  2,_pin_7:  2,
	_pin_8:  2,_pin_9:  2,_pin_10: 2,_pin_11: 2,
	_pin_12: 2,_pin_13: 2,_pin_14: 2,_pin_15: 2,
	_pin_16: 2,_pin_17: 2,_pin_18: 2,_pin_19: 2;
} _gpioDpinH_t;

typedef union { UINT8  map; _gpioPin8_t  pin; } _gpioMap8_t;
typedef union { UINT16 map; _gpioPin16_t pin; } _gpioMap16_t;
typedef union { UINT32 map; _gpioPin32_t pin; } _gpioMap32_t;
typedef union {UINT32 map[2];_gpioPin52_t pin;_gpioPinH_t hpin;} _gpioMap52_t;

typedef union { UINT32 map;    _gpioDpin16_t pin; } _gpioDmap16_t;
typedef union { UINT32 map[2]; _gpioDpin32_t pin; } _gpioDmap32_t;
typedef union {
	UINT32 map[4]; _gpioDpin52_t pin; _gpioDpinH_t hpin;
} _gpioDmap52_t;

/**
 * \typedef  _xxxGpio_t
 * \brief  GPIO register opened structure definitions
 */

/**< ------GENGPIO0:15 (16:17 dedicated to uart)------*/
/** GPIO pwmsel[0:5] correspoding bit 1:gpio[8,10,9,14,15,6] is pwm[0:5];
 * GPIO16:17 dedicated to uart rx:tx
 * pwmsel[6] 1:gpio[16:17] is uart rx:tx
 * global [0x00b0] audsel[0:4] corresponding bit 1:gpio[8,10:13] is audio
 * global [0x00b1] gpiomode[0:1]=0 to select GPIO/AUD */
typedef struct { UINT8
	_pin_8 :1, _pin_10 :1, _pin_11 :1, _pin_12 :1, _pin_13 :1,
	:3;
} _gpioPinAudsel_t;
typedef union { UINT8  map; _gpioPinAudsel_t pin; } _gpioMapAudsel_t;

typedef struct { UINT8
	_pin_8 :1, _pin_10 :1, _pin_9 :1, _pin_14 :1, _pin_15 :1, _pin_6 :1,
	_pin_uart :1, :1;
} _gpioPinPwmsel_t;
typedef union { UINT8  map; _gpioPinPwmsel_t pin; } _gpioMapPwmsel_t;

typedef struct {
	UINT8 offset[0x00b0];
	volatile _gpioMapAudsel_t audsel;
	volatile UINT8 gpiomode;
	UINT8 rsv00b2[0x0114-0x00b2];
	volatile _gpioMap32_t pull;
	UINT8 rsv0118[0x0138 - 0x0118];
	volatile _gpioDmap32_t ds;
	UINT8 rsv0140[0x1030 - 0x0140];
	volatile _gpioMap16_t ov;
	UINT8 rsv1032[2];
	volatile _gpioMap16_t oe;
	UINT8 rsv1036[2];
	volatile _gpioMap16_t iv;
	UINT8 rsv103a[2];
	volatile _gpioMapPwmsel_t pwmsel;
} _genGpio_t;

/**< ------FMGPIO0:51---------------------------------*/
/* lcdsel[10:31] bit set 1 to direct DISPGPIO[18:39] to FMGPIO[32:45,24:31] */
typedef struct { UINT8
	:8, :2,
	_pin_32: 1,_pin_33: 1,_pin_34: 1,_pin_35: 1,
	_pin_36: 1,_pin_37: 1,_pin_38: 1,_pin_39: 1,
	_pin_40: 1,_pin_41: 1,_pin_42: 1,_pin_43: 1,
	_pin_44: 1,_pin_45: 1,_pin_24: 1,_pin_25: 1,
	_pin_26: 1,_pin_27: 1,_pin_28: 1,_pin_29: 1,
	_pin_30: 1,_pin_31: 1;
} _gpioPinLcdsel_t;
typedef union { UINT32 map; _gpioPinLcdsel_t pin; } _gpioMapLcdsel_t;

typedef struct {
	UINT8 offset[0x00b4];
	volatile _gpioMapLcdsel_t lcdsel;
	UINT8 rsv00b8[0x010c - 0x00b8];
	volatile _gpioMap52_t pull;
	UINT8 rsv0114[0x0140 - 0x0114];
	volatile _gpioDmap52_t ds;
	UINT8 rsv0150[0x4030 - 0x0150];
	volatile _gpioMap52_t ov;
	volatile _gpioMap52_t oe;
	volatile _gpioMap52_t iv;
	volatile _gpioMap52_t sel;
} _fmGpio_t;

/**< ------TGGPIO0:19---------------------------------*/
/*volatile _gpioMap8_t opol;*/ /* spca6350 opol at MSB of ov */
/*volatile _gpioMap8_t i2csel;*/ /* spca6350 i2csel at MSB of sel */
/*volatile _gpioMap8_t ipol;*/ /* spca6350 ipol at MSB of iv */
typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	: 4,
	opol_pin_3:  1,            : 1, opol_pin_4:  1, opol_pin_8:  1,
	opol_pin_10: 1, opol_pin_11: 1, opol_pin_12: 1, opol_pin_19: 1;
} _gpioPinTgOv_t;
typedef union { UINT32 map; _gpioPinTgOv_t pin; } _gpioMapTgOv_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	: 4,
	sif12ndi2csel: 1, sif22ndi2csel: 1,
	: 6;
} _gpioPinTgSel_t;
typedef union { UINT32 map; _gpioPinTgSel_t pin; } _gpioMapTgSel_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	: 4,
	ipol_pin_9:  1, ipol_pin_10:  1, ipol_pin_11:  1,
	: 5;
} _gpioPinTgIv_t;
typedef union { UINT32 map; _gpioPinTgIv_t pin; } _gpioMapTgIv_t;

typedef struct {
	UINT8 offset[0x0108];
	volatile _gpioMap32_t pull;
	UINT8 rsv010c[0x0130 - 0x010c];
	volatile _gpioDmap32_t ds;
	UINT8 rsv0138[0x9030 - 0x0138];
	volatile _gpioMap32_t oe;
	volatile _gpioMapTgOv_t ov;
	/*volatile _gpioMap8_t opol;*/ /*spca6350 opol at MSB of ov*/
	volatile _gpioMapTgSel_t sel;
	/*volatile _gpioMap8_t i2csel;*/ /*spca6350 i2csel at MSB of sel*/
	volatile _gpioMapTgIv_t iv;
	/*volatile _gpioMap8_t ipol;*/ /*spca6350 ipol at MSB of iv*/
} _tgGpio_t;

/**< ------LMIGPIO0:21--------------------------------*/
/*volatile _gpioMap8_t opol;*/ /* spca6350 opol at MSB of ov */
/*volatile _gpioMap8_t fpga_fpclknxphase;*/ /* spca6350 test at MSB of iv */
typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	_pin_20: 1,_pin_21: 1,
	: 2,
	opol_pin_11: 1, opol_pin_15: 1,
	: 6;
} _gpioPinLmiOv_t;
typedef union { UINT32 map; _gpioPinLmiOv_t pin; } _gpioMapLmiOv_t;

#ifdef FPGA
typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	_pin_20: 1,_pin_21: 1,
	: 2,
	fpga_fpclknx0phase: 1, fpga_fpclknx1phase: 1,
	: 6;
} _gpioPinLmiIv_t;
typedef union { UINT32 map; _gpioPinLmiIv_t pin; } _gpioMapLmiIv_t;
#endif /*#ifdef FPGA*/

typedef struct {
	UINT8 offset[0x0100];
	volatile _gpioMap32_t pull;
	UINT8 rsv0104[0x0120 - 0x0104];
	volatile _gpioDmap32_t ds;
	UINT8 rsv0128[0x9040 - 0x0128];
	volatile _gpioMap32_t oe;
	volatile _gpioMapLmiOv_t ov;
	volatile _gpioDmap16_t sel;
#ifdef FPGA
	volatile _gpioMapLmiIv_t iv;
#else  /*#ifdef FPGA*/
	volatile _gpioMap32_t iv;
#endif /*#ifdef FPGA*/
} _lmiGpio_t;

/**< ------DISPGPIO0:17-------------------------------*/
typedef struct {
	UINT8 offset[0x0104];
	volatile _gpioMap32_t pull;
	UINT8 rsv0108[0x0128 - 0x0108];
	volatile _gpioDmap32_t ds;
	UINT8 rsv0130[0xa108 - 0x0130];
	volatile _gpioMap32_t sel;
	volatile _gpioMap32_t iv;
	volatile _gpioMap32_t ov;
	volatile _gpioMap32_t oe;
} _dispGpio_t;

/**< ------POCUGPIO0:4--------------------------------*/
typedef struct {
	UINT8 offset[0x00c1];
	volatile _gpioMap8_t  iv;
	UINT8 rsv00c2[0x0264 - 0x00c2];
	volatile _gpioMap8_t  ilvl;
} _pocuGpio_t;

/**< ------USBGPIO0:1---------------------------------*/
typedef struct {
	UINT8 offset[0x510d];
	volatile _gpioMap8_t  iv;
} _usbGpio_t;


/**< ------SARGPIO0:9---------------------------------*/
/** SARGPIO sarsel[0:4] corresponding bit 1: sargpio[4,6:9] for audio */
typedef struct { UINT8
	_pin_4 :1, _pin_6 :1, _pin_7 :1, _pin_8 :1, _pin_9 :1,
	:3;
} _gpioPinSarsel_t;
typedef union { UINT8  map; _gpioPinSarsel_t pin; } _gpioMapSarsel_t;

typedef struct {
	UINT8 offset[0x64a0];
	volatile _gpioMapSarsel_t sarsel;
	UINT8 rsv64a1[0x0f];
	volatile _gpioMap16_t ov;
	volatile _gpioMap16_t ie;
	volatile _gpioMap16_t oe;
	volatile _gpioMap16_t iv;
} _sarGpio_t;

/**< ------LNGPIO-------------------------------------*/
typedef struct { UINT8
	ov :1, ie :1, oe :1, iv :1, :4;
} _gpioPinLn_t;

typedef struct {
	UINT8 offset[0x64b8];
	volatile union {UINT8 map; _gpioPinLn_t pin;} op;
} _lnGpio_t;

/**< ------RGBGPIO0:25--------------------------------*/
/*volatile _gpioMap8_t ipol;*/ /* spca6350 ipol at MSB of iv */
typedef struct { UINT8
	_pin_0:  1,_pin_14: 1, /*fpclk*/
	: 6,
	_pin_spi2en: 1, /*RGBGPIO[6,7,20,21]*/
	_pin_i2c2en: 1, /*RGBGPIO[6,7]*/
	: 6,
	_pin_sen2opol: 1,
	: 7, : 8;
} _gpioPinRgbSel_t;
typedef union { UINT32 map; _gpioPinRgbSel_t pin; } _gpioMapRgbSel_t;

typedef struct { UINT8
	_pin_0:  1,_pin_1:  1,_pin_2:  1,_pin_3:  1,
	_pin_4:  1,_pin_5:  1,_pin_6:  1,_pin_7:  1,
	_pin_8:  1,_pin_9:  1,_pin_10: 1,_pin_11: 1,
	_pin_12: 1,_pin_13: 1,_pin_14: 1,_pin_15: 1,
	_pin_16: 1,_pin_17: 1,_pin_18: 1,_pin_19: 1,
	_pin_20: 1,_pin_21: 1,_pin_22: 1,_pin_23: 1,
	_pin_24: 1,_pin_25: 1,
	: 2,
	ipol_pin_16 :1, /*d7p*/
	ipol_pin_15: 1, /*d6n*/
	: 2;
} _gpioPinRgbIv_t;
typedef union { UINT32 map; _gpioPinRgbIv_t pin; } _gpioMapRgbIv_t;

typedef struct {
	UINT8 offset[0x0118];
	volatile _gpioMap32_t pull;
	UINT8 rsv011c[0x02b4 - 0x011c];
	volatile _gpioMap32_t ie;
	UINT8 rsv02b8[0x98c0 - 0x02b8];
	volatile _gpioMap32_t oe;
	volatile _gpioMap32_t ov;
	volatile _gpioMapRgbSel_t sel;
	volatile _gpioMapRgbIv_t iv;
} _rgbGpio_t;

/**
 * \def  _xxxGpioPinXXX
 * \def  _xxxGpioMapXXX
 * \brief  GPIO direct access port macros----------*/

/**< ------GENGPIO------------------------------------*/
#define _genGpioPinAS  (((_genGpio_t *)LOGI_ADDR_REG)->audsel.pin)
#define _genGpioPinPS  (((_genGpio_t *)LOGI_ADDR_REG)->pwmsel.pin)
#define _genGpioPinD   (((_genGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _genGpioPinO   (((_genGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _genGpioPinI   (((_genGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _genGpioMapAS  (((_genGpio_t *)LOGI_ADDR_REG)->audsel.map)
#define _genGpioMapPS  (((_genGpio_t *)LOGI_ADDR_REG)->pwmsel.map)
#define _genGpioMapD   (((_genGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _genGpioMapO   (((_genGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _genGpioMapI   (((_genGpio_t *)LOGI_ADDR_REG)->iv.map)
#define _genGpioMode   (((_genGpio_t *)LOGI_ADDR_REG)->gpiomode)

/**< ------FMGPIO-------------------------------------*/
/* lcdsel[10:31] bit set 1 to direct DISPGPIO[18:39] to FMGPIO[32:45,24:31] */
/*.....*/
#define _fmGpioPinD    (((_fmGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _fmGpioPinO    (((_fmGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _fmGpioPinI    (((_fmGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _fmGpioHPinD   (((_fmGpio_t *)LOGI_ADDR_REG)->oe.hpin)
#define _fmGpioHPinO   (((_fmGpio_t *)LOGI_ADDR_REG)->ov.hpin)
#define _fmGpioHPinI   (((_fmGpio_t *)LOGI_ADDR_REG)->iv.hpin)
#define _fmGpioLMapD   (((_fmGpio_t *)LOGI_ADDR_REG)->oe.map[0])
#define _fmGpioLMapO   (((_fmGpio_t *)LOGI_ADDR_REG)->ov.map[0])
#define _fmGpioLMapI   (((_fmGpio_t *)LOGI_ADDR_REG)->iv.map[0])
#define _fmGpioHMapD   (((_fmGpio_t *)LOGI_ADDR_REG)->oe.map[1])
#define _fmGpioHMapO   (((_fmGpio_t *)LOGI_ADDR_REG)->ov.map[1])
#define _fmGpioHMapI   (((_fmGpio_t *)LOGI_ADDR_REG)->iv.map[1])
#define _fmGpioPinS    (((_fmGpio_t *)LOGI_ADDR_REG)->sel.pin)
#define _fmGpioHPinS   (((_fmGpio_t *)LOGI_ADDR_REG)->sel.hpin)
#define _fmGpioLMapS   (((_fmGpio_t *)LOGI_ADDR_REG)->sel.map[0])
#define _fmGpioHMapS   (((_fmGpio_t *)LOGI_ADDR_REG)->sel.map[1])

/**< ------TGGPIO-------------------------------------*/
/* spca6350 opol at MSB of ov */
/* spca6350 i2csel at MSB of sel */
/* spca6350 ipol at MSB of iv */
#define _tgGpioPinOP   (((_tgGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _tgGpioPinS    (((_tgGpio_t *)LOGI_ADDR_REG)->sel.pin)
#define _tgGpioPinD    (((_tgGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _tgGpioPinO    (((_tgGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _tgGpioPinI    (((_tgGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _tgGpioPinIP   (((_tgGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _tgGpioPinPl   (((_tgGpio_t *)LOGI_ADDR_REG)->pull.pin)
#define _tgGpioPinDs   (((_tgGpio_t *)LOGI_ADDR_REG)->ds.pin)

#define _tgGpioMapOP   (((_tgGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _tgGpioMapS    (((_tgGpio_t *)LOGI_ADDR_REG)->sel.map)
#define _tgGpioMapD    (((_tgGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _tgGpioMapO    (((_tgGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _tgGpioMapI    (((_tgGpio_t *)LOGI_ADDR_REG)->iv.map)
#define _tgGpioMapIP   (((_tgGpio_t *)LOGI_ADDR_REG)->iv.map)
#define _tgGpioMapPl   (((_tgGpio_t *)LOGI_ADDR_REG)->pull.map)
#define _tgGpioMapDs   (((_tgGpio_t *)LOGI_ADDR_REG)->ds.map)

/**< ------LMIGPIO------------------------------------*/
#define _lmiGpioPinOP  (((_lmiGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _lmiGpioPinS   (((_lmiGpio_t *)LOGI_ADDR_REG)->sel.pin)
#define _lmiGpioPinD   (((_lmiGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _lmiGpioPinO   (((_lmiGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _lmiGpioPinI   (((_lmiGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _lmiGpioPinPl  (((_lmiGpio_t *)LOGI_ADDR_REG)->pull.pin)
#define _lmiGpioPinDs  (((_lmiGpio_t *)LOGI_ADDR_REG)->ds.pin)
#define _lmiGpioMapOP  (((_lmiGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _lmiGpioMapS   (((_lmiGpio_t *)LOGI_ADDR_REG)->sel.map)
#define _lmiGpioMapD   (((_lmiGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _lmiGpioMapO   (((_lmiGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _lmiGpioMapI   (((_lmiGpio_t *)LOGI_ADDR_REG)->iv.map)
#define _lmiGpioMapPl  (((_lmiGpio_t *)LOGI_ADDR_REG)->pull.map)
#define _lmiGpioMapDs  (((_lmiGpio_t *)LOGI_ADDR_REG)->ds.map)

/**< ------DISPGPIO-----------------------------------*/
#define _dispGpioPinD  (((_dispGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _dispGpioPinO  (((_dispGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _dispGpioPinI  (((_dispGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _dispGpioPinS  (((_dispGpio_t *)LOGI_ADDR_REG)->sel.pin)
#define _dispGpioMapS  (((_dispGpio_t *)LOGI_ADDR_REG)->sel.map)
#define _dispGpioMapD  (((_dispGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _dispGpioMapO  (((_dispGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _dispGpioMapI  (((_dispGpio_t *)LOGI_ADDR_REG)->iv.map)

/**< ------POCUGPIO-----------------------------------*/
#define _pocuGpioPinI  (((_pocuGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _pocuGpioMapI  (((_pocuGpio_t *)LOGI_ADDR_REG)->iv.map)
#define _pocuGpioPinIlvl  (((_pocuGpio_t *)LOGI_ADDR_REG)->ilvl.pin)
#define _pocuGpioMapIlvl  (((_pocuGpio_t *)LOGI_ADDR_REG)->ilvl.map)

/**< ------USBGPIO------------------------------------*/
#define _usbGpioPinI   (((_usbGpio_t *)LOGI_ADDR_REG)->iv.pin)

/**< ------SARGPIO------------------------------------*/
#define _sarGpioPinSS  (((_sarGpio_t *)LOGI_ADDR_REG)->sarsel.pin)
#define _sarGpioPinR   (((_sarGpio_t *)LOGI_ADDR_REG)->ie.pin)
#define _sarGpioPinD   (((_sarGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _sarGpioPinO   (((_sarGpio_t *)LOGI_ADDR_REG)->ov.pin)
#define _sarGpioPinI   (((_sarGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _sarGpioMapSS  (((_sarGpio_t *)LOGI_ADDR_REG)->sarsel.map)
#define _sarGpioMapR   (((_sarGpio_t *)LOGI_ADDR_REG)->ie.map)
#define _sarGpioMapD   (((_sarGpio_t *)LOGI_ADDR_REG)->oe.map)
#define _sarGpioMapO   (((_sarGpio_t *)LOGI_ADDR_REG)->ov.map)
#define _sarGpioMapI   (((_sarGpio_t *)LOGI_ADDR_REG)->iv.map)

/**< ------LNGPIO-------------------------------------*/
#define _lnGpioPinR    (((_lnGpio_t *)LOGI_ADDR_REG)->op.pin.ie)
#define _lnGpioPinD    (((_lnGpio_t *)LOGI_ADDR_REG)->op.pin.oe)
#define _lnGpioPinO    (((_lnGpio_t *)LOGI_ADDR_REG)->op.pin.ov)
#define _lnGpioPinI    (((_lnGpio_t *)LOGI_ADDR_REG)->op.pin.iv)
#define _lnGpioMap     (((_lnGpio_t *)LOGI_ADDR_REG)->op.map)

/**< ------RGBGPIO0:25--------------------------------*/
#define _rgbGpioPinR   (((_rgbGpio_t *)LOGI_ADDR_REG)->ie.pin)
#define _rgbGpioPinI   (((_rgbGpio_t *)LOGI_ADDR_REG)->iv.pin)
#define _rgbGpioPinD   (((_rgbGpio_t *)LOGI_ADDR_REG)->oe.pin)
#define _rgbGpioPinO   (((_rgbGpio_t *)LOGI_ADDR_REG)->ov.pin)


/**
 * \def  XXXGPIO_PIN_XXX(pin)
 * \brief  GPIO direct access pin macros-----------
 *
 * \param pin  must be a decimal literal number to indicate pin index.
 * \note xxGPIOH_PIN_xxx(10...) is equivalent to xxGPIO_PIN_xxx(42...)
 * user may choose to use xxGPIO_PIN_xxx(42...) or xxGPIOH_PIN_xxx(10...) */

/**< ------GENGPIO------------------------------------*/
#define _GENGPIO_PIN_ASEL(n)  _CAT(_genGpioPinAS._pin_,n)
#define _GENGPIO_PIN_PSEL(n)  _CAT(_genGpioPinPS._pin_,n)
#define _GENGPIO_PIN_DIR(n)   _CAT(_genGpioPinD._pin_,n)
#define _GENGPIO_PIN_OUT(n)   _CAT(_genGpioPinO._pin_,n)
#define _GENGPIO_PIN_IN(n)    _CAT(_genGpioPinI._pin_,n)

/**< ------FMGPIO-------------------------------------*/
/* lcdsel[10:31] bit set 1 to direct DISPGPIO[18:39] to FMGPIO[32:45,24:31] */
/*.....*/
#define _FMGPIO_PIN_DIR(n)    _CAT(_fmGpioPinD._pin_,n)
#define _FMGPIO_PIN_OUT(n)    _CAT(_fmGpioPinO._pin_,n)
#define _FMGPIO_PIN_IN(n)     _CAT(_fmGpioPinI._pin_,n)
#define _FMGPIOH_PIN_DIR(n)   _CAT(_fmGpioHPinD._pin_,n)
#define _FMGPIOH_PIN_OUT(n)   _CAT(_fmGpioHPinO._pin_,n)
#define _FMGPIOH_PIN_IN(n)    _CAT(_fmGpioHPinI._pin_,n)
#define _FMGPIO_PIN_SEL(n)    _CAT(_fmGpioPinS._pin_,n)
#define _FMGPIOH_PIN_SEL(n)   _CAT(_fmGpioHPinS._pin_,n)

/**< ------TGGPIO-------------------------------------*/
/* spca6350 opol at MSB of ov */
/* spca6350 i2csel at MSB of sel */
/* spca6350 ipol at MSB of iv */
#define _TGGPIO_PIN_OPOL(n)   _CAT(_tgGpioPinOP.opol_pin_,n)
#define _TGGPIO_PIN_SEL(n)    _CAT(_tgGpioPinS._pin_,n)
#define _TGGPIO_PIN_DIR(n)    _CAT(_tgGpioPinD._pin_,n)
#define _TGGPIO_PIN_OUT(n)    _CAT(_tgGpioPinO._pin_,n)
#define _TGGPIO_PIN_IN(n)     _CAT(_tgGpioPinI._pin_,n)
#define _TGGPIO_PIN_IPOL(n)   _CAT(_tgGpioPinIP.ipol_pin_,n)
#define _TGGPIO_PIN_PULL(n)   _CAT(_tgGpioPinPl._pin_,n)
#define _TGGPIO_PIN_DS(n)     _CAT(_tgGpioPinDs._pin_,n)

/**< ------LMIGPIO------------------------------------*/
#define _LMIGPIO_PIN_OPOL(n)  _CAT(_lmiGpioPinOP.opol_pin_,n)
#define _LMIGPIO_PIN_SEL(n)   _CAT(_lmiGpioPinS._pin_,n)
#define _LMIGPIO_PIN_DIR(n)   _CAT(_lmiGpioPinD._pin_,n)
#define _LMIGPIO_PIN_OUT(n)   _CAT(_lmiGpioPinO._pin_,n)
#define _LMIGPIO_PIN_IN(n)    _CAT(_lmiGpioPinI._pin_,n)
#define _LMIGPIO_PIN_PULL(n)  _CAT(_lmiGpioPinPl._pin_,n)
#define _LMIGPIO_PIN_DS(n)    _CAT(_lmiGpioPinDs._pin_,n)

/**< ------DISPGPIO-----------------------------------*/
#define _DISPGPIO_PIN_DIR(n)  _CAT(_dispGpioPinD._pin_,n)
#define _DISPGPIO_PIN_OUT(n)  _CAT(_dispGpioPinO._pin_,n)
#define _DISPGPIO_PIN_IN(n)   _CAT(_dispGpioPinI._pin_,n)
#define _DISPGPIO_PIN_SEL(n)  _CAT(_dispGpioPinS._pin_,n)

/**< ------POCUGPIO-----------------------------------*/
#define _POCUGPIO_PIN_IN(n)   _CAT(_pocuGpioPinI._pin_,n)
#define _POCUGPIO_PIN_ILVL(n) _CAT(_pocuGpioPinIlvl._pin_,n)

/**< ------USBGPIO------------------------------------*/
#define _USBGPIO_PIN_IN(n)    _CAT(_usbGpioPinI._pin_,n)


/**< ------SARGPIO------------------------------------*/
#define _SARGPIO_PIN_SSEL(n)  _CAT(_sarGpioPinSS._pin_,n)
#define _SARGPIO_PIN_IEN(n)   _CAT(_sarGpioPinR._pin_,n)
#define _SARGPIO_PIN_OEN(n)   _CAT(_sarGpioPinD._pin_,n)
#define _SARGPIO_PIN_OUT(n)   _CAT(_sarGpioPinO._pin_,n)
#define _SARGPIO_PIN_IN(n)    _CAT(_sarGpioPinI._pin_,n)

/**< ------LNGPIO-------------------------------------*/
#define _LNGPIO_PIN_IEN(n)    _lnGpioPinR
#define _LNGPIO_PIN_OEN(n)    _lnGpioPinD
#define _LNGPIO_PIN_OUT(n)    _lnGpioPinO
#define _LNGPIO_PIN_IN(n)     _lnGpioPinI

/**< ------RGBGPIO0:25--------------------------------*/
#define _RGBGPIO_PIN_IEN(n)   _CAT(_rgbGpioPinR._pin_,n)
#define _RGBGPIO_PIN_IN(n)    _CAT(_rgbGpioPinI._pin_,n)
#define _RGBGPIO_PIN_OEN(n)   _CAT(_rgbGpioPinD._pin_,n)
#define _RGBGPIO_PIN_OUT(n)   _CAT(_rgbGpioPinO._pin_,n)



/**
 * \def  XXXGPIO_PIN_XXX(pin,...)
 * \def  XXXGPIO_MASK_XXX(mask,...)
 * \brief  GPIO config macros----------------------
 *
 * \param pin  must be a decimal literal number to indicate pin index.
 * \param mask 1 bits: corresponding bits are GPIO pins.
 * \param dir  0:GPIO in to sp5k, 1:GPIO out from sp5k.
 *
 * \note any pin config with these macros are used as GPIO.
 * function pins should config in related function modules,
 * not config with these macros. */

/**< ------GENGPIO------------------------------------*/
#define GENGPIO_PIN_WRITE(pin,val)   do { \
	_GENGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define GENGPIO_PIN_READ(pin)  _GENGPIO_PIN_IN(pin)

#define GENGPIO_MASK_WRITE(mask,valmap)   do { \
	_genGpioMapO = (_genGpioMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define GENGPIO_MASK_READ(mask)  (_genGpioMapI & (mask))

#define GENGPIO_MASK_SET(mask)   do { \
	_genGpioMapO |= (mask); \
	} while (0)

#define GENGPIO_MASK_CLR(mask)   do { \
	_genGpioMapO &= ~(mask); \
	} while (0)

/** GPIO pwmsel[0:5] correspoding bit 1:gpio[8,10,9,14,15,6] is pwm[0:5];
 * GLOBAL audsel[0:4] corresponding bit 1:gpio[8,10:13] is audio */
#define GENGPIO_PIN_CFG(pin,dir)   do { \
	if (pin==6 || (pin>=8 && pin<=10) || pin==14 || pin==15) \
		_GENGPIO_PIN_PSEL(pin) = 0; \
	if (pin==8 || (pin>=10 && pin<=13)) \
		_GENGPIO_PIN_ASEL(pin) = 0; \
	_GENGPIO_PIN_DIR(pin) = (dir); \
	_genGpioMode = 0; \
	} while (0)

/** GPIO pwmsel[0:5] correspoding bit 1:gpio[8,10,9,14,15,6] is pwm[0:5];
 * GLOBAL audsel[0:4] corresponding bit 1:gpio[8,10:13] is audio */
#define GENGPIO_MASK_SEL(mask,selmap)   do { \
	UINT32 setmap = (mask) & ~(selmap); \
	if (((mask) & ((_gpioMap32_t){ .pin = { \
		._pin_8 =1, ._pin_10=1, ._pin_9 =1, \
		._pin_14=1, ._pin_15=1, ._pin_6 =1, \
	} }).map) != 0) { \
		_gpioMap32_t gpioselclrmap = { .map = mask }; \
		_gpioMapPwmsel_t pwmselclrmap = { .pin = { \
			._pin_8  = gpioselclrmap.pin._pin_8  , \
			._pin_10 = gpioselclrmap.pin._pin_10 , \
			._pin_9  = gpioselclrmap.pin._pin_9  , \
			._pin_14 = gpioselclrmap.pin._pin_14 , \
			._pin_15 = gpioselclrmap.pin._pin_15 , \
			._pin_6  = gpioselclrmap.pin._pin_6  , \
		} }; \
		_gpioMap32_t gpioselsetmap = { .map = setmap }; \
		_gpioMapPwmsel_t pwmselsetmap = { .pin = { \
			._pin_8  = gpioselsetmap.pin._pin_8  , \
			._pin_10 = gpioselsetmap.pin._pin_10 , \
			._pin_9  = gpioselsetmap.pin._pin_9  , \
			._pin_14 = gpioselsetmap.pin._pin_14 , \
			._pin_15 = gpioselsetmap.pin._pin_15 , \
			._pin_6  = gpioselsetmap.pin._pin_6  , \
		} }; \
		_genGpioMapPS = (_genGpioMapPS & ~pwmselclrmap.map) \
				| pwmselsetmap.map; \
	} \
	if (((mask) & ((_gpioMap32_t){ .pin = { \
		._pin_8 =1, ._pin_10=1, ._pin_11 =1, ._pin_12=1, ._pin_13=1, \
	} }).map) != 0) { \
		UINT32 audselclrmap, audselsetmap; \
		audselclrmap = (((mask) >> 8) & 1) | (((mask) >> 9) & 0x1e); \
		audselsetmap = ((setmap >> 8) & 1) | ((setmap >> 9) & 0x1e); \
		_genGpioMapAS = (_genGpioMapAS & ~audselclrmap)|audselsetmap; \
	} \
	if (((mask) & (selmap)) != 0) _genGpioMode = 0; \
	} while (0)

#define GENGPIO_MASK_DIR(mask,dirmap)   do { \
	_genGpioMapD = (_genGpioMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define GENGPIO_MASK_CFG(mask,dirmap)   do { \
	if (((mask) & ((_gpioMap32_t){ .pin = { \
		._pin_8 =1, ._pin_10=1, ._pin_9 =1, \
		._pin_14=1, ._pin_15=1, ._pin_6 =1, \
	} }).map) != 0) { \
		_gpioMap32_t gpioselmap = { .map = (mask) }; \
		_gpioMapPwmsel_t pwmselmap = { .pin = { \
			._pin_8  = gpioselmap.pin._pin_8  , \
			._pin_10 = gpioselmap.pin._pin_10 , \
			._pin_9  = gpioselmap.pin._pin_9  , \
			._pin_14 = gpioselmap.pin._pin_14 , \
			._pin_15 = gpioselmap.pin._pin_15 , \
			._pin_6  = gpioselmap.pin._pin_6  , \
		} }; \
		_genGpioMapPS &= ~pwmselmap.map; \
	} \
	if (((mask) & ((_gpioMap32_t){ .pin = { \
		._pin_8 =1, ._pin_10=1, ._pin_11 =1, ._pin_12=1, ._pin_13=1, \
	} }).map) != 0) \
	_genGpioMapAS &= ~((((mask) >> 8) & 1) | (((mask) >> 9) & 0x1e)); \
	if ((mask) != 0) _genGpioMode = 0; \
	_genGpioMapD = (_genGpioMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

/**< ------FMGPIO-------------------------------------*/
/* lcdsel[10:31] bit set 1 to direct DISPGPIO[18:39] to FMGPIO[32:45,24:31] */
/*.....*/
#define FMGPIO_PIN_WRITE(pin,val)   do { \
	_FMGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define FMGPIO_PIN_READ(pin)  _FMGPIO_PIN_IN(pin)

#define FMGPIOH_PIN_WRITE(pin,val)   do { \
	_FMGPIOH_PIN_OUT(pin) = (val); \
	} while (0)

#define FMGPIOH_PIN_READ(pin)  _FMGPIOH_PIN_IN(pin)

#define FMGPIOL_MASK_WRITE(mask,valmap)   do { \
	_fmGpioLMapO = (_fmGpioLMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define FMGPIOL_MASK_READ(mask)  (_fmGpioLMapI & (mask))

#define FMGPIOL_MASK_SET(mask)   do { \
	_fmGpioLMapO |= (mask); \
	} while (0)

#define FMGPIOL_MASK_CLR(mask)   do { \
	_fmGpioLMapO &= ~(mask); \
	} while (0)

#define FMGPIOH_MASK_WRITE(mask,valmap)   do { \
	_fmGpioHMapO = (_fmGpioHMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define FMGPIOH_MASK_READ(mask)  (_fmGpioHMapI & (mask))

#define FMGPIOH_MASK_SET(mask)   do { \
	_fmGpioHMapO |= (mask); \
	} while (0)

#define FMGPIOH_MASK_CLR(mask)   do { \
	_fmGpioHMapO &= ~(mask); \
	} while (0)

#define FMGPIO_PIN_CFG(pin,dir)   do { \
	_FMGPIO_PIN_SEL(pin) = 1; \
	_FMGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define FMGPIOH_PIN_CFG(pin,dir)   do { \
	_FMGPIOH_PIN_SEL(pin) = 1; \
	_FMGPIOH_PIN_DIR(pin) = (dir); \
	} while (0)

#define FMGPIOL_MASK_CFG(mask,dirmap)   do { \
	_fmGpioLMapS |= (mask); \
	_fmGpioLMapD = (_fmGpioLMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define FMGPIOH_MASK_CFG(mask,dirmap)   do { \
	_fmGpioHMapS |= (mask); \
	_fmGpioHMapD = (_fmGpioHMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

/**< ------TGGPIO-------------------------------------*/
/* spca6350 tggposel design changed to 1:function, 0:gpio */
#define _TGGPIO_PIN_SEL_GPIO   0
#define _TGGPIO_PIN_SEL_FUNC   1
#define _TGGPIO_PIN_DIR_IN     0
#define _TGGPIO_PIN_DIR_OUT    1

#define TGGPIO_PIN_WRITE(pin,val)   do { \
	_TGGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define TGGPIO_PIN_READ(pin)  _TGGPIO_PIN_IN(pin)

/* spca6350 tggposel design changed to 1:function, 0:gpio */
#define TGGPIO_PIN_SEL(pin,sel)   do { \
	_TGGPIO_PIN_SEL(pin) = (sel); \
	} while (0)

#define TGGPIO_PIN_DIR(pin,dir)   do { \
	_TGGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define TGGPIO_PIN_CFG(pin,dir)   do { \
	_TGGPIO_PIN_SEL(pin) = _TGGPIO_PIN_SEL_GPIO; \
	_TGGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define TGGPIO_PIN_OPOL(pin,val)   do { \
	_TGGPIO_PIN_OPOL(pin) = (val); \
	} while (0)

#define TGGPIO_PIN_IPOL(pin,val)   do { \
	_TGGPIO_PIN_IPOL(pin) = (val); \
	} while (0)

#define TGGPIO_PIN_PULL(pin,val)   do { \
	_TGGPIO_PIN_PULL(pin) = (val); \
	} while (0)

#define TGGPIO_PIN_DRIVE(pin,val)   do { \
	_TGGPIO_PIN_DS(pin) = (val); \
	} while (0)

/* spca6350 tggposel design changed to 1:function, 0:gpio */
/* spca6350 opol at MSB of ov */
/* spca6350 i2csel at MSB of sel */
/* spca6350 ipol at MSB of iv */
#define _TGGPIO_EFFECT_MASK  0x0fffff  /* bit[0:19] */

#if _TGGPIO_PIN_SEL_GPIO == 0
#define TGGPIO_MASK_SEL(mask,selmap)   do { \
	UINT32 emask = (mask) & _TGGPIO_EFFECT_MASK; \
	_tgGpioMapS = (_tgGpioMapS & ~(emask)) | (~(selmap) & (emask)); \
	} while (0)

#define TGGPIO_MASK_CFG(mask,dirmap)   do { \
	UINT32 emask = (mask) & _TGGPIO_EFFECT_MASK; \
	_tgGpioMapS &= ~(emask); \
	_tgGpioMapD = (_tgGpioMapD & ~(emask)) | ((dirmap) & (emask)); \
	} while (0)
#else
#define TGGPIO_MASK_SEL(mask,selmap)   do { \
	UINT32 emask = (mask) & _TGGPIO_EFFECT_MASK; \
	_tgGpioMapS = (_tgGpioMapS & ~(emask)) | ((selmap) & (emask)); \
	} while (0)

#define TGGPIO_MASK_CFG(mask,dirmap)   do { \
	UINT32 emask = (mask) & _TGGPIO_EFFECT_MASK; \
	_tgGpioMapS |= (emask); \
	_tgGpioMapD = (_tgGpioMapD & ~(emask)) | ((dirmap) & (emask)); \
	} while (0)
#endif

#define TGGPIO_MASK_DIR(mask,dirmap)   do { \
	_tgGpioMapD = (_tgGpioMapD & ~(emask)) | ((dirmap) & (emask)); \
	} while (0)

#define TGGPIO_MASK_WRITE(mask,valmap)   do { \
	UINT32 emask = (mask) & _TGGPIO_EFFECT_MASK; \
	_tgGpioMapO = (_tgGpioMapO & ~(emask)) | ((valmap) & (emask)); \
	} while (0)

#define TGGPIO_MASK_READ(mask)  (_tgGpioMapI & (mask) & _TGGPIO_EFFECT_MASK)

#define TGGPIO_MASK_SET(mask)   do { \
	_tgGpioMapO |= (mask) & _TGGPIO_EFFECT_MASK; \
	} while (0)

#define TGGPIO_MASK_CLR(mask)   do { \
	_tgGpioMapO &= ~((mask) & _TGGPIO_EFFECT_MASK); \
	} while (0)

#define TGGPIO_MASK_OPOL(mask,opolmap)   do { \
	if (((mask) & ((_gpioMap32_t){ .pin = { \
		._pin_3 =1, ._pin_4 =1, ._pin_8 =1, \
		._pin_10=1, ._pin_11=1, ._pin_12=1, ._pin_19=1, \
	} }).map) != 0) { \
		_gpioMap32_t gpioopolclrmap = { .map = mask }; \
		_gpioMapTgOv_t tgopolclrmap = { .pin = { \
			.opol_pin_3  = gpioopolclrmap.pin._pin_3  , \
			.opol_pin_4  = gpioopolclrmap.pin._pin_4  , \
			.opol_pin_8  = gpioopolclrmap.pin._pin_8  , \
			.opol_pin_10 = gpioopolclrmap.pin._pin_10 , \
			.opol_pin_11 = gpioopolclrmap.pin._pin_11 , \
			.opol_pin_12 = gpioopolclrmap.pin._pin_12 , \
			.opol_pin_19 = gpioopolclrmap.pin._pin_19 , \
		} }; \
		_gpioMap32_t gpioopolsetmap = { .map = (mask) & (opolmap) }; \
		_gpioMapTgOv_t tgopolsetmap = { .pin = { \
			.opol_pin_3  = gpioopolsetmap.pin._pin_3  , \
			.opol_pin_4  = gpioopolsetmap.pin._pin_4  , \
			.opol_pin_8  = gpioopolsetmap.pin._pin_8  , \
			.opol_pin_10 = gpioopolsetmap.pin._pin_10 , \
			.opol_pin_11 = gpioopolsetmap.pin._pin_11 , \
			.opol_pin_12 = gpioopolsetmap.pin._pin_12 , \
			.opol_pin_19 = gpioopolsetmap.pin._pin_19 , \
		} }; \
		_tgGpioMapOP = (_tgGpioMapOP & ~tgopolclrmap.map) \
				| tgopolsetmap.map; \
	} \
	} while (0)

#define TGGPIO_MASK_IPOL(mask,ipolmap)   do { \
	if (((mask) & ((_gpioMap32_t){ .pin = { \
		._pin_9 =1, ._pin_10=1, ._pin_11=1, \
	} }).map) != 0) { \
		_gpioMap32_t gpioipolclrmap = { .map = mask }; \
		_gpioMapTgIv_t tgipolclrmap = { .pin = { \
			.ipol_pin_9  = gpioipolclrmap.pin._pin_9  , \
			.ipol_pin_10 = gpioipolclrmap.pin._pin_10 , \
			.ipol_pin_11 = gpioipolclrmap.pin._pin_11 , \
		} }; \
		_gpioMap32_t gpioipolsetmap = { .map = (mask) & (ipolmap) }; \
		_gpioMapTgIv_t tgipolsetmap = { .pin = { \
			.ipol_pin_9  = gpioipolsetmap.pin._pin_9  , \
			.ipol_pin_10 = gpioipolsetmap.pin._pin_10 , \
			.ipol_pin_11 = gpioipolsetmap.pin._pin_11 , \
		} }; \
		_tgGpioMapIP = (_tgGpioMapIP & ~tgipolclrmap.map) \
				| tgipolsetmap.map; \
	} \
	} while (0)

#define TGGPIO_MASK_PULL(mask,pullmap)   do { \
	_tgGpioMapPl = (_tgGpioMapPl & ~(mask)) | ((pullmap) & (mask)); \
	} while (0)

#define TGGPIO_MASKL_DRIVE(mask,dsmap)   do { \
	_tgGpioMapDs[0] = (_tgGpioMapDs[0] & ~(mask)) | ((dsmap) & (mask)); \
	} while (0)

#define TGGPIO_MASKH_DRIVE(mask,dsmap)   do { \
	_tgGpioMapDs[1] = (_tgGpioMapDs[1] & ~(mask)) | ((dsmap) & (mask)); \
	} while (0)

/**< ------LMIGPIO------------------------------------*/
#define _LMIGPIO_PIN_SEL_GPIO   0

#define LMIGPIO_PIN_WRITE(pin,val)   do { \
	_LMIGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define LMIGPIO_PIN_READ(pin)  _LMIGPIO_PIN_IN(pin)

#define LMIGPIO_PIN_SEL(pin,sel)   do { \
	if ((pin) < 16) _LMIGPIO_PIN_SEL(pin) = (sel); \
	} while (0)

#define LMIGPIO_PIN_DIR(pin,dir)   do { \
	_LMIGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define LMIGPIO_PIN_CFG(pin,dir)   do { \
	if ((pin) < 16) _LMIGPIO_PIN_SEL(pin) = _LMIGPIO_PIN_SEL_GPIO; \
	_LMIGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define _LMIGPIO_EFFECT_MASK  0x3fffff  /* bit[0:21] */

#define LMIGPIO_MASK_CFG(mask,dirmap)   do { \
	dmask.... duo bits..... \
	_lmiGpioMapS |= (dmask); \
	_lmiGpioMapD = (_lmiGpioMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define LMIGPIO_MASK_WRITE(mask,valmap)   do { \
	UINT32 emask = (mask) & _LMIGPIO_EFFECT_MASK; \
	_lmiGpioMapO = (_lmiGpioMapO & ~(emask)) | ((valmap) & (emask)); \
	} while (0)

#ifdef FPGA
#define LMIGPIO_MASK_READ(mask)  (_lmiGpioMapI & (mask) & _LMIGPIO_EFFECT_MASK)
#else  /*#ifdef FPGA*/
#define LMIGPIO_MASK_READ(mask)  (_lmiGpioMapI & (mask))
#endif /*#ifdef FPGA*/

#define LMIGPIO_MASK_SET(mask)   do { \
	_lmiGpioMapO |= (mask) & _LMIGPIO_EFFECT_MASK; \
	} while (0)

#define LMIGPIO_MASK_CLR(mask)   do { \
	_lmiGpioMapO &= ~(mask) & _LMIGPIO_EFFECT_MASK; \
	} while (0)

/**< ------DISPGPIO-------------------------------------*/
#define DISPGPIO_PIN_WRITE(pin,val)   do { \
	_DISPGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define DISPGPIO_PIN_READ(pin)  _DISPGPIO_PIN_IN(pin)

#define DISPGPIO_PIN_CFG(pin,dir)   do { \
	_DISPGPIO_PIN_SEL(pin) = 1; \
	_DISPGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

#define DISPGPIO_MASK_CFG(mask,dirmap)   do { \
	_dispGpioMapS |= (mask); \
	_dispGpioMapD = (_dispGpioMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define DISPGPIO_MASK_WRITE(mask,valmap)   do { \
	_dispGpioMapO = (_dispGpioMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define DISPGPIO_MASK_READ(mask)  (_dispGpioMapI & (mask))

#define DISPGPIO_MASK_SET(mask)   do { \
	_dispGpioMapO |= (mask); \
	} while (0)

#define DISPGPIO_MASK_CLR(mask)   do { \
	_dispGpioMapO &= ~(mask); \
	} while (0)

/**< ------POCUGPIO-------------------------------------*/
#define POCUGPIO_PIN_READ(pin)  _POCUGPIO_PIN_IN(pin)
#define POCUGPIO_MASK_READ(mask)  (_pocuGpioMapI & (mask))
#define POCUGPIO_PIN_RLVL(pin)  _POCUGPIO_PIN_ILVL(pin)
#define POCUGPIO_MASK_RLVL(mask)  (_pocuGpioMapIlvl & (mask))

/**< ------SARGPIO--------------------------------------*/
#define SARGPIO_PIN_WRITE(pin,val)   do { \
	_SARGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define SARGPIO_PIN_READ(pin)  _SARGPIO_PIN_IN(pin)

/** SARGPIO sarsel[0:4] corresponding bit 1: sargpio[4,6:9] for audio */
#define SARGPIO_PIN_CFG(pin,dir)   do { \
	if (pin==4 || (pin>=6 && pin<=9)) \
		_SARGPIO_PIN_SSEL(pin) = 0; \
	_SARGPIO_PIN_IEN(pin) = !(dir); \
	_SARGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

/** SARGPIO sarsel[0:4] corresponding bit 1: sargpio[4,6:9] for audio */
#define SARGPIO_MASK_CFG(mask,dirmap)   do { \
	_sarGpioMapSS &= ~((((mask) >> 4) & 1) | (((mask) >> 5) & 0x1e)); \
	_sarGpioMapR = (_sarGpioMapR & ~(mask)) | (~(dirmap) & (mask)); \
	_sarGpioMapD = (_sarGpioMapD & ~(mask)) | ((dirmap) & (mask)); \
	} while (0)

#define SARGPIO_MASK_WRITE(mask,valmap)   do { \
	_sarGpioMapO = (_sarGpioMapO & ~(mask)) | ((valmap) & (mask)); \
	} while (0)

#define SARGPIO_MASK_READ(mask)  (_sarGpioMapI & (mask))

#define SARGPIO_MASK_SET(mask)   do { \
	_sarGpioMapO |= (mask); \
	} while (0)

#define SARGPIO_MASK_CLR(mask)   do { \
	_sarGpioMapO &= ~(mask); \
	} while (0)

/**< ------LNGPIO---------------------------------------*/
#define LNGPIO_PIN_WRITE(pin,val)   do { \
	_LNGPIO_PIN_OUT(pin) = (val); \
	} while (0)

#define LNGPIO_PIN_READ(pin)  _LNGPIO_PIN_IN(pin)

/*#define LNGPIO_PIN_CFG(pin,dir)   do { \
	.... \
	_LNGPIO_PIN_IEN(pin) = !(dir); \
	_LNGPIO_PIN_DIR(pin) = (dir); \
	} while (0)

....*/
#define OISGPIO_MASK_CFG(mask,dirmap)   do { \
	_oisGpioMapR = (_oisGpioMapR & ~((mask) << 4)) | ((~(dirmap) & (mask)) << 4); \
	_oisGpioMapD = (_oisGpioMapD & ~((mask) << 4)) | (((dirmap) & (mask)) << 4); \
	} while (0)

#define OISGPIO_MASK_WRITE(mask,valmap)   do { \
	_oisGpioMapO = (_oisGpioMapO & ~((mask) << 4)) | (((valmap) & (mask)) << 4); \
	} while (0)

#define LNGPIO_MASK_READ(mask)  (_lnGpioPinI & (mask))

#define OISGPIO_MASK_SET(mask)   do { \
	_oisGpioMapO |= ((mask) << 4); \
	} while (0)

#define OISGPIO_MASK_CLR(mask)   do { \
	_oisGpioMapO &= ~((mask) << 4); \
	} while (0)

/**< ------RGBGPIO0:25--------------------------------*/
/**TODO*.....*/


#ifdef __cplusplus
}
#endif

#endif /*_GPIO_DEF_H_*/
