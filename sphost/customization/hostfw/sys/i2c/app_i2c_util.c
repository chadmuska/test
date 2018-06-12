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
 /*--------------------------------------------------------------------------*
 *  File name	: 	app_i2c_util.c
 *  Function 	: 	i2c data process	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.06.04  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#define HOST_DBG 1

#include <stdio.h>

#include "sp5k_sensor_api.h"
#include "sp5k_fs_api.h"
#include "middleware/sensor_model.h"

#include "app_still.h"
#include "app_i2c_lib.h"


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
static const UINT32 stSpiIdInfo[9][3] = {
#if 0
	/*EON*/
	{0x001C3117,},
	0x001C2016,
	0x001C3116,
	0x001C3115,
	0x001C3114,
	0x001C3113,
	/*Spansion*/
	0x00012018,
	0x00010216,
	0x00010215,
	0x00010214,
	/*ST*/
	0x00202018,
	0x00202017,
	0x00202016,
	0x00202015,
	0x00202014,
	/*MXIC*/
	0x00C22018,
	0x00C22017,
	0x00C22016,
	0x00C25e16,
	0x00C22015,
	0x00C22014,
	0x00C22013,
#endif
	/*Winbond*/
	{0x00EF3017, 4096, 2048},
	{0x00EF3016, 4096, 1024},
	{0x00EF3015, 4096, 512},
	{0x00EF3014, 4096, 256},
	{0x00EF5014, 4096, 256},
	{0x00EF3013, 4096, 128},
	{0x00EF5013, 4096, 128},
	/* Add new Winbond ID @ 2014/09/01 */
	{0x00C22534, 4096, 256},
	
	/*Fail*/
	{0x00000000, 0, 0},
};

static const UINT32 sstSpiIdInfo[6][3] = {
	/*ESMT*/
	{0x008C4016, 4096, 512},
	/*SST*/
	{0x00BF254A, 4096, 1024},
	{0x00BF2541, 4096, 512},
	{0x00BF258E, 4096, 256},
	{0x00BF258D, 4096, 128},
	/*Fail*/
	{0x00000000, 0, 0},
};

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
#if(SP5K_SENSOR_FAKE&&(defined(HW_SBC))||SP5K_SENSOR_FAKE&&(defined(HW_JETDVR))||SP5K_SENSOR_FAKE&&(defined(HW_PDVR))||SP5K_SENSOR_FAKE&&(defined(HW_17DVR))||SP5K_SENSOR_FAKE&&(defined(HW_19DVR)))
extern void sensorFakeSPCA7002_I2CDataWrite(UINT32 a_udwAddr, UINT32 a_udwData);
extern UINT32 sensorFakeSPCA7002_I2CDataRead(UINT32 a_udwAddr);
#else
extern void sensorSingleWrite(UINT16 addr,UINT16 data);
extern UINT16 sensorRead(UINT16 addr);
#endif
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name : seqI2CDataRead
* Function		: seq I2C Data Read
* Return value	: void
* Parameter1    : address (UINT32)
* Parameter2    : buffer pointer (UINT8 pointer)
* History	   	: 
*                	:  2018.06.04  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void seqI2CDataRead(UINT32 a_InputAddr, UINT8* pBuf)
{
	UINT32 cmdportid = 0;
	UINT8 i, regread[10];
	
	#if(SP5K_SENSOR_FAKE&&(defined(HW_SBC))||SP5K_SENSOR_FAKE&&(defined(HW_JETDVR))||SP5K_SENSOR_FAKE&&(defined(HW_PDVR))||SP5K_SENSOR_FAKE&&(defined(HW_17DVR))||SP5K_SENSOR_FAKE&&(defined(HW_19DVR)))
		extern UINT32 cmdportid_fake_spca7002_seq;		
		cmdportid = cmdportid_fake_spca7002_seq;
	#else
		extern UINT32 cmdportid_seq;
		cmdportid = cmdportid_seq; /*sensorCmdportIdGet(SENSOR_CMDPORTID_SEL_XTG);*/
	#endif
	
	regread[0] = a_InputAddr>>8;
	regread[1] = a_InputAddr&0xFF;

	sensorSerialBlockRead(cmdportid, 1, regread, 0);
	/*DBG_PRINTF("\nREAD [%x] = [%x]\n", a_InputAddr, regread[2]);*/

	for (i = 0; i < 8; i++) {
		*(pBuf+i) = regread[2+i];
	}
}

