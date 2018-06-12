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
 *                                                                        *
 *  Author: Shengkai Lin                                                  *
 *                                                                        *
 *  20150904 ver. 0.00.02, Shane Chuang  
 *  1.add sensor AWB function,sensor_ar0230_wbgain_set(),sensor_wbgain_apply()
 *  20151021 ver. 0.00.03, Shane Chuang  
 *  1.modify pre-gain offset
 *  20160902 ver. 0.00.04, Shane Chuang  
 *  1.modify WB default gain
 *  20161226 ver. 0.00.05, Shane Chuang  
 *  1.default WB gain must set to all 1024
 *  20171113 ver. 0.00.06, Shane Chuang  
 *  1.add BLC for high-temperature ofsset issue
 **************************************************************************/


#define MODEL_VERSION "$unp_afe_ar0230-0.00.06"
#define EVTBL_TEST_CODE 		0
#define AFE_PRE_GAIN_EN         1
#if AFE_PRE_GAIN_EN
#define	TG_NBIT_REGADDR	16		/*ssc/spi register address numbers */
#define	TG_NBIT_REGDATA	8		/*ssc/spi register data numbers */
#define PRE_GAIN_REG_ORDER 3    /*which reg# is pregain ,always the last one*/
#define PRE_GAIN_ADDR 0xE0AF    /* {0xAF,0xE0,PRE_GAIN(0x80~0xFF)}*/
#endif

#include "common.h"
#include "../flow/sensor_model.h"
#include "middleware/proflog.h"

#if SP5K_DIQ_FRMWRK
#define USE_AGC_DELAY_SYNC			 (1)
#else
#define USE_AGC_DELAY_SYNC			 (0)
#endif

/*global valuable*/
/*static UINT16 EV_diff_1=0,EV_diff_2=0;*/

/*prototypes of sensor model methods*/
static void sensor_afe_svc_resstop();
static void sensor_afe_svc_resload();
static void sensor_afe_svc_stop();
static void sensor_afe_svc_init();
static void sensor_afe_mode_set(UINT32 mode);
static void sensor_cmdport_agc_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_dcoffset_write(UINT32 idx);

#if AFE_PRE_GAIN_EN
/*static void sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata);*/
#endif
extern UINT32 sensorModeGet();
extern void sensorOVWrite(UINT16 addr, UINT8 data);
extern void sensor_frame_cb_reg(UINT32 svcid,void *f,UINT32 param);
extern UINT8 ar0230_sensor_hdr_status_get();
extern UINT32 ae_preview_agc_get(void);

extern UINT8 Dummydata[6];
extern UINT8 lastDummydata[2];
extern UINT8 AgcDummySyncWrite;

UINT32 WBRgain = 1024;
UINT32 WBGgain = 1024;
UINT32 WBBgain = 1024;

/*sensor cfg*/
static sensorAfeCfg_t sensorAfeCfg = {
	/*sensor:normal,gray,diff:int;siggen;tv:yuvS*/
	.img_src_type = FRONT_IMG_SRC_SENSOR,
	.img_src_cfg = 0, /*cfg for diff,siggen,tv*/
	.nbit_rgb = FRONT_IMG_SENSOR_BIT_10,
	.afe_effect_frame_count = 1, /*CCD typically 1*/
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
	},
	/*remove this definition if no separate channel support*/
	/****TODO****AFE RGB separate gain,offset*/
	/*.afeRGGB = &(const sensorAfeRGGB_t) { , },*/
	.afeRGGB = NULL,
	.verStr = MODEL_VERSION,
};

static const UINT8 cmd_linear_lowgain_code[][4]={
	{0x30, 0x96, 0x00, 0x80},
	{0x30, 0x98, 0x00, 0x80},
	{0x32, 0x06, 0x0B, 0x08},
	{0x32, 0x08, 0x1E, 0x13},
	{0x32, 0x02, 0x00, 0x80},
	{0x32, 0x00, 0x00, 0x02},
	{0x31, 0x00, 0x00, 0x00},
};

