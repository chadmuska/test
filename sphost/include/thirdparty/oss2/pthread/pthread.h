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
#ifndef _PTHEAD_H_
#define _PTHEAD_H_

#ifdef  __cplusplus
extern  "C" {
#endif


#include "common.h"
#include "pthread_def.h"
#include "oss_ros_api.h"

#define MAXTOTALWAITHREAD 100
#define cleanCntMax 10

typedef enum
{

  PThreadStateInitial = 0,	/* Thread not running                   */
  PThreadStateRunning,		/* Thread alive & kicking               */
  PThreadStateSuspended,	/* Thread alive but suspended           */
  PThreadStateCancelPending,	/* Thread alive but is                  */
  /* has cancelation pending.        */
  PThreadStateCanceling,	/* Thread alive but is                  */
  PThreadStateDelete,	/* Thread alive but exiting             */
  /* due to an exception                  */
  PThreadStateLast
}
PThreadState;

typedef int pthread_once_t;

typedef struct {
	unsigned long event_flag_id;
	unsigned long flags;
	UINT32 option; 
} pthread_cond_t;


typedef struct{
}pthread_condattr_t;

typedef struct {
	UINT32 owner_thread_id; 
} pthread_mutex_t;

typedef struct {
	UINT32 kind;
} pthread_mutexattr_t;

typedef struct {
	UINT32 cancleState;
	UINT32 cancleType;
	UINT32 thrState;
	UINT32 detachState;
} arg_pthread_t;

typedef struct linklist_s {
	struct linklist_s *prev;
	struct linklist_s *next;
} linklist_t;

typedef struct {
	arg_pthread_t arg;
	UINT32 thread_id;
	int top;
	linklist_t link;
	void(*cleanup[cleanCntMax])(void *);
	void *cleanarg[cleanCntMax];
} ios_pthread_t;
typedef UINT32 pthread_t;

typedef struct pthread_rwlock
{
	int active_readers; /* -1 when writer lock locked, >0 when read lock locked */
	int pending_readers;
 	int pending_writers;
 	pthread_mutex_t mutex;
 	pthread_cond_t ok_to_read;
 	pthread_cond_t ok_to_write;
}pthread_rwlock_t ;

typedef struct pthread_rwlockattr_t_
{
  int pshared;
}pthread_rwlockattr_t;

struct pthread_key_t_
{
  UINT32 key;
  void (*destructor) (void *);
  void *keyValue;
};
typedef struct pthread_key_t_ * pthread_key_t;


typedef struct pthread_barrierattr_t_{
	int pshared;
}pthread_barrierattr_t;

typedef struct 
{
  UINT32 pshared;
  UINT32 nCurrentCnt;
  UINT32 nTotalCnt;
  UINT32 threadCnt[MAXTOTALWAITHREAD];
  pthread_mutex_t mutex; 
}pthread_barrier_t_;
typedef pthread_barrier_t_*  pthread_barrier_t;
	

int pthread_create(pthread_t * thread,const pthread_attr_t *attr,
	void *(*entry_func)(void*),
	void *arg);	
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
int pthread_cancel (pthread_t thread);
void pthread_testcancel (void);
int pthread_join(pthread_t thread, void **retval);
int pthread_detach(pthread_t thread);
pthread_t pthread_self(void);
int pthread_equal(pthread_t tid1,pthread_t tid2);
int pthread_kill (pthread_t thread, int sig);
void pthread_exit (void *value_ptr);
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param);
int pthread_cond_init(pthread_cond_t *cond,pthread_condattr_t *cond_attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_signal (pthread_cond_t * cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_wait (pthread_cond_t * cond, pthread_mutex_t * mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);
int pthread_condattr_init (pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);
int pthread_condattr_getpshared(const pthread_condattr_t *attr,int *pshared);
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_setdetachstate (pthread_attr_t *attr, int detachstate);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_once (pthread_once_t * once_control, void (*init_routine) (void));
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_setspecific (pthread_key_t key, const void *value);
void *pthread_getspecific (pthread_key_t key);
int pthread_key_delete (pthread_key_t key);
int pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param);
int pthread_attr_getschedparam(pthread_attr_t *attr, const struct sched_param *param);
void pthread_cleanup_push(void(*rtn)(void *), void *_arg);
void pthread_cleanup_pop(int execute);
int pthread_mutex_init (pthread_mutex_t * mutex, const pthread_mutexattr_t * attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock (pthread_mutex_t * mutex);
int pthread_mutex_trylock( pthread_mutex_t *mutex );
int pthread_mutex_unlock (pthread_mutex_t * mutex);
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr , int type);
int pthread_mutexattr_gettype(pthread_mutexattr_t *attr , int *type);
int pthread_mutex_init (pthread_mutex_t * mutex, const pthread_mutexattr_t * attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock (pthread_mutex_t * mutex);
int pthread_mutex_trylock( pthread_mutex_t *mutex );
int pthread_mutex_unlock (pthread_mutex_t * mutex);
int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);
int pthread_mutexattr_settype(pthread_mutexattr_t *attr , int type);
int pthread_mutexattr_gettype(pthread_mutexattr_t *attr , int *type);
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t * rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
int pthread_rwlock_timedrdlock (pthread_rwlock_t *rwlock,	const struct timespec *abstime);
int pthread_rwlock_timedwrlock (pthread_rwlock_t * rwlock,const struct timespec *abstime);
int pthread_rwlock_tryrdlock (pthread_rwlock_t * rwlock);
int pthread_rwlock_trywrlock (pthread_rwlock_t * rwlock);
int pthread_rwlockattr_destroy (pthread_rwlockattr_t *attr);
int pthread_rwlockattr_getpshared (const pthread_rwlockattr_t * attr,int *pshared);
int pthread_rwlockattr_setpshared (pthread_rwlockattr_t * attr, int pshared);
int pthread_rwlockattr_init (pthread_rwlockattr_t * attr);
int pthread_barrier_init (pthread_barrier_t *barrier,
		      const pthread_barrierattr_t *attr, unsigned int count);

int pthread_barrier_destroy (pthread_barrier_t * barrier);
int pthread_barrier_wait (pthread_barrier_t * barrier);
int pthread_all_thread();





#ifdef  __cplusplus
}
#endif

#endif /* _PTHEAD_H_*/
