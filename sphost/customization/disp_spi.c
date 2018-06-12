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

#include "api/sp5k_global_api.h"
#include "customization/dev_init.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define LCD_MSK  (CS_MSK)

#ifdef SP5K_DISP_PIN_CB
static iopin_t spipin[4];  /* note that id is field used as mask here */
#define SCL_MSK  spipin[SPI_SCL].msk
#define SDA_MSK  spipin[SPI_SDA].msk
#define CS_MSK   spipin[SPI_CS ].msk
#define CS2_MSK  spipin[SPI_CS2].msk

#define SP5K_DISP_SPI_SCL_GRP  spipin[SPI_SCL].grp
#define SP5K_DISP_SPI_SDA_GRP  spipin[SPI_SDA].grp
#define SP5K_DISP_SPI_CS_GRP   spipin[SPI_CS ].grp
#define SP5K_DISP_SPI_CS2_GRP  spipin[SPI_CS2].grp

#ifndef LCD_BACKLIGHT_ON
#define LCD_BACKLIGHT_ON    SetPowerBacklight(1)
#define LCD_BACKLIGHT_OFF   SetPowerBacklight(0)
#define IS_BACKLIGHT_ON     GetPowerBacklight()
#define LCD2_BACKLIGHT_ON   SetPowerBacklight2(1)
#define LCD2_BACKLIGHT_OFF  SetPowerBacklight2(0)
#define IS_BACKLIGHT2_ON    GetPowerBacklight2()
#endif
#else
#define SCL_MSK  (1 << SP5K_DISP_SPI_SCL_ID)
#define SDA_MSK  (1 << SP5K_DISP_SPI_SDA_ID)
#define CS_MSK   (1 << SP5K_DISP_SPI_CS_ID)
#define CS2_MSK  (1 << SP5K_DISP_SPI_CS2_ID)

#define RSET_MSK (1 << SP5K_DISP_LCD_RESET_MASK)
#define BL_MSK   (1 << SP5K_DISP_LCD_BACKLIGHT_MASK)
#define BL2_MSK  (1 << SP5K_DISP_LCD2_BACKLIGHT_MASK)
#endif

#define SCL_LO sp5kGpioWrite(SP5K_DISP_SPI_SCL_GRP, SCL_MSK, 0)
#define SCL_HI sp5kGpioWrite(SP5K_DISP_SPI_SCL_GRP, SCL_MSK, SCL_MSK)
#define SDA_LO sp5kGpioWrite(SP5K_DISP_SPI_SDA_GRP, SDA_MSK, 0)
#define SDA_HI sp5kGpioWrite(SP5K_DISP_SPI_SDA_GRP, SDA_MSK, SDA_MSK)
#define CS_LO  sp5kGpioWrite(SP5K_DISP_SPI_CS_GRP, CS_MSK, 0)
#define CS_HI  sp5kGpioWrite(SP5K_DISP_SPI_CS_GRP, CS_MSK, CS_MSK)
#define CS2_LO sp5kGpioWrite(SP5K_DISP_SPI_CS_GRP, CS2_MSK, 0)
#define CS2_HI sp5kGpioWrite(SP5K_DISP_SPI_CS_GRP, CS2_MSK, CS2_MSK)

#if !defined(SP5K_DISP_PIN_CB)
 #if SP5K_DISP_SPI_CS_GRP==SP5K_DISP_SPI_SCL_GRP && SP5K_DISP_SPI_CS_GRP==SP5K_DISP_SPI_SDA_GRP
 #define ALL_EN         sp5kGpioCfgSet(SP5K_DISP_SPI_CS_GRP, CS2_MSK|CS_MSK|SCL_MSK|SDA_MSK, CS2_MSK|CS_MSK|SCL_MSK|SDA_MSK)
 #define SCL_EN_SDA_DIS sp5kGpioCfgSet(SP5K_DISP_SPI_SCL_GRP, SCL_MSK|SDA_MSK, SCL_MSK)
 #endif
#endif

#if !defined(ALL_EN)
#define ALL_EN {\
	sp5kGpioCfgSet(SP5K_DISP_SPI_CS2_GRP, CS2_MSK, CS2_MSK);\
	sp5kGpioCfgSet(SP5K_DISP_SPI_CS_GRP, CS_MSK, CS_MSK);\
	sp5kGpioCfgSet(SP5K_DISP_SPI_SCL_GRP, SCL_MSK, SCL_MSK);\
	sp5kGpioCfgSet(SP5K_DISP_SPI_SDA_GRP, SDA_MSK, SDA_MSK);\
	}
#endif

#if !defined(SCL_EN_SDA_DIS)
#define SCL_EN_SDA_DIS {\
	sp5kGpioCfgSet(SP5K_DISP_SPI_SCL_GRP, SCL_MSK, SCL_MSK);\
	sp5kGpioCfgSet(SP5K_DISP_SPI_SDA_GRP, SDA_MSK, 0);\
	}
#endif

#if !defined(LCD_BACKLIGHT_ON)
#define LCD_BACKLIGHT_ON     sp5kGpioWrite(SP5K_DISP_LCD_BACKLIGHT_GRP, BL2_MSK, BL2_MSK)
#define LCD_BACKLIGHT_OFF    sp5kGpioWrite(SP5K_DISP_LCD_BACKLIGHT_GRP, BL2_MSK, 0)
#define LCD2_BACKLIGHT_ON    sp5kGpioWrite(SP5K_DISP_LCD2_BACKLIGHT_GRP, BL2_MSK, BL2_MSK)
#define LCD2_BACKLIGHT_OFF   sp5kGpioWrite(SP5K_DISP_LCD2_BACKLIGHT_GRP, BL2_MSK, 0)
#endif

