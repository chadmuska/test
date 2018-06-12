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
 * @file lc_jiffies.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_JIFFIES_H_
#define _LINUXCOMPAT_JIFFIES_H_

/**************************************************************************
 *                        H E A D E R   F I L E S
 **************************************************************************/
#include <linux/types.h>

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/

#define MAX_JIFFY_OFFSET ((LONG_MAX >> 1)-1)
#define USEC_PER_SEC	1000000L

/*
 *	These inlines deal with timer wrapping correctly. You are
 *	strongly encouraged to use them
 *	1. Because people otherwise forget
 *	2. Because if the timer wrap changes in future you won't have to
 *	   alter your driver code.
 *
 * time_after(a,b) returns true if the time a is after time b.
 *
 * Do this with "<0" and ">=0" to only test the sign of the result. A
 * good compiler would generate better code (and a really good compiler
 * wouldn't care). Gcc is currently neither.
 */
/*gordon 10.9.3 */
#define usb_time_after(a,b)		\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(b) - (long)(a) < 0))
#define usb_time_before(a,b)	usb_time_after(b,a)

#if 1
#define HZ (1000)
extern unsigned int tmrMsTimeGet(void);
#define jiffies ((unsigned long)tmrMsTimeGet())
#define jiffies64 getmstimeofday()
#define jiffies_to_msecs(j) (j)
#define msecs_to_jiffies(m) (m)
#else

#define HZ (100)
#define jiffies ((unsigned long)tx_time_get()) /* get current os tick */

/*
 * Convert jiffies to milliseconds and back.
 *
 * Avoid unnecessary multiplications/divisions in the
 * two most common HZ cases:
 */

#define MAX_JIFFY_OFFSET ((~0UL >> 1)-1)

static inline unsigned int jiffies_to_msecs(const unsigned long j)
{
#if HZ <= 1000 && !(1000 % HZ)
	return (1000 / HZ) * j;
#elif HZ > 1000 && !(HZ % 1000)
	return (j + (HZ / 1000) - 1)/(HZ / 1000);
#else
	return (j * 1000) / HZ;
#endif
}

static inline unsigned long msecs_to_jiffies(const unsigned int m)
{
	if (m > jiffies_to_msecs(MAX_JIFFY_OFFSET))
		return MAX_JIFFY_OFFSET;
#if HZ <= 1000 && !(1000 % HZ)
	return (m + (1000 / HZ) - 1) / (1000 / HZ);
#elif HZ > 1000 && !(HZ % 1000)
	return m * (HZ / 1000);
#else
	return (m * HZ + 999) / 1000;
#endif
}
#endif

#define time_after(a,b)		\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)((b) - (a)) < 0))
#define time_before(a,b)	time_after(b,a)

#define time_after_eq(a,b)	\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)((a) - (b)) >= 0))
#define time_before_eq(a,b)	time_after_eq(b,a)

#define time_is_before_jiffies(a) time_after(jiffies, a)
#define time_is_after_jiffies(a) time_before(jiffies, a)

/* round down */
static inline unsigned long round_jiffies(unsigned long j)
{
	unsigned long rem = j % HZ;
	if (rem) {
		j = j - rem;
	}
	return j;
}

static inline unsigned long round_jiffies_up(unsigned long j)
{
	unsigned long rem = j % HZ;
	if (rem) {
		j += (HZ - rem);
	}
	return j;
}

static inline unsigned long round_jiffies_relative(unsigned long j)
{
	unsigned long j0 = jiffies;
	return round_jiffies(j + j0) - j0;
}

static inline unsigned int jiffies_to_usecs(const unsigned long j)
{
	return (USEC_PER_SEC / HZ) * j;
}

static inline unsigned long _usecs_to_jiffies(const unsigned int u)
{
	return (u + (USEC_PER_SEC / HZ) - 1) / (USEC_PER_SEC / HZ);
}

static inline unsigned long usecs_to_jiffies(const unsigned int u)
{
	if (u > jiffies_to_usecs(MAX_JIFFY_OFFSET))
		return MAX_JIFFY_OFFSET;
	return _usecs_to_jiffies(u);
}

s64 getmstimeofday();
u32 getmstimeofday32();

/* timespec functions */
#include <sys/types.h>

#define MSEC_PER_SEC	1000L
#define USEC_PER_MSEC	1000L
#define NSEC_PER_USEC	1000L
#define NSEC_PER_MSEC	1000000L
#define USEC_PER_SEC	1000000L
#define NSEC_PER_SEC	1000000000L
#define FSEC_PER_SEC	1000000000000000LL

void getnstimeofday(struct timespec *ts);

static inline s64 timespec_to_ns(const struct timespec *ts)
{
	return ((s64) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

/* ktime */

#define KTIME_MAX			((s64)~((u64)1 << 63))
#define KTIME_SEC_MAX			(KTIME_MAX / NSEC_PER_SEC)

union ktime {
	s64	tv64;
};

typedef union ktime ktime_t;		/* Kill this */

static inline ktime_t ktime_set(const s64 secs, const unsigned long nsecs)
{
	if (secs >= KTIME_SEC_MAX)
		return (ktime_t){ .tv64 = KTIME_MAX };

	return (ktime_t) { .tv64 = secs * NSEC_PER_SEC + (s64)nsecs };
}

#endif
