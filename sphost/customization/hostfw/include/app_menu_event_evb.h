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
/* Created at January,08,2018 05:40:35 */
#ifndef _MENU_API_EVB_H_
#define _MENU_API_EVB_H_

#include "app_com_def.h"

typedef UINT32 (*menufunc) (UINT8);
typedef UINT32 (*menuexe) (UINT8, UINT8, UINT32, UINT32);

UINT32 appMenuEvt_ModeStill (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_FisrstItemProcess (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ImageSizeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ImageQualitySet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_PhotoBurstSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_LastItemProcess (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_StillStampModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeVideo (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_VideoSizeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_VideoQualitySet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SeamlessSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_VideoStampModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModePlayback (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_PbFileProtect (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_PbFileDelete (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ModeSetup (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SoundInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SoundSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_VideoWBModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_LightFreqSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SleepTimeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_powerOnRecordSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_PeriodicalCapSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_RTCInit (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_RTCSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_LanguageSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_TVModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_USBModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_Format (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_ResetAll (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_SceneModeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);
UINT32 appMenuEvt_FileTypeSet (UINT8 layer, UINT8 item, UINT32 msg, UINT32 para);

UINT32 appMenuAct_ImageSize (UINT8 item);
UINT32 appMenuAct_VideoSizeSet (UINT8 item);
UINT32 appMenuAct_TVMode (UINT8 item);

#endif /* _MENU_API_H_ */

