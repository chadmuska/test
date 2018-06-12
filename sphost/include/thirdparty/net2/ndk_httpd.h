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
#ifndef __NDK_HTTPD_H__
#define __NDK_HTTPD_H__

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************
 *   CONSTANTS
 ***********************************************************************************/
/* HTTPd options */
enum {
	NDK_HTTPD_OPT_ROOT = 0, /* root directory. default D: */
	NDK_HTTPD_OPT_PORT,     /* listening port. default 80. */

	NDK_HTTPD_OPT_NUM       /* Number of options. */
};

/* CGI Environment Variables
 * e.g. URI in the HTTP request is: http://192.168.1.1/do/cgi-name/path/to/file?a=b&c=d
 */
typedef enum {
	NDK_CGI_ENV_GATEWAY_INTERFACE,  /* CGI/1.1 */
	NDK_CGI_ENV_SERVER_PROTOCOL,    /* "HTTP/1.1" */
	NDK_CGI_ENV_SERVER_PORT,	/* integer, 80 */
	NDK_CGI_ENV_SERVER_NAME,
	NDK_CGI_ENV_SERVER_ROOT,        /* D: */
	NDK_CGI_ENV_DOCUMENT_ROOT,      /* D: */
	NDK_CGI_ENV_REQUEST_METHOD,     /* GET or POST */
	NDK_CGI_ENV_LOCAL_ADDR,
	NDK_CGI_ENV_REMOTE_ADDR,
	NDK_CGI_ENV_REMOTE_PORT,	/*long*/

	NDK_CGI_ENV_REQUEST_URI,        /* /path/to/file */
	NDK_CGI_ENV_CONTENT_TYPE,
	NDK_CGI_ENV_CONTENT_LENGTH,	/*unsigned long*/
	NDK_CGI_ENV_QUERY_STRING,       /* a=b&c=d */
	NDK_CGI_ENV_REMOTE_USER,
	NDK_CGI_ENV_AUTH_TYPE,
	NDK_CGI_ENV_FULL_PATH,          /* D:/path/to/file */

	NDK_CGI_ENV_VARS
} NDKCgiEnvId;

/* CGI Private Flags */
enum {
	NDK_CGI_PRIV_FLG_FIRST    = 1 << 0, /* This CGI instance is running for the first time. Initialization is needed. */
	NDK_CGI_PRIV_FLG_CONN_ERR = 1 << 1, /* The HTTP socket connection is closed by the client. The CGI
	                                       instance will be terminated. Resources must be released in
	                                       this callback. */
	NDK_CGI_PRIV_FLG_ABORT    = 1 << 2, /* The CGI is aborted by the user. */
};

enum {
	NDK_CGI_CTRL_ENABLE = 0,
	NDK_CGI_CTRL_DISABLE,
	NDK_CGI_CTRL_GET_INST_NUM,
	NDK_CGI_CTRL_ABORT_ALL,
};

/***********************************************************************************
 *   TYPES
 ***********************************************************************************/
typedef struct {
	int id;
	const char *value;
} NDKHttpdOpt;

/* CGI Instance Handler */
typedef struct NDKCgiInstStru {} *NDKCgiInstance;

/* \brief CGI callback function
 * \param udata: user data
 * \param flags: NDK_CGI_PRIV_FLG_OPEN, NDK_CGI_PRIV_FLG_CONNECTION_ERROR
 * \param priv: User's private data buffer. The buffer size is determinated by the argument
 * priv_size of the API function ndk_cgi_register_fast. NULL if priv_size is 0.
 */
typedef void (*NDKCgiCallback)(NDKCgiInstance inst, void *udata, UINT flags, void *priv);

/***********************************************************************************
 *   API Functions
 ***********************************************************************************/
void ndk_httpd_global_init();

/* \brief Start HTTP server.
 * \param num number of options.
 * \param opts Option array.
 * \return 0: succeed. < 0: error code.
 */
