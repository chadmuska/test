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
#ifndef _APP_GSENSOR_H_
#define _APP_GSENSOR_H_


 /**************************************************************************
  * 						  C O N S T A N T S 						   *
  **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
typedef enum _gSensorSelector_e {
	GSENSOR_CHIP_ID,
	GSENSOR_CAPABILITY_IDX,
	GSENSOR_CAPABILITY_DATA,
	GSENSOR_XYZ,
	GSENSOR_ANGULAR_RATE,
} gSensorSelector_e;

typedef struct _gSensorInit_t {
	UINT32 ulAddr;
	UINT32 ulSize;
} gSensorInit_t;

typedef struct _gSensorCfg_t {
	UINT32 ulselector;
	UINT32 ulvalue;
} gSensorCfg_t;

typedef enum _gSensorCfgSelector_e {
	GSENSOR_CFG_NULL,
	GSENSOR_CFG_NBIT_REGADDR,
	GSENSOR_CFG_NBIT_REGDATA,
	GSENSOR_CFG_READBIT,
	GSENSOR_CFG_NBIT_UNIT,
	GSENSOR_CFG_SLAVEADDR,
	GSENSOR_CFG_FERQUENCY_I2C,
	GSENSOR_CFG_ENADDR2BYTE,
	GSENSOR_CFG_READMODE,
	GSENSOR_CFG_CS_GRP_ID,
	GSENSOR_CFG_CS_PIN_MASK,
	GSENSOR_CFG_FERQUENCY_SPI,
	GSENSOR_CFG_MODE_SPI,
} gSensorCfgSelector_e;

typedef struct _gSensorCapab_t {
	UINT32 ucMeasureRange;
	UINT32 ucSensitivity;
} gSensorCapab_t;

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appGsensorDriverInit(UINT32 ulRegInitTblSize);
void appGSensorDriverWriteStatus(UINT32 regAddr,UINT32  regData[],UINT32 writeLen);
UINT32 appGSensorDriverReadStatus(UINT32 regAddr,UINT32  regData[],UINT32 writeLen,UINT32 readLen);
void appGsensorDriverCfgSet(UINT32 selector,UINT32 value);
UINT8 *CombineRegData(UINT8 *pui8Buf, UINT32 *pui32Data,UINT8 shiftnbit);
UINT8 *CombineRegAddr(UINT8 *pui8Buf, UINT32 *pui32Data,UINT8 shiftnbit);


#endif  /* _APP_GSENSOR_H_ */



