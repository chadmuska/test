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
 *  Author: Richie Lee
 *
 ******************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 26855 $
$Date:: 2016-08-08 19:03:23#$
 */

#ifndef _SNAP_DEF_H_
#define _SNAP_DEF_H_

/*******************************************************************************
 *                           C O N S T A N T S
 ******************************************************************************/

#define SNAP_FAKE_FIELD_NAUM	 6
#define JPG_UNDERFLOW_RECONDE  1 /* 0: not reencode if vlc is underflow; 1: encode again if vlc is underflow */
#define ULTRA_SNAP_SAVE_YUV	0
#define SNAP_REDO_BUFF_SHARE_WITH_YUV_BUFF 1


/* RAW callback return value */
#define SNAP_RAW_CB_RET_REDO_LSC 		( 1 << 0 )
#define SNAP_RAW_CB_RET_REDO_WB_OFFSET	( 1 << 1 )
#define SNAP_RAW_CB_RET_REDO_WB_PREGAIN	( 1 << 2 )



/* YUV callback return value */
#define SNAP_YUV_CB_RET_SKIP_FLOW ( 1 << 0 ) /* the snap flow will be skipped if host return this value in yuv callback */
#define SNAP_YUV_CB_RET_LOCK_BUF ( 1 << 1 ) /* force to not return raw buffer yuv callback */
#define SNAP_YUV_CB_RET_UNLOCK_ALL_BUF ( 1 << 2 ) /*return all locked raw buffers */
#define SNAP_YUV_CB_RET_CDSP_REDO ( 1 << 3 ) /*return all locked raw buffers */
#define SNAP_YUV_CB_RET_WAIT_FOR_HOST ( 1 << 4 ) /* wait for host for next step */
#define SNAP_YUV_CB_RET_UNLOCK_SPECIFIC_BUF ( 1 << 5 ) /* return specific locked buffer */
#define SNAP_YUV_CB_RET_REDO_SPECIFIC_BUF ( 1 << 6 ) /* return specific locked buffer */
#define SNAP_YUV_CB_RET_SPECIFIC_BUF_ID_BIT_POS ( 16 ) /* return some locked buffer */
#define SNAP_YUV_CB_RET_UNLOCK_SPECIFIC_BUF_ID ( 0xffff << SNAP_YUV_CB_RET_SPECIFIC_BUF_ID_BIT_POS ) /* return some locked buffer */


#define SNAP_YUV_CB_RET_SKIP_AND_LOCK_BUF (SNAP_YUV_CB_RET_LOCK_BUF | SNAP_YUV_CB_RET_SKIP_FLOW) /* the snap flow will be skipped and raw buffer will not be returned */
#define SNAP_YUV_CB_RET_SKIP_AND_UNLOCK_BUF (SNAP_YUV_CB_RET_UNLOCK_ALL_BUF | SNAP_YUV_CB_RET_SKIP_FLOW) /*  the snap flow will be skipped and all locked raw buffer will be returned */


#define SNAP_QV_CB_RET_QV_SHOW ( 0 << 0 )	/* The qv disp will be skipped if host return this value in callback */
											/* For backward compatible, return value 0 mean to display. */
#define SNAP_QV_CB_RET_QV_LOCK_CURR_BUF ( 1 << 1 )
#define SNAP_QV_CB_RET_QV_UNLOCK_ALL_BUF ( 1 << 2 )

#define IS_QV_CB_RET_QV_SHOW(x)  ( ( x & 1 ) == SNAP_QV_CB_RET_QV_SHOW )
#define IS_QV_CB_RET_LOCK_CURR_BUF(x)  ( ( x & SNAP_QV_CB_RET_QV_LOCK_CURR_BUF ) == SNAP_QV_CB_RET_QV_LOCK_CURR_BUF )
#define IS_QV_CB_RET_UNLOCK_ALL_BUF(x)  ( ( x & SNAP_QV_CB_RET_QV_UNLOCK_ALL_BUF ) == SNAP_QV_CB_RET_QV_UNLOCK_ALL_BUF )

#define IS_YUV_CB_RET_SKIP(x)  ( ( x & SNAP_YUV_CB_RET_SKIP_FLOW ) == SNAP_YUV_CB_RET_SKIP_FLOW )
#define IS_YUV_CB_RET_LOCK_BUF(x)  ( ( x & SNAP_YUV_CB_RET_LOCK_BUF ) == SNAP_YUV_CB_RET_LOCK_BUF )
#define IS_YUV_CB_RET_UNLOCK_ALL_BUF(x)  ( ( x & SNAP_YUV_CB_RET_UNLOCK_ALL_BUF ) == SNAP_YUV_CB_RET_UNLOCK_ALL_BUF )
#define IS_YUV_CB_RET_CDSP_REDO(x)  ( ( x & SNAP_YUV_CB_RET_CDSP_REDO ) == SNAP_YUV_CB_RET_CDSP_REDO )
#define IS_YUV_CB_RET_WAIT_FOR_HOST(x)  ( ( x & SNAP_YUV_CB_RET_WAIT_FOR_HOST ) == SNAP_YUV_CB_RET_WAIT_FOR_HOST )
#define IS_YUV_CB_RET_UNLOCK_SPECIFIC_BUF(x)  ( ( x & SNAP_YUV_CB_RET_UNLOCK_SPECIFIC_BUF ) == SNAP_YUV_CB_RET_UNLOCK_SPECIFIC_BUF )
#define IS_YUV_CB_RET_REDO_SPECIFIC_BUF(x)  ( ( x & SNAP_YUV_CB_RET_REDO_SPECIFIC_BUF ) == SNAP_YUV_CB_RET_REDO_SPECIFIC_BUF )

