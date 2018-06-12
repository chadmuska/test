/*
 * device.h - generic, centralized driver model
 *
 * Copyright (c) 2001-2003 Patrick Mochel <mochel@osdl.org>
 *
 * This file is released under the GPLv2
 *
 * See Documentation/driver-model/ for more information.
 */

#ifndef _LINUX_DEVICE_H_
#define _LINUX_DEVICE_H_

#include <linux/config.h>
#include <linux/list.h>
#include <linux/spinlock.h>
#include <linux/types.h>
#include <linux/poll.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/wait.h>
#include <linux/kernel.h>
#include <linux/kref.h>
#include <asm/atomic.h>
#include <linux/pm.h>

#if CONFIG_USB_DEBUG
#define DEBUG_GENERAL_USB
#else
#undef DEBUG_GENERAL_USB
#endif

struct device;
struct device_driver;

struct bus_type {
	char		* name;
	struct list_head glob_list;
	struct list_head drivers;
	struct list_head devices;

	int		(*match)(struct device * dev, struct device_driver * drv);
	int		(*hotplug) (struct device *dev, char **envp,
				    int num_envp, char *buffer, int buffer_size);
	int		(*suspend)(struct device * dev, pm_message_t msg);
	int		(*resume)(struct device * dev);
	const struct dev_pm_ops *pm;
};

extern int bus_register(struct bus_type * bus);
extern void bus_unregister(struct bus_type * bus);
extern void bus_dump_all();

static inline struct bus_type * get_bus(struct bus_type * bus)
{
	return bus;
}

static inline void put_bus(struct bus_type * bus)
{
}


/*
 * The type of device, "struct device" is embedded in. A class
 * or bus can contain devices of different types
 * like "partitions" and "disks", "mouse" and "event".
 * This identifies the device type and carries type-specific
 * information, equivalent to the kobj_type of a kobject.
 * If "name" is specified, the uevent will contain it in
 * the DEVTYPE variable.
 */
struct device_type {
	const char *name;
	const struct attribute_group **groups;
	void (*release)(struct device *dev);

	const struct dev_pm_ops *pm;
};


struct device_driver {
	char			* name;
	struct list_head	bus_list; 		/* node in bus's list */
	struct bus_type	* bus;
	struct list_head	devices;			/* to bind devices to this driver */

	int	(*probe)		(struct device * dev);
	int 	(*remove)	(struct device * dev);
	void	(*shutdown)	(struct device * dev);
	int	(*suspend)	(struct device * dev, u32 state/*, u32 level*/);
	int	(*resume)	(struct device * dev/*, u32 level*/);
	const struct dev_pm_ops *pm;
};
extern int driver_register(struct device_driver * drv);
extern void driver_unregister(struct device_driver * drv);

struct device {
	/* for ndk device */
	char name[16];
	int  type;
	int  dev_id;
	/* for linux */
	struct kref			kref;			/* reference counter */
	struct list_head	bus_list;	/* node in bus's list */
	struct list_head	driver_list; /* to bind driver to this device */
	struct device 		*parent;
	struct bus_type	*bus;		/* type of bus device is on */
	struct device_driver *driver;	/* which driver has allocated this device */
	void					*driver_data;	/* data private to the driver */
	u32					*dma_mask;	/* dma mask (if dma'able device) */
	struct list_head dma_pools;	/* dma pools (if dma'ble) */
	void	(*release)(struct device * dev);
	struct dev_pm_info power;
	struct dev_pm_ops *pm;
};

static inline const char *dev_name(const struct device *dev)
{
	return dev->name;
}

static inline void * dev_get_drvdata (struct device *dev)
{
	return dev->driver_data;
}

static inline void dev_set_drvdata (struct device *dev, void *data)
{
	dev->driver_data = data;
}

/*
 * High level routines for use by the bus drivers
 */
extern int device_register(struct device * dev);
extern void device_unregister(struct device * dev);
extern void device_initialize(struct device * dev);
extern int device_add(struct device * dev);
extern void device_del(struct device * dev);

/*
 * Manual binding of a device to driver. See drivers/base/bus.c
 * for information on use.
 */
extern int  driver_probe_device(struct device_driver * drv, struct device * dev);
extern void device_bind_driver(struct device * dev);
extern void device_release_driver(struct device * dev);
extern int  device_attach(struct device * dev);
extern void driver_attach(struct device_driver * drv);

extern struct device * get_device(struct device * dev);
extern void put_device(struct device * dev);


/* debugging and troubleshooting/diagnostic helpers. */
extern const char *dev_driver_string(const struct device *dev);
#define dev_printk(level, dev, format, arg...)	\
do { \
	if (dev) \
		printk(level "%s:%d> %s %s: " format, __func__, __LINE__, dev_driver_string((const struct device *)(dev)) , \
			dev_name((const struct device *)(dev)) , ## arg); \
	else \
		printk(level "%s:%d> dev(NULL): " format, __func__, __LINE__, ## arg); \
}while(0)

#ifdef CONFIG_USE_MINI_DEBUG
#define usb_dbg  printk
#else
#define usb_dbg(format, arg...) do {  } while (0)
#endif

#ifdef DEBUG_GENERAL_USB
#define dev_dbg(dev, format, arg...)		\
	dev_printk(KERN_DEBUG , dev , format , ## arg)
#else
#define dev_dbg(dev, format, arg...) do { (void)(dev); } while (0)
#endif
#define dev_err(dev, format, arg...)		\
	dev_printk(KERN_ERR , dev , format , ## arg)
#define dev_info(dev, format, arg...)		\
	dev_printk(KERN_INFO , dev , format , ## arg)
#define dev_warn(dev, format, arg...)		\
	dev_printk(KERN_WARNING , dev , format , ## arg)

#endif /* _DEVICE_H_ */
