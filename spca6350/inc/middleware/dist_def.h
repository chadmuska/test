/**************************************************************************
 *
 *       Copyright (c) 2002-2016 by iCatch Technology, Inc.
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
 *  Author: Ymkao
 *
 ******************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */


#ifndef _DIST_DEF_H_
#define _DIST_DEF_H_

/*******************************************************************************
 *                           C O N S T A N T S
 ******************************************************************************/

typedef enum {
	STILL_DIST_EW_BLOCKS_H = 0x00,
	STILL_DIST_EW_BLOCKS_V,
	STILL_DIST_EW_THRESHOLD_H,
	STILL_DIST_EW_THRESHOLD_V,
	STILL_DIST_ME_BLOCKS_THRESHOLD = 0x10,
	STILL_DIST_GMV_INFO_LENGTH = 0x20,
	STILL_DIST_EXP_ISO_STEP = 0x30,
	STILL_DIST_ENV_MODE = 0x40,
} stillDistSel_t;

/*******************************************************************************
 *                              M A C R O S
 ******************************************************************************/

/*******************************************************************************
 *                          D A T A    T Y P E S
 ******************************************************************************/
/** @brief Gmv Related Information */
typedef struct stillDistGmvInfo_s {
	UINT32 frameId;      /*<! the frame id of this gmv pair */
	SINT16 gmvx, gmvy;   /*<! the x, y gmv value */
	SINT16 gmvx2, gmvy2; /*<! the x, y gmv value */
	UINT8 numx, numy;    /*<! number of blocks that have x, y movement */
} stillDistGmvInfo_t;

/** @brief Customer Cb - dist compensate offset calculate information */
typedef struct {
	SINT32 rscx,rscy;     /* <!returned RSC value */
	SINT32 curx,cury;     /* <!current DIST postion (read only) */
	SINT32 rangex,rangey; /* <!valid curx & cury range is [-rangex, rangex] & [-rangey, rangey] (read only) */
} distCompensateCalcCbEx_t;

/** @brief Customer Cb - dist GMV calculate information */
typedef struct {
	UINT16 x,y;           /* <!motion vector of x, motion vector of y */
	UINT8 rotate, rsv;    /* <!rotation state, reserve */
} distGmvCalcCbEx_t;



/*******************************************************************************
 *                         G L O B A L    D A T A
 ******************************************************************************/

/*******************************************************************************
 *                  E X T E R N A L    R E F E R E N C E
 ******************************************************************************/

/*******************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 ******************************************************************************/

#endif /* _DIST_DEF_H_ */


