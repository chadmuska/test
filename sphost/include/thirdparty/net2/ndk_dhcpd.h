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

#ifndef __NDK_DHCPD_API_H__
#define __NDK_DHCPD_API_H__

#include <ndk_tcpip_api.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Option ID for ndk_dhcpd_opt_set */
enum {
	NDK_DHCPD_OPT_GATEWAY = 3, /* Argument: struct in_addr* */
	NDK_DHCPD_OPT_DNS     = 6, /* Argument: struct in_addr* */
};

/*
 * Arguments:
 *   ifname: interface name, such as "wlan0", "eth0"
 *   ipa_start: start address for allocation. The address must be in network byte order.
 *   str_start: start address in Internet standard dot notation.
 *   nr: number of addres for allocating.
 * \return 0: succeed; < 0: failed
 */
int
ndk_dhcpd_add_interface(
	const char     *ifname,
	struct in_addr *local,
	struct in_addr *start,
	struct in_addr *netmask,
	struct in_addr *gateway,
	int             addr_num);

int
ndk_dhcpd_add_interface_s(
	const char *ifname,
	const char *local,
	const char *start,
	const char *netmask,
	const char *gateway,
	int         addr_num);

void ndk_dhcpd_del_interface(const char *ifname);

int  ndk_dhcpd_opt_set(const char* ifname, unsigned optid, int services, ...);
int  ndk_dhcpd_opt_get(char* optInfo, unsigned optid, int services, ...);

int  ndk_dhcpd_start();
void ndk_dhcpd_stop();
BOOL ndk_dhcpd_is_started();
void ndk_dhcpd_printall();

BOOL ndk_dhcpd_find_macaddr(struct in_addr* inaddr, UINT8 macaddr_ret[6]);
BOOL ndk_dhcpd_find_inaddr(UINT8 macaddr[6], struct in_addr* inaddr_ret);

void ndk_dhcpd_register_cmds();

#ifdef __cplusplus
}
#endif

#endif

