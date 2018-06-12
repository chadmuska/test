/**************************************************************************
 *
 *       Copyright (c) 2015 by iCatch Technology, Inc.
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

#ifndef __NDK_FTPD_H__
#define __NDK_FTPD_H__

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************************************/
/*  FTP Server                                                                     */
/***********************************************************************************/
enum {
	FTPD_START_SUCCESS			= 0,
	FTPD_NOT_CLOSED_ERROR		=(-1),
	FTPD_MALLOC_ERROR			=(-2),
	FTPD_INIT_ERROR				=(-3),
	FTPD_THREAD_CREATE_ERROR	=(-4)
};

int  ndk_ftpd_start(const char* rootdir, int port, const char *opts);
void ndk_ftpd_stop();
void ndk_ftpd_register_cmds();

#ifdef __cplusplus
}
#endif

#endif


