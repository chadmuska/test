#ifndef _LINUX_CDEV_H
#define _LINUX_CDEV_H

#include <linux/fs.h>
#include <linux/device.h>

struct cdev {
	struct device dev;
	struct file_operations* ops;
};

#endif

