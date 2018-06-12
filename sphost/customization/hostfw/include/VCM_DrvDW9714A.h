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
#ifndef _VCM_DrvDW9714A_H_
#define _VCM_DrvDW9714A_H_


UINT32  VCM_DrvDW9714A_StatusInit(void);

UINT32 VCM_DrvDW9714A_I2CWrite( UINT8 uByte1 , UINT8 uByte2 );

UINT32 VCM_DrvDW9714A_I2CRead( UINT32 Addr, UINT32 *Data );

UINT32 VCM_DrvDW9714A_ModeSet( UINT32 ulMode);

UINT32 VCM_DrvDW9714A_SleepSet( UINT32 ulSleep);

UINT32 VCM_DrvDW9714A_StepSet(SINT32 ulStep);

UINT32 VCM_DrvDW9714A_IsStepReady(UINT32 ulStep);

UINT32 VCM_DrvDW9714A_TransitionTimeGet(  PS_VCM_Status pVCMStatus , SINT32 uToDegree );

UINT32 VCM_DrvDW9714A_PowerControl(UINT32 ulPower);

UINT32 VCM_DrvDW9714A_PowerOnInit(void);

void  VCM_DrvDW9714A_TSRCTable_Use(UINT32 value);


#endif 
