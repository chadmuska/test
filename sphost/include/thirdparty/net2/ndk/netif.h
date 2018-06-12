/**************************************************************************
 *                                                                        *
 *         Copyright (c) 2015 by iCatch Technology Co., Ltd.             *
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
#ifndef __NDK_NET_INTERFACE_H__
#define __NDK_NET_INTERFACE_H__

#include <linux/if_ether.h>

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Forward declaration only. Defined in the network interface linux compatible layer.
struct sk_buff;
struct net_device;
struct ifreq;

typedef struct {
	int  (*register_device)(struct net_device *netdev);
	int  (*unregister_device)(struct net_device *netdev);
	int  (*proc_rx_skb)(struct sk_buff *skb);
	void (*carrier_on)(struct net_device *netdev);
	void (*carrier_off)(struct net_device *netdev);
	BOOL (*carrier_ok)(struct net_device *netdev);
	struct sk_buff* (*alloc_skb)(struct net_device *dev, UINT32 payload_size);
	UINT32 (*ipv4_addr_get)(struct net_device *dev);
} NDKNetifDevOps;

typedef BOOL (*NDKNetifTxFilter)(UINT8 *data, UINT32 len);
////////////////////////////////////////////////////////////////////////////////////////////////////
// Implemented in the TCPIP module.
int ndk_netif_standard_ioctl(int cmd, void *arg); // For standard IOCTL codes.

enum {
	NETIF_IOCS_UP,
	NETIF_IOCS_DOWN,
	NETIF_IOCS_ARP_REQUEST,
	NETIF_IOCS_ARP_QUERY,
	NETIF_IOCS_PCAP_START, /* Arg: ifrn_name; ifru_data: file name; ifru_datb: NDKPcapArg*. */
	NETIF_IOCS_PCAP_STOP,
	NETIF_IOCS_DEFAULT,
	NETIF_IOCS_TX_FILTER, /* Net packet filter. Arg: ifrn_name; ifru_data: NDKNetifPktTxFilter. */

	NETIF_IOCG_DEFAULT = 100,
	NETIF_IOCG_GATEWAY,
	NETIF_IOCG_CARRIER,

	NETIF_IOC_NR
};

// For the IOCTL codes defined by us.
int ndk_netif_priv_ioctl(int cmd, struct ifreq *ifr);

////////////////////////////////////////////////////////////////////////////////////////////////////
int ndk_netif_register_device(struct net_device *dev);
void ndk_netif_unregister_device(struct net_device *dev);
void ndk_netif_register_dev_ops(NDKNetifDevOps *ops);
void ndk_netif_set_skbuff_room_size(unsigned size);

struct net_device *ndk_netif_get_device_by_index(unsigned index);
struct net_device *ndk_netif_get_device_by_name(const char* name);

/* index start from 1. 0 means error. */
char *   ndk_netif_index2name(unsigned index, char *ifname);
unsigned ndk_netif_name2index(const char *ifname);

#define if_indextoname(index, name)     ndk_netif_index2name(index, name)
#define if_nametoindex(name)            ndk_netif_name2index(name)

////////////////////////////////////////////////////////////////////////////////////////////////////
// Misc. Functions
const char* eth_ntoa(const unsigned char *hwaddr);
int   eth_aton(unsigned char *d_hwaddr,  const char *s_hwaddr);

// Implemented in the TCPIP module.
char *inet_ntoa(struct in_addr in);
int   inet_aton(const char *cp, struct in_addr *inp);

#ifdef __cplusplus
}
#endif

#endif
