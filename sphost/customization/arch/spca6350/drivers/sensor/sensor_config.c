/**************************************************************************
 *
 *       Copyright (c) 2004-2018 by iCatch Technology, Inc.
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
 *  Author: Ricky Lin
 *                                                                        
 *  Version: 0.01, 2014.1.22: 
 *  for 6350 RDK EVB  */
 
#include <stdio.h>
#include "common.h"
#include "api/sp5k_global_api.h"
#include "middleware/proflog.h"
#include "sensor_config.h"
#include "debug_mod/dbg_api.h"
#include "flow/sensor_model.h"

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define SENSOR_PROF_LOG_EN             	(1)

#if SENSOR_PROF_LOG_EN
    #define PROF_ID                                 (0)
    #define SENSOR_PROF_LOG_ADD(str)                profLogAdd(PROF_ID, str)
    #define SENSOR_PROF_LOG_PRINT(fmt, arg...)      profLogPrintf(PROF_ID, fmt, ##arg)     
#else
    #define SENSOR_PROF_LOG_ADD(str)
    #define SENSOR_PROF_LOG_PRINT(fmt,arg...)
#endif
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
static UINT8 sensorID;
static UINT8 mainSensor;
static UINT8 bk1Sensor;

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
extern void sensorSIFInit();
/*-------------------------------------------------------------------------
 * Function : devFrontTgDetect
 *------------------------------------------------------------------------*/
void devFrontTgDetect(void)
{
    SENSOR_PROF_LOG_ADD("devFrontTgDetect() s");
#if SP5K_SENSOR_SONYIMX175
    mainSensor=SENSOR_ID_SONYIMX175;
#elif SP5K_SENSOR_SONYIMX200
    mainSensor=SENSOR_ID_SONYIMX200;
#endif
#if SP5K_SENSOR_BK1_SONYIMX175
    bk1Sensor=SENSOR_ID_SONYIMX175;
#elif SP5K_SENSOR_BK1_SONYIMX200
    bk1Sensor=SENSOR_ID_SONYIMX200;
#endif
    SENSOR_PROF_LOG_PRINT("mainSensor=%d, bk1Sensor=%d",mainSensor,bk1Sensor);

#if SP5K_SENSOR_SONYIMX175||SP5K_SENSOR_BK1_SONYIMX175
	sensorSIFInit();
	tmrUsWait(30000);/*30ms*/	
    sensorTg_sonyimx175_Install();
	sensorAfe_sonyimx175_Install();	
	sensor_exptbl_delete();
	sensor_agctbl_delete();	
	if(sensor_sonyimx175_ReadID()){
		sensorID=SENSOR_ID_SONYIMX175;		
	    return;
	}
#endif
#if SP5K_SENSOR_SONYIMX200||SP5K_SENSOR_BK1_SONYIMX200
	sensorSIFInit();
	tmrUsWait(30000);/*30ms*/	
    sensorTg_sonyimx200_Install();
	sensorAfe_sonyimx200_Install();	
	sensor_exptbl_delete();
	sensor_agctbl_delete();	
	if(sensor_sonyimx200_ReadID()){
		sensorID=SENSOR_ID_SONYIMX200;
	    return;
	}
#endif
    SENSOR_PROF_LOG_ADD("devFrontTgDetect() e");
}

/*-------------------------------------------------------------------------
 * Function : getSensorID
 *------------------------------------------------------------------------*/
UINT8 getSensorID(void)
{
    return sensorID;
}

/*-------------------------------------------------------------------------
 * Function : sensorCmdPortExpLineSend
 *------------------------------------------------------------------------*/
#if SP5K_SENSOR_BACKUP_ENABLE
void sensorCmdPortExpLineSend(UINT16 expLine)
{
#if SP5K_SENSOR_SONYIMX175||SP5K_SENSOR_BK1_SONYIMX175
	if(SENSOR_ID_SONYIMX175==sensorID){
		sony_imx175_CmdPortExpLineSend(expLine);	    
	}
#endif
#if SP5K_SENSOR_SONYIMX200||SP5K_SENSOR_BK1_SONYIMX200
	if(SENSOR_ID_SONYIMX200==sensorID){
		sony_imx200_CmdPortExpLineSend(expLine);	    
	}
#endif
}
#endif /*SP5K_SENSOR_BACKUP_ENABLE*/
