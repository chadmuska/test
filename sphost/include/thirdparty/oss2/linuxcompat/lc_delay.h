/**************************************************************************
 *
 *       Copyright (c) 2013-2013 by iCatch Technology, Inc.
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
/**
 * @file lc_delay.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_DELAY_H_
#define _LINUXCOMPAT_DELAY_H_

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define udelay(n)	tmrUsHardWait(n)
#define mdelay(n)	lc_thread_sleep(n)
#define msleep(n)	lc_thread_sleep(n)

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
extern void tmrUsHardWait(unsigned int delay);
extern void lc_thread_sleep(unsigned int ms);

#endif /* defined(_LINUXCOMPAT_DELAY_H_) */
