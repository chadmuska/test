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
#include <stdio.h>
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
static UINT8 nreg;
#if I2C_INTERFACE
#define I2C_PORT 		SP5K_SSC_PORT_1
#elif SPI_PORT2_INTERFACE
#define SPI_PORT 		SP5K_SSC_PORT_2
#else
#define SPECIAL_PORT 	SP5K_SSC_PORT_3
#endif

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

UINT32 ulNBIT_REGADDR;
UINT32 ulNBIT_REGDATA;
UINT32 ulREADBIT;
UINT32 ulNBIT_UNIT;

UINT32 ulSLAVEADDR;
UINT32 ulFERQUENCY_I2C;
UINT32 ulENADDR2BYTE;
UINT32 ulREADMODE;

UINT32 ulCS_GRP_ID;
UINT32 ulCS_PIN_MASK;
UINT32 ulFERQUENCY_SPI;
UINT32 ulMODE_SPI;

UINT8 regInitTbl[100];
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void
appGsensorDriverInit(
	UINT32 ulRegInitTblSize
)
{
	nreg = ulRegInitTblSize/((ulNBIT_REGADDR+ulNBIT_REGDATA)/8);
	printf("nreg = %x, sizeof(regInitTbl) = %x, sizeof(regInitTbl[0] = %x<<<<<<<<<<<<<<<\n",
		nreg, ulRegInitTblSize,(ulNBIT_REGADDR+ulNBIT_REGDATA)/8);

	#if I2C_INTERFACE
	UINT32 param1;
	param1 = ((ulSLAVEADDR<<16) | (ulFERQUENCY_I2C<<8) | (ulENADDR2BYTE<<5) | ulREADMODE);
	sp5kSscCfgSet(I2C_PORT, SP5K_SSC_CFG_MODE, SP5K_SSC_MODE_I2C);
	sp5kSscCfgSet(I2C_PORT, SP5K_SSC_CFG_PIN, SP5K_SSC_CFG_PIN1);
	sp5kSscCfgSet(I2C_PORT, SP5K_I2C_CFG_PHY, param1);					/* ena2byte : slave address whether to write two bytes */

	#elif SPI_PORT2_INTERFACE
	sp5kSscCfgSet(SPI_PORT, SP5K_SPI_CFG_PHY, (ulFERQUENCY_SPI<<24)|(ulMODE_SPI<<2));
	sp5kGpioCfgSet(ulCS_GRP_ID, (((UINT32)1)<<ulCS_PIN_MASK), (((UINT32)1)<<ulCS_PIN_MASK));
	sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)1)<<ulCS_PIN_MASK);
	halPinMux293(0);

	#else 	/* SPI_PORT3_INTERFACE	*/
	sp5kSscCfgSet(SPECIAL_PORT, SP5K_SSC_CFG_PIN, 0);
	sp5kSscCfgSet(SPECIAL_PORT, SP5K_GSI_CFG_PHY, (ulMODE_SPI << 16 | ulFERQUENCY_SPI << 8));
	sp5kGpioCfgSet(ulCS_GRP_ID, (((UINT32)1)<<ulCS_PIN_MASK), (((UINT32)1)<<ulCS_PIN_MASK));
	sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)1)<<ulCS_PIN_MASK);
	halPinMux293(0);
	#endif

	appGSensorDriverWriteStatus(0, 0, 0);
}


void
appGSensorDriverWriteStatus(
	UINT32 regAddr,
	UINT32 regData[],
	UINT32 writeLen
)