#define DELAY  { UINT32 _delay; for (_delay=0; _delay<3; _delay++); }

/* 1: LCD2, 0:LCD */
static UINT32 dispDev = -1;
/*volatile UINT32 dispDev;*/
extern void appDispIoCfg(iopin_t *io);
extern void SetPowerBacklight(UINT32 en);
extern void SetPowerBacklight2(UINT32 en);
extern UINT32 GetPowerBacklight(void);
extern UINT32 GetPowerBacklight2(void);

/**************************************************************************
 *                    M O D U L E    V A R I A B L E S                    *
 **************************************************************************/
/*
    Vendor      bitsLen  lsbFirst
    ------      -------  --------
    AUO           16        0
    CASIO         12        1
    SAMSUNG       24        0
    SONY          16        0
    Toppoly       16        0
*/

/* mode 1: lcd2, 0:lcd */
void
disp5kSpiInit(
	UINT32 mode
)
{
#ifdef HW_DISP_SPI
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_SSC_CFG_MODE, SP5K_SSC_MODE_SPI);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_SSC_CFG_PIN, SP5K_SSC_CFG_SSC1_PIN0);
	sp5kSscCfgSet(SP5K_SSC_PORT_1, SP5K_SPI_CFG_PHY, (200<<8));
#else
	/* 1.SAMSUNG, Hitachi, LOAD/DATA/CLK HI before transfer */
	/* CS_HI; CS_EN; SDA_HI; SDA_EN; SCL_HI; SCL_EN;		  */
	/* 2.others, LOAD HI before transfer						  */
#ifdef SP5K_DISP_PIN_CB
	if ( spipin[SPI_CS].msk == 0 ) {
		appDispIoCfg(spipin);
	}
	dispDev = mode ? 1 : 0;
#endif
	dispDev ? CS2_HI : CS_HI;
	#if SP5K_LCD_TD025THEB1_UPS052 /* scl/sda init low */
	SCL_LO; SDA_LO;
	#else
	SCL_HI; SDA_HI;
	#endif
	ALL_EN;
#endif
}

static UINT32
disp5kSpiDataPrepare(
	UINT32 val,
	UINT32 bitsLen,
	UINT32 lsbFirst
)
{
	UINT32 data;
	if (lsbFirst) {
		data = val;
	}
	else {
		UINT32 i;
		data = 0;
		for (i=0 ; i<24 ; i++) {
			if (val & 0x800000) {
				data |= (0x0001 << i);
			}
			val <<= 1;
		}
		data >>= (24 - bitsLen);
	}
	return data;
}

void
disp5kSpiCtrl(
	UINT32 val,
	UINT32 bitsLen,
	UINT32 lsbFirst
)
{
	UINT32	i,data;
	if ( dispDev == -1 ) {
		disp5kSpiInit(0); /* CS_LO; CS_EN; SDA_LO; SDA_EN; SCL_LO; SCL_EN;*/
		dispDev = 0;
	}
	data = disp5kSpiDataPrepare(val, bitsLen, lsbFirst);

#ifdef HW_DISP_SPI
	#if SPCA5330 || SPCA6330
	sp5kSpiWrite(SP5K_SSC_PORT_1, &data, 16);
	#else
	sp5kSpiWrite(SP5K_SSC_PORT_1+2, &data, 16);
	#endif
	return;
#endif


	/* Start transfer:
	   LOAD:
	   HI -> LO
		1.SAMSUNG, Hitachi, DATA/CLK HI
		2.others, DATA/CLK LO
	   LO -> HI
		1.PVI3001A
	*/
	dispDev ? CS2_LO : CS_LO; /* SDA_LO; SCL_LO;*/
	for ( i = 0; i < bitsLen; i++ ) {
		SCL_LO;
		if ( data & 0x0001 ) {
			SDA_HI;
		}
		else {
			SDA_LO;
		}
		DELAY;
		SCL_HI;
		data >>= 1;
		DELAY;
		SCL_LO;
		DELAY;
		#if SP5K_LCD_TD025THEB1_UPS052
		SDA_LO;
		#endif
	}
	dispDev ? CS2_HI : CS_HI;
	SCL_HI;
}

UINT32
disp5kSpiRead(
	UINT32 addr,
	UINT32 bitsLen,
	UINT32 lsbFirst
)
{
	UINT32 i,data,val;

	/*disp5kSpiInit();*/
	disp5kSpiDataPrepare(addr, bitsLen, lsbFirst);
#ifdef HW_DISP_SPI
	/*sp5kSpiWrite(SP5K_SSC_PORT_1, data, 16);*/
	return 0;
#endif

	dispDev ? CS2_LO : CS_LO; /* SDA_LO; SCL_LO;*/
	for ( i = 0; i < bitsLen ; i++ ) {
		SCL_LO;
		if ( data & 0x0001 ) {
			SDA_HI;
		}
		else {
			SDA_LO;
		}
		DELAY;
		SCL_HI;
		data >>= 1;
		DELAY;
		SCL_LO;
		DELAY;
		#if SP5K_LCD_TD025THEB1_UPS052
		SDA_LO;
		#endif
	}

	val = 0;

	SCL_EN_SDA_DIS;

	for ( i = 0; i < 8; i++ ) {
		SCL_HI;
		sp5kGpioRead(SP5K_DISP_SPI_SDA_GRP, SDA_MSK, &data);
		if(data & SDA_MSK)
			val |= (0x01 << (0x07 - i));
		DELAY;
		SCL_LO;
		DELAY;
	}
	dispDev ? CS2_HI : CS_HI;
	SCL_HI;
	return(val);
}

