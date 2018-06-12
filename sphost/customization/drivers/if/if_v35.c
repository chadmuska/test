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

#define HOST_DBG 0
#include "common.h"
#include <stdio.h>
#include <string.h>
#include "sp5k_disk_api.h"
#include "sp5k_utility_api.h"
#include "sp5k_dbg_api.h"
#include "sp5k_global_api.h"
#include "sp5k_os_api.h"
#include "middleware/sensor_model.h"

#include "customization/dev_init.h"
#include GPIO_INC

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define GPIO_CFG_IN(PinNo)  (0x00000000L << (PinNo))
#define GPIO_CFG_OUT(PinNo) (0x00000001L << (PinNo))
#define GPIO_MASK(PinNo)    (0x00000001L << (PinNo))

#define GPIO_PIN_VAL_LO(PinNo) (0x00000000L << (PinNo))
#define GPIO_PIN_VAL_HI(PinNo) (0x00000001L << (PinNo))

#ifndef SENSOR_DEV_POWER_ID_CMOS_SEC_SEL
#define SENSOR_DEV_POWER_ID_CMOS_SEC_SEL 0x8000
#endif
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define WRITE8(a,b)	*(volatile UINT8*)(a) = (b)

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/**************************************************************************
 * Function Name: gpioPinLevelGet
 **************************************************************************/
UINT32
gpioPinLevelGet(
	UINT32 ioGrp,
	UINT32 ioMask
)
{
	UINT32 ioPinLevel;
	sp5kGpioRead(ioGrp, ioMask, &ioPinLevel);
	return (!!ioPinLevel);
}

void SetPowerBacklight(UINT32 en)
{
#ifdef SET_PWR_BACKLIGHT
	SET_PWR_BACKLIGHT(en);
#endif
}

void SetPowerBacklight2(UINT32 en)
{
#ifdef SET_PWR_BACKLIGHT2
	SET_PWR_BACKLIGHT2(en);
#endif
}

UINT32 GetPowerBacklight(void)
{
#ifdef OUT_PWR_BACKLIGHT
	return OUT_PWR_BACKLIGHT;
#endif
}

UINT32 GetPowerBacklight2(void)
{
#ifdef OUT_PWR_BACKLIGHT2
	return OUT_PWR_BACKLIGHT2;
#endif
}

UINT32 dummyioLcdBL;

/**************************************************************************
 * Function Name: gpioInitialCfgSet
 * Purpose : To configure all camera hw IOs at the camera power up time.
 **************************************************************************/
