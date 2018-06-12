/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2012 by Sunplus Technology Co., Ltd.             *
 *                                                                        *
 *  This software is copyrighted by and is the property of Sunplus        *
 *  Technology Co., Ltd. All rights are reserved by Sunplus Technology    *
 *  Co., Ltd. This software may only be used in accordance with the       *
 *  corresponding license agreement. Any unauthorized use, duplication,   *
 *  distribution, or disclosure of this software is expressly forbidden.  *
 *                                                                        *
 *  This Copyright notice MUST not be removed or modified without prior   *
 *  written consent of Sunplus Technology Co., Ltd.                       *
 *                                                                        *
 *  Sunplus Technology Co., Ltd. reserves the right to modify this        *
 *  software without notice.                                              *
 *                                                                        *
 *  Sunplus Technology Co., Ltd.                                          *
 *  19, Innovation First Road, Science-Based Industrial Park,             *
 *  Hsin-Chu, Taiwan, R.O.C.                                              *
 *                                                                        *
 **************************************************************************/
#ifndef __NDK_INNER_SOCKET_H__
#define __NDK_INNER_SOCKET_H__

#include <ndk/skbuff.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Supported address families. */
enum {
	NDK_AF_UNIX	= 0,	/* Unix domain sockets 		*/
	NDK_AF_LOCAL,		/* POSIX name for AF_UNIX	*/
	NDK_AF_NETLINK,
	NDK_AF_PACKET,		/* Packet family		*/
	NDK_AF_BLUETOOTH,	/* Bluetooth sockets 		*/
	NDK_AF_ALG,		/* Algorithm */

	NDK_SOCKET_FAMILY_NR
};

enum {
	NDK_SOL_SOCKET = 0x10000,
	NDK_SOL_NETLINK,

	NDK_SO_RCVCLOSED = 0,
	NDK_SO_SNDTIMEO,
	NDK_SO_DOMAIN,
	NDK_SO_TYPE,
	NDK_SO_PROTOCOL,
	NDK_SO_SKB_HEADROOM,	/* unsigned int */
	NDK_SO_SKB_TAILROOM,	/* unsigned int */
	NDK_SO_MONITOR,
};

/* Create a NDK socket.
 * Argumetns:
 *   family: can only be PF_PACKET, PF_NETLINK, PF_BLUETOOTH or PF_LOCAL
 *   type: not used. Always 0.
 *   protocol: socket protocol.
 * Return: socket fd. < 0: failed.
 */
extern int ndk_socket_create(int family, int type, int protocol);

/* Destroy a NDK socket. Free memory. */
extern int ndk_socket_destroy(int skfd);

/* Bind the NDK socket to an address.
 */
extern int ndk_socket_bind(int skfd, PSocketAddress addr, size_t addrlen);
extern int ndk_socket_connect(int skfd, PSocketAddress addr, size_t addrlen);

/* Send a socket message to destination 'to'. */
extern int ndk_socket_sendto_skb(int skfd, pskbc_t skbc, PSocketAddress to);

extern int ndk_socket_sendto_iovec(int skfd, const struct iovec *iov, int iovsz, PSocketAddress to);

static inline int ndk_socket_sendto(int skfd, const void* data, size_t size, PSocketAddress to, size_t addrlen) {
	iovec_t iov = {(void*)data, size};
	return ndk_socket_sendto_iovec(skfd, &iov, 1, to);
}

/* Send a socket message to the destination. Must connect first. */
extern int ndk_socket_send(int skfd, const void* data, size_t size);

static inline int ndk_socket_writev(int skfd, const struct iovec *iov, int iovsz) {
	return ndk_socket_sendto_iovec(skfd, iov, iovsz, NULL);
}

extern int ndk_socket_listen(int skfd, int backlog);
extern int ndk_socket_accept(int skfd, PSocketAddress addr, size_t *addrlen);

typedef struct {
	UINT32 bits;
} ndk_fd_set;

#define NDK_FD_SET(n, fds)		((fds)->bits |=  (1 << (n)))
#define NDK_FD_CLR(n, fds)		((fds)->bits &= ~(1 << (n)))
#define NDK_FD_ISSET(n,fds)		(!!((fds)->bits & (1 << (n))))
#define NDK_FD_ZERO(fds)		(fds)->bits=0

