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
 *  Author: Will Chiu                                                     *
 *                                                                        *
 *  ver. 0.00.02, 20141230, Vincent Tsao
 *    1. Change the timing of setting middle exp/gain and short exp/gain. Change the setting timing to
 *        the position between group hold and group release.
 *
 *  ver. 0.00.03, 20150525, Vincent Tsao
 *    1. Add the function to check the steps per EV in AGC table.  This information will be used to
 *       calculate the gain value for HDR use.
 *    2. Modify the agc index calculation function used in HDR.
 *
 *  ver. 0.00.04, 20151207, Vincent Tsao
 *    1. Add one flag (g_ucRetryInit) to disable the agc settings when processing the initialization
 *        in hot plug in/out.
 *
 *  ver. 0.00.05, 20160902, Vincent Tsao
 *    1. Add the mode swicth flag check before using sensor_frame_cb_reg to avoid registering 
 *       the same callback too many times and cause the system program dead.
 *
 *  ver. 0.00.06, 20161003, Vincent Tsao
 *    1. Change the timing sensor pregain configuration to CDSP SOF. This could make sure that
 *        sensor pregain become effective at the correct time.
 **************************************************************************/


#define MODEL_VERSION "$unp_afe_ov4689-5.07.0F"
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

#define READ8(_value_)          (*((volatile UINT8*)(_value_)))

#define SENSOR_IRQ_SVC_CDSP_SOF 8

/*global valuable*/
static UINT16 EV_diff_1=0,EV_diff_2=0;
UINT8  preGainDelay = 0;
const UINT8 *gpdata;

/*prototypes of sensor model methods*/
static void sensor_afe_svc_resstop();
static void sensor_afe_svc_resload();
static void sensor_afe_svc_stop();
static void sensor_afe_svc_init();
static void sensor_afe_mode_set(UINT32 mode);
static void sensor_cmdport_agc_write(UINT32 nreg,const UINT8 *pdata);
static void sensor_cmdport_dcoffset_write(UINT32 idx);
void sensor_HDR_ISO_do(void);

#if AFE_PRE_GAIN_EN
static void sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata);
#endif
extern UINT32 sensorModeGet();
extern void sensorOVWrite(UINT16 addr, UINT8 data);
extern void sensor_frame_cb_reg(UINT32 svcid,void *f,UINT32 param);
/*s Added by Eric 100413 */
extern UINT8 Dummydata[6];
extern UINT8 lastDummydata[2];
extern UINT8 AgcDummySyncWrite;
/*e Added by Eric 100413 */

typedef struct {
	UINT16 agcIdxMin;
	UINT16 agcIdxMax;
} sensor_agcidx_limits_t;

static UINT8 g_ucAgcTblStepsPerEV;

static UINT8 g_ucMiddleAgcData[][3] SENSOR_SIF1DMA_VARBUF = {
	{0x35,0x0E,0x00},
	{0x35,0x0F,0x00},
};

static UINT8 g_ucShortAgcData[][3] SENSOR_SIF1DMA_VARBUF = {
	{0x35,0x14,0x00},
	{0x35,0x15,0x00},
};

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

void sensorAfe_ov4689_Install()
{
	sensorAfeDevInstall(SENSOR_DEV_ID_SENSOR,&sensorAfeDev);
}

