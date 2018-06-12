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
#ifndef APP_HW_EVB_DEF_H
#define APP_HW_EVB_DEF_H

#if defined(HW_19DVR)
#include "customization/dev_init.h"

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                           EVB_SPE7001 HW		                          *
 **************************************************************************/

#define ADC_BATTERY_CH  11 /* should be no used*/

/* Audio Setting */
#define AUD_PLAY_SRC_FOR_LCD	SP5K_AUD_DEV_PLAY_SRC_SPEAKER
#define AUD_PLAY_SRC_FOR_TV		SP5K_AUD_DEV_PLAY_SRC_LINEOUT
#define AUD_REC_SRC				SP5K_AUD_DEV_REC_SRC_MIC_IN | SP5K_AUD_REC_CH_LEFT


#endif /* _EVB_HW_ */

#endif  /* APP_HW_EVB_DEF_H */


