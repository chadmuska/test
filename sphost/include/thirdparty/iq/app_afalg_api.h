/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch         *
 *  Technology Co., Ltd. All rights are reserved by iCatch Technology     *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of iCatch  Technology Co., Ltd.                       *
 *                                                                        *
 *  iCatch  Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch  Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Vance Hunag                                                   *
 *                                                                        *
 **************************************************************************/
#ifndef _FOCUS_H_
#define _FOCUS_H_

#include "api/sp5k_os_api.h"
#include "api/sp5k_af_api.h"
#define AF_ALG_VER			"01.00.00"



#define ENABLE_SIMPLIFICATION_MAKERNOTE 

#define LENS_LMC_MAPPING_MODE_LB1846	0

#define LENS_LMC_MAPPING_MODE_LB1848			16
#define AF_MAX_SUBWIN_GRID_Y			16

#define AF_DEFOCUS_SUBWIN_GRID_X		16
#define AF_DEFOCUS_SUBWIN_GRID_Y		16


#define AF_SINGLE_SUBWIN_GRID_X		3
#define AF_SINGLE_SUBWIN_GRID_Y		2
#define AF_SINGLE_SUNWIN_REGION_TOTAL	(AF_SINGLE_SUBWIN_GRID_X*AF_SINGLE_SUBWIN_GRID_Y)


#define AF_MULTI_SUBWIN_GRID_X		3	/*16*/
#define AF_MULTI_SUBWIN_GRID_Y		3	/*16*/

#define AF_MULTI_REGION_X 3
#define AF_MULTI_REGION_Y 3
#define AF_MULTI_REGION_TOTAL	(AF_MULTI_REGION_X*AF_MULTI_REGION_Y)


#define AF_SINGLE_SUB_CENTRAL_REGION_IDX	5	/*for single sub window*/
#define AF_MULTI_CENTRAL_REGION_IDX		    5	/* for 3x3 window */
#define AF_DEFOCUS_CENTRAL_REGION_IDX		5	/* for 16x16 window */


#define AF_CHECK_STABLE_MIN_EDGE	5000 	/* 3400 */
#define AF_SMALL_GAIN_ACK_EDGE 		3000 	/* 1ack edge for preview small iso gain*/
#define AF_HIGH_GAIN_ACK_EDGE 		100		/* ack edge for preview high iso gain*/

#if 1
#define FOCUS_DIRECT_PWM_DRIVING
#endif

#if 0 
#define ENABLE_FOCUS_CALIBRATION_SEARCH_RESET
#endif

#define SEARCH_RESET_CNT	3

enum {
	AF_BUSY	= 0,
	AF_FAIL,
	AF_SUCCESS,
	AF_FINI,
	AF_MAX
};

enum
{
	AF_AREA_SPOT,
	AF_AREA_MULTI,
};
enum
{
   FOCUS_PF_MACRO,
   FOCUS_PF_NORMAL_INDOOR,
   FOCUS_PF_NORMAL_OUTDOOR,
   FOCUS_PF_VIDEO,
   FOCUS_PF_TOTAL,
};

enum
{
   FOCUS_REGION_SINGLE,
   FOCUS_REGION_SINGLE_SUBWIN,
   FOCUS_REGION_3x3,
   FOCUS_REGION_DEFOCUS,
   FOCUS_REGION_TOTAL,
};

enum
{
   CONTINUE_AF_RANGE_RESET,
   CONTINUE_AF_RANGE_INIT,
   CONTINUE_AF_RANGE_ERROR_OVER_POSITION,		/*ERR_OVER_FOCUS_POSITION*/
   CONTINUE_AF_RANGE_ERROR_UNDER_POSITION,		/*ERR_UNDER_FOCUS_POSITION*/
   CONTINUE_AF_RANGE_ERROR,
   CONTINUE_AF_RANGE_TOTAL,
};

enum
{
	FOCUS_MODE_NORMAL,
	FOCUS_MODE_MACRO,
	FOCUS_MODE_FULL_SEARCH_MODE,
	FOCUS_MODE_SMALL_SEARCH_MODE,
	FOCUS_MODE_INFINITY_FIXED,

	FOCUS_MODE_UNKNOW = 0xFFFF,
};

enum
{
	ERR_NONE,
	ERR_UNKNOW,
	ERR_INVALID_PARA,
	ERR_ABORT,
	ERR_NOT_CALIBRATE,
	ERR_NO_PREVIEW_EXPOSURE_VALUE,

	/* FOCUS*/
	ERR_FOCUS_UNKNOW_MOVING_CONFIG,
	ERR_FOCUS_IS_MOVING,
	ERR_FOCUS_AF_SEARCH_OFF,
	ERR_IRIS_IS_MOVING,

	ERR_FILE_VERIFY_CHECKSUM_FAIL,
	
	ERR_FACE_TRACKING_IS_ILLEGAL,
	ERR_SCENE_MODE_CHANGED,

};


#if ( defined LENS_VCM7002 )
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		1
#define AF_WIN_SEGMENT			 	6	/*8*/
#elif ( defined LENS_LARGAN9377A8D )
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		1
#define AF_WIN_SEGMENT			 	6	/*8*/
#define IDX_DIST_INF_NOT_CONDISION_CALIBRATION 
#elif ( defined LENS_LARGAN9547A1 )
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		1
#define AF_WIN_SEGMENT			 	6	/*8 */
#define IDX_DIST_INF_NOT_CONDISION_CALIBRATION 
#elif ( defined LENS_LARGAN40025A1 )
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		1
#define AF_WIN_SEGMENT			 	6	
#define IDX_DIST_INF_NOT_CONDISION_CALIBRATION 
#elif ( defined IU091F4_Z )
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		1
#define AF_WIN_SEGMENT				6	
#define IDX_DIST_INF_NOT_CONDISION_CALIBRATION 
#elif ( defined VKY_AD5816 )
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		1
#define AF_WIN_SEGMENT				6	
#define IDX_DIST_INF_NOT_CONDISION_CALIBRATION 
#elif ( defined AF85C_S45 )
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		1
#define AF_WIN_SEGMENT				6	
#define IDX_DIST_INF_NOT_CONDISION_CALIBRATION 
#elif ( defined GLORY )
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		1
#define AF_WIN_SEGMENT				6	
#define IDX_DIST_INF_NOT_CONDISION_CALIBRATION
#elif ( defined GAGA )
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		10
#define AF_WIN_SEGMENT				6	
#define IDX_DIST_INF_NOT_CONDISION_CALIBRATION 




#elif ( defined LENS_VCM3H2 )
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		1
#define AF_WIN_SEGMENT			 	6	
#else
#define FOCUS_DIST_TOTAL 				17
#define FOCUS_TOTAL_DEF_POS 	 		7
#define AF_WIN_SEGMENT			 	6	/*8*/ 
#endif



#if 1

    #if	1	/*def LENS_DCU14 KX1193: Macro distance is closer than spec*/
    #define AF_MACRO_USE_CLOSER_DISTANCE
    #endif

   	#if 1
    #define AF_MACRO_USE_FW_LIMITED_POSITION
    #endif

    #if 1
    #define ENABLE_LENS_SHADING_COMPENSATION
    #endif

    	/*Define Signle Windows AF
    	#ifdef AF_AF_REALDY_WAIT_MSG_ID
        #define AF_AFS_REALDY_WAIT_MSG_ID
    	#else
        #define AF_AFS_REALDY_WAIT_HCE_CB
    	#endif
        */
    	/*Define Enable / Disable Multi Windows AF*/
    #if 1
        #define AF_RUN_MULTI_WINDOWS_ENABLE
		#ifdef AF_AF_REALDY_WAIT_MSG_ID
		#define AF_AFM_REALDY_WAIT_MSG_ID
	    #else
	    #define AF_AFM_REALDY_WAIT_HCE_CB
	    #endif
	    #define AF_MULTI_WINDOWS_SUB_WIN_HOR_CNT      3
	    #define AF_MULTI_WINDOWS_SUB_WIN_VER_CNT      2
	    #define AF_MULTI_WINDOWS_SUB_WIN_SIZE_X       80
	    #define AF_MULTI_WINDOWS_SUB_WIN_SIZE_Y       128
		#if 0
	  	#define ENABLE_MULTI_WINDOWS_CALIBRATION
	    #endif
    #endif
	
   	#if 0
    #define AF_ENABLE_AF_LAMP
   	#endif

   	#if 1	
   	#define AF_ENABLE_AUTO_MACRO
   	#endif

#endif



#if 1 
#define ENABLE_ONE_SEARCH_DYNAM_STEPS
#endif

/*add for using in afALG_SearchRangeGet() and FOCUS_Calibration_Check_Focus()*/
#if ( defined LENS_VCM7002 )
#define FOCUS_CALI_OPTICAL_EXTEND		12
#elif ( defined LENS_LARGAN9377A8D )
#define FOCUS_CALI_OPTICAL_EXTEND		12
#elif ( defined LENS_LARGAN9547A1  )
#define FOCUS_CALI_OPTICAL_EXTEND		12
#elif ( defined LENS_LARGAN40025A1  )
#define FOCUS_CALI_OPTICAL_EXTEND		12
#elif ( defined IU091F4_Z  )
#define FOCUS_CALI_OPTICAL_EXTEND		12
#elif ( defined LENS_VCM3H2 )
#define FOCUS_CALI_OPTICAL_EXTEND		12
#else
#define FOCUS_CALI_OPTICAL_EXTEND		12
#endif

#define FOCUS_CALI_REPEAT_CNT            1

#if 0
#define AF_2M_CALIBRATION		/*For Color Chart*/
#elif ( defined LENS_VCM3H2) || (defined LENS_LARGAN9547A1) || (defined LENS_LARGAN9377A8D ) || (defined LENS_LARGAN40025A1 )
#define AF_MACRO_CALIBRATION 	/*For 50cm Start chart*/
#elif (defined IU091F4_Z )
#define AF_MACRO_CALIBRATION
#else
#define AF_INF_CALIBRATION	/*For collimator*/
#endif

#define IDX_DIST_NORMAL_DEFAULT            IDX_DIST_INF /*(IDX_DIST_1500MM)	(IDX_DIST_2000MM)*/	/*Modify For Casio*/

#if  ( defined LENS_VCM7002 )
#define IDX_DIST_MACRO_DEFAULT            (IDX_DIST_MACRO)
#elif ( defined LENS_LARGAN9377A8D )
#define IDX_DIST_MACRO_DEFAULT            (IDX_DIST_MACRO)
#elif ( defined LENS_LARGAN9547A1 )
#define IDX_DIST_MACRO_DEFAULT            (IDX_DIST_MACRO)
#elif ( defined LENS_LARGAN40025A1 )
#define IDX_DIST_MACRO_DEFAULT            (IDX_DIST_MACRO)
#elif ( defined IU091F4_Z )
#define IDX_DIST_MACRO_DEFAULT            (IDX_DIST_MACRO)
#elif ( defined LENS_VCM3H2 )
#define IDX_DIST_MACRO_DEFAULT            (IDX_DIST_MACRO)
#elif ( defined LENS_ARIES )
#endif

#ifdef AF_2M_CALIBRATION
#define IDX_DIST_CALIBRATION                (IDX_DIST_2000MM)
#elif defined AF_MACRO_CALIBRATION
#define IDX_DIST_CALIBRATION                (IDX_DIST_0100MM)
/*#define IDX_DIST_CALIBRATION                (IDX_DIST_0500MM)*/
#else
#define IDX_DIST_CALIBRATION                (IDX_DIST_INF)
#endif

#define  AFOCUS_EVENT_AFS_STATISTICS_READY    0x00000001
#define  AFOCUS_EVENT_AFM_STATISTICS_READY    0x00000002

/*for AF Continued Searching*/
#if 1
   #define  CONTINUE_SEARCH
#endif

