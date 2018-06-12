#ifndef _LINUX_KERNEL_H
#define _LINUX_KERNEL_H

/*
 * 'kernel.h' contains some often-used function prototypes etc
 */

#include <linux/types.h>
#include <linux/compiler.h>
#include <linux/bitops.h>
#include <asm/byteorder.h>
#include <asm/bug.h>
#include <linux/completion.h>
#include <linux/gfp.h>

#include <stdio.h>
#include <stdlib.h>

#define diag_printf printf
#define simple_strtoul strtoul
#define scnprintf	snprintf

#define TASK_INTERRUPTIBLE	1
#define TASK_UNINTERRUPTIBLE 2
#define __init
#define __exit

#define copy_to_user(to, from, n)	({memcpy(to, from, n); 0;})
#define copy_from_user(to, from, n)	({memcpy(to, from, n); 0;})

#define get_user(x, ptr)		({(x) = *(ptr); 0;})
#define put_user(x, ptr)		({*(ptr) = (x); 0;})

#define PRINTK
#if CONFIG_USB_DEBUG
#define DEBUG_GENERAL_USB
#endif

#ifndef SCSI_DEBUG
#define SCSI_DEBUG(x...)  do {printk(KERN_DEBUG x);} while(0)
#endif

#if SUPPORT_STEADY_DEBUG
#define STEADY_DEBUG(x...)  printk(KERN_DEBUG x)
#else
#define STEADY_DEBUG(x...)  do {} while (0)
#endif

#ifndef USB_DEBUG
#define USB_DEBUG (x...)  do {} while (0)
#endif

#ifndef DEVICE_ADD__DEBUG
#define DEVICE_ADD_DEBUG(x...)  printk(KERN_DEBUG x)
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))


#define	KERN_EMERG              "<0>" /* system is unusable */
#define	KERN_ALERT              "<1>" /* action must be taken immediately */
#define	KERN_CRIT               "<2>" /* critical conditions */
#define	KERN_ERR                "<3>" /* error conditions */
#define	KERN_WARNING            "<4>" /* warning conditions */
#define	KERN_NOTICE             "<5>" /* normal but significant condition */
#define	KERN_INFO               "<6>" /* informational */
#define	KERN_DEBUG              "<7>" /* debug-level messages */
#define  might_sleep() do {} while(0)

#ifdef PRINTK
	#define printk diag_printf
#else
	#define printk(fmt,arg...) \
		do { } while (0)
#endif

#ifdef DEBUG_GENERAL_USB
	#define pr_debug(fmt,arg...) \
		printk(KERN_DEBUG fmt,##arg)
#else
	#define pr_debug(fmt,arg...) \
		do { } while (0)
#endif

#define pr_info(fmt,arg...) \
	printk(KERN_INFO fmt,##arg)

/*
 * min()/max() macros that also do
 * strict type-checking.. See the
 * "unnecessary" pointer comparison.
 */
#define min(x,y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);		\
	_x < _y ? _x : _y; })

#define max(x,y) ({ \
	typeof(x) _x = (x);	\
	typeof(y) _y = (y);	\
	(void) (&_x == &_y);		\
	_x > _y ? _x : _y; })

/*
 * ..and if you can't take the strict
 * types, you can specify one yourself.
 *
 * Or not use min/max at all, of course.
 */
#define min_t(type,x,y) \
	({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })
#define max_t(type,x,y) \
	({ type __x = (x); type __y = (y); __x > __y ? __x: __y; })

#define clamp(val, min, max) ({	\
	typeof(val) __val = (val);	\
	typeof(min) __min = (min);	\
	typeof(max) __max = (max);	\
	(void) (&__val == &__min);	\
	(void) (&__val == &__max);	\
	__val = __val < __min ? __min : __val;	\
	__val > __max ? __max: __val; })
	
	
/**
 * container_of - cast a member of a structure out to the containing structure
 *
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */

#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})
/*
 * Check at compile time that something is of a particular type.
 * Always evaluates to 1 so you may use it easily in comparisons.
 */
#define typecheck(type,x) \
({	type __dummy; \
	typeof(x) __dummy2; \
	(void)(&__dummy == &__dummy2); \
	1; \
})

#endif /* _LINUX_KERNEL_H */
