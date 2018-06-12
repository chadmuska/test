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
 *  Author: Potato Chang                                                   *
 *                                                                        *
 **************************************************************************/
#ifndef _VCM_DrvDW9714A_H_
#define _VCM_DrvDW9714A_H_

#define DW9800W_RESONANCE_DOUBLE	0x40
#define DW9800W_RESONANCE_DEFAULT	0x60
#define DW9800W_RESONANCE_HALF		0xC0
#define DW9800W_RESONANCE_QUARTER	0xE0

UINT32 VCM_DrvDW9800W_StatusInit(void);

UINT32 VCM_DrvDW9800W_I2CWrite( UINT8 uByte1 , UINT8 uByte2 );

UINT32 VCM_DrvDW9800W_I2CRead( UINT32 Addr, UINT32 *Data );

UINT32 VCM_DrvDW9800W_ModeSet( UINT32 ulMode);

UINT32 VCM_DrvDW9800W_StepSet( UINT32 ulStep);

UINT32 VCM_DrvDW9800W_PowerDown( UINT32 uPD);

UINT32 VCM_DrvDW9800W_ResonanceDivSet(UINT32 uDiv);

#endif 

