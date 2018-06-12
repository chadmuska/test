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
 *  File name	: 	app_i2c_lib.c
 *  Function 	: 	i2c library	 	
 *  First editor	: 	
 *  First date  	: 	
 *  History		:
 *  2018.04.16  Paul.Chang     "Add description"
 *--------------------------------------------------------------------------*/

#define HOST_DBG 1

#include <stdio.h>

#include "common.h"
#include "sp5k_global_api.h"

#include "app_i2c_lib.h"
#include "app_i2c_util.h"


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define SPI_CMD_BYTE_READ 			0x03
#define SPI_CMD_RD_ID 				0x9F
#define SPI_CMD_WRT_EN				0x06
#define SPI_CMD_BYTE_PROG 			0x02
#define SPI_CMD_RD_STS 				0x05
#define SPI_CMD_BYTE_PROG_AAI 		0xAD
#define SPI_CMD_WRT_STS_EN  		0x50
#define SPI_CMD_WRT_STS 			0x01
#define SPI_CMD_WRT_DIS 			0x04
#define SPI_CMD_ERASE_ALL			0xC7
#define SPI_CMD_SECTOR_ERASE		0x20
#define SPI_CMD_32KB_BLOCK_ERASE	0x52
#define SPI_CMD_64KB_BLOCK_ERASE	0xD8

#define WAIT_COUNT 1000

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
extern void ros_thread_sleep(UINT32 ms);
/*
extern void sensorSingleWrite(UINT16 addr,UINT16 data);
extern UINT16 sensorRead(UINT16 addr);
*/
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/

/*--------------------------------------------------------------------------*
* Function name : BB_SPIInit
* Function		: SPI Initial
* Return value	: void
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_SPIInit(void)
{
	BB_I2CDataWrite(0x0026, 0xc0);
	BB_I2CDataWrite(0x4051, 0x01); /* spien */
	BB_I2CDataWrite(0x40e1, 0x00); /* spi mode */
	BB_I2CDataWrite(0x40e0, 0x11); /* spi freq */
}