int   ndk_httpd_start(int num, NDKHttpdOpt *opts);
void  ndk_httpd_stop();

/* \brief Register a fast CGI.
 * \param name: CGI name. Max name size is 31.
 * \param priv_size: Size of the user's privae CGI data structure.
 * \param cbfunc: CGI callback function.
 * \param udata: User data. Pass to user through the NDKCgiCallback's argument udata.
 * \return 0: succeed; < 0: error number.
 */
int  ndk_cgi_register_fast(const char *name, UINT priv_size, NDKCgiCallback cbfunc, void *udata);

int  ndk_cgi_control(UINT code, void *arg);

/* \brief Unregister CGI
 * \param name: name of the CGI. Can be NULL.
 * \param id: CGI id. If name is NULL, id will be used to find the CGI.
 * \return None.
 */
void ndk_cgi_unregister(const char *name);
void ndk_cgi_dump();

/* CGI stream IO */
/* The write function will send data to client immediately. So ndk_cgi_flush is not needed.
 */
int  ndk_cgi_write(NDKCgiInstance inst, const void *buf, UINT len);

/* The following functions will write the data in a local buffer temporarily.
   This can reduce the calling number to socket functions.
 */
/* The length of formated string must be less than or equal to  512. */
int  ndk_cgi_printf(NDKCgiInstance inst, const char *fmt, ...) __attribute__((format(printf,2,3)));
int  ndk_cgi_puts (NDKCgiInstance inst, const void *s); /* equas to ndk_httpd_cgi_putsn(inst, s, 0) */
int  ndk_cgi_putsn(NDKCgiInstance inst, const void *s, UINT n /* 0 => strlen(s). */);

/* Sent the data in the local buffer to client immediately. */
void ndk_cgi_flush(NDKCgiInstance inst);

/* Print Status-Line and some common headers: Connection, Content-Type and Content-Length. e.g.:
HTTP/1.1 200 OK\r\n
Connection: close\r\n
Content-Type: text/plain\r\n
Content-Length: nnn\r\n      -- optional
\r\n      -- optional
  Call ndk_cgi_put_printf/puts/putsn to put other headers.
 * \param close Close connection or not. TRUE: close; FALSE: keep-alive.
 * \param ct Content-Type.
 * \param cl Content-Length. If cl==0, will not print the content-length header. If you use
 *   chunked transfering, always set cl to 0.
 * \param finished TRUE: will print an additional '\r\n' to finish the response header; FALSE: not.
 */
int  ndk_cgi_put_status(NDKCgiInstance inst, UINT status, BOOL close, const char *ct, UINT cl, BOOL finished);

void ndk_cgi_wakeup(NDKCgiInstance inst);
void ndk_cgi_schedule(NDKCgiInstance inst, UINT timeout);

/* Stop the CGI program. Before exit, you should have called the write/printf/puts/putsn or
  other functions to send HTTP response to the client. */
void ndk_cgi_set_finished(NDKCgiInstance inst);

/* Terminate the CGI program and send a simple text/plain message to the client. */
void ndk_cgi_terminate(NDKCgiInstance inst, UINT status, const char *content);

/* \return a mutable string. */
char *ndk_cgi_env_get(NDKCgiInstance inst, NDKCgiEnvId envid);

/* \brief Get argument by index.
 * \param idx: index, start from 0.
 * \param value: A pointer to store the value string. Can be NULL. For no value name-value pair, return NULL value.
 * \return Argument name. NULL: failed.
 */
BOOL ndk_cgi_arg_get(NDKCgiInstance inst, UINT idx, const char **name, char **value);

/* \brief Find if argument 'name' exist. If exist, return it's value.
 * \param value: A pointer to store the value string. Can be NULL.
 * \return TRUE: the argument 'name' is found.
 * \note For no value name-value pair, if the argument 'name' is found, this function will return TRUE
 *   and set the value pointer to NULL.
 */