void
gpioInitialCfgSet(
	void
)
{
	/***** Initial H/L *************************************************************/
	sp5kGpioWrite(SP5K_GPIO_GRP_GEN, SPCA_GEN_GPIO_INIT_MASK, SPCA_GEN_GPIO_INIT_VAL);
	sp5kGpioWrite(SP5K_GPIO_GRP_TGL, SPCA_TGL_GPIO_INIT_MASK, SPCA_TGL_GPIO_INIT_VAL );
	sp5kGpioWrite(SP5K_GPIO_GRP_DISP, SPCA_DISP_GPIO_INIT_MASK, SPCA_DISP_GPIO_INIT_VAL);
	sp5kGpioWrite(SP5K_GPIO_GRP_FML, SPCA_FML_GPIO_INIT_MASK, SPCA_FML_GPIO_INIT_VAL );
	sp5kGpioWrite(SP5K_GPIO_GRP_FMH, SPCA_FMH_GPIO_INIT_MASK, SPCA_FMH_GPIO_INIT_VAL );
	sp5kGpioWrite(SP5K_GPIO_GRP_LMI, SPCA_LMI_GPIO_INIT_MASK, SPCA_LMI_GPIO_INIT_VAL );

	/***** Configuration *************************************************************/
	sp5kGpioCfgSet(SP5K_GPIO_GRP_GEN,  SPCA_GEN_GPIO_MASK, SPCA_GEN_GPIO_CFGIO  );
	sp5kGpioCfgSet(SP5K_GPIO_GRP_TGL,  SPCA_TGL_GPIO_MASK, SPCA_TGL_GPIO_CFGIO  );
	sp5kGpioCfgSet(SP5K_GPIO_GRP_DISP, SPCA_DISP_GPIO_MASK, SPCA_DISP_GPIO_CFGIO );
	sp5kGpioCfgSet(SP5K_GPIO_GRP_FML,  SPCA_FML_GPIO_MASK, SPCA_FML_GPIO_CFGIO  );
	sp5kGpioCfgSet(SP5K_GPIO_GRP_FMH,  SPCA_FMH_GPIO_MASK, SPCA_FMH_GPIO_CFGIO  );
	sp5kGpioCfgSet(SP5K_GPIO_GRP_LMI,  SPCA_LMI_GPIO_MASK, SPCA_LMI_GPIO_CFGIO  );

	/***** Define the pull setting here **********************************************/
	#ifdef SP5K_GPIO_GEN_PULL
		sp5kGpioPullSet(SP5K_GPIO_GRP_GEN, SP5K_GPIO_GEN_PULL_MASK, SP5K_GPIO_GEN_PULL);
	#endif
	#ifdef SP5K_GPIO_TGL_PULL
		sp5kGpioPullSet(SP5K_GPIO_GRP_TGL, SP5K_GPIO_TGL_PULL_MASK, SP5K_GPIO_TGL_PULL);
	#endif
	#ifdef SP5K_GPIO_DISP_PULL
		sp5kGpioPullSet(SP5K_GPIO_GRP_DISP, SP5K_GPIO_DISP_PULL_MASK, SP5K_GPIO_DISP_PULL);
	#endif
	#ifdef SP5K_GPIO_LMI_PULL
		sp5kGpioPullSet(SP5K_GPIO_GRP_LMI, SP5K_GPIO_LMI_PULL_MASK, SP5K_GPIO_LMI_PULL);
	#endif
	#ifdef SP5K_GPIO_FML_PULL
		sp5kGpioPullSet(SP5K_GPIO_GRP_FML, SP5K_GPIO_FML_PULL_MASK, SP5K_GPIO_FML_PULL);
	#endif
	#ifdef SP5K_GPIO_FMH_PULL
		sp5kGpioPullSet(SP5K_GPIO_GRP_FMH, SP5K_GPIO_FMH_PULL_MASK, SP5K_GPIO_FMH_PULL);
	#endif
}

UINT32
gpioMatrixPinLevelGet( UINT32 grpWr, UINT32 pinWr, UINT32 grpRd, UINT32 pinRd, UINT8 polar ) {
	UINT32 val;
	UINT32 maskWr, maskRd;

	maskWr = 1 << pinWr;
	sp5kGpioWrite( grpWr, maskWr, maskWr );
	maskRd = 1 << pinRd;
	sp5kGpioRead( grpRd, maskRd, &val );
	val = val >> pinRd;
	val ^=( !polar );

	return val;
}

void appDiskIoCfg(iopin_t *io)
{
	memset(io, 0, 4*sizeof(iopin_t));
	io[SP5K_DISK_DETECT_PIN].grp = GRP_SD_CARD_DETECT;
	io[SP5K_DISK_DETECT_PIN].id = CFG_SD_CARD_DETECT;
	io[SP5K_DISK_DETECT_PIN].pol = POL_SD_CARD_DETECT;
	#if GRP_SD_CARD_POWER!=SP5K_GPIO_GRP_NO_USE
	io[SP5K_DISK_POWER_ON_PIN].grp = GRP_SD_CARD_POWER;
	io[SP5K_DISK_POWER_ON_PIN].id = CFG_SD_CARD_POWER;
	io[SP5K_DISK_POWER_ON_PIN].pol = SP5K_GPIO_ACTIVE_HIGH;
	#endif
	io[SP5K_DISK_WRITE_PROTECT_PIN].grp = GRP_SD_CARD_WP;
	io[SP5K_DISK_WRITE_PROTECT_PIN].id = CFG_SD_CARD_WP;
	io[SP5K_DISK_WRITE_PROTECT_PIN].pol = POL_SD_CARD_WP;
}

