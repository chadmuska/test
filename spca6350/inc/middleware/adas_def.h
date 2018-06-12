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
#ifndef _ADAS_DEF_H_
#define _ADAS_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif 

#include "common.h"
#include "middleware/modesw_def.h"

/**< frame buffer addr must align to 16 byte and width must be mutiple of 8 */
#if 1
typedef struct {
	UINT8 *yuvaddr;
	UINT16 yuvw;
	UINT16 yuvh;
	UINT16 dispx;
	UINT16 dispy;
	UINT16 dispw;
	UINT16 disph;
	UINT16 frmid;
} padasFrameBuf_t;
#else
typedef modeswFrameBuf_t padasFrameBuf_t;
#endif

typedef enum {
	ADAS_ALGO_LDWS = 0x0,
	ADAS_ALGO_LPD,
	ADAS_ALGO_TSR,
	ADAS_ALGO_BLINDSPOT,	
	ADAS_ALGO_ROD,	
	ADAS_ALGO_BIRDVIEW,	
	ADAS_ALGO_FCMD,
	ADAS_ALGO_HEADLIGHT,
	ADAS_ALGO_NUMBER_MAX
} ADAS_algo_t;

typedef enum {
	ADAS_ALGO_PRI_LOW = 0x0,
	ADAS_ALGO_PRI_MID,
	ADAS_ALGO_PRI_TOP,
	ADAS_ALGO_PRI_END
} ADAS_algo_priority_t;

typedef enum {
	ADAS_ALGO_STATUS_RUNNING = 0x1,
	ADAS_ALGO_STATUS_STOP,
	ADAS_ALGO_STATUS_NOWOKER,
} ADAS_algo_status_t;

typedef enum {
	ADAS_ALGO_ENABLE = 0x1,
	ADAS_ALGO_DISABLE = 0x0
} ADAS_algo_valid_t;


typedef enum {
	ADAS_WORKER_FREE = 0x1,	
} ADAS_Worker_Status_t;

typedef enum {
	ADAS_INPUTBUF_CFG_SENSOR_FRONT = 0x0,
	ADAS_INPUTBUF_CFG_SENSOR_REAR,
	ADAS_INPUTBUF_CFG_SENSOR_LEFT,
	ADAS_INPUTBUF_CFG_SENSOR_RIGHT,	
	ADAS_INPUTBUF_CFG_SENSOR_MAX
} ADAS_InputBuf_Cfg_Sensor;

typedef enum {
	ADAS_INPUTBUF_CFG_INPUT_CDSPMAIN = 0x0,
	ADAS_INPUTBUF_CFG_INPUT_CDSPSEC,
	ADAS_INPUTBUF_CFG_INPUT_3RD,
	ADAS_INPUTBUF_CFG_INPUT_4TH,
	ADAS_INPUTBUF_CFG_INPUT_PBVDO,	
	ADAS_INPUTBUF_CFG_INPUT_MAX
} ADAS_InputBuf_Cfg_Input;


#define ADAS_WORKER_TOTALAMOUNT 3
#define ADAS_BUFMGR_DONE (1<<ADAS_WORKER_TOTALAMOUNT)
#define ADAS_AGGREGATOR_DONE_BASE (1<<(ADAS_WORKER_TOTALAMOUNT+1))
#define ADAS_DONE_ALL (0xffff>>(16-ADAS_WORKER_TOTALAMOUNT-1-ADAS_INPUTBUF_CFG_INPUT_MAX)) /*worker Number + BUFMGR + AGGREGATOR*/

#define ADAS_BUFSRC_TOTALAMOUNT 10
#define ADAS_BUFLIST_TOTALAMOUNT 100

#define ADAS_COORD_HSIZE     65536 /*adas coordinate window*/
#define ADAS_COORD_VSIZE     65536


/**************************************************************************
 * ADAS
 **************************************************************************/

typedef union {
	struct {
		UINT16 x,y,w,h;
	} elem;
	UINT64 l;
} crop_t;

typedef struct adasAlgoBufCrop_s {
	crop_t	crop;
	UINT8	set;
}adasAlgoBufCrop_t;