#pragma pack(1)
typedef struct
{
	SP5K_MUTEX				txMutex;
	SP5K_EVENT_FLAGS_GROUP	txEvent, txAckEvent;
	SP5K_THREAD 				*txThread;

	SP5K_EVENT_FLAGS_GROUP  txDelayEvent;
	SP5K_EVENT_FLAGS_GROUP  txAFS_Even , txAFM_Even;

	UINT32	uFlag, uMovingDir;
	UINT32	uFlag2;

	UINT16	wIdxDist;
	UINT16	wMode;
	UINT16	wBackUpMode;
	UINT16	wAFArea;

	UINT16	wAutoMode;
	UINT16	wVdoMode;
	  
	SINT16	iCurPos;
	UINT32	 uCurDist;
	   
	SINT16	iMoveStopPos;
	   UINT32	uDelayEventFlags;
	   UINT32	uZoomPosIdx;
	   UINT32	 uContinuedSearching;
	   UINT16	uFocusFramePosition;
	   UINT16	uFocusFrameSegment;
	   SINT32	iFocusDeFocusStep;
	   UINT32	ulContinueAF_ConditionIndexY;
	   SINT32	ilContinueAF_ConditionY[5];
	   UINT32	ulContinueAF_LastEdge;
	   UINT32	ulContinueAF_BestEdge;
	   UINT32	ulContinueAF_TarceEdge;  
	   SINT32	ulContinueAF_IndexCount;
	   UINT32	ulContinueAFRange;  
	   SINT32   iMoveStep;
	   SINT8   iReEntey;
	   SINT8 iDrivePWMFocusBoot; 
	   SINT8 iInitFocusFinsh;	
	   /*UINT16  uMultiWinSubWinHorCnt;*/
	   /*UINT16  uMultiWinSubWinVerCnt;*/
	   /*UINT16  uMultiWinSubWinSizeX*/
	   /*UINT16  uMultiWinSubWinSizeY;*/
	   /*UINT16  uMultiWindowsSizeX;*/
	   /*UINT16  uMultiWindowsSizeY;*/
	   /*UINT16  uMultiWindowsStartX;*/
	   /*UINT16  uMultiWindowsStartY;*/
	   SINT16 iFocusPreBestStep;
	   SINT16 iContinuePreBestPos;
	   UINT16  uAfManualObjectDisIdx;
	   UINT16  iFaceTrackingWinX;
	   UINT16  iFaceTrackingWinY;
	   UINT16  uFaceTrackingWinV;
	   UINT16  uFaceTrackingWinH;
	   UINT16  uObjectTrackingWinX;
	   UINT16  uObjectTrackingWinY;
	   UINT16  uObjectTrackingWinV;
	   UINT16  uObjectTrackingWinH;
	   UINT16  uObjectTrackingFrameW;
	   UINT16  uObjectTrackingFrameH;
	   UINT16  uObjectTrackingWinX_Last;
	   UINT16  uObjectTrackingWinY_Last;
	   UINT16  uObjectTrackingReDoCnt;
	   UINT16 uwRegionType;
	   UINT16 uwTotalRegion;
	   UINT16 uwSubWinNumX;
	   UINT16 uwSubWinNumY;
	   UINT16 uPWMTimeCnt;
	   UINT16 uPFType;
	   UINT16 uFNumNominator;
	   UINT16 uFNumDenominator;
	   SINT32 iCenYDiffEntry;	
	   SINT32 iAfPreLv1000;	    
       SINT16 iCafSearchRangeStart;
	   SINT16 iBacklash;
	   UINT32	 uMfDist;  
	   UINT8 uGyroStatus;
	} S__FOCUS;
#pragma pack()

#define AF_FLAG_Video_SELECT_ON                0x00000001
#define AF_FLAG_MOVING                         0x00000002
#define AF_FLAG_MOVING_INIT_POS_NEAR_CLOSE     0x00000004
#define AF_FLAG_POWERON_TO_HOME                0x00000008
#define AF_FLAG_DETECT_AFPI_CHANGED            0x00000010
#define AF_FLAG_BEGIN_SEARCH                   0x00000020
#define AF_FLAG_SEARCHING                      0x00000040
#define AF_FLAG_CALIBRATION                    0x00000080
#define AF_FLAG_DO_AF_SEARCH                   0x00000100
#define AF_FLAG_SHOW_AF_WINDOWS                0x00000200
#define AF_FLAG_DO_AF_DEFOCUS                  0x00000400
#define AF_FLAG_DO_CONTINUE_AF                 0x00000800
#define AF_FORCE_TO_BREAK_LOOP                 0x00001000
#define AF_FLAG_CONTINUE_AF_MOVING             0x00002000
#define AF_FLAG_CONTINUE_AF_FORCE_STOP         0x00004000
#define AF_FLAG_FOCUS_RUN_HIGHT_SPEED          0x00008000
#define AF_FLAG_CW_STEP_DECTECT_HP_ERROR       0x00010000
#define AF_FLAG_CCW_STEP_DECTECT_HP_ERROR      0x00020000
#define AF_FLAG_CW_STEP_DECTECT_OVER_STEP      0x00040000
#define AF_FLAG_CCW_STEP_DECTECT_OVER_STEP     0x00080000
#define AF_FLAG_AF_EV_COMPENSATION             0x00100000
#define AF_FLAG_DECTECT_POWER_UNDER_VALUES     0x00200000
#define AF_FLAG_AF_MANUAL_OBJECT_DIST          0x00400000
#define AF_FLAG_AF_FACE_TRACKING_ENABLE        0x00800000
#define AF_FLAG_AF_FACE_TRACKING_START         0x01000000
#define AF_FLAG_AF_FACE_TRACKING_WINDOW        0x02000000
#define AF_FLAG_LRP_WEIGHT_ON                  0x04000000
#define AF_FLAG_PF_SET_OUTDOOR                 0x08000000
#define AF_FLAG_CALIB_START_POS_ON             0x10000000
#define AF_FLAG_AFOPEN_READY_ON                0x20000000
#define AF_FLAG_TSRC_Table_ON           	   0x40000000
#define AF_FLAG_DO_LSC_CALIBRATION			   0x80000000

