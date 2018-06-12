/**************************************************************************
 *
 *       Copyright (c) 2004-2016 by iCatch Technology, Inc.
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
 *                                                                        *
 *  Author:                                                               *
 *                                                                        *
 **************************************************************************/
#ifndef _CMD_API_H_
#define _CMD_API_H_

#include "common.h"

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void cmdDefaultSet(UINT32);
void cmdCUnitInit(void);
void cmdUsbInit(void);
void cmdGpeInit(void);
void cmdMemInit(void);
void cmdOsInit(void);
void cmdDosInit(void);
void cmdErrInit(void);
void cmdGlobalInit(void);
void cmdMediaInit(void);
void cmdIspInit(void);
void cmdHwInit(void);
void cmdSnapInit(void);
void cmdIcatInit(void);
void cmdNandInit(void);
void cmdDiskInit(void);
void cmdSpiInit(void);
void cmdLmiInit(void);
void cmdSdInit(void);
void cmdGpioInit(void);
void cmdAaaInit(void);
void cmdApiMonInit(void);
void cmdRawYuvInit(void);
void cmdCdspInit(void);
void cmdOsdInit(void);
void cmdDispInit(void);
/*j.li add for usbhost*/
void cmdUsbHostInit(void);
void cmdUsbHostTestInit(void);
void cmdEhciTestInit(void);
void cmdOhciTestInit(void);
/*j.li add end*/
void cmdModeswInit(void);
void cmdFrontInit(void);
void cmdSofyGyroInit(void);
void cmdKernInit(void);	/*add for kernel load*/
void cmdAudioTestInit(void);
void cmdDramTestInit(void);

#endif  /* _CMD_API_H_ */
