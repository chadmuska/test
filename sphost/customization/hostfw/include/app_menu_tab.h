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
/* Created at September,15,2017 14:53:49 */
#ifndef _MENU_TAB_H_
#define _MENU_TAB_H_

#if defined(PRJ_SBC)
#include "app_menu_tab_sbc.h"
#elif (defined(PRJ_EVB))
#include "app_menu_tab_evb.h"
#elif (defined(PRJ_DUAL2))
#include "app_menu_tab_dual2.h"
#elif (defined(PRJ_17DVR))
#include "app_menu_tab_17dvr.h"
#elif (defined(PRJ_JETDVR))
#include "app_menu_tab_jetdvr.h"
#elif (defined(PRJ_PDVR))
#include "app_menu_tab_pdvr.h"
#elif (defined(PRJ_19DVR))
#include "app_menu_tab_19dvr.h"
#else
#error Unsupported menu tab Configured !
#endif


#endif /* _MENU_TAB_H_ */
