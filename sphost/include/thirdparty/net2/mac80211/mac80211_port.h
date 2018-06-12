/**************************************************************************
 *
 *       Copyright (c) 2009-2016 by iCatch Technology, Inc.
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
 *                                                                        *
 **************************************************************************/
#ifndef __MAC80211_PORT_H__
#define __MAC80211_PORT_H__

#ifndef ABS_MOD_TIMER
#error  "ABS_MOD_TIMER not defined"
#endif

/* Do not use the kmalloc, kfree, ..., functions defined in in lc_slab.h */
#define NO_OSS_KMEM
// Use NULL as invalid list pointer
#define LIST_POISON_NULL
/* Enable NDK sk_buff_head */
#define NDK_SKBUFF_HEAD

#include <string.h>
#include <stdlib.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pm_config.h>
#include <linux/errno.h>
#include <linux/socktypes.h>
#include <linux/notifier.h>

#define CONFIG_MAC80211_RC_DEFAULT		"minstrel"
#define CONFIG_NET

#ifndef IS_ENABLED
#define IS_ENABLED(x)				(0)
#endif

#define MSEC_PER_SEC				1000L

#define __bitwise__
#define __net_exit
#define __acquire(x)				((void)0)
#define __release(x)				((void)0)
#define synchronize_net()			((void)0)
#define ASSERT_RTNL()				((void)0)

#define lockdep_assert_held(x)			((void)0)

#define DECLARE_BITMAP(name,bits)		unsigned long name[BITS_TO_LONGS(bits)]

/** RCU **/
#define __rcu
#define rcu_barrier()				((void)0)
#define rcu_read_lock()				((void)0)
#define rcu_read_unlock()			((void)0)
#define rcu_access_pointer(p)			(p)
#define rcu_dereference_check(p, c)		(p)
#define rcu_dereference_rtnl(p)			rcu_dereference_check(p, lockdep_rtnl_is_held())
#define rcu_dereference_protected(p, c)		(p)
#define rcu_dereference_raw(p)			(p)
#define RCU_INIT_POINTER(p, v)			((p) = (v))
#define rcu_assign_pointer(p, v) 		((p) = (v))
#define kfree_rcu(p, r)				((void)0)
#define synchronize_rcu()			((void)0)

/** RTNL **/
#define rtnl_lock()				((void)0)
#define rtnl_unlock()				((void)0)
#define rtnl_is_locked()			(0)
#define rtnl_dereference(p)			(p)

#define MAC80211_ASSERT(v)			do { if (!(v)) { oss_assert(__FILE__, __LINE__, NULL); } } while(0)

#define mac80211_error(fmt...)			ndk_error(fmt)
#define mac80211_info(fmt...)			ndk_info(fmt)

#define MAC80211_DBG_ON				0

#if MAC80211_DBG_ON
#define mac80211_dbg_print(fmt...)		ndk_info(fmt)
#define mac80211_dbg_callstack()		callStack()
#define mac80211_dbg_memdump(mem, len)		ndk_hexdump(NULL, mem, len, len)
#else
#define mac80211_dbg_print(fmt...)		((void)0)
#define mac80211_dbg_callstack()		((void)0)
#define mac80211_dbg_memdump(mem, len)		((void)0)
#endif

/* structures */
struct dentry { int nothing; };

/* functions */
void* cfg80211_malloc(size_t size, BOOL zero);
void  cfg80211_free(void *ptr);

static inline void *kmalloc(size_t size, int flags) { return cfg80211_malloc(size, flags | __GFP_ZERO); }
static inline void  kfree(const void *ptr) { cfg80211_free((void*)ptr); }

static inline void *kzalloc(size_t size, int flags) { return cfg80211_malloc(size, TRUE); }
static inline void *kcalloc(size_t n, size_t size, gfp_t flags) { return kmalloc(size*n, flags); }
static inline void  kzfree(const void *ptr) { kfree(ptr); }

static inline void* vmalloc(size_t size) { return kmalloc(size, 0); }
static inline void  vfree(const void *ptr) { kfree(ptr); }

static inline void *kmemdup(const void *src, size_t len, gfp_t gfp) {
	void *p = kmalloc(len, 0);
	if (p) memcpy(p, src, len);
	return p;
}

int cfg80211_global_init(const char *options);
int mac80211_global_init(const char *options);

#define ALIGN_ASSERT(p, n) do {\
	u32 tmp123_p = (u32)(p); \
	if (tmp123_p & ((n)-1)) { \
		printf("*** align4 assert: 0x%08x ***\n", tmp123_p); \
		NDK_ASSERT(0); \
	} \
} while(0)

#define ALIGN_FAIL(p, n)	((u32)(p) & ((n)-1))

#endif
