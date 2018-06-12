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
#ifndef APP_LENS_API_H
#define APP_LENS_API_H

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/
#include "sp5k_global_api.h"
#include "api/sp5k_os_api.h"

//------martin edit  0524 ---------
//------martin edit  0524 ---------
#define S1_receive         (((UINT32)1)<<0)
#define S1_send             (((UINT32)1)<<1)
#define S2_send             (((UINT32)1)<<2)
#define S3_send             (((UINT32)1)<<3)
#define S4_send             (((UINT32)1)<<4)
#define S5_send             (((UINT32)1)<<5)

#define  Lens_open0       1
#define  AF_scan0           2
#define  AF_scan1           3
#define  AF_macro0        4
#define  Iris_big0            5
#define  Iris_small0        6
#define  Shutter_On0      7
#define  Shutter_Off0      8
#define  Zoom_A0           10
#define  Zoom_A1           11
#define  Zoom_A2           12
#define  Zoom_A3           13
#define  Zoom_A4           14
#define  Zoom_A5           15
#define  Zoom_A6           16
#define  Zoom_A7           17
#define  Zoom_A8           18
#define  Zoom_A9           19
#define  ND_big0             20
#define  ND_small0         21
#define  Focus_CW0        22
#define  Focus_CCW0      23
#define  Zoom_CW0         24
#define  Zoom_CCW0       25
#define  AF_calib0	     26
#define  Step_calib0 	     27
#define  Lens_close0	     28
#define  Cmd_max           29

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/*-- Lens Configuration */
#define LENS_CLOSE_ZHM_LED_LEVEL_HI		0
	/* the Zoom Home Led IO level when Lens is closed */

#define ZOOM_ZS_NUM				(7)
		/* number of default stop of Zoom lens.
		 * It incluse the WS & TS. WS=ZS[0], TS=ZS[ZOOM_ZS_NUM-1]
		 */

//Modified by Brian 08/08/08
	/*--
	 * Focus Position
	 */	
#define FOCUS_HP_POS			(40)
#define FOCUS_CLOSE_POS			(FOCUS_HP_POS-40)
#define FOCUS_SPEC_CLOSE_POS	(-40)
	
#define FOCUS_POSITION_OFFSET	(FOCUS_CLOSE_POS - FOCUS_SPEC_CLOSE_POS)
	
#define FOCUS_0_S 274       	
#define FOCUS_0_E 312 
#define FOCUS_1_S 228         
#define FOCUS_1_E 285 
#define FOCUS_2_S 190         
#define FOCUS_2_E 271 
#define FOCUS_3_S 159         
#define FOCUS_3_E 274 
#define FOCUS_4_S 149         
#define FOCUS_4_E 309 
#define FOCUS_5_S 141         
#define FOCUS_5_E 362 
#define FOCUS_6_S 100         
#define FOCUS_6_E 402 
		//Macro
#define FOCUS_7_S 324   
#define FOCUS_7_E 369 
		//
		
#define FOCUS_0_STEP	  5
#define FOCUS_1_STEP	  6
#define FOCUS_2_STEP	  8
#define FOCUS_3_STEP	  9
#define FOCUS_4_STEP	  11
#define FOCUS_5_STEP	  12
#define FOCUS_6_STEP	  14
#define FOCUS_7_STEP	  5
		
#define FOCUS_0_2M	282
#define FOCUS_1_2M	240
#define FOCUS_2_2M	208
#define FOCUS_3_2M	184
#define FOCUS_4_2M	184
#define FOCUS_5_2M	192
#define FOCUS_6_2M	174


#define MSH_GPIO_ISR_ENABLE 0  //use sensor shutter trigger to close m-shutter

#if MSH_GPIO_ISR_ENABLE
#define SENSOR_RAW_RESET_TIME  797  //7.97ms
#else
#if (SP5K_SENSOR_MI9130)
#define SENSOR_RAW_RESET_TIME  3120 //31.2ms
#else
#define SENSOR_RAW_RESET_TIME  0
#endif
#endif
/*For record exif of lens parameter Lation@20130131*/
#define RECORD_LENS_EXIF_ON		TRUE
#define RECORD_EXIF_AF_SIZE     150

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
// *****Xavier Start
/* lensApp task event */
typedef enum lensAppTaskEventBitOrder_e {
    BO_EVENT_LENS_APP_TELE = 0,
	BO_EVENT_LENS_APP_WIDE,
    BO_EVENT_LENS_APP_STOP,
    BO_EVENT_FOCUS_APP_STOP,
} lensAppTaskEventBitOrder_t;

