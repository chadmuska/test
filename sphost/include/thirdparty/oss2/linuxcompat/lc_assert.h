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
 * @file lc_assert.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_ASSERT_H_
#define _LINUXCOMPAT_ASSERT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "debug_mod/dbg_api.h"

#define oss_assert(f, l, m...) do{ dbgAssert(f, l, NULL); }while(0)

#ifdef __cplusplus
}
#endif

#endif /* _LINUXCOMPAT_ASSERT_H_ */
