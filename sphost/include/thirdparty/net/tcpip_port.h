/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2012 by iCatch Technology Co., Ltd.             *
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
#ifndef __TCPIP_PORT_H__
#define __TCPIP_PORT_H__

#ifndef TCPIP_LWIP
# define TCPIP_LWIP
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <lwip/sockets.h>
#include <lwip/netdb.h>

#ifndef NDK_NO_TCPIP_API
static __inline__ int accept(int s, struct sockaddr *addr, socklen_t *addrlen) { return lwip_accept(s, addr, addrlen); }
static __inline__ int bind(int s, const struct sockaddr *name, socklen_t namelen) { return lwip_bind(s, name, namelen); }
static __inline__ int shutdown(int s, int how) { return lwip_shutdown(s, how); }

static __inline__ int closesocket(int s) { if (s >= 0) return lwip_close(s); else return -1; }
static __inline__ int connect(int s, const struct sockaddr *name, socklen_t namelen) { return lwip_connect(s, name, namelen); }
static __inline__ int getsockname(int s, struct sockaddr *name, socklen_t *namelen) { return lwip_getsockname(s, name, namelen); }
static __inline__ int getpeername(int s, struct sockaddr *name, socklen_t *namelen) { return lwip_getpeername(s, name, namelen); }

static __inline__ int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen) { return lwip_setsockopt(s, level, optname, optval, optlen); }
static __inline__ int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen) { return lwip_getsockopt(s, level, optname, optval, optlen); }

static __inline__ int listen(int s, int backlog) { return lwip_listen(s, backlog); }
static __inline__ int recv(int s, void *mem, size_t len, int flags) { return lwip_recv(s, mem, len, flags); }
static __inline__ int recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen) { return lwip_recvfrom(s, mem, len, flags, from, fromlen); }
static __inline__ int send(int s, const void *data, size_t size, int flags) { return lwip_send(s, data, size, flags); }
static __inline__ int sendto(int s, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen) { return lwip_sendto(s, data, size, flags, to, tolen); }
static __inline__ int socket(int domain, int type, int protocol) { return lwip_socket(domain, type, protocol); }
static __inline__ int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout) { return lwip_select(maxfdp1, readset, writeset, exceptset, timeout); }
static __inline__ int select_interruptible(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, void *user_event) { return lwip_select_interruptible(maxfdp1, readset, writeset, exceptset, timeout, (UINT32*)user_event); }
static __inline__ void select_wakeup(void *user_event) { lwip_select_wakeup(user_event); }
static __inline__ int ioctlsocket(int s, long cmd, void *argp) { return lwip_ioctl(s, cmd, argp); }

#if LWIP_DNS
static __inline__ struct hostent* gethostbyname(const char *name) { return lwip_gethostbyname(name); }
static __inline__ int gethostbyname_r(const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop) { return lwip_gethostbyname_r(name, ret, buf, buflen, result, h_errnop); }
static __inline__ void freeaddrinfo(struct addrinfo *ai) { lwip_freeaddrinfo(ai); }
static __inline__ int getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res) { return lwip_getaddrinfo(nodename, servname, hints, res); }
#endif

static __inline__ int setsockopt_int(int s, int level, int optname, int val) { return lwip_setsockopt(s, level, optname, &val, sizeof(val)); }

static __inline__ int soc_read(int s, void *mem, size_t len) { return lwip_read(s, mem, len); }
static __inline__ int soc_write(int s, const void *data, size_t size) { return lwip_write(s, data, size); }
static __inline__ int soc_fcntl(int s, int cmd, int val) { return lwip_fcntl(s, cmd, val); }
static __inline__ int soc_geterr(int s) { return lwip_geterr(s); }

#endif /* NDK_NO_TCPIP_API */

#ifdef __cplusplus
}
#endif

#endif