{
	UINT8 *p = regInitTbl;
	UINT32 Addr;

	#if I2C_INTERFACE
	if(regAddr || regData){		/* Write single register */
		sp5kI2cWrite(I2C_PORT, regAddr, (UINT8*)regData, (UINT32)(writeLen/ulNBIT_UNIT));
	}
	else{
		UINT8 k;
		UINT32 Data[ulNBIT_REGDATA/ulNBIT_UNIT];
		for(k=0;k<nreg;k++){	/* Write register table */
			p = CombineRegAddr(p,&Addr,ulNBIT_REGADDR);
			p = CombineRegData(p,Data,ulNBIT_REGDATA);
			printf("Addr = %x, Data = %x <<<<<<<<<<<<<<\n", Addr, Data[0]);
			sp5kI2cWrite(I2C_PORT,Addr, (UINT8*)Data, (UINT32)(ulNBIT_REGDATA/ulNBIT_UNIT));
		 }
	}

	#elif SPI_PORT2_INTERFACE
	UINT8 idata[32];
	int i;

	sp5kSscCfgSet(SPI_PORT, SP5K_SSC_LOCK, 1);

	if(regAddr || regData){
		for(i=0; i<writeLen/ulNBIT_UNIT; i++)
			idata[i] = regData[i];
		sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)0));
		sp5kSpiWrite(SPI_PORT, (UINT8 *)&regAddr, ulNBIT_REGADDR);
		sp5kSpiWrite(SPI_PORT, (UINT8 *)&idata[0], writeLen);
		sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)1)<<ulCS_PIN_MASK);
	}
	else{
		UINT8 k;
		UINT32 Data[ulNBIT_REGDATA/ulNBIT_UNIT];
		for(k=0;k<nreg;k++)
		{
			p = CombineRegAddr(p,&Addr,ulNBIT_REGADDR);
			p = CombineRegData(p,Data,ulNBIT_REGDATA);
			sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)0));	 			/* CS low */
			sp5kSpiWrite(SPI_PORT, (UINT8 *)&Addr, ulNBIT_REGADDR);
			for(i=0; i<ulNBIT_REGDATA/ulNBIT_UNIT; i++)
				idata[i] = Data[i];
			sp5kSpiWrite(SPI_PORT, (UINT8 *)idata, ulNBIT_REGDATA);
			sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)1)<<ulCS_PIN_MASK); 			/* CS high */
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 10);
			printf("%s, %d, %x, %x\n", __FUNCTION__, __LINE__, Addr, Data[0]);
		}
	}

	sp5kSscCfgSet(SPI_PORT, SP5K_SSC_LOCK, 0);

	#else	/* SPI_PORT3_INTERFACE */

	if(regAddr || regData){
		for(i=0; i<writeLen/ulNBIT_UNIT; i++)
			idata[i] = regData[i];
		sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)0));
		sp5kSpiWrite(SPECIAL_PORT, (UINT8 *)&regAddr, ulNBIT_REGADDR);
		sp5kSpiWrite(SPECIAL_PORT, (UINT8 *)&idata[0], writeLen);
		sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)1)<<ulCS_PIN_MASK);
	}
	else{
		UINT8 k;
		UINT32 Data[ulNBIT_REGDATA/ulNBIT_UNIT];
		for(k=0;k<nreg;k++)
		{
			p = CombineRegAddr(p,&Addr,ulNBIT_REGADDR);
			p = CombineRegData(p,Data,ulNBIT_REGDATA);
			sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)0));	 			/* CS low */
			sp5kSpiWrite(SPECIAL_PORT, (UINT8 *)&Addr, ulNBIT_REGADDR);
			for(i=0; i<ulNBIT_REGDATA/ulNBIT_UNIT; i++)
				idata[i] = Data[i];
			sp5kSpiWrite(SPECIAL_PORT, (UINT8 *)idata, ulNBIT_REGDATA);
			sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)1)<<ulCS_PIN_MASK); 			/* CS high */
			sp5kTimeDelay(SP5K_TIME_DELAY_1MS, 10);
		}
	}
	#endif

}


UINT32
appGSensorDriverReadStatus(
	UINT32 regAddr,
	UINT32 regData[],
	UINT32 writeLen,
	UINT32 readLen
)


{
	UINT32 p =0;
	UINT8 odata[32];

	#if I2C_INTERFACE

	if(regAddr){		/* Read single register address */
		UINT32 Data[readLen/ulNBIT_UNIT];
		sp5kI2cRead(I2C_PORT, regAddr, (UINT8*)Data, (UINT32)(readLen/ulNBIT_UNIT));
		int i;
		for(i=0; i<readLen/ulNBIT_UNIT; i++)
			odata[i] = Data[i];
		p = odata[0];
	}

	#elif SPI_PORT2_INTERFACE
	UINT8 idata[32];
	UINT32 readAddr;

	sp5kSscCfgSet(SPI_PORT, SP5K_SSC_LOCK, 1);

	if(regAddr){		/* Read single register address */
		int i;
		for(i=0; i<writeLen/ulNBIT_UNIT; i++)
			idata[i] = regData[i];

		readAddr = (regAddr | ulREADBIT);
		sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)0));

		sp5kSpiWrite(SPI_PORT, (UINT8 *)&readAddr, ulNBIT_REGADDR);

		if(writeLen != 0)
			sp5kSpiWrite(SPI_PORT, (UINT8 *)&idata[0], writeLen);

		sp5kSpiRead(SPI_PORT, (UINT8 *)&odata[0], readLen);

		sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)1)<<ulCS_PIN_MASK);
		p = odata[0];

	}

	sp5kSscCfgSet(SPI_PORT, SP5K_SSC_LOCK, 0);

	#else	/* SPI_PORT3_INTERFACE */

	if(regAddr){		/* Read single register address */
		int i;
		for(i=0; i<writeLen/ulNBIT_UNIT; i++)
			idata[i] = regData[i];

		readAddr = (regAddr | ulREADBIT);
		sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)0));

		sp5kSpiWrite(SPECIAL_PORT, (UINT8 *)&readAddr, ulNBIT_REGADDR);

		if(writeLen != 0)
			sp5kSpiWrite(SPECIAL_PORT, (UINT8 *)&idata[0], writeLen);

		sp5kSpiRead(SPECIAL_PORT, (UINT8 *)&odata[0], readLen);

		sp5kGpioWrite(ulCS_GRP_ID, ((UINT32)1)<<ulCS_PIN_MASK, ((UINT32)1)<<ulCS_PIN_MASK);
		p = odata[0];

	}
	#endif

	return p;
}