static const UINT8 cmd_linear_highgain_code[][4]={
	{0x30, 0x96, 0x00, 0x80},
	{0x30, 0x98, 0x00, 0x80},
	{0x32, 0x06, 0x1C, 0x0E},
	{0x32, 0x08, 0x4E, 0x39},
	{0x32, 0x02, 0x00, 0xB0},
	{0x32, 0x00, 0x00, 0x02},
	{0x31, 0x00, 0x00, 0x04},
};

static const UINT8 cmd_hdr_lowgain_code[][4]={
	{0x30, 0x96, 0x04, 0x80},
	{0x30, 0x98, 0x04, 0x80},
	{0x32, 0x06, 0x0B, 0x08},
	{0x32, 0x08, 0x1E, 0x13},
	{0x32, 0x02, 0x00, 0x80},
	{0x32, 0x00, 0x00, 0x02},
	{0x31, 0x00, 0x00, 0x00},
};

static const UINT8 cmd_hdr_highgain_code[][4]={
	{0x30, 0x96, 0x07, 0x80},
	{0x30, 0x98, 0x07, 0x80},
	{0x32, 0x06, 0x1C, 0x0E},
	{0x32, 0x08, 0x4E, 0x39},
	{0x32, 0x02, 0x00, 0xB0},
	{0x32, 0x00, 0x00, 0x02},
	{0x31, 0x00, 0x00, 0x04},
};

static const UINT8 cmd_blc_code[][4] SENSOR_SIF1DMA_BUF ={
	{0x30, 0xB4, 0x00, 0x91},
};

void sensorAfe_ar0230_Install()
{
	sensorAfeDevInstall(SENSOR_DEV_ID_SENSOR,&sensorAfeDev);
}

static void
sensor_afe_svc_resstop()
{
	/*call this if this afe driver supports agc table*/
	sensor_agctbl_delete();
}
static void
sensor_afe_svc_resload()
{
	sensorAfeCfg_t *pacfg = &sensorAfeCfg;
	/*if afe shares identical port with tg*/
	pacfg->cmdportid_agc =
	pacfg->cmdportid_dcoffset =
	pacfg->cmdportid_afe = sensorCmdportIdGet(SENSOR_CMDPORTID_SEL_XTG);

#if EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
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
	#if AFE_PRE_GAIN_EN
	   sensorPreGainEnSet(1);  
       sensorPreGainCfg(0x100);
	#endif  

	#if USE_AGC_DELAY_SYNC
		/*sensorStsCfg(SENSOR_SYS_CFG_AGC_DLEAY_SYNC,1);*//*agc is send for every 2VD*/
		sensorStsCfg(SENSOR_SYS_CFG_AGC_DLEAY_SYNC,2);    /*agc is send for every VD*/
	#endif
}
static void
sensor_afe_mode_set(
		UINT32 mode
		)
{
/*****TODO****/
}

void
sensor_ar0230_wbgain_set(UINT32 Rgain, UINT32 Ggain, UINT32 Bgain )
{
	WBRgain = Rgain;
	WBGgain = Ggain;
	WBBgain = Bgain;	
}

static void
sensor_wbgain_apply()
{
	UINT16 sRgain,sGgain,sBgain;
	static UINT8 SENSOR_SIF1DMA_VARBUF regdata1[4][4]={
		{0x30,0x56,0x00,0x00},/*green1*/
		{0x30,0x58,0x00,0x00},/*Blue*/
		{0x30,0x5A,0x00,0x00},/*Red*/
		{0x30,0x5C,0x00,0x00},/*green2*/		
	};
	
	sRgain = (WBRgain / 1024.0)*133.0;
	sGgain = (WBGgain / 1024.0)*133.0;
	sBgain = (WBBgain / 1024.0)*133.0;

	regdata1[0][2] = (sGgain&0xff00)>>8;
	regdata1[0][3] = (sGgain&0x00ff);
	regdata1[1][2] = (sBgain&0xff00)>>8;
	regdata1[1][3] = (sBgain&0x00ff);
	regdata1[2][2] = (sRgain&0xff00)>>8;
	regdata1[2][3] = (sRgain&0x00ff);
	regdata1[3][2] = (sGgain&0xff00)>>8;
	regdata1[3][3] = (sGgain&0x00ff);	

	sensorSerialDirectBufSnoop(regdata1[0], sizeof(regdata1));
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,4, (const UINT8*)regdata1);

}