void appDispIoCfg(iopin_t *io)
{
	io[SPI_CS2].grp = GRP_LCD2_SPI_CS;  /* CS2 */
	io[SPI_CS2].id  = CFG_LCD2_SPI_CS;
	io[SPI_CS2].msk = 1<<CFG_LCD2_SPI_CS;
	io[SPI_CS ].grp = GRP_LCD_SPI_CS;  /* CS */
	io[SPI_CS ].id  = CFG_LCD_SPI_CS;
	io[SPI_CS ].msk = 1<<CFG_LCD_SPI_CS;
	io[SPI_SCL].grp = GRP_LCD_SCLK;  /* CLK */
	io[SPI_SCL].id  = CFG_LCD_SCLK;
	io[SPI_SCL].msk = 1<<CFG_LCD_SCLK;
	io[SPI_SDA].grp = GRP_LCD_SDATA;  /* DATA */
	io[SPI_SDA].id  = CFG_LCD_SDATA;
	io[SPI_SDA].msk = 1<<CFG_LCD_SDATA;
}

void appUsbIoCfg( iopin_t *io ) {
	io[0].grp = GRP_USB_DETECT;
	io[0].id = CFG_USB_DETECT;
	io[0].pol = !POL_USB_DETECT;
}



/* CMOS_PWR_ANA is about 3.5v~3.3v or 2.8~2.7v */
/* CMOS_PWR_IO   is about 1.8v */
/* CMOS_PWR_CORE is 1.2v */
/* CMOS_PWDN is for OV spec */
enum {  CMOS_PWR_ANA,
        CMOS_PWR_IO,
        CMOS_PWR_CORE,
        CMOS_RESET,
        CMOS_PWDN,
        CMOS_PWR_ANA_2ND,
        CMOS_PWR_IO_2ND,
        CMOS_PWR_CORE_2ND,
        CMOS_RESET_2ND,
        CMOS_PWDN_2ND};

void appSensorIoCfg(iopin_t *pwrio, iopin_t *devio)
{
/*pwrio*/
#ifdef GRP_CMOS_PWR_ANA
	pwrio[CMOS_PWR_ANA].grp  = GRP_CMOS_PWR_ANA;
	pwrio[CMOS_PWR_ANA].msk  = 1<<CFG_CMOS_PWR_ANA;
#endif
#ifdef GRP_CMOS_PWR_IO
	pwrio[CMOS_PWR_IO].grp   = GRP_CMOS_PWR_IO;
	pwrio[CMOS_PWR_IO].msk   = 1<<CFG_CMOS_PWR_IO;
#endif
#ifdef GRP_CMOS_PWR_CORE
	pwrio[CMOS_PWR_CORE].grp = GRP_CMOS_PWR_CORE;
	pwrio[CMOS_PWR_CORE].msk = 1<<CFG_CMOS_PWR_CORE;
#endif
#ifdef GRP_CMOS_RESET
	pwrio[CMOS_RESET].grp    = GRP_CMOS_RESET;
	pwrio[CMOS_RESET].msk    = 1<<CFG_CMOS_RESET;
	devio[SENSOR_IO_RESET].grp	= GRP_CMOS_RESET;
	devio[SENSOR_IO_RESET].msk	= 1<<CFG_CMOS_RESET;
#endif
#ifdef GRP_CMOS_PWDN
	pwrio[CMOS_PWDN].grp     = GRP_CMOS_PWDN;
	pwrio[CMOS_PWDN].msk     = 1<<CFG_CMOS_PWDN;
	devio[SENSOR_IO_PWDN].grp	= GRP_CMOS_RESET;
	devio[SENSOR_IO_PWDN].msk	= 1<<CFG_CMOS_RESET;
#endif

#ifdef GRP_CMOS_PWR_ANA_2ND
	pwrio[CMOS_PWR_ANA_2ND].grp  = GRP_CMOS_PWR_ANA_2ND;
	pwrio[CMOS_PWR_ANA_2ND].msk  = 1<<CFG_CMOS_PWR_ANA_2ND;
#endif
#ifdef GRP_CMOS_PWR_IO_2ND
	pwrio[CMOS_PWR_IO_2ND].grp   = GRP_CMOS_PWR_IO_2ND;
	pwrio[CMOS_PWR_IO_2ND].msk   = 1<<CFG_CMOS_PWR_IO_2ND;
#endif
#ifdef GRP_CMOS_PWR_CORE_2ND
	pwrio[CMOS_PWR_CORE_2ND].grp = GRP_CMOS_PWR_CORE_2ND;
	pwrio[CMOS_PWR_CORE_2ND].msk = 1<<CFG_CMOS_PWR_CORE_2ND;
#endif
#ifdef GRP_CMOS_RESET_2ND
	pwrio[CMOS_RESET_2ND].grp    	= GRP_CMOS_RESET_2ND;
	pwrio[CMOS_RESET_2ND].msk    	= 1<<CFG_CMOS_RESET_2ND;
	devio[SENSOR_IO_RESET_2ND].grp	= GRP_CMOS_RESET_2ND;
	devio[SENSOR_IO_RESET_2ND].msk	= 1<<CFG_CMOS_RESET_2ND;
#endif
#ifdef GRP_CMOS_PWDN_2ND
	pwrio[CMOS_PWDN_2ND].grp     = GRP_CMOS_PWDN_2ND;
	pwrio[CMOS_PWDN_2ND].msk     = 1<<CFG_CMOS_PWDN_2ND;
	devio[SENSOR_IO_PWDN_2ND].grp	= GRP_CMOS_RESET_2ND;
	devio[SENSOR_IO_PWDN_2ND].msk	= 1<<CFG_CMOS_RESET_2ND;
#endif
/*devio*/
#ifdef GRP_CMOS_FREX
	devio[SENSOR_IO_FREX].grp	 = GRP_CMOS_FREX;
	devio[SENSOR_IO_FREX].msk	 = 1<<CFG_CMOS_FREX;
#endif

#ifdef GRP_CMOS_FREX_2ND
	devio[SENSOR_IO_FREX_2ND].grp	 = GRP_CMOS_FREX_2ND;
	devio[SENSOR_IO_FREX_2ND].msk	 = 1<<CFG_CMOS_FREX_2ND;
#endif


}

