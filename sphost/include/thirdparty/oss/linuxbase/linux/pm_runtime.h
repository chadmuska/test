/*
 * pm_runtime.h - Device run-time power management helper functions.
 *
 * Copyright (C) 2009 Rafael J. Wysocki <rjw@sisk.pl>
 *
 * This file is released under the GPLv2.
 */

#ifndef _LINUX_PM_RUNTIME_H
#define _LINUX_PM_RUNTIME_H


#include <linux/device.h>
#include <linux/atomic.h>
#include <linux/jiffies.h>
#include "pm.h"
#include "pm_config.h"
#include "errno.h"


#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif

/* Runtime PM flag argument bits */
#define RPM_ASYNC		0x01	/* Request is asynchronous */
#define RPM_NOWAIT		0x02	/* Don't wait for concurrent
					    			    state change */
#define RPM_GET_PUT		0x04	/* Increment/decrement the
					    			    usage_count */
#define RPM_AUTO		0x08	/* Use autosuspend_delay */

#ifdef CONFIG_PM_RUNTIME

/*extern struct workqueue_struct *pm_wq;*/

extern int pm_runtime_idle(struct device *dev, int rpmflags);
extern int pm_runtime_suspend(struct device *dev, int rpmflags);
extern int pm_runtime_resume(struct device *dev, int rpmflags);
extern int pm_schedule_suspend(struct device *dev, unsigned int delay);
extern int __pm_runtime_set_status(struct device *dev, unsigned int status);
extern void pm_runtime_enable(struct device *dev);
extern void __pm_runtime_disable(struct device *dev, bool check_resume);
#if 0
extern int pm_request_resume(struct device *dev);
extern int pm_request_idle(struct device *dev);
extern int pm_runtime_barrier(struct device *dev);
#endif
static inline bool pm_children_suspended(struct device *dev)
{
	return dev->power.ignore_children
		|| !atomic_read(&dev->power.child_count);
}

static inline void pm_suspend_ignore_children(struct device *dev, bool enable)
{
	dev->power.ignore_children = enable;
}

static inline void pm_runtime_get_noresume(struct device *dev)
{
	atomic_inc(&dev->power.usage_count);
}

static inline void pm_runtime_put_noidle(struct device *dev)
{
	atomic_add(-1,&dev->power.usage_count);
}
#if 0
static inline bool device_run_wake(struct device *dev)
{
	return dev->power.run_wake;
}

static inline void device_set_run_wake(struct device *dev, bool enable)
{
	dev->power.run_wake = enable;
}
#endif
static inline void pm_runtime_mark_last_busy(struct device *dev)
{
	ACCESS_ONCE(dev->power.last_busy) = jiffies;
}

void __pm_runtime_use_autosuspend(struct device *dev, bool use);

void pm_runtime_set_autosuspend_delay(struct device *dev, int delay);




#else /* !CONFIG_PM_RUNTIME */

static inline int pm_runtime_idle(struct device *dev, int rpmflags) { return -ENOSYS; }
static inline int pm_runtime_suspend(struct device *dev, int rpmflags) { return -ENOSYS; }
static inline int pm_runtime_resume(struct device *dev, int rpmflags) { return -1; }
static inline int pm_schedule_suspend(struct device *dev, unsigned int delay)
{
	return -ENOSYS;
}
static inline int __pm_runtime_set_status(struct device *dev,
					    unsigned int status) { return 0; }
#if 0
static inline int pm_runtime_barrier(struct device *dev) { return 0; }
static inline int pm_request_idle(struct device *dev) { return -ENOSYS; }
static inline int pm_request_resume(struct device *dev) { return 0; }
#endif
static inline void pm_runtime_enable(struct device *dev) {}
static inline void __pm_runtime_disable(struct device *dev, bool c) {}

static inline bool pm_children_suspended(struct device *dev) { return false; }
static inline void pm_suspend_ignore_children(struct device *dev, bool en) {}
static inline void pm_runtime_get_noresume(struct device *dev) {}
static inline void pm_runtime_put_noidle(struct device *dev) {}
static inline void __pm_runtime_use_autosuspend(struct device *dev, bool use){}

static inline void pm_runtime_set_autosuspend_delay(struct device *dev, int delay){}

#if 0
static inline bool device_run_wake(struct device *dev) { return false; }
static inline void device_set_run_wake(struct device *dev, bool enable) {}
#endif
#endif /* !CONFIG_PM_RUNTIME */

static inline int pm_runtime_get(struct device *dev)
{
	return pm_runtime_resume(dev, RPM_GET_PUT | RPM_ASYNC);
}

static inline int pm_runtime_put(struct device *dev)
{
	return pm_runtime_idle(dev, RPM_GET_PUT | RPM_ASYNC);
}

static inline int pm_runtime_set_active(struct device *dev)
{
	return __pm_runtime_set_status(dev, RPM_ACTIVE);
}

static inline void pm_runtime_set_suspended(struct device *dev)
{
	__pm_runtime_set_status(dev, RPM_SUSPENDED);
}

static inline void pm_runtime_disable(struct device *dev)
{
	__pm_runtime_disable(dev, true);
}

static inline int pm_runtime_put_sync_autosuspend(struct device *dev)
{
	return pm_runtime_idle(dev, RPM_GET_PUT | RPM_AUTO);
}

static inline int pm_runtime_get_sync(struct device *dev)
{
	return pm_runtime_resume(dev, RPM_GET_PUT);
}

static inline int pm_runtime_put_sync(struct device *dev)
{
	return pm_runtime_idle(dev, RPM_GET_PUT);
}

static inline int pm_runtime_autosuspend(struct device *dev)
{
	return pm_runtime_suspend(dev, RPM_AUTO);
}

#endif

