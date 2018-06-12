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
#ifndef _TSR_DEF_H_
#define _TSR_DEF_H_

/**************************************************************************
 *                            H E A D E R   F I L E S
 **************************************************************************/
#include "common.h"
#include "middleware/modesw_def.h"

/**************************************************************************
 *                              C O N S T A N T S
 **************************************************************************/
typedef enum {
	TSR_RESULT_SHOW_TIME,
	TSR_CENTER_Y,

	/*the following for get only*/
	TSR_RESULT_GET = 0x1000,
	TSR_CFG_GET = 0x2000,
} TSR_CfgSel_t;
/**************************************************************************
 *                                  M A C R O S
 **************************************************************************/

/**************************************************************************
 *                              D A T A   T Y P E S
 **************************************************************************/
typedef struct {
	UINT32 dw0,dw1,dw2,dw3,dw4,dw5,dw6,dw7,dw8,dw9,dw10,dw11
		#if SPCA5330
		,
		dw12,
		dw13,
		dw14,
		dw15
		#endif
		;
}SD_moeQue_t;

typedef struct {
	SINT32 min;	/*!< 2's complement */
	SINT32 max; /*!< 2's complement */
} SD_MoeClipCfg_t;

typedef struct {
	SINT16 para0;	/*!< 1S10bit */
	SINT16 para1;	/*!< 1S10bit */
	SINT16 para2;	/*!< 1S10bit */
	SINT16 para3;	/*!< 1S10bit */
	SINT16 para4;	/*!< 1S10bit */
	SINT16 para5;	/*!< 1S10bit */
	SINT16 para6;	/*!< 1S10bit */
	SINT16 para7;	/*!< 1S10bit */
	SINT16 para8;	/*!< 1S10bit */
	SINT16 para9;	/*!< 1S10bit */
} SD_MoeFilterObj_t;
typedef struct {
	UINT8 topMrrPixel;/*!< 0 ~ 3 */
	UINT8 topMrrEn;/*!< 0 or 1 */
	UINT8 botMrrPixel;/*!< 0 ~ 3 */
	UINT8 botMrrEn;/*!< 0 or 1 */
	UINT8 leftMrrPixel;/*!< 0 ~ 3 */
	UINT8 leftMrrEn;/*!< 0 or 1 */
	UINT8 rightMrrPixel;/*!< 0 ~ 3 */
	UINT8 rightMrrEn;/*!< 0 or 1 */
	UINT8 reserved;/*!< should be 0 */
} SD_MoeMirrorCfg_t;

typedef struct {
	void *pbuf;
	UINT16 width;	/*!< max 8184, multiple of 8 */
	UINT16 height;	/*!< max 4095 */
	UINT16 x;		/*!< max width-1 */
	UINT16 y;		/*!< max height-1 */
	UINT8 fmt;		/*!< MoeDataFmt_t */
	UINT8 sign;		/*!< MoeDataType_t */
	UINT8 shift;	/*!< max 16, left shift for src, right shift for dst */ 
	UINT8 abs;		/*!< see MoeDataAbs_t */
} SD_MoeObj_t;

typedef struct {
	UINT16 common;
	UINT8 sign;		/*!< MoeDataType_t */
	UINT8 shift;	/*!< left shift */
} SD_MoeComObj_t;

typedef struct tsrCfgGet_s{
	UINT32 result_show_time;
	UINT32 center_y;
}tsrCfgGet_t;

/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
typedef modeswFrameBuf_t tsrFrameBuf_t;

#endif