static ioseq_t cmosSysOn[]     ={{CMOS_PWR_ANA, 0,0},
                                 {CMOS_PWR_IO,  0,0},
                                 {CMOS_PWR_CORE,0,0},
                                 {CMOS_RESET,   0,0},
                                 {CMOS_PWDN,    0,0},{0xff}};

static ioseq_t cmosPwrAnaOn[]  ={{CMOS_PWR_ANA, 1, 0},{0xff}};
static ioseq_t cmosPwrIoOn[]   ={{CMOS_PWR_IO,  1, 0},{0xff}};
static ioseq_t cmosPwrCoreOn[] ={{CMOS_PWR_CORE,1, 0},{0xff}};
static ioseq_t cmosRstOn[]     ={{CMOS_RESET,   1, 0},{0xff}};
static ioseq_t cmosPwdnOn[]    ={{CMOS_PWDN,    1, 0},{0xff}};

static ioseq_t cmosPwdnOff[]   ={{CMOS_PWDN,    0, 0},{0xff}};
static ioseq_t cmosRstOff[]    ={{CMOS_RESET,   0, 0},{0xff}};
static ioseq_t cmosPwrCoreOff[]={{CMOS_PWR_CORE,0, 0},{0xff}};
static ioseq_t cmosPwrIoOff[]  ={{CMOS_PWR_IO,  0, 0},{0xff}};
static ioseq_t cmosPwrAnaOff[] ={{CMOS_PWR_ANA, 0, 0},{0xff}};
static ioseq_t cmosSysOff[]    ={{0xff}};


static ioseq_t cmos2SysOn[]	    ={{CMOS_PWR_ANA_2ND, 0,0},
								  {CMOS_PWR_IO_2ND,	0,0},
							 	  {CMOS_PWR_CORE_2ND,0,0},
								  {CMOS_RESET_2ND,	0,0},
								  {CMOS_PWDN_2ND,	0,1},{0xff}};

static ioseq_t cmos2PwrAnaOn[]  ={{CMOS_PWR_ANA_2ND, 1, 1},{0xff}};
static ioseq_t cmos2PwrIoOn[]   ={{CMOS_PWR_IO_2ND,  1, 1},{0xff}};
static ioseq_t cmos2PwrCoreOn[] ={{CMOS_PWR_CORE_2ND,1, 1},{0xff}};
static ioseq_t cmos2RstOn[]	    ={{CMOS_RESET_2ND,	 1, 1},{0xff}};
static ioseq_t cmos2PwdnOn[]    ={{CMOS_PWDN_2ND,	 1, 1},{0xff}};

