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
 *  Author: Potato Chang
 *
 *  2015.08.14 added Negative, Warm, Cold, Red, Green, Blue,
 *
 *  2015.10.14 added BrightnessContrast, Hue.
 *
 *  2017.06.16 added Sketch mode.
 *
 *  2017.07.03 added effect mode get.
 *
 *  2017.07.19 added negative yuv mode.
 **************************************************************************/

#ifndef APP_IE_APIH
#define APP_IE_APIH

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
enum
{
    UI_EFFECT_NORMAL,
    UI_EFFECT_BW,
    UI_EFFECT_SEPIA,
    UI_EFFECT_VIVID,
    UI_EFFECT_NATURAL,
    UI_EFFECT_NEGATIVE,
    UI_EFFECT_WARM,
    UI_EFFECT_COLD,
    UI_EFFECT_RED,
    UI_EFFECT_GREEN,
    UI_EFFECT_BLUE,
    UI_EFFECT_BRIGHTNESS_CONTRAST,
    UI_EFFECT_HUE,
    UI_EFFECT_SKETCH,
    UI_EFFECT_NEGATIVE_YUV,
};

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void appIqEffectSet(UINT32 value, ...);

UINT32 appIqEffectGet();

#endif