/*
 * Waiting until one socket in the socket set has message comming.
 * timeout: -1, wait forever. Unit: milli-seconds .
 */
extern int ndk_socket_select(int nfds, ndk_fd_set* rfds, int timeout);

extern int ndk_socket_select_interruptible(int nfds, ndk_fd_set* rfds, int timeout);

/* Poll one socket.
 * \return <0: error; 1: succeed; 0: timedout
 */
extern int ndk_socket_poll(int skfd, int timeout);

extern int ndk_socket_poll_event(int skfd, int timeout);

/*
 * Receive skbuff from socket.
 * The caller should release the skbuff object by skb_core_free function.
 * timeout: < 0, wait for ever; = 0, nowait; > 0, wait time.
 * Returns: 0, succeed; < 0, error.
 */
extern int ndk_socket_get_skb(int skfd, pskbc_t *skb, int timeout);

/*
 * Copy the data to buffer.
 * timeout: < 0, wait for ever; = 0, nowait; > 0, wait time.
 * flags: NDK_MSG_PEEK
 * Returns: < 0, error; 0, no data received; > 0, data size.
 */
enum
{
	NDK_SOCKET_MSG_PEEK = 0x01 << 0 // get the data and don't remove the data from socket receiving buffer
};
extern int ndk_socket_receive(int skfd, void* buf, size_t buflen
	, int timeout, unsigned int flags
	, PSocketAddress address);

static inline int ndk_socket_recv(int skfd, void* buf, size_t buflen)
{
	return ndk_socket_receive(skfd, buf, buflen, -1, 0, NULL);
}

static inline int ndk_socket_recvfrom(int skfd, void* buf, size_t buflen
	, unsigned int flags
	, PSocketAddress address)
{
	return ndk_socket_receive(skfd, buf, buflen, -1, flags, address);
}

/* Clear all messages in the queue */
extern int ndk_socket_queue_purge(int skfd);

/* call socket ioctl function */
extern int ndk_socket_ioctl(int skfd, int cmd, unsigned long arg);

/* configure device */
extern int ndk_device_ioctl(int cmd, unsigned long arg);

extern int ndk_socket_getsockopt(int skfd, int level, int optname, void *optval, socklen_t *optlen);
extern int ndk_socket_setsockopt(int skfd, int level, int optname, const void *optval, socklen_t optlen);

extern int ndk_socket_getpeername(int skfd, PSocketAddress addr, size_t *addrlen);
extern int ndk_socket_getsockname(int skfd, PSocketAddress addr, size_t *addrlen);

extern int ndk_socket_gettxdatalen(int skfd);

extern int ndk_socket_wakeup(int skfd);
extern int ndk_socket_wakeup_fds(ndk_fd_set *fds);
extern int ndk_socket_wakeup_all();

extern BOOL ndk_socket_is_hangup(int skfd);
extern BOOL ndk_socket_has_read_event(int skfd, BOOL *closed);
extern const char *ndk_socket_get_desc(int skfd);

extern void ndk_socket_dump(int skfd);
extern void ndk_socket_dump_all();

////////////////////////////////////////////////////////////////////////////////////////////////////
/* For driver to send wext_event to application through AF_NETLINK socket */
extern int ndk_socket_wext_event_rx(
	unsigned int if_index,
	unsigned int cmd,
	int      hdr_len,
	void *   wrqu, // Type: union iwreq_data
	int      wrqu_off,
	const    char *extra,
	int      extra_len);

/* For driver to send l2 packets to application through AF_PACKET socket */
extern int ndk_socket_l2_packet_rx(unsigned if_index, struct sk_buff *skb);

////////////////////////////////////////////////////////////////////////////////////////////////////
// NETLINK socket helper functions.
extern int ndk_nlsocket_create(int type, int protocol, UINT32 nl_pid, UINT32 nl_groups);

#ifndef NDK_NO_SOCKET_API
#ifdef FD_ZERO
#undef FD_ZERO
#undef FD_SET
#undef FD_CLR
#undef FD_ISSET
#undef fd_set
#endif

#define fd_set		ndk_fd_set
#define FD_ZERO		NDK_FD_ZERO
#define FD_SET		NDK_FD_SET
#define FD_CLR		NDK_FD_CLR
#define FD_ISSET	NDK_FD_ISSET