static ioseq_t cmos2PwdnOff[]   ={{CMOS_PWDN_2ND,	 0, 1},{0xff}};
static ioseq_t cmos2RstOff[]    ={{CMOS_RESET_2ND,	 0, 1},{0xff}};
static ioseq_t cmos2PwrCoreOff[]={{CMOS_PWR_CORE_2ND,0, 1},{0xff}};
static ioseq_t cmos2PwrIoOff[]  ={{CMOS_PWR_IO_2ND,  0, 1},{0xff}};
static ioseq_t cmos2PwrAnaOff[] ={{CMOS_PWR_ANA_2ND, 0, 1},{0xff}};
static ioseq_t cmos2SysOff[]    ={{0xff}};


static struct {
	UINT32 id;
	ioseq_t *seq;
} cmosSeq[12]={
	{ SENSOR_DEV_POWER_ID_CMOS_SYS_ON,      cmosSysOn      },
	{ SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_ON,  cmosPwrAnaOn   },
	{ SENSOR_DEV_POWER_ID_CMOS_PWR_IO_ON,   cmosPwrIoOn    },
	{ SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_ON, cmosPwrCoreOn  },
    { SENSOR_DEV_POWER_ID_CMOS_RST_ON,      cmosRstOn      },
    { SENSOR_DEV_POWER_ID_CMOS_PWDN_ON,     cmosPwdnOn     },
	{ SENSOR_DEV_POWER_ID_CMOS_PWDN_OFF,    cmosPwdnOff    },
	{ SENSOR_DEV_POWER_ID_CMOS_RST_OFF,     cmosRstOff     },
	{ SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_OFF,cmosPwrCoreOff },
	{ SENSOR_DEV_POWER_ID_CMOS_PWR_IO_OFF,  cmosPwrIoOff   },
    { SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_OFF, cmosPwrAnaOff  },
	{ SENSOR_DEV_POWER_ID_CMOS_SYS_OFF,     cmosSysOff     },
};

 static struct {
	 UINT32 id;
	 ioseq_t *seq;
 } cmos2Seq[12]={
	 { SENSOR_DEV_POWER_ID_CMOS_SYS_ON, 	 cmos2SysOn		},
	 { SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_ON,  cmos2PwrAnaOn	},
	 { SENSOR_DEV_POWER_ID_CMOS_PWR_IO_ON,	 cmos2PwrIoOn	},
	 { SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_ON, cmos2PwrCoreOn	},
	 { SENSOR_DEV_POWER_ID_CMOS_RST_ON, 	 cmos2RstOn		},
	 { SENSOR_DEV_POWER_ID_CMOS_PWDN_ON,	 cmos2PwdnOn 	},
	 { SENSOR_DEV_POWER_ID_CMOS_PWDN_OFF,	 cmos2PwdnOff	},
	 { SENSOR_DEV_POWER_ID_CMOS_RST_OFF,	 cmos2RstOff 	},
	 { SENSOR_DEV_POWER_ID_CMOS_PWR_CORE_OFF,cmos2PwrCoreOff },
	 { SENSOR_DEV_POWER_ID_CMOS_PWR_IO_OFF,  cmos2PwrIoOff	},
	 { SENSOR_DEV_POWER_ID_CMOS_PWR_ANA_OFF, cmos2PwrAnaOff	},
	 { SENSOR_DEV_POWER_ID_CMOS_SYS_OFF,	 cmos2SysOff 	},
 };



ioseq_t* appSensorSeqCfg(UINT32 id)
{
	UINT16 i;
	UINT16 sel;
	sel=(id&SENSOR_DEV_POWER_ID_CMOS_SEC_SEL)?1:0;
	id&=(~SENSOR_DEV_POWER_ID_CMOS_SEC_SEL);
	if(!sel)
	{
		for (i=0 ; i<12 ; i++) {
			if (cmosSeq[i].id==id) {
				return cmosSeq[i].seq;
			}
		}
	}
	else{
		for (i=0 ; i<12 ; i++) {
			if (cmos2Seq[i].id==id) {
				return cmos2Seq[i].seq;
			}
		}
	}
	return NULL;
}

