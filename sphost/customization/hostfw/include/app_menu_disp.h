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
 
/**
 * @file      app_menu_disp.h
 * @brief     Brief
 * @author    Phil Lin
 * @since     2012-10-12
 * @date      2012-10-12
*/

#ifndef __APP_MENU_DISP_H__
#define __APP_MENU_DISP_H__

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "app_menu.h" 
/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/
 
/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/

#define MENU_PROMPT_OK_OFF	0x00
#define MENU_PROMPT_OK_ON		0x01

#define MENU_PROMPT_MENU_OFF	0x00
#define MENU_PROMPT_MENU_ON	0x02

#define MENU_PROMPT_PAGEUPDN_OFF	0x00
#define MENU_PROMPT_PAGEUPDN_ON		0x04

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/
 typedef enum 
 {
	IDX_MENU_CHAR_WIDTH = 0,
	IDX_MENU_CHAR_HEIGHT,
	IDX_MENU_BAR_LAYER2_X_OFFSET,
	IDX_MENU_BAR_LAYER2_Y_OFFSET,
	IDX_MENU_MU2BARSL_WIDTH,
 }menuDispParaIdx_e;
/**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/
 
/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/
 
/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void menuProcDisp (void);
void menuProcDisp2Layer (void);
void menuProcDispYesNo(UINT16 idx);
void menuEVDisp(void);
void menuRTCDisp(void);
#if 0
void menuWIFIPasswordDisp(void);
#endif
void menuProcDispPalette(UINT16 msg);
UINT16 appMenuDispParaGet(UINT32 paraIdx);
#if HDMI_MENU_BY_JPG
UINT32 menuHDMIPIPInit(void);
void menuDlgShow(UINT32 num);
void menuDlgBarShow(UINT32 dlgNum,UINT32 item);
void menuHDMIClean(void);
#endif
#endif  /* __APP_MENU_DISP_H__ */

