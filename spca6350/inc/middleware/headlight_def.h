/**************************************************************************
 *
 *        Copyright (c) 2008 by Sunplus mMedia Inc., Ltd.
 *
 *  This software is copyrighted by and is the property of Sunplus
 *  mMedia Inc., Ltd. All rights are reserved by Sunplus mMedia
 *  Inc., Ltd. This software may only be used in accordance with the
 *  corresponding license agreement. Any unauthorized use, duplication,
 *  distribution, or disclosure of this software is expressly forbidden.
 *
 *  This Copyright notice MUST not be removed or modified without prior
 *  written consent of Sunplus mMedia Inc., Ltd.
 *
 *  Sunplus mMedia Inc., Ltd. reserves the right to modify this
 *  software without notice.
 *
 *  Sunplus mMedia Inc., Ltd.
 *  19-1, Innovation First Road, Science-Based Industrial Park,
 *  Hsin-Chu, Taiwan.
 *
 **************************************************************************/
#ifndef _HEADLIGHT_DEF_H_
#define _HEADLIGHT_DEF_H_

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/modesw_def.h"
#include "adas_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
typedef enum{
	HEADLIGHT_CFG_SET1,

	/*the following for get only*/
	HEADLIGHT_RESULT_GET = 0x1000,
	HEADLIGHT_CFG_GET = 0x2000,
}HeadLight_CfgSel_t;
/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct hlResult_s {
	UINT32 HL_flag;
}hlResult_t;


/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
typedef modeswFrameBuf_t HeadLineFrameBuf_t;

#endif
