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
 ****************************************************************************/
#ifndef APP_I2C_UTIL_H
#define APP_I2C_UTIL_H


/****************************************************************************
 *						C O N S T A N T S 										*
 ****************************************************************************/

/****************************************************************************
 *						D A T A    T Y P E S									*
 ****************************************************************************/
typedef enum {
	_AE_LUCK_			= 0x01,
	_AWB_LUCK_			= _AE_LUCK_,
	_AE_UNLUCK_			= 0x02,
	_AWB_UNLUCK_		= _AE_UNLUCK_,
	_AE_SET_EXP_LINES_	= 0x04,
	_AE_SET_AGC_		= 0x06,
} VQVendreqParamgroup_e;

typedef enum {
	PVSIZE_1280_960 = 0,
	PVSIZE_3264_2448,
	PVSIZE_1920_1080,
	PVSIZE_320_240,
	PVSIZE_1280_720,
	PVSIZE_1040_780,
	PVSIZE_2080_1560,
	PVSIZE_3648_2736,
	PVSIZE_4160_3120,
	PVSIZE_3360_1890,
	PVSIZE_2592_1944,
	PVSIZE_640_480,
} pvSizeList_e;

typedef enum {
	YUV2RAW = 0x01,
	RAW2YUV = 0x02,
} RawCalibFlag_e;

typedef enum {
	YUV_CALIB_START = 0x01,
	YUV_CALIB_END = 0x02,
} YuvCalibFlag_e;

/****************************************************************************
 *						 M A C R O S											*
 ****************************************************************************/

/****************************************************************************
 *						 G L O B A L    D A T A									*
 ****************************************************************************/

/****************************************************************************
 *						F U N C T I O N    D E C L A R A T I O N S					*
 ****************************************************************************/
void seqI2CDataRead(UINT32 a_InputAddr, UINT8* pBuf);
void seqI2CDataWrite(UINT32 a_InputAddr, UINT8* pBuf);

UINT32 BB_SerialFlashTypeCheck(UINT32 id, UINT32 *spiSize);
void BB_EraseSPIFlash(UINT32 type, UINT32 spiSize);
void BB_WrSPIFlash(UINT32 size);
void BB_RdSPIFlash(UINT32 addr, UINT32 size);

void BB_I2CDataWrite(UINT32 a_udwAddr, UINT32 a_udwData);
UINT32 BB_I2CDataRead(UINT32 a_udwAddr);

#endif  /* APP_I2C_UTIL_H */

