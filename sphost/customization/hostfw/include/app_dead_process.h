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

#ifndef _APP_DEAD_PROCESS_H_
#define _APP_DEAD_PROCESS_H_

/**************************************************************************
 *                           C O N S T A N T S                            *
 **************************************************************************/

/**************************************************************************
 *                              M A C R O S                               *
 **************************************************************************/
#define SYS_PWR_OFF_NOR     sp5kPowerCtrl(SP5K_POWER_OFF, OPT_HOST_NORMAL)
#define SYS_PWR_OFF_DEAD    sp5kPowerCtrl(SP5K_POWER_OFF, OPT_HOST_DEAD)    /*must avoid memory flush after program dead*/
#define SYS_REBOOT_NOR      appRtcReset(OPT_HOST_NORMAL);
#define SYS_REBOOT_DEAD     appRtcReset(OPT_HOST_DEAD);                     /*must avoid memory flush after program dead*/

/**************************************************************************
 *                          D A T A    T Y P E S                          *
 **************************************************************************/

 /**************************************************************************
 *                         G L O B A L    D A T A                         *
 **************************************************************************/

/**************************************************************************
 *                 E X T E R N A L    R E F E R E N C E S                 *
 **************************************************************************/

/**************************************************************************
 *               F U N C T I O N    D E C L A R A T I O N S               *
 **************************************************************************/
void appPowerKeyNormalInit(void);
void appHostDeadMunuCb(void);
void appHostDeadDump(void);

#endif