/*--------------------------------------------------------------------------*
* Function name : BB_SPIFlashPortWait
* Function		: wait spi flash time out
* Return value	: result (UINT32 SUCCESS/FAIL)
*				    SUCCESS : normal
*                                  FAIL : if wait spi flash time out
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT32 BB_SPIFlashPortWait(void)
{
#if 0
	UINT32 cnt = WAIT_COUNT;

	while (BB_I2CDataRead(0x40e6) != 0x00) {
		cnt--;
		if (cnt == 0x00) {
			printf("serial flash port wait time out!!\n");
			return FAIL;
		}
	}
#endif
	return SUCCESS;
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPIFlashPortRead
* Function		: SPI Flash Read
* Return value	: result (UINT32 SUCCESS/FAIL)
*				    SUCCESS : normal
*                                  FAIL : if wait spi flash time out
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT32 BB_SPIFlashPortRead(void)
{
	UINT32 ret;

	ret = BB_I2CDataRead(0x40e4);
	/* polling SPI state machine ready */
	if (BB_SPIFlashPortWait() != SUCCESS) {
        return FAIL;
    }
	ret = BB_I2CDataRead(0x40e5);

	return ret;
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPIFlashPortWrite
* Function		: SPI Flash write
* Return value	: result (UINT32 SUCCESS/FAIL)
*				    SUCCESS : normal
*                                  FAIL : if wait spi flash time out
* Parameter1	: Data (UINT32)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
UINT32 BB_SPIFlashPortWrite(UINT32 wData)
{
    BB_I2CDataWrite(0x40e3, (UINT8)wData);
	return BB_SPIFlashPortWait();
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPIFlashReadId
* Function		: SPI Flash Read ID
* Return value	: result (UINT32 SUCCESS/FAIL)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

UINT32 BB_SPIFlashReadId(void)
{
    UINT32 ID, err;
	UINT8 id[3] = {0, 0, 0};

	BB_I2CDataWrite(0x40e7, 0x00);

	err = BB_SPIFlashPortWrite(SPI_CMD_RD_ID); /*read ID command*/
	if (err != SUCCESS) {
        printf("Get serial flash ID failed\n");
        return FAIL;
    }

	id[0] = BB_SPIFlashPortRead();    /* Manufacturer's  ID */
    id[1] = BB_SPIFlashPortRead();    /* Device ID          */
    id[2] = BB_SPIFlashPortRead();    /* Manufacturer's  ID */

	BB_I2CDataWrite(0x40e7, 0x01);

	printf("ID %2x %2x %2x\n", id[0], id[1], id[2]);
	ID = ((UINT32)id[0] << 16) | ((UINT32)id[1] << 8) | ((UINT32)id[2] << 0);
	return ID;
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPIFlashWrEnable
* Function		: SPI Flash write enable
* Return value	: void
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_SPIFlashWrEnable(void)
{
	BB_I2CDataWrite(0x40e7, 0x00);
    BB_SPIFlashPortWrite(SPI_CMD_WRT_EN);
    BB_I2CDataWrite(0x40e7, 0x01);
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPIStsRegRead
* Function		: SPI state register read
* Return value	: result (UINT32 SUCCESS/FAIL)
* Parameter1	: void
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
UINT32 BB_SPIStsRegRead(void)
{
	UINT32 ret;

	BB_I2CDataWrite(0x40e7, 0x00);

	BB_SPIFlashPortWrite(SPI_CMD_RD_STS);
    ret = BB_SPIFlashPortRead();

    BB_I2CDataWrite(0x40e7, 0x01);

    return ret;
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPITimeOutWait
* Function		: SPI wait time out 
* Return value	: void
* Parameter1    : poll (UINT32)
* Parameter2    : timeOut (UINT32)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_SPITimeOutWait(UINT32 poll, UINT32 ptimeOut)
{
	/* MAX_TIME for SECTOR/BLOCK ERASE is 25ms */
	UINT32 time = 0;

	while (1) {
		UINT32 sts;
		sts = BB_SPIStsRegRead();
		if (!(sts & poll)) { /* sfStatusRead() > 4.8us */
            break;
        }
		time++;
		if (ptimeOut < time) {
            printf("TimeOut %u, sts=0x%x, poll=0x%x\n", time, sts, poll);
            break;
        }
	}
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPISectorErase
* Function		: SPI sector erase
* Return value	: void
* Parameter1    : address (UINT32)
* Parameter2    : stFlag(UINT32)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_SPISectorErase(UINT32 address, UINT32 stFlag)
{
	UINT32 timeout = 5000000;

	printf("addr: 0x%x\n", address);
	if (!stFlag) {
		BB_SPIFlashWrEnable();

		BB_I2CDataWrite(0x40e7, 0x00);
		BB_SPIFlashPortWrite(SPI_CMD_WRT_STS_EN); /* Write Status register command */
		BB_I2CDataWrite(0x40e7, 0x01);
	}

	BB_I2CDataWrite(0x40e7, 0x00);
	BB_SPIFlashPortWrite(SPI_CMD_WRT_STS); /* Write Status register command */
	BB_SPIFlashPortWrite(0x02);
	BB_I2CDataWrite(0x40e7, 0x01);

	BB_SPIFlashWrEnable();
	BB_I2CDataWrite(0x40e7, 0x00);
	BB_SPIFlashPortWrite(SPI_CMD_SECTOR_ERASE);
	BB_SPIFlashPortWrite(address >> 16);	/* A23~A16 */
	BB_SPIFlashPortWrite(address >> 8);		/* A15~A08 */
	BB_SPIFlashPortWrite(address);			/* A07~A00 */
	BB_I2CDataWrite(0x40e7, 0x01);

	BB_SPITimeOutWait(0x01, timeout);
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPI32KBBlockErase
* Function		: SPI 32KB Block erase
* Return value	: void
* Parameter1    : address (UINT32)
* Parameter2    : stFlag(UINT32)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_SPI32KBBlockErase(UINT32 address, UINT32 stFlag)
{
	UINT32 timeout = 5000000;

	printf("addr: 0x%x\n", address);
	if (!stFlag) {
		BB_SPIFlashWrEnable();

		BB_I2CDataWrite(0x40e7, 0x00);
		BB_SPIFlashPortWrite(SPI_CMD_WRT_STS_EN); /* Write Status register command */
		BB_I2CDataWrite(0x40e7, 0x01);
	}

	BB_I2CDataWrite(0x40e7, 0x00);
	BB_SPIFlashPortWrite(SPI_CMD_WRT_STS); /* Write Status register command */
	BB_SPIFlashPortWrite(0x02);
	BB_I2CDataWrite(0x40e7, 0x01);

	BB_SPIFlashWrEnable();
	BB_I2CDataWrite(0x40e7, 0x00);
	BB_SPIFlashPortWrite(SPI_CMD_32KB_BLOCK_ERASE);
	BB_SPIFlashPortWrite(address >> 16);	/* A23~A16 */
	BB_SPIFlashPortWrite(address >> 8);		/* A15~A08 */
	BB_SPIFlashPortWrite(address);			/* A07~A00 */
	BB_I2CDataWrite(0x40e7, 0x01);

	BB_SPITimeOutWait(0x01, timeout);
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPI64KBBlockErase
* Function		: SPI 64KB Block erase
* Return value	: void
* Parameter1    : address (UINT32)
* Parameter2    : stFlag(UINT32)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_SPI64KBBlockErase(UINT32 address, UINT32 stFlag)
{
	UINT32 timeout = 5000000;

	printf("addr: 0x%x\n", address);
	if (!stFlag) {
		BB_SPIFlashWrEnable();

		BB_I2CDataWrite(0x40e7, 0x00);
		BB_SPIFlashPortWrite(SPI_CMD_WRT_STS_EN); /* Write Status register command */
		BB_I2CDataWrite(0x40e7, 0x01);
	}

	BB_I2CDataWrite(0x40e7, 0x00);
	BB_SPIFlashPortWrite(SPI_CMD_WRT_STS); /* Write Status register command */
	BB_SPIFlashPortWrite(0x02);
	BB_I2CDataWrite(0x40e7, 0x01);

	BB_SPIFlashWrEnable();
	BB_I2CDataWrite(0x40e7, 0x00);
	BB_SPIFlashPortWrite(SPI_CMD_64KB_BLOCK_ERASE);
	BB_SPIFlashPortWrite(address >> 16);	/* A23~A16 */
	BB_SPIFlashPortWrite(address >> 8);		/* A15~A08 */
	BB_SPIFlashPortWrite(address);			/* A07~A00 */
	BB_I2CDataWrite(0x40e7, 0x01);

	BB_SPITimeOutWait(0x01, timeout);
}

/*--------------------------------------------------------------------------*
* Function name : BB_7002DmemWr
* Function		: 7002D memory write
* Return value	: void
* Parameter1    : bankNum (UINT32)
* Parameter2    : byteNum (UINT32)
* Parameter3    : buffer pointer(UINT8 pointer)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_7002DmemWr(UINT32 bankNum, UINT32 byteNum, UINT8* pbuf)
{
	UINT32 i, bank;

	bank = 0x40 + bankNum;
	BB_I2CDataWrite(0x10A6, bank);

	for (i = 0; i < byteNum; i+=8) {
		seqI2CDataWrite((0x1800+i), (pbuf+i));
	}

	bank = 0x40 + ((bankNum+1)%2);
	BB_I2CDataWrite(0x10A6, bank);
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPIFlashWrite_DMA
* Function		: SPI Flash write DMA
* Return value	: void
* Parameter1    : addr (UINT32)
* Parameter2    : pages (UINT32)
* Parameter3    : usage (UINT32)
*				1 will skip writing reserved block of calibration data
*    				0 will write reserved block of calibration data
* Parameter4    : buffer pointer(UINT8 pointer)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/

void BB_SPIFlashWrite_DMA(UINT32 addr, UINT32 pages, UINT32 usage, UINT8 *pbuf)
{
	UINT32 pageSize = 0x100;
	UINT32 rsvSec1, rsvSec2, chk1=0, chk2=0, dmemBank=0, count, size, temp, i;

	rsvSec1 = pages*pageSize - 0x7000;
	rsvSec2 = pages*pageSize - 0x1000;
	addr = addr * pageSize;

	/* Set DMA bytecnt as 256-1 */
	BB_I2CDataWrite(0x4170, 0xff);
	BB_I2CDataWrite(0x4171, 0x00);
	BB_I2CDataWrite(0x4172, 0x00);

	/* Set DMA bank & DMA start address */
	BB_I2CDataWrite(0x1084, 0x01);
	BB_I2CDataWrite(0x1080, 0x00);
	BB_I2CDataWrite(0x1081, 0x00);
	BB_I2CDataWrite(0x1082, 0x00);

	/* enable DMA checksum and reset checksum */
	BB_I2CDataWrite(0x4280, 0x01);
	BB_I2CDataWrite(0x4284, 0x00);
	BB_I2CDataWrite(0x4285, 0x00);
	BB_I2CDataWrite(0x4164, 0x00);

	size = pages * pageSize;
	for (i = 0; i < size; i++) {
		if ((i >= rsvSec2) || (i < rsvSec1)) {
			chk1 += *(pbuf+i);
		}
		if (chk1 >= 0x10000) {
			chk1 -= 0x10000;
		}
	}

	while (pages) {
		if ((pages%0x40) == 0) {
			printf("page: 0x%x\n", pages);
		}

		if ((addr >= rsvSec1) && (addr <rsvSec2) && (usage != 0)) {
			addr += 0x1000;
			pbuf += 0x1000;
			pages -= 0x10;
			continue;
		}

		if (pages == 1) {
			for (i = 0; i < pageSize; i++) {
            	printf("%2x ", *(pbuf+i));
				if ((i%0x10) == 0x0f)
					printf("\n");
       		}
		}

		dmemBank = pages % 2;
		BB_I2CDataWrite(0x1081, dmemBank*0x20);
		BB_I2CDataWrite(0x1084, (1<<dmemBank));
		BB_7002DmemWr(dmemBank, pageSize, pbuf);

		BB_SPIFlashWrEnable();
        BB_I2CDataWrite(0x40e7, 0x00);
        BB_SPIFlashPortWrite(SPI_CMD_BYTE_PROG); /* Write one byte command */
        BB_SPIFlashPortWrite((UINT8)(addr >> 16)); /* Send 3 bytes address */
        BB_SPIFlashPortWrite((UINT8)(addr >> 8));
        BB_SPIFlashPortWrite((UINT8)(addr));

		BB_I2CDataWrite(0x4160, 0x01);
		count = 30;
		pbuf += pageSize;
        addr += pageSize;
        pages--;

		while (BB_I2CDataRead(0x4003) == 0) {
			count--;
			sp5kTimeDelay(SP5K_TIME_DELAY_1US, 200); /* wait for DMA done */
			if (count == 0) {
				printf("DMA time out: %2x, 0x%2x%2x, %2x\n", pages,
					BB_I2CDataRead(0x4179), BB_I2CDataRead(0x4178), BB_I2CDataRead(0x40E6));
				BB_I2CDataWrite(0x4011, 0x10);
				BB_I2CDataWrite(0x1010, 0x02);
				pbuf -= pageSize;
        		addr -= pageSize;
        		pages++;
				BB_I2CDataWrite(0x1010, 0x00);
				break;
			}
		}
		BB_I2CDataWrite(0x4003, 0x02);
		BB_I2CDataWrite(0x40e7, 0x01);
	}


	sp5kTimeDelay(SP5K_TIME_DELAY_1US, 500); /* wait for DMA done */

	temp = BB_I2CDataRead(0x4285);
	chk2 = BB_I2CDataRead(0x4284);
	chk2 = chk2 | (temp<<8);
	printf("checksum: 0x%x 0x%x\n", chk1, chk2);
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPISstStatusWrite
* Function		: SPI sst status write
* Return value	: void
* Parameter1    : data (UINT8)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_SPISstStatusWrite(UINT8 dat)
{
	UINT32 timeout = 100000, poll = 0x01;

	BB_SPIFlashWrEnable();
	BB_I2CDataWrite(0x40e7, 0x00);
	BB_SPIFlashPortWrite(SPI_CMD_WRT_STS_EN);
	BB_I2CDataWrite(0x40e7, 0x01);

	BB_I2CDataWrite(0x40e7, 0x00);
    BB_SPIFlashPortWrite(SPI_CMD_WRT_STS);
    BB_SPIFlashPortWrite(dat);
    BB_I2CDataWrite(0x40e7, 0x01);
#if 0
	if (spiDev.bus != SPI_1BIT_MODE) { /* 1 bit mode */
		poll = 0x80;
	}
	else {
		poll = 0x01;
	}
#endif
	BB_SPITimeOutWait(poll, timeout);
}

/*--------------------------------------------------------------------------*
* Function name : BB_SPISstFlashWrite
* Function		: SPI sst Flash write
* Return value	: void
* Parameter1    : addr (UINT32)
* Parameter2    : pages (UINT32)
* Parameter3    : buffer pointer(UINT8 pointer)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_SPISstFlashWrite(UINT32 addr, UINT32 pages, UINT8 *pbuf)
{
	UINT32 pageSize = 0x100;
    UINT32 timeout = 100000;
	UINT32 i;

	addr = addr * pageSize;

	printf("SST type writing...\n");
	BB_SPISstStatusWrite(0x40);
	while (pages) {
		printf("page: 0x%x\n", pages);
		if ((addr >= 0x7C000) && (addr < 0x7F000)) {
			addr += 0x1000;
			pages -= 0x10;
			continue;
		}
		BB_SPIFlashWrEnable();
		BB_I2CDataWrite(0x40e7, 0x00);
		BB_SPIFlashPortWrite(SPI_CMD_BYTE_PROG_AAI); /* Write one byte command */
		BB_SPIFlashPortWrite((UINT8)(addr >> 16)); /* Send 3 bytes address */
		BB_SPIFlashPortWrite((UINT8)(addr >> 8));
		BB_SPIFlashPortWrite((UINT8)(addr));
		BB_SPIFlashPortWrite(*pbuf);
		pbuf++;
		BB_SPIFlashPortWrite(*pbuf);
		pbuf++;
		BB_I2CDataWrite(0x40e7, 0x01);
		BB_SPITimeOutWait(0x01, timeout);

		for (i = 2; i < pageSize; i+=2) {
			BB_I2CDataWrite(0x40e7, 0x00);
			BB_SPIFlashPortWrite(SPI_CMD_BYTE_PROG_AAI);
			BB_SPIFlashPortWrite(*pbuf);
			pbuf++;
			BB_SPIFlashPortWrite(*pbuf);
			pbuf++;
			BB_I2CDataWrite(0x40e7, 0x01);
			BB_SPITimeOutWait(0x01, timeout);
		}

		BB_I2CDataWrite(0x40e7, 0x00);
		BB_SPIFlashPortWrite(SPI_CMD_WRT_DIS);
		BB_I2CDataWrite(0x40e7, 0x01);

		addr += pageSize;
		pages--;

		BB_I2CDataWrite(0x40e7, 0x00);
		BB_SPIFlashPortWrite(SPI_CMD_WRT_DIS);
		BB_I2CDataWrite(0x40e7, 0x01);
	}
}

/*--------------------------------------------------------------------------*
* Function name : BB_7002DmemRd
* Function		: 7002D mem read
* Return value	: void
* Parameter1    : bankNum(UINT32)
* Parameter2    : byteNum (UINT32)
* Parameter3    : buffer pointer(UINT8 pointer)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_7002DmemRd(UINT32 bankNum, UINT32 byteNum, UINT8* pbuf)
{
	UINT32 i, bank;

	bank = 0x40 + bankNum;
	BB_I2CDataWrite(0x10A6, bank);

	for (i = 0; i < byteNum; i+=8) {
		seqI2CDataRead((0x1800+i), (pbuf+i));
	}

	bank = 0x40 + ((bankNum+1)%2);
	BB_I2CDataWrite(0x10A6, bank);
}

/*--------------------------------------------------------------------------*
* Function name :BB_SPIFlashRead_DMA
* Function		: SPI Flash DMA Read
* Return value	: void
* Parameter1    : adr (UINT32)
* Parameter2    : size (UINT32)
* Parameter3    : buffer pointer (UINT8 pointer)
* History	   	: 
*                	:  2018.06.01  Paul.Chang     "Add function description"
*--------------------------------------------------------------------------*/
void BB_SPIFlashRead_DMA(UINT32 addr, UINT32 size, UINT8 *pbuf)
{
	UINT32 pageSize = 0x100;
	UINT32 tempSize, count=0;

	/* Set DMA bytecnt as 256-1 */
	BB_I2CDataWrite(0x4170, 0xff);
	BB_I2CDataWrite(0x4171, 0x00);
	BB_I2CDataWrite(0x4172, 0x00);

	/* Set DMA bank & DMA start address */
	BB_I2CDataWrite(0x1084, 0x01);
	BB_I2CDataWrite(0x1080, 0x00);
	BB_I2CDataWrite(0x1081, 0x00);
	BB_I2CDataWrite(0x1082, 0x00);

	/* enable DMA checksum and reset checksum */
	BB_I2CDataWrite(0x4280, 0x01);
	BB_I2CDataWrite(0x4284, 0x00);
	BB_I2CDataWrite(0x4285, 0x00);
	BB_I2CDataWrite(0x4164, 0x01);

	while (size) {
		UINT32 dmemBank;
		if (size > pageSize) {
			tempSize = 0x100;
			size -= tempSize;
		}
		else {
			tempSize = size;
			size = 0;
		}

		BB_I2CDataWrite(0x40e7, 0x00);
		BB_SPIFlashPortWrite(SPI_CMD_BYTE_READ); /* Write one byte command */
		BB_SPIFlashPortWrite((UINT8)(addr >> 16)); /* Send 3 bytes address */
		BB_SPIFlashPortWrite((UINT8)(addr >> 8));
		BB_SPIFlashPortWrite((UINT8)(addr));

		if (((size/0x100)%0x40) == 0x00) {
			printf("RE:0x%x\n", ((size/0x100)%0x40));
		}
		dmemBank = count % 2;
		BB_I2CDataWrite(0x1081, dmemBank*0x20);
		BB_I2CDataWrite(0x1084, (1<<dmemBank));
    	BB_I2CDataWrite(0x4160, 0x01);
		sp5kTimeDelay(SP5K_TIME_DELAY_1US, 100);
    	BB_I2CDataWrite(0x40e7, 0x01);
		BB_7002DmemRd(dmemBank, tempSize, pbuf);

		pbuf += pageSize;
		addr += pageSize;
		count++;
	}
}

