/**************************************************************************
 *
 *       Copyright (c) 2006-2018 by iCatch Technology, Inc.
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
 **************************************************************************/
/*reference code, a simple demo only*/
#include <stdio.h>
#include "common.h"
#include "customization/dev_init.h"
#include "middleware/sensor_def.h"
#include "middleware/sensor_model.h"
#include "middleware/sensor_model_snap.h"
#include "middleware/proflog.h"

#include "api/sp5k_global_api.h"
#include "api/sp5k_ae_api.h"
#include "../../customization/arch/spca6350/drivers/sensor/flow/sensor_custom.h"


#undef PROF_LOG_ADD
#undef PROF_LOG_PRINTF
#if 1
#define PROF_LOG_ADD(...)
#define PROF_LOG_PRINTF(...)
#else
#define PROF_LOG_ADD(...)		profLogAdd(...)
#define PROF_LOG_PRINTF(...)	profLogPrintf(...)
#endif

#define SEN_IO_DBG  (0)

#if SEN_IO_DBG
#define PROF_LOG_SEN(str,args...) \
    do{\
        profLogPrintf(3, str, ##args);\
    }while(0)
#else
#define PROF_LOG_SEN(str,args...)
#endif

#define NO_USE   	            0x7FFFFFFF

extern void appSensorIoCfg(iopin_t *pwrio, iopin_t *devio);
extern ioseq_t* appSensorSeqCfg(UINT32 id);

#ifdef ENABLE_OIS_CONTROL
    extern void VR_SensorDriverCB_Start( void);
    extern void VR_SensorDriverCB_CloseShutter(void);
#endif

static sensorCustomCb_t *g_psenCustomCb=NULL;

#ifdef SP5K_SENSOR_PIN_CB
#if SP5K_SENSOR_POWERIC_ENABLE

typedef struct {
	UINT32 pwrIcReg;
	UINT32 msk;
	UINT32 val;
	UINT16 delayms;
} pwrIcCmdseq_t;

#if 1 /* for Example for sensor power IC control (G2207) */
static pwrIcCmdseq_t cmosPwrIcSysOn[]     ={{NO_USE}};
static pwrIcCmdseq_t cmosPwrIcPwrAnaOn[]  ={{0x0005, BIT2, BIT2, 0}};
static pwrIcCmdseq_t cmosPwrIcPwrIoOn[]   ={{0x0005, BIT1, BIT1, 0}};
static pwrIcCmdseq_t cmosPwrIcPwrCoreOn[] ={{0x0005, BIT0, BIT0, 0}};
static pwrIcCmdseq_t cmosPwrIcRstOn[]     ={{NO_USE}};
static pwrIcCmdseq_t cmosPwrIcPwdnOn[]    ={{NO_USE}};

static pwrIcCmdseq_t cmosPwrIcPwdnOff[]   ={{NO_USE}};
static pwrIcCmdseq_t cmosPwrIcRstOff[]    ={{NO_USE}};
static pwrIcCmdseq_t cmosPwrIcPwrCoreOff[]={{0x0005, BIT0, BIT0, 0}};
static pwrIcCmdseq_t cmosPwrIcPwrIoOff[]  ={{0x0005, BIT1, 0, 0}};
static pwrIcCmdseq_t cmosPwrIcPwrAnaOff[] ={{0x0005, BIT2, 0, 0}};
static pwrIcCmdseq_t cmosPwrIcSysOff[]    ={{NO_USE}};
#endif

static struct {
	UINT32 id;
	pwrIcCmdseq_t *seq;
} cmosPwrIcSeq[12]={
	{ SENSOR_DEV_POWER_ID_CMOS_SYS_ON,      cmosPwrIcSysOn      },
	{ SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_ON,  cmosPwrIcPwrAnaOn   },
	{ SENSOR_DEV_POWER_ID_CMOS_PWR_IO_ON,   cmosPwrIcPwrIoOn    },
	{ SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_ON, cmosPwrIcPwrCoreOn  },
    { SENSOR_DEV_POWER_ID_CMOS_RST_ON,      cmosPwrIcRstOn      },
    { SENSOR_DEV_POWER_ID_CMOS_PWDN_ON,     cmosPwrIcPwdnOn     },
	{ SENSOR_DEV_POWER_ID_CMOS_PWDN_OFF,    cmosPwrIcPwdnOff    },
	{ SENSOR_DEV_POWER_ID_CMOS_RST_OFF,     cmosPwrIcRstOff     },
	{ SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_OFF,cmosPwrIcPwrCoreOff },
	{ SENSOR_DEV_POWER_ID_CMOS_PWR_IO_OFF,  cmosPwrIcPwrIoOff   },
    { SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_OFF, cmosPwrIcPwrAnaOff  },
	{ SENSOR_DEV_POWER_ID_CMOS_SYS_OFF,     cmosPwrIcSysOff     },
};

extern UINT32 appI2CWrite(UINT32 par0, UINT32 mask, UINT32 par1);

static UINT32
appSensorPowerICCtrl(
    UINT32 id
)
{
    UINT32 ret = FAIL;
	UINT16 i;
    pwrIcCmdseq_t *pCmdseq_t;

    PROF_LOG_SEN("    SenPowerIcCtrl ID:0x%x",id);

	for (i=0 ; i<12 ; i++)
	{
	        if( cmosPwrIcSeq[i].id == id  )
		{
	            pCmdseq_t = cmosPwrIcSeq[i].seq;
	        	while( (pCmdseq_t->pwrIcReg) != NO_USE)
			{
	                PROF_LOG_SEN("    I2C Wrt Addr:0x%x, mask:0x%x, value:0x%x", pCmdseq_t->pwrIcReg,
	                                                                             pCmdseq_t->msk,
	                                                                             pCmdseq_t->val );

	                appI2CWrite(pCmdseq_t->pwrIcReg,
	                            pCmdseq_t->msk,
	                            pCmdseq_t->val );

	        		if (pCmdseq_t->delayms) sp5kTimeDelay(SP5K_TIME_DELAY_1MS, pCmdseq_t->delayms);

	                pCmdseq_t++;
	            	}
		}
	}
	return ret;
}
#else
#define appSensorPowerICCtrl(id)
#endif

static iopin_t pwrio[10];/*CMOS_PWDN_2ND+1*/
static iopin_t devio[(SENSOR_IO_EXTEND+1)];

static UINT32
sensorDevCmosPowerSequence(
	UINT32 id
)
{

	UINT8 i;
    ioseq_t *seq;
	if (pwrio[0].msk==0)
		appSensorIoCfg(pwrio, devio);

	seq = appSensorSeqCfg(id);
	while ((i=seq->ioidx)!=0xff) {
		if(pwrio[i].grp!=SP5K_GPIO_GRP_NO_USE){
        	sp5kGpioWrite(pwrio[i].grp, pwrio[i].msk, (seq->pol?pwrio[i].msk:0) );
			if (seq->delayms) sp5kTimeDelay(SP5K_TIME_DELAY_1MS, seq->delayms);
		}
		seq++;
	}

    appSensorPowerICCtrl(id);
	return SUCCESS;
}
#endif

UINT32 sensorDevIoCtrl(
	sensorIoCtrl_t iopin,
	UINT32 opt
)
{
	if (iopin==SENSOR_IO_EXTEND) {

	}else{
#ifdef SP5K_SENSOR_PIN_CB
		
		/* printf("%d:%d %x %x\n", iopin, opt, devio[iopin].grp, devio[iopin].msk); */
		if(devio[iopin].grp!=SP5K_GPIO_GRP_NO_USE)
		{
			static UINT32 devioInit[SENSOR_IO_EXTEND+1];
			sp5kGpioWrite(devio[iopin].grp, devio[iopin].msk, opt?devio[iopin].msk:0);

			if (!devioInit[iopin]) {
				sp5kGpioCfgSet(devio[iopin].grp, devio[iopin].msk, devio[iopin].msk);
				#if 0
				sp5kGpioFuncSet(devio[iopin].grp, devio[iopin].msk, devio[iopin].msk);
				#endif
				devioInit[iopin] = 1;
			}
		}
#endif
	}
	return SUCCESS;
}

UINT32
sensorDevPowerCustomCb(
	UINT32 id
)
{
#ifdef SP5K_SENSOR_PIN_CB
	UINT32 ret;

	ret = sensorDevCmosPowerSequence(id);

	return ret;
#else
/*V35 EVB default */
UINT8 pwrid=id&0xFF;
UINT8 sel=(id&SENSOR_DEV_POWER_ID_CMOS_SEC_SEL)?1:0;/*0: primary, 1: secondary*/

switch (pwrid) {
	default: break;
	case SENSOR_DEV_POWER_ID_SEN_ON:
	case SENSOR_DEV_POWER_ID_CMOS_SYS_ON:

#if ((SP5K_SENSOR_SONYIMX118)||(SP5K_SENSOR_SONYIMX147))
	#define SENSOR_RESET	(1<<4)
		sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL, SENSOR_RESET, SENSOR_RESET);
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, 0); /* RESET LOW*/
		tmrUsWait(5000); /*5ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, SENSOR_RESET); /* RESET High*/

#elif SP5K_SENSOR_SONYIMX175
	#define SENSOR_RESET	(1<<4)|(1<<6) /*Dual sensor 4,6*/
		sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET);
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,0);
		tmrUsWait(100000); /*10ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET);
		tmrUsWait(100000); /*10ms*/