#define PF_UNIX		NDK_AF_UNIX
#define PF_LOCAL	NDK_AF_LOCAL
#define PF_NETLINK	NDK_AF_NETLINK
#define PF_PACKET	NDK_AF_PACKET
#define PF_BLUETOOTH	NDK_AF_BLUETOOTH
#define PF_ALG		NDK_AF_ALG

#define AF_UNIX		NDK_AF_UNIX
#define AF_LOCAL	NDK_AF_LOCAL
#define AF_NETLINK	NDK_AF_NETLINK
#define AF_PACKET	NDK_AF_PACKET
#define AF_BLUETOOTH	NDK_AF_BLUETOOTH
#define AF_ALG		NDK_AF_ALG

#define SOL_SOCKET	NDK_SOL_SOCKET
#define SOL_NETLINK	NDK_SOL_NETLINK
#define SO_RCVCLOSED	NDK_SO_RCVCLOSED
#define SO_SNDTIMEO	NDK_SO_SNDTIMEO
#define SO_DOMAIN	NDK_SO_DOMAIN
#define SO_TYPE		NDK_SO_TYPE
#define SO_PROTOCOL	NDK_SO_PROTOCOL

static inline int accept(int s, const void *addr, socklen_t *addrlen) { return ndk_socket_accept(s, (PSocketAddress)addr, (size_t*) addrlen); }
static inline int bind(int s, const void *addr, socklen_t addrlen) { return ndk_socket_bind(s, (PSocketAddress)addr, addrlen); }

static inline int closesocket(int s) { return ndk_socket_destroy(s); }
static inline int connect(int s, void *addr, size_t addrlen) { return ndk_socket_connect(s, (PSocketAddress)addr, addrlen); }
static inline int getsockname(int s, void *addr, socklen_t *addrlen) { return ndk_socket_getsockname(s, (PSocketAddress)addr, addrlen); }
static inline int getpeername(int s, void *addr, socklen_t *addrlen) { return ndk_socket_getpeername(s, (PSocketAddress)addr, addrlen); }

static inline int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen) { return ndk_socket_setsockopt(s, level, optname, optval, optlen); }
static inline int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen) { return ndk_socket_getsockopt(s, level, optname, optval, optlen); }

static inline int listen(int s, int backlog) { return ndk_socket_listen(s, backlog); }
static inline int recv(int s, void *mem, size_t len, int flags) { return ndk_socket_recv(s, mem, len); }
static inline int recvfrom(int s, void *mem, size_t len, int flags, void *from, size_t *fromlen) {
	if (fromlen && *fromlen) { NDK_ASSERT(*fromlen >= sizeof(SocketAddress)); }
	return ndk_socket_recvfrom(s, mem, len, (unsigned)flags, from);
}
static inline int send(int s, const void *data, size_t size, int flags) { return ndk_socket_send(s, data, size); }
static inline int sendto(int s, const void *data, size_t size, int flags, void *to, size_t tolen) { return ndk_socket_sendto(s, data, size, to, tolen); }
static inline int socket(int domain, int type, int protocol) { return ndk_socket_create(domain, type, protocol); }
static inline int select(int maxfdp1, ndk_fd_set *readset, ndk_fd_set *writeset, ndk_fd_set  *exceptset,  int timeout) { return ndk_socket_select(maxfdp1, readset, timeout); }
static inline int select_inerruptible(int maxfdp1, ndk_fd_set *readset, ndk_fd_set *writeset, ndk_fd_set  *exceptset, int timeout) { return ndk_socket_select_interruptible(maxfdp1, readset, timeout); }
static inline int ioctlsocket(int s, long cmd, unsigned long arg) { return ndk_socket_ioctl(s, cmd, arg); }

static inline int soc_read(int s, void *mem, size_t len) { return ndk_socket_recv(s, mem, len); }
static inline int soc_write(int s, void *data, size_t size) { return ndk_socket_send(s, data, size); }
#endif

/* pipe functions implemented by ndk-socket */
int ndk_pipe2_create(int pipefd[2]);
void ndk_pipe2_close(int fd);

#ifdef __cplusplus
}
#endif

#endif

