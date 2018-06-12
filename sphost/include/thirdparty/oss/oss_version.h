/**************************************************************************
 *
 *       Copyright (c) 2014 by iCatch Technology, Inc.
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
 *  Author: Andy.Li
 *
 **************************************************************************/
#ifndef __OSS_VER_H__
#define __OSS_VER_H__

#define OSS_RELEASE_VERSION "00.01.03"
#define OSS_SVN_NUMBER "19549"

/* Inner used only */
#define OSS_SVN_REVISION	"$Rev: 19549 $"
#define OSS_SVN_DATE		"$Date: 2014-09-05 17:29:34 +0800 (周五, 05 九月 2014) $"

#include <../inc/version.h>

#ifndef SPCA6350
#error SPCA6350 not defined
#endif

#if !defined(BASEFW_REVISION_CODE) || BASEFW_REVISION_CODE < 26836
#error Need basefw revision 26836 or higher.
#endif

#endif
