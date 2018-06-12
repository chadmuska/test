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
 *  Author: Ryan Kuo                                                      
 *  Version: 0.01, 2011.9.22:                                             
 *                                                                        
 *************************************************************************/

#define MODEL_VERSION "$unp_afe_Fake-2.00.13"


#include "common.h"
#include "../flow/sensor_model.h"

#if SP5K_DIQ_FRMWRK
#define USE_AGC_DELAY_SYNC			 (1)
#else
#define USE_AGC_DELAY_SYNC			 (0)
#endif
#define AFE_PRE_GAIN_EN 			 (0)
#define AFE_OB_BASE_GAIN_EN 		 (0)


/*prototypes of sensor model methods*/
static void sensor_afe_svc_resstop();
static void sensor_afe_svc_resload();
static void sensor_afe_svc_stop();
static void sensor_afe_svc_init();
static void sensor_afe_mode_set(UINT32 mode);
static void sensor_cmdport_agc_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_dcoffset_write(UINT32 idx);
#if AFE_PRE_GAIN_EN
static void sensor_cmdport_agcetbl_do(UINT32 sv_error, UINT32 err_unit);
#endif


/*sensor cfg*/
static sensorAfeCfg_t sensorAfeCfg = {
	/*sensor:normal,gray,diff:int;siggen;tv:yuvS*/
	.img_src_type = FRONT_IMG_SRC_SENSOR,
	.img_src_cfg = 0x00,                     /*cfg for diff,siggen,tv*/
	.nbit_rgb = FRONT_IMG_SENSOR_BIT_10,
	.afe_effect_frame_count = 1,             /*CCD typically 1*/
};

static const sensorAfeDev_t sensorAfeDev = {
	.afeCfg = &sensorAfeCfg,
	.afeSvc = &(const sensorAfeSvc_t) {
		.resload = sensor_afe_svc_resload,
		.resstop = sensor_afe_svc_resstop,                  
    	.init = sensor_afe_svc_init,                        
    	.stop = sensor_afe_svc_stop,                        
    	.pwrsave = sensor_afe_mode_set,                     
    	.agcwrite = sensor_cmdport_agc_write,               
    	.dcoffsetwrite = sensor_cmdport_dcoffset_write, 
#if AFE_PRE_GAIN_EN
		.agcetbldo =sensor_cmdport_agcetbl_do,
#endif
		},
	/*remove this definition if no separate channel support*/          
  	/****TODO****AFE RGB separate gain,offset*/                        
  	/*.afeRGGB = &(const sensorAfeRGGB_t) { , },*/                     
	.afeRGGB = NULL,
	.verStr = MODEL_VERSION,
};

void sensorAfe_Fake_Install()
{
	sensorAfeDevInstall(SENSOR_DEV_ID_SENSOR,&sensorAfeDev);
}

static void
sensor_afe_svc_resstop()
{
	/*call this if this afe driver supports agc table*/
	sensor_agctbl_delete();
}
#ifdef EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
DECLARE_AGCTBL();
#endif

static void
sensor_afe_svc_resload()
{
	sensorAfeCfg_t *pacfg = &sensorAfeCfg;                                   
  	/*if afe shares identical port with tg*/                                 
  	pacfg->cmdportid_agc =                                                   
  	pacfg->cmdportid_dcoffset =                                              
  	pacfg->cmdportid_afe = sensorCmdportIdGet(SENSOR_CMDPORTID_SEL_XTG);     
#ifdef EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
		sensor_agctbl_create(afetbl_stream, sizeof(afetbl_stream));
#else
		sensor_agctbl_init();
#endif

}

static void
sensor_afe_svc_stop()
{
}
static void
sensor_afe_svc_init()
{
	#if 0                                             
  		sensorAfeCfg_t *pacfg = &sensorAfeCfg;            
  	    UINT32 cmdportid;                             
  	    static const UINT8 regdata[][2] = {           
  	    	{0x11, 0x40,},                              
  	    };                                            
		/*if afe shares identical port with tg*/
		cmdportid = pacfg->cmdportid_afe;
		sensorSerialBulkWrite(cmdportid, sizeof(regdata)/sizeof(regdata[0]), regdata[0],0);
	#endif
	#if AFE_PRE_GAIN_EN
		sensorPreGainEnSet(1);  
        sensorPreGainCfg(0x100);
	#endif  

	#if USE_AGC_DELAY_SYNC
		sensorStsCfg(SENSOR_SYS_CFG_AGC_DLEAY_SYNC,1);
	#endif
}
static void
sensor_afe_mode_set(UINT32 mode)
{
/*****TODO****/
}
#if AFE_PRE_GAIN_EN
static void sensor_cmdport_agcetbl_do(UINT32 sv_error, UINT32 err_unit)
{
	SINT32 exp;
	UINT32 agc;
	extern UINT32 sp5kPreviewExpAgcGet(SINT32 *pexpIdx,UINT32 *pagcIdx);
	sp5kPreviewExpAgcGet(&exp, &agc);

	profLogPrintf(3,"expidx=%d agcidx=%d sensor_cmdport_agcetbl_do(%d %d)",exp,agc,sv_error,err_unit);
	sensorPreGainSvIdxCfg(sv_error);
}
#endif

static void
sensor_cmdport_agc_write(UINT32 nreg, const UINT8 *pdata)
{
#if AFE_OB_BASE_GAIN_EN
	sensorObBaseGainCfg(0x111);/*(4096)/(4096-256)*256 */
#endif

    /*don't control SIF due to fake sensor*/
}
static void
sensor_cmdport_dcoffset_write(UINT32 idx)
{
}

