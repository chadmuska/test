/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2014 by iCatch Technology Co., Ltd.              *
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
/* \brief The symbols, macros and API functions listed in this file help the
 * user applications to switch between lwIP/NetX socket and TUTK socket.
 */

#ifndef __NDK_SOCOPS_API_H__
#define __NDK_SOCOPS_API_H__

#ifdef __cplusplus
extern "C" {
#endif

/*#define USE_HOST_SOCKET*/

/* Socket API ID */
#define NDK_SOCAPI_TCPIP	0  /* lwIP or NetX */
#define NDK_SOCAPI_TUTK		1
#define NDK_SOCAPI_HOST		2

/**************************************************************************
 *                          M A C R O S
 **************************************************************************/
#define NDK_SOCOPS_GET_ID(app_name)                (app_name##_soc_ops->soc_id)
#define NDK_SOCOPS_GET_FUNC(app_name, func)        (app_name##_soc_ops->soc_##func)

#define NDK_SOCOPS_IMPLEMENT(app_name)             struct NDKSocOps *app_name##_soc_ops = NULL;
#define NDK_SOCOPS_INITIALIZE(app_name, id)        app_name##_soc_ops = ndk_socops_get_ops(id)

/* BSD socket compatible API functions
 * Note, you must define macro NDK_NO_TCPIP_API before include <ndk_tcpip_api.h>. This
 * let lwIP or netX don't define BSD socket API functions. Otherwise, a redefinition compiling error may occurs.
 *
 * #define NDK_NO_TCPIP_API
 * #include <ndk_tcpip_api.h>
 */
#define NDK_SOCOPS_DECLARE(app_name) \
extern struct NDKSocOps *app_name##_soc_ops; \
static inline int socket(int domain, int soctype, int protocol) { return ((int (*)(int, int, int))app_name##_soc_ops->soc_socket)(domain, soctype, protocol); } \
static inline int bind(int fd, const struct sockaddr *name, socklen_t namelen) { return ((int (*)(int, const struct sockaddr *, socklen_t))app_name##_soc_ops->soc_bind)(fd, name, namelen); } \
static inline int listen(int fd, int backlog) { return ((int (*)(int, int))app_name##_soc_ops->soc_listen)(fd, backlog); } \
static inline int connect(int fd, const struct sockaddr *name, socklen_t namelen) { return ((int (*)(int, const struct sockaddr *, socklen_t))app_name##_soc_ops->soc_connect)(fd, name, namelen); } \
static inline int accept(int fd, struct sockaddr *addr, socklen_t *addrlen) { return ((int (*)(int, struct sockaddr *, socklen_t *))app_name##_soc_ops->soc_accept)(fd, addr, addrlen); } \
static inline int sendto(int fd, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen) { return ((int (*)(int, const void *, size_t, int, const struct sockaddr *, socklen_t))app_name##_soc_ops->soc_sendto)(fd, data, size, flags, to, tolen); } \
static inline int send(int fd, const void *data, size_t size, int flags) { return ((int (*)(int, const void *, size_t, int))app_name##_soc_ops->soc_send)(fd, data, size, flags); } \
static inline int recvfrom(int fd, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen) { return ((int (*)(int, void *, size_t, int, struct sockaddr *, socklen_t *))app_name##_soc_ops->soc_recvfrom)(fd, mem, len, flags, from, fromlen); } \
static inline int recv(int fd, void *mem, size_t len, int flags) { return ((int (*)(int, void *, size_t, int))app_name##_soc_ops->soc_recv)(fd, mem, len, flags); } \
static inline int shutdown(int fd, int how) { return ((int (*)(int, int))app_name##_soc_ops->soc_shutdown)(fd, how); } \
static inline int closesocket(int fd) { return ((int (*)(int))app_name##_soc_ops->soc_close)(fd); } \
static inline int ioctlsocket(int fd, long cmd, void *argp) { return ((int (*)(int, long, void *))app_name##_soc_ops->soc_ioctl)(fd, cmd, argp); } \
static inline int getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen) { return ((int (*)(int, int, int, void *, socklen_t *))app_name##_soc_ops->soc_getsockopt)(fd, level, optname, optval, optlen); } \
static inline int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen) { return ((int (*)(int, int, int, const void *, socklen_t))app_name##_soc_ops->soc_setsockopt)(fd, level, optname, optval, optlen); } \
static inline int getsockname(int fd, struct sockaddr *addr, socklen_t *addrlen) { return ((int (*)(int, struct sockaddr *, socklen_t *))app_name##_soc_ops->soc_getsockname)(fd, addr, addrlen); } \
static inline int getpeername(int fd, struct sockaddr *addr, socklen_t *addrlen) { return ((int (*)(int, struct sockaddr *, socklen_t *))app_name##_soc_ops->soc_getpeername)(fd, addr, addrlen); } \
static inline int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout) { return ((int (*)(int, fd_set *, fd_set *, fd_set *, struct timeval *))app_name##_soc_ops->soc_select)(maxfdp1, readset, writeset, exceptset, timeout); } \
static inline int select_interruptible(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, void *user_event) { return ((int (*)(int, fd_set *, fd_set *, fd_set *, struct timeval *, void *))app_name##_soc_ops->soc_select_interruptible)(maxfdp1, readset, writeset, exceptset, timeout, user_event); } \
static inline void select_wakeup(void *user_event) { return ((void (*)(void *))app_name##_soc_ops->soc_select_wakeup)(user_event); } \
static inline int soc_read(int fd, void *mem, size_t len) { return ((int (*)(int, void *, size_t))app_name##_soc_ops->soc_read)(fd, mem, len); } \
static inline int soc_write(int fd, const void *data, size_t size) { return ((int (*)(int, const void *, size_t))app_name##_soc_ops->soc_write)(fd, data, size); } \
static inline int soc_fcntl(int fd, int cmd, int val) { return ((int (*)(int, int, int) )app_name##_soc_ops->soc_fcntl)(fd, cmd, val); } \
static inline int soc_geterr(int fd) { return ((int (*)(int))app_name##_soc_ops->soc_geterr)(fd); } \
static inline int setsockopt_int(int fd, int level, int optname, int val) { return setsockopt(fd, level, optname, &val, sizeof(val)); }

/**************************************************************************
 *                          T Y P E S
 **************************************************************************/
struct NDKSocOps
{
	int soc_id;
	unsigned tcp_mss;
	/* Socket API Function Pointers. To eliminate the dependency on the TCPIP headers which are defined
	  in the lwIP or NetX module, the types of function pointer are defined to void*. */
	void *soc_socket;
	void *soc_bind;
	void *soc_listen;
	void *soc_connect;
	void *soc_accept;
	void *soc_sendto;
	void *soc_send;
	void *soc_recvfrom;
	void *soc_recv;
	void *soc_read;
	void *soc_write;
	void *soc_shutdown;
	void *soc_close;
	void *soc_ioctl;
	void *soc_getsockopt;
	void *soc_setsockopt;
	void *soc_getsockname;
	void *soc_getpeername;
	void *soc_select;
	void *soc_select_interruptible;
	void *soc_select_wakeup;
	void *soc_fcntl;
	void *soc_geterr;
};

void ndk_socops_register(struct NDKSocOps *socops);
struct NDKSocOps *ndk_socops_get_ops(int socid);
static inline int ndk_socops_get_tcpmss(int socid) { struct NDKSocOps *socops = ndk_socops_get_ops(socid); return socops ? socops->tcp_mss : 0; }

#ifdef __cplusplus
}
#endif

#endif

