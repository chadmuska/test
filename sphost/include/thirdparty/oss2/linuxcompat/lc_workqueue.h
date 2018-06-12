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
 * @file lc_workqueue.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_WORKQUEUE_H_
#define _LINUXCOMPAT_WORKQUEUE_H_

#include <sp5k_os_api.h>
#include <linux/list.h>
#include <asm/types.h>

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include <sys/time.h>
#ifdef __cplusplus
extern "C" {
#endif

#define WORKQUEUE_DEFAULT_PRIORITY	20

/**************************************************************************
 *                          D A T A   T Y P E S
 **************************************************************************/
struct work_struct;
typedef void (*work_func_t)(struct work_struct *work);

struct workqueue_struct
{
	SP5K_EVENT_FLAGS_GROUP evt_flags;
	struct list_head list;
	UINT32 thrd;
	UINT32 flushing : 1,
	       draining : 1,
	       freeable : 1;
	char   name[32];
};
typedef struct workqueue_struct workqueue_struct_t;

struct work_struct
{
	list_head_t list;
	work_func_t func;
	const char *name;
	SINT64 expire_time;
	UINT8  running : 1;
	struct workqueue_struct *wq;
};
typedef struct work_struct work_struct_t;

struct delayed_work {
	struct work_struct work;
};
typedef struct delayed_work delayed_work_t;

extern struct workqueue_struct	*system_wq;

/**************************************************************************
 *              F U N C T I O N   D E C L A R A T I O N S
 **************************************************************************/
extern void tmrTimeStampGet(struct timeval *tv);

struct workqueue_struct *
__alloc_workqueue(
	struct workqueue_struct *wq,
	int                      priority,
	unsigned int             stack_size,
	const char              *func,
	unsigned int             line);

#define alloc_workqueue(fmt, flags, max_active, args...) \
	__alloc_workqueue(NULL, WORKQUEUE_DEFAULT_PRIORITY, 0, __func__, __LINE__)

#define alloc_ordered_workqueue(fmt, flags, args...) \
	__alloc_workqueue(NULL, WORKQUEUE_DEFAULT_PRIORITY, 0, __func__, __LINE__)

#define create_singlethread_workqueue(name) \
	__alloc_workqueue(NULL, WORKQUEUE_DEFAULT_PRIORITY, 0, __func__, __LINE__)

#define create_workqueue(name)						\
	alloc_workqueue("%s", WQ_MEM_RECLAIM, 1, (name))

#define create_freezable_workqueue(name)				\
	alloc_workqueue("%s", WQ_FREEZABLE | WQ_UNBOUND | WQ_MEM_RECLAIM, \
			1, (name))

BOOL workqueue_init(struct workqueue_struct *wq, int priority, unsigned int stack_size, const char *name);
UINT32 workqueue_next_timeout(struct workqueue_struct *wq);

void __INIT_WORK(struct work_struct* w, work_func_t f, const char *name);
BOOL __queue_delayed_work(struct workqueue_struct *wq, struct work_struct *w, unsigned long delay);
BOOL __flush_work(struct work_struct *w);
BOOL __cancel_work(struct work_struct *w, BOOL sync);

/* >= 0,  time left. < 0, expired time */
long dwork_remaining_time(struct delayed_work *w);

static inline BOOL work_pending(struct work_struct *work) { return work->list.next != NULL; }
static inline BOOL work_running(struct work_struct *work) { return work->running; }

void destroy_workqueue(struct workqueue_struct *wq);
void flush_workqueue(struct workqueue_struct *wq);
void drain_workqueue(struct workqueue_struct *wq);
void flush_scheduled_work(void);
void dump_workqueue(struct workqueue_struct *wq);
BOOL in_workqueue_thread(struct workqueue_struct *wq);

BOOL mod_delayed_work(struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay);

#define INIT_WORK(w, f)				__INIT_WORK((struct work_struct *)w, f, #f)
#define INIT_DELAYED_WORK(w, f)			__INIT_WORK((struct work_struct *)w, f, #f)
#define queue_delayed_work(wq, w, delay)	__queue_delayed_work(wq, (struct work_struct *)w, delay)
#define queue_work(wq, w)			__queue_delayed_work(wq, (struct work_struct *)w, 0)
#define flush_work(w)				__flush_work(w)
#define flush_delayed_work(w)			__flush_work((struct work_struct*)w)
#define schedule_work(w)			__queue_delayed_work(system_wq, (struct work_struct *)w, 0)
#define schedule_delayed_work(w, delay)		__queue_delayed_work(system_wq, (struct work_struct *)w, delay)

#define delayed_work_pending(w)			work_pending((struct work_struct *)w)
#define cancel_work_sync(w)			__cancel_work((struct work_struct *)w, TRUE)
#define cancel_delayed_work(w)			__cancel_work((struct work_struct *)w, FALSE)
#define cancel_delayed_work_sync(w)		__cancel_work((struct work_struct *)w, TRUE)

#define DECLARE_WORK(w, f) \
	struct work_struct w = {{(list_head_t*)&w, (list_head_t*)&w}, f, 0, 0, NULL}

#define DECLARE_DELAYED_WORK(w, f) \
	struct delayed_work w = {{{(list_head_t*)&w, (list_head_t*)&w}, f, 0, 0, NULL}}

#ifdef __cplusplus
}
#endif

#endif /* _LINUXCOMPAT_WORKQUEUE_H_ */
