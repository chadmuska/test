/**************************************************************************
 *
 *       Copyright (c) 2012 by iCatch Technology, Inc.
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
#ifndef __NET_VER_H__
#define __NET_VER_H__

#define NET_RELEASE_VERSION "00.15.80"
#define NET_SVN_NUMBER "26607"

/* Inner used only */
#define NET_SVN_REVISION	"$Rev: 19183 $"
#define NET_SVN_DATE		"$Date: 2015-03-05 05:51:52 +0800 (週四, 05 三月 2015) $"

#include <../inc/version.h>

#ifndef SPCA6350
#error SPCA6350 not defined
#endif

#if !defined(BASEFW_REVISION_CODE) || BASEFW_REVISION_CODE < 26675
#error Need basefw revision 26675 or higher.
#endif

#endif
