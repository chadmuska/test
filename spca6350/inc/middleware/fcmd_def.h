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
#ifndef _FCMD_DEF_H_
#define _FCMD_DEF_H_

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/modesw_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
typedef enum {
	FCMD_FW_CARREMOVE_FLAG,

	/*the following for get only*/
	FCMD_RESULT_GET = 0x1000,
	FCMD_CFG_GET = 0x2000,
} FCMD_CfgSel_t;
/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct fcmdResult_s {
	UINT32 fcmd_flag;
}fcmdResult_t;

typedef struct fcmdCfgGet_s{
	UINT32 fw_carRemove_flag;
}fcmdCfgGet_t;

/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
typedef modeswFrameBuf_t fcmdFrameBuf_t;

#endif