void
appGsensorDriverCfgSet(
	UINT32 selector,
	UINT32 value
)
{
	switch(selector)
	{
		case GSENSOR_CFG_NBIT_REGADDR:
			ulNBIT_REGADDR = value;
			break;
		case GSENSOR_CFG_NBIT_REGDATA:
			ulNBIT_REGDATA = value;
			break;
		case GSENSOR_CFG_READBIT:
			ulREADBIT = value;
			break;
		case GSENSOR_CFG_NBIT_UNIT:
			ulNBIT_UNIT = value;
			break;
		case GSENSOR_CFG_SLAVEADDR:
			ulSLAVEADDR = value;
			break;
		case GSENSOR_CFG_FERQUENCY_I2C:
			ulFERQUENCY_I2C = value;
			break;
		case GSENSOR_CFG_ENADDR2BYTE:
			ulENADDR2BYTE = value;
			break;
		case GSENSOR_CFG_READMODE:
			ulREADMODE = value;
			break;
		case GSENSOR_CFG_CS_GRP_ID:
			ulCS_GRP_ID = value;
			break;
		case GSENSOR_CFG_CS_PIN_MASK:
			ulCS_PIN_MASK = value;
			break;
		case GSENSOR_CFG_FERQUENCY_SPI:
			ulFERQUENCY_SPI = value;
			break;
		case GSENSOR_CFG_MODE_SPI:
			ulMODE_SPI = value;
			break;
		default:
			break;
	}
}

UINT8 *CombineRegData(
	UINT8 *pui8Buf,
	UINT32 *pui32Data,
	UINT8 shiftnbit
)
{
	/* big endian */
	if(pui8Buf != NULL && pui32Data != NULL && shiftnbit != 0)
	{
		if(shiftnbit == 8){
		*pui32Data = *pui8Buf;
		pui8Buf += 1;
		}
		if(shiftnbit == 16){
			pui32Data[0] = pui8Buf[0];
			pui32Data[1] = pui8Buf[1];
			pui8Buf += 2;
		}
		else if(shiftnbit == 24){
			pui32Data[0] = pui8Buf[0];
			pui32Data[1] = pui8Buf[1];
			pui32Data[2] = pui8Buf[2];
			pui8Buf += 3;
		}
		else if(shiftnbit == 32){
			pui32Data[0] = pui8Buf[0];
			pui32Data[1] = pui8Buf[1];
			pui32Data[2] = pui8Buf[2];
			pui32Data[3] = pui8Buf[3];
			pui8Buf += 4;
		}

	}

	return pui8Buf;

 }

UINT8 *CombineRegAddr(
	UINT8 *pui8Buf,
	UINT32 *pui32Data,
	UINT8 shiftnbit
)
{
	/* big endian */
	if(pui8Buf != NULL && pui32Data != NULL && shiftnbit != 0)
	{
		if(shiftnbit == 8){
		*pui32Data = *pui8Buf;
		pui8Buf += 1;
		}
		if(shiftnbit == 16){
			*pui32Data = (UINT32)pui8Buf[1] |
				((UINT32)pui8Buf[0] << 8);
			pui8Buf += 2;
		}
		else if(shiftnbit == 24){
			*pui32Data = (UINT32)pui8Buf[2] |
				((UINT32)pui8Buf[1] << 8) |
				((UINT32)pui8Buf[0] << 16);
			pui8Buf += 3;
		}
		else if(shiftnbit == 32){
			*pui32Data = (UINT32)pui8Buf[3] |
				((UINT32)pui8Buf[2] << 8) |
				((UINT32)pui8Buf[1] << 16) |
				((UINT32)pui8Buf[0] << 24);
			pui8Buf += 4;
		}

	}

	return pui8Buf;

 }

UINT8 *SeparateRegAddrData(
	UINT8 *pui8Buf,
	UINT32 *pui32Data,
	UINT8 shiftnbit
)
{
	/* big endian */
	if(pui8Buf != NULL && pui32Data != NULL && shiftnbit != 0)
	{
		if(shiftnbit == 8){
		*pui8Buf = *pui32Data;
		pui8Buf += 1;
		}
		if(shiftnbit == 16){
			*pui8Buf = (*pui32Data & 0xff00)>>8;
			*(pui8Buf+1) = *pui32Data & 0x00ff;

			pui8Buf += 2;
		}
		else if(shiftnbit == 24){
			*pui8Buf = (*pui32Data & 0xff0000)>>16;
			*(pui8Buf+1) = (*pui32Data & 0x00ff00)>>8;
			*(pui8Buf+2) = *pui32Data & 0x0000ff;
			pui8Buf += 3;
		}
		else if(shiftnbit == 32){
			*pui8Buf = (*pui32Data & 0xff000000)>>24;
			*(pui8Buf+1) = (*pui32Data & 0x00ff0000)>>16;
			*(pui8Buf+2) = (*pui32Data & 0x0000ff00)>>8;
			*(pui8Buf+3) = *pui32Data & 0x000000ff;
			pui8Buf += 4;
		}

	}

	return pui8Buf;

 }

