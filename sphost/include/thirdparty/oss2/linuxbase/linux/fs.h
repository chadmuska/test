#ifndef _LINUX_FS_H
#define _LINUX_FS_H

#include <linux/types.h>
#include <linux/poll.h>

struct devfile;
struct device;

struct file_operations {
	void* owner;
	ssize_t (*read) (struct devfile *, char *, size_t, loff_t *);
	ssize_t (*write) (struct devfile *, const char *, size_t, loff_t *);
	unsigned int (*poll) (struct devfile *, struct poll_table_struct *);
	int (*ioctl) (struct devfile *, unsigned int, unsigned long);
	int (*open) (struct device *, struct devfile *);
	int (*release) (struct device *, struct devfile *);
};

#endif