#elif (SP5K_SENSOR_SONYIMX206) ||(SP5K_SENSOR_DUAL_SONYIMX206)
	#define SENSOR_RESET	(1<<4)
		sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL, SENSOR_RESET, SENSOR_RESET);
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, 0); /* RESET LOW*/
		tmrUsWait(5000); /*5ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SENSOR_RESET, SENSOR_RESET); /* RESET High*/

#elif (SP5K_SENSOR_AR0330CS)||(SP5K_SENSOR_AR0330)||(SP5K_SENSOR_AR0230)||(SP5K_SENSOR_AR0330_SPCA7002)
	#define SENSOR_RESET	(1<<5) /*Dual sensor 4,6*/
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET);
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /* RESET High*/
	tmrUsWait(35000); /*35ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,0); /* RESET LOW*/
	tmrUsWait(5000); /*5ms*/
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET); /* RESET High*/

#elif (SP5K_SENSOR_OV4689)|| (SP5K_SENSOR_OV4689_SPCA7002)
	#define SENSOR_RESET	(1<<4)
		sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET);
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,0);
		tmrUsWait(100000); /*10ms*/
		sp5kGpioWrite(SP5K_GPIO_GRP_TGL,SENSOR_RESET,SENSOR_RESET);
		tmrUsWait(100000); /*10ms*/