#if EVTBL_TEST_CODE /****TODO*5110*evtbl resource workaround*/
/*afe tables*/
static const UINT8 afetbl_stream[]
COMMON_ALIGNED = {
64,
0,
1,
0,
16,
1,
0x30,
0x00,
0,
64,
1,
0,
1,
0,
0,
16,
8,
0,
0,
1,
2,
3,
3,
4,
5,
6,
7,
8,
9,
10,
12,
13,
14,
15,
15,
17,
18,
19,
19,
20,
21,
22,
23,
24,
25,
26,
28,
29,
30,
31,
31,
49,
50,
51,
51,
52,
53,
54,
55,
56,
57,
58,
60,
61,
62,
63,
63,
113,
114,
115,
115,
116,
117,
118,
119,
120,
121,
122,
124,
125,
126,
64,
0,
64,
0,
4,
2,
1,
0,
4,
3,
3,
3,
2,
3,
3,
3,
0,
1,
2,
3,
7,
2,
1,
0,
4,
3,
3,
3,
2,
3,
3,
3,
0,
1,
2,
3,
7,
2,
1,
0,
4,
3,
3,
3,
2,
3,
3,
3,
0,
1,
2,
3,
7,
2,
1,
0,
4,
3,
3,
3,
2,
3,
3,
3,
0,
1,
2
};
#endif

