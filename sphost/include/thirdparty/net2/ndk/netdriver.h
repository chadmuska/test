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
#ifndef __NDK_NETDRIVER_H__
#define __NDK_NETDRIVER_H__

#include <ndk_types.h>
#include <linux/if_ether.h>
#include <ndk/netif.h>

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// Network Driver Type, BIT[0:1]
enum {
	NDK_DRV_WLAN      = 1,
	NDK_DRV_ETHER     = 2,

	NDK_DRV_TYPE_MASK = 0x03
};

// Vendor ID, BIT[2:]
enum {
	// wireless
	NDK_DRV_WLAN_CSR  = 1 << 2,
	NDK_DRV_WLAN_MTK  = 2 << 2,
	NDK_DRV_WLAN_RTK  = 3 << 2,
	NDK_DRV_WLAN_BCM  = 4 << 2,
	NDK_DRV_WLAN_MRVL = 5 << 2,
	NDK_DRV_WLAN_QCA  = 6 << 2,
	NDK_DRV_WLAN_TI   = 7 << 2,

	// ethernet
	NDK_DRV_ETHER_RTK = 3 << 2,
};

#define NDK_DRV_GET_TYPE(id)       ((id) & 0x03)
#define NDK_DRV_GET_VENDOR_ID(id)  ((id) & ~0x03)

///////////////////////////////////////////////////////////////////////////////
/* Forward declaration. */
struct net_device; /* Implemented in the net driver. */
struct sk_buff;
struct skb_core;

/* Must be the first memeber of struct net_device */
#define NETDEVCORE_MEMBERS \
	char  name[IFNAMSIZ]; \
	UINT8 dev_addr[ETH_ALEN]; \
	int   (*pf_xmit)(struct net_device* dev, struct skb_core* skbc); \
	int   (*pf_open)(struct net_device *dev); \
	int   (*pf_stop)(struct net_device *dev); \
	int   (*pf_ioctl)(int cmd, struct net_device *dev, void *arg); \
	union { UINT8 if_index; UINT8 ifindex; }; \
	UINT8 queue_stopped : 1, \
	      if_running : 1, \
	      prefer_hostap : 1, \
	      prefer_station : 1; \
	UINT16 priv_size; \
	void* ndev_priv; \
	void* ndev_netif

struct netdevcore {
	NETDEVCORE_MEMBERS;
};
#define netdev2core(dev)  ((struct netdevcore*)(dev))

typedef struct {
	int sioc_code; /* IOCTL code based on SIOCDEVPRIVATE */
	int ioc_code;  /* IOCTL code defined in the above enum. -1: not defined.*/
	const char * ioc_cmd;   /* Command string defined by the driver. */
} NDKNetDrvPrivIoctl;

typedef int (*NDKNetDrvPrivIoctlFunc)(int code, const char *ifname, long *value);

typedef struct {
	unsigned int id;
	const char *name;
	int  (*load)(const char *options);
	void (*unload)(void);
	void (*mpset)(int enable);
} NDKNetDriver;

///////////////////////////////////////////////////////////////////////////////
extern NDKNetifDevOps* netif_dev_ops;

static inline void *ndk_netdrv_netdev_priv(const struct net_device *dev) { return netdev2core(dev)->ndev_priv; }
static inline const char *ndk_netdrv_netdev_name(const struct net_device *dev) { return netdev2core(dev)->name; }
static inline int ndk_netdrv_register_device(struct net_device *netdev) { return netif_dev_ops->register_device(netdev); }
static inline int ndk_netdrv_unregister_device(struct net_device *netdev) { return netif_dev_ops->unregister_device(netdev); }
static inline int ndk_netdrv_netif_rx(struct sk_buff *skb) { return netif_dev_ops->proc_rx_skb(skb); }
static inline void ndk_netdrv_netif_carrier_on(struct net_device *netdev) { return netif_dev_ops->carrier_on(netdev); }
static inline void ndk_netdrv_netif_carrier_off(struct net_device *netdev) { return netif_dev_ops->carrier_off(netdev); }
static inline BOOL ndk_netdrv_netif_carrier_ok(struct net_device *netdev) { return netif_dev_ops->carrier_ok(netdev); }
static inline UINT32 ndk_netdrv_ipv4_addr_get(struct net_device *dev) { return netif_dev_ops->ipv4_addr_get(dev); }

struct sk_buff* ndk_netdrv_dev_alloc_skb(struct net_device *dev, UINT32 payload_size);

static inline BOOL ndk_netdrv_netif_queue_stopped(const struct net_device *dev) { return netdev2core(dev)->queue_stopped; }
static inline void ndk_netdrv_netif_start_queue(struct net_device *dev) {
	//profLogPuts(0, "netif start queue");
	netdev2core(dev)->queue_stopped = 0;
}
static inline void ndk_netdrv_netif_stop_queue(struct net_device *dev) {
	//profLogPuts(0, "netif stop queue");
	netdev2core(dev)->queue_stopped = 1;
}
static inline void ndk_netdrv_netif_wake_queue(struct net_device *dev) {
	//profLogPuts(0, "netif wake queue");
	netdev2core(dev)->queue_stopped = 0;
}

static inline BOOL ndk_netdrv_netif_running(struct net_device *dev) {
	return netdev2core(dev)->if_running;
}

struct net_device *ndk_netdrv_alloc_device(
	unsigned   sizeof_net_device,
	unsigned   sizeof_priv,
	const char *namefmt,
	void       (*setup)(struct net_device *));

void ndk_netdrv_free_device(struct net_device *dev);
int  ndk_netdrv_alloc_device_name(struct net_device *dev, const char *namefmt);
struct net_device *ndk_netdrv_get_device_by_name(const char *name);

/* Called by network device driver */
int ndk_netdrv_register_priv_ioctls(
	const char *ifname,
	NDKNetDrvPrivIoctlFunc ioctl_func,
	NDKNetDrvPrivIoctl *ioctls);

void ndk_netdrv_unregister_priv_ioctls(
	const char *ifname,
	NDKNetDrvPrivIoctlFunc ioctl_func,
	NDKNetDrvPrivIoctl *ioctls);

unsigned int ndk_netdev_get_driver_id(int index, unsigned int type);
#define ndk_netif_get_wlan_driver_id()		ndk_netdev_get_driver_id(0, NDK_DRV_WLAN)

/* Regisetr wireless-lan or ethernet driver. The driver variable must be a global variable. */
int ndk_netdrv_register_driver(NDKNetDriver *driver);
/* Set size of sk_buff. must be set before loading driver. */
void ndk_netdrv_setup_skbuff(unsigned size);

UINT16 eth_type_trans(struct sk_buff *skb, struct net_device *dev);

#ifdef __cplusplus
}
#endif

#endif

