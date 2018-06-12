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
#include "app_usb_dpslib.h"

#ifndef _APP_USBDPSNEBU_H_
#define _APP_USBDPSNEBU_H_
#include "app_usb_dpslib.h"
#define DPS_PARTIAL_PIC_NUM						48

void appUsbDpsMainMenuStateCreate( dpsMenuOption_t  gDpsMenuOpt );
void appUsbDpsSubMenuStateCreate( dpsMenuOption_t gDpsMenuOpt );
void appDPSMenu_SubMenuChange( unsigned char bMenuDown);
void appUsbDpsSubMenu_SetKeyCBFReg(	appMenuFunctionSetCBF_t *setupKeyPressCbf);
void appUsbDpsSubMenu_SetKeyCBF(appMenuAction_e action,appMenuInfo_t *menuTab,appSubMenuInfo_t *SubmenuItem);
void appUsbDpsSubMenuState(UINT32 msg,UINT32 param);
void appDPSMenuFunctionSetCB(appMenuAction_e action,appMenuInfo_t *pmenuCurTab,	appSubMenuInfo_t *psubMenuItem);
void _DpsMenuKeyPressSettingChange(appMenuSetupMenu_e menuCurTab,UINT32 subMenuItem);
void _DpsSettingMaintain();
void _DpsAddImageProcess();
void appUsbDps2ndMenuLeftRightCangeSet(unsigned char bMenuLeft);
void appDPSMenu_SubMenuSetKeyProcess();

#endif