/* extracting elememnts from an UINT32 or UINT64 of crop */
#define CROP_X(crop)        (((crop_t){ .l = (crop), }).elem.x)
#define CROP_Y(crop)        (((crop_t){ .l = (crop), }).elem.y)
#define CROP_W(crop)        (((crop_t){ .l = (crop), }).elem.w)
#define CROP_H(crop)        (((crop_t){ .l = (crop), }).elem.h)
/* packing an UINT32 or UINT64 of crop from elements */
#define CROP_MAKE(x,y,w,h)  (((crop_t){ {x,y,w,h,} }).l)

#if ADAS_BUF_ALLOC_BOTTOM
	#if ADAS_BUF_ALLOC_CACHE
	#define ADAS_BUF_ALLOC(x)	ros_malloc_bot_c(x)
	#define ADAS_BUF_FREE(x)  ros_free_c(x)
	#else
	#define ADAS_BUF_ALLOC(x)	ros_malloc_bot_uc(x)
	#define ADAS_BUF_FREE(x)  ros_free_uc(x)
	#endif
#else
	#if ADAS_BUF_ALLOC_CACHE
	#define ADAS_BUF_ALLOC(x)	ros_malloc_c(x)
	#define ADAS_BUF_FREE(x)  ros_free_c(x)
	#else
	#define ADAS_BUF_ALLOC(x)	ros_malloc_uc(x)
	#define ADAS_BUF_FREE(x)  ros_free_uc(x)
	#endif
#endif


/*===================================================*/
/* adas algo */
/*===================================================*/

typedef struct adasAlgoBuf_s {
	padasFrameBuf_t bufinfo;
	ADAS_InputBuf_Cfg_Sensor bufsrc;
} adasAlgoBuf_t;

typedef struct adasWorkerBuf_s {
	adasAlgoBuf_t buf[ADAS_BUFLIST_TOTALAMOUNT];
} adasWorkerBuf_t;

typedef void (*fp_adasAlgo)(adasWorkerBuf_t *ppframes, UINT32 nframes);
typedef UINT32 (*fp_adasAlgoCb)(UINT32 param, UINT32 param2);
typedef void (*fp_adasAlgoCfgSet)(UINT32 selector,va_list args);
typedef void (*fp_adasAlgoCfgGet)(UINT32 selector,va_list args);
typedef void (*fp_adasAlgoCreate)(UINT32 param, UINT32 param2);
typedef void (*fp_adasAlgoDestory)(UINT32 param, UINT32 param2);

typedef struct {
	ADAS_InputBuf_Cfg_Sensor src;	/* ADAS algo frame source */
	UINT32 nframes;					/* ADAS algo needs nframes */
	UINT32 cnt;						/* ADAS algo get nframes */
}adasAlgoBufSrc_t;

typedef struct {
	char pname[20];				/* ADAS algo name */
	char pver[20];				/* ADAS algo ver */
	fp_adasAlgo padasAlgo; 		/* ADAS algo function pointer */
	fp_adasAlgoCfgSet padasAlgoCfgSet;		/* ADAS algo set function pointer */
	fp_adasAlgoCfgGet padasAlgoCfgGet;		/* ADAS algo get function pointer */
	fp_adasAlgoCreate padasAlgoCreate;
	fp_adasAlgoDestory padasAlgoDestroy;
	fp_adasAlgoCb padasAlgoCb;	/* ADAS algo callback function pointer */
	UINT32 prio;    			/* ADAS algo priority */
	UINT32 period;				/* ADAS algo accumulation period */
	adasAlgoBufSrc_t bufsrc[ADAS_BUFSRC_TOTALAMOUNT]; /* ADAS algo frame source */
} adasAlgo_t;

typedef struct {
	adasAlgo_t algo;	/* registed ADAS algo */
	UINT32 status;		/* ADAS algo status [running/stopping] */
	UINT32 enable;		/* ADAS algo avaliable or not , configure by host */
	UINT32 counter;		/* ADAS scheduler use temporarily */
	UINT32 workerId;	/* ADAS scheduler will dispatch ADAS algo to free worker task through workerID */
	adasAlgoBuf_t bufs[ADAS_BUFLIST_TOTALAMOUNT]; /* ADAS algo required buffer array */
} adasAlgoCtrl_t;


/*===================================================*/
/* adas aggregator */
/*===================================================*/

typedef struct adasBufInput_s {
	padasFrameBuf_t bufInputInfo;		/*Input buf from real input */
	ADAS_InputBuf_Cfg_Sensor buf_src;	/*Input buf to which buf_src(algo)*/
}adasBufInput_t;

