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

#ifndef __RTK_NETDEV_SUPPORT_H__
#define __RTK_NETDEV_SUPPORT_H__

#ifdef __cplusplus
extern "C" {
#endif

enum {
	/* Derived from NDK. */
	RTK_IOC_FIRST = NDK_IOC_MAX,
	RTK_IOCS_WIFI_SUSPEND,          /* arg: netif name; value: not used. */
	RTK_IOCS_WIFI_RESUME,           /* arg: netif name; value: not used. */
	RTK_IOCS_WIFI_COUNTRY_US,       /* arg: netif name; value: not used. Channel 1 - 11 */
	RTK_IOCS_WIFI_COUNTRY_EU,       /* arg: netif name; value: not used. Channel 1 - 13 */
	RTK_IOCS_WIFI_COUNTRY_JP,       /* arg: netif name; value: not used. Channel 1 - 14 */
	RTK_IOCS_WIFI_CCK_PWR,          /* arg: netif name; value: long*, cck power (rtk: 0 ~ 3F) */
	RTK_IOCS_WIFI_STAINFO_SETMAC,   /* arg: netif name value: mac address */
	RTK_IOCS_ETHER_SETMAC = RTK_IOCS_WIFI_STAINFO_SETMAC,
	RTK_IOCS_WIFI_TXFIFO_CONFIG,    /* arg: netif name value: '[delay|retry|wait] value' */
	                                /* delay - ms:  sleep n ms after xmit success */
	                                /* retry - count , wait - ms:  if retry over limit then sleep wait for n ms */
	RTK_IOCS_WIFI_QUEUE_CLR,
	RTK_IOCS_WIFI_EFUSE_READ,       /* arg: netif name; value: address pointer that store efuse context */
	RTK_IOCS_WIFI_EFUSE_WRITE,      /* arg: netif name; value: address pointer that update context into efuse */

	RTK_IOCS_MAC_CONSTRAIN,
	RTK_IOCS_DBG_LEVEL,             /* arg: netif name; value: debug message level 0:off 1:always 2:emerg 3:alert 4:crit
	                                   5:err, 6:warnng, 7:notice, 8:info, 9:dump, 10:debug */

	/* Get */
	/* \brief Get network interface loading. 100 is overload. 0 is no-overload */
	RTK_IOCG_IF_BUF_LOADING,        /* arg: netif name; value: 0~100 */
	RTK_IOCG_IF_HI_FIFO,            /* arg: netif name; value: number of free page */
	RTK_IOCG_IF_MID_FIFO,           /* arg: netif name; value: number of free page */
	RTK_IOCG_IF_LOW_FIFO,           /* arg: netif name; value: number of free page */
	RTK_IOCG_IF_PUB_FIFO,           /* arg: netif name; value: number of free page */
	RTK_IOCG_IF_FALSEALARM_OFDM,    /* arg: netif name; */
	RTK_IOCG_IF_FALSEALARM_CCK,     /* arg: netif name; */

	RTK_IOCG_WIFI_STAINFO_NORESP,   /* arg: netif name value: no response in x second */
	RTK_IOCG_WIFI_EFUSE_LEN,        /* arg: netif name; value: efuse size */
};

int rtk_wlan_global_init(const char *options);

#ifdef __cplusplus
}
#endif

#endif