#else

#endif
		break;

	case SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_ON:
		break;
	case SENSOR_DEV_POWER_ID_CMOS_PWR_IO_ON:
		break;
	case SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_ON:
		break;
	case SENSOR_DEV_POWER_ID_CMOS_RST_OFF:
		break;
	case SENSOR_DEV_POWER_ID_CMOS_RST_ON:
		break;
	}
	return SUCCESS;
#endif
}

void sensorSnapExpBeginVdCB(UINT32 param){
	/*the latest VD before snap*/
#ifdef ENABLE_OIS_CONTROL
	PROF_LOG_ADD(0,"sensorSnapExpBeginVdCB BGN");
	VR_SensorDriverCB_Start();
	PROF_LOG_ADD(0,"sensorSnapExpBeginVdCB END");
#endif
}

void sensorSnapExpEndVdCB(UINT32 param){
	/*the 1'st VD after exposure finish*/
#ifdef ENABLE_OIS_CONTROL
	PROF_LOG_ADD(0,"sensorSnapExpEndVdCB BGN");
	VR_SensorDriverCB_CloseShutter();
	PROF_LOG_ADD(0,"sensorSnapExpEndVdCB END");
#endif
}

void sensorSnap_4PIX_ExpEndVdCB(UINT32 param){
	/*the 1'st VD after exposure finish*/
#ifdef ENABLE_OIS_CONTROL
	VR_SensorDriverCB_CloseShutter();
#endif
}

void senRegWR(UINT16 reg, UINT16 val){
#if SP5K_SENSOR_SONYIMX118 || SP5K_SENSOR_SONYIMX206 || SP5K_SENSOR_DUAL_SONYIMX206||SP5K_SENSOR_SONYIMX078
    /* SONYIMX-SPI(Reg16, Dat8) */
	static UINT8 regdata[4];
	regdata[0] = 0x81;      /* Chip ID */
	regdata[1] = reg>>8;
	regdata[2] = reg&0xFF;
	regdata[3] = val&0xFF;
	sensorSerialDirectBufSnoop(regdata, sizeof(regdata));
	sensorSerialBulkWrite(0,1,regdata,0);
    printf("[SONY][SPI] WR[0x%x]=[0x%x]\n", reg, val);
#elif SP5K_SENSOR_SONYIMX175
    /* SONYIMX-I2C(Reg16, Dat8) */
	static UINT8 regdata[3];
	regdata[0] = reg>>8;
	regdata[1] = reg&0xFF;
	regdata[2] = val&0xFF;
	sensorSerialBulkWrite(0,1,regdata,0);
	printf("[SONY][I2C] WR[0x%x]=[0x%x]\n", reg, val);
#else

#endif
}

void
sensorCustomCbInstall(sensorCustomCb_t *psenCustomCb)
{
	g_psenCustomCb = psenCustomCb;
}

/*For setting 360 cam offset calibration value.*/

