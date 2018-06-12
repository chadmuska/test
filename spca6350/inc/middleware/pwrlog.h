/**************************************************************************
 *
 *       Copyright (c) 2004-2016 by iCatch Technology, Inc.
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
 *                                                                        *
 *  Author: Anmin Deng                                                    *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */

#ifndef _PWRLOG_H_
#define _PWRLOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "os/os_bmap.h"

#define PWR2_DOMAIN_ERR (-1L)
#define PWR2_OVERFLOW 0x7fffffffL
#define LOG2_DOMAIN_ERR 0x80000000L


/**
 * \return how many bits are 1 in the 32-bit wide integer t
 */
UINT32 u32bitOneCount(UINT32 t);

/**
 * \return value of power(2, x), where x is an unsigned integer, and
 * power(2, x) is the smallest possible value which >= t,
 * undefined if t > 0x80000000
 */
UINT32 u32bitRoundUpPower2(UINT32 t);

void u32mulOverflowShrink(UINT32 x,UINT32 y,UINT32 *product,UINT32 *shrink);

/**
 * \brief rational base 2 power, where
 * *pwr2X / *pwr2Y := power(2, num/denom)
 * \return 0: success; PWR2_DOMAIN_ERR: denom==0 not supported;
 *  PWR2_OVERFLOW: result range *pwr2X, *pwr2Y not fits in UINT32;
 */
SINT32 power2xyND(SINT32 num,SINT32 denom,UINT32 *pwr2X,UINT32 *pwr2Y);

/**
 * \brief rational base 2 logarithm, where
 * *log2X / *log2Y := log(2, num/denom)
 * \return 0: success; LOG2_DOMAIN_ERR: denom==0 or num/denom <= 0;
 */
SINT32 log2xyND(SINT32 num,SINT32 denom,SINT32 *log2X,SINT32 *log2Y);

/**
 * \return integer square root sqrt(s)
 * \param precise, 0: rough value (< +-0.5% error), faster; 1: the most
 * accurate but slower
 */
UINT32 u32sqrt(UINT32 s, UINT32 precise);


#define ARCTAN_PI_MAP  512

/**
 * \return ARCTAN_PI_MAP * arctan(x / y) / PI, range 0 .. 2*ARCTAN_PI_MAP
 */
UINT32 xyArctanPiMap512(SINT32 x, SINT32 y);


#define SINCOS_UNIT_BITS  12
#define SINCOS_UNIT       (1 << SINCOS_UNIT_BITS)
#define SINCOS_PI_MAP     512

/**
 * \param x, the angle, in radius / PI * SINCOS_PI_MAP;
 * \return SINCOS_UNIT * sin(PI * x / SINCOS_PI_MAP), range +- 0 .. SINCOS_UNIT
 */
int fixed_sin(int x);

/**
 * \param x, the angle, in radius / PI * SINCOS_PI_MAP;
 * \return SINCOS_UNIT * cos(PI * x / SINCOS_PI_MAP), range +- 0 .. SINCOS_UNIT
 */
int fixed_cos(int x);


#ifdef __cplusplus
}
#endif

#endif