typedef enum lensAppTaskEventDef_s {
	EVENT_LENS_APP_TELE				= (((UINT32)1)<<BO_EVENT_LENS_APP_TELE),
	EVENT_LENS_APP_WIDE				= (((UINT32)1)<<BO_EVENT_LENS_APP_WIDE),
    EVENT_LENS_APP_STOP         = (((UINT32)1)<<BO_EVENT_LENS_APP_STOP),
    EVENT_FOCUS_APP_STOP        = (((UINT32)1)<<BO_EVENT_FOCUS_APP_STOP),
} lensAppTaskEventDef_t;

typedef enum lensAppTaskPhase_s {
    PHASE_BOOT_FINDING_PR = 0,
    PHASE_BOOT_FINDED_PR,
    PHASE_NORMAL,
    PHASE_CLOSE_FINDING_PR,
    PHASE_CLOSE_FINDED_PR,
} lensAppTaskPhase_t;

typedef enum lensAppTaskIris_e {
    IRIS_BIG = 0,
    IRIS_SMALL,
    IRIS_INIT,
} lensAppTaskIris_t;


typedef void (*LensProc_t)(UINT32 flag);

typedef struct LensCtrlParam_s {
    LensProc_t LensProc;
    LensProc_t FocusProc;
    lensAppTaskPhase_t ZoomPhase;
    lensAppTaskPhase_t FocusPhase;
    UINT8 ZoomDir;
    UINT8 ZoomStopFlag;
    UINT8 ZoomStep;
    UINT8 ZoomTargetStep;      /*Add by Lation@20130122*/
    UINT8 ZoomTargetFlag;       /*Add by Lation@20130122*/
    UINT8 ZoomLensInFlag;       /*Add by Lation@20130122*/
    UINT16 ZoomPos;
    UINT16 FocusPos;
    UINT8 FocusPosZoomidx;
    lensAppTaskIris_t IrisStatus;
} LensCtrlParam_t;
// *****Xavier Stop

/*-- Lens Query ID */
typedef enum lensQueryID_s {
	LENS_QUERY_MOTION_STATE,
	LENS_QUERY_BOOT_DONE,
	LENS_QUERY_LENS_CLOSED,
	LENS_QUERY_ZOOM_TELE_PARKED,
	LENS_QUERY_ZOOM_WIDE_PARKED,
	LENS_QUERY_ZOOM_SEG_POS,
	LENS_QUERY_ZOOM_SEG_NUM,
	LENS_QUERY_ZOOM_SEG2POS,
	LENS_QUERY_ZOOM_STEP_POS,
	LENS_QUERY_IRIS_SEG_POS,
	LENS_QUERY_FOCUS_MOTION_STATE,
	LENS_QUERY_FOCUS_HOME_POS,
	LENS_QUERY_FOCUS_IS_HOME,
	LENS_QUERY_FOCUS_STEP_POS,
	LENS_QUERY_MAX
} lensQueryID_t;

typedef void appLensStopNotifyCBF_t(void);


typedef enum {
	MSHUT_SHORT_EXP_DELAY,
	MSHUT_LONG_EXP_DELAY,
	MSHUT_NONE_CLOSE
} mshutDelayMode_t;

/*Lens Zoom Point Define Add by Lation@20120929*/
typedef enum{
    ZOOM_POINT_1 = 0x0,
    ZOOM_POINT_2,
    ZOOM_POINT_3,
    ZOOM_POINT_4,
    ZOOM_POINT_5,
    ZOOM_POINT_6,
    ZOOM_POINT_7,
    ZOOM_POINT_8,
    ZOOM_POINT_9,
    ZOOM_POINT_10,
    /*ZOOM_POINT_11,*/
    ZOOM_POINT_WIDE = ZOOM_POINT_1,
    ZOOM_POINT_TELE = ZOOM_POINT_10,
    ZOOM_POINT_TOTAL = ZOOM_POINT_TELE + 1
}LensZoomPoint_e;

/*Lens focus distance table*/
typedef enum{
    FOCUS_DIST_05M = 0x0,
    FOCUS_DIST_07M,
    FOCUS_DIST_10M,
    FOCUS_DIST_20M,
    FOCUS_DIST_50M,
    FOCUS_DIST_INFI,
    FOCUS_DIST_TOTALS
}LensStepDistanceTbl_e;

/*The position of zoom*/
typedef enum{
    ZOOM_POS_WIDE2TELE = 0x0,
    ZOOM_POS_CLOSE,
    ZOOM_POS_WIDE2CLOSE
}LensZoomState_e;