BOOL ndk_cgi_arg_find(NDKCgiInstance inst, const char *name, char **value);

/* \brief Get value string.
 * \param name: Argument name. Case-insensitive.
 * \return value string. Return NULL if argument name not found or the value is empty string.
 */
char *ndk_cgi_arg_get_string(NDKCgiInstance inst, const char *name);

/* \return number of arguments. */
UINT ndk_cgi_arg_get_number(NDKCgiInstance inst);

/* \brief Make URI root and return the address next to the uri-root's last char.
 * You can call sprintf to append extra path or query string to the return pointer. e.g.
 * The URI root does not include the "http" prefix, ip address and the port number;
 *   p = ndk_cgi_make_uri_root(buf, end, "SOME-METHOD");
 *   if (!p) return error;
 *   // URI root is: DO/SOME-METHOD
 *   sprintf(p, "?%s=%s", name, value);
 */
char *ndk_cgi_make_uri_root(char *buf, const char *end, const char *cgi_name);

/***********************************************************************************
 *   Predefined CGI Functions
 ***********************************************************************************/
/* \brief Predefined CGI ID
 * 0: invalid id.
 */
#define NDK_CGI_NAME_FOLDER             "folder"
#define NDK_CGI_NAME_JPGTHUMB           "jpgthumb"
#define NDK_CGI_NAME_VIDTHUMB           "vidthumb"
#define NDK_CGI_NAME_M3U8               "m3u8"
#define NDK_CGI_NAME_HLS                "hls"
#define NDK_CGI_NAME_CMDLINE            "cmdline"
#define NDK_CGI_NAME_TEST               "test"

int  ndk_cgi_register_all();
void ndk_cgi_unregister_all();

/* Folder Preview */
/* URI: /do/folder/path/to/folder[?other_params] */
int  ndk_cgi_folder_make_url(char *uribuf, UINT buflen, const char *path);

/* JPEG thumbnail */
/* URI: /do/jpgthumb/path/to/jpeg/file[?other_params] */
int  ndk_cgi_jpgthumb_make_url(char *uribuf, UINT buflen, const char *path);

/* Video Thumbnail */
/* URI: /do/vidthumb/path/to/video/file[?other_params] */
int  ndk_cgi_vidthumb_make_url(char *uribuf, UINT buflen, const char *path);

/***********************************************************************************
 *   HTTP Live Streaming
 ***********************************************************************************/
/* Attributes */
enum {
	/* For Live HLS */
	NDK_HLS_ATTR_AUDIO_ON    = 0,
	NDK_HLS_ATTR_VIDEO_CODEC,
	NDK_HLS_ATTR_STREAM_ID,
};

typedef struct NDKHlsInstStru {} *NDKHlsInstance;

/*
 * \param bLive: TRUE is live streaming and FALSE is file streaming.
 * \param path: For example:
 *    live mode:  /live#;
 *    file mode:  D:/path/to/video/file.mov
 * \param stream_id: meaningless if bLive is FALSE.
 *    V33: 0 is normal record; 1 is dualstream_jpeg.
 *    V35: stream id.
 * \return TRUE: continue; FALSE: abort.
 */
typedef BOOL (*NDKHlsOnStart)(NDKHlsInstance h, BOOL bLive, const char *path, UINT32 cookie);
typedef void (*NDKHlsOnStop) (NDKHlsInstance h, BOOL bLive, const char *path, UINT32 cookie);

int  ndk_hls_init(
	NDKHlsOnStart pfOnStart,
	NDKHlsOnStop  pfOnStop,
	UINT          fileSegTime,
	UINT          liveSegTime,
	const char *  extra_opts);

void ndk_hls_deinit();

int  ndk_hls_set_attribute(NDKHlsInstance h, int attrId, unsigned long value);
void ndk_hls_monitor();
void ndk_hls_dump();

#ifdef __cplusplus
}
#endif

#endif

