/**************************************************************************
 *
 *       Copyright (c) 2011 by iCatch Technology, Inc.
 *
 *  This software is copyrighted by and is the property of iCatch Technology,
 *  Inc.. All rights are reserved by iCatch Technology, Inc..
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

#ifndef __NDK_SOCK_NET_H__
#define __NDK_SOCK_NET_H__

#ifdef __cplusplus
extern "C" {
#endif

enum { 	SKC_FLG_USER = 0x01 };

struct sock;
struct socket;
typedef struct socket *psocket_t;

#define SOCKET_CORE_MEMBERS			\
	char		skc_name[12];		\
	list_node_t 	skc_list;		\
	UINT16		skc_family;		\
	UINT16		skc_type;		\
	UINT16		skc_protocol;		\
	SINT16		skc_fd;			\
	UINT32		skc_soflags;		\
	int		skc_sndtimeo;		\
	SP5K_QUEUE	skc_receive_queue;	\
	pskbc_t		skc_rx_skb;		\
	void		(*skc_destruct)(struct sock* sk); \
	int             (*skc_rx_skb_handler)(struct sock* sk, pskbc_t skbc); \
	BOOL		(*skc_has_read_event)(struct sock* sk, BOOL *closed); \
	const struct	SockProtoOps *skc_ops;	\
	SINT8		skc_refcnt;		\
	UINT8		skc_flags;		\
	UINT16		skc_skb_headroom;	\
	UINT16		skc_skb_tailroom;	\
	void *		skc_priv;		\
	uintptr_t	skc_stack[8]

typedef struct
{
	SOCKET_CORE_MEMBERS;
} SockInfo;
typedef SockInfo *PSockInfo;

/*
 * socket protocol operations
 */
struct SockProtoOps {
	UINT16 family;
	const char *name;
	int (*release)   (psocket_t sk);
	int (*bind)      (psocket_t sk, PSocketAddress addr);
	int (*connect)   (psocket_t sk, PSocketAddress addr, int flags);
	int (*accept)    (psocket_t sk, psocket_t *newsock, int flags);
	int (*getname)   (psocket_t sk, PSocketAddress addr, int peer, size_t *addrlen);

	int (*ioctl)     (psocket_t sk, unsigned int cmd, unsigned long arg);

	int (*listen)    (psocket_t sk, int len);
	int (*shutdown)  (psocket_t sk);

	int (*setsockopt)(psocket_t sk, int level, int optname, const char *optval, socklen_t optlen);
	int (*getsockopt)(psocket_t sk, int level, int optname, char *optval, socklen_t *optlen);
	// Get length of the data in the transmitting queue.
	int (*gettxdatalen)(psocket_t sk);
	// \return < 0: error; > 0: succeed and length to send.
	int (*sendmsg)   (psocket_t sk, const struct iovec *iov, int iovsz, PSocketAddress dest);
	// \return < 0: error; 0: succeed.
	int (*sendskb)   (psocket_t sk, pskbc_t skbc, PSocketAddress dest);
	// \return < 0: error; 0, receive skb and skb->len > 0 (not empty)
	int (*recvmsg)   (psocket_t sk, pskbc_t *skbc, int timeout);
	void (*dump)     (psocket_t sk);
};

struct SockProtoFamily {
	unsigned short family;
	size_t (*get_proto_size)(int protocol);
	int    (*create)(psocket_t sk, int protocol);
};

#define SOCKINFO_PTR(sk)	((PSockInfo)(sk))

extern int sock_rcv_queue_purege(SP5K_QUEUE *q);

extern PSockInfo ndk_sock_create(int family, int type, int protocol);

// Note: The refcnt of the socket is initialized to 0. Call ndk_sock_ref to increase to 1.
extern PSockInfo ndk_sock_alloc(const struct SockProtoFamily* pf, int type, int protocol);

extern PSockInfo ndk_sock_ref(PSockInfo sk);
extern int  ndk_sock_unref(PSockInfo sk);

