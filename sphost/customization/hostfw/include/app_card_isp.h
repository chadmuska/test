/**************************************************************************
 *
 *       Copyright (c) 2012-2018 by iCatch Technology, Inc.
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
#ifndef _APP_CARD_ISP_H_
#define _APP_CARD_ISP_H_

#if defined(PRJ_17DVR) || defined(PRJ_19DVR)/*weikai added 180524*/
UINT32 appFirmwareUpdate(UINT8 * filename,BOOL buzzer);
#else
UINT32 appFirmwareUpdate(void);
#endif

#endif  /* _APP_CARD_ISP_H_*/