static UINT8 sensor_afe_agc_tbl_steps_per_ev_get(void)
{
	sensor_agcidx_limits_t t_AgcIndexLimit;
	extern UINT32 sensor_agcidx_limits_get(UINT32 mode,sensor_agcidx_limits_t *palimit);

	sensor_agcidx_limits_get(SENSOR_MODE_PREVIEW,&t_AgcIndexLimit);

	profLogPrintf(3,"@#@ AgcMin=%d,AgcMax=%d",t_AgcIndexLimit.agcIdxMin,t_AgcIndexLimit.agcIdxMax);
	
	if ((t_AgcIndexLimit.agcIdxMax-t_AgcIndexLimit.agcIdxMin)> 255)
		return 64;
	else
		return 16;
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

	g_ucAgcTblStepsPerEV=sensor_afe_agc_tbl_steps_per_ev_get();

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
UINT8
sensor_gain_MSBreg_cal(UINT32 gain)
{
	UINT32 integer;
	UINT8  MSB = 0;

	integer = gain/1000;

	if(integer<2){
		MSB = 0;
	}else if(integer<4){
		MSB = 1;
	}else if(integer<8){
		MSB = 3;
	}else if(integer<=15){
		MSB = 7;
	}
		
	return MSB;
}
UINT8
sensor_gain_LSBreg_cal(UINT32 gain)
{
	UINT32 integer = 0;
	UINT8  LSB;

	if(gain<2000){
		integer = (gain*128)/1000;
	}else if(gain<4000){
		integer = (gain*64)/1000-8;
	}else if(gain<8000){
		integer = (gain*32)/1000-12;
	}else if(gain<=15500){
		integer = (gain*16)/1000-8;
	}
	LSB = (UINT8)integer;	
	return LSB;
}

UINT32
sensor_gain_cal(SINT16 entry_idx)
{
	UINT32 Anlgain = 0;

	UINT16 udwAgcEvUnit;
	udwAgcEvUnit = g_ucAgcTblStepsPerEV;	
	
	if(entry_idx >= 0 && entry_idx < (udwAgcEvUnit*1)){
		Anlgain = (entry_idx*((2000-1000)*1000/(udwAgcEvUnit)))/1000+1000;  /*rounding:entry_idx*((200-100)/16)*/
	}
	else if(entry_idx >= (udwAgcEvUnit*1) &&entry_idx < (udwAgcEvUnit*2)){
		Anlgain = ((entry_idx-(udwAgcEvUnit*1))*((4000-2000)*1000/udwAgcEvUnit))/1000+2000;  /*rounding:(entry_idx-16)*((400-200)/16)/+200*/
	}
	else if(entry_idx >= (udwAgcEvUnit*2) &&entry_idx < (udwAgcEvUnit*3)){
		Anlgain = (entry_idx-(udwAgcEvUnit*2))*((8000-4000)/udwAgcEvUnit)+4000;
	}
	else if(entry_idx >= (udwAgcEvUnit*3) &&entry_idx < (udwAgcEvUnit*4)){
		Anlgain = (entry_idx-(udwAgcEvUnit*3))*((16000-8000)/udwAgcEvUnit)+8000;
	}
		
	return Anlgain;
}

static void
sensor_cmdport_agc_write(
		UINT32 nreg,
		const UINT8 *pdata
		)
{
	/*
	printf("\n%d\nAgc:%d %d %d \n%d %d %d \n%d %d %d \n%d %d %d \n%d %d %d \n",nreg,*(pdata+0),*(pdata+1),*(pdata+2),
																		*(pdata+3),*(pdata+4),*(pdata+5),
																		*(pdata+6),*(pdata+7),*(pdata+8),
																		*(pdata+9),*(pdata+10),*(pdata+11));
	*/		
	UINT8 ucCurSenMode;

	ucCurSenMode = sensorModeGet();

#if (SP5K_SERDES_SPCA100_SPCA101_CONNECT_ENABLE)
	extern UINT8 g_ucRetryInit;
	if (g_ucRetryInit)
	{
		return;
	}
#endif
	
#if AFE_PRE_GAIN_EN
		UINT32 agc_addr[4],agc_data[4];
		
		static UINT8 ucSensorModeRec;
		UINT8 ucModeSwFlg=0;
			
		sensor_afe_cmd_paser(nreg,pdata,agc_addr,agc_data,TG_NBIT_REGADDR,TG_NBIT_REGDATA);

		/*printf("%x %x %x %x\n",agc_addr[0],agc_addr[1],agc_addr[2],agc_addr[3]);
		printf("%d %d %d %d %d\n",agc_data[0],agc_data[1],agc_data[2],agc_data[3],PRE_GAIN_REG_ORDER);
		*/
		/*profLogPrintf(1,"agc_addr[0]:0x%x, agc_addr[1]:0x%x",agc_addr[0],agc_addr[PRE_GAIN_REG_ORDER]);*/

		if(ucCurSenMode!=ucSensorModeRec)
			ucModeSwFlg=1;
		else
			ucModeSwFlg=0;
		
		if(agc_addr[PRE_GAIN_REG_ORDER-1]==PRE_GAIN_ADDR)
		{
			UINT32 pregain;
			pregain=(agc_data[PRE_GAIN_REG_ORDER]*256+agc_data[PRE_GAIN_REG_ORDER-1]); /*0x100~0x200*/
			
			/*printf("pregain:%d %d\n",pregain,agc_data[2]+agc_data[3]*256);*/
			if(ucCurSenMode==0x40)
			{
				sensorSnapPreGainCfg(pregain);		
				/*			
				profLogPrintf(0,"[2:RegCnt:%d] [%x %x %x %x]",sizeof(regdata)/sizeof(regdata[0]),regdata[0][0],regdata[0][1],regdata[0][2],regdata[0][3]);
				profLogPrintf(0,"[%x %x %x %x]",regdata[1][0],regdata[1][1],regdata[1][2],regdata[1][3]);	
				sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,(sizeof(regdata)/sizeof(regdata[0]))-1, regdata); 		
				*/				
			}
			else
			{
				if (ucModeSwFlg)
				{
					sensorPreGainCfg(pregain);
				}
				else
				{
					if (!(READ8(0xb0009090)&0x01)) /*Check VD watch dog thread which enables test pattern or not.*/
					{
						/*Current preview is from sensor not from TG test pattern. frame callback only registered when preview is from sensor.*/
						if(preGainDelay){			
							sensor_frame_cb_reg(SENSOR_IRQ_SVC_CDSP_SOF,sensorPreGainCfg,pregain); /*change to CDSP SOF*/
						}
						else{				
							sensorPreGainCfg(pregain);
						}
					}
				}
			}
	
			/*
			profLogPrintf(1,"[RegCnt:%d] [%x %x %x %x]",nreg,pdata[0],pdata[1],pdata[2],pdata[3]);
			*/
			sensorSerialDirectBufSnoop((UINT8 *)pdata, (nreg-2)*4);
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,(nreg-2), pdata);
			
		}

		ucSensorModeRec = ucCurSenMode;
		
#else
		sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,nreg, pdata);
