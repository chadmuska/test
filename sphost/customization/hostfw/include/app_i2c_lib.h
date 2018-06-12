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
#ifndef APP_I2C_LIB_H
#define APP_I2C_LIB_H


/****************************************************************************
 *						C O N S T A N T S 										*
 ****************************************************************************/

/****************************************************************************
 *						D A T A    T Y P E S									*
 ****************************************************************************/

/****************************************************************************
 * 					  M A C R O S											 *
 ****************************************************************************/

/****************************************************************************
 * 					  G L O B A L    D A T A									 *
 ****************************************************************************/

/****************************************************************************
 * 					 F U N C T I O N    D E C L A R A T I O N S 				 *
 ****************************************************************************/
void BB_SPIInit(void);
UINT32 BB_SPIFlashPortWait(void);
UINT32 BB_SPIFlashPortRead(void);
UINT32 BB_SPIFlashPortWrite(UINT32 wData);
UINT32 BB_SPIFlashReadId(void);
void BB_SPIFlashWrEnable(void);
UINT32 BB_SPIStsRegRead(void);
void BB_SPITimeOutWait(UINT32 poll, UINT32 ptimeOut);
void BB_SPISectorErase(UINT32 address, UINT32 stFlag);
void BB_SPI32KBBlockErase(UINT32 address, UINT32 stFlag);
void BB_SPI64KBBlockErase(UINT32 address, UINT32 stFlag);
void BB_7002DmemWr(UINT32 bankNum, UINT32 byteNum, UINT8* pbuf);
void BB_SPIFlashWrite_DMA(UINT32 addr, UINT32 pages, UINT32 usage, UINT8 *pbuf);
void BB_SPISstStatusWrite(UINT8 dat);
void BB_SPISstFlashWrite(UINT32 addr, UINT32 pages, UINT8 *pbuf);
void BB_7002DmemRd(UINT32 bankNum, UINT32 byteNum, UINT8* pbuf);
void BB_SPIFlashRead_DMA(UINT32 addr, UINT32 size, UINT8 *pbuf);

#endif  /* APP_I2C_LIB_H */

