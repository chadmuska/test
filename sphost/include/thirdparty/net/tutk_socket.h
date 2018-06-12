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

#ifndef __TUTK_SOCKET_H__
#define __TUTK_SOCKET_H__

#define TUTK_SOCKET_SVN_REVISION       "$Rev: 19341 $"
#define TUTK_SOCKET_SVN_DATE           "$Date: 2015-04-29 13:24:55 +0800 $"

#ifdef __cplusplus
extern "C" {
#endif

#define TUTK_SOCKET_VERSION            0x0100

/**************************************************************************
 *  CONSTANTS & TYPES
 **************************************************************************/
enum
{
	TUTK_SOCKET_TCP_MSS            = 1280 , /* 1384 */
};

/* TUTK Socket Message */
enum
{
	TUTK_SOCKET_MSG_STATE_CHANGED  = 0,      /* Argument: TUTK_SOCKET_STATE_XXX. */
	TUTK_SOCKET_MSG_SERVICE_INITIALIZED = 1, /* Argument: None. */
};

/* Argument for TUTK_SOCKET_MSG_STATE_CHANGED */
enum
{
	TUTK_SOCKET_STATE_DISCONNECTED = 0, /* Connection is broken. No Arg.*/
	TUTK_SOCKET_STATE_CONNECTED,        /* Connection is created. No Arg.*/
	TUTK_SOCKET_STATE_CLOSED            /* TUTK service is closed. User can deinitialize their applications now. No Arg.*/
};

/* Extra socket initialize options. For backward compatibility. */
enum {
	TUTK_SOCKET_INIT_ENCRYPTION    = 0, /* 1: enable TUTK encryption. 0: (default) disable. */
	TUTK_SOCKET_INIT_CREATE_RDT_TIMEOUT, /* Timeout to create RDT channel. */

	TUTK_SOCKET_INIT_OPT_NUM
};

typedef void (*TUTKSocketMsgHandler)(int msgId, long arg, void *user_data);

/**************************************************************************
 *  TUTK Socket System Functions
 **************************************************************************/

/* \brief Initialize TUTK socket system.
 */
int tutk_socket_sys_init(
	TUTKSocketMsgHandler handler, /* Socket message handler. */
	void                 *udata   /* User data passed to the socket message handler. */
);

/* \brief DeInitializeTUTK socket system.
 */
int tutk_socket_sys_deinit();

/* \brief Get current tutk socket system state.
 */
int tutk_socket_get_state();

/* \brief For SOCK_STREAM, it is a listening port, for SOCK_DGRAM, it is a UDP port.
 * \return Boolean 1 or 0.
 */
unsigned char tutk_socket_check_remote_port(unsigned short port, int soctype);

/* \brief Return current socket number. */
int  tutk_socket_get_info(int *socket_count, int *socket_available);

void tutk_socket_print_info(const char *opts);

/* *** TUTK Service API *** */
int  tutk_service_start(
	const char           *uid,         /* TUTK UID. Cannot be NULL. */
	unsigned char        bServer,      /* 1: Act as TUTK server; 0: TUTK client. */
	unsigned int         rdtTxBufSize, /* 0: Use default value(512*1024). >0: tx buffer size.*/
	unsigned int         opt_num,      /* Option number. */
	unsigned long        *opts);       /* Option value array. */
int  tutk_service_stop(unsigned char bWait);
const char *tutk_service_get_uid();

/**************************************************************************
 *  TUTK BSD SOCKET & EXTEND API FUNCTIONS
 **************************************************************************/
int  tutk_socket(int domain, int soctype, int protocol);
int  tutk_bind(int fd, const struct sockaddr *name, socklen_t namelen);
int  tutk_listen(int fd, int backlog);
int  tutk_connect(int fd, const struct sockaddr *name, socklen_t namelen);
int  tutk_accept(int fd, struct sockaddr *addr, socklen_t *addrlen);
int  tutk_sendto(int fd, const void *data, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
int  tutk_send(int fd, const void *data, size_t size, int flags);
int  tutk_write(int fd, const void *data, size_t size);
int  tutk_recvfrom(int fd, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
int  tutk_recv(int fd, void *mem, size_t len, int flags);
int  tutk_read(int fd, void *mem, size_t len);
int  tutk_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout);
int  tutk_select_interruptible(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset, struct timeval *timeout, void *user_event);
void tutk_select_wakeup(void *user_event);
int  tutk_shutdown(int fd, int how);
int  tutk_close(int fd);
int  tutk_ioctl(int fd, long cmd, void *argp);
int  tutk_fcntl(int fd, int cmd, int val);
int  tutk_getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen);
int  tutk_setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen);
int  tutk_getsockname(int fd, struct sockaddr *addr, socklen_t *addrlen);
int  tutk_getpeername(int fd, struct sockaddr *addr, socklen_t *addrlen);
int  tutk_geterr();

#ifdef __cplusplus
}
#endif

#endif

