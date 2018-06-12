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
#include "common.h"
#include "api/sp5k_utility_api.h"
#include "api/sp5k_global_api.h"
#include "app_gSensor.h"

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

#define NBIT_REGADDR 	8
#define NBIT_REGDATA	8
#define READBIT			0x80	/* SPI INTERFACE sync NBIT_REGADDR */
#define NBIT_UNIT		8

#define SLAVEADDR	0x32
#define FERQUENCY_I2C 12    /*temp value for fix compiler error, please design this value by real project ,original is SP5K_I2C_CLK_DIVISOR_1024*/
#define ENADDR2BYTE	0
#define READMODE	0x04

#define CS_GRP_ID	SP5K_GPIO_GRP_LMI	/* SPI CHIP SELECT GPIO GROUP ID */
#define CS_PIN_MASK 10
#if SPCA6330
#define FERQUENCY_SPI SP5K_SPI_PORT2_CLK_1_5MHZ
#else
#define FERQUENCY_SPI 5
#endif
#define MODE_SPI	0x03

/* Register Initialization Table	*/
UINT8 regInitTbl_01[] = {		/* {ADDR, DATA} */
};

static gSensorCfg_t tgSensorCfgGrp[] = {
	{GSENSOR_CFG_NBIT_REGADDR, NBIT_REGADDR},
	{GSENSOR_CFG_NBIT_REGDATA, NBIT_REGDATA},
	{GSENSOR_CFG_READBIT, READBIT},
	{GSENSOR_CFG_NBIT_UNIT, NBIT_UNIT},
	{GSENSOR_CFG_SLAVEADDR, SLAVEADDR},
	{GSENSOR_CFG_FERQUENCY_I2C, FERQUENCY_I2C},
	{GSENSOR_CFG_ENADDR2BYTE, ENADDR2BYTE},
	{GSENSOR_CFG_READMODE, READMODE},
	{GSENSOR_CFG_CS_GRP_ID, CS_GRP_ID},
	{GSENSOR_CFG_CS_PIN_MASK, CS_PIN_MASK},
	{GSENSOR_CFG_FERQUENCY_SPI, FERQUENCY_SPI},
	{GSENSOR_CFG_FERQUENCY_SPI, FERQUENCY_SPI},
	{GSENSOR_CFG_NULL, 0},
};

static gSensorInit_t gSensorInitTblGrp[] = {
	{regInitTbl_01, sizeof(regInitTbl_01)},
};

static gSensorCapab_t tgSensorCapab[] = {
	{0, 0}
};

static UINT8 ucCapabIdx = 0;

#define GSENSOR_CMD_ANGULAR_RATE 0xff

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

void
appGsensorInit(
	void
)
{
	return;
}


void
appGSensorWriteStatus(
	UINT32 regAddr,
	UINT32 regData[],
	UINT32 writeLen
)
{
	return;
}


UINT8*
appGSensorReadStatus(
	UINT32 regAddr,
	UINT32 regData[],
	UINT32 writeLen,
	UINT32 readLen
)
{
	return NULL;
}

void appGSensorGetData(UINT32 selector, UINT32* param1, UINT32* param2, UINT32* param3)
{
	return;
}

void appGSensorSetData(UINT32 selector, UINT32 param1, UINT32 param2, UINT32 param3)
{
	return;
}

