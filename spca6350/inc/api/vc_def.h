/**************************************************************************
 *
 *       Copyright (c) 2002-2016 by iCatch Technology, Inc.
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
 *  Author: ZhongHai                                                      *
 *                                                                        *
 **************************************************************************/
#ifndef _VCMD_DEF_H_
#define _VCMD_DEF_H_

enum
{
    sp5kDcfFsFileOperate_CallBackFunc  = 0x00,
    sp5kUsbVendreqRead_CallBackFunc,
    sp5kUsbVendreqWrite_CallBackFunc,
    sp5kStillCapRaw_CallBackFunc,
    sp5kStillCapYUV_CallBackFunc,      
    sp5kStillCapBurst_CallBackFunc,      
    sp5kStillCapExifHdr_CallBackFunc,    
    sp5kGpioIsrReg_IsrFunc,
    sp5kAeCustom_CallBackFunc,
    sp5kDiskDetectPinCfgDir_CallBackFunc,
    sp5kDiskDetectPinCfgRd_CallBackFunc,
    sp5kDiskWpPinCfgDir_CallBackFunc,
    sp5kDiskWpPinCfgRd_CallBackFunc,
    sp5kDiskPowerPinCfgDir_CallBackFunc,
    sp5kDiskPowerPinCfgWr_CallBackFunc,
    sp5kDiskEnablePinCfgDir_CallBackFunc,
    sp5kDiskEnablePinCfgWr_CallBackFunc,
    sp5kDiskPwrRecycleFuncSet_CallBackFunc,
    sp5kMediaPlayPeriodical_CallBackFunc,
    sp5kAeCustomEarly_CallBackFunc,
    sp5kAwbCustom_CallBackFunc,
    sp5kAwbCustomEarly_CallBackFunc,
    sp5kSnapShutterCfgSet_CallBackFunc,    
    sp5kTimerIsrReg_IsrFunc,        /*xxian 2006/02/28*/
    sp5kAfCustom_CallBackFunc,      /*xxian 2006/05/20*/
    sp5kCntr_CallBackFunc,          /*xxian 2006/07/14*/
    sp5kStillCapRaw_CallBackFunc1,  /*xxian 2006/08/11*/
    MAX_CUSCALLBACKFUN /* must be at end */
};

#define MAX_CUSTASK_ENTRY     10

typedef void (*fpvcCusTaskEntry_t)(unsigned long);

UINT32 vcSetCustomTask(fpvcCusTaskEntry_t entry_function, char *name_ptr);
UINT32 vcSetCallbackFun(UINT32 funAddr, UINT32 idx);
void VC_init(UINT32 *pCallbackFunTab, UINT32 numCallbackFun);

UINT32 vcIsHostMsgToAPITest(void);
void vcHostMsgToAPITestEnable(UINT32 en);

#endif
