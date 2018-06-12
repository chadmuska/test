/**************************************************************************
 *
 *       Copyright (c) 2008-2016 by iCatch Technology, Inc.
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
 *  Author: Anmin Deng
 *
 **************************************************************************/
/* DO NOT EDIT! The next 2 lines are for auto revision by subversion
$Rev: 26010 $
$Date:: 2016-04-20 16:06:10#$
 */
#ifndef _SP5K_OTRACK_API_H_
#define _SP5K_OTRACK_API_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "common.h"
#include "middleware/otrack_def.h"


/**< INTERFACE TO SP5K HOST */

/**
 * called in app_init.c and switch on/off in app_init.h
 * otrack FW module will not be linked in without calling
 */
/**< calling this if host wants HW obj track support for preview */
/*void otrackPvInit(void); */
/**< calling this if host wants HW obj track support for playback (TBD) */
/* void otrackPbInit(void); */


/** < ==== obj tracking operating APIs ==== */
/** the following obj tracking operating APIs
 * only working if obj tracking is running (by sp5kObjTrackModeSet), and
 * only working during a session of preview modes (or movie playback --- TBD)
 * once mode switched, all obj tracking activities will be forced to stop,
 */
#define SP5K_OBJ_TRACK_TARGET_ID_ALL      OTRACK_TARGET_ID_ALL
#define SP5K_OBJ_TRACK_TARGET_ID_INVALID  OTRACK_TARGET_ID_INVALID

#define SP5K_OBJ_TRACK_TARGET_SIZE_AUTO   OTRACK_TARGET_SIZE_AUTO


/**
 * to create a new target for obj tracking
 * \return SUCCESS; o/w failed, typically due to not in preview mode,
 * already too many tracking targets, etc
 * \param pobjid, points to a user prepared UINT32, the target objid returned
 * should be later used as objid in other sp5kObjTrackTargetXxxx APIs
 * host should check if pobjid points to value SP5K_OBJ_TRACK_TARGET_ID_INVALID
 * \param x,
 * \param y, coordinates of target center
 * \param sz, size of target object, 0 uses auto size detecting
 * \note later when obj started OK, message SP5K_MSG_OBJ_TRACK_START,
 * param objid will send to host
 * \note later when obj started failed, message SP5K_MSG_OBJ_TRACK_START_FAIL,
 * param objid will send to host
 * \note messages SP5K_MSG_OBJ_TRACK_READY will send to host
 * periodically, host passing the attached param in sp5kObjTrackResultGet();
 */
UINT32 sp5kObjTrackTargetStart(UINT32 *pobjid, UINT32 x, UINT32 y, UINT32 sz);

/**
 * to update an existed obj tracking target created by sp5kObjTrackTargetStart
 * \return SUCCESS; o/w failed
 * \param objid, returned by sp5kObjTrackTargetStart pointed by pobjid;
 * objid == SP5K_OBJ_TRACK_TARGET_ID_ALL not supported
 * \param x,
 * \param y, new coordinates of target center;
 * -1 keeps using previous obj tracking historical results;
 * \param sz, new size of target object; 0 uses auto size detecting;
 * -1 keeps using previous obj tracking historical results;
 * \note a message SP5K_MSG_OBJ_TRACK_UPDATE, param objid will send to host
 */
UINT32 sp5kObjTrackTargetUpdate(UINT32 objid, UINT32 x, UINT32 y, UINT32 sz);


/**
 * to destroy an obj tracking target created by sp5kObjTrackTargetStart
 * \return SUCCESS; o/w failed
 * \param objid, the obj returned by sp5kObjTrackTargetStart pointed by pobjid;
 * objid == SP5K_OBJ_TRACK_TARGET_ID_ALL stops all the obj track activities
 * \note a message SP5K_MSG_OBJ_TRACK_STOP with param objid will send to host
 */
UINT32 sp5kObjTrackTargetStop(UINT32 objid);


/**< obj types bitwise-OR bitmap */
#define SP5K_OBJ_TRACK_FOUND_MODE  0x80000000
#define SP5K_OBJ_TRACK_MISS_MODE   0x40000000

typedef otrackResult_t sp5kObjTrackResult_t;


/**< upon receiving host message SP5K_MSG_OBJ_TRACK_READY, host calls */
/**
 * \param param is the param attached to the received host message for checking
 * \param pnobj points to a user prepare UINT32 in which
 * otrack system fills number of objs being tracked in presult
 * \param presult points to a user prepared sp5kObjTrackResult_t list,
 * user is responsible for giving adequate size of the list
 * presult[0].. presult[*pnobj -1] are properties of those objs,
 */
SINT32 sp5kObjTrackResultGet(UINT32 param,UINT32 *pnobj,
		sp5kObjTrackResult_t *presult);


/**
 * \param cfg in otrackCfgSel_t
 */
UINT32 sp5kObjTrackCfgSet(UINT32 cfg,UINT32 val);
UINT32 sp5kObjTrackCfgGet(UINT32 cfg,UINT32 *pval);

/**
 * \param cfg in
 * UINT32 sp5kObjTrackCfgSet(UINT32 cfg,UINT32 val);
 */
typedef enum {
	SP5K_OBJ_TRACK_CFG_ACCUM_PERIOD      = OTRACK_CFG_ACCUM_PERIOD      ,
	SP5K_OBJ_TRACK_CFG_TARGET_CNT_MAX    = OTRACK_CFG_TARGET_CNT_MAX    ,
	SP5K_OBJ_TRACK_CFG_MIN_OBJ_SIZE      = OTRACK_CFG_MIN_OBJ_SIZE      ,
	SP5K_OBJ_TRACK_CFG_PROC_REF_YPOP_THRS= OTRACK_CFG_PROC_REF_YPOP_THRS,
	SP5K_OBJ_TRACK_CFG_MANU_MODE_DEV     = OTRACK_CFG_MANU_MODE_DEV     ,
	SP5K_OBJ_TRACK_CFG_PROC_DONE_CB      = OTRACK_CFG_PROC_DONE_CB      ,
	SP5K_OBJ_TRACK_CFG_SEARCH_RANGE      = OTRACK_CFG_SEARCH_RANGE      ,
	SP5K_OBJ_TRACK_CFG_PROC_OUTBUF_NUM   = OTRACK_CFG_PROC_OUTBUF_NUM   ,
	SP5K_OBJ_TRACK_CFG_TOTAL,
} sp5kObjTrackCfgSel_t;



/**
 * FIXME
 * optical zoom?
 */
/**
 * \param fac, 0 or 1000 means x1, ie, no FOV change;
 */
UINT32 sp5kObjTrackFOVFacSet(UINT32 fac);


#if 0
#warning otrack custom user grid TBD
/**
 * obj tracking coordinates must be squared pixel
 * \param size, only specifies horizontal size, may not be larger than 1024;
 * 0: uses system default; -1: do not change, only return current size
 * \return the actual result coordinates xsize
 */
UINT32 sp5kObjTrackCoordSizeSet(UINT32 size);
#endif


/**
 * \param mode 0:resume, 1:suspend
 */
UINT32 sp5kObjTrackSuspend(UINT32 mode);


#ifdef __cplusplus
}
#endif

#endif /* _SP5K_OTRACK_API_H_ */
