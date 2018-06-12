/**************************************************************************
 *
 *        Copyright (c) 2006-2011 by Sunplus mMedia Inc., Ltd.
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
 *  Hsin-Chu, Taiwan, R.O.C. 
 *
 **************************************************************************/
#ifndef APP_ICATCH_AE_LIB_H
#define APP_ICATCH_AE_LIB_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

typedef struct
{
	UINT8  region_num;				//for txt file
	UINT32 HighLightCnt;
	UINT32 LowLightCnt;
	UINT32 AERegionLuma[64];
	UINT32 WeightLuma[64];
	UINT32 DnyAERegionLuma[64];
	UINT32 frame_id;				//for stamp
	SINT16 LVx1000;	
	UINT32 aeLuma;
	UINT16 Target;
	UINT16 flow;
	UINT16 NewEvValuex1000;
	UINT8  S_gamma;
}aeDebugInfo_t;

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/

#endif  /* APP_ICATCH_AE_LIB_H */