/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CALIBRATION_ON()                  afALG_StatusFlagSet(AF_FLAG_CALIBRATION,1)
#define afALG_SET_CALIBRATION_OFF()                 afALG_StatusFlagSet(AF_FLAG_CALIBRATION,0)
#define afALG_IS_CALIBRATION()                      afALG_StatusFlagGet(AF_FLAG_CALIBRATION)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_SEARCHING_ON()                    afALG_StatusFlagSet(AF_FLAG_SEARCHING,1)
#define afALG_SET_SEARCHING_OFF()                   afALG_StatusFlagSet(AF_FLAG_SEARCHING,0)
#define afALG_IS_SEARCHING()                        afALG_StatusFlagGet(AF_FLAG_SEARCHING)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_BEGIN_SEARCH_ON()                 afALG_StatusFlagSet(AF_FLAG_BEGIN_SEARCH,1)
#define afALG_SET_BEGIN_SEARCH_OFF()                afALG_StatusFlagSet(AF_FLAG_BEGIN_SEARCH,0)
#define afALG_IS_BEGIN_SEARCH()                     afALG_StatusFlagGet(AF_FLAG_BEGIN_SEARCH)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_DETECT_AFPI_CHANGED_ON()          afALG_StatusFlagSet(AF_FLAG_DETECT_AFPI_CHANGED,1)
#define afALG_SET_DETECT_AFPI_CHANGED_OFF()         afALG_StatusFlagSet(AF_FLAG_DETECT_AFPI_CHANGED,0)
#define afALG_IS_DETECT_AFPI_CHANGED()              afALG_StatusFlagGet(AF_FLAG_DETECT_AFPI_CHANGED)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_POWERON_TO_HOME_ON()              afALG_StatusFlagSet(AF_FLAG_POWERON_TO_HOME,1)
#define afALG_SET_POWERON_TO_HOME_OFF()             afALG_StatusFlagSet(AF_FLAG_POWERON_TO_HOME,0)
#define afALG_ISALREADY_POWERON_TO_HOME()           afALG_StatusFlagGet(AF_FLAG_POWERON_TO_HOME)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_MOVING_INIT_POS_NEAR_CLOSE()      afALG_StatusFlagSet(AF_FLAG_MOVING_INIT_POS_NEAR_CLOSE,1)
#define afALG_SET_MOVING_INIT_POS_NOT_NEAR_CLOSE()  afALG_StatusFlagSet(AF_FLAG_MOVING_INIT_POS_NEAR_CLOSE,0)
#define afALG_IS_MOVING_INIT_POS_NEAR_CLOSE()       afALG_StatusFlagGet(AF_FLAG_MOVING_INIT_POS_NEAR_CLOSE)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_MOVING()                          afALG_StatusFlagSet(AF_FLAG_MOVING,1)
#define afALG_SET_NOT_MOVING()                      afALG_StatusFlagSet(AF_FLAG_MOVING,0)
#define afALG_IS_MOVING()                           afALG_StatusFlagGet(AF_FLAG_MOVING)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_Video_SELECT_ON()                  afALG_StatusFlagSet(AF_FLAG_Video_SELECT_ON,1)
#define afALG_SET_Video_SELECT_OFF()                 afALG_StatusFlagSet(AF_FLAG_Video_SELECT_ON,0)
#define afALG_IS_Video_SELECT_ON()                   afALG_StatusFlagGet(AF_FLAG_Video_SELECT_ON)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_DO_AF_SEARCH_ON()                 afALG_StatusFlagSet(AF_FLAG_DO_AF_SEARCH,1)
#define afALG_SET_DO_AF_SEARCH_OFF()                afALG_StatusFlagSet(AF_FLAG_DO_AF_SEARCH,0)
#define afALG_IS_DO_AF_SEARCH_ON()                  afALG_StatusFlagGet(AF_FLAG_DO_AF_SEARCH)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_DO_AF_DEFOCUS_ON()                afALG_StatusFlagSet(AF_FLAG_DO_AF_DEFOCUS,1)
#define afALG_SET_DO_AF_DEFOCUS_OFF()               afALG_StatusFlagSet(AF_FLAG_DO_AF_DEFOCUS,0)
#define afALG_IS_DO_AF_DEFOCUS()                    afALG_StatusFlagGet(AF_FLAG_DO_AF_DEFOCUS)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CONTINUE_AF_ON()                  afALG_StatusFlagSet(AF_FLAG_DO_CONTINUE_AF,1)
#define afALG_SET_CONTINUE_AF_OFF()                 afALG_StatusFlagSet(AF_FLAG_DO_CONTINUE_AF,0)
#define afALG_IS_CONTINUE_AF_ON()                   afALG_StatusFlagGet(AF_FLAG_DO_CONTINUE_AF)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_FORCE_TO_BREAK_LOOP_ON()          afALG_StatusFlagSet(AF_FORCE_TO_BREAK_LOOP,1)
#define afALG_SET_FORCE_TO_BREAK_LOOP_OFF()         afALG_StatusFlagSet(AF_FORCE_TO_BREAK_LOOP,0)
#define afALG_IS_FORCE_TO_BREAK_LOOP()              afALG_StatusFlagGet(AF_FORCE_TO_BREAK_LOOP)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CONTINUE_AF_MOVING()              afALG_StatusFlagSet(AF_FLAG_CONTINUE_AF_MOVING,1)
#define afALG_SET_CONTINUE_AF_NOT_MOVING()          afALG_StatusFlagSet(AF_FLAG_CONTINUE_AF_MOVING,0)
#define afALG_IS_CONTINUE_AF_MOVING()               afALG_StatusFlagGet(AF_FLAG_CONTINUE_AF_MOVING)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CONTINUE_AF_FORCE_STOP()          afALG_StatusFlagSet(AF_FLAG_CONTINUE_AF_FORCE_STOP,1)
#define afALG_SET_CONTINUE_AF_FORCE_STOP_OFF()      afALG_StatusFlagSet(AF_FLAG_CONTINUE_AF_FORCE_STOP,0)
#define afALG_IS_CONTINUE_AF_FORCE_STOP()           afALG_StatusFlagGet(AF_FLAG_CONTINUE_AF_FORCE_STOP)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_FOCUS_RUN_HIGHT_SPEED_NO()        afALG_StatusFlagSet(AF_FLAG_FOCUS_RUN_HIGHT_SPEED,1)
#define afALG_SET_FOCUS_RUN_HIGHT_SPEED_OFF()       afALG_StatusFlagSet(AF_FLAG_FOCUS_RUN_HIGHT_SPEED,0)
#define afALG_IS_FOCUS_RUN_HIGHT_SPEED_NO()         afALG_StatusFlagGet(AF_FLAG_FOCUS_RUN_HIGHT_SPEED)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CW_STEP_DECTECT_HP_ERROR()       afALG_StatusFlagSet(AF_FLAG_CW_STEP_DECTECT_HP_ERROR,1)
#define afALG_CLR_CW_STEP_DECTECT_HP_ERROR()       afALG_StatusFlagSet(AF_FLAG_CW_STEP_DECTECT_HP_ERROR,0)
#define afALG_IS_CW_STEP_DECTECT_HP_ERROR()        afALG_StatusFlagGet(AF_FLAG_CW_STEP_DECTECT_HP_ERROR)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CCW_STEP_DECTECT_HP_ERROR()      afALG_StatusFlagSet(AF_FLAG_CCW_STEP_DECTECT_HP_ERROR,1)
#define afALG_CLR_CCW_STEP_DECTECT_HP_ERROR()      afALG_StatusFlagSet(AF_FLAG_CCW_STEP_DECTECT_HP_ERROR,0)
#define afALG_IS_CCW_STEP_DECTECT_HP_ERROR()       afALG_StatusFlagGet(AF_FLAG_CCW_STEP_DECTECT_HP_ERROR)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CCW_STEP_DECTECT_OVER_STEP()      afALG_StatusFlagSet(AF_FLAG_CCW_STEP_DECTECT_OVER_STEP,1)
#define afALG_CLR_CCW_STEP_DECTECT_OVER_STEP()      afALG_StatusFlagSet(AF_FLAG_CCW_STEP_DECTECT_OVER_STEP,0)
#define afALG_IS_CCW_STEP_DECTECT_OVER_STEP()       afALG_StatusFlagGet(AF_FLAG_CCW_STEP_DECTECT_OVER_STEP)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CW_STEP_DECTECT_OVER_STEP()       afALG_StatusFlagSet(AF_FLAG_CW_STEP_DECTECT_OVER_STEP,1)
#define afALG_CLR_CW_STEP_DECTECT_OVER_STEP()       afALG_StatusFlagSet(AF_FLAG_CW_STEP_DECTECT_OVER_STEP,0)
#define afALG_IS_CW_STEP_DECTECT_OVER_STEP()        afALG_StatusFlagGet(AF_FLAG_CW_STEP_DECTECT_OVER_STEP)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_AF_EV_COMPENSATION()              afALG_StatusFlagSet(AF_FLAG_AF_EV_COMPENSATION,1)
#define afALG_CLR_AF_EV_COMPENSATION()              afALG_StatusFlagSet(AF_FLAG_AF_EV_COMPENSATION,0)
#define afALG_IS_AF_EV_COMPENSATION()               afALG_StatusFlagGet(AF_FLAG_AF_EV_COMPENSATION)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_DECTECT_POWER_UNDER_VALUES()      afALG_StatusFlagSet(AF_FLAG_DECTECT_POWER_UNDER_VALUES,1)
#define afALG_CLR_DECTECT_POWER_UNDER_VALUES()      afALG_StatusFlagSet(AF_FLAG_DECTECT_POWER_UNDER_VALUES,0)
#define afALG_IS_DECTECT_POWER_UNDER_VALUES()       afALG_StatusFlagGet(AF_FLAG_DECTECT_POWER_UNDER_VALUES)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_AF_MANUAL_OBJECT_DIST()           afALG_StatusFlagSet(AF_FLAG_AF_MANUAL_OBJECT_DIST,1)
#define afALG_CLR_AF_MANUAL_OBJECT_DIST()           afALG_StatusFlagSet(AF_FLAG_AF_MANUAL_OBJECT_DIST,0)
#define afALG_IS_AF_MANUAL_OBJECT_DIST()            afALG_StatusFlagGet(AF_FLAG_AF_MANUAL_OBJECT_DIST)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_AF_FACE_TRACKING_ENABLED()        afALG_StatusFlagSet(AF_FLAG_AF_FACE_TRACKING_ENABLE,1)
#define afALG_SET_AF_FACE_TRACKING_DISABLED()       afALG_StatusFlagSet(AF_FLAG_AF_FACE_TRACKING_ENABLE,0)
#define afALG_IS_AF_FACE_TRACKING_ENABLED()         afALG_StatusFlagGet(AF_FLAG_AF_FACE_TRACKING_ENABLE)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_AF_FACE_TRACKING_START()          afALG_StatusFlagSet(AF_FLAG_AF_FACE_TRACKING_START,1)
#define afALG_SET_AF_FACE_TRACKING_END()           	afALG_StatusFlagSet(AF_FLAG_AF_FACE_TRACKING_START,0)
#define afALG_IS_AF_FACE_TRACKING_START()           afALG_StatusFlagGet(AF_FLAG_AF_FACE_TRACKING_START)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_USE_FACE_TRACKING_WINDOW()        afALG_StatusFlagSet(AF_FLAG_AF_FACE_TRACKING_WINDOW,1)
#define afALG_NOT_USE_FACE_TRACKING_WINDOW()        afALG_StatusFlagSet(AF_FLAG_AF_FACE_TRACKING_WINDOW,0)
#define afALG_IS_USE_FACE_TRACKING_WINDOW()         afALG_StatusFlagGet(AF_FLAG_AF_FACE_TRACKING_WINDOW)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_LRP_WEIGHT()                	afALG_StatusFlagSet(AF_FLAG_LRP_WEIGHT_ON,1)
#define afALG_CLR_LRP_WEIGHT()                	afALG_StatusFlagSet(AF_FLAG_LRP_WEIGHT_ON,0)
#define afALG_IS_LRP_WEIGHT_ON()              	afALG_StatusFlagGet(AF_FLAG_LRP_WEIGHT_ON)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_PF_OUTDOOR_ON()           	afALG_StatusFlagSet(AF_FLAG_PF_SET_OUTDOOR,1)
#define afALG_SET_PF_OUTDOOR_OFF()           	afALG_StatusFlagSet(AF_FLAG_PF_SET_OUTDOOR,0)
#define afALG_IS_PF_OUTDOOR_ON()            	afALG_StatusFlagGet(AF_FLAG_PF_SET_OUTDOOR)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CALIB_START_POS()              afALG_StatusFlagSet(AF_FLAG_CALIB_START_POS_ON,1)
#define afALG_CLR_CALIB_START_POS()              afALG_StatusFlagSet(AF_FLAG_CALIB_START_POS_ON,0)
#define afALG_IS_CALIB_START_POS()               afALG_StatusFlagGet(AF_FLAG_CALIB_START_POS_ON)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_GET_EDGE_READY_ON()              	afALG_StatusFlagSet(AF_FLAG_AFOPEN_READY_ON,1)
#define afALG_SET_GET_EDGE_READY_OFF()             	afALG_StatusFlagSet(AF_FLAG_AFOPEN_READY_ON,0)
#define afALG_IS_GET_EDGE_READY()               		afALG_StatusFlagGet(AF_FLAG_AFOPEN_READY_ON)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_TSRC_Table_ON()              	afALG_StatusFlagSet(AF_FLAG_TSRC_Table_ON,1)
#define afALG_SET_TSRC_Table_OFF()              afALG_StatusFlagSet(AF_FLAG_TSRC_Table_ON,0)
#define afALG_IS_TSRC_Table()               		afALG_StatusFlagGet(AF_FLAG_TSRC_Table_ON)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_LSC_CALIBRATION_ON()              afALG_StatusFlagSet(AF_FLAG_DO_LSC_CALIBRATION,1)
#define afALG_SET_LSC_CALIBRATION_OFF()             afALG_StatusFlagSet(AF_FLAG_DO_LSC_CALIBRATION,0)
#define afALG_IS_LSC_CALIBRATION_ON()               afALG_StatusFlagGet(AF_FLAG_DO_LSC_CALIBRATION)
/*---------------------------------------------------------------------------------------------------------------*/

#define AF_FLAG_ENABLE_FAF							0x00000001
#define AF_FLAG_POWERON_FOCUS_MOVING				0x00000002
#define AF_FLAG_THREAD_ACK_DONE						0x00000004 
#define AF_FLAG_MOVIE_PF_MODE						0x00000008
#define AF_FLAG_NORMAL_EXPEND_FOCUS_SUCCESS			0x00000010
#define AF_FLAG_MACRO_EXPEND_FOCUS_SUCCESS			0x00000020
#define AF_FLAG_THERAD_POWER_ON_EVEN				0x00000040  
#define AF_FLAG_CAF_EDGE_READLY                  	0x00000080  
#define AF_FLAG_USE_IIR_FILTER_EDGE					0x00000100
#define AF_FLAG_EXPEND_NORMAL_SEARCH				0x00000200
#define AF_FLAG_EXPEND_MACRO_SEARCH					0x00000400
#define AF_FLAG_ALLOW_FAF							0x00000800
#define AF_FLAG_ENABLE_AUTOMACRO					0x00001000
#define AF_FLAG_THERAD_RUNNING				    	0x00002000
#define AF_FLAG_THERAD_RESET_TO_HOME				0x00004000
#define AF_FLAG_OBJECT_TRACKING_WINDOW				0x00008000
#define AF_FLAG_OBJECT_TRACKING_START				0x00010000
#define AF_FLAG_FIRST_CONTINUE_AF_START				0x00020000
#define AF_FLAG_CONTINUE_AF_PWM_TIMER_CONFIG		0x00040000
#define AF_FLAG_CONTINUE_AF_PWM_TIMER_CONFIG2		0x00080000
#define AF_FLAG_TEST_LOST_STEP_AND_BACKLASH			0x00100000
#define AF_FLAG_ENABLE_LOW_LIGHT_CLOSE_FAF			0x00200000
#define AF_FLAG_BACKGROUND_3A_DISABLE_FAF			0x00400000
#define AF_FLAG_RUNNING_AF_OPEN_PROCESS_FRAME  		0x00800000
#define AF_FLAG_SNAP1_AF_REFERENCE_CONTINUE_AF   	0x01000000 
#define AF_FLAG_REFERENCE_SINGLE_WINDOWS_EDGE    	0x02000000 
#define AF_FLAG_SEARCH_DYNAM_STEPS               	0x04000000 
#define AF_FLAG_AUTO_FOCUS_EVEN_DONE             	0x08000000 
#define AF_FLAG_ROI_WINDOWS                      	0x10000000 
#define AF_FLAG_CONTIUNE_EDGE                    	0x20000000 
#define AF_FLAG_SET_CONTIUNE_TRACE_EDGE          	0x40000000 
#define AF_FLAG_SET_ROI_AF_ABORT                 	0x80000000 

