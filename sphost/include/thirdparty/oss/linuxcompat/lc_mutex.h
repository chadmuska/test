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
 * @file lc_mutex.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_MUTEX_H_
#define _LINUXCOMPAT_MUTEX_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "lc_compiler.h"

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
struct mutex
{
	unsigned int mtxid;
};

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
extern void __mutex_init(struct mutex* m, char lock, const char* name);
extern void mutex_destroy(struct mutex* m);
extern void mutex_lock(struct mutex* m);
extern int  mutex_lock_interruptible(struct mutex* m);
extern void mutex_unlock(struct mutex* m);

#define mutex_init(m)			__mutex_init(m, 0, __FILE__ ":" __STR_LINE__)
#define mutex_lock_nested(m, subclass)	mutex_lock(m)

#define DECLARE_MUTEX(name) 		struct mutex name = { .mtxid = 0 }
#define DEFINE_MUTEX(name)		struct mutex name = { .mtxid = 0 }

#define init_MUTEX(m)			__mutex_init(m, 0, __FILE__ ":" __STR_LINE__)
#define init_MUTEX_LOCKED(m)		__mutex_init(m, 1, __FILE__ ":" __STR_LINE__)

#endif