typedef struct adasInputBufSenCfg_s {
	ADAS_InputBuf_Cfg_Sensor id;
	crop_t roi;
}adasInputBufSenCfg_t; 

typedef struct adasBufInputCfg_s {
	ADAS_InputBuf_Cfg_Input input;			 					/*main/second source in V35*/
	UINT32 nSensor; 											/*n sensor frames combine to one input */
	adasInputBufSenCfg_t sen[ADAS_INPUTBUF_CFG_SENSOR_MAX];		/*input sensor config*/
}adasInputBufCfg_t;

#define ADAS_WORKERCMD_NUM 64


/*===================================================*/
/* adas worker */
/*===================================================*/

#define ADAS_WORKER_STACK_SIZE (8192/*4096*/)
#define ADAS_WORKERCMD_NUM 64


/*===================================================*/
/* adas buffer manager */
/*===================================================*/

typedef void (*fp_adasWorkerCb)(adasAlgoBuf_t *pbuf ,UINT32 buf_src);
/*typedef void (*fp_adasBufSrcIrqCb)(void);*/

typedef struct adasbufbak_s {
	padasFrameBuf_t bufinfo;
	ADAS_InputBuf_Cfg_Sensor bufsrc;
	UINT32 ref_cnt;
	UINT32 lock;	
	struct adasbufbak_s *pNext;
} adasbufbak_t;

typedef struct {
	/*fp_adasBufSrcIrqCb pBufSrcCb;*/
	void * pBufSrcCb;
	UINT32 cbIdx;
	UINT32 srcSync; 	
} adasBufSrc_t;

#define ADAS_ALGOWORK_SEND_DONE	1


#define ADAS_BUFMGR_STACK_SIZE (4096)
#define ADAS_ALGOWORKCMD_NUM 64



/*===================================================*/
/* adas scheduler */
/*===================================================*/

typedef struct adasWork_s {
	adasAlgoCtrl_t *palgoWork;
	/*adasbufbak_t bufs[ADAS_BUFLIST_TOTALAMOUNT];*/
	fp_adasWorkerCb padasWorkerCb;
} adasWork_t;


typedef struct adasWorkList_s { 
	adasWork_t work;
	struct adasWorkList_s *pNext;
} adasWorkList_t;


typedef struct adasTaskList_s{
	adasAlgoCtrl_t *pCtrl;				/* ADAS algo ctrol pointer */
	struct adasTaskList_s *pNext;		/* ADAS task list pointer */
} adasTaskList_t;

/*===================================================*/
/* adas debug */
/*===================================================*/

typedef enum {
	ADAS_DBG_BUF_TYPE_TASKLIST,
	ADAS_DBG_BUF_TYPE_WORKLIST,
	ADAS_DBG_BUF_TYPE_BUFBAKLIST,	
	ADAS_DBG_BUF_TYPE_YUVBUF,
	ADAS_DBG_BUF_TYPE_MAX
}adasDbgBuf_e;



/*===================================================*/
/* adas configuration */
/*===================================================*/


/**
 * \param cfg in
 * UINT32 sp5kAdasCfgSet(UINT32 cfg,UINT32 val);
 */
typedef enum {

	/* adas common cfg */
	ADAS_CFG_START = 0x1000,
	ADAS_CFG_ALGO_ENABLE = ADAS_CFG_START,
	ADAS_CFG_INPUTBUF,
	ADAS_CFG_RESULT_CB,

	ADAS_CFG_ALGO_PERIOD,

	/* adas algo cfg */
	ADAS_CFG_ALGO_START = 0x2000,
	ADAS_CFG_ALGO_LDWS = ADAS_CFG_ALGO_START,
	ADAS_CFG_ALGO_ROD,
	ADAS_CFG_ALGO_LPD,
	ADAS_CFG_ALGO_TSR,
	ADAS_CFG_ALGO_FCMD,
	ADAS_CFG_ALGO_HEADLIGHT,
	ADAS_CFG_TOTAL,

	/* adas algorithm shared parameter */
	ADAS_CFG_ALGO_SHARE_START = 0x3000,
	ADAS_CFG_ALGO_SHARE_CAR_ROI,
	ADAS_CFG_ALGO_SHARE_TOTAL

	
} padasCfgSel_t;



/**************************************************************************
 * Obsolete
 **************************************************************************/






#ifdef __cplusplus
}
#endif

#endif /* _FTRACK_DEF_H_ */
