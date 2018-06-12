/**************************************************************************
 *
 *       Copyright (c) 2006-2015 by iCatch Technology, Inc.
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
 *  Hsin-Chu, Taiwan, R.O.C.
 *
 **************************************************************************/

#ifndef __NDK_SKBUFF_H__
#define __NDK_SKBUFF_H__

#define NETCFG_SKBC_DBG_ON              0
#define NETCFG_SKBC_DBG_CALLSTACK       0

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Address
typedef union {
	unsigned short		family;
	struct { // struct sockaddr
		unsigned short  sa_family;
		char            sa_data[14];
	};
	struct { // struct sockaddr_ll
		unsigned short  sll_family;
		UINT16          sll_protocol;
		int             sll_ifindex;
		unsigned short  sll_hatype;
		unsigned char   sll_pkttype;
		unsigned char   sll_halen;
		unsigned char   sll_addr[8];
	};
	struct { // struct sockaddr_in
		unsigned short  sin_family;
		UINT16          sin_port;
		struct in_addr  sin_addr;
		char            sin_zero[8];
	};
	struct { // struct sockaddr_un
		unsigned short  sun_family;   /*AF_UNIX*/
		const char     *sun_path;     /*path name*/
	};
	struct { // sockaddr_nl
		unsigned short  nl_family;
		unsigned short  nl_pad;
		UINT32          nl_pid;
		UINT32          nl_groups;
	};
	// Bluetooth
	struct { // sockaddr_hci
		unsigned short  hci_family;
		unsigned short  hci_dev;
		unsigned short  hci_channel;
	};
	struct { // l2cap: sockaddr_l2
		unsigned short  l2_family;
		unsigned short  l2_psm;
		struct bdaddr   l2_bdaddr;
		unsigned short  l2_cid;
		UINT8		l2_bdaddr_type;
	};
	struct { // struct sockaddr_rc
		unsigned short  rc_family;
		struct bdaddr   rc_bdaddr;
		unsigned char   rc_channel;
	};
	struct { // struct sockaddr_alg
		unsigned short  salg_family;
		const char     *salg_type;
		UINT32          salg_feat;
		UINT32          salg_mask;
		const char     *salg_name;
	};
} SocketAddress, *PSocketAddress;

///////////////////////////////////////////////////////////////////////////////
// io vector
enum {
	IOV_TYPE_BUF    = 0,
	IOV_TYPE_MSG    = 1,
};

typedef struct iovec {
	void           *iov_base;
	size_t          iov_len;
	unsigned        iov_type;
} iovec_t;

///////////////////////////////////////////////////////////////////////////////
// sk_buff core
struct sk_buff;
struct net_device;
typedef struct sk_buff *pskb_t, **ppskb_t;

#if NETCFG_SKBC_DBG_CALLSTACK
enum { SKBC_DBG_CALLSTACK_NUM = 6, SKBC_DBG_CALLSTACK_DEPTH = 12 };
struct skbc_callstack_item {
	char   state; /* 0: idle; 1:ref; 2: unref */
	UINT32 callstack[SKBC_DBG_CALLSTACK_DEPTH];
};

#define SKBC_DBG_CALLSTACK      ; struct skbc_callstack_item *csitems
#else  /* NETCFG_SKBC_DBG_CALLSTACK */
#define SKBC_DBG_CALLSTACK
#endif /* NETCFG_SKBC_DBG_CALLSTACK */

#if NETCFG_SKBC_DBG_ON
#include <linux/list.h>
#define SKBC_DBG_MEMBERS \
	; list_node_t  skbc_dbg_list

#else
#define SKBC_DBG_MEMBERS
#endif

// Must be the first member in your sk_buff definition.
#define SKBC_MEMBERS \
	struct sk_buff *next, *prev \
	; SocketAddress skbc_srcaddr \
	; skbc_delete_func_t skbc_delete_func \
	; struct net_device *dev \
	; SINT8  skbc_refcnt \
	; UINT8  skbc_hdr_sz/* multiple of 4 */ \
	; UINT16 protocol \
	; UINT16 skbc_index \
	; UINT8  pkt_type:3 \
	; UINT8  l3_alloc:1 \
	; UINT32 len \
	; UINT16 transport_header \
	; UINT16 network_header \
	; UINT16 mac_header \
	; UINT8  *head, *end \
	; UINT8  *data, *tail \
	; char cb[48] __attribute__((aligned(8))); \
	SKBC_DBG_MEMBERS SKBC_DBG_CALLSTACK

