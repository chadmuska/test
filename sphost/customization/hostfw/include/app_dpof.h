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
#ifndef _APP_DPOF_H_
#define _APP_DPOF_H_
#include "sp5k_dpof_api.h"
/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/


/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
#define APP_DPOF_CRT_NAME		"DigitalCAM"
#define DPOF_FILENAME_NAND	"C:\\MISC\\AUTPRINT.MRK"
#define DPOF_FILENAME_SDCARD   "D:\\MISC\\AUTPRINT.MRK"

 typedef enum {
	APP_DPOF_MODE_STD  = SP5K_DPOF_PRINT_MODE_STD,  
	APP_DPOF_MODE_IDX   = SP5K_DPOF_PRINT_MODE_IDX,         
	APP_DPOF_MODE_SIZ   = SP5K_DPOF_PRINT_MODE_SIZ,	
} appDpofMode_e;

 typedef enum {
 	APP_DPOF_JOB_CURR = SP5K_DPOF_DEL_CUR,
	APP_DPOF_JOB_ALL = SP5K_DPOF_DEL_ALL,
 } appDpofJobOption_e;

 typedef enum {
 	APP_DPOF_PRINT_SIZE_UNKNOW = 0,
	APP_DPOF_PRINT_SIZE_3x5   = 3,  
	APP_DPOF_PRINT_SIZE_4x6   = 4,         
	APP_DPOF_PRINT_SIZE_5x7   = 5,	
	APP_DPOF_PRINT_SIZE_8x10 = 8,	
} appDpofPrintSize_e;
/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 
/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/


/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

void appDpofInit(void);
void appDpofClose(BOOL write2File);
void appDpofFileRemove(UINT32 isCardUsed);
BOOL appDpofJobExistQuery(appDpofMode_e dpofMode);
UINT32 appDpofPrintQuantityGet(appDpofMode_e dpofMode);
UINT32 appDpofDateStampGet(appDpofMode_e dpofMode, UINT8 *pszDTBuf);
BOOL appDpofDateStampExistQuery(appDpofMode_e dpofMode);
UINT32 appDpofPrintSizeGet(void);
UINT32 appDpofJobDelete(appDpofMode_e dpofMode,appDpofJobOption_e jobOption);
UINT32 appDpofStdJobAdd(UINT32 printQty,	UINT8 *dateStamp);
UINT32 appDpofFileWrite(void);
#endif  /* _APP_DPOF_H_ */

