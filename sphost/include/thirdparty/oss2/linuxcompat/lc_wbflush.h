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
 * @file lc_wbflush.h
 * @brief
 * @author LiJun
 *
 * The detail information about this file.
 */

#ifndef _LINUXCOMPAT_WBFLUSH_H_
#define _LINUXCOMPAT_WBFLUSH_H_

#include "common.h"

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
extern void wbflush(void);

extern void ros_wbflush(void);

#endif /* _LINUXCOMPAT_WBFLUSH_H_ */