/* For Power IC use, added by Allen.Lin@2014/10/29 */
/* Sensor Power IC I2C configuration */
#define I2C_PORT				SP5K_SSC_PORT_5
#define I2C_PIN				SP5K_SSC_CFG_PIN1
#define PWRIC_I2C_ADDR		0x12

static SP5K_MUTEX txI2CMutex;

static UINT32 appPwrIC_I2C_Cfg(void)
{
	sp5kSscCfgSet(I2C_PORT, SP5K_SSC_CFG_MODE, SP5K_SSC_MODE_I2C);
	sp5kSscCfgSet(I2C_PORT, SP5K_SSC_CFG_PIN, I2C_PIN);
	sp5kSscCfgSet(I2C_PORT, SP5K_I2C_CFG_PHY, ((PWRIC_I2C_ADDR<<1)<<16)|(0<<8)|0x04);
	return SUCCESS;
}

void appI2CLockGet(void)
{
	HOST_ASSERT(txI2CMutex);
	sp5kOsMutexGet(&txI2CMutex, TX_WAIT_FOREVER);
}

void appI2CLockPut(void)
{
	HOST_ASSERT(txI2CMutex);
	sp5kOsMutexPut(&txI2CMutex);
}

UINT32 appI2CRead(UINT32 subAddr,UINT32 *pdata)
{
	UINT32 ret;
	appI2CLockGet();

		appPwrIC_I2C_Cfg();

	ret = sp5kI2cRead(I2C_PORT, subAddr, (UINT8 *)pdata, 1);
	if (ret != SUCCESS) {
		ret = FAIL;
	}
	appI2CLockPut();
	return ret;
}


UINT32 appI2CWrite(UINT32 par0, UINT32 mask, UINT32 par1)
{
	UINT32 data, tmp, buf[1];
	UINT32 ret=FAIL;
	appI2CLockGet();

		appPwrIC_I2C_Cfg();

	par1 &= mask;
	UINT32 timeout=50;

	if (SUCCESS == sp5kI2cRead(I2C_PORT, par0, (UINT8 *)&data, 1)) {
_write:
		buf[0] = (data & ~mask) | par1;
		ret = sp5kI2cWrite(I2C_PORT, par0, (UINT8 *)buf, 1);
		#if 0
		if (ret != SUCCESS) {
			ret = FAIL;
		}
		#else
		/**
		 * \note Edward 20111127
		 * add read-back check due to sometimes write function returns
		 * SUCCESS but the read-back value is not
		 */
		if ((ret == SUCCESS) && (SUCCESS == sp5kI2cRead(I2C_PORT, par0, (UINT8 *)&tmp, 1))) {
			if ((tmp & mask) != par1) {
				data = tmp;

				if (timeout==0) {
                    ret = FAIL;
                    goto _err;
                }
                timeout --;


				goto _write;
			}
		}
		else {
			ret = FAIL;
		}
		#endif
	}

_err:
	appI2CLockPut();
	return ret;
}

UINT32 appI2CInit(void)
{
	static UINT8 inited=0;
	if (!inited) {
		if (txI2CMutex==0) {
			sp5kOsMutexCreate(&txI2CMutex, "TX_I2C_MUTEX", TX_NO_INHERIT);
			HOST_ASSERT(txI2CMutex);
		}
		appPwrIC_I2C_Cfg();

	#if 1/* Example for sensor power IC initial setting (G2207) */
		appI2CWrite(0x0001, 0xFF, 0xF8); /* Initial */
		appI2CWrite(0x0002, 0xFF, 0x0F); /* CH6~CH9 : LDO */
		appI2CWrite(0x0005, 0xFF, 0xA0); /* CH5=2.5V, CH6~CH9 Shut down */
		appI2CWrite(0x0006, 0xFF, 0xCB); /* CH6=3.0V, CH7=2.8V(SEN_ANA) */
		appI2CWrite(0x0007, 0xFF, 0x52); /* CH8=1.8V(SEN_IO), CH9=1.2V(SEN_CORE) */
		appI2CWrite(0x0005, BIT3, BIT3); /* Enable CH6 (Audio) */
		appI2CWrite(0x0001, BIT1, BIT1); /* Enabe CH5(2710) */
	#endif
		inited = 1;
	}

	return SUCCESS;
}

