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
 * @file lc_semaphore.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_SEMAPHORE_H_
#define _LINUXCOMPAT_SEMAPHORE_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "lc_compiler.h"

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
struct semaphore {
	unsigned int semaid;
};

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
extern void __sema_init(struct semaphore* m, unsigned int count, const char* name);
extern void sema_down(struct semaphore* s);
extern int sema_down_trylock(struct semaphore* s);
extern void sema_up(struct semaphore* s);
extern void sema_destroy(struct semaphore* s);
extern void sema_cnt_get(struct semaphore* s, unsigned int *cnt);

#define sema_init(s, count) 		__sema_init(s, count, __FILE__ ":" __STR_LINE__)

static inline void down(struct semaphore *s) { sema_down(s); }
static inline void up(struct semaphore *s)   { sema_up(s); }
static inline int  down_interruptible(struct semaphore *s) { sema_down(s); return 0; }
static inline int  down_trylock(struct semaphore *s) { sema_down(s); return 0; }

#endif /* _LINUXCOMPAT_SEMAPHORE_H_ */

