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
#include <linux/types.h>
#include <linux/list.h>
#include "lc_compiler.h"
/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
#define MUTEX_DEBUG		0
#define MUTEX_MAX_THREADS	6

typedef struct mutex
{
#if MUTEX_DEBUG
	list_node_t link;
#endif
	s16_t glob_cnt;
	s16_t thrd_cnt;
	const char *name;
	/* thrds[0]: thread owns the mutex.
	 * thrds[1]: first thread waiting for the mutex.
	 */
	u32_t thrds[MUTEX_MAX_THREADS];
} mutex_t;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
extern void __mutex_init(struct mutex* m, char lock, const char* name);
extern void mutex_destroy(struct mutex* m);
extern void mutex_dump(struct mutex* m);
extern void mutex_lock(struct mutex* m);
extern int  mutex_lock_interruptible(struct mutex* m);
extern int  mutex_trylock(struct mutex* m);
extern void mutex_unlock(struct mutex* m);
static inline int mutex_is_locked(struct mutex* m) { return m->glob_cnt > 0; }

#define mutex_init(m)			__mutex_init(m, 0, __FILE__ ":" __STR_LINE__)
#define mutex_lock_nested(m, subclass)	mutex_lock(m)

#define MUTEX_INITIALIZER(name)		{ .glob_cnt = 0, .thrd_cnt = 0 }
#define DECLARE_MUTEX(name) 		struct mutex name = MUTEX_INITIALIZER(name)
#define DEFINE_MUTEX(name)		struct mutex name = MUTEX_INITIALIZER(name)

#define init_MUTEX(m)			__mutex_init(m, 0, __FILE__ ":" __STR_LINE__)
#define init_MUTEX_LOCKED(m)		__mutex_init(m, 1, __FILE__ ":" __STR_LINE__)

#endif
