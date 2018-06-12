/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2009-2014 by iCatch Technology Co., Ltd.         *
 *                                                                        *
 *  This software is copyrighted by and is the property of iCatch         *
 *  Technology Co., Ltd. All rights are reserved by iCatch Technology     *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of iCatch Technology Co., Ltd.                        *
 *                                                                        *
 *  iCatch Technology Co., Ltd. reserves the right to modify this         *
 *  software without notice.                                              *
 *                                                                        *
 *  iCatch Technology Co., Ltd.                                           *
 *  19-1, Innovation First Road, Science-Based Industrial Park,           *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 *  Author: Timothy Wu                                                    *
 *  Author: Anmin Deng                                                    *
 *  Author: Philip Lin                                                    *
 *                                                                        *
 **************************************************************************/
/* DO NOT EDIT the next 2 lines for auto revision by subversion
$Rev: 12606 $
$Date:: 2014-03-19 21:27:29#$
 */
#ifndef _PTHEAD_SEMAPHORE_H_
#define _PTHEAD_SEMAPHORE_H_

#ifdef  __cplusplus
extern  "C" {
#endif


#include "common.h"
#include "pthread_def.h"


typedef unsigned int mode_t;

typedef struct 
{
  UINT32 sem_id;
}sem_t;


int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
sem_t *sem_open(const char *name,int oflag,mode_t mode,unsigned int value);
int sem_close (sem_t * sem);
int sem_getvalue(sem_t *sem, int *sval);
int sem_post(sem_t *sem);
int sem_wait(sem_t * sem);
int sem_trywait(sem_t *sem);
int sem_timedwait(sem_t *sem, const struct timespec *abs_timeout);
int sem_unlink (const char *name);



#ifdef  __cplusplus
}
#endif

#endif /* _PTHEAD_SEMAPHORE_H_*/
