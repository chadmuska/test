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
#include "middleware/sensor_model.h"
#include "app_spca7002_utility.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
extern const sensorTgDev_t *psensorSecTgDevGet(void);

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 appSPCA7002_I2CDataWrite(UINT32 a_udwInputAddr, UINT32 a_udwInputData)
{
	const sensorTgDev_t *pSPCA7002tgdev;
	UINT32 udwComPortid;
	UINT32 udwErrRet;
	pSPCA7002tgdev = psensorSecTgDevGet();
	if (pSPCA7002tgdev == NULL)
		return 0xff;
	
	udwComPortid = pSPCA7002tgdev->tgCfg->cmdportid_xtg;
	
	static UINT8 regdata[3] SENSOR_SIF1DMA_VARBUF = 
	{
		0x71,0x00,0x00
	};

	regdata[0] = (UINT8)((a_udwInputAddr>>8)&0xff);
	regdata[1] = (UINT8)(a_udwInputAddr &0xff);
	regdata[2] = (UINT8)(a_udwInputData &0xff);

	sensorSerialDirectBufSnoop(regdata, sizeof(regdata));
	udwErrRet = sensorSerialBulkWrite(udwComPortid,1,regdata,0);
	/*printf("#@# SPCA7002 cmdport[%d] I2c Write [0x%x]=0x%x\n",udwComPortid,(regdata[0]<<8)|(regdata[1]),a_udwInputData);*/

	return udwErrRet;
	
}

UINT32 appSPCA7002_I2CDataRead(UINT32 a_udwInputAddr)
{
	const sensorTgDev_t *pSPCA7002tgdev;
	UINT32 udwComPortid;
	UINT32 udwErrRet;
	static UINT8 regdata[3];

	regdata[0] = (UINT8)((a_udwInputAddr>>8)&0xff);
	regdata[1] = (UINT8)(a_udwInputAddr &0x00ff);
	pSPCA7002tgdev = psensorSecTgDevGet();
	if (pSPCA7002tgdev == NULL)
		return 0xff;
	
	udwComPortid = pSPCA7002tgdev->tgCfg->cmdportid_xtg;
	sensorSerialDirectBufSnoop(regdata, sizeof(regdata));
	udwErrRet = sensorSerialBlockRead(udwComPortid,1,regdata,0);
	/*printf("#@# SPCA7002 cmdport[%d] I2C Read [0x%x]=0x%x\n",udwComPortid,(regdata[0]<<8)|(regdata[1]),regdata[2]);*/

	if(udwErrRet == SUCCESS)
		return regdata[2];
	else
		return FAIL;

}

