/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2013 by iCatch Technology Co., Ltd.             *
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
 *  Author: Andy.Li                                                       *
 *                                                                        *
 **************************************************************************/

#ifndef __NDK_MAIL_H__
#define __NDK_MAIL_H__

#ifdef __cplusplus
extern "C" {
#endif

/* SMTP Mail Argument ID */
enum
{
	NDK_MAIL_END = 0,       // End flag of mail arguments
	NDK_MAIL_FROM,          // "Bob Example" <bob@example.org>
	NDK_MAIL_TO,            // "Alice Example" <alice@example.com>. At least one TO must be provided.
	NDK_MAIL_CC,            // Optional, theboss@example.com
	NDK_MAIL_BCC,           // Optional, other_people@example.com
	NDK_MAIL_SUBJECT,       // Subject.

	// The following body arguments can only appear at most one time.
	NDK_MAIL_BODY,          // Optional, mail body string.
	NDK_MAIL_BODY_CHARSET,  // "us-ascii" (default), "big5", etc
	// "0" or not present: string (default).
	// "1": NDK_MAIL_BODY is the name of the file which contains the mail body.
	NDK_MAIL_BODY_FILE,

	// File name of attachment. Optional. Can appear multiple times.
	NDK_MAIL_ATT_TEXT,      // Content-Type: text/plain
	NDK_MAIL_ATT_JPEG,      // image/jpeg
	NDK_MAIL_ATT_MOV,       // video/quicktime
	NDK_MAIL_ATT_AVI,       // video/avi
	NDK_MAIL_ATT_FILE       // Any type of file. application/octct-stream
};

/*
 * CharSet
 */
#define NDK_MAIL_US_ASCII       ((const char*)0x04)
#define NDK_MAIL_BIG5           ((const char*)0x08)

typedef struct {
	int         id;
	const char* val;
} NDKMailHeader;

/* \brief Setup basic SMTP parameters. Must be called before sending mail.
 * Can be called for multiple times.
 */
int ndk_mail_setup(const char* mailhost
	, int port
	, const char* auth_user
	, const char* auth_passwd
	, const char* auth_method);

/*
 NDKMailHeader hdrs [] = {
	{NDK_MAIL_FROM, "\"Bob\"<bob@example.org>"},
	{NDK_MAIL_TO, "\"Alice\" <alice@example.com>"},
	{NDK_MAIL_TO, "\"Gray\" <gray@example.com>"},
	{NDK_MAIL_CC, "\"Blue\" <blue@example.com>"},
	{NDK_MAIL_CC, "\"Boss\" <boss@example.com>"},
	{NDK_MAIL_BCC, "\"Yellow\" <yellow@example.com>"},
	{NDK_MAIL_BODY, "Hello, this is a test mail.\r\n"},
	{NDK_MAIL_BODY_CHARSET, NDK_MAIL_US_ASCII},
	{NDK_MAIL_END, NULL}
};
	ndk_mail_send(hdrs);
 */
int ndk_mail_send(NDKMailHeader *hdrs);

#ifdef __cplusplus
}
#endif

#endif

