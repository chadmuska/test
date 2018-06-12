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
 * @file lc_compiler.h
 * @brief The brief introduction for this file
 * @author LiJun
 *
 * The detail information about this file.
 */
#ifndef _LINUXCOMPAT_COMPILER_H_
#define _LINUXCOMPAT_COMPILER_H_

/**************************************************************************
 *                              M A C R O S
 **************************************************************************/
#define __user
#define __force
#define __iomem
#define __acquires(x)
#define __releases(x)
#define __percpu
#define __rcu
#define __init

#define likely(x)		(x)
#define unlikely(x)		(x)
#define barrier() 		asm volatile ( "" : : : "memory" )

#define STR_LINE_STRING2(x)	#x
#define STR_LINE_STRING(x)	STR_LINE_STRING2(x)
#define __STR_LINE__		STR_LINE_STRING(__LINE__)
#define kstrtoul(s,b,d)		strtoul(s,(char **)d,b)

/*
 * Prevent the compiler from merging or refetching accesses.  The compiler
 * is also forbidden from reordering successive instances of ACCESS_ONCE(),
 * but only when the compiler is aware of some particular ordering.  One way
 * to make the compiler aware of ordering is to put the two invocations of
 * ACCESS_ONCE() in different C statements.
 *
 * This macro does absolutely -nothing- to prevent the CPU from reordering,
 * merging, or refetching absolutely anything at any time.  Its main intended
 * use is to mediate communication between process-level code and irq/NMI
 * handlers, all running on the same CPU.
 */
#define ACCESS_ONCE(x)		(*(volatile typeof(x) *)&(x))

#ifndef __must_check
#define __must_check
#endif

/* Indirect stringification.  Doing two levels allows the parameter to be a
 * macro itself.  For example, compile with -DFOO=bar, __stringify(FOO)
 * converts to "bar".
 */

#define __stringify_1(x...)	#x
#define __stringify(x...)	__stringify_1(x)

#define noinline
#define __visible

#ifndef __pure
#define __pure			__attribute__((pure))
#endif
#ifndef __aligned
#define __aligned(x)		__attribute__((aligned(x)))
#endif
#define __printf(a, b)		__attribute__((format(printf, a, b)))
#define __scanf(a, b)		__attribute__((format(scanf, a, b)))
#define __attribute_const__	__attribute__((__const__))
#define __maybe_unused		__attribute__((unused))
#define __always_unused		__attribute__((unused))

#endif /* _LINUXCOMPAT_COMPILER_H_ */