#endif
		if(ucCurSenMode==0x41 || ucCurSenMode==0x35 ||  ucCurSenMode==0x39 || ucCurSenMode==0x3A)
		{
			gpdata = pdata;
			if (ucCurSenMode==0x3A)
			{
				sensor_HDR_ISO_do();
				sensorSerialDirectBufSnoop(g_ucMiddleAgcData[0], sizeof(g_ucMiddleAgcData));
				sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,sizeof(g_ucMiddleAgcData)/sizeof(g_ucMiddleAgcData[0]), g_ucMiddleAgcData[0]);
				sensorSerialDirectBufSnoop(g_ucShortAgcData[0], sizeof(g_ucShortAgcData));
				sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,sizeof(g_ucShortAgcData)/sizeof(g_ucShortAgcData[0]), g_ucShortAgcData[0]);
				/*profLogPrintf(3,"#@# Send HDR Middle/Short Agc Settings!!! ");*/
			}
		}
}
/*e modified by Eric Lee 20100413 */
static void
sensor_cmdport_dcoffset_write(
		UINT32 idx
		)
{
}
#if AFE_PRE_GAIN_EN
static void
sensor_afe_cmd_paser(UINT32 nreg,const UINT8 *pdata,UINT32 *addr,UINT32 *data,UINT8 nbitaddr,UINT8 nbitdata)
{
	UINT8 i,j;
	UINT8 nbyte_addr,nbyte_data,nbyte_len;
	

	nbyte_addr=nbitaddr/8;
	nbyte_data=nbitdata/8;
	nbyte_len=nbyte_addr+nbyte_data;
	
	for(i=0;i<nreg;i++)
	{
		UINT8 offset,idx;
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
void
sensor_HDR_ev_diff_set(UINT16 diff1, UINT32 diff2)
{
	EV_diff_1 = diff1;
	EV_diff_2 = diff2;
}
void
sensor_PregainDelay(UINT8 flag)
{
	preGainDelay=flag;
}
void
sensor_HDR_ISO_do(void)
{
	#define HDR_debug 0
	
	
	SINT16 ev1_entry = 0, ev2_entry = 0;
	UINT8  gain_reg[2];
	UINT8 ucSensorMode;

	ucSensorMode = sensorModeGet();
	
	/*HDR middle & short gain set*/
	if(ucSensorMode==0x41 || ucSensorMode==0x35 || ucSensorMode==0x39|| ucSensorMode==0x3A)
	{
		UINT32 Analog_gain = 0;
		UINT16 Agc_idx = 0;
		if(gpdata[2] == 0){
			Analog_gain = gpdata[5]*1000/128;
		}
		else if(gpdata[2] == 1){
			Analog_gain = (gpdata[5]+8)*1000/64;
		}
		else if(gpdata[2] == 3){
			Analog_gain = (gpdata[5]+12)*1000/32;
		}
		else if(gpdata[2] == 7){
			Analog_gain = (gpdata[5]+8)*1000/16;
		}
			
		#if HDR_debug
		profLogPrintf(3,"@#@ AnalogGain:%d",Analog_gain);
		#endif

		UINT16 udwAgcEvUnit;
		udwAgcEvUnit = g_ucAgcTblStepsPerEV;
		/*	
		if(Analog_gain >= 1000 && Analog_gain < 2000){
			Agc_idx = (Analog_gain-1000)/((2000-1000)/udwAgcEvUnit);
		}
		else if(Analog_gain >= 2000 && Analog_gain < 4000){
			Agc_idx = (Analog_gain-2000)/((4000-2000)/udwAgcEvUnit)+(udwAgcEvUnit*1);
		}
		else if(Analog_gain >= 4000 && Analog_gain < 8000){
			Agc_idx = (Analog_gain-4000)/((8000-4000)/udwAgcEvUnit)+(udwAgcEvUnit*2);
		}
		else if(Analog_gain >= 8000 && Analog_gain < 16000){
			Agc_idx = (Analog_gain-8000)/((1600-8000)/udwAgcEvUnit)+(udwAgcEvUnit*3);
		}

		profLogPrintf(3,"@#@1. Agc_idx:%d",Agc_idx);
		*/

		extern UINT32 ae_preview_agc_get();
		extern UINT32 ae_snap_agc_get();
		if ((ucSensorMode&0xf0)==0x30)
			Agc_idx = ae_preview_agc_get();
		else
			Agc_idx = ae_snap_agc_get();
			
		#if HDR_debug
		profLogPrintf(3,"@#@2. Agc_idx:%d",Agc_idx);
		#endif
			
		ev1_entry = Agc_idx-(EV_diff_1*(udwAgcEvUnit/16));/*If use 1/64ev table, must take ev unit into consideration.*/
		if(ev1_entry<0)
			ev1_entry = 0;
		Analog_gain = sensor_gain_cal(ev1_entry);
		gain_reg[0] = sensor_gain_MSBreg_cal(Analog_gain);
		gain_reg[1] = sensor_gain_LSBreg_cal(Analog_gain);
		if(EV_diff_1 == 0){

			g_ucMiddleAgcData[0][2]= gpdata[2];
			g_ucMiddleAgcData[1][2]= gpdata[5];

			/*
			sensorOVWrite(0x350E,gpdata[2]);
			sensorOVWrite(0x350F,gpdata[5]);
			*/
			#if 0
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,0x350E,&pdata[2]);
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,0x350E,&pdata[5]);
			#endif
		}
		else{
			g_ucMiddleAgcData[0][2]= gain_reg[0];
			g_ucMiddleAgcData[1][2]= gain_reg[1];

			/*
			sensorOVWrite(0x350E,gain_reg[0]);
			sensorOVWrite(0x350F,gain_reg[1]);
			*/
			#if 0
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,0x350E,&gain_reg[0]);
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,0x350E,&gain_reg[1]);
			#endif
		}
		#if HDR_debug
		profLogPrintf(3,"@#@ EV_diff_1: %d, Analog_gain1 :%d %d %d",EV_diff_1,Analog_gain,gain_reg[0],gain_reg[1]);
		#endif
			
		ev2_entry = Agc_idx-(EV_diff_2*(udwAgcEvUnit/16));
		if(ev2_entry<0)
			ev2_entry = 0;
		Analog_gain = sensor_gain_cal(ev2_entry);
		gain_reg[0] = sensor_gain_MSBreg_cal(Analog_gain);
		gain_reg[1] = sensor_gain_LSBreg_cal(Analog_gain);
		if(EV_diff_2 == 0){

			g_ucShortAgcData[0][2]= gpdata[2];
			g_ucShortAgcData[1][2]= gpdata[5];
			/*
			sensorOVWrite(0x3514,gpdata[2]);
			sensorOVWrite(0x3515,gpdata[5]);
			*/
			#if 0
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,0x3514,&pdata[2]);
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,0x3515,&pdata[5]);
			#endif
		}
		else{
			g_ucShortAgcData[0][2]= gain_reg[0];
			g_ucShortAgcData[1][2]= gain_reg[1];
			/*
			sensorOVWrite(0x3514,gain_reg[0]);
			sensorOVWrite(0x3515,gain_reg[1]);
			*/
			#if 0
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,0x3514,&gain_reg[0]);
			sensorSerialAsyncWrite(sensorAfeCfg.cmdportid_agc,0x3515,&gain_reg[1]);
			#endif
		}
		#if HDR_debug
		profLogPrintf(3,"@#@ EV_Diff_2: %d, Analog_gain2 :%d %d %d",EV_diff_2,Analog_gain,gain_reg[0],gain_reg[1]);	
		#endif
	}
}