/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_ENABLE_FAF_ON()               afALG_StatusFlagSet(AF_FLAG_ENABLE_FAF,1)
#define afALG_SET_ENABLE_FAF_OFF()              afALG_StatusFlagSet(AF_FLAG_ENABLE_FAF,0)
#define afALG_IS_ENABLE_FAF()                   afALG_StatusFlagGet(AF_FLAG_ENABLE_FAF)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_POWERON_FOCUS_MOVING_ON()	  	afALG_StatusFlagSet(AF_FLAG_POWERON_FOCUS_MOVING,1)
#define afALG_SET_POWERON_FOCUS_MOVING_OFF()		afALG_StatusFlagSet(AF_FLAG_POWERON_FOCUS_MOVING,0)
#define afALG_IS_POWERON_FOCUS_MOVING()		  		afALG_StatusFlagGet(AF_FLAG_POWERON_FOCUS_MOVING)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_FLAG_SET_FOCUS_MOVING_ACK_DONE()  afALG_StatusFlagSet(AF_FLAG_THREAD_ACK_DONE,1)
#define afALG_FLAG_CLR_FOCUS_MOVING_ACK_DONE()  afALG_StatusFlagSet(AF_FLAG_THREAD_ACK_DONE,0)
#define afALG_FLAG_IS_FOCUS_MOVING_ACK_DONE()		afALG_StatusFlagGet(AF_FLAG_THREAD_ACK_DONE)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_PRESS_MOVIE_KEY_PF_MODE()			afALG_StatusFlagSet(AF_FLAG_MOVIE_PF_MODE,1)
#define afALG_CLR_PRESS_MOVIE_KEY_PF_MODE()			afALG_StatusFlagSet(AF_FLAG_MOVIE_PF_MODE,0)
#define afALG_IS_PRESS_MOVIE_KEY_PF_MODE()			afALG_StatusFlagGet(AF_FLAG_MOVIE_PF_MODE)
/*---------------------------------------------------------------------------------------------------------------*/
/*focus search from Macro to Normal range and focusing success*/
#define afALG_SET_NORMAL_EXPEND_FOCUS_SUCCESS()	afALG_StatusFlagSet(AF_FLAG_NORMAL_EXPEND_FOCUS_SUCCESS,1)
#define afALG_CLR_NORMAL_EXPEND_FOCUS_SUCCESS()	afALG_StatusFlagSet(AF_FLAG_NORMAL_EXPEND_FOCUS_SUCCESS,0)
#define afALG_IS_NORMAL_EXPEND_FOCUS_SUCCESS()	afALG_StatusFlagGet(AF_FLAG_NORMAL_EXPEND_FOCUS_SUCCESS)
/*---------------------------------------------------------------------------------------------------------------*/
/*focus search from Normal to Macro range and focusing success*/
#define afALG_SET_MACRO_EXPEND_FOCUS_SUCCESS()	afALG_StatusFlagSet(AF_FLAG_MACRO_EXPEND_FOCUS_SUCCESS,1)
#define afALG_CLR_MACRO_EXPEND_FOCUS_SUCCESS()	afALG_StatusFlagSet(AF_FLAG_MACRO_EXPEND_FOCUS_SUCCESS,0)
#define afALG_IS_MACRO_EXPEND_FOCUS_SUCCESS()		afALG_StatusFlagGet(AF_FLAG_MACRO_EXPEND_FOCUS_SUCCESS)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_THERAD_POWER_ON_EVEN_ON()			afALG_StatusFlagSet(AF_FLAG_THERAD_POWER_ON_EVEN,1)
#define afALG_CLR_THERAD_POWER_ON_EVEN_ON()			afALG_StatusFlagSet(AF_FLAG_THERAD_POWER_ON_EVEN,0)
#define afALG_IS_THERAD_POWER_ON_EVEN_ON()			afALG_StatusFlagGet(AF_FLAG_THERAD_POWER_ON_EVEN)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CAF_EDGE_READLY()		                afALG_StatusFlagSet(AF_FLAG_CAF_EDGE_READLY,1)
#define afALG_CLR_CAF_EDGE_READLY()	                	afALG_StatusFlagSet(AF_FLAG_CAF_EDGE_READLY)
#define afALG_IS_CAF_EDGE_READLY()		                afALG_StatusFlagGet(AF_FLAG_CAF_EDGE_READLY)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_USE_IIR_FILTER_EDGE()								afALG_StatusFlagSet(AF_FLAG_USE_IIR_FILTER_EDGE,1)
#define afALG_NOT_USE_IIR_FILTER_EDGE()						afALG_StatusFlagSet(AF_FLAG_USE_IIR_FILTER_EDGE,0)
#define afALG_IS_USE_IIR_FILTER_EDGE()		    		afALG_StatusFlagGet(AF_FLAG_USE_IIR_FILTER_EDGE)
/*---------------------------------------------------------------------------------------------------------------*/
/* focus search from Macro to Normal range */
#define afALG_SET_EXPEND_NORMAL_SEARCH()		afALG_StatusFlagSet(AF_FLAG_EXPEND_NORMAL_SEARCH,1)
#define afALG_CLR_EXPEND_NORMAL_SEARCH()		afALG_StatusFlagSet(AF_FLAG_EXPEND_NORMAL_SEARCH,0)
#define afALG_IS_EXPEND_NORMAL_SEARCH()			afALG_StatusFlagGet(AF_FLAG_EXPEND_NORMAL_SEARCH)
/*---------------------------------------------------------------------------------------------------------------*/
/* focus search from Normal to Macro range */
#define afALG_SET_EXPEND_MACRO_SEARCH()		afALG_StatusFlagSet(AF_FLAG_EXPEND_MACRO_SEARCH,1)
#define afALG_CLR_EXPEND_MACRO_SEARCH()		afALG_StatusFlagSet(AF_FLAG_EXPEND_MACRO_SEARCH,0)
#define afALG_IS_EXPEND_MACRO_SEARCH()		afALG_StatusFlagGet(AF_FLAG_EXPEND_MACRO_SEARCH)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_ALLOW_FAF()				afALG_StatusFlagSet(AF_FLAG_ALLOW_FAF,1)
#define afALG_NOT_ALLOW_FAF()				afALG_StatusFlagSet(AF_FLAG_ALLOW_FAF,0)
#define afALG_IS_ALLOW_FAF()				afALG_StatusFlagGet(AF_FLAG_ALLOW_FAF)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_ENABLE_AUTOMACRO()		afALG_StatusFlagSet(AF_FLAG_ENABLE_AUTOMACRO,1)
#define afALG_SET_DISABLE_AUTOMACRO()		afALG_StatusFlagSet(AF_FLAG_ENABLE_AUTOMACRO,0)
#define afALG_IS_ENABLE_AUTOMACRO()			afALG_StatusFlagGet(AF_FLAG_ENABLE_AUTOMACRO)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_THERAD_RUNNING()			afALG_StatusFlagSet(AF_FLAG_THERAD_RUNNING,1)
#define afALG_SET_THERAD_STOP()		    	afALG_StatusFlagSet(AF_FLAG_THERAD_RUNNING,0)
#define afALG_IS_THERAD_RUNNING()		    afALG_StatusFlagGet(AF_FLAG_THERAD_RUNNING)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CAF_BEST_EDGE()		    afALG_StatusFlagSet(AF_FLAG_THERAD_RESET_TO_HOME,1)
#define afALG_CLR_CAF_BEST_EDGE()		    afALG_StatusFlagSet(AF_FLAG_THERAD_RESET_TO_HOME,0)
#define afALG_IS_SET_CAF_BEST_EDGE()		afALG_StatusFlagGet(AF_FLAG_THERAD_RESET_TO_HOME)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_USE_OBJECT_TRACKING_WINDOW()           afALG_StatusFlagSet(AF_FLAG_OBJECT_TRACKING_WINDOW,1)
#define afALG_NOT_USE_OBJECT_TRACKING_WINDOW()           afALG_StatusFlagSet(AF_FLAG_OBJECT_TRACKING_WINDOW,0)
#define afALG_IS_USE_OBJECT_TRACKING_WINDOW()            afALG_StatusFlagGet(AF_FLAG_OBJECT_TRACKING_WINDOW)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_START_OBJECT_TRACKING()           		afALG_StatusFlagSet(AF_FLAG_OBJECT_TRACKING_START,1)
#define afALG_END_OBJECT_TRACKING()           			afALG_StatusFlagSet(AF_FLAG_OBJECT_TRACKING_START,0)
#define afALG_IS_START_OBJECT_TRACKING()            afALG_StatusFlagGet(AF_FLAG_OBJECT_TRACKING_START)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_DOING_CONTINUE_AF_ON()               	afALG_StatusFlagSet(AF_FLAG_FIRST_CONTINUE_AF_START,1)
#define afALG_DOING_CONTINUE_AF_OFF()              	afALG_StatusFlagSet(AF_FLAG_FIRST_CONTINUE_AF_START,0)
#define afALG_IS_DOING_CONTINUE_AF_ON()            	afALG_StatusFlagGet(AF_FLAG_FIRST_CONTINUE_AF_START)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CONTINUE_AF_PWM_TIMER_CONFIG()    afALG_StatusFlagSet(AF_FLAG_CONTINUE_AF_PWM_TIMER_CONFIG,1)
#define afALG_CLR_CONTINUE_AF_PWM_TIMER_CONFIG()    afALG_StatusFlagSet(AF_FLAG_CONTINUE_AF_PWM_TIMER_CONFIG,0)
#define afALG_IS_CONTINUE_AF_PWM_TIMER_CONFIG()     afALG_StatusFlagGet(AF_FLAG_CONTINUE_AF_PWM_TIMER_CONFIG)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_FOCUS_THERAD_SEARCH_EDGE()				afALG_StatusFlagSet(AF_FLAG_CONTINUE_AF_PWM_TIMER_CONFIG,1)
#define afALG_CLR_FOCUS_THERAD_SEARCH_EDGE()				afALG_StatusFlagSet(AF_FLAG_CONTINUE_AF_PWM_TIMER_CONFIG,0)
#define afALG_IS_FOCUS_THERAD_SEARCH_EDGE()   			afALG_StatusFlagGet(AF_FLAG_CONTINUE_AF_PWM_TIMER_CONFIG)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_ENABLE_TEST_LOST_STEP_AND_BACKLASH()		afALG_StatusFlagSet(AF_FLAG_TEST_LOST_STEP_AND_BACKLASH,1)
#define afALG_DISABLE_TEST_LOST_STEP_AND_BACKLASH()		afALG_StatusFlagSet(AF_FLAG_TEST_LOST_STEP_AND_BACKLASH,0)
#define afALG_IS_ENABLE_TEST_LOST_STEP_AND_BACKLASH() afALG_StatusFlagGet(AF_FLAG_TEST_LOST_STEP_AND_BACKLASH)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_ENABLE_LOW_LIGHT_CLOSE_FAF()					afALG_StatusFlagSet(AF_FLAG_ENABLE_LOW_LIGHT_CLOSE_FAF,1)
#define afALG_DISABLE_LOW_LIGHT_CLOSE_FAF()					afALG_StatusFlagSet(AF_FLAG_ENABLE_LOW_LIGHT_CLOSE_FAF,0)
#define afALG_IS_ENABLE_LOW_LIGHT_CLOSE_FAF()   		afALG_StatusFlagGet(AF_FLAG_ENABLE_LOW_LIGHT_CLOSE_FAF)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_BACKGROUND_3A_DISABLE_FAF()				afALG_StatusFlagSet(AF_FLAG_BACKGROUND_3A_DISABLE_FAF,1)
#define afALG_CLR_BACKGROUND_3A_DISABLE_FAF()				afALG_StatusFlagSet(AF_FLAG_BACKGROUND_3A_DISABLE_FAF,0)
#define afALG_IS_BACKGROUND_3A_DISABLE_FAF()   			afALG_StatusFlagGet(AF_FLAG_BACKGROUND_3A_DISABLE_FAF)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_RUNNING_AF_OPNE_PROCESS_FRAME()			afALG_StatusFlagSet(AF_FLAG_RUNNING_AF_OPEN_PROCESS_FRAME,1)
#define afALG_SET_STOP_AF_OPNE_PROCESS_FRAME()  			afALG_StatusFlagSet(AF_FLAG_RUNNING_AF_OPEN_PROCESS_FRAME,0)
#define afALG_IS_RUNNING_AF_OPNE_PROCESS_FRAME()   		afALG_StatusFlagGet(AF_FLAG_RUNNING_AF_OPEN_PROCESS_FRAME)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_SNAP1_AF_REFERENCE_CONTINUE_AF()		afALG_StatusFlagSet(AF_FLAG_SNAP1_AF_REFERENCE_CONTINUE_AF,1)
#define afALG_CLR_SNAP1_AF_REFERENCE_CONTINUE_AF()  	afALG_StatusFlagSet(AF_FLAG_SNAP1_AF_REFERENCE_CONTINUE_AF,0)
#define afALG_IS_SNAP1_AF_REFERENCE_CONTINUE_AF()   	afALG_StatusFlagGet(AF_FLAG_SNAP1_AF_REFERENCE_CONTINUE_AF)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_REFERENCE_SINGLE_WINDOWS_EDGE()			afALG_StatusFlagSet(AF_FLAG_REFERENCE_SINGLE_WINDOWS_EDGE,1)
#define afALG_CLR_REFERENCE_SINGLE_WINDOWS_EDGE()  		afALG_StatusFlagSet(AF_FLAG_REFERENCE_SINGLE_WINDOWS_EDGE,0)
#define afALG_IS_REFERENCE_SINGLE_WINDOWS_EDGE()   		afALG_StatusFlagGet(AF_FLAG_REFERENCE_SINGLE_WINDOWS_EDGE)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_SEARCH_DYNAM_STEPS()						afALG_StatusFlagSet(AF_FLAG_SEARCH_DYNAM_STEPS,1)
#define afALG_CLR_SEARCH_DYNAM_STEPS()  					afALG_StatusFlagSet(AF_FLAG_SEARCH_DYNAM_STEPS,0)
#define afALG_IS_SEARCH_DYNAM_STEPS()     				afALG_StatusFlagGet(AF_FLAG_SEARCH_DYNAM_STEPS)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_AUTO_FOCUS_EVEN_DONE()					afALG_StatusFlagSet(AF_FLAG_AUTO_FOCUS_EVEN_DONE,1)
#define afALG_CLR_AUTO_FOCUS_EVEN_DONE()  				afALG_StatusFlagSet(AF_FLAG_AUTO_FOCUS_EVEN_DONE,0)
#define afALG_IS_AUTO_FOCUS_EVEN_DONE()   				afALG_StatusFlagGet(AF_FLAG_AUTO_FOCUS_EVEN_DONE)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_FLAG_ROI_WINDOWS()							afALG_StatusFlagSet(AF_FLAG_ROI_WINDOWS,1)
#define afALG_CLR_FLAG_ROI_WINDOWS()  						afALG_StatusFlagSet(AF_FLAG_ROI_WINDOWS,0)
#define afALG_IS_SET_ROI_WINDOWS()   							afALG_StatusFlagGet(AF_FLAG_ROI_WINDOWS)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_FLAG_CONTIUNE_EDGE()						afALG_StatusFlagSet(AF_FLAG_CONTIUNE_EDGE,1)
#define afALG_CLR_FLAG_CONTIUNE_EDGE()  					afALG_StatusFlagSet(AF_FLAG_CONTIUNE_EDGE,0)
#define afALG_IS_SET_CONTIUNE_EDGE()    					afALG_StatusFlagGet(AF_FLAG_CONTIUNE_EDGE)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_CAF_TRACE_EDGE()	     					afALG_StatusFlagSet(AF_FLAG_SET_CONTIUNE_TRACE_EDGE,1)
#define afALG_CLR_CAF_TRACE_EDGE()         				afALG_StatusFlagSet(AF_FLAG_SET_CONTIUNE_TRACE_EDGE,0)
#define afALG_IS_CAF_TRACE_EDGE()          				afALG_StatusFlagGet(AF_FLAG_SET_CONTIUNE_TRACE_EDGE)
/*---------------------------------------------------------------------------------------------------------------*/
#define afALG_SET_ROI_AF_ABORT()	     						afALG_StatusFlagSet( AF_FLAG_SET_ROI_AF_ABORT,1)
#define afALG_CLR_ROI_AF_ABORT()       						afALG_StatusFlagSet(AF_FLAG_SET_ROI_AF_ABORT,0)
#define afALG_IS_ROI_AF_ABORT()        						afALG_StatusFlagGet(AF_FLAG_SET_ROI_AF_ABORT)
/*---------------------------------------------------------------------------------------------------------------*/
enum
{
	CUR_POS=0,
	CUR_DIST,
	PF_TYPE,
	CAF_SEARCHRANGE_START,
	BACKLASH,
	MF_DIST,
};