/*
a_uwHsize : H size value.
a_uwVsize : V size value.
*/
void
sensorCustomCropSizeCfgSet(UINT16 a_uwHsize, SINT16 a_uwVsize)
{
	if (g_psenCustomCb==NULL)
		return;

	if (g_psenCustomCb->fp_customCropSizeCfgCb!=NULL)
		g_psenCustomCb->fp_customCropSizeCfgCb(a_uwHsize,a_uwVsize);
}


/*a_ucSenSel: 0=>master, 1=>slave
a_swHOffset : H offset value.
a_swVOffset : V offset value.
*/
void
sensorCustomOffsetCfgSet(UINT8 a_ucSenSel, SINT16 a_swHoffset, SINT16 a_swVoffset)
{
	if (g_psenCustomCb==NULL)
		return;

	if (g_psenCustomCb->fp_customOffsetCfgCb!=NULL)
		g_psenCustomCb->fp_customOffsetCfgCb(a_ucSenSel,a_swHoffset,a_swVoffset);
}


/*a_ucSenSel: 0=>master, 1=>slave
a_swHOffset : H offset value.
a_swVOffset : V offset value.
*/
void
sensorCustomFullSizeOffsetCfgSet(UINT8 a_ucSenSel, SINT16 a_swHoffset, SINT16 a_swVoffset)
{
	if (g_psenCustomCb==NULL)
		return;

	if (g_psenCustomCb->fp_customFullSizeOffsetCfgCb!=NULL)
		g_psenCustomCb->fp_customFullSizeOffsetCfgCb(a_ucSenSel,a_swHoffset,a_swVoffset);
}


/*a_ucSenSel: 0=>master, 1=>slave
a_swHOffset : H offset value.
a_swVOffset : V offset value.
*/
void
sensorCustomOffsetPvChk(UINT8 a_ucSenSel, SINT16 a_swHoffset, SINT16 a_swVoffset)
{
	if (g_psenCustomCb==NULL)
		return;

	if (g_psenCustomCb->fp_customOffsetPvChkCb!=NULL)
		g_psenCustomCb->fp_customOffsetPvChkCb(a_ucSenSel,a_swHoffset,a_swVoffset);
}

/*
a_ucSenSel :  0:master sensor, 1:slave sensor
a_udwRGain, a_udwBGain, a_udwGrGain, a_udwGbGain: (1024 is 1x, 2048 is 2x...)
*/
void sensorCustomSenAWBGainCfg(UINT8 a_ucSenSel, UINT32 a_udwRGain, UINT32 a_udwBGain,UINT32 a_udwGrGain, UINT32 a_udwGbGain)
{
	if (g_psenCustomCb==NULL)
		return;

	if (g_psenCustomCb->fp_customWbgainRegCfgCb!=NULL)
		g_psenCustomCb->fp_customWbgainRegCfgCb(a_ucSenSel,a_udwRGain,a_udwBGain,a_udwGrGain,a_udwGbGain);
}

/*
Following function is reserved to be compatiable with the old host.
*/
void sensorDualSenAWBGainCfg(UINT8 a_ucSenSel, UINT32 a_udwRGain, UINT32 a_udwBGain,UINT32 a_udwGrGain, UINT32 a_udwGbGain)
{
	sensorCustomSenAWBGainCfg(a_ucSenSel,a_udwRGain,a_udwBGain,a_udwGrGain,a_udwGbGain);
}

/*Return 0 means implement WB gain in sensor side.*/
/*Return 1 means implement WB gain in CDSP side. This means the sensor did not support separate channel gain.*/
/*Return 0xffffffff means the callback is not registered.*/
UINT32 sensorCustomSenAWBGainGet(UINT8 a_ucSenSel, UINT32 *a_udwRGain, UINT32 *a_udwBGain,UINT32 *a_udwGrGain, UINT32 *a_udwGbGain)
{
	UINT32 ret=0xffffffff;
	if (g_psenCustomCb==NULL)
		return ret;/*paul 2018.04.13 modify 'return' with no value*/
	
	if (g_psenCustomCb->fp_customWbGainGetCb!=NULL)
		ret=g_psenCustomCb->fp_customWbGainGetCb(a_ucSenSel,a_udwRGain,a_udwBGain,a_udwGrGain,a_udwGbGain);

	return ret;
}

/*
a_udwRGain, a_udwBGain, a_udwGrGain, a_udwGbGain: (1024 is 1x, 2048 is 2x...)
*/
void sensorCustomHdrSenWBGainCfg(UINT32 a_udwRGain, UINT32 a_udwBGain,UINT32 a_udwGrGain, UINT32 a_udwGbGain)
{
	if (g_psenCustomCb==NULL)
		return;

	if (g_psenCustomCb->fp_customHdrSenWbgainRegCfgCb!=NULL)
		g_psenCustomCb->fp_customHdrSenWbgainRegCfgCb(a_udwRGain,a_udwBGain,a_udwGrGain,a_udwGbGain);
}

