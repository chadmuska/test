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
#ifndef _APP_FLASH_PARAM_H_
#define _APP_FLASH_PARAM_H_

#define PREFH_EXIF_DEBUG	1	/*-- better to be 0 @ official release */

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

#include "customization/dev_init.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define APP_PREFLASH_PCLK				36		/* MHz */
#define APP_PREFLASH_LINETOTAL  		1714
#define APP_PREFLASH_WIDTH_MAX			13000
#define APP_PREFLASH_POS_MAX			239
#define APP_PREFLASH_POS_NULL			100	/*-- the pos of in-effective */
#define APP_PREFLASH_DEFAULT_WIDTH		430	/*-- preflash trigger pos */
#define APP_PREFLASH_DIM_WIDTH			600	/*-- preflash pulse width @ dim scene */
#define APP_PREFLASH_DEFAULT_POS		238	/*-- preflash trigger pos */
#define APP_SNAPFLASH_DEFAULT_POS		350	/*-- snap flash trigger pos */

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* _APP_FLASH_PARAM_H_ */