#define afALG_GET_CUR_POS()                         afALG_VarGet(CUR_POS)
#define afALG_INC_CUR_POS()                         afALG_VarSet(CUR_POS,afALG_VarGet(CUR_POS)+1)
#define afALG_DEC_CUR_POS()                         afALG_VarSet(CUR_POS,afALG_VarGet(CUR_POS)-1)
#define afALG_SET_CUR_POS_TO_HOME()                 afALG_VarSet(CUR_POS,FOCUS_GET_HOME_POS)
#define afALG_SET_CUR_POS_TO_CLOSE()                afALG_VarSet(CUR_POS,FOCUS_GET_CLOSE_POS)

#define afALG_SET_CUR_POS(iPos)                     afALG_VarSet(CUR_POS,iPos)
#define afALG_IS_CUR_POS_HOME()                     (afALG_VarGet(CUR_POS)== FOCUS_GET_HOME_POS)  ? 1 : 0)
#define afALG_IS_CUR_POS_CLOSE()                    (afALG_VarGet(CUR_POS)== FOCUS_GET_CLOSE_POS) ? 1 : 0)


#define afALG_GET_CUR_DIST()                        afALG_VarGet(CUR_DIST)
#define afALG_SET_CUR_DIST(uDist)                   afALG_VarSet(CUR_DIST,uDist)


#define afALG_GET_PF_TYPE()                         afALG_VarGet(PF_TYPE)
#define afALG_SET_PF_TYPE(uType)                    afALG_VarSet(PF_TYPE,uType)


#define afALG_GET_CAF_SEARCHRANGE_START()           afALG_VarGet(CAF_SEARCHRANGE_START)
#define afALG_SET_CAF_SEARCHRANGE_START(a)          afALG_VarSet(CAF_SEARCHRANGE_START,a)



#define afALG_GET_BACKLASH()                        afALG_VarGet(BACKLASH)
#define afALG_SET_BACKLASH(a)                     	afALG_VarSet(BACKLASH,a)
#define afALG_GET_MF_DIST()                         afALG_VarGet(MF_DIST)
#define afALG_SET_MF_DIST(uDist)                    afALG_VarSet(MF_DIST,uDist)


enum
{
   PF_TYPE_INDOOR,
   PF_TYPE_OUTDOOR,
   PF_TYPE_VIDEO,
   PF_TYPE_NIGHTPORTRAIT,

   PF_TYPE_TOTAL,
};

/*---------------------------------------------------------------------------------------------------------------*/
#define AF_WIN_MULTIPLE_BASE                      16
#define AF_WINDOWS_H                              1024
#define AF_WINDOWS_V                              1024
/*---------------------------------------------------------------------------------------------------------------*/
#ifdef 	AF_RUN_MULTI_WINDOWS_ENABLE
#define AF_MULTI_WINDOWS_SIZE_X                   (AF_MULTI_WINDOWS_SUB_WIN_SIZE_X * AF_MULTI_WINDOWS_SUB_WIN_HOR_CNT )  /* 80 * 3 = 240 ~= 1/4.2 Full Windows*/
#define AF_MULTI_WINDOWS_SIZE_Y                   (AF_MULTI_WINDOWS_SUB_WIN_SIZE_Y * AF_MULTI_WINDOWS_SUB_WIN_VER_CNT )  /* 80 * 2 = 240 ~= 1/4.2 Full Windows*/
#define AF_MULTI_WINDOWS_START_X                  ((AF_WINDOWS_H / 2)-(AF_MULTI_WINDOWS_SIZE_X / 2))
#define AF_MULTI_WINDOWS_START_Y                  ((AF_WINDOWS_V / 2)-(AF_MULTI_WINDOWS_SIZE_Y / 2))
#define AF_MULTI_SUBWINDOWS_TOTAL                 (AF_MULTI_WINDOWS_SUB_WIN_HOR_CNT * AF_MULTI_WINDOWS_SUB_WIN_VER_CNT)
#define AF_MULTI_WINDOWS_SUB_WIN_CENTRAL_IDX      (AF_MULTI_SUBWINDOWS_TOTAL / 2)
#define MAX_MULTI_SUBWINDOWS_TOTAL                (28)
#endif
/*---------------------------------------------------------------------------------------------------------------*/
#define AF_DEFOCUS_STEP_RANGE_SEGMENT                       FOCUS_TOTAL_DEF_POS
#define AF_DEFOCUS_WIN_IDX_LEFT_UP                          0xFFFB
#define AF_DEFOCUS_WIN_IDX_LEFT_DOWN                        0xFFFC
#define AF_DEFOCUS_WIN_IDX_RIGHT_UP                         0xFFFD
#define AF_DEFOCUS_WIN_IDX_RIGHT_DOWN                       0xFFFE
#define AF_DEFOCUS_WIN_IDX_CENTRAL                          0xFFFF
/*---------------------------------------------------------------------------------------------------------------*/
#define ERR_UNDER_ACKNOWLEDGEMENT_EDGE                        1
#define ERR_OVER_FOCUS_POSITION                               2
#define ERR_UNDER_FOCUS_POSITION                              3
#define ERR_UNDER_FOCUS_EDGE_SCALE                            4
#define ERR_UNDER_FOCUS_LUX_LOW_LIGTH                         5
#define ERR_UNDER_SCALE_RATE_ACKNOWLEDGEMENT                  6
#define ERR_UNDER_SCALE_RATE_AVG_EDGE                         7
#define ERR_FOCUS_EDGE_UNSTABLE                               8
#define ERR_FOCUS_SECOND_POS_NOT_NEAR_BEST_POS                9
#define ERR_FOCUS_MULTI_WINDOWS_EDGE_SMOOTH                  10
#define ERR_FOCUS_CW_STEP_DECTECT_HP_ERROR                   11
#define ERR_FOCUS_CCW_STEP_DECTECT_HP_ERROR                  12
#define ERR_FOCUS_CW_STEP_DECTECT_OVER_STEP                  13
#define ERR_FOCUS_CCW_STEP_DECTECT_OVER_STEP                 14
#define ERR_HOR_VER_MAX_EDAG_DIFFERENT_STEP                  15
#define ERR_FIRST_AND_SECOND_DIFFERENT_STEP                  16
#define ERR_AFM_UNDER_ACKNOWLEDGEMENT_EDGE                   17
#define ERR_AFM_OVER_FOCUS_POSITION                          18
#define ERR_AFM_UNDER_FOCUS_POSITION                         19
#define ERR_AFM_SUBWIN_BEST_EDGE_UNDER_ACKNOWLEDGEMENT_EDGE  20
#define ERR_AFM_UNDER_SCALE_RATE_ACKNOWLEDGEMENT             21
#define ERR_FOCUS_DECTECT_POWER_UNDER_VALUES                 22  
#define ERR_FOCUS_PRESS_SNAP2_DIRECTLY                       23	
#define ERR_UNKNOWN_CASE                                     24
#define ERR_SEARCH_RANGE_ERROR                               25
#define ERR_AFSEARCH_GET_EDGE_READY_ERROR                    26
#define ERR_UNKNOWN_CASE_1                                   101
#define ERR_UNKNOWN_CASE_2                                   102
#define ERR_UNKNOWN_CASE_3                                   103
#define ERR_UNKNOWN_CASE_4                                   104
#define ERR_UNKNOWN_CASE_5                                   105
#define ERR_UNKNOWN_CASE_6                                   106
#define ERR_UNKNOWN_CASE_7                                   107
/*---------------------------------------------------------------------------------------------------------------*/

extern  S__FOCUS  _gsFocus;


/*--------------------------------------------------------------------------------------------------------*/
/*Event for Focus Thread*/
#define FOCUS_EVENT_TERMINATE                            	0x00000001
#define FOCUS_EVENT_POWERON_TO_DEFAULT                   	0x00000002
#define FOCUS_EVENT_TO_DEFAULT                           	0x00000004
#define FOCUS_EVENT_TO_HOME                              	0x00000008
#define FOCUS_EVENT_TO_CLOSE                             	0x00000010
#define FOCUS_EVENT_CCW                                  	0x00000020
#define FOCUS_EVENT_CW                                   	0x00000040
#define FOCUS_EVENT_PWM_MOVING_DIR_POWERON_TO_DEFAULT    	0x00000080 
#define FOCUS_EVENT_PWM_MOVING_DIR_CW                    	0x00000100 
#define FOCUS_EVENT_PWM_MOVING_DIR_CCW                   	0x00000200 
#define FOCUS_EVENT_PWM_MOVING_DIR_TO_HOME               	0x00000400 
#define FOCUS_EVENT_PWM_MOVING_DIR_REALDY_AT_HOME        	0x00000800 
#define FOCUS_EVENT_PWM_MOVING_DIR_TODO_GOTO             	0x00001000 
#define FOCUS_EVENT_PWM_MOVING_DIR_DONE_GOTO             	0x00002000 
#define FOCUS_EVENT_PWM_MOVING_DIR_DO_BACK_LASH          	0x00004000 
#define FOCUS_EVENT_PWM_MOVING_DIR_CHK_DONE_GOTO         	0x00008000 
#define FOCUS_EVENT_PWM_MOVING_DIR_GOTO_HOME_CW_JAMMED   	0x00010000 
#define FOCUS_EVENT_PWM_MOVING_DIR_GOTO_HOME_CCW_JAMMED  	0x00020000 
#define FOCUS_EVENT_PWM_BE_POWER_OFF                     	0x00040000 
#define FOCUS_EVENT_PWM_MOVING_DIR_GOTO_JAMMED           	0x00080000 
#define FOCUS_EVENT_PWM_MOVING_FINISH 	 									0x00100000
#define FOCUS_EVENT_PWM_MOVING_DIR_JAMMED_CCW_DONE_GOTO  	0x00200000 
#define FOCUS_EVENT_PWM_MOVING_DIR_JAMMED_CW_DONE_GOTO   	0x00400000 


