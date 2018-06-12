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
#ifndef __NDK_TCPIP_API_H__
#define __NDK_TCPIP_API_H__

#ifndef TCPIP_LWIP
#define TCPIP_LWIP
#endif

#ifdef TCPIP_INET_SOCKTYPES
#error "Must include 'ndk_tcpip_api.h' before 'linux/socktypes.h'"
#endif

/* Don't include the types defined in the linux/socktypes.h */
#define TCPIP_INET_SOCKTYPES

#ifdef __cplusplus
extern "C" {
#endif

#include <lwip/sockets.h>
#include <lwip/netdb.h>

#ifndef NDK_NO_TCPIP_API
static inline int accept(int s, struct sockaddr *addr, socklen_t *addrlen) { return lwip_accept(s, addr, addrlen); }
static inline int bind(int s, const struct sockaddr *name, socklen_t namelen) { return lwip_bind(s, name, namelen); }
static inline int shutdown(int s, int how) { return lwip_shutdown(s, how); }

static inline int closesocket(int s) { if (s >= 0) return lwip_close(s); else return -1; }
static inline int connect(int s, const struct sockaddr *name, socklen_t namelen) { return lwip_connect(s, name, namelen); }
static inline int getsockname(int s, struct sockaddr *name, socklen_t *namelen) { return lwip_getsockname(s, name, namelen); }
static inline int getpeername(int s, struct sockaddr *name, socklen_t *namelen) { return lwip_getpeername(s, name, namelen); }

static inline int setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen) { return lwip_setsockopt(s, level, optname, optval, optlen); }
static inline int getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen) { return lwip_getsockopt(s, level, optname, optval, optlen); }

static inline int listen(int s, int backlog) { return lwip_listen(s, backlog); }
static inline int recv(int s, void *mem, size_t len, int flags) { return lwip_recv(s, mem, len, flags); }
static inline int recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen) { return lwip_recvfrom(s, mem, len, flags, from, fromlen); }
static inline int send(int s, const void *data, size_t size, int flags) { return lwip_send(s, data, size, flags); }
static inline int sendto(int s, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen) { return lwip_sendto(s, data, size, flags, to, tolen); }
static inline int socket(int domain, int type, int protocol) { return lwip_socket(domain, type, protocol); }
static inline int select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout) { return lwip_select(maxfdp1, readset, writeset, exceptset, timeout); }
static inline int select_interruptible(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, void *user_event) { return lwip_select_interruptible(maxfdp1, readset, writeset, exceptset, timeout, (UINT32*)user_event); }
static inline void select_wakeup(void *user_event) { lwip_select_wakeup(user_event); }
static inline int ioctlsocket(int s, long cmd, void *argp) { return lwip_ioctl(s, cmd, argp); }

#if LWIP_DNS
static inline struct hostent* gethostbyname(const char *name) { return lwip_gethostbyname(name); }
static inline int gethostbyname_r(const char *name, struct hostent *ret, char *buf, size_t buflen, struct hostent **result, int *h_errnop) { return lwip_gethostbyname_r(name, ret, buf, buflen, result, h_errnop); }
static inline void freeaddrinfo(struct addrinfo *ai) { lwip_freeaddrinfo(ai); }
static inline int getaddrinfo(const char *nodename, const char *servname, const struct addrinfo *hints, struct addrinfo **res) { return lwip_getaddrinfo(nodename, servname, hints, res); }
#endif

static inline int setsockopt_int(int s, int level, int optname, int val) { return lwip_setsockopt(s, level, optname, &val, sizeof(val)); }

static inline int soc_read(int s, void *mem, size_t len) { return lwip_read(s, mem, len); }
static inline int soc_write(int s, const void *data, size_t size) { return lwip_write(s, data, size); }
static inline int soc_fcntl(int s, int cmd, int val) { return lwip_fcntl(s, cmd, val); }
static inline int soc_geterr(int s) { return lwip_geterr(s); }

#endif /* NDK_NO_TCPIP_API */

#define CLOSESOCKET(s) ({ int _tmp_r = 0; \
	if (s >= 0) { shutdown(s, SHUT_RDWR); _tmp_r = closesocket(s); s = -1; }  \
	_tmp_r; \
})

/***********************************************************************************/
/*  DHCP Client & Server                                                           */
/***********************************************************************************/
/* \brief Startup the DHCP client service
 * \param ifname: interface name, such as: wlan0
 * \return 0: succeed; < 0: failed
 * Notes:
 *   Before call this function, you should call ndk_netif_set_address function
 *   to set the interface's ip-address to zero.
 */
int ndk_dhcp_start(const char *ifname);

/* \brief Stop the DHCP client service
 * \param ifname: interface name, such as: wlan0
 */
void ndk_dhcp_stop(const char *ifname);

/***********************************************************************************/
/*  Initialize TCP/IP protocol stack                                               */
/***********************************************************************************/
int ndk_tcpip_global_init(const char *options);

#ifdef __cplusplus
}
#endif

#endif
