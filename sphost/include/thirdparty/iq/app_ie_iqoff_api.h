/**************************************************************************
 *
 *  Copyright (c) 2015 by iCatch Technology Co., Ltd.
 *
 *  This software is copyrighted by and is the property of iCatch Technology Co., Ltd.
 *  All rights are reserved by iCatch Technology Co., Ltd.
 *  This software may only be used in accordance with the corresponding license
 *  agreement. Any unauthorized use, duplication, distribution, or disclosure of this
 *  software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of iCatch Technology Co., Ltd.
 *
 *  iCatch Technology Co., Ltd. reserves the right to modify this software without notice.
 *
 *  iCatch Technology Co., Ltd.
 *  19, Innovation First Road, Science-Based Industrial Park, Hsin-Chu, Taiwan, R.O.C.
 *
 *  Author: Xavier Lee
 *
 *  2015.08.14 added Negative, Warm, Cold, Red, Green, Blue,
 *
 *  2015.10.14 added BrightnessContrast, Hue.
 **************************************************************************/

#ifndef APP_IE_IQOFF_APIH
#define APP_IE_IQOFF_APIH

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
enum
{
	UI_EFFECT_IQOFF_NORMAL,
	UI_EFFECT_IQOFF_BW,
    UI_EFFECT_IQOFF_SEPIA,
    UI_EFFECT_IQOFF_VIVID,
    UI_EFFECT_IQOFF_NATURAL,
    UI_EFFECT_IQOFF_WARM,
    UI_EFFECT_IQOFF_COLD,
    UI_EFFECT_IQOFF_RED,
    UI_EFFECT_IQOFF_GREEN,
    UI_EFFECT_IQOFF_BLUE,
    UI_EFFECT_IQOFF_BRIGHTNESS_CONTRAST,
    UI_EFFECT_IQOFF_HUE,
};

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
 void appIqOffEffectSet(UINT32 value, ...);

#endif