#define FOCUS_THREAD_SUPPORT_EVENT       (	 		 FOCUS_EVENT_TERMINATE          |\
                                                 FOCUS_EVENT_POWERON_TO_DEFAULT |\
                                                 FOCUS_EVENT_TO_DEFAULT         |\
                                                 FOCUS_EVENT_TO_HOME            |\
                                                 FOCUS_EVENT_TO_CLOSE           |\
                                                 FOCUS_EVENT_CCW                |\
                                                 FOCUS_EVENT_CW                 |\
                                                 FOCUS_EVENT_PWM_BE_POWER_OFF   |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_TO_HOME |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_REALDY_AT_HOME |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_TODO_GOTO |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_DONE_GOTO |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_CHK_DONE_GOTO |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_DO_BACK_LASH |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_POWERON_TO_DEFAULT |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_CW |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_CCW |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_GOTO_HOME_CW_JAMMED |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_GOTO_HOME_CCW_JAMMED |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_JAMMED_CW_DONE_GOTO |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_JAMMED_CCW_DONE_GOTO |\
                                                 FOCUS_EVENT_PWM_MOVING_DIR_GOTO_JAMMED \
                                             )
/*--------------------------------------------------------------------------------------------------------*/
/* Event for AutoFocus Thread*/
#define AFOCUS_THREAD_EVENT_TERMINATE                     0x00000001
#define AFOCUS_THREAD_EVENT_AUTO_FOCUS                    0x00000002
#define AFOCUS_THREAD_EVENT_MAMUL_FOCUS                   0x00000004
#define AFOCUS_THREAD_EVENT_TO_HOME                       0x00000008
#define AFOCUS_THREAD_EVENT_CLOSE                         0x00000010
#define AFOCUS_THREAD_EVENT_CONTINUE_AF                   0x00000020
#define AFOCUS_THREAD_EVENT_CONTINUE_EDGE                 0x00000040
#define AFOCUS_THREAD_EVENT_CAF_MOVING_CW                  0x00000080
#define AFOCUS_THREAD_EVENT_CAF_MOVING_CCW                 0x00000100
   
#define AFOCUS_THREAD_EVENT              (	AFOCUS_THREAD_EVENT_TERMINATE    |\
                                            	AFOCUS_THREAD_EVENT_AUTO_FOCUS   |\
                                            	AFOCUS_THREAD_EVENT_MAMUL_FOCUS  |\
                                            	AFOCUS_THREAD_EVENT_CONTINUE_AF  |\
                                            	AFOCUS_THREAD_EVENT_CONTINUE_EDGE|\
                                            	AFOCUS_THREAD_EVENT_TO_HOME      |\
                                            	AFOCUS_THREAD_EVENT_CAF_MOVING_CW |\
                                            	AFOCUS_THREAD_EVENT_CAF_MOVING_CCW|\
                                            	AFOCUS_THREAD_EVENT_CLOSE )
/*--------------------------------------------------------------------------------------------------------*/
void 	afALG_Focus_Thread( ULONG ulData );
SINT16 	afALG_HomePosGet(void);
SINT16 	afALG_DefPosGet(void);
SINT16 	afALG_CurPosGet(void);
SINT16 	afALG_ClosePosGet(void);
SINT16 	afALG_MaxPosGet(void);
SINT16  afALG_DIST_0100MM_STEPGet(void);


/* Event for Focus Delay*/
#define FOCUS_DELAY_EVENT_TIME_INIT     0x00000001
#define FOCUS_DELAY_EVENT_TIME_OUT      0x00000002

/*---------------------------------------------------------------------------------------------------------------*/
/* The Focus moving direct member*/
enum
{
   FOCUS_MOVING_DIR_NONE,
   FOCUS_MOVING_DIR_TO_HOME,
   FOCUS_MOVING_DIR_TO_CLOSE,
   FOCUS_MOVING_DIR_POWERON_TO_HOME,
   FOCUS_MOVING_DIR_POWERON_TO_DEFAULT,
   FOCUS_MOVING_DIR_TO_DEFAULT,
   FOCUS_MOVING_DIR_CCW,
   FOCUS_MOVING_DIR_CW,
   FOCUS_MOVING_DIR_ABSOLUTE_POS,
   FOCUS_MOVING_DIR_CONTINUE_OFF,
   FOCUS_MOVING_DIR_CONTINUE_START,
   FOCUS_MOVING_DIR_CONTINUE_RUNNING,
   FOCUS_MOVING_DIR_CONTINUE_STOP,
   FOCUS_DIRECT_PWM_MOVING_DIR_CW,
   FOCUS_DIRECT_PWM_MOVING_DIR_CCW,
   FOCUS_DIRECT_PWM_MOVING_ABSOLUTE_POS,
   FOCUS_DIRECT_PWM_MOVING_DIR_TO_DEFAULT,
   FOCUS_DIRECT_PWM_MOVING_DIR_POWERON_TO_DEFAULT,
   FOCUS_MOVING_DIR_PAUSE,
   FOCUS_MOVING_DIR_CW_ML,
   FOCUS_MOVING_DIR_TO_CLOSE_FROM_ML,
};
/*---------------------------------------------------------------------------------------------------------------*/
/* The Focus dist member*/
#if	(defined LENS_VCM7002 )  
enum
{
	IDX_DIST_INF,
	IDX_DIST_32000MM,
	IDX_DIST_16000MM,
	IDX_DIST_8000MM,
	IDX_DIST_4000MM,
	IDX_DIST_2000MM,
	IDX_DIST_1500MM,
	IDX_DIST_1200MM,
	IDX_DIST_1000MM,
	IDX_DIST_0800MM,
	IDX_DIST_0700MM,
	IDX_DIST_0600MM,
	IDX_DIST_0500MM,
	IDX_DIST_0100MM,	
	IDX_DIST_NORMAL_STOP,
	IDX_DIST_MACRO_START,
	IDX_DIST_MACRO,
	IDX_DIST_LIMITED,
   	IDX_DIST_TOTAL,
};
#elif (defined LENS_LARGAN9377A8D )  
enum
{
	IDX_DIST_INF,
	IDX_DIST_32000MM,
	IDX_DIST_16000MM,
	IDX_DIST_8000MM,
	IDX_DIST_4000MM,
	IDX_DIST_2000MM,
	IDX_DIST_1500MM,
	IDX_DIST_1200MM,
	IDX_DIST_1000MM,
	IDX_DIST_0800MM,
	IDX_DIST_0700MM,
	IDX_DIST_0600MM,
	IDX_DIST_0500MM,
	IDX_DIST_0100MM,	
	IDX_DIST_NORMAL_STOP,
	IDX_DIST_MACRO_START,
	IDX_DIST_MACRO,
	IDX_DIST_LIMITED,
   	IDX_DIST_TOTAL,
};
#elif (defined LENS_LARGAN9547A1 )  
enum
{
	IDX_DIST_INF,
	IDX_DIST_32000MM,
	IDX_DIST_16000MM,
	IDX_DIST_8000MM,
	IDX_DIST_4000MM,
	IDX_DIST_2000MM,
	IDX_DIST_1500MM,
	IDX_DIST_1200MM,
	IDX_DIST_1000MM,
	IDX_DIST_0800MM,
	IDX_DIST_0700MM,
	IDX_DIST_0600MM,
	IDX_DIST_0500MM,
	IDX_DIST_0100MM,	
	IDX_DIST_NORMAL_STOP,
	IDX_DIST_MACRO_START,
	IDX_DIST_MACRO,
	IDX_DIST_LIMITED,
   	IDX_DIST_TOTAL,
};
#elif (defined LENS_LARGAN40025A1 )  
enum
{
	IDX_DIST_INF,
	IDX_DIST_32000MM,
	IDX_DIST_16000MM,
	IDX_DIST_8000MM,
	IDX_DIST_4000MM,
	IDX_DIST_2000MM,
	IDX_DIST_1500MM,
	IDX_DIST_1200MM,
	IDX_DIST_1000MM,
	IDX_DIST_0800MM,
	IDX_DIST_0700MM,
	IDX_DIST_0600MM,
	IDX_DIST_0500MM,
	IDX_DIST_0100MM,	
	IDX_DIST_NORMAL_STOP,
	IDX_DIST_MACRO_START,
	IDX_DIST_MACRO,
	IDX_DIST_LIMITED,
   	IDX_DIST_TOTAL,
};
#elif (defined IU091F4_Z )  
enum
{
	IDX_DIST_INF,
	IDX_DIST_32000MM,
	IDX_DIST_16000MM,
	IDX_DIST_8000MM,
	IDX_DIST_4000MM,
	IDX_DIST_2000MM,
	IDX_DIST_1500MM,
	IDX_DIST_1200MM,
	IDX_DIST_1000MM,
	IDX_DIST_0800MM,
	IDX_DIST_0700MM,
	IDX_DIST_0600MM,
	IDX_DIST_0500MM,
	IDX_DIST_0100MM,	
	IDX_DIST_NORMAL_STOP,
	IDX_DIST_MACRO_START,
	IDX_DIST_MACRO,
	IDX_DIST_LIMITED,
   	IDX_DIST_TOTAL,
};

#elif (defined LENS_VCM3H2 )  
enum
{
	IDX_DIST_INF,
	IDX_DIST_32000MM,
	IDX_DIST_16000MM,
	IDX_DIST_8000MM,
	IDX_DIST_4000MM,
	IDX_DIST_2000MM,
	IDX_DIST_1500MM,
	IDX_DIST_1200MM,
	IDX_DIST_1000MM,
	IDX_DIST_0800MM,
	IDX_DIST_0700MM,
	IDX_DIST_0600MM,
	IDX_DIST_0500MM,
	IDX_DIST_0100MM,	
	IDX_DIST_NORMAL_STOP,
	IDX_DIST_MACRO_START,
	IDX_DIST_MACRO,
	IDX_DIST_LIMITED,
   	IDX_DIST_TOTAL,
};
#else
enum
{
	IDX_DIST_INF,
	IDX_DIST_32000MM,
	IDX_DIST_16000MM,
	IDX_DIST_8000MM,
	IDX_DIST_4000MM,
	IDX_DIST_2000MM,
	IDX_DIST_1500MM,
	IDX_DIST_1200MM,
	IDX_DIST_1000MM,
	IDX_DIST_0800MM,
	IDX_DIST_0700MM,
	IDX_DIST_0600MM,
	IDX_DIST_0500MM,
	IDX_DIST_0100MM,	
	IDX_DIST_NORMAL_STOP,
	IDX_DIST_MACRO_START,
	IDX_DIST_MACRO,
	IDX_DIST_LIMITED,
   	IDX_DIST_TOTAL,
};
#endif

/*----------------------------------------------------------------------------------------------------------------*/
enum
{
   QVAF_BEST_FOCUS_POS_IN_SINGLE_WINDOWS,
   QVAF_BEST_FOCUS_POS_IN_MULTI_WINDOWS,
   QVAF_BEST_FOCUS_POS_IN_TOTAL_KID,
};
enum
{
	FOCUS_ISR_STATUS_STOP,
	FOCUS_ISR_STATUS_WAIT_TO_MOVE,
	FOCUS_ISR_STATUS_MOVING,
	FOCUS_ISR_STATUS_MOVING_FINISH,
};
enum
{
	FOCUS_NOT_JAMMED,
	FOCUS_JAMMED,
};

#ifdef AF_ENABLE_MORE_AF_DBG_INFO
enum
{
	AF_MISC_INFO_FLAT_PLANE,
	AF_MISC_INFO_FIRST_CENTER_AVERAGE,
	AF_MISC_INFO_LAST_CENTER_AVERAGE,
	AF_MISC_INFO_IS_USE_SUME_AVERAGE,
   	AF_MISC_INFO_TOTAL,
};

#endif
typedef enum tag_AF_TRACKING_TYPE
{
	AF_FACE_TRACKING,
	AF_EASY_FACE_TRACKING,
	AF_OBJECT_TRACKING,
	AF_TRACKING_TYPE_TOTAL,
}tag_AF_TRACKING_TYPE;
typedef enum tag_AF_TRACKING_CANTIUNE_AF_STATUS
{
	AF_TRACKING_CLOSE_CANTIUNE_AF,
	AF_TRACKING_OPEN_CANTIUNE_AF,
	AF_TRACKING_CANTIUNE_AF_STATUS,
}tag_AF_TRACKING_CANTIUNE_AF_STATUS;

                                /*  87654321*/
