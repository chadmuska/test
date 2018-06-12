/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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
 *                                                                        *
 *  Author: Vance Hunag                                                   *
 *                                                                        *
 **************************************************************************/
#ifndef _VCMFOCUS_H_
#define _VCMFOCUS_H_

#if 0 
#define VCM_DRIVER_OV8810Internal

#elif (defined SP5K_SENSOR_SAMSUNG3H2)||(defined SP5K_SENSOR_OV5693) ||(defined SP5K_SENSOR_AR0543) 
#define VCM_DRIVER_AD5823

#elif (defined SP5K_SENSOR_SONYIMX175)||(defined SP5K_SENSOR_OV8830)
#define VCM_DRIVER_DW9714A

#elif (defined SP5K_SENSOR_OV5650)
#define VCM_DRIVER_DW9714A  

#elif (defined SP5K_SENSOR_SONYIMX091)
	#ifdef SP5K_VCM_DRV_AD5816
		#define VCM_DRIVER_AD5816
	#else
		#define VCM_DRIVER_DW9714A
	#endif

#elif (defined SP5K_SENSOR_OV8835)
#define VCM_DRIVER_TI201

#elif (defined SP5K_SENSOR_SONYIMX111)
#define VCM_DRIVER_R2A30451BM

#else
#define VCM_DRIVER_DW9714A
#endif

#pragma pack(1)
typedef struct tag_VCM_Status
{
 UINT32 ulPower;
 UINT32 ulExtClk;
 UINT32 uVCMMode;
 UINT32 uSleepType;
 UINT32 ulSlew;
 UINT32 uCurDegree;
 UINT32 uTransitionTime;  
 UINT32 ulSlaveAddr;
 UINT32 ulADDRBit;
 UINT32 ulDATABit;
 UINT32 ulCmdPortID;
 UINT32 ulCmdFormat2;
 UINT32 ulCmdFormat3;
 UINT32 ulCmdSleep;
}S_VCM_Status , *PS_VCM_Status;
#pragma pack()

extern S_VCM_Status _gsVCMStatus;


UINT32 afALG_VCM_Sensor_IICWrite( UINT32 Addr,UINT32 Data );
UINT32 afALG_VCM_Sensor_IICRead( UINT32 Addr,UINT32 *Data );
UINT32 afALG_VCM_IICWrite( UINT32 Addr, UINT32 Data );
UINT32 afALG_VCM_IICRead( UINT32 Addr, UINT32 *Data );
UINT32 afALG_VCM_ModeSet( UINT32 ulMode);
UINT32 afALG_VCM_SleepSet( UINT32 ulSleep);
UINT32 afALG_VCM_StepSet(SINT32 ulStep);
UINT32 afALG_VCM_PowerControl(UINT32 ulPower);
UINT32 afALG_VCM_PowerOnInit(void);
UINT32 afALG_VCM_StatusInit( void );
UINT32 afALG_VCM_CurDegreeGet(void); 
UINT32 afALG_VCM_GoPosition( SINT32 iPos ); 
void	afALG_VCM_GoPosition_No_Watting( SINT32 iPos );
UINT32 afALG_VCM_TransitionTimeGet(  PS_VCM_Status pVCMStatus , SINT32 uToDegree );

UINT32 	afALG_Calibration_NextZoom (void);
UINT32 	afALG_Calibration_Focus (UINT16 wFocusCaliMode, UINT16 wIdxDist);
void 	afALG_NoiseThreadTest(SINT16 FocusPos , UINT32 uNoiseMax , UINT32 uCorThMax  , UINT32 uCorValMax );
UINT32 	afALG_ContinueAF_EdgeGet( void );

#endif
