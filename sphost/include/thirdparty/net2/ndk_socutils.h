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

#ifndef __NDK_SOC_UTILS_H__
#define __NDK_SOC_UTILS_H__

#ifdef __cplusplus
extern "C" {
#endif

int ndk_utils_global_init(const char *options);

/***********************************************************************************/
/*  PING                                                                           */
/***********************************************************************************/
/* \brief Do ping test through ICMP echo message
 * Arguments:
 *   host: Host to ping.
 *   timeout: Timeout in milli-second.
 *   triptime - Trip-time in 1/10000 second.
 * \return 0: succeed; < 0: failed
 */
int ndk_ping(struct in_addr *host, long timeout, long *triptime);
int ndk_ping_s(const char *host, long timeout, long *triptime);

/***********************************************************************************/
/*  Misc.                                                                          */
/***********************************************************************************/
const char* eth_addr_ntoa(unsigned char *hwaddr);
int eth_addr_aton(unsigned char *d_hwaddr, const char *s_hwaddr);

#ifdef __cplusplus
}
#endif

#endif

