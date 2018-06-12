#ifndef _LINUX_MODULE_H
#define _LINUX_MODULE_H

#include <linux/config.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/compiler.h>
#include <linux/kobject.h>

#define MODULE_LICENSE(_license)
#define MODULE_AUTHOR(_author)
#define MODULE_DESCRIPTION(_description)
#define MODULE_PARM_DESC(_parm, desc)
#define MODULE_DEVICE_TABLE(type,name)
#define module_param(name, type, perm)
#define MODULE_VERSION(name)
#define EXPORT_SYMBOL(name)
#define EXPORT_SYMBOL_GPL(name)
#endif /* _LINUX_MODULE_H */
