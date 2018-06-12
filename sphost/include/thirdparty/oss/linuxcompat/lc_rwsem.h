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
 * @file lc_rwsem.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_RWSEM_H_
#define _LINUXCOMPAT_RWSEM_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include "lc_spinlock.h"
#include "lc_compiler.h"

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
/*
 * the rw-semaphore definition
 * - if activity is 0 then there are no active readers or writers
 * - if activity is +ve then that is the number of active readers
 * - if activity is -1 then there is one active writer
 */

struct rw_semaphore {
	unsigned int mutex;
	unsigned int events;
	int			 activity;
};

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/

extern void __init_rwsem(struct rw_semaphore *sem, char * name);
extern void destroy_rwsem(struct rw_semaphore *sem);
extern void down_read(struct rw_semaphore *sem);
extern int down_read_trylock(struct rw_semaphore *sem);
extern void down_write(struct rw_semaphore *sem);
extern void up_read(struct rw_semaphore *sem);
extern void up_write(struct rw_semaphore *sem);

#define init_rwsem(sem) __init_rwsem(sem, __FILE__ ":" __STR_LINE__)
#endif /* _LINUXCOMPAT_RWSEM_H_ */