struct skb_core;

typedef void (*skbc_delete_func_t)(struct skb_core*);

typedef struct skb_core
{
	SKBC_MEMBERS;
} skbc_t, *pskbc_t;

/* sizeof(struct sk_buff) */
extern unsigned sizeof_skbuff;

/* Exported SK-Buff Functions */
pskbc_t	skbc_alloc(size_t payload, skbc_delete_func_t delfunc);

// Callback for skbc_clone and skbc_copy_expand which allocate sk_buff with the netif sk_buff allocate function.
pskbc_t __netdrv_alloc_skb(const pskbc_t skb, size_t len);

// Real clone. Copy data.
pskbc_t skbc_clone(const pskbc_t skb, pskbc_t (*pfn_alloc_skb)(const pskbc_t skb, size_t len));

// Initialize header variables. Set data length to 0.
// |<--header-->|head<------------------>end|
//              |data/tail
void	skbc_init_header(
	pskbc_t skbc,
	size_t  skbc_hdr_size,
	UINT8 * buf_head,
	size_t  payload_size,
	skbc_delete_func_t delete_func);

int	skbc_copy_payload(pskbc_t dest, const pskbc_t src);
int	skbc_copy_bits(const pskbc_t skbc, int offset, void *to, int len);
int	skbc_copy_head(pskbc_t dest, const pskbc_t src);
pskbc_t skbc_copy_expand(
	const pskbc_t skb,
	int   newheadroom,
	int   newtailroom,
	pskbc_t (*pfn_alloc_skb)(const pskbc_t skb, size_t len));

// Call the skbc_delete_func function to free the memory.
void	skbc_delete(pskbc_t skbc);

// Free the skb memory.
void	skbc_mem_free(pskbc_t skbc);

pskbc_t	skbc_ref(pskbc_t skbc);

// Decrease the refcnt but not delete it.
SINT16	skbc_unref(pskbc_t skbc);

// Decrease the refcnt and delete the skbc object if refcnt == 0.
static inline void __skbc_unref(pskbc_t skb) { if (skb && skbc_unref(skb) == 0) skbc_delete(skb); }

#define SKBC_PTR(skb)   ((pskbc_t)(skb))

/*
 * Valid sk_buff structure must be like:
 *                                   |headroom |             |tailroom|
 * |<==skbc_hdr==>|<--extended_hdr-->|head<==>data<=======>tail<==>end|
 * head <= data <= tail <= end
 */
int	skbc_sanity_check(pskbc_t skbc);
#define SKBC_SANITY_CHECK(skb)	NDK_ASSERT(skbc_sanity_check((pskbc_t)skb) == 0)

#define SKBC_HEAD(skb)		(SKBC_PTR(skb)->head)
#define SKBC_END(skb)		(SKBC_PTR(skb)->end)
#define SKBC_DATA(skb)		(SKBC_PTR(skb)->data)
#define SKBC_TAIL(skb)		(SKBC_PTR(skb)->tail)
#define SKBC_LEN(skb)		(SKBC_PTR(skb)->len)
#define SKBC_DEV(skb)		(SKBC_PTR(skb)->dev)
#define SKBC_HDR_SIZE(skb)	(SKBC_PTR(skb)->skbc_hdr_sz * 4)

/******************************************************************************
 * sk_buff functions
 *****************************************************************************/
static inline void skb_reserve(struct sk_buff *skb, int len)
{
	SKBC_DATA(skb) += len;
	SKBC_TAIL(skb) += len;
	NDK_BUG_ON(SKBC_TAIL(skb) > SKBC_END(skb));
}

static inline void __skb_trim(struct sk_buff *skb, unsigned int len)
{
	SKBC_LEN(skb) = len;
	SKBC_TAIL(skb) = SKBC_DATA(skb) + len;
}

static inline void skb_trim(struct sk_buff *skb, unsigned int len)
{
	if (SKBC_LEN(skb) > len)
		__skb_trim(skb, len);
}