/*Return 0 means the getting result (WB gain) in sensor side is SUCCESS.*/
/*Return 0xffffffff means the callback is not registered.*/
UINT32 sensorCustomHdrSenWBGainGet(UINT32 *a_udwRGain, UINT32 *a_udwBGain,UINT32 *a_udwGrGain, UINT32 *a_udwGbGain)
{
	UINT32 ret=0xffffffff;
	if (g_psenCustomCb==NULL)
		return ret;/*paul 2018.04.13 modify 'return' with no value*/
	
	if (g_psenCustomCb->fp_customHdrSenWbGainGetCb!=NULL)
		ret=g_psenCustomCb->fp_customHdrSenWbGainGetCb(a_udwRGain,a_udwBGain,a_udwGrGain,a_udwGbGain);

	return ret;
}

#if 1
static SINT32 presetExp=80;/*initial value*/
static UINT32 presetAgc=80;/*initial value*/

/*Host have to config exp/agc before sensor init ...*/
void
sensorExpAgcPreCfgSet(SINT32 exp,UINT32 agc)
{
	presetExp=exp;
	presetAgc=agc;
	profLogPrintf(0,"sensorExpAgcPreCfg[%d %d]",presetExp,presetAgc);
}

void
sensorExpAgcPreCfgGet(SINT32 *exp,UINT32 *agc)
{
	*exp=presetExp;
	*agc=presetAgc;
}

void 
sensorExpAgcPreSet(UINT32 smode, SINT32 expidx, UINT32 agcidx)
{
	sp5kExpSts_t *pexpsts;
	sp5kAgcSts_t *pagcsts;
	SINT32 exp=expidx;
	UINT32 agc=agcidx;
	UINT32 nreg;
	UINT8 *expdata;
	UINT8 *agcdata;
	UINT32 udwSensorMode;
	sensorCmdport_t *t_SensorCmdPort;
	UINT8 ucOneCmdByteCnt;
	UINT32 cmdportid;

	extern UINT32 sensorModeGet();
	extern sensorCmdport_t *psensorCmdportGet(UINT32 cmdportid);

	cmdportid = sensorCmdportIdGet(SENSOR_CMDPORTID_SEL_XTG);
	t_SensorCmdPort = psensorCmdportGet(cmdportid);

	ucOneCmdByteCnt = (t_SensorCmdPort->nbit_regaddr+t_SensorCmdPort->nbit_regdata)/8;

	if (smode==0)/*Pass 0, use sensorModeGet to get sensor mode number.*/
	{
		udwSensorMode=sensorModeGet();
	}
	else
	{
		udwSensorMode=smode;
	}
	
	pexpsts=(sp5kExpSts_t*)sp5kAeExpStsGet((UINT32)exp,udwSensorMode,NULL);
	expdata=&pexpsts->burstrenderbufitem[0];
	nreg = pexpsts->nbyte /ucOneCmdByteCnt;

	sensorSerialDirectBufSnoop(expdata, pexpsts->nbyte);

	extern const sensorTgDev_t *psensorTgDevGet();
	const sensorTgDev_t *t_CurrentTgDev;

	t_CurrentTgDev = psensorTgDevGet();
	t_CurrentTgDev->tgSvc->expwrite(nreg,expdata);
	sensorSerialFlush(cmdportid);

	#if 1
	if (t_CurrentTgDev->tgSvc->frmwrite != NULL)
	{
		t_CurrentTgDev->tgSvc->frmwrite(smode,0,0,nreg,expdata);
		sensorSerialFlush(cmdportid);
	}		
	#endif

	pagcsts=(sp5kAgcSts_t*)sp5kAeAgcStsGet((UINT32)agc,udwSensorMode,NULL);
	agcdata=&pagcsts->burstrenderbufitem[0];		
	nreg = pagcsts->nbyte /ucOneCmdByteCnt;

	sensorSerialDirectBufSnoop(agcdata, pagcsts->nbyte);

	extern const sensorAfeDev_t *psensorAfeDevGet();
	const sensorAfeDev_t *t_CurrentAfeDev;
	t_CurrentAfeDev = psensorAfeDevGet();
	t_CurrentAfeDev->afeSvc->agcwrite(nreg,agcdata);

	sensorSerialFlush(cmdportid);
}
#endif