/*--------------------------------------------------------------------------*
* Function name : seqI2CDataWrite
* Function		: seq I2C Data Write
* Return value	: void
* Parameter1    : address (UINT32)
* Parameter2    : buffer pointer (UINT8 pointer)
* History	   	: 
*                	:  2018.06.04  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void seqI2CDataWrite(UINT32 a_InputAddr, UINT8* pBuf)
{
	UINT32 cmdportid = 0;
	
#if(SP5K_SENSOR_FAKE&&(defined(HW_SBC))||SP5K_SENSOR_FAKE&&(defined(HW_JETDVR))||SP5K_SENSOR_FAKE&&(defined(HW_PDVR))||SP5K_SENSOR_FAKE&&(defined(HW_17DVR))||SP5K_SENSOR_FAKE&&(defined(HW_19DVR)))
	extern UINT32 cmdportid_fake_spca7002_seq;		
	cmdportid = cmdportid_fake_spca7002_seq;
#else
	extern UINT32 cmdportid_seq;
	cmdportid = cmdportid_seq;
#endif

	static UINT8 regdata1[] SENSOR_SIF1DMA_VARBUF = {
		0x10, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	regdata1[0] = (a_InputAddr & 0xff00)>>8;
	regdata1[1] = a_InputAddr & 0x00ff;
	regdata1[2] = *(pBuf) & 0x00ff;
	regdata1[3] = *(pBuf+1) & 0x00ff;
	regdata1[4] = *(pBuf+2) & 0x00ff;
	regdata1[5] = *(pBuf+3) & 0x00ff;
	regdata1[6] = *(pBuf+4) & 0x00ff;
	regdata1[7] = *(pBuf+5) & 0x00ff;
	regdata1[8] = *(pBuf+6) & 0x00ff;
	regdata1[9] = *(pBuf+7) & 0x00ff;
	/*printf("regdata1[0]=%x, regdata1[1]=%x, regdata1[2]=%x\n", regdata1[0], regdata1[1], regdata1[2]);*/

	sensorSerialDirectBufSnoop(regdata1, sizeof(regdata1));
	sensorSerialBulkWrite(cmdportid, 1, regdata1,0);
}

/*--------------------------------------------------------------------------*
* Function name : BB_I2CDataWrite
* Function		: I2C Data Write
* Return value	: void
* Parameter1    : address (UINT32)
* Parameter2    : Data (UINT32)
* History	   	: 
*                	:  2018.06.04  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void BB_I2CDataWrite(UINT32 a_udwAddr, UINT32 a_udwData)
{
	#if(SP5K_SENSOR_FAKE&&(defined(HW_SBC))||SP5K_SENSOR_FAKE&&(defined(HW_JETDVR))||SP5K_SENSOR_FAKE&&(defined(HW_PDVR))||SP5K_SENSOR_FAKE&&(defined(HW_17DVR))||SP5K_SENSOR_FAKE&&(defined(HW_19DVR)))
		sensorFakeSPCA7002_I2CDataWrite(a_udwAddr,a_udwData);
	#else
		sensorSingleWrite((UINT16)a_udwAddr,(UINT16)a_udwData);
	#endif
}

/*--------------------------------------------------------------------------*
* Function name : BB_I2CDataRead
* Function		: I2C Data Read
* Return value	: void
* Parameter1    : address (UINT32)
* History	   	: 
*                	:  2018.06.04  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT32 BB_I2CDataRead(UINT32 a_udwAddr)
{
	#if(SP5K_SENSOR_FAKE&&(defined(HW_SBC))||SP5K_SENSOR_FAKE&&(defined(HW_JETDVR))||SP5K_SENSOR_FAKE&&(defined(HW_PDVR))||SP5K_SENSOR_FAKE&&(defined(HW_17DVR))||SP5K_SENSOR_FAKE&&(defined(HW_19DVR)))
		return sensorFakeSPCA7002_I2CDataRead(a_udwAddr);
	#else
		return sensorRead((UINT16)a_udwAddr);
	#endif
}

/*--------------------------------------------------------------------------*
* Function name : BB_SerialFlashTypeCheck
* Function		: Check serial flash type
* Return value	: flash type (UINT32)
*				0(FAIL), 1(ST), 2(SST), 3(UNKNOWN)
* Parameter1    : id (UINT32)
* Parameter2    : size of spi (UINT32 pointer)
* History	   	: 
*                	:  2018.06.04  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT32 BB_SerialFlashTypeCheck(UINT32 id, UINT32 *spiSize)
{
	UINT32 tblId, shift = 0, type = 0, fullID = 1, i = 0;

	/*printf("id:0x%x spiSize:0x%x\n",id,spiSize);*/
	/* check whether SST type serial flash */
	while (1) {
		tblId = sstSpiIdInfo[i][0] >> shift;
		if (id == tblId) {
			printf("SST type serial flash:%x %x %x\n", i, id, sstSpiIdInfo[i][0]);
			type = 2;
			*spiSize = sstSpiIdInfo[i][1]*sstSpiIdInfo[i][2];
			break;
		}
		if (id == 0x00FFFFFF || id == 0x00000000) {
			return 0;
		}
		if (sstSpiIdInfo[i][0] == 0x00000000) {
			#if 0
			if (fullID) {
				fullID = 0; /* sarch partial ID */
				i = 0;
				shift = 16;
				id = id >> shift;
				continue;
			}
			#endif
			type = 3;
			break;
		}
		i++;
	}

	if (type == 2)
		return type;

	i = 0;
	/* check whether ST type serial flash */
	while (1) {
		tblId = stSpiIdInfo[i][0] >> shift;
		if (id == tblId) {
			printf("ST Type serial flash:%x %x %x\n", i, id, stSpiIdInfo[i][0]);
			type = 1;
			*spiSize = stSpiIdInfo[i][1]*stSpiIdInfo[i][2];
			/*printf("spiSize:0x%x\n",*spiSize);*/
			break;
		}
		if (id == 0x00FFFFFF || id == 0x00000000) {
			return 0;
		}
		if (stSpiIdInfo[i][0] == 0x00000000) {
			if( fullID ){
				fullID = 0; /* sarch partial ID */
				i = 0;
				shift = 16;
				id = id >> shift;
				continue;
			}
			type = 3;
			break;
		}
		i++;
	}

	return type;
}