static inline unsigned char *skb_put(struct sk_buff *skb, unsigned int len)
{
	unsigned char *tmp = SKBC_TAIL(skb);
	SKBC_TAIL(skb) += len;
	SKBC_LEN(skb)  += len;
	NDK_BUG_ON(SKBC_TAIL(skb) > SKBC_END(skb));
	return tmp;
}

static inline unsigned char *skb_push(struct sk_buff *skb, unsigned int len)
{
	SKBC_DATA(skb) -= len;
	SKBC_LEN(skb)  += len;
	NDK_BUG_ON(SKBC_DATA(skb) < SKBC_HEAD(skb));
	return SKBC_DATA(skb);
}

static inline unsigned char *__skb_pull(struct sk_buff *skb, unsigned int len)
{
	NDK_BUG_ON(SKBC_LEN(skb) < len);
	SKBC_LEN(skb) -= len;
	return SKBC_DATA(skb) += len;
}

static inline unsigned char *skb_pull(struct sk_buff *skb, unsigned int len)
{
	return (len > SKBC_LEN(skb)) ? NULL : __skb_pull(skb, len);
}

static inline int skb_headroom(const struct sk_buff *skb)
{
	return SKBC_DATA(skb) - SKBC_HEAD(skb);
}

static inline int skb_tailroom(const struct sk_buff *skb)
{
	return SKBC_END(skb) - SKBC_TAIL(skb);
}

static inline BOOL skb_transport_header_was_set(const struct sk_buff *skb)
{
	return SKBC_PTR(skb)->transport_header != (UINT16)~0U;
}

static inline unsigned char *skb_transport_header(const struct sk_buff *skb)
{
	return SKBC_HEAD(skb) + SKBC_PTR(skb)->transport_header;
}

static inline void skb_reset_transport_header(struct sk_buff *skb)
{
	SKBC_PTR(skb)->transport_header = SKBC_DATA(skb) - SKBC_HEAD(skb);
}

static inline void skb_set_transport_header(struct sk_buff *skb,
					    const int offset)
{
	skb_reset_transport_header(skb);
	SKBC_PTR(skb)->transport_header += offset;
}

static inline unsigned char *skb_network_header(const struct sk_buff *skb)
{
	return SKBC_HEAD(skb) + SKBC_PTR(skb)->network_header;
}

static inline void skb_reset_network_header(struct sk_buff *skb)
{
	SKBC_PTR(skb)->network_header = SKBC_DATA(skb) - SKBC_HEAD(skb);
}

static inline void skb_set_network_header(struct sk_buff *skb, const int offset)
{
	skb_reset_network_header(skb);
	SKBC_PTR(skb)->network_header += offset;
}

static inline unsigned char *skb_mac_header(const struct sk_buff *skb)
{
	return (UINT8*)SKBC_HEAD(skb) + SKBC_PTR(skb)->mac_header;
}

static inline int skb_mac_header_was_set(const struct sk_buff *skb)
{
	return SKBC_PTR(skb)->mac_header != (typeof(SKBC_PTR(skb)->mac_header))~0U;
}

static inline void skb_reset_mac_header(struct sk_buff *skb)
{
	SKBC_PTR(skb)->mac_header = (UINT8*)SKBC_DATA(skb) - (UINT8*)SKBC_HEAD(skb);
}

static inline void skb_set_mac_header(struct sk_buff *skb, const int offset)
{
	skb_reset_mac_header(skb);
	SKBC_PTR(skb)->mac_header += offset;
}

static inline void skb_pop_mac_header(struct sk_buff *skb)
{
	SKBC_PTR(skb)->mac_header = SKBC_PTR(skb)->network_header;
}

static inline int skb_transport_offset(const struct sk_buff *skb)
{
	return skb_transport_header(skb) - SKBC_DATA(skb);
}

static inline u32 skb_network_header_len(const struct sk_buff *skb)
{
	return SKBC_PTR(skb)->transport_header - SKBC_PTR(skb)->network_header;
}

static inline int skb_network_offset(const struct sk_buff *skb)
{
	return skb_network_header(skb) - SKBC_DATA(skb);
}

static inline BOOL skb_is_nonlinear(const struct sk_buff *skb)
{
	return FALSE; /* Always false */
}