/*
 * sock functions
 */
extern int  ndk_sock_accept(PSockInfo sk, PSockInfo *nsk);

extern int  ndk_sock_sendto_iovec(PSockInfo sk, const struct iovec *iov, int iovsz, PSocketAddress to);
extern int  ndk_sock_send_iovec(PSockInfo sk, const struct iovec *iov, int iovsz);
static inline int ndk_sock_send(PSockInfo sk, const void* data, size_t size) {
	iovec_t iov[1] = { {(void*)data, size} };
	return ndk_sock_send_iovec(sk, iov, 1);
}

extern int ndk_sock_sendto_skb(PSockInfo sk, pskbc_t skbc, PSocketAddress to);

/* Process receiving sk-buff */
extern int  ndk_sock_queue_rcv_skb(PSockInfo sk, pskbc_t skbc);
extern int  ndk_sock_queue_rcv_skb_timeout(PSockInfo sk, pskbc_t skbc, UINT32 timeout);
extern int  ndk_sock_queue_purge(PSockInfo sk);

extern int  ndk_sock_getsockname(PSockInfo sk, PSocketAddress addr, size_t *addrlen);
extern int  ndk_sock_getpeername(PSockInfo sk, PSocketAddress addr, size_t *addrlen);

extern BOOL ndk_sock_has_read_event(PSockInfo sk, BOOL *closed);

// timeout: milli-seconds. -1: wait forever.
// default recvmsg handler
extern int  ndk_sock_recvmsg_handler(psocket_t sk, pskbc_t *skb, int timeout);

// timeout: milli-seconds. -1: wait forever.
// Return: < 0: error; 0 OK.
extern int  ndk_sock_get_skb(PSockInfo sk, pskbc_t *skb, int timeout);

extern int  ndk_sock_gettxdatalen(PSockInfo sk);

extern int  ndk_sock_wakeup(PSockInfo sk);

// call the socket release function
extern int  ndk_sock_release(PSockInfo sk);

// free members of socket core. refcnt must be 0.
extern void ndk_sock_delete(PSockInfo sk);

extern PSockInfo ndk_sock_getptr(int skfd);
extern pskbc_t ndk_sock_alloc_skbuff(PSockInfo sk, size_t payload_size);

extern size_t iovec_get_len(const struct iovec *iov, int iovlen);

/* return: bytes copied. */
extern size_t memcpy_fromiovec(
	void   *dest,   /* OUT. Destination memory. */
	const   struct iovec *iov, /* IN. IO vector array. */
	size_t  iovsz,  /* IN. Size of the IO vector array. */
	/* IN/OUT. Start address to copy. Changed to the next address after copying:
	 *   *offset += return-value.
	 * NULL: copy from the begining of the IO vector.
	 */
	size_t *offset,
	size_t  count   /* IN. Bytes to be copied. */
);

extern void ndk_sock_register_proto(struct SockProtoFamily* family);

extern void ndk_sock_internal_lock();
extern void ndk_sock_internal_unlock();

// Don't free the string returned.
extern const char* ndk_sock_dump_address(PSocketAddress addr);
extern const char *ndk_sock_get_desc(PSockInfo sk);
extern void ndk_sock_dump(PSockInfo sk);

/* NETLINK socket special functions */
/* user port id starts from 100 */
extern UINT32 ndk_nlsock_alloc_user_portid();
extern int ndk_nlsock_unicast(PSockInfo ssk, pskbc_t skbc, UINT32 portid);
extern int ndk_nlsock_broadcast(PSockInfo src_sk, UINT32 src_pid, pskbc_t skbc, UINT32 group);
extern PSockInfo ndk_nlsock_lookup(int protocol, int port_id);
extern int ndk_nlsock_sendto_sock(PSockInfo ssk, pskbc_t skbc, PSockInfo dsk);

#ifdef __cplusplus
}
#endif

#endif