/*--------------------------------------------------------------------------*
* Function name :BB_EraseSPIFlash
* Function		: Erase serial flash 
* Return value	: void
* Parameter1    : type (UINT32)
* Parameter2    : size of spi (UINT32)
* History	   	: 
*                	:  2018.06.04  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void BB_EraseSPIFlash(UINT32 type, UINT32 spiSize)
{
	UINT32 i, temp1;
	UINT8 typeFlag = 1;

	/*printf("spiSize: 0x%x\n", spiSize);*/
	if (type == 2) { /* SST */
		typeFlag = 0;
	}
	else if (type == 1 || type == 3) { /* ST */
		typeFlag = 1;
	}

	if (spiSize == (512*1024)) {
		/* skip 0x7B000 ~ 0x7EFF, to keep calibration data */
		temp1 = (spiSize / 0x10000)-1;
		for (i = 0; i < temp1; i++) {
			BB_SPI64KBBlockErase(i*0x10000, typeFlag);
		}
		BB_SPI32KBBlockErase(temp1*0x10000, typeFlag);
		temp1 = temp1*0x10000 + 0x8000;
		for (i = temp1; i < spiSize-0x5000; i+=0x1000) {
			BB_SPISectorErase(i, typeFlag);
		}
		BB_SPISectorErase(spiSize-0x1000, typeFlag);
	}
	else if(spiSize == (1024*1024)) {
		/* only erase 256*3KB */
		temp1 = (spiSize / 0x10000)-2;
		for (i = 0; i < temp1; i++) {
			BB_SPI64KBBlockErase(i*0x10000, typeFlag);
		}
		BB_SPI32KBBlockErase((temp1)*0x10000, typeFlag);
		BB_SPISectorErase(spiSize-0x1000, typeFlag);
	}
}