#define SNAP_VLC_CB_RET_OPT_BIT ( 0 )
#define SNAP_VLC_CB_RET_BEST_SHOT_ACT_BIT ( 1 )
#define SNAP_VLC_CB_RET_BEST_SHOT_FLOW_BIT ( 2 )

#define SNAP_VLC_CB_RET_BEST_SHOT_DROP ( 0 << SNAP_VLC_CB_RET_BEST_SHOT_ACT_BIT )
#define SNAP_VLC_CB_RET_BEST_SHOT_KEEP ( 1 << SNAP_VLC_CB_RET_BEST_SHOT_ACT_BIT )

#define SNAP_VLC_CB_RET_BEST_SHOT_CONTINUE ( 0 << SNAP_VLC_CB_RET_BEST_SHOT_FLOW_BIT )
#define SNAP_VLC_CB_RET_BEST_SHOT_FINISH ( 1 << SNAP_VLC_CB_RET_BEST_SHOT_FLOW_BIT )

#define IS_BEST_SHOT_DROP(x) ( ( x & SNAP_VLC_CB_RET_BEST_SHOT_KEEP ) != ( SNAP_VLC_CB_RET_BEST_SHOT_KEEP ) )
#define IS_BEST_SHOT_KEEP(x) ( ( x & SNAP_VLC_CB_RET_BEST_SHOT_KEEP ) == ( SNAP_VLC_CB_RET_BEST_SHOT_KEEP ) )
#define IS_BEST_SHOT_CONTINUE(x) ( ( x & SNAP_VLC_CB_RET_BEST_SHOT_FINISH ) != ( SNAP_VLC_CB_RET_BEST_SHOT_FINISH ) )
#define IS_BEST_SHOT_FINISH(x) ( ( x & SNAP_VLC_CB_RET_BEST_SHOT_FINISH ) == ( SNAP_VLC_CB_RET_BEST_SHOT_FINISH ) )

#define SNAP_HDR_MODE_OFF 0
#define SNAP_HDR_MODE_ACC 1
#define SNAP_HDR_MODE_NORMAL 2

#define SNAP_HDR_ACC_AVERAGE_OFF 0
#define SNAP_HDR_ACC_AVERAGE_ON 1
#define JPG_ENCODE_OVERHEAD (HAL_JPEG_ENC_OVERHEAD + 4)

/*******************************************************************************
 *                              M A C R O S
 ******************************************************************************/

/*******************************************************************************
 *                          D A T A    T Y P E S
 ******************************************************************************/
#define SNAP_CALLBACK_DATA_FMT_RAW_PACK_8_BIT HAL_CDSP_DATA_TYPE_RAW_8
#define SNAP_CALLBACK_DATA_FMT_RAW_PACK_10_BIT HAL_CDSP_DATA_TYPE_RAW_10
#define SNAP_CALLBACK_DATA_FMT_RAW_PACK_12_BIT HAL_CDSP_DATA_TYPE_RAW_12
#define SNAP_CALLBACK_DATA_FMT_RAW_PACK_14_BIT HAL_CDSP_DATA_TYPE_RAW_14
#define SNAP_CALLBACK_DATA_FMT_RAW_PACK_16_BIT HAL_CDSP_DATA_TYPE_RAW_16

typedef enum {	
	SNAP_INTERNAL_CB_PRE_INIT = 0,  /* cb for internal cb init */
	SNAP_INTERNAL_CB_CAP_INIT,  /* cb for sg init */
	SNAP_INTERNAL_CB_CDSP_INIT,  /* cb after cdsp adjusted size info */
	SNAP_INTERNAL_CB_YUVCB_IN_CDSP, /* yuv cb in cdsp task */
	SNAP_INTERNAL_CB_YUVCB_IN_JPG,  /* yuv cb in jpg task */
	SNAP_INTERNAL_CB_JPG_START,     /* final yuv ready for vlc encode */
	SNAP_INTERNAL_CB_ABORT, 	/* final yuv ready for vlc encode */	
	SNAP_INTERNAL_CB_MAX,
} snapInternalCb_t;


/* capture trig YUV CB mode */
typedef enum{
	SNAP_YUV_CB_REF_YUV	= 0x00,
	SNAP_YUV_CB_MODIFY_YUV,
}snapYuvCbType_e;


/*******************************************************************************
 *                         G L O B A L    D A T A
 ******************************************************************************/

/*******************************************************************************
 *                  E X T E R N A L    R E F E R E N C E
 ******************************************************************************/

/*******************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S
 ******************************************************************************/

#endif /* _SNAP_DEF_H_ */