#define SEARCH_ABORT1   1       /*0x00000001*/
#define SEARCH_ABORT2   2       /*0x00000002*/
#define SEARCH_ABORT3   3       /*0x00000004*/
#define SEARCH_ABORT4   4       /*0x00000008*/
#define SEARCH_ABORT5   5       /*0x00000010*/
#define SEARCH_ABORT6   6       /*0x00000020*/
#define SEARCH_ABORT7   7       /*0x00000040*/
#define SEARCH_ABORT8   8       /*0x00000080*/
#define SEARCH_ABORT9   9       /*0x00000100*/
#define SEARCH_ABORT10  10      /*0x00000200*/
#define SEARCH_ABORT11  11      /*0x00000400*/
#define SEARCH_ABORT12  12      /*0x00000800*/
#define SEARCH_ABORT13  13      /*0x00001000*/
#define SEARCH_ABORT14  14      /*0x00002000*/
#define SEARCH_ABORT15  15      /*0x00004000*/
#define SEARCH_ABORT16  16      /*0x00008000*/
#define SEARCH_ABORT17  17      /*0x00010000*/
#define SEARCH_ABORT18  18      /*0x00020000*/
#define SEARCH_ABORT19  19      /*0x00040000*/
#define SEARCH_ABORT20  20      /*0x00080000*/
#define SEARCH_ABORT21  21      /*0x00100000*/
#define SEARCH_ABORT22  22      /*0x00200000*/
#define SEARCH_ABORT23  23      /*0x00400000*/
#define SEARCH_ABORT24  24      /*0x00800000*/
#define SEARCH_ABORT25  25      /*0x01000000*/
#define SEARCH_ABORT26  26      /*0x02000000*/
#define SEARCH_ABORT27  27      /*0x04000000*/
#define SEARCH_ABORT28  28      /*0x08000000*/
#define SEARCH_ABORT29  29      /*0x10000000*/
#define SEARCH_ABORT30  30      /*0x20000000*/
#define SEARCH_ABORT31  31      /*0x40000000*/
#define SEARCH_ABORT32  32      /*0x80000000*/
#define SEARCH_ABORT33  33      /*0x00000001*/
#define SEARCH_ABORT34  34      /*0x00000002*/
#define SEARCH_ABORT35  35      /*0x00000004*/
#define SEARCH_ABORT36  36      /*0x00000008*/
/*----------------------------------------------------------------------------------------------------------------*/
#define MAX_FOCUS_EDGES                         74 /*100*/  /*Modify 800 Chage to 200*/
#define FOCUS_SEARCH_FLAG_ABORT                 0x00000001
#define FOCUS_SEARCH_FLAG_ADVANCE_BREAK         0x00000002  
#define FOCUS_SEARCH_FLAG_HAND_SHAKE            0x00000004  
#define FOCUS_SEARCH_FLAG_KEY_RELEASE_BREAK     0x00000008  
#define FOCUS_SEARCH_FLAG_ABORT_FLAG2           0x00000010 
/*-----------------------------------------------------------------------------------------------------------------------------*/
#define  HAND_SHAKE_COUNT_MAX           6
#pragma pack(1)
typedef struct
{

   SINT16  iFocusPos[MAX_FOCUS_EDGES]; 
   UINT32  uOriMultiWinFocusEdge[AF_MULTI_REGION_TOTAL+1][MAX_FOCUS_EDGES];
   UINT32  uMultiWinFocusEdge[AF_MULTI_REGION_TOTAL+1][MAX_FOCUS_EDGES];
   UINT32  uVerticalFocusEdge[2][MAX_FOCUS_EDGES];
   UINT32  uSingleFocusEdge[2][MAX_FOCUS_EDGES];
   SINT16  iMultiWinBestPos[AF_MULTI_REGION_TOTAL+1 ];
   UINT32  iMultiWinBestEdge[AF_MULTI_REGION_TOTAL+1];
   UINT32  iMultiWinMinEdge[AF_MULTI_REGION_TOTAL+1];  
   UINT32  iVerticalBestEdge;
   UINT32  iVerticalMinEdge;  
   UINT32  iSingleBestEdge;
   UINT32  iSingleMinEdge;  
   UINT32  uFlag;
   UINT16  uZoomPosIdx;
   SINT16  iBestPos;
   UINT32  uBestEdge;

   SINT16   iFVidx, iFPidx;
   SINT16   iStartPos, iStopPos, iSteps;
   
   SINT16  iMultiWinFVidx; 
   UINT32  uMultiWindowsCnt;
   UINT32  uMultiWindowsBestPosIdx ;
   SINT16   iMultiWindowsBestPos ;
   
   BOOL	   bMultiAFResult[AF_MULTI_REGION_TOTAL];	/* 1=SUCCESS, 0=FAIL*/
   BOOL	   bAFStableResult[AF_MULTI_REGION_TOTAL];	/* 1=Stable, 0=Unstable*/
   BOOL	   bAFWeightUp[AF_MULTI_REGION_TOTAL];		
   BOOL	   bAFUseVertical[AF_MULTI_REGION_TOTAL];
   UINT32  uMultiWinAverageFocusEdge[MAX_FOCUS_EDGES];
   SINT16  iMultiWinAverageBestPos;
   UINT32  iMultiWinAverageBestEdge;
   UINT16	uwAFThreshold[2];
   UINT16	uwWinH;
   UINT16	uwWinV;
   UINT16	uwWinX;
   UINT16	uwWinY;
   UINT16	uwWinIndex;
	
   SINT16  cSign, cPreviousSign, cSignCount  , cFocusCount ,cMaxCntDown;
   UINT16  cCount[AF_MULTI_REGION_TOTAL+1];
   UINT32  cCountBestEdge[AF_MULTI_REGION_TOTAL+1];
   UINT32  uMultiWinCurveShape[AF_MULTI_REGION_TOTAL+1];

   UINT16  	cAF_Result;
   UINT16  	uMultiAF_Peak;
   UINT16  	uVerticalAF_Peak;

   UINT32 	uBolaCnt;
   UINT32 	uBolaMaxIdx;
   SINT16  	iDelayVD ;

   UINT16 	FNumber;
   SINT16   iMinPos;
   UINT32  	uMinEdge;
   UINT32  	uAvgEdge;
   UINT32 	uTemperature;
   UINT8 	uEdgeRate;
   UINT32 	uTiming[6];
} S_AFOCUS_SEARCH;
#pragma pack()

extern S_AFOCUS_SEARCH  _gsAFocusSearch;

#pragma pack(1)
typedef struct
{
	SINT32 iPrvwISOEntry; 	/*iPrvwISOEntry*/
	UINT32 uStatMode; 		/*SP5K_AF_STAT_MODE*/
	UINT32 uGammaType; 		/*SP5K_AF_CFG_GAMMA_TYPE*/
	UINT32 uFilterWeight; 	/*SP5K_AF_CFG_FILTER_WEIGHT*/
	UINT32 uGain; 			/*SP5K_AF_GAIN*/
	UINT32 uNoiseThr; 		/*SP5K_AF_NOISE_THR*/
	UINT32 uCoringThr;  	/*SP5K_AF_CORING_THR*/
	UINT32 uCoringVal; 		/*SP5K_AF_CORING_VALUE*/
	UINT32 uWinPosTest; 	/*SP5K_AF_WIN_POS_TEST*/
}S_AFOCUS_STATISTICS;
#pragma pack()

extern S_AFOCUS_STATISTICS  _gsAFocusStatistic;

#pragma pack(1)
typedef struct
{
	
   SINT8	uGSensor_X;
   SINT8	uGSensor_Y;	
   SINT8	uGSensor_Z;

   SINT8	uGyro_X;
   SINT8	uGyro_Y;	
   SINT8	uGyro_Z;
}S_AFOCUS_GSensor;
#pragma pack()

/* Exif MakerNote field*/
#pragma pack(1)
typedef struct
{
  SINT16   iFocuscurPos;
  SINT16   iRealStep;
  SINT16   iDefPos;
  SINT16   iCaliDefPosOft;
  SINT16   iBestPos , iStartPos , iStopPos ;
  UINT16  uMode;
  UINT16  cAF_Result;
  UINT8   uAfIsAbort;
  UINT32  uBestEdge;
  UINT8   uGain;  
  UINT8   uNoiseThr;  
  UINT8   uwTotalRegion;
  UINT32 uTiming[6];
  
  #ifndef ENABLE_SIMPLIFICATION_MAKERNOTE  

  SINT16   iZoomCurPos;  
  UINT8   uIsAllowFAF;  
  UINT16  uVCM_CurDegree;  
  UINT8   uUseIIR_FilterEdge;  
  UINT8   uStatMode;  
  UINT8   uGammaType;  
  UINT32  uFilterWeight;  
  UINT8   uCoringThr;   
  UINT8   uCoringVal;  
  UINT8   uWinPosTest; 
  UINT32  uCurDistanceFromStep;
  UINT16  uMultiWindowsCnt;
  SINT32   iPrvwISOEntry;  
  SINT16   iFocusPos[MAX_FOCUS_EDGES];  
  UINT32  uFocusEdge[AF_MULTI_REGION_TOTAL+1][MAX_FOCUS_EDGES];  
  #else
  SINT16   iFocusPos[MAX_FOCUS_EDGES/2];  
  UINT32  uFocusEdge[1][MAX_FOCUS_EDGES/2];
  #endif
  
} S_MAKER_AF;
extern S_MAKER_AF  _gsMakerAf;
#pragma pack()

#pragma pack(1)
typedef struct
{
   UINT32  uFlag;
   ULONG   ulFileID;
   UINT16  wBackUpFocusMode, wFocusCaliMode, wIdxDist;
   UINT16  wRepeatCnt;
   SINT16   iBestPos[FOCUS_CALI_REPEAT_CNT], iTotalBestPos;
   SINT16   iStopPos;
   SINT16   iCaiFourCentsPos[4];
   #ifdef _FOCUS_CALIB_CONSIDE_IRIS_OFFSET_
  
   SINT32   iFocusIrisOffset[FOCUS_CALI_REPEAT_CNT][FOCUS_TOTAL_DEF_POS][IRIS_TOTAL_POS];
   SINT32   iFocusIrisOffsetTotalBestPos[IRIS_TOTAL_POS];
   #endif
   
   #if 1 /*#ifdef ENABLE_FOCUS_CALIB_BLOCK_KEY*/
   UINT8 uFocusCalibZoomCnt; /*Zoom moving counter for Focus calibration success*/
   #endif
} S_FOCUS_CALI;
#pragma pack()

#pragma pack(1)
typedef struct
{
	UINT8	PeakDiff;
	UINT8	M_EdgeRatio;
	UINT8	V_EdgeRatio;
	UINT8	S_EdgeRatio;
	UINT8	M_EdgeRatio_TooClose;
	UINT8	V_EdgeRatio_TooClose;
	UINT8	M_EdgeRatio_Flat;
	UINT8	V_EdgeRatio_Flat;
	UINT8	L_Light_M_EdgeRatio;
	UINT8	H_Light_M_EdgeRatio;
	UINT8	L_Light_V_EdgeRatio;
	UINT8	H_Light_V_EdgeRatio;
	UINT8	M_EdgeRatio_Fail;
	UINT8	V_EdgeRatio_Fail;
	UINT8	S_EdgeRatio_Fail;
	UINT16	H_Light_Luma_Var;
	UINT16	L_Light_Luma_Var;
	UINT8	SceneChange_Ratio;
	SINT8	Gyro_X_Th;
	SINT8	Gyro_Y_Th;
	SINT8	Gyro_Z_Th;
	UINT8	GyroFilter_En;
	UINT8	EdgeStable_Ratio;
	UINT8	LumaStable_Ratio;
	UINT8	GyroStable_Th;
	UINT8	SceneStable_VDCnt;
	UINT8	GyroStable_VDCnt;
	UINT8	SearchRange_Offset;
	UINT8	SearchJump_Offset;
} S_AF_Para;
#pragma pack()