static void
sensor_cmdport_agc_write(
		UINT32 nreg,
		const UINT8 *pdata
		)
{			
		sensor_wbgain_apply();
#if AFE_PRE_GAIN_EN
		UINT32 pregain;
		UINT8 hdrStatus;
		static UINT8 gainStatus = 0, gainStatusChanged = 0;

		hdrStatus = ar0230_sensor_hdr_status_get();

		static UINT8 SENSOR_SIF1DMA_VARBUF regdata1[1][4]={
			{0x30,0x60,0x00, 0x00},
		};

		if( ae_preview_agc_get() < 52 )/*HCG thr*/
		{
			if( gainStatus == 1 )
			{
				gainStatusChanged = 1;
			}
			gainStatus = 0;
		}
		else
		{
			if( gainStatus == 0 )
			{
				gainStatusChanged = 1;
			}
			gainStatus = 1;
		}
		
		if( gainStatusChanged )
		{
			if( gainStatus )
			{
				if( hdrStatus )
				{
					sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc, sizeof(cmd_hdr_highgain_code)/sizeof(cmd_hdr_highgain_code[0]), (const UINT8*)cmd_hdr_highgain_code);
				}
				else
				{
					sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc, sizeof(cmd_linear_highgain_code)/sizeof(cmd_linear_highgain_code[0]), (const UINT8*)cmd_linear_highgain_code);
				}
			}
			else
			{
				if( hdrStatus )
				{
					sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc, sizeof(cmd_hdr_lowgain_code)/sizeof(cmd_hdr_lowgain_code[0]), (const UINT8*)cmd_hdr_lowgain_code);
				}
				else
				{
					sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc, sizeof(cmd_linear_lowgain_code)/sizeof(cmd_linear_lowgain_code[0]), (const UINT8*)cmd_linear_lowgain_code);
				}
			}
			gainStatusChanged = 0;
			/*sensorSerialFlush(sensorAfeCfg.cmdportid_agc);*/
		}
		
		/*sensor_afe_cmd_paser(nreg,pdata,agc_addr,agc_data,TG_NBIT_REGADDR,TG_NBIT_REGDATA);*/
		
		regdata1[0][2] = pdata[2];
		regdata1[0][3] = pdata[3];

		pregain= pdata[6]*0x100 + pdata[7]; /*0x100~0x200*/
		/*sensor_frame_cb_reg(12,sensorPreGainCfg,pregain);*/ /*rising, next SOF*/
		sensorPreGainCfg(pregain);
		/*printf("%04x %02x%02x\n", pregain, regdata1[0][2], regdata1[0][3]);*/
		sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc, sizeof(cmd_blc_code)/sizeof(cmd_blc_code[0]), cmd_blc_code[0]);/*BLC trigger*/
		sensorSerialDirectBufSnoop(regdata1[0], sizeof(regdata1));
		sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc, 1, (const UINT8*)regdata1);
#else
	sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
#endif
}
/*e modified by Eric Lee 20100413 */
static void
sensor_cmdport_dcoffset_write(
		UINT32 idx
		)
{
}
#if 0
static void
sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata)
{
	UINT8 i,j;
	UINT8 nbyte_addr,nbyte_data,nbyte_len;
	UINT8 offset,idx;

	nbyte_addr=nbitaddr/8;
	nbyte_data=nbitdata/8;
	nbyte_len=nbyte_addr+nbyte_data;
	
	for(i=0;i<nreg;i++)
	{
		offset=nbyte_len*i;
		addr[i]=0;
		idx=offset;
		for(j=0;j<nbyte_addr;j++)
			addr[i]|=pdata[idx+j]<<(8*j);

		data[i]=0;
		idx=offset+nbyte_addr;
		for(j=0;j<nbyte_data;j++)
			data[i]|=pdata[idx+j]<<(8*j);
			
			
	}
}
#endif

