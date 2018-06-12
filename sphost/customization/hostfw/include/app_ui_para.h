/**************************************************************************
 *         Copyright(c) 2012 by iCatch Technology Co., Ltd.              *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch Tech-  *
 *  nology Co., Ltd. All rights are reserved by iCatch Technology Co.,   *
 *  Ltd. This software may only be used in accordance with the            *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice "M U S T" not be removed or modified without    *
 *  prior written consent of iCatch Technology Co., Ltd.                 *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                          *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *************************************************************************/

#ifndef __APP_UI_PARA_H__
#define __APP_UI_PARA_H__

#if defined(PRJ_SBC)
#include "app_ui_para_sbc.h"
#elif (defined(PRJ_EVB))
#include "app_ui_para_evb.h"
#elif (defined(PRJ_DUAL2))
#include "app_ui_para_dual2.h"
#elif (defined(PRJ_17DVR))
#include "app_ui_para_17dvr.h"
#elif (defined(PRJ_JETDVR))
#include "app_ui_para_jetdvr.h"
#elif (defined(PRJ_PDVR))
#include "app_ui_para_pdvr.h"
#elif (defined(PRJ_19DVR))
#include "app_ui_para_19dvr.h"
#else
#error Unsupported ui para Configured !
#endif


void appUiParaRevert2Base(void);
uiPara_t*  appUiParaBaseGet(void);
uiPara_t* appUiParaGet(void);
void appUIParaReset(void);

#endif /*__APP_UI_PARA_H__*/
