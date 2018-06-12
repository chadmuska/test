/**************************************************************************
 *
 *       Copyright (c) 2009-2018 by iCatch Technology, Inc.
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
 *  Author: Ricky Lin                                                  *
 *                                                                         *
 **************************************************************************/
#ifndef _CUSTOM_SENSOR_CONFIG_H_

#define _CUSTOM_SENSOR_CONFIG_H_

typedef enum {
	SENSOR_ID_NULL,
#if SP5K_SENSOR_SONYIMX175||SP5K_SENSOR_BK1_SONYIMX175
	SENSOR_ID_SONYIMX175,
#endif
#if SP5K_SENSOR_SONYIMX200||SP5K_SENSOR_BK1_SONYIMX200
	SENSOR_ID_SONYIMX200,
#endif
} sensorId_t;

#if SP5K_SENSOR_SONYIMX175||SP5K_SENSOR_BK1_SONYIMX175
    UINT32 sensor_sonyimx175_ReadID(void);
	void sensorTg_sonyimx175_Install(void);
	void sensorAfe_sonyimx175_Install(void);
#endif

#if SP5K_SENSOR_SONYIMX200||SP5K_SENSOR_BK1_SONYIMX200
    UINT32 sensor_sonyimx200_ReadID(void);
	void sensorTg_sonyimx200_Install(void);
	void sensorAfe_sonyimx200_Install(void);
#endif

void devFrontTgDetect(void);
UINT8 getSensorID(void);

#endif /* _CUSTOM_SENSOR_CONFIG_H_*/