static inline unsigned char *skb_end_pointer(const struct sk_buff *skb)
{
	return SKBC_END(skb);
}

static inline int skb_end_offset(const struct sk_buff *skb)
{
	return SKBC_END(skb) - SKBC_HEAD(skb);
}

static inline unsigned char *skb_tail_pointer(const struct sk_buff *skb)
{
	return SKBC_TAIL(skb);
}

static inline void skb_reset_tail_pointer(struct sk_buff *skb)
{
	SKBC_TAIL(skb) = SKBC_DATA(skb);
}

static inline void skb_set_tail_pointer(struct sk_buff *skb, const int offset)
{
	SKBC_TAIL(skb) = SKBC_DATA(skb) + offset;
}

static inline void skb_headers_offset_update(struct sk_buff *skb, int off)
{
	SKBC_PTR(skb)->transport_header += off;
	SKBC_PTR(skb)->network_header   += off;
	if (skb_mac_header_was_set((pskb_t)skb))
		SKBC_PTR(skb)->mac_header += off;
}

void skbc_dump(pskbc_t skbc, const char *func, int line);
#define skb_dump(skb)		skbc_dump((pskbc_t)(skb), __func__, __LINE__)

#if NETCFG_SKBC_DBG_ON
void skb_core_dump_info();
#else
#define skb_core_dump_info()	((void)0)
#endif

/******************************************************************************
 * sk_buff queue
 *****************************************************************************/
#define SKB_LIST_MEMBERS \
	pskb_t next;	 \
	pskb_t prev;	 \
	int    qlen

typedef struct skb_list {
	SKB_LIST_MEMBERS;
} skb_list_t;

struct sk_buff_head;

#define SKBLIST_PTR(list)	((skb_list_t*)(list))

/* SKB list functions */
void	skb_list_init(skb_list_t *list);
void	skb_list_deinit(skb_list_t *list);

size_t	skb_list_datalen(skb_list_t *list);

void	skb_list_insert(skb_list_t *list, pskb_t prev, pskb_t next, pskb_t newsk);

void	skb_list_unlink(skb_list_t *list, pskb_t skb);
void	skb_list_unlink_safe(skb_list_t *list, pskb_t skb);

void	skb_list_purge(skb_list_t *list);
void	skb_list_purge_safe(skb_list_t *list);

void	skb_list_splice(skb_list_t *list, pskb_t prev, pskb_t next);
void	skb_list_splice_head(skb_list_t *list, skb_list_t *joined);
void	skb_list_splice_tail(skb_list_t *list, skb_list_t *joined);

pskb_t	skb_list_pop_head(skb_list_t *list);
pskb_t	skb_list_pop_head_safe(skb_list_t *list);

pskb_t	skb_list_pop_tail(skb_list_t *list);
pskb_t	skb_list_pop_tail_safe(skb_list_t *list);

static inline void skb_list_insert_head(skb_list_t *list, pskb_t nskb) { skb_list_insert(list, (pskb_t)list, list->next, nskb); }
void	skb_list_insert_head_safe(skb_list_t *list, pskb_t nskb);

static inline void skb_list_insert_tail(skb_list_t *list, pskb_t nskb) { skb_list_insert(list, list->prev, (pskb_t)list, nskb); }
void	skb_list_insert_tail_safe(skb_list_t *list, pskb_t nskb);

static inline int skb_list_is_empty(const skb_list_t *list) { return list->next == (pskb_t)list; }

pskb_t	skb_list_get_head(skb_list_t *list);
pskb_t	skb_list_get_tail(skb_list_t *list);

/*
 * sk_buff queue
 */
/* The skb_queue_head_init will init the lock. Not implemented here. */
#define __skb_queue_head_init(list)		skb_list_init(SKBLIST_PTR(list))

#define __skb_unlink(skb, list)			skb_list_unlink(SKBLIST_PTR(list), skb);
#define skb_unlink(skb, list)			skb_list_unlink_safe(SKBLIST_PTR(list), skb);

#define skb_peek(list)				skb_list_get_head(SKBLIST_PTR(list))
#define skb_peek_tail(list)			skb_list_get_tail(SKBLIST_PTR(list))

#define __skb_queue_head(list, nskb)		skb_list_insert_head(SKBLIST_PTR(list), nskb)
#define skb_queue_head(list, nskb)		skb_list_insert_head_safe(SKBLIST_PTR(list), nskb)

