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
#ifndef __NDK_COMMON_H__
#define __NDK_COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <sp5k_os_api.h>
#include <ndk_types.h>
#include <ndk/errno.h>
#include <ndk/misc.h>
#include <ndk/debug.h>
#include <ndk/mem.h>
#include <ndk/basefw_internal.h>

///////////////////////////////////////////////////////////////////////////////
// bit read/write/swap functions
typedef struct {
	UINT32 bit_buf;
	int    bit_left;
	UINT8 *buf, *buf_ptr, *buf_end;
	int    size_in_bits;
} BStream;

void bstream_init(BStream *s, UINT8 *buffer, int buffer_size);
void bstream_write(BStream *s, int n, UINT32 value);
void bstream_flush(BStream *s);

int ndk_adts_header_encode(UINT8 buf[7], int sample_rate, int channels, int data_len);
///////////////////////////////////////////////////////////////////////////////
#define strdup		ndk_strdup
#define strerror	ndk_strerror

#undef  errno
#define errno		ndk_errno

char *ndk_strncpy(char *dest, const char *src, unsigned int n);
char *ndk_strnchr(const char *s, size_t count, int c);

// UINT to double. Sometimes, 30/100 becomes 0.29999999. This is not my what I want.
const char *uintdiv2str(unsigned int numerator, unsigned int denominator);

long kmgstr_to_long(const char *s);

///////////////////////////////////////////////////////////////////////////////
#define gettimeofday(tv, tz)	ndk_gettimeofday(tv)

// Return num of encoded bytes. src_pp pointer to the remainder src buffer.
int    ndk_base64_encode(char *dest_p, unsigned dest_len, unsigned char **src_pp, unsigned src_len);
// Return -1 if failed.
// substitute_char > 0: if invalid char found in in_p, pretend it as error_char.
int    ndk_base64_decode(unsigned char *dest_p, char const *src_p, unsigned src_len, char substitute_char);

///////////////////////////////////////////////////////////////////////////////
void   ndk_cpuidle_begin(unsigned prio);
UINT32 ndk_cpuidle_get();
void   ndk_cpuidle_end();

///////////////////////////////////////////////////////////////////////////////
// time difference between server and client.
int    ndk_ntp_server_start(int dbglvl);
void   ndk_ntp_server_stop();

///////////////////////////////////////////////////////////////////////////////
// Deprecated
#define ndk_yield ndk_thread_yield

///////////////////////////////////////////////////////////////////////////////
#define NDK_CRITICAL_EXPR(expr) do { \
	unsigned int _cr_tmp0123; \
	ENTER_CRITICAL(_cr_tmp0123); \
	{ expr; } \
	EXIT_CRITICAL(_cr_tmp0123); \
} while(0)
///////////////////////////////////////////////////////////////////////////////
void* ndk_blkpool_create(const char* name, int blk_size, int blk_num);
void  ndk_blkpool_destroy(void *pool_id);
void* ndk_blkpool_alloc_block(void *pool_id);
void  ndk_blkpool_free_block(void *pool_id, void *block_ptr);
void  ndk_blkpool_info_get(void *pool_id, int *available, int *total);

///////////////////////////////////////////////////////////////////////////////
enum {
	NDK_TELNET_GETC        = 1 << 0,
	NDK_TELNET_PUTC        = 1 << 1,
	NDK_TELNET_UART0_WRITE = 1 << 2
};

int  ndk_telnetd_start(int port, UINT32 flags);
int  ndk_telnet_open(UINT32 flags, void (*on_data_ready)(void *user_data), void *user_data);
void ndk_telnet_close();
void ndk_telnet_get(UINT8 **buf, UINT32 *len);
void ndk_telnet_put(UINT8 *buf, UINT32 len);
BOOL ndk_telnet_is_bye();
///////////////////////////////////////////////////////////////////////////////
typedef void (* aes_callback) (char* result, int result_len);
///////////////////////////////////////////////////////////////////////////////
// alarm

///////////////////////////////////////////////////////////////////////////////
/* \brief Send NDK system event.
 */
void ndk_sysevt_send(UINT32 evt, unsigned long param);

///////////////////////////////////////////////////////////////////////////////
// Miscellaneous

/* Convert the command_string to argc, argv and call the main function.
 * Note, the first word in the command_string must be the main function's name.
 *       The maximum number of arguments including main function's name cannot
 *       be greater than 16
 * Returns the result from main_func or -1 if command_string is invalid.
 */
int ndk_exec_main(const char* command_string, int (*main_func)(int, char**));

/* Like ndk_exec_main. The last argument must be NULL.
 * Example: ndk_exec_main_v(ifconfig_main, "ifconfig", "wlan0", "up", NULL);
 */
int ndk_exec_main_v(int (*main_func)(int, char**), const char *arg0, ...);

const char *ndk_optstr_parse(char *left, size_t len, char **right, const char *optstr);
///////////////////////////////////////////////////////////////////////////////
// Pcap Functions
typedef struct {} *NDKPcapHandle;
typedef struct {
	const char *prefix;
	unsigned caplen;
	unsigned fileSizeMax;
	unsigned fileIdxMax;
} NDKPcapArg;

int  ndk_pcap_init(NDKPcapHandle *h, NDKPcapArg *arg);
void ndk_pcap_destroy(NDKPcapHandle *h);
int  ndk_pcap_flush(NDKPcapHandle *h);
int  ndk_pcap_write(NDKPcapHandle *h, void *data, unsigned len);

///////////////////////////////////////////////////////////////////////////////
#ifdef perror
# undef perror
#endif

#define perror(s)  ndk_warning(s)

#ifdef __cplusplus
}
#endif

#endif

