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
 * user application to convert between lwIP/NetX socket and TUTK socket.
 */

#ifndef __NDK_SOC_API_H__
#define __NDK_SOC_API_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Socket ID */
enum {
	NDK_SOC_ID_INVALID = 0,
	NDK_SOC_ID_DEFAULT,     /* lwIP or NetX*/
	NDK_SOC_ID_TUTK,

	NDK_SOC_ID_NUM
};

/**************************************************************************
 *                          M A C R O S
 **************************************************************************/
#define NDK_SOC_OPS_GET_ID(app_name)                (app_name##_soc_ops.soc_id)
#define NDK_SOC_OPS_GET_FUNC(app_name, func)        (app_name##_soc_ops.soc_##func)
#define NDK_SOC_OPS_IMPLEMENT(app_name)             struct NDKSocOps app_name##_soc_ops = { NDK_SOC_ID_INVALID }
#define NDK_SOC_OPS_INITIALIZE(app_name, id)        ndk_soc_init_ops(&(app_name##_soc_ops), id)

/* BSD socket compatible API functions
 * Note, you must define macro NDK_NO_TCPIP_API before include <ndk_global_api.h> or <ndk_tcpip.h>. This
 * let lwIP or netX don't define BSD socket API functions. Otherwise, a redefinition compiling error may occurs.
 * #define NDK_NO_TCPIP_API
 * #include <ndk_global_api.h>
 */
#define NDK_SOC_OPS_DECLARE(app_name) \
 extern struct NDKSocOps app_name##_soc_ops; \
 static __inline__ int socket(int domain, int soctype, int protocol) { if (app_name##_soc_ops.soc_id == NDK_SOC_ID_INVALID) return -1; return app_name##_soc_ops.soc_socket(domain, soctype, protocol); } \
 static __inline__ int bind(int fd, const struct sockaddr *name, socklen_t namelen) { return app_name##_soc_ops.soc_bind(fd, name, namelen); } \
 static __inline__ int listen(int fd, int backlog) { return app_name##_soc_ops.soc_listen(fd, backlog); } \
 static __inline__ int connect(int fd, const struct sockaddr *name, socklen_t namelen) { return app_name##_soc_ops.soc_connect(fd, name, namelen); } \
 static __inline__ int accept(int fd, struct sockaddr *addr, socklen_t *addrlen) { return app_name##_soc_ops.soc_accept(fd, addr, addrlen); } \
 static __inline__ int sendto(int fd, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen) { return app_name##_soc_ops.soc_sendto(fd, data, size, flags, to, tolen); } \
 static __inline__ int send(int fd, const void *data, size_t size, int flags) { return app_name##_soc_ops.soc_send(fd, data, size, flags); } \
 static __inline__ int recvfrom(int fd, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen) { return app_name##_soc_ops.soc_recvfrom(fd, mem, len, flags, from, fromlen); } \
 static __inline__ int recv(int fd, void *mem, size_t len, int flags) { return app_name##_soc_ops.soc_recv(fd, mem, len, flags); } \
 static __inline__ int shutdown(int fd, int how) { return app_name##_soc_ops.soc_shutdown(fd, how); } \
 static __inline__ int closesocket(int fd) { return app_name##_soc_ops.soc_close(fd); } \
 static __inline__ int ioctlsocket(int fd, long cmd, void *argp) { return app_name##_soc_ops.soc_ioctl(fd, cmd, argp); } \
 static __inline__ int getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen) { return app_name##_soc_ops.soc_getsockopt(fd, level, optname, optval, optlen); } \
 static __inline__ int setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen) { return app_name##_soc_ops.soc_setsockopt(fd, level, optname, optval, optlen); } \
 static __inline__ int getsockname(int fd, struct sockaddr *addr, socklen_t *addrlen) { return app_name##_soc_ops.soc_getsockname(fd, addr, addrlen); } \
 static __inline__ int getpeername(int fd, struct sockaddr *addr, socklen_t *addrlen) { return app_name##_soc_ops.soc_getpeername(fd, addr, addrlen); } \
 static __inline__ int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout) { return app_name##_soc_ops.soc_select(maxfdp1, readset, writeset, exceptset, timeout); } \
 static __inline__ int select_interruptible(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, void *user_event) { return app_name##_soc_ops.soc_select_interruptible(maxfdp1, readset, writeset, exceptset, timeout, user_event); } \
 static __inline__ void select_wakeup(void *user_event) { return app_name##_soc_ops.soc_select_wakeup(user_event); } \
 static __inline__ int soc_read(int fd, void *mem, size_t len) { return app_name##_soc_ops.soc_read(fd, mem, len); } \
 static __inline__ int soc_write(int fd, const void *data, size_t size) { return app_name##_soc_ops.soc_write(fd, data, size); } \
 static __inline__ int soc_fcntl(int fd, int cmd, int val) { return app_name##_soc_ops.soc_fcntl(fd, cmd, val); } \
 static __inline__ int soc_geterr(int fd) { return app_name##_soc_ops.soc_geterr(fd); } \
 static __inline__ int setsockopt_int(int fd, int level, int optname, int val) { return setsockopt(fd, level, optname, &val, sizeof(val)); }

/**************************************************************************
 *                          T Y P E S
 **************************************************************************/
struct NDKSocOps
{
	int soc_id;
	int (*soc_socket)(int domain, int soctype, int protocol);
	int (*soc_bind)(int fd, const struct sockaddr *name, socklen_t namelen);
	int (*soc_listen)(int fd, int backlog);
	int (*soc_connect)(int fd, const struct sockaddr *name, socklen_t namelen);
	int (*soc_accept)(int fd, struct sockaddr *addr, socklen_t *addrlen);
	int (*soc_sendto)(int fd, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
	int (*soc_send)(int fd, const void *data, size_t size, int flags);
	int (*soc_recvfrom)(int fd, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
	int (*soc_recv)(int fd, void *mem, size_t len, int flags);
	int (*soc_read)(int fd, void *mem, size_t len);
	int (*soc_write)(int fd, const void *data, size_t size);
	int (*soc_shutdown)(int fd, int how);
	int (*soc_close)(int fd);
	int (*soc_ioctl)(int fd, long cmd, void *argp);
	int (*soc_getsockopt)(int fd, int level, int optname, void *optval, socklen_t *optlen);
	int (*soc_setsockopt)(int fd, int level, int optname, const void *optval, socklen_t optlen);
	int (*soc_getsockname)(int fd, struct sockaddr *addr, socklen_t *addrlen);
	int (*soc_getpeername)(int fd, struct sockaddr *addr, socklen_t *addrlen);
	int (*soc_select)(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
	int (*soc_select_interruptible)(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, void *user_event);
	void(*soc_select_wakeup)(void *user_event);
	int (*soc_fcntl)(int fd, int cmd, int val);
	int (*soc_geterr)(int fd);
};

int  ndk_soc_init_ops(struct NDKSocOps *socops, int socid);
int  ndk_soc_get_tcp_mss(int soc_id);

#ifdef __cplusplus
}
#endif

#endif