#define __skb_queue_tail(list, nskb)		skb_list_insert_tail(SKBLIST_PTR(list), nskb)
#define skb_queue_tail(list, nskb)		skb_list_insert_tail_safe(SKBLIST_PTR(list), nskb)

#define __skb_dequeue(list)			skb_list_pop_head(SKBLIST_PTR(list))
#define skb_dequeue(list)			skb_list_pop_head_safe(SKBLIST_PTR(list))

#define __skb_dequeue_tail(list)		skb_list_pop_tail(SKBLIST_PTR(list))
#define skb_dequeue_tail(list)			skb_list_pop_tail_safe(SKBLIST_PTR(list))

#define skb_queue_purge(list)			skb_list_purge(SKBLIST_PTR(list))
#define __skb_queue_purge(list)			skb_list_purge_safe(SKBLIST_PTR(list))

#define skb_queue_splice(list, head)		skb_list_splice_head(SKBLIST_PTR(head), SKBLIST_PTR(list))
#define skb_queue_splice_tail(list, head)	skb_list_splice_tail(SKBLIST_PTR(head), SKBLIST_PTR(list))

#define skb_queue_empty(list)			skb_list_is_empty(SKBLIST_PTR(list))
#define skb_queue_len(list)			((SKBLIST_PTR(list))->qlen)
#define skb_queue_is_last(list, skb)		(SKBC_PTR(skb)->next == (pskb_t)(SKBLIST_PTR(list)))
#define skb_queue_next(list, skb)		(SKBC_PTR(skb)->next)

#define __skb_queue_splice(list, prev, next)	skb_list_splice(SKBLIST_PTR(list), (pskb_t)(prev), (pskb_t)(next))

#define skb_queue_datalen(list)			skb_list_datalen(SKBLIST_PTR(list))

#define skb_queue_walk(queue, skb)			\
	for (skb = (queue)->next;			\
	     skb != (struct sk_buff *)(queue);		\
	     skb = skb->next)

#define skb_queue_walk_safe(queue, skb, tmp)		\
	for (skb = (queue)->next, tmp = skb->next;	\
	     skb != (pskb_t )(queue);			\
	     skb = tmp, tmp = skb->next)

#define skb_queue_walk_from(queue, skb)			\
	for (; skb != (struct sk_buff *)(queue);	\
	     skb = skb->next)

static inline void skb_queue_splice_init(struct sk_buff_head *list,
					 struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(SKBLIST_PTR(list), (struct sk_buff *) head, SKBLIST_PTR(head)->next);
		SKBLIST_PTR(head)->qlen += SKBLIST_PTR(list)->qlen;
		__skb_queue_head_init(list);
	}
}

static inline void skb_queue_splice_tail_init(struct sk_buff_head *list,
					      struct sk_buff_head *head)
{
	if (!skb_queue_empty(list)) {
		__skb_queue_splice(SKBLIST_PTR(list), SKBLIST_PTR(head)->prev, (struct sk_buff *) head);
		SKBLIST_PTR(head)->qlen += SKBLIST_PTR(list)->qlen;
		__skb_queue_head_init(list);
	}
}

/* skb queue implementation with NDKSpinLock */
#ifdef NDK_SKBUFF_HEAD
#include <linux/spinlock.h>

struct sk_buff_head {
	SKB_LIST_MEMBERS;
	mutex_spinlock_t lock;
};

#define __skb_queue_lock(list)			spin_lock(&(list)->lock)
#define __skb_queue_unlock(list)		spin_unlock(&(list)->lock)

#define skb_queue_spin_lock_bh(lock)		spin_lock_bh(lock)
#define skb_queue_spin_unlock_bh(lock)		spin_unlock_bh(lock)

#define skb_queue_spin_lock_irqsave(lock, f)	spin_lock_irqsave(lock, f)
#define skb_queue_spin_unlock_irqrestore(lock, f) spin_unlock_irqrestore(lock, f)

static inline void skb_queue_head_init(struct sk_buff_head *list) {
	spin_lock_init(&list->lock);
	__skb_queue_head_init(list);
}
#endif

#ifdef __cplusplus
}
#endif

#endif