#if 1 /* Moved From FocusCali.c*/
extern S_FOCUS_CALI   _gsFocusCali;
extern SINT16  _giFocusCaliDefPos[IDX_DIST_TOTAL][FOCUS_TOTAL_DEF_POS + 1]; /* last one is checksum*/
extern SINT16  _giFocusCaliDeFocusOffset[IDX_DIST_TOTAL][FOCUS_TOTAL_DEF_POS + 1]; /* last one is checksum*/
#endif

void	afALG_Lens_PowerOnInit(void);
void	afALG_BeginSearch(void);
void	afALG_EndSearch(void);
UINT32	afALG_SearchRangeGet(SINT16 *iStartPos, SINT16 *iStopPos);
UINT32	afALG_SearchRangeGet_ByMode (UINT16 uMode, SINT16 * iStartPos, SINT16 * iStopPos);

SINT16	afALG_FocusDo(UINT32 *uEdgeValue);
SINT16	afALG_InfoGet(UINT32 *uEdgeValue);
UINT8 	afALG_IsSearchAbort(void);
void   	afALG_SearchAbort(UINT32 uAbort);

UINT32 	afALG_IsSearch(void);         
UINT32 	afALG_ModeSet(UINT16 wFocusMode);
UINT16 	afALG_ModeGet(void);


SINT16	afALG_Calibration_DefPosOffsetGet(UINT16 wIdxDist, UINT16 wZoomPos);
SINT16 	afALG_Calibration_ZoomOffsetGet(UINT32 uIdx);
SINT16	afALG_SpecDefPosGet(UINT16 wIdxDist, UINT16 wZoomPos);

SINT32	afALG_IrisCompensationStepOffsetGet( UINT32 uZoomIdx , UINT8 uIris);

SINT16  afALG_AdjMinSearchRangeGet(void);

SINT16  afALG_AdjMaxSearchRangeGet(void);
void	afALG_Progressive_MovingDir(SINT16 target_pos,SINT16 progressive_step);

UINT32	afALG_IsHandShake(void);
void	afALG_SearchReset(void);
SINT16  afALG_Calibration_DeFocusOffsetGet(UINT16 wIdxDist, UINT16 wZoomPos);
SINT32  afALG_Calibration_DeFocusOffsetSave(UINT16 wIdxDist, UINT16 wZoomPos , SINT16 Data );



void	afALG_ContinueAF_Init (void);
void	afALG_ContinueAF_Reset(void );
void	afALG_ContinueAF_Control( void );
void	afALG_ContinueAF_Do(void );
void	afALG_ContinueAF_EventSent(void);

void	afALG_ACKStepsGet(SINT16 *iGetAckStep );


extern SINT16  _AF_DEFOCUS_OFFSET[AF_DEFOCUS_STEP_RANGE_SEGMENT][2];
extern SINT16 _giDefFocusPos[IDX_DIST_TOTAL][FOCUS_TOTAL_DEF_POS];

void	afALG_Calibration_PowerOnInit(void);

UINT32	afALG_IdxDistanceSet(UINT16 wIdxDist) ;
UINT16	afALG_IdxDistanceGet(void);
UINT32	afALG_Calibration_DefPosOffsetLoad(UINT16 wIdxDist);


SINT32	afALG_ParaBolaMaxXFind(UINT32 ValueNum, SINT16 *pX,UINT32 *pY , SINT16 *maxStep , UINT32 *maxEdge);

SINT32  afALG_Calibration_RefTemperatureGet(UINT16 wIdxDist, UINT16 wZoomPos);
SINT16 	afALG_TemperatureOffsetGet(UINT16 wIdxDist, UINT16 wZoomPos);

SINT32 	afALG_TemperatureTransfer(SINT32 X);

void 	afALG_SetAFWindowThresholds(void);


/* to check 7002 SINT32 AF_SetObjectTrackingWindow(OT_INFO *psObjectBuffers);*/
#ifdef DEF_OBJECT_TRACKING_ENABLE
SINT32 	afALG_ObjectTrackingWinGet(UINT16 *uWinX, UINT16 *uWinY, UINT16 *uWinV, UINT16 *uWinH);
SINT32 	afALG_ObjectTrackingFrameGet(UINT16 *uFrameW, UINT16 *uFrameH);
#endif



UINT32 	afALG_RoiWindowSet( UINT8 bEnable , UINT16 uWinX,UINT16 uWinY,UINT16 uWinV , UINT16 uWinH , UINT16 baseW , UINT16 baseH );
UINT32	afALG_RoiWindowGet( UINT16 *uWinX, UINT16 *uWinY, UINT16 *uWinV, UINT16 *uWinH );
UINT32 	afALG_RoiWindowsFrameGet(UINT16 *uFrameW, UINT16 *uFrameH);
void	afALG_PowerOnInit(void);	
void	afALG_StatusFlagSet(UINT32 selector, UINT32 value);
UINT32 	afALG_StatusFlagGet(UINT32 selector);
UINT16 	afALG_AFResultGet(void);


UINT32 	afALG_DistanceFromStepGet(SINT16 iFocusStep);
SINT16 	afALG_StepFromDistanceGet(UINT32 uDistance);


SINT32 	afALG_Calibration_DefPosOffsetSave(UINT16 wIdxDist, UINT16 wZoomPos, SINT16 Data);
SINT32 	afALG_Calibration_RefTemperatureSave(UINT16 wIdxDist, UINT16 wZoomPos, SINT32 Data);
void	afALG_SceneDetection(void);
#if 1
void	afALG_ProcessFrameGet(UINT32 fid);
#else
BOOL	afALG_ProcessFrameGet(PSSTATINFO psInfo, PSAFGEOMETRY psGeometry, ULONG ulCurrFrameCount);
#endif
UINT16	afALG_OTP_ModuleModelGet(void);
UINT16	afALG_OTP_F_NumberGet(void);
UINT16	afALG_OTP_InfAFGet(void);
UINT16	afALG_OTP_1mAFGet(void);
UINT16	afALG_OTP_100mmAFGet(void);
UINT16	afALG_OTP_StartCurrentGet(void);
UINT16	afALG_OTP_OperSensitivityGet(void);
UINT16	afALG_OTP_CheckSumGet(void);
UINT16	afALG_EE_AFCali_Get(UINT8 select);
UINT32 	afALG_FocusEdgeGet(SINT16 iFocusPos);
UINT32 	afALG_FullSearch(SINT16 iStartPos, SINT16 iStopPos, SINT16 iSteps);
UINT32 	afALG_FastSearch(SINT16 iStartPos, SINT16 iStopPos, SINT16 iSteps);
UINT32 	afALG_SmallRangeSearch(SINT16 iStartPos, SINT16 iStopPos, SINT16 iSteps);
UINT32 	afALG_ContinueAF_Search(SINT16 iStartPos, SINT16 iStopPos, SINT16 iSteps);
UINT32 	afALG_FastScan_NonBacklash(SINT16 iStartPos, SINT16 iStopPos, SINT16 iSteps);
UINT32 	afALG_FastScan_Backlash(SINT16 iStartPos, SINT16 iStopPos, SINT16 iSteps);
UINT32 	afALG_FastScan_Backlash_No_Sorting(SINT16 iStartPos, SINT16 iStopPos, SINT16 iSteps);
UINT32 	afALG_FastScan_NonBacklash_No_Sorting(SINT16 iStartPos, SINT16 iStopPos, SINT16 iSteps);
UINT8  	afALG_ContentTrendCheck( UINT8 ucCurStepPoint);

UINT32 	afALG_EdgeCurveOptimize(S_AFOCUS_SEARCH  *pAFSearch);
SINT16 	afALG_BestAFPosDecide(S_AFOCUS_SEARCH  *pAFSearch);
SINT16 	afALG_GetMajorPeak(S_AFOCUS_SEARCH  *pAFSearch);
BOOL 	afALG_IsSinglelsFlat(S_AFOCUS_SEARCH  *pAFSearch);
BOOL 	afALG_IsVerticalsFlat(S_AFOCUS_SEARCH  *pAFSearch);
BOOL 	afALG_FocusCheck(S_AFOCUS_SEARCH  *pAFSearch);
UINT32 	afALG_StepCheck(S_AFOCUS_SEARCH  *pAFSearch,SINT16 iBestPos,UINT8 SinglelsFlat,SINT16 iVerticalBestPos,SINT16 iSingleBestPos);
void 	afALG_DividedZeroAvoid(S_AFOCUS_SEARCH  *pAFSearch,UINT32 *uSingleBestEdge,UINT32 *uVerticalBestEdge);

SINT32 	afALG_RegionBestEdgePosGet(UINT8 bRegion);
void 	afALG_GeometrySet_Cfg_A(sp5kAfWinSize_t *afwin);
void 	afALG_GeometrySet_Cfg_B(sp5kAfWinSize_t *afwin);
void 	afALG_GeometrySet(void);
UINT32 	afALG_StatisticsSet(void);


UINT32 	afALG_Snap1FNumSet(UINT16 Nominator, UINT16 Denominator);
UINT32 	afALG_Snap1FNumGet( UINT16 *Nominator, UINT16 *Denominator );

SINT32 	afALG_GetRealStep(SINT32 iFocusStep);

#ifdef ENABLE_FOCUS_CALIBRATION_SEARCH_RESET
SINT32 	afALG_SearchStepGet(UINT32 uIdx);
#endif

UINT32 	afALG_IsKeyReleaseBreak(void);
BOOL 	afALG_FAFScaleWindowGet( UINT16 uFrameW  /* Down sample FAF Width  or  VDO Width for Cut*/
									, UINT16 uFrameH /* Down sample FAF Height  or  VDO Height for Cut*/
									, UINT16  * uStartX
    								, UINT16  * uStartY
    								, UINT16  * uWidth
    								, UINT16  * uHeight);

void 	afALG_ContinueAF_ProcessWait (UINT32 uDisableAF);
UINT32 	afALG_SmallGainACKEdgeGet(void);

void  	afALG_PreBestPositionSet(SINT16 iPreBest);
SINT16 	afALG_PreBestPositionGet(void);

void 	afALG_ContinueAF_ReDo(BOOL bDoContAFAgain );
void  	afALG_AFPreLv1000Set(SINT32  iAfPreLv1000  ); 
SINT32	afALG_AFPreLv1000Get(void);              


void	afALG_Focus_TISR_ProcessGo(SINT32 iGoToPos );
void	afALG_TISR_MovingReset(void);
void	afALG_ContinuedSearchingSet(void);
void	afALG_ContinuedSearchingClr(void);
UINT32	afALG_ContinuedSearchingGet(void);

UINT32	afALG_TISR_InitMove(void);
UINT32	afALG_TISR_Process( SINT16 iMovingSteps );
UINT32	afALG_TISR_MovingDir_Config(UINT32 uEvenCase, SINT16 iSteps, UINT8 bCheckDone);



SINT16	afALG_MacroPosGet(void );
SINT16	afALG_IntPosGet(void );



UINT32 	afALG_RunningCustomCbWait(void);
SINT8 	afALG_IrisPosGet(void);	
UINT32 	afALG_ContinueAF_Check(void);
void 	afALG_CustomCbStart(UINT8 uFlag);
void    afALG_FlagSmartAFTriggerClr(void);
void    afALG_FlagSmartAFTriggerSet(void);
void  	afALG_FlagRoiAfAbrotClr(void);
void  	afALG_FlagRoiAfAbrotSet(void);
UINT8 	afALG_IsFlagRoiAfAbrot(void);
void 	afALG_LrpWeightSet( SINT16 Corner , SINT16 Center  );
void 	afALG_VarSet(UINT32 selector,UINT32 value);
UINT32 	afALG_VarGet(UINT32 selector);
UINT32	afALG_ContinueAF_ConditionEdgeCheck(void);
UINT32	afALG_ContinueAF_ConditionYCheck(void);
BOOL	afALG_GyroStateCheck(void);
BOOL 	afALG_EdgeStableCheck(BOOL Clear);
BOOL 	afALG_YStableCheck(BOOL Clear);
BOOL 	afALG_GyroStableCheck(BOOL Clear);
void    afALG_FlagSmartAFTriggerClr(void);
void    afALG_FlagSmartAFTriggerSet(void);
UINT16 afALG_FocusMode_Set(UINT16 uPara);
UINT16 afALG_FocusMode_Get();
UINT16 afALG_AFArea_Set(UINT16 uPara);
UINT16 afALG_AFArea_Get(void);



#endif 

