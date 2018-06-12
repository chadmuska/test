#ifndef __MAC80211_SKBUFF_H__
#define __MAC80211_SKBUFF_H__

#include <linux/types.h>
#include <linux/socktypes.h>
#include <ndk_types.h>
#include <ndk/debug.h>
#include <ndk/misc.h>
#include <ndk/skbuff.h>

struct sk_buff {
	SKBC_MEMBERS;
	struct sock* sk;
	UINT32 priority;
	UINT16 queue_mapping;
	UINT8  ip_summed;
} __attribute__((aligned(4)));

#endif
