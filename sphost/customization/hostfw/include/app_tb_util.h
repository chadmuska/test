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
#ifndef _APP_TB_UTIL_H_
#define _APP_TB_UTIL_H_

/**************************************************************************
 *                         H E A D E R   F I L E S                        *
 **************************************************************************/

//#include "app_osd.h"
//#include "app_osdfont.h"
//#include "app_osd_api.h"

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

/**************************************************************************
 *          M O D U L E   V A R I A B L E S   R E F E R E N C E S         *
 **************************************************************************/

/**************************************************************************
 *                               M A C R O S                              *
 **************************************************************************/

/**************************************************************************
 *                F U N C T I O N   D E C L A R A T I O N S               *
 **************************************************************************/
UINT32 tbLogFile_FdGet(void);
void tbLogFile_Print(const char *format, ...);
void tbLogFile_PrintEx(BOOL isWrFile, const char *format, ...);
UINT8 tbLogFile_Open(UINT8 *szLogFname);
void tbLogFile_Close(void);
void tbLogBuf_Clear(void);
void tbLogBuf_Add(UINT32 val);
UINT32 tbLogBuf_Get(void);
void tbLogBuf_PtrReset(void);
void tbLogBuf_Create(void);
void tbLogBuf_Destroy(void);

void tbUtil_PrintfCtrl(BOOL isOn);

void tbUtil_CalibCdspSet(BOOL isObOn);

void tbUtil_ViewEnter(void);
void tbUtil_ViewSet(void);
void tbUtil_AeAwb_Dark_Off(void);
void tbUtil_AeControl(BOOL isOn);
void tbUtil_CalibAeWeightCfg(UINT32 region);
void tbUtil_AwbControl(BOOL isOn);
void tbUtil_LSCCtrl(BOOL islscOn);
void tbUtil_ObApply(BOOL isObOn);
void tbUtil_ViewAgcLimtGet(UINT32 *pmax, UINT32 *pmin);
void tbUtil_SnapAgcLimtGet(UINT32 *pmax, UINT32 *pmin);
void tbUtil_ViewExpLimtGet(SINT32 *pmax, SINT32 *pmin);
void tbUtil_SnapExpLimtGet(SINT32 *pmax, SINT32 *pmin);
UINT16 tbUtil_PvRawLumaGet(void);
UINT16 tbUtil_CapRawLumaGet(void);
BOOL tbUtil_CapRawSaturatedCheck(UINT16 sat_th, UINT16 *pluma);
BOOL tbUtil_PvRawSaturatedCheck(UINT16 sat_th, UINT16 *pluma);
UINT32 tbUtil_AgcApproachTest(BOOL isPvMode, UINT32 loBound, UINT32 hiBound, SINT32 expidx);
SINT32 tbUtil_AeAgcConverge(SINT32 expidx);
void tbUtil_QuickSweepHostMessage(UINT32 wait_ms);
UINT8 tbUtil_TestProlog(UINT8 *pszTest);
void tbUtil_Beep(void);
void tbUtil_BeepWarn(void);
void tbUtil_BeepOK(void);

UINT8 tbUtil_QueryIrisStop(void);
void tbUtil_MShutterCtrl(BOOL isClose);
void tbUtil_IrisCtrl(BOOL isLarge);

BOOL tbUtil_TestLoopAbort(void);

/*The Following Fuctions are Added by Brian.*/
void tbUtil_DataMinMaxFind(UINT32 *pData);
void tbUtil_DataNormal(UINT32 *pData, UINT32 NumData,UINT32 DataMax, UINT32 DataMin, UINT32 NorMaxX, UINT32 NorMaxY);
void tbUtil_DataCurveDraw(UINT32 *pData, UINT32 NumData);

/*End of Brian's*/

#endif  /* _APP_TB_UTIL_H_ */


