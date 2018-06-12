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
#ifndef _MENU_RES_H_
#define _MENU_RES_H_
    
    
#if defined(PRJ_SBC)
#include "app_menu_res_sbc.h"
#elif (defined(PRJ_EVB))
#include "app_menu_res_evb.h"
#elif (defined(PRJ_DUAL2))
#include "app_menu_res_dual2.h"
#elif (defined(PRJ_17DVR))
#include "app_menu_res_17dvr.h"
#elif (defined(PRJ_JETDVR))
#include "app_menu_res_jetdvr.h"
#elif (defined(PRJ_PDVR))
#include "app_menu_res_pdvr.h"
#elif (defined(PRJ_19DVR))
#include "app_menu_res_19dvr.h"
#else
#error Unsupported menu res Configured !
#endif
    
    
#endif /* _MENU_RES_H_ */
