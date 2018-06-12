/*************************************************************************
 *                                                                        
 *       Copyright (c) 2004-2006 by Sunplus Technology Co., Ltd.          
 *                                                                        
 *  This software is copyrighted by and is the property of Sunplus        
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    
 *  Co., Ltd. This software may only be used in accordance with the       
 *  corresponding license agreement. Any unauthorized use, duplication,   
 *  distribution, or disclosure of this software is expressly forbidden.  
 *                                                                        
 *  This Copyright notice MUST not be removed or modified without prior   
 *  written consent of Sunplus Technology Co., Ltd.                       
 *                                                                        
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        
 *  software without notice.                                              
 *                                                                        
 *  Sunplus Technology Co., Ltd.                                          
 *  19, Innovation First Road, Science-Based Industrial Park,             
 *  Hsin-Chu, Taiwan                                                      
 *                                                                        
 *  Author: Vincent Tsao                                                  
 *                                                                        
 *  Version: 0.00.01, 2015.7.22:
 *  for 6350 EVB  
 *  dual sensor application
 *
 *
 *
 *
*/
#define MODEL_VERSION "$unp_tg-ar0230tp2825-0.00.01"
#define BUILD_TIME        __DATE__  "-"  __TIME__

#include <stdio.h>
#include "common.h"
#include "customization/dev_init.h"
#include "../flow/hal_front.h"
#include "middleware/sensor_model.h"
#include "api/sp5k_dbg_api.h"
#include "middleware/proflog.h"
/*declare dual sensor  install */
extern	void sensorTg_ar0230_Install();
extern	void sensorTg_tp2825_Install();
extern	void sensorAfe_ar0230_Install();
extern	void sensorAfe_tp2825_Install();
void sensorDualInit(void);
void sensorDualModeSwBegin(UINT32 s0_mode,UINT32 s1_mode);
void sensorDualModeSwNext(UINT32 s0_next_mode,UINT32 s1_next_mode);


static const sensorDualDev_t sensorDualDev = {
	.Svc = &(const sensorDualSvc_t) {
		.init 		= sensorDualInit,
		.modeswBegin= sensorDualModeSwBegin,
		.modeswNext = sensorDualModeSwNext,
	},
	.verStr =MODEL_VERSION,
};

void
sensorTg_ar0230_tp2825_Install()
{
	printf("@#@ sensorTg_ar0230_tp2825_Install TG install!!!\n");

	sensorDualDevInstall(&sensorDualDev);
	sensorTg_ar0230_Install();
	sensorTg_tp2825_Install();
}
void
sensorAfe_ar0230_tp2825_Install()
{
	printf("@#@ sensorAfe_ar0230_tp2825_Install AFE install!!!\n");

	sensorAfe_ar0230_Install();
	sensorAfe_tp2825_Install();
}
void sensorDualInit()
{
	profLogPrintf(3,"________Sensor driver ver <<%s>>", MODEL_VERSION);
	profLogPrintf(3,"________Sensor driver build time <<%s>>", BUILD_TIME);

	printf("@#@ ar0230+TP2825 sensor driver Initial!!!\n");
	sensorModelApiCfgSet(SENSOR_API_DUAL_PCLK1X,300000,0);/*set pclk1x maximum value for both sensor.*/
	sensorModelApiCfgSet(SENSOR_API_DUAL_CDSPCLK,300000,0);/*set cdsp clock maximum value for both sensor.*/
}

void sensorDualModeSwBegin(UINT32 s0_mode,UINT32 s1_mode)
{
	profLogPrintf(3,"sensorDualModeSwBegin(%x %x)",s0_mode,s1_mode);
}
void sensorDualModeSwNext(UINT32 s0_next_mode,UINT32 s1_next_mode)
{
	profLogPrintf(3,"sensorDualModeSwNext(%x %x)",s0_next_mode,s1_next_mode);
}

#if SP5K_SENSOR_AR0230_TP2825
extern void ar0230_sensorCmdPortExpLineSend(UINT16 expLine);
void sensorCmdPortExpLineSend(UINT16 expLine)
{
	ar0230_sensorCmdPortExpLineSend(expLine);
}
#endif

