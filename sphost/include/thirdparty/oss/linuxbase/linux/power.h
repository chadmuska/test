#ifndef __POWER_H
#define __POWER_H
#include "pm_config.h"
#ifdef CONFIG_PM_RUNTIME

extern void pm_runtime_init(struct device *dev);
extern void pm_runtime_remove(struct device *dev);

#else /* !CONFIG_PM_RUNTIME */

static inline void pm_runtime_init(struct device *dev) {}
static inline void pm_runtime_remove(struct device *dev) {}

#endif

#ifdef CONFIG_PM

/*
 * pm_main.c
 */

extern struct list_head dpm_list;	/* The active device list */

static inline struct device *to_device(struct list_head *entry)
{
	return container_of(entry, struct device, power.entry);
}

extern void device_pm_init(struct device *dev);
extern void device_pm_add(struct device *);
extern void device_pm_remove(struct device *);

#else 

static inline void device_pm_init(struct device *dev)
{
	pm_runtime_init(dev);
}

static inline void device_pm_remove(struct device *dev)
{
	pm_runtime_remove(dev);
}

static inline void device_pm_add(struct device *dev) {}

#endif /* !CONFIG_PM */

#if 0 /*this is for sys file system*/
#ifdef CONFIG_PM

/*
 * sysfs.c
 */

extern int dpm_sysfs_add(struct device *);
extern void dpm_sysfs_remove(struct device *);

#else /* CONFIG_PM */

static inline int dpm_sysfs_add(struct device *dev)
{
	return 0;
}

static inline void dpm_sysfs_remove(struct device *dev)
{
}
#endif
#endif

#endif