/*--------------------------------------------------------------------------*
* Function name :BB_WrSPIFlash
* Function		: for writing data to serial flash through I2C 
* Return value	: void
* Parameter1    : size (UINT32)
* History	   	: 
*                	:  2018.06.04  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void BB_WrSPIFlash(UINT32 size)
{
	UINT32 fd, fileSize, id, type, pages, spiSize;
	UINT8* pbootBuf = NULL;

	printf("loadcode from file\n");
	/*#define FILE_PATH_BOOTCODE "D:\\FPGA.IME"*/
	#define FILE_PATH_BOOTCODE "D:\\DCIM\\BOOT.BIN"

	fd = sp5kFsFileOpen((const UINT8 *)FILE_PATH_BOOTCODE, SP5K_FS_OPEN_RDONLY);
	BB_I2CDataWrite(0x70c4, 0x00);
	BB_I2CDataWrite(0x70c5, 0x00);

	BB_I2CDataWrite(0x1011, 0x01);
	BB_I2CDataWrite(0x001C, 0x08);
	BB_I2CDataWrite(0x001C, 0x00);
	BB_I2CDataWrite(0x108C, 0x00);
	BB_I2CDataWrite(0x009a, 0x00);

	if (fd != 0) {
		fileSize = sp5kFsFileSizeGet(fd);
		printf("fileSize:0x%x\n", fileSize);
		pbootBuf = (UINT8*)sp5kMalloc(fileSize);
		sp5kFsFileRead(fd, pbootBuf, fileSize);
		printf("pbootBuf:0x%x\n", (UINT32)pbootBuf);
		fsFileClose(fd);
	}
	else {
		printf("file open error\n");
		return;
	}

	if (pbootBuf == NULL) {
		printf("buffer allocate failed\n");
		return;
	}
#if 0
	pbootBuf += 0x10;
	fileSize -= 0x10;
#endif
	BB_SPIInit();
	id = BB_SPIFlashReadId();
	if (id == 0) {
		printf("read id failed\n");
	 	return;
	}
	/*printf("spiSize:0x%x\n", &spiSize);*/
	type = BB_SerialFlashTypeCheck(id, &spiSize);
	if (type == 0) {
		printf("read id failed\n");
	 	return;
	}

	if (size > 0 && size < fileSize) {
		pages = size/0x100;
		if ((size%0x100) != 0)
			pages += 1;
	}
	else {
		pages = fileSize/0x100;
	}
	/*printf("pages:0x%x\n", pages);*/

	BB_EraseSPIFlash(type, spiSize);
	if (type == 2) {
		printf("SST operation\n");
		BB_SPISstFlashWrite(0, pages, pbootBuf);
	}
	else if (type == 1 || type == 3) {
		printf("ST operation\n");
		/*I2C_SPIFlashWrite(0, pages, pbootBuf);*/
		BB_SPIFlashWrite_DMA(0, pages, 1, pbootBuf);
	}
	BB_I2CDataWrite(0x70f8,0x08);
#if 0
	pbootBuf -= 0x10;
#endif
	sp5kFree(pbootBuf);
}


/*--------------------------------------------------------------------------*
* Function name :BB_RdSPIFlash
* Function		: for reading data to serial flash through I2C
* Return value	: void
* Parameter1    : addr (UINT32)
* Parameter2    : size (UINT32)
* History	   	: 
*                	:  2018.06.04  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void BB_RdSPIFlash(UINT32 addr, UINT32 size)
{
	UINT32 id;
	UINT8* pbootBuf = NULL;

	BB_I2CDataWrite(0x70c4, 0x00);
	BB_I2CDataWrite(0x70c5, 0x00);
	BB_I2CDataWrite(0x1011,0x01);
	BB_I2CDataWrite(0x001C,0x08);
	BB_I2CDataWrite(0x001C,0x00);
	BB_I2CDataWrite(0x108C,0x00);
	BB_I2CDataWrite(0x009A,0x00);

	BB_SPIInit();
	id = BB_SPIFlashReadId();
	if (id == 0) {
		printf("read id failed\n");
		return;
	}

	printf("size:0x%x\n", size);
	pbootBuf = sp5kMalloc(size);
	printf("pbootBuf:0x%x\n", (UINT32)pbootBuf);
	/*I2C_SPIFlashRead(0, pages, pbootBuf);*/
	BB_SPIFlashRead_DMA(addr, size, pbootBuf);

	#if 1	/* save file for verification */
	UINT32 fd;
	fd = sp5kFsFileOpen((const UINT8 *)"D:\\BOOTRD.BIN", SP5K_FS_OPEN_CREATE);
	if (fd){
		sp5kFsFileWrite(fd, pbootBuf, size);
		sp5kFsFileClose(fd);
		printf("save boot.bin\n");
	}
	#endif

	sp5kFree(pbootBuf);
}