/*For record the lens param in exif log Lation@20130129*/
#if RECORD_LENS_EXIF_ON
typedef struct LensExifLog_s {
    UINT8 ZoomPoint;
    UINT8 LensIn;
    UINT8 AfMode;
    UINT8 NearCalibSign;
    UINT8 FarCalibSign;
    UINT16 PeakPoint;
    UINT16 NearFocus[ZOOM_POINT_TOTAL];
    UINT16 FarFocus[ZOOM_POINT_TOTAL];
    UINT16 LensInFocus[ZOOM_POINT_TOTAL-ZOOM_POINT_5];
    UINT16 AfStep[RECORD_EXIF_AF_SIZE];
    UINT32 PeakPointCV;
    UINT32 AfCV[RECORD_EXIF_AF_SIZE];
} LensExifLog_t;
#endif
/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

#define LENS_SLEEP(ms)	do { if (((UINT32)ms)>0) sp5kTimeDelay(SP5K_TIME_DELAY_1MS, (UINT32)ms); } while(0)

#define LENS_MSG_POST(msg, param) sp5kHostMsgSend((UINT32)(msg), (UINT32)(param))

/*-- Lens position check */
#if !defined(LENS_CLOSE_ZHM_LED_LEVEL_HI)
#error "LENS_CLOSE_ZHM_LED_LEVEL_HI is not defined !!"
#endif
#if LENS_CLOSE_ZHM_LED_LEVEL_HI
#define IS_LENS_CLOSE 	IO_LENS_LED_ZHM_GET
#else
#define IS_LENS_CLOSE 	(!IO_LENS_LED_ZHM_GET)
#endif

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

extern LensExifLog_t LensParamExif;       /*Add by Lation@20130129*/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
void lensAppTask(ULONG init_data);
void lensAppTaskInit(void);
//void appLensAFCreate(void);		/*Mark by Lation@20121023*/
//void appLensAFDestroy(void);		/*Mark by Lation@20121023*/
//void appLensCreate(void);			/*Mark by Lation@20121023*/
//void appLensDestroy(void);		/*Mark by Lation@20121023*/
/**UINT32 appLensQuery(UINT32 queryId);**/
#define appLensQuery(x)	0
//UINT32 appLensStopWait(void);	/*Mark by Lation@20121023*/
/**UINT32 appLensCrashRecover(void);**/
#define appLensCrashRecover()
/**void appLensBoot(void);**/
#define appLensBoot()
//void appLensOpen(void);			/*Mark by Lation@20121023*/
/*void appLensClose(void);*/
#define appLensClose()
//void appLensStopCallbackSet(appLensStopNotifyCBF_t *pcallbackFunc);		/*Mark by Lation@20121023*/
//void calibLensZoomSegGo(	UINT32 zmSeg);							/*Mark by Lation@20121023*/
//UINT32 appLensZoomBootSlipCalib(void);								/*Mark by Lation@20121023*/
//UINT32 appLensZoomTeleWideSlipCalib(UINT32 startSeg, UINT32 stopSeg);	/*Mark by Lation@20121023*/
//UINT32 appLensZoomBlashSlipCalib(UINT32 startSeg, UINT32 stopSeg);		/*Mark by Lation@20121023*/

/**void appLensInit(void);**/
#define appLensInit()
/*-- Zoom APIs */
UINT32 appLensTele(void);
UINT32 appLensWide(void);
void appLensStop(void);
UINT8 appLensZoomStep(void);
/*-- Focus APIs */
void appLensAFRange(UINT8 zoomstep, UINT16 *near_end, UINT16 *far_end, UINT8 *sitvl, UINT8 *bitvl);
void appCalibSearchRangeInit(UINT8 ZoomStep, UINT16 *far_end, UINT16 *near_end, UINT8  *sitvl, UINT8  *bitvl);/*Add by Lation@20121030*/


UINT32 appFocusPosGo(UINT16 pos);
UINT16 appFocusPosGet(void);
//void appFocusStopWait(void);		/*Mark by Lation@20121023*/
/*-- Mshutter APIs */
UINT32 appMShutOpen(void);
UINT32 appMShutClose(void);
/*void appLensMShutCallbackRegister(void);*/
#define appLensMShutCallbackRegister()

/*-- Iris APIs */
UINT32 appIrisLargeSet(void);
UINT32 appIrisSmallSet(void);
/*-- ND APIs */
UINT32 appNDLargeSet(void);
UINT32 appNDSmallSet(void);
UINT32 appLensZoomPointSet(LensZoomPoint_e ZoomPointIdx);
void appLensInfinityLoad(void);
UINT8 appZoomCurrentStepGet(void);
void appZoomLensInFlagSet(UINT8 Flag);
UINT8 appZoomLensInFlagGet(void);    /*Lation@20130123*/
UINT16 appZoomCurrentPosGet(void);  /*Lation@20130228*/

#if RECORD_LENS_EXIF_ON  /*Lation@20130129*/
void appLensExifLogReset(void);
UINT32 appLensAlgWriteAppn( UINT32 Addr, UINT32 Size);
#endif
#endif  /* APP_LENS_API_H */


